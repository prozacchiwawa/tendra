/*
 * Copyright 2002-2011, The TenDRA Project.
 * Copyright 1997, United Kingdom Secretary of State for Defence.
 * Copyright 1993, Open Software Foundation, Inc.
 *
 * See doc/copyright/ for the full copyright terms.
 */

#include <assert.h>

#include <shared/bool.h>
#include <shared/check.h>
#include <shared/error.h>

#include <local/exp.h>

#include <tdf/tag.h>

#include <reader/exp.h>

#include <construct/aldefs.h>

#include <refactor/refactor.h>

#include <utility/max.h>

#include <main/flags.h>
#include <main/print.h>

#include "reg.h"
#include "addr.h"
#include "regexps.h"

struct regpeept {
	ans inans;
	exp keptexp;
	bool iscont;
};

typedef struct regpeept regpeep;

#define FR_OFFSET		KEPT_FREG_OFFSET	/* 32 */
#define	LAST_ALL_REGS		(FR_OFFSET+FR_LAST)	/* 63 */

static regpeep regexps[LAST_ALL_REGS + 1];	/* [0:31] fix pt - [32:63] floating pt */

static bool sim_exp(exp, exp);


/* both either floating or fixed and same size and alignment */
bool
keep_eq_size(shape as, shape bs)
{
	bool as_flt = is_floating(as->tag);
	bool bs_flt = is_floating(bs->tag);

	if (as_flt != bs_flt) {
		return 0;    /* dissimilar float/fixed */
	}

	return shape_size(as) == shape_size(bs) && shape_align(as) == shape_align(bs);
}

static bool
sim_explist(exp al, exp bl)
{
	if (al == NULL && bl == NULL) {
		return 1;
	}

	if (al == NULL || bl == NULL) {
		return 0;
	}

	if (!sim_exp(al, bl)) {
		return 0;
	}

	if (al->last && bl->last) {
		return 1;
	}

	if (al->last || bl->last) {
		return 0;
	}

	return sim_explist(next(al), next(bl));
}

static bool
sim_exp(exp a, exp b)
{
	/*
	 * basically eq_exp except equal shapes requirement is weakened to equal
	 * sizes and alignments
	 */
	if (a->tag == b->tag) {
		if (a->tag == name_tag) {
			/* See if both are name_tags for same ident
			with same offsets and same size and alignment */
			return child(a) == child(b) && no(a) == no(b) &&
			       keep_eq_size(sh(a), sh(b));
		}

		/*
		 * If it is not is_a
		 *  OR
		 * if they are not the same size and alignment and same register type
		 */
		if (!is_a(a->tag) || !keep_eq_size(sh(a), sh(b))) {
			return 0;
		}

		if (a->tag == float_tag) {
			return eq_exp(child(a), child(b));
			/* float_tag is special since we could have e.g float (-1 slongsh) float (-1 ulongsh) */
		}

		return no(a) == no(b) && sim_explist(child(a), child(b));
	}

	return 0;
}

/* forget all register<->exp associations */
void
clear_all(void)
{
	int i;

	for (i = 0; i <= LAST_ALL_REGS; i++) {
		regexps[i].keptexp = NULL;
		setregalt(regexps[i].inans, 0);
	}
}

/* forget reg i - exp association */
void
clear_reg(int i)
{
	i = ABS(i);
	if (i >= 0 && i <= LAST_ALL_REGS) {
		regexps[i].keptexp = NULL;
		setregalt(regexps[i].inans, 0);
	}
}

/*
 * find if e has already been evaluated into a register low_reg..hi_reg
 *
 * Register tracking:
 * The array regexps[] is an array of regpeep structures
 * The elements of the structure regpeep are:
 *
 * ans inans;     This helps specify where the exp came from
 * exp keptexp;   The exp
 * bool iscont;   This specifies whether or not
 */
