/*
 * Copyright 2011, The TenDRA Project.
 * Copyright 1997, United Kingdom Secretary of State for Defence.
 *
 * See doc/copyright/ for the full copyright terms.
 */

/*
 * Trivial 'peephole' optimisations
 */

#include <assert.h>
#include <stddef.h>

#include <shared/bool.h>
#include <shared/check.h>
#include <shared/error.h>

#include <local/exp.h>

#include <tdf/shape.h>
#include <tdf/tag.h>

#include <reader/exp.h>

#include <construct/aldefs.h>

#include <refactor/refactor.h>

#include <utility/max.h>

#include <main/print.h>

#include "procrec.h"
#include "addr.h"
#include "move.h"
#include "bits.h"
#include "reg.h"
#include "regexps.h"

static regpeep regexps[64]; /* [0:31] fix pt - [32:47] floating pt */

static bool sim_exp(exp, exp);

/* Same size and alignment and "both float or both fixed". */
bool
eq_sze(shape as, shape bs)
{
	if (is_floating(as->tag)) {
		return as->tag == bs->tag;
	}

	if (is_floating(bs->tag)) {
		return 0;
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

/*
 * basically eq_exp except equal shapes requirement  is weakened to equal
 * sizes and alignments
 */
static bool
sim_exp (exp a, exp b)
{
	if (a->tag != b->tag) {
		return 0;
	}

	if (a->tag == name_tag) {
		return child(a) == child(b) && no(a) == no(b) && eq_sze(sh(a), sh(b));
	}

	if (!is_a(a->tag) || !eq_sze(sh(a), sh(b))) {
		return 0;
	}

	if (a->tag == float_tag) { /* NEW bit */
		return eq_exp(child(a), child(b));
	}

	return no(a) == no(b) && sim_explist(child(a), child(b));
}

/* forget all register - exp associations */
void
clear_all(void)
{
	int i;

	for (i = 0; i < 48; i++) {
		regexps[i].keptexp = NULL;
		setregalt(regexps[i].inans, 0);
	}
}

/* forget reg i - exp association */
void
clear_reg(int i)
{
	i = ABS(i);

	if (i >= 0 && i < 48) {
		regexps[i].keptexp = NULL;
		setregalt(regexps[i].inans, 0);
	}
}

/* find if e has already been evaluated into a register */
ans
iskept(exp e)
{
	int i;
	ans nilans;

	setregalt(nilans, 0);		/* init nilans */

#ifdef NO_KEPT_OPTS
	/* no reg tracking */
	return nilans;
#endif

	/* reg tracking of unions unsafe, as views of location can differ */
	/* +++ track on fields */
	/* +++ safe to allow structs but not unions */
	if (sh(e)->tag == cpdhd) {
		return nilans;
	}

	for (i = 0; i < 48; i++) {
		exp ke = regexps[i].keptexp;
		bool isc = regexps[i].iscont;

		if (ke == NULL) {
			/* there isn't an accociation with reg i */
			continue;
		}

		if (((!isc && sim_exp(ke, e)) ||
		     (e->tag == cont_tag && isc && eq_sze(sh(ke), sh(e))
		      && sim_exp(ke, child(e))))
		) {
			ans aa;
			aa = (regexps[i].inans);

#if 0
			asm_comment("iskept found: reg=%d isc=%d e->tag =%d child(e)->tag =%d", i, isc, e->tag, child(e)->tag);
			asm_comment("	hd(e) =%d hd(child(e)) =%d hd(ke) =%d", sh(e)->tag, sh(child(e))->tag, sh(ke)->tag);
			asm_comment("	sim_exp(ke, e) =%d sim_exp(ke, child(e)) =%d eq_size(sh(ke), sh(e)) =%d",
			            sim_exp(ke, e), sim_exp(ke, child(e)), eq_size(sh(ke), sh(e)));
#endif

			switch (discrim(aa)) {
			case notinreg:
				if (!aa.val.instoreans.adval) {
					/*
					 * the expression is given indirectly - it may have also been
					 * loaded into a register
					 */
					continue;
				}

				FALL_THROUGH;

			default:
				return aa;
			}
		} else if (ke->tag == cont_tag && !isc) {
			ans aq;

			aq = regexps[i].inans;
			if (discrim(aq) == notinreg) {
				instore is;

				is = insalt(aq);
				if (!is.adval && is.b.offset == 0 && IS_FIXREG(is.b.base)
				    && sim_exp(child(ke), e)) {
					/* the contents of req expression is here as a reg-offset */
					is.adval = 1;
					setinsalt(aq, is);
					return aq;
				}
			}
		} else if (ke->tag == reff_tag && !isc) {
			ans aq;

			aq = regexps[i].inans;
			if (discrim(aq) == notinreg) {
				instore is;

				is = insalt(aq);
				if (is.adval && is.b.offset == (no(ke) / 8)
				    && IS_FIXREG(is.b.base)
				    && sim_exp(child(ke), e)) {
					/* a ref select of req expression is here as a reg-offset */
					is.adval = 1;
					is.b.offset = 0;
					setinsalt(aq, is);
					return aq;
				}
			}
		}
	}

	return nilans;
}

/* set up exp - address association */
void
keepexp(exp e, ans loc)
{
	int reg = 0;

	switch (discrim(loc)) {
	case insomereg:
	case insomefreg:
		error(ERR_SERIOUS, "keep ? reg");

#if USE_BITAD
	case bitad:
		return;
#endif

	case inreg:
		reg = regalt(loc);
		break;

	case infreg:
		reg = fregalt(loc).fr + 32;
		break;

	case notinreg:
		reg = insalt(loc).b.base;
		if (!IS_FIXREG(reg)) {
			return;
		}
		break;

	default:
		;
	}

	assert(reg >= 0 && reg < 48);
	assert(reg != GR1);

	regexps[reg].keptexp = e;
	regexps[reg].inans = loc;
	regexps[reg].iscont = 0;
}

/* set up cont(e)-reg association */
void
keepcont(exp e, int regcode)
{
	freg fr;
	int reg = ABS(regcode);

	assert(reg >= 0 && reg < 48);
	assert(reg != GR1);

	if (reg > 31) {
		fr.dble = ((regcode < 0) ? 1 : 0);
		fr.fr = reg - 32;
		setfregalt(regexps[reg].inans, fr);
	} else {
		instore is;

		is.b.base   = regcode;
		is.b.offset = 0;
		is.adval = 1;
		setinsalt(regexps[reg].inans, is);
	}

	regexps[reg].keptexp = e;
	regexps[reg].iscont  = 1;
}

/* set up e-reg association */
void
keepreg(exp e, int regcode)
{
	freg fr;
	int reg = ABS(regcode);

	assert(reg >= 0 && reg < 48);
	assert(reg != GR1);

	if (reg > 31) {
		fr.dble = regcode < 0 ? 1 : 0;
		fr.fr = reg - 32;
		setfregalt(regexps[reg].inans, fr);
	} else {
		instore is;

		is.b.base = regcode;
		is.b.offset = 0;
		is.adval = 1;
		setinsalt(regexps[reg].inans, is);
	}

	regexps[reg].keptexp = e;
	regexps[reg].iscont  = 0;
}

static bool couldaffect(exp, exp);

/* could e be lhs */
static bool
couldbe(exp e, exp lhs)
{
	int ne = e->tag;
	exp s = child(e);

	if (ne == name_tag) {
		if (lhs != 0 && s == child(lhs)) {
			return 1;
		}

		if (isvar(s)) {
			return lhs == 0 && (isvis(s) || isglob(s));
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

/* could alteration to z effect e? */
static bool
couldaffect(exp e, exp z /* a name or zero */)
{
	int ne;

	ne = e->tag;
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
			return 1; /* can it happen? */
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

	for (i = 0; i < 48; i++) {
		if (dependson(regexps[i].keptexp, regexps[i].iscont, lhs)) {
			regexps[i].keptexp = NULL;
			setregalt(regexps[i].inans, 0);
		}
	}
}