static ans
iskept_regrange(exp e, int low_reg, int hi_reg)
{
	int i;
	ans aa;
	setregalt(aa, 0); /* nilans until we know better */

	/* reg tracking of unions unsafe, as views of location can differ */
	/* +++ improve this */
	if (sh(e)->tag == cpdhd) {
		return aa;
	}

	for (i = low_reg; i <= hi_reg; i++) {
		exp ke = regexps[i].keptexp;
		bool isc;

		if (ke == NULL) {
			/* There isn't an association with register i */
			continue;
		}

		isc = regexps[i].iscont;

		assert(!IS_R_TMP(i));	/* should not track R_TMP */

		if (((!isc && sim_exp(ke, e)) ||
		     (e->tag == cont_tag && isc && keep_eq_size(sh(ke), sh(e))
		      && sim_exp(ke, child(e)) && al1(sh(child(e))) == al1(sh(ke))))
		) {
			aa = (regexps[i].inans);

			asm_comment("iskept found 1: reg=%d isc=%d e->tag =%d child(e)->tag =%d",
			            i, isc, e->tag, child(e)->tag);
			asm_comment("iskept found: no = %ld", no(e));

			switch (aa.discrim) {
			case notinreg:
				if (!aa.val.instoreans.adval) {

					/*
					 * the expression is given indirectly - it may have also been
					 * loaded into a register
					 */
					continue;
				}
				/* else ... */

			default:
				return aa;
			}
		} else if (ke->tag == cont_tag && !isc) {
			ans aq;

			aq = regexps[i].inans;

			if (aq.discrim == notinreg) {
				instore is;

				is = insalt(aq);
				if (!is.adval && is.b.offset == 0 && IS_FIXREG(is.b.base)
				    && sim_exp(child(ke), e)) {

					/*
					 * the contents of req expression is here as a reg-offset
					 */
					is.adval = 1;
					setinsalt(aq, is);

					asm_comment("iskept found 2: reg=%d isc=%d e->tag =%d child(e)->tag =%d",
					            i, isc, e->tag, child(e)->tag);

					return aq;
				}
			}
		} else if (ke->tag == reff_tag && !isc) {
			ans aq;

			aq = regexps[i].inans;
			if (aq.discrim == notinreg) {
				instore is;

				is = insalt(aq);
				if (is.adval && is.b.offset == (no(ke) / 8)
				    && IS_FIXREG(is.b.base)
				    && sim_exp(child(ke), e)) {

					/*
					 * a ref select of req expression is here as a reg-offset
					 */
					is.adval = 1;
					is.b.offset = 0;
					setinsalt(aq, is);

					asm_comment("iskept found 3: reg=%d isc=%d e->tag =%d child(e)->tag =%d",
					            i, isc, e->tag, child(e)->tag);

					return aq;
				}
			}
		}
	}

	return aa;
}

/* find if e has already been evaluated into register 'reg' */
ans
iskept_inreg(exp e, int reg)
{
	return iskept_regrange(e, reg, reg);
}

/* find if e has already been evaluated into a fixed point register */
ans
iskept_reg(exp e)
{
	return iskept_regrange(e, 0, R_LAST);
}

/* find if e has already been evaluated into any register */
ans
iskept(exp e)
{
	return iskept_regrange(e, 0, LAST_ALL_REGS);
}

/* return reg if 'a' can is in fixed reg */
int
ans_reg(ans aa)
{
	if (aa.discrim == inreg && regalt(aa) != 0) {
		/* the same expression has already been evaluated into a reg */
		return regalt(aa);
	}

	if (aa.discrim == notinreg) {
		instore is;
		is = insalt(aa);	/* no init to avoid IBM cc bug */

		if (is.adval && is.b.offset == 0) {
			/* the same expression has already been evaluated into a reg */
			return is.b.base;
		}
	}

	return R_NO_REG;
}

/* set up exp - address association */
void
keepexp(exp e, ans loc)
{
	int pos = 0;

	switch (loc.discrim) {
	case insomereg:
	case insomefreg:
		error(ERR_SERIOUS, "Keep ? reg");

	case inreg:
		pos = regalt(loc);
		break;

	case infreg:
		pos = fregalt(loc).fr + FR_OFFSET;
		break;

	case notinreg:
		pos = insalt(loc).b.base;
		if (!IS_FIXREG(pos)) {
			return;
		}
	}

	assert(pos >= 0 && pos <= LAST_ALL_REGS);

	/* don't track R_TMP which is used outside tracking scheme */
	if (IS_R_TMP(pos)) {
		return;
	}

	regexps[pos].keptexp = e;
	regexps[pos].inans   = loc;
	regexps[pos].iscont  = 0;
	asm_comment("keepexp : reg %d kept name is %d", pos, e->tag);
}

/* set up cont(e)-reg association */
/*
 * if 0=<reg<=31  this means a fixed point register
 * if 31<reg<=63  this means a float point register single precision
 * if -63<=reg<-31 this means a float point register double precision
 */
void
keepcont(exp e, int reg)
{
	freg fr;
	int z = ABS(reg);

	if (z >= FR_OFFSET) {
		fr.dble = (reg < 0);
		fr.fr = z - FR_OFFSET;
		setfregalt(regexps[z].inans, fr);
	} else {
		instore is;

		/* don't track R_TMP which is used outside tracking scheme */
		if (IS_R_TMP(z)) {
			return;
		}

		is.b.base = reg;
		is.b.offset = 0;
		is.adval = 1;
		setinsalt(regexps[z].inans, is);
	}

	assert(z >= 0 && z <= LAST_ALL_REGS);
	regexps[z].keptexp = e;
	regexps[z].iscont  = 1;

	asm_comment("keepcont : reg %d kept name is %d", z, e->tag);
}

/* keepreg keeps the exp e */
/*
 * if 0=<reg<=31  this means a fixed point register
 * if 31<reg<=63  this means a float point register single precision
 * if -63<=reg<-31 this means a float point register double precision
 */
void
keepreg(exp e, int reg)
{
	freg fr;
	int z = ABS(reg);

	if (z >= FR_OFFSET) {
		/* It is a float register */
		/* HACK: if reg <0 then it is double
		   otherwise it is single precision */
		fr.dble = (reg < 0);
		fr.fr = z - FR_OFFSET;
		setfregalt(regexps[z].inans, fr);
	} else {
		instore is;

		/* don't track R_TMP which is used outside tracking scheme */
		if (IS_R_TMP(z)) {
			return;
		}

		is.b.base = reg;
		is.b.offset = 0;
		is.adval = 1;

		setinsalt(regexps[z].inans, is);
	}

	assert(z >= 0 && z <= LAST_ALL_REGS);
	regexps[z].keptexp = e;
	regexps[z].iscont = 0;

	asm_comment("keepreg : reg %d kept name is %d no %ld", z, e->tag, no(e));
}

static bool couldaffect(exp , exp);

/* could 'e' be 'lhs' */
static bool
couldbe(exp e, exp lhs)/* is var name_tag exp or 0 meaning cont */
{
	int ne = e->tag;
	exp s = child(e);

	if (ne == name_tag) {
		if (lhs != 0 && s == child(lhs)) {
			return 1;
		}

		if (isvar(s)) {
			return lhs == 0 && (isglob(s) || isvis(s));
		}

		if (IS_A_PROC(s)) {
			return lhs == 0;
		}

		if (child(s) == NULL) {
			return 1;
		}

		return couldbe(child(s), lhs);
	}

	if (ne == cont_tag) {
		if (lhs != 0 && s->tag == name_tag && child(s) != NULL) {
			return child(s) == child(lhs) || isvis(child(lhs)) || isvis(child(s));
		}

		return 1;
	}

	if (ne == reff_tag || ne == field_tag) {
		return couldbe(s, lhs);
	}

	if (ne == addptr_tag || ne == subptr_tag) {
		return couldbe(s, lhs) || couldaffect(next(s), lhs);
	}

	return 1;
}

/* could alteration to z affect e? */
static bool
couldaffect(exp e, exp z)/* a name or zero */
{
	int ne = e->tag;

	if (ne == cont_tag) {
		return couldbe(child(e), z);
	}

	if (ne == name_tag) {
		if (isvar(child(e))) {
			return z == 0 && isvis(child(e));
		}

		if (IS_A_PROC(child(e))) {
			return 0;
		}

		if (child(child(e)) == NULL) {
			return 1 /* could it happen? */ ;
		}

		return couldaffect(child(child(e)), z);

	}

	if (ne < plus_tag || ne == contvol_tag) {
		return 1;
	}

	e = child(e);

	while (e != NULL) {
		if (couldaffect(e, z)) {
			return 1;
		}

		if (e->last) {
			return 0;
		}

		e = next(e);
	}

	return 0;
}

/* does e depend on z */
bool
dependson(exp e, bool isc, exp z)
{
	if (e == NULL) {
		return 0;
	}

	for (;;) {
		if (z->tag == reff_tag || z->tag == addptr_tag ||
		    z->tag == subptr_tag) {
			z = child(z);
		}

		if (z->tag != name_tag) {
			if (z->tag != cont_tag) {
				return 1;
			}
			z = 0;
			break;
		}

		if (isvar(child(z))) {
			break;
		}

		if (IS_A_PROC(child(z))) {
			z = 0;
			break;
		}

		if (child(child(z)) == NULL) {
			return 1;    /* can it happen? */
		}

		z = child(child(z));
	}

	/* z is now unambiguous variable name or 0 meaning some contents */

	return isc ? couldbe(e, z) : couldaffect(e, z);
}

/* remove association of any register which depends on lhs */
void
clear_dep_reg(exp lhs)
{
	int i;

	for (i = 0; i <= LAST_ALL_REGS; i++) {
		if (regexps[i].keptexp == NULL) {
			continue;
		}

		switch (regexps[i].keptexp->tag) {
		case val_tag:
		case null_tag:
		case real_tag:
		case string_tag:
		case name_tag:
		case current_env_tag:
			if (!regexps[i].iscont) {
				/* constant value, cannot be changed by assign */
				continue;
			}

			FALL_THROUGH;

		default:
			if (dependson(regexps[i].keptexp, regexps[i].iscont, lhs)) {
				asm_comment("clear_dep_reg: reg=%d iscont=%d", i, regexps[i].iscont);
				regexps[i].keptexp = NULL;
				setregalt(regexps[i].inans, 0);
			}
		}
	}
}

