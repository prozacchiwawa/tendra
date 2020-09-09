/*
 * Copyright 2002-2012, The TenDRA Project.
 * Copyright 1997, United Kingdom Secretary of State for Defence.
 * Copyright 1993, Open Software Foundation, Inc.
 *
 * See doc/copyright/ for the full copyright terms.
 */

/*
 * Defines the scan through a program which reorganises it so that all
 * arguments of operations are suitable for later code-production.
 *
 * The procedure scan evaluates the register requirements of an exp.
 * The exps are produced from the decoding process and the various
 * exp -> exp transformations in the proc independent
 * (common to other translators)
 */

#include <assert.h>

#include <shared/bool.h>
#include <shared/check.h>
#include <shared/error.h>

#include <local/cpu.h>
#include <local/ash.h>
#include <local/exp.h>
#include <local/tag.h>

#include <tdf/nat.h>
#include <tdf/shape.h>
#include <tdf/tag.h>

#include <reader/exp.h>
#include <reader/externs.h>

#include <construct/installtypes.h>
#include <construct/dec.h>
#include <construct/ash.h>
#include <construct/exp.h>
#include <construct/install_fns.h>
#include <construct/reg_result.h>
#include <construct/me_fns.h>

#include <flpt/flpt.h>

#include <utility/max.h>

#include <main/flags.h>
#include <main/print.h>

#include <refactor/const.h>
#include <refactor/refactor.h>

#include "reg.h"
#include "addr.h"
#include "procrec.h"
#include "bits.h"
#include "regable.h"
#include "tempdecs.h"
#include "translate.h"
#include "make_code.h"
#include "scan.h"
#include "stack.h"
#include "error.h"
#include "dynamic_init.h"

/*
 * Will an ident exp be in memory?
 * We have to guess at this stage for non globals.
 */
#define INMEMIDENT(ident_exp)	(isvis(ident_exp) || isglob(ident_exp))

/*
 * Will evaluating exp 'e' cause last instruction to be a load from store?
 * Always false if generating diagnostics, we dont want to rearrange in
 * this case as it makes debugging less obvious.
 * This is not exact, register contents tracking may find a copy in reg.
 */
#define LOADFROMSTORE(e) \
	(diag == DIAG_NONE && ( \
	 ((e)->tag == cont_tag && (child(e)->tag!= name_tag || INMEMIDENT(child(child(e))))) \
	  || ((e)->tag == name_tag && isglob(child(e))) /* load of TOC entry */ \
	))

int maxfix, maxfloat; /* the maximum number of t-regs */
static bool tail_call;
bool gen_call;
long callee_size;
long max_callees;
long no_of_returns;
int end_param;

/* The following are used for allocating param regs to t-regs */
static int freefixed;
static int freefloat;

static int stparam, fixparam, floatparam;

static bool nonevis = true;
static bool rep_tag_scanned;

static int scan_cond(exp *, exp);
needs scan(exp *, exp **);
static void number_caller_parameter(exp);
static void number_callee_parameter(exp);

/*
 * declaration of scan.
 *
 * needs is defined in procrectypes.h.
 *
 * This is a structure which has two integers giving the number of fixed
 * and floating point registers required to contain live values in the
 * expression parameters. A further field prop is used for various
 * flags about certain forms of exp (mainly idents and procs).
 * The maxargs field gives the maximum size in bits for the parameters of
 * all the procs called in the exp. The needs of a proc body are preserved
 * in the needs field of the procrec (see procrectypes.h).
 */

/*
 * This procedure effectively inserts a new declaration into an exp.
 * This is used to stop a procedure requiring more than the available
 * number of registers.
 */
static void
cca(exp **to, exp * x)
{
	if ((**to)->tag == diagnose_tag) {
		*to = &child(**to);
	}

	if (x == (*to)) {
		exp def = *x;

		/* replace by  Let tag = def In tag Ni */
		exp id = getexp(sh(def), next(def), def->last, def, NULL,
		                0, 1, ident_tag);
		exp tag = getexp(sh(def), id, 1, id, NULL,
		                 0, 0, name_tag);

		pt(id) = tag;	/* use of tag */
		next(def) = tag;	/* next(def) is body of Let = tag */
		def->last = false;

		*x = id;		/* replace pointer to x by Let */
	} else {
		/* replace by Let tag = def In ato/def = tag Ni */
		exp def = *x;
		exp ato = **to;

		exp id  = getexp(sh(ato), next(ato), ato->last, def, NULL, 0, 1, ident_tag);
		exp tag = getexp(sh(def), next(def), def->last, id, NULL, 0, 0, name_tag);

		pt(id) = tag;	/* use of tag */
		next(def) = ato;	/* ato is body of Let */
		def->last = false;
		next(ato) = id;	/* its father is Let */
		ato->last = true;

		**to = id;	/* replace pointer to 'to' by Let */
		*x = tag;	/* replace use of x by tag */
		*to = &next(def);	/* later replacement to same 'to' will be at * body of Let */
	}
}

/* General useful needs */
static needs onefix    = { 1, 0, 0, 0 }; /* needs one fix pt reg */
static needs twofix    = { 2, 0, 0, 0 }; /* needs 2 fix pt regs */
static needs onefloat  = { 0, 1, 0, 0 }; /* needs 1 flt pt regs */
static needs zeroneeds = { 0, 0, 0, 0 }; /* has no needs */

/* This gives the needs for manipulating a value of shape s */
static needs
shapeneeds(shape s)
{
	if (is_floating(s->tag)) {
		return onefloat;
	} else {
		if (valregable(s)) {
			return onefix;
		} else {
			/* If the shape does not fit into a reg, needs
			 * two fixed regs for moving ... */
			return twofix;
		}
	}
}

/*
 * These are basically the expressions which cannot be accessed
 * by a simple load or store instruction.
 */
static bool complex(exp e)
{
	if (e->tag == name_tag) {
		return 0;
	}

	if (e->tag == cont_tag && child(e)->tag == name_tag && isvar(child(child(e)))) {
		return 0;
	}

	if (e->tag == val_tag || e->tag == real_tag) {
		return 0;
	}

	return 1;
}

/*
 * does the scan on commutative and associative operations and may perform
 * various transformations allowed by these properties
 */
static needs
commutative_scan(exp * e, exp **at)
{
	needs a1;
	needs a2;
	exp dad = *(e);
	exp *br = &child(*e);
	exp prev;
	bool dont_commute;

	assert(father(child(*e)) == *e);

	/* scan the first operand - won't be a val_tag */
	a1 = scan(br, at);

	/*
	 * if first operand is a proc, or ends with a load instruction,
	 * it is not worth doing a commute to a later position
	 */
	dont_commute = (a1.fixneeds >= maxfix || (a1.propneeds & hasproccall))
	               || LOADFROMSTORE(*br);

	do {
		exp *prevbr;

		prevbr = br;
		prev = *(br);
		br = &next(prev);
		a2 = scan(br, at);

		/* scan the next operand ... */
		if ((*br)->tag != val_tag) {
			prop pc;
			bool all_treg_needs;

			a1.floatneeds = MAX(a1.floatneeds, a2.floatneeds);
			pc = a2.propneeds & hasproccall;
			all_treg_needs = (a2.fixneeds >= maxfix || pc != 0);

			if (!dont_commute
			    &&
			    (all_treg_needs || LOADFROMSTORE(*br))
			    &&
			    (a1.fixneeds < maxfix && (a1.propneeds & hasproccall) == 0))
			{
				/*
				 * ...its evaluation will call a proc, or ends with a load instruction,
				 * so put it first
				 */

				exp op1 = child(dad);
				exp cop = *(br);
				bool lcop = cop->last;

				next(prev) = next(cop);
				if (lcop) {
					prev->last = true;
				}

				next(cop) = op1;
				cop->last = false;
				child(dad) = cop;
				br = (prev == op1) ? &next(cop) : prevbr;
				dont_commute = 1;

				a1.fixneeds = MAX(a2.fixneeds, a1.fixneeds + 1);
				a1.propneeds |= a2.propneeds;
				a1.maxargs = MAX(a1.maxargs, a2.maxargs);
			} else if (all_treg_needs) {
				/* ... its evaluation would disturb
				 * accumulated result, so replace it by a
				 * newly declared tag */
				cca(at, br);
				a1.fixneeds = MAX(a1.fixneeds, 2);
				a1.propneeds = a1.propneeds | morefix | (pc << 1);
				a1.maxargs = MAX(a1.maxargs, a2.maxargs);
			} else {
				/*
				 * ... its evaluation  will not disturb the accumulated result
				 */
				a1.fixneeds = MAX(a1.fixneeds, a2.fixneeds + 1);
				a1.propneeds = a1.propneeds | a2.propneeds;
			}
		}
	} while (!(*br)->last);

	return a1;
}

/*
 * scan non-commutative fix pt operation
 */
static needs
non_commutative_scan(exp * e, exp **at)
{
	needs l;
	needs r;
	prop pc;
	exp *arg = &child(*e);

	l = scan(arg, at);
	/* scan 1st operand */
	arg = &next(*arg);
	r = scan(arg, at);
	/* scan second operand ... */
	l.floatneeds = MAX(l.floatneeds, r.floatneeds);
	pc = r.propneeds & hasproccall;

	if (r.fixneeds < maxfix && pc == 0) {
		/* ...it fits into registers */
		l.fixneeds = MAX(l.fixneeds, r.fixneeds + 1);
		l.propneeds = l.propneeds | r.propneeds;
	} else {
		/* ...it requires new declaration of second operand */
		cca(at, arg);
		l.fixneeds = MAX(l.fixneeds, 1);
		l.propneeds = l.propneeds | morefix | (pc << 1);
		l.maxargs = MAX(l.maxargs, r.maxargs);
	}

	return l;
}

/* scans diadic floating point operation  */
static needs
fpop(exp *e, exp **at)
{
	needs l;
	needs r;
	exp op = *(e);
	prop pcr, pcl;
	exp *arg = &child(op);

	l = scan(arg, at);
	arg = &next(*arg);
	r = scan(arg, at);
	l.fixneeds = MAX(l.fixneeds, r.fixneeds);

	pcr = r.propneeds & hasproccall;
	pcl = l.propneeds & hasproccall;

	if (r.floatneeds <= l.floatneeds && r.floatneeds < maxfloat && pcr == 0) {
		l.floatneeds = MAX(2, MAX(l.floatneeds, r.floatneeds + 1));
		l.propneeds = l.propneeds | r.propneeds;
		ClearRev(op);
	} else if (pcl == 0 && l.floatneeds <= r.floatneeds && l.floatneeds < maxfloat) {
		l.floatneeds = MAX(2, MAX(r.floatneeds, l.floatneeds + 1));
		l.propneeds = l.propneeds | r.propneeds;
		SetRev(op);
	} else if (r.floatneeds < maxfloat && pcr == 0) {
		l.floatneeds = MAX(2, MAX(l.floatneeds, r.floatneeds + 1));
		l.propneeds = l.propneeds | r.propneeds;
		ClearRev(op);
	} else {
		cca(at, arg);
		ClearRev(op);
		l.floatneeds = MAX(l.floatneeds, 2);
		l.propneeds = l.propneeds | morefloat | (pcr << 1);
		l.maxargs = MAX(l.maxargs, r.maxargs);
	}

	return l;
}

/*
 * Calculates a needs value. Each element of which is the maximum of the
 * corresponding elements in the two parameter needs.
 */
static needs
maxneeds(needs a, needs b)
{
	needs an;

	an.fixneeds = MAX(a.fixneeds, b.fixneeds);
	an.floatneeds = MAX(a.floatneeds, b.floatneeds);
	an.maxargs = MAX(a.maxargs, b.maxargs);
	an.propneeds = a.propneeds | b.propneeds;

	return an;
}

/*
 * Calculates the needs of a tuple of expressions; any new declarations
 * required by a component expression will replace the component expression.
 */
static needs
maxtup(exp e, exp **at)
{
	exp *stat = &child(e);
	needs an;

	an = zeroneeds;
	if (*stat == NULL) {
		return an;
	}

	while (an = maxneeds(an, scan(stat, at)), !(*stat)->last) {
		stat = &next(*stat);
	}

	return an;
}

/*
 * Finds if usedname is only used in cont operation or as result of ident
 * i.e. value of name is unchanged over its scope.
 */
static bool
unchanged(exp usedname, exp ident)
{
	exp uses;

	for (uses = pt(usedname); uses != NULL; uses = pt(uses)) {
		if (!intnl_to(ident, uses)) {
			continue;
		}

		if (!uses->last || next(uses)->tag != cont_tag) {
			exp z;

			for (z = uses; z != ident; z = next(z)) {
				if (!z->last || (next(z)->tag != seq_tag && next(z)->tag != ident_tag)) {
					return 0;
				}
			}
		}
	}

	return 1;
}

static exp *
ptr_position(exp e)
{
	exp *a;
	exp dad = father(e);

	if (child(dad) == e) {
		a = &child(dad);
	} else {
		exp sib = child(dad);

		while (next(sib) != e) {
			sib = next(sib);
		}

		a = &next(sib);
	}

	return a;
}

/*
 * The POWER convention for delivering a struct from a procedure is is have an
 * extra pointer parameter in the proc; this means that there always must be
 * space in the calling work-space for the result struct whether or not the
 * value is used e.g. as in f(x); or f(x).a etc.
 *
 * This proc is part of the mechanism to determine whether it is necessary to
 * insert a dummy declaration to ensure that this space exists.
 *
 * distribute selection throughout compound expressions
 */
static bool
chase(exp sel, exp * e)
{
	bool b = 0;
	exp *one;

	switch ((*e)->tag) {
	case ident_tag:
	case seq_tag:
	case rep_tag:
	case labst_tag: {
		b = chase(sel, &next(child(*e)));
		break;
	}

	case solve_tag:
	case cond_tag: {
		one = &child(*e);

		for (;;) {
			b |= chase(sel, one);
			if ((*one)->last) {
				break;
			}

			one = &next(*one);
		}

		break;
	}

	case field_tag: {
		if (chase(*e, &child(*e))) {
			/* inner field has been distributed */
			exp stare = *e;
			exp ss = child(stare);

			if (!stare->last) {
				ss->last = false;
			}

			next(ss) = next(stare);
			sh(ss) = sh(stare);
			*e = ss;
			return chase(sel, e);
		}
		/* ... continue to default */
	}

	default:
		if (child(sel) != *e) {
			/* only change if not outer */
			exp stare = *e;
			exp newsel = getexp(sh(sel), next(stare), stare->last, stare, NULL,
			                    sel->props, no(sel), sel->tag);

			*e = newsel;
			next(stare) = newsel;
			stare->last = true;
			b = 1;
		}
	}

	if (b) {
		sh(*e) = sh(sel);
	}

	return b;
}

/*
 * This procedure works out register requirements of an exp. At each call
 * the fix field of the needs is the number of fixpnt registers required
 * to contain live values to evaluate this expression. This never exceeds
 * maxfix because if it would have, a new declaration is introduced in
 * the exp tree (similarly for floating regs and maxfloat).
 * In these cases the prop field will contain the bits morefix (or morefloat).
 *
 * Scan also works out various things concerned with proc calls. The maxargs
 * field contains the max size in bits of the space required for the parameters
 * of all the procedures called in the exp. An exp proc call produces a
 * hasproccall bit in the prop field, if this is transformed as part of
 * the definition of a new declaration the bit is replaced by a usesproccall.
 * The distinction is only used in unfolding nested proc calls; POWER requires
 * this to be done statically. The condition that a proc exp is a leaf
 * (i.e no proc calls) is that its prop contains neither bit.
 *
 * If an ident exp is suitable, scan marks the props of ident with either
 * inreg or infreg bits to indicate that a t reg may be used for this tag.
 *
 * A thorough understanding of needs along with other procedures that do
 * switch (exp->tag) requires a knowledge of the meaning of the fields
 * of the exp in each case.
 *
 * e is the expression to be scanned, at is the place to put any new decs.
 * NB order of recursive calls with same at is critical
 */
needs
scan(exp *e, exp **at)
{
	exp ste = *e;
	int nstare = ste->tag;
	static long exp_num = 0; /* count of exps we evaluate */

	exp_num++;
	assert(*e != NULL);

	switch (nstare) {
	case 0:
		/* zero_tag */
		return zeroneeds;

	case compound_tag:
	case nof_tag:
	case concatnof_tag:
	case ncopies_tag: {
		needs nl;
		bool cantdo;
		exp dad;

		if (nstare == nof_tag && child(ste) == NULL) {
			return zeroneeds;
		}

		if (ste->tag == ncopies_tag && child(ste)->tag != name_tag
		    && child(ste)->tag != val_tag) {
			nl = scan(&child(*e), at);
			cca(at, &child(*e));
		} else {
			nl = maxtup(*(e), at);
		}

		ste = *e;
		dad = father(ste);
		if (dad->tag == compound_tag ||
		    dad->tag == nof_tag ||
		    dad->tag == concatnof_tag) {
			cantdo = 0;
		} else if (ste->last) {
			if (next(ste)->tag == ass_tag) {
				exp a = child(next(ste));

				cantdo = (a->tag != name_tag || !isvar(child(a)));
			} else {
				cantdo = 1;
			}
		} else if (next(ste)->last) {
			cantdo = (next(next(ste))->tag != ident_tag);
		} else {
			cantdo = 1;
		}

		if (cantdo) {
			/* can only deal with tuples in simple assignment or identity */
			int prps = (nl.propneeds & hasproccall) << 1;

			cca(at, ptr_position(ste));
			nl = shapeneeds(sh(*(e)));
			nl.propneeds |= morefix;
			nl.propneeds |= prps;
		}

		nl.fixneeds = MAX(nl.fixneeds, 2);

		return nl;
	}

	case cond_tag:
		if (scan_cond(e, NULL) != 0) {
			return scan(e, at);
		}                   /* Else goto next case */

		FALL_THROUGH;

	case rep_tag:
	case solve_tag: {
		exp *stat;
		exp *statat;
		needs an;

		stat = &child(*e);
		statat = stat;
		an = zeroneeds;
		rep_tag_scanned = 0;

		/*
		 * Simply scan each argument.
		 *
		 * The arguments are effectively independent pieces of code
		 * for these constructions.
		 *
		 *    _        _________
		 *   |_|----->|        _|
		 *  /         |    _  |_|
		 * e          |___|_|___|
		 *               / |
		 *              /  |
		 *          stat(1)|       stat(2)       stat(3)
		 *             ____v____  /  _________  /  _________
		 *            |        _|/  |        _|/  |        _|
		 *            |    _  |_|-->|    _  |_|-->|    _  |_|-->
		 *            |___|_|___|   |___|_|___|   |___|_|___|
		 *
		 */

		while (an = maxneeds(an, scan(stat, &statat)), !(*stat)->last) {
			stat = &next(*stat);
			statat = stat;
		}

		if ((*e)->tag == rep_tag) {
			if (rep_tag_scanned == 0) {
				setinnermost(*e);
			} else {
				clearinnermost(*e);
			}
		}

		rep_tag_scanned = 1;
		if ((an.propneeds & usesproccall) != 0) {
			an.propneeds |= hasproccall;
		}

		return an;
	}

	case labst_tag: {
		exp *stat;
		exp *statat;
		needs an;
		int expn = exp_num;

		assert(!child(*e)->last);
		assert(next(child(*e))->last);

		/*
		 *    _     _________
		 *   |_|-->| labst   |
		 *  /      |    _    |
		 * e       |___|_|___|   stat
		 *              |       /
		 *          ____v____  /  ________
		 *         | clear  _|/  |       _|
		 *         |       |_|-->|      |_|
		 *         |_________|   |________|
		 *
		 */

		stat = &next(child(*e));
		statat = stat;
		an = scan(stat, &statat);

		if ((an.propneeds & usesproccall) != 0) {
			an.propneeds |= hasproccall;
		}

		/*
		 * ptno(child()) is set to exp number so that make_code can estimate
		 * distances of conditional branches, which is limited on POWER.
		 */
		ptno(child(*e)) = expn;

		return an;
	}

	/*
	 * shape of exp is body, child is def, next of child is body,
	 * ptr of ident exp is chain of uses.
	 */
	case ident_tag: {
		needs bdy;
		needs defneeds;
		exp stare = *(e);
		exp *arg;
		exp t = pt(stare), s;
		bool fxregble;
		bool flregble;
		bool old_nonevis = nonevis;
		bool uses_R_RESULT;
		bool uses_FR_RESULT;

#if 0
		/* We can't do this because of env_offset not appearing in the list of uses */
		if (pt(stare) == NULL) {
			/* no uses, should have caonly flag set (not already set for params) */
			setcaonly(stare);
		}
#endif

		if (isvar(stare) && (!iscaonly(stare) || all_variables_visible)) {
			setvis(stare);
		}

		if (isparam(stare)) {
			if (child(stare)->tag != formal_callee_tag) {
				number_caller_parameter(stare);
			} else {
				number_callee_parameter(stare);
			}
		}

		nonevis &= !isvis(stare);

		/* Scan the body of the ident */
		arg = &next(child(stare));
		bdy = scan(arg, &arg);

		/* Scan the def of the ident */
		arg = &child(stare);
		defneeds = scan(arg, &arg);
		assert(stare == *e);

		nonevis = old_nonevis;
		t = child(stare);
		s = next(t);
		fxregble = fixregable(stare);
		flregble = floatregable(stare);
		uses_R_RESULT = (bdy.propneeds & uses_R_RESULT_bit) != 0;
		uses_FR_RESULT = (bdy.propneeds & uses_FR_RESULT_bit) != 0;

		if (child(stare)->tag == caller_name_tag) {
			/*
			 * IDENT is a caller in postlude
			 */
			no(stare) = R_NO_REG;

			/*
			 * At present all callers in postludes are only allowed on the stack
			 * This is because of the problems created by nested postludes
			 */
		} else if (isparam(stare) && child(stare)->tag == formal_callee_tag) {
			/*
			 * IDENT is a callee parameter
			 */
			no(stare) = R_NO_REG;
		}

		else if (isparam(stare) && child(stare)->tag != formal_callee_tag) {
			/*
			 * IDENT is a caller parameter
			 */
			if (do_profile) {
				/* mcount is called after the last param is dealt with */
				/* So we must put all params on the stack or in s-regs */
				bdy.propneeds |= hasproccall;
			}
			no(stare) = R_NO_REG;
		} else {
			/*
			 * IDENT is a normal ident
			 */
			assert(!isparam(*e));

			if (!isvis(*e) &&
			    (bdy.propneeds & anyproccall) == 0 &&
			    (
			        (uses_R_RESULT == 0 && fxregble) ||
			        (uses_FR_RESULT == 0 && flregble)
			    ) && (
			        t->tag == apply_tag || /* Let a := f()*/
			        (s->tag == seq_tag && next(child(s))->tag == res_tag &&
			         child(next(child(s)))->tag == cont_tag && isvar(stare) &&
			         child(child(next(child(s))))->tag == name_tag &&
			         child(child(child(next(child(s))))) == stare
					/* Let a := ..; return cont a */
			        )))
			{
				/* Ident suitable for res reg */
				if (fxregble) {
					stare->props |= inreg_bits;
					no(stare) = R_RESULT;
					bdy.propneeds |= uses_R_RESULT_bit;
				} else {
					stare->props |= infreg_bits;
					no(stare) = FR_RESULT;
					bdy.propneeds |= uses_FR_RESULT_bit;
				}
			}
#if 1
			else if (!isvar(*e) && !isparam(*e) &&
			         ((t->tag == reff_tag && child(t)->tag == cont_tag &&
			           child(child(t))->tag == name_tag && isvar(child(child(child(t))))
			           && !isvis(child(child(child(t)))) && !isglob(child(child(child(t))))
			           && unchanged(child(child(child(t))), stare)
			           /*
			            * reff cont variable-not assigned to in scope
			            */
			          ) ||
			          (t->tag == cont_tag && child(t)->tag == name_tag &&
			           isvar(child(child(t))) && !isvis(child(child(t))) && !isglob(child(child(t)))
			           && unchanged(child(child(t)), stare)
			           /*
			            * cont variable - not assigned to in scope
			            */
			          )
			         )
			        ) {
				stare->props |= defer_bit;
				/* dont take space for this dec */
			}
#endif
#if 0	/* dont undo large const in loop optimisation */
			else if (!isvar(stare) &&
			         (isusereg(stare) == 0)
			         && (t->tag == name_tag || t->tag == val_tag)) {
				stare->props |= defer_bit;
				/* dont take space for this dec */
			}
#endif
#if 1
			else if (!isvar(stare)
			         && t->tag == name_tag
			         && !isvar(child(t))
			         && !isvis(child(t))
			         && (child(t)->props & inanyreg))
			{
				/*
				 * dont take space for this constant dec,
				 * initialiser is another simple constant ident
				 * (eg from double nested loop optimisation)
				 */
				stare->props |= defer_bit;
			}
#endif
			/*
			 * All the parameters have been scanned at this point so
			 * maxfix gives the total no of free t-regs
			 */
			else if (fxregble &&
			         bdy.fixneeds < maxfix &&
			         (bdy.propneeds & morefix) == 0 &&
			         ((bdy.propneeds & anyproccall) == 0
#if 0
			          ||
			          tempdec(stare, ((bdy.propneeds & morefix) == 0) && bdy.fixneeds < 2)
#endif
			         )
			        ) {
				if ((stare->props & notparreg) == 0) {
					no(stare) = R_NO_REG;
					stare->props |= inreg_bits;
					if (uses_R_RESULT == 1) {
						stare->props |= notresreg;
					}
					bdy.fixneeds += 1;
				}
			} else if (flregble &&
			           bdy.floatneeds < maxfloat &&
			           (bdy.propneeds & morefloat) == 0 &&
			           ((bdy.propneeds & anyproccall) == 0
#if 0
			            ||
			            tempdec(stare, ((bdy.propneeds & morefloat) == 0 &&
			                            bdy.floatneeds < 1))
#endif
			           ))
			{
				if ((stare->props & notparreg) == 0) {
					/* Ident suitable for float t-reg */
					no(stare) = FR_NO_REG;
					stare->props |= infreg_bits;
					if (uses_FR_RESULT == 1) {
						stare->props |= notresreg;
					}
					bdy.floatneeds += 1;
				}
			} else {
				/*
				 * allocate either on stack or saved reg, decided later by
				 * regalloc()
				 */
				no(stare) = R_NO_REG;
			}
		}

		bdy = maxneeds(bdy, defneeds);
		if ((bdy.propneeds & usesproccall) != 0) {
			bdy.propneeds |= hasproccall;
		}

		return bdy;
	}

	case seq_tag: {
		exp *arg = &next(child(*e));
		needs an;
		exp *stat;
		exp *atsc = &child(child(*e));

		for (;;) {
			exp sc = *atsc;
			if (sc->tag == cond_tag && sh(child(sc))->tag == bothd
			    && next(child(next(child(sc))))->tag == top_tag) {
				/* sc is cond(... goto | make_top); can replace
				   make_top by next exp in sequence */
				exp lbst = next(child(sc));
				exp mkt = next(child(lbst));
				exp ne = (sc->last) ? next(child(*e)) : next(sc);
				exp bne = next(ne);
				bool lne = ne->last;

				if (ne->tag != cond_tag) {
					/* only worthwhile eliding if ne is a cond */
					if (sc->last) {
						break;
					}

					atsc = &next(sc);
					continue;
				}

				sh(sc) = sh(ne);
				next(ne) = lbst;
				ne->last = true;
				next(child(lbst)) = ne;

				/* sc is now cond( ... goto | next cond exp) */
				if (!sc->last) {
					/* not last in seq - swallow next*/
					next(sc) = bne;
					if (lne) {
						sc->last = true;
					} else {
						sc->last = false;
					}
					no(child(*e))--; /* one less statement */
				} else if (no(child(*e)) != 1) {
					/* last but not only - replace by
					   make_top and put cond in res posn */
					next(mkt) = next(sc);
					mkt->last = true;
					*atsc = mkt;
					next(sc) = bne;
					if (lne) {
						sc->last = true;
					} else {
						sc->last = false;
					}
					*arg = sc;
					sc = mkt;
				} else {
					/* whole sequence can be replace by cond */
					next(sc) = next(*e);
					if ((*e)->last) {
						sc->last = true;
					} else {
						sc->last = false;
					}
					*e = sc;
					return scan(e, at);
				}

			}

			if (sc->last) {
				break;
			}
			atsc = &next(sc);
		}

		an = scan(arg, &arg);
		stat = &child(child(*e));

		arg = stat;
		for (;;) {
			needs stneeds;

			stneeds = scan(stat, &arg);
			/* initial statements voided */
			an = maxneeds(an, stneeds);
			if ((*stat)->last) {
				if ((an.propneeds & usesproccall) != 0) {
					an.propneeds |= hasproccall;
				}
				return an;
			}
			stat = &next(*stat);
			arg = stat;
		}
	}

	case goto_tag:
	case trap_tag: /* By popular request the  infamous trap_tag */
		return zeroneeds;

	case ass_tag:
	case assvol_tag: {
		exp *lhs = &child(*e);
		exp *rhs = &next(*lhs);
		needs nr;
		ash a;

		nr = scan(rhs, at);
		/* scan source */

		a = ashof(sh(*(rhs)));
		if (!valregable(sh(*rhs))) {
			/* complicated memory move */
			if (!(a.ashsize <= 32 && a.ashsize == a.ashalign)) {
				nr.fixneeds += 2;    /* memory block copy */
			}
		}

		if ((*lhs)->tag == name_tag &&
		    ((isvar(child(*(lhs))) && !isglob(child(*(lhs)))) ||
		     ((nr.propneeds & (hasproccall | morefix)) == 0
		      && nr.fixneeds + 1 < maxfix)))
		{
			/* simple destination */
			if (isvar(child(*(lhs))) && isglob(child(*(lhs)))) {
				nr.fixneeds += 1;    /* for TOC access */
			}

			return nr;
		} else {
			needs nl;
			prop prps = (nr.propneeds & hasproccall) << 1;

			nl = scan(lhs, at); /* scan destination */

			if (APPLYLIKE(*(rhs)) &&
			    nstare == ass_tag &&
			    (nl.propneeds & (anyproccall | uses_R_RESULT_bit | uses_FR_RESULT_bit)) == 0)
			{
				/*
				 * source is proc call, so assign result reg directly
				 */
				;
			} else if (nr.fixneeds >= maxfix || prps != 0) {
				/*
				 * source and destination regs overlap, so identify source
				 */
				cca(at, rhs);
				nl = shapeneeds(sh(*(rhs)));
				nl.propneeds |= morefix;
				nl.propneeds &= ~(prps >> 1);
				nl.propneeds |= prps;
			}

			nr.fixneeds += 1;
			return maxneeds(nl, nr);
		}
	}

	case untidy_return_tag:
	case res_tag: {
		needs x;
		shape s;
		exp *arg = &child(*e);
		s = sh(*(arg));
		(*e)->props = 0;/* clear possibility of tlrecirsion; may be * set later */
		no_of_returns++;
		x = scan(arg, at); /* scan result exp ... */

		if (shape_size(s) != 0) {
			/* ...not a void result */
			x.propneeds |= has_result_bit;

			if (is_floating(s->tag)) {
				/* ... floating pt result */
				x.propneeds |= realresult_bit;
				if (s->tag != shrealhd) {
					x.propneeds |= longrealresult_bit;
				}
			} else {
				if (!valregable(s)) {
					assert(!redo_structfns);
					x.propneeds |= long_result_bit;
				}
			}
		}

#if 0
		/* +++ if we can avoid leaf proc conflict */
		if ((x.propneeds & (long_result_bit | anyproccall | uses_res_reg_bit)) == 0) {
			r = child(*(e));
			if (r->tag == ident_tag && isvar(r) &&
			    (ss = next(child(r)))->tag == seq_tag &&
			    (t = next(child(ss)))->tag == cont_tag &&
			    child(t)->tag == name_tag && child(child(t)) == r) {

				/*
				 * result is tag allocated into result reg - see ident_tag:
				 */
				if ((r->props & inreg_bits) != 0) {
					x.fixneeds--;
				} else if ((r->props & infreg_bits) != 0) {
					x.floatneeds--;
				} else {
					r->props |= (is_floating(s))->tag ? infreg_bits : inreg_bits;
				}
				x.propneeds |= uses_res_reg_bit;
				no(r) = R_USE_RES_REG;/* identification  uses result reg in body */
			}
		}
#endif
		return x;
	}

	case apply_tag: {
		exp application = *e;
		exp fn = child(application);
		exp *par = &next(fn);
		exp *fnexp = &child(*e);
		int parsize = 0;
		needs nds;
		bool regresult = reg_result(sh(application));
		int i;

		nds = scan(fnexp, at);
		/* scan the function exp ... */
		if ((nds.propneeds & hasproccall) != 0) {
			/* .... it must be identified */
			cca(at, fnexp);
			nds.propneeds &= ~hasproccall;
			nds.propneeds |= usesproccall;
			fn = child(application);
			par = &next(fn);
		}

		for (i = 1; !fn->last; ++i) {
			/* scan parameters in turn ... */
			needs onepar;
			shape shpar = sh(*par);
			int par_regs_used;
			int move_to_stack_regs;

			par_regs_used = (parsize >> 5);

			if (par_regs_used >= (end_param - R_FIRST_PARAM + 1)) {
				par_regs_used = (end_param - R_FIRST_PARAM + 1);
				move_to_stack_regs = 1;	/* can no longer move to param reg, need spare */
			} else {
				move_to_stack_regs = 0;
			}

			onepar = scan(par, at);

			asm_comment("scan: apply_tag: i=%d parn=%d pars=%d mover=%d",
			            i, onepar.fixneeds, par_regs_used, move_to_stack_regs);

			if (((i != 1 || regresult) && (onepar.propneeds & hasproccall) != 0)) {
				/* if it isn't the first parameter, and it calls a proc, identify it */
				asm_comment("scan apply_tag: cca bring forward apply");
				cca(at, par);
				nds.propneeds |= usesproccall;
				nds = maxneeds(shapeneeds(sh(*(par))), nds);
				nds.maxargs = MAX(nds.maxargs, onepar.maxargs);
			} else if ((i != 1 && (onepar.propneeds & hasproccall) != 0) ||
			           onepar.fixneeds >= 6 /* +++ remove safety net */ ||
			           onepar.fixneeds + move_to_stack_regs + par_regs_used > maxfix) {
				/* requires to many regs to evaluate, identify it */
				asm_comment("scan apply_tag: cca, param has too large fixneeds");
				cca(at, par);
				nds.propneeds |= morefix;
				nds = maxneeds(shapeneeds(sh(*(par))), nds);
				nds.maxargs = MAX(nds.maxargs, onepar.maxargs);
			} else {
				nds = maxneeds(onepar, nds);
			}

			parsize = ALIGNNEXT(parsize, shape_align(shpar));
			parsize = ALIGNNEXT(parsize + shape_size(shpar), 32);

			if ((*par)->last) {
				break;
			}

			par = &next(*par);
		}

		if (!regresult && father(application)->tag != ass_tag) {
			/* find space for non reg result */
			asm_comment("scan apply_tag: cca space for non reg result");
			cca(at, ptr_position(application));
			nds.propneeds |= usesproccall;
		} else {
			nds.propneeds |= hasproccall;
		}

		nds.maxargs = MAX(nds.maxargs, parsize);
		return nds;
	}

	case null_tag:
		/* many int optimisations keyed off val_tag so represent NULL as val_tag */
		(*e)->tag = val_tag;
		no(*e) = 0;		/* NULL represented as 0 */
		return shapeneeds(sh(*e));

	case val_tag: {
		exp s = sh(*e);

		if (s->tag == offsethd && al2(s) >= 8) {
			/* express disps in bytes */
			no(*e) = no(*e) >> 3;
		}

		/* ... and continue */
	}

	case name_tag:
	case real_tag:
	case string_tag:
	case env_offset_tag:
	case current_env_tag:
	case make_lv_tag:
	case last_local_tag:
		return shapeneeds(sh(*(e)));

	case clear_tag:
	case top_tag:
	case prof_tag:
	case local_free_all_tag:
		return zeroneeds;

	case neg_tag:
	case case_tag:
	case not_tag:
	case offset_negate_tag:
	case diagnose_tag:
	case goto_lv_tag:
	case alloca_tag:
		return scan(&child(*e), at);

	case abs_tag:
		if (cpu != CPU_RS6000) {
			/* We cant use abs instruction so we convert */
			shape int_shpe = sh(*e);
			exp arg = child(*e);
			exp id = me_startid(int_shpe, arg, 0);
			exp LABST__TAG;
			exp CLEAR__TAG;
			exp SEQ__TAG;
			exp TEST__TAG;
			exp VAL__TAG;
			exp NEG__TAG;
			exp COND__TAG;
			exp ZERO__TAG;
			exp ABS__TAG;
			ABS__TAG = *e;

			CLEAR__TAG = getexp(f_top, NULL, 0, NULL, NULL, 0, 0, clear_tag);
			LABST__TAG = me_b3(int_shpe, CLEAR__TAG, me_obtain(id), labst_tag);

			VAL__TAG  = me_shint(int_shpe, 0);
			TEST__TAG = me_q1(no_nat_option, f_less_than, &LABST__TAG,
			                  me_obtain(id), VAL__TAG, test_tag);
			NEG__TAG = me_u3(int_shpe, me_obtain(id), neg_tag);
			pt(NEG__TAG) = pt(ABS__TAG);
			(NEG__TAG)->props = (ABS__TAG)->props;

			ZERO__TAG = me_u3(f_top, TEST__TAG, 0);
			SEQ__TAG  = me_b3(int_shpe, ZERO__TAG, NEG__TAG, seq_tag);
			COND__TAG = me_b3(int_shpe, SEQ__TAG, LABST__TAG, cond_tag);

			id = me_complete_id(id, COND__TAG);

			if ((*e)->last) {
				id->last = true;
			} else {
				id->last = false;
			}

			next(id) = next(*e);
			*e = id;

			return scan(e, at);
		}

	case fneg_tag:
	case fabs_tag:
	case chfl_tag: {
		needs nds;

		nds = scan(&child(*e), at);
		return nds;
	}

	case bitf_to_int_tag:
	case int_to_bitf_tag: {
		exp *arg = &child(*e);

		return scan(arg, at);
	}

	case round_tag: {
		needs s;
		exp *arg = &child(*e);
		shape sres = sh(*e);

		if (shape_size(sres) != 32) {
			exp ch = getexp(sres, next(*e), (*e)->last, *e, pt(*e), (*e)->props, 0, chvar_tag);
			next(*e) = ch;
			(*e)->last = true;
			sh(*e) = slongsh;
			*e = ch;
			return scan(e, at);
		}

		s = scan(arg, at);
		s.fixneeds = MAX(s.fixneeds, 1);
		s.floatneeds = MAX(s.floatneeds, 2);

		return s;
	}

	case shl_tag:
	case shr_tag:
	case long_jump_tag: {
		exp *lhs = &child(*e);
		exp *rhs = &next(*lhs);
		needs nr;
		needs nl;
		prop prps;

		nr = scan(rhs, at);
		nl = scan(lhs, at);
		rhs = &next(*lhs);

		prps = (nr.propneeds & hasproccall) << 1;
		if (nr.fixneeds >= maxfix || prps != 0) {
			/*
			 * if reg requirements overlap, identify second operand
			 */
			cca(at, rhs);
			nl = shapeneeds(sh(*(rhs)));
			nl.propneeds |= morefix;
			nl.propneeds &= ~(prps >> 1);
			nl.propneeds |= prps;
		}

		nr.fixneeds += 1;
		return maxneeds(nl, nr);
	}

	case test_tag: {
		exp stare = *(e);
		exp l = child(stare);
		exp r = next(l);

		if (!stare->last && next(stare)->tag == test_tag &&
		    no(stare) == no(next(stare)) &&
		    stare->props == next(stare)->props &&
		    eq_exp(l, child(next(stare))) && eq_exp(r, next(child(next(stare)))))
		{
			/* same test following in seq list - remove
			* second test */
			if (next(stare)->last) {
				stare->last = true;
			}

			next(stare) = next(next(stare));
		}

		if (stare->last && next(stare)->tag == 0	/* seq holder */
		    && next(next(stare))->tag == test_tag &&
		    next(next(next(stare)))->tag == seq_tag &&
		    no(stare) == no(next(next(stare))) &&
		    stare->props == next(next(stare))->props &&
		    eq_exp(l, child(next(next(stare))))
		    && eq_exp(r, next(child(next(next(stare))))))
		{
			/* same test following in seq res - void
			* second test */
			next(next(stare))->tag = top_tag;
			child(next(next(stare))) = NULL;
			pt(next(next(stare))) = NULL;
		}

		/*
		 * commute tests if
		 * a) lhs a const (to support convention of const on rhs); or
		 * b) rhs is a load from mem, move to lhs in hope of reducing
		 *    load-use delays
		 */
		if (( test_number(stare) == TEST_NE || test_number(stare) == TEST_EQ ||
		      !is_floating(sh(l)->tag))
		    &&
		    ((l->tag == val_tag) || (LOADFROMSTORE(r) && !LOADFROMSTORE(l))))
		{
			/* commute */
			next(l) = stare;
			l->last = true;
			next(r) = l;
			r->last = false;
			child(stare) = r;
			r = l;
			l = child(stare);
			settest_number(stare, cbranch(test_number(stare)));
		}

		if (is_floating(sh(l)->tag)) {
			return fpop(e, at);
		} else if (r->tag == val_tag && no(r) == 1
		           && (test_number(stare) == TEST_GE ||
		               test_number(stare) == TEST_LT))
		{
			/*
			 * The only reason for this optimisation is that it increases
			 * the chance of using the Record bit.
			 */
			no(r) = 0;
			if (test_number(stare) == TEST_GE) {
				/* Branch >=1 is same as >0 */
				settest_number(stare, TEST_GT);
			} else {
				/* Branch <1 is same as <= 0 */
				settest_number(stare, TEST_LE);
			}
		}

		return non_commutative_scan(e, at);
	}

	case movecont_tag: {
		exp *d  = &child(*e);
		exp *s  = &next(*d);
		exp *sz = &next(*s);
		needs nd;
		needs ns;
		needs nsz;
		prop prps;
		nd  = scan(d, at);
		ns  = scan(s, at);
		nsz = scan(sz, at);
		prps = (ns.propneeds & hasproccall) << 1;

		if (ns.fixneeds >= maxfix || prps != 0) {
			/* if reg requirements overlap, identify second operand */
			cca(at, s);
			ns = shapeneeds(sh(*(s)));
			ns.propneeds |= morefix;
			ns.propneeds &= ~(prps >> 1);
			ns.propneeds |= prps;
		}

		nd.fixneeds += 1;
		nd = maxneeds(nd, ns);
		prps = (nsz.propneeds & hasproccall) << 1;
		if (nd.fixneeds + nsz.fixneeds >= maxfix || prps != 0) {
			/* if reg requirements overlap, identify last operand */
			cca(at, sz);
			nsz = shapeneeds(sh(*(sz)));
			nsz.propneeds |= morefix;
			nsz.propneeds &= ~(prps >> 1);
			nsz.propneeds |= prps;
		}

		nd.fixneeds += 1;
		nd = maxneeds(nd, nsz);
		if (nd.fixneeds < 4) {
			nd.fixneeds = 3;
		}

		return nd;
	}

	case plus_tag: {
		/* replace any operands which are neg(..) by - if poss */
		exp sum = *(e);
		exp list = child(sum);
		bool someneg = 0;
		bool allneg = 1;

		/* check BUGP13 [corruption by extract_consts()] is fixed */
		/* check father set correctly */
		assert(father(child(sum)) == sum);

		for (; optop(sum);) {
			if (list->tag == neg_tag) {
				someneg = 1;
			} else {
				allneg = 0;
			}

			if (list->last) {
				break;
			}

			list = next(list);
		}

		if (someneg) {
			/* there are some neg() operands */
			if (allneg) {
				/* transform -..-... to -(..+.. +...) */
				exp x;

				/*
				 * Build a new list form operand of neg_tags, which will
				 * become plus_tag operands.
				 */
				x = child(sum);
				list = child(x);
				for (;;) {
					/*
					 * 'x' moves along neg_tag's lists
					 * 'list' moves along children of neg_tag's lists, building a new list
					 * eventually new list is made child of plus_tag
					 */

					assert(list == child(x));

					next(list) = child(next(x));
					if (!x->last) {
						list->last = false;
						list = next(list);
						x = next(x);
					} else {

						list->last = true;
						next(list) = sum;		/* set father to be */
						child(sum) = child(child(sum)); /* set new children of plus_tag */
						break;
					}
				}

				/*
				 * create new neg_tag to replace plus_tag,
				 * old plus_tag being the operand of the new neg_tag.
				 */
				x = getexp(sh(sum), next(sum), sum->last, sum, NULL,
				           0, 0, neg_tag);

				sum->last = true;

				/* set father of sum, new neg_tag exp */
				next(sum) = x;

				/* check father of sum is correct */
				assert(father(child(sum)) == sum);

				*e = x;
			} else {
				/* transform to  ((..(..+..) - ..) -..) */
				int n = 0;
				exp nextsum = next(sum);
				bool lastsum = sum->last;
				exp x = child(sum);
				exp newsum = sum;

				list = NULL;
				for (;;) {
					exp nxt = next(x);
					bool final = x->last;

					if (x->tag == neg_tag) {
						next(child(x)) = list;
						list = child(x);
					} else {
						next(x) = newsum;
						newsum = x;
						if ((n++) == 0) {
							newsum->last = true;
						} else {
							newsum->last = false;
						}
					}

					if (final) {
						break;
					}

					x = nxt;
				}

				if (n > 1) {
					child(sum) = newsum;
					newsum = sum;	/* use existing exp for add operations */
				}

				for (;;) {
					/* introduce - operations */
					exp nxt = next(list);

					next(newsum) = list;
					newsum->last = false;
					x = getexp(sh(sum), NULL, 0, newsum, NULL, 0, 0, minus_tag);

					next(list) = x;
					list->last = true;
					newsum = x;
					if ((list = nxt) == NULL) {
						break;
					}
				}

				next(newsum) = nextsum;
				if (lastsum) {
					newsum->last = true;
				} else {
					newsum->last = false;
				}

				*e = newsum;
			}

			/* check father set correctly */
			assert(father(child(*e)) == *e);

			return scan(e, at);
		}
	}

	case addptr_tag: {
		exp p = child(*e);
		exp d = next(p);

		int fal = frame_al_of_ptr(sh(p));
		if (fal != 0 && i_reckon_its_a_general_proc(fal)) {
			int oal = frame_al1_of_offset(sh(d));
			/*	if( ((oal-1)&oal) != 0)
				{
				  error(ERR_SERIOUS, "can't cope with mixed frame offsets yet");
				}*/
			if (!l_or_cees(oal)) {
				/* callers are referenced through R_TP */
				/* to get this we use locptr to access through R_FP(current_env)*/
				exp ne = getexp(sh(p), d, 0, p, NULL, 0, 0, locptr_tag);
				next(p) = ne;
				p->last = true;
				child(*e) = ne;
			}
		}
		/* ... and continue */
	}

	case local_free_tag:
	case mult_tag:
	case and_tag:
	case or_tag:
	case xor_tag:
		return commutative_scan(e, at);

	case reff_tag:
	case offset_pad_tag:
	case chvar_tag:
	case locptr_tag: {
		exp *arg = &child(*e);
		return maxneeds(scan(arg, at), shapeneeds(sh(*(e))));
	}

	case float_tag: {
		needs nds;
		exp *arg = &child(*e);

		nds = maxneeds(scan(arg, at), shapeneeds(sh(*(e))));
		if ((sh(child(*(e))))->tag == ulonghd) {
			if (nds.floatneeds < 2) {
				nds.floatneeds = 2;
			}
		}

		return nds;
	}

	case cont_tag:
	case contvol_tag: {
		exp *arg = &child(*e);
		needs nds;

		nds = maxneeds(scan(arg, at), shapeneeds(sh(*(e))));
		nds.fixneeds = MAX(nds.fixneeds, 2);
		return nds;
	}

	case offset_mult_tag:
	case offset_div_tag: {
		exp op1 = child(*e);
		exp op2 = next(op1);
		shape s = sh(op2);

		if (op2->tag == val_tag  && s->tag == offsethd && al2(s) >= 8) {
			int n = no(op2) / 8;
			if (n == 1) {
				/* offset is one  byte */
				next(op1) = next(*e);
				if ((*e)->last) {
					op1->last = true;
				} else {
					op1->last = false;
				}
				*e = op1;
				return scan(e, at);
			} else if ((*e)->tag == offset_mult_tag && n > 1 && (n & (n - 1)) == 0) {
				if (op1->tag == and_tag
				    && child(op1)->tag == shr_tag &&
				    next(child(op1))->tag == val_tag) {
					exp shexp = child(op1);
					exp ac = next(shexp);
					exp shop1 = child(shexp);
					exp shop2 = next(shop1);
					int na = no(ac);

					if ((na & (na + 1)) == 0 && shop2->tag == val_tag) {
						int pn = 0;
						int ns = no(shop2);
						int i = n;
						while (i > 1) {
							i >>= 1;
							pn++;
						}

						if (ns > pn) {
							/* can do transform: (((shop1>>ns) & na) * n) =>
							   shop1>>(ns-pn) & (na*n) */
							no(shop2) = ns - pn;
							no(ac) = na * n;
							next(op1) = next(*e);
							if ((*e)->last) {
								op1->last = true;
							} else {
								op1->last = false;
							}
							*e = op1;
							return scan(e, at);
						}
					}
				} else {
					/* will do this by literal shift */
					no(op2) = n;
					return scan(&child(*e), at);
				}
			}
		}

		return non_commutative_scan(e, at);
	}

	case div0_tag:
	case div1_tag:
	case div2_tag:
	case rem0_tag:
	case mod_tag:
	case rem2_tag:
	case minus_tag:
	case subptr_tag:
	case minptr_tag:
	case offset_div_by_int_tag:
	case max_tag:
	case min_tag:
	case offset_max_tag:
	case component_tag:
	case make_stack_limit_tag:
		return non_commutative_scan(e, at);

	case offset_add_tag: {
		exp l = child(*e);
		exp r = next(l);

		if (l->tag == val_tag) {
			sh(l) = sh(r);   /* both offsets will be treated the same */
			child(*e) = r;
			r->last = false;
			next(r) = l;
			l->last = true;
			next(l) = *e;
			/* ... and put val last */
		} else {
			if (al2(sh(l)) >= 8 && al2(sh(r)) < 8) {
				return non_commutative_scan(e, at);
			}
		}

		(*e)->tag = plus_tag;

		return commutative_scan(e, at);
	}

	case offset_subtract_tag: {
		exp l = child(*e);
		exp r = next(l);

		if (r->tag == val_tag) {
			sh(r) = sh(l);   /* both offsets will be treated the same */
		} else if (al2(sh(r)) >= 8 && al2(sh(l)) < 8) {
			return non_commutative_scan(e, at);
		}

		(*e)->tag = minus_tag;

		return non_commutative_scan(e, at);
	}

	case fdiv_tag:
	case fplus_tag:
	case fminus_tag:
	case fmult_tag: {
		exp op = *(e);
		exp a2 = next(child(op));

		if (!a2->last) {
			/*
			 * + and * can have >2 parameters - make them diadic
			 * - can do better a+exp => let x = exp in a+x
			 */
			exp opn = getexp(sh(op), op, 0, a2, NULL, 0, 0, op->tag);

			/* dont need to transfer error treatment - nans */
			exp nd = getexp(sh(op), next(op), op->last, opn, NULL, 0, 1, ident_tag);
			exp id = getexp(sh(op), op, 1, nd, NULL, 0, 0, name_tag);

			pt(nd) = id;
			next(child(op)) = id;
			op->last = true;
			next(op) = nd;

			while (!a2->last) {
				a2 = next(a2);
			}

			next(a2) = opn;
			*e = nd;
			return scan(e, at);
		}

		return fpop(e, at);
	}

	case field_tag: {
		needs str;
		exp *arg = &child(*e);

		if (chase(*e, arg)) {
			/* field has been distributed */
			exp stare = *e;
			exp ss = child(stare);

			if (!stare->last) {
				ss->last = false;
			}

			next(ss) = next(stare);
			sh(ss) = sh(stare);
			*e = ss;
			return scan(e, at);
		}

		str = scan(arg, at);
		return maxneeds(str, shapeneeds(sh(*(e))));
	}

	case proc_tag:
	case general_proc_tag: {
		exp *bexp;
		exp *bat;
		needs body;

		exp_num = 0;
		callee_size = 0;
		max_callees = -1;
		no_of_returns = 0;
		maxfix = maxfix_tregs;
		maxfloat = MAXFLT_TREGS;
		stparam = 0;
		fixparam = R_FIRST_PARAM;
		floatparam = FR_FIRST_PARAM;

		/* Parameter allocation t-regs */
		freefixed = PROC_TREGS;
		freefloat = PROC_FLT_TREGS;

		if ((*e)->tag == general_proc_tag) {
			end_param = GENERAL_PROC_PARAM_REGS + R_FIRST_PARAM - 1;
		} else {
			end_param = PROC_PARAM_REGS + R_FIRST_PARAM - 1;
		}

		nonevis   = true;
		gen_call  = false;
		tail_call = false;
		bexp = &child(*e);
		bat = bexp;
		body = scan(bexp, &bat);

		if (do_dynamic_init && proc_is_main(*e)) {
			/* we need a call to __main */
			body.propneeds |= usesproccall;
		}

		return body;
	}

	case apply_general_tag: {
		exp application = *e;
		exp *fn = &child(application);
		exp cers = next(*fn);
		exp *cerl = &child(cers);
		long stpar = 0;
		needs nds;
		needs plnds;
		int i;

		gen_call = true;

		/* scan the function */
		nds = scan(fn, at);

		if ((nds.propneeds & hasproccall) != 0) {
			/* .... it must be identified */
			cca(at, fn);
			nds.propneeds &= ~hasproccall;
			nds.propneeds |= usesproccall;
			fn = &child(application);
		}

		/* scan the callers */
		for (i = 0; i < no(cers); i++) {
			needs onepar;
			shape shonepar = sh(*cerl);
			exp * par = ((*cerl)->tag == caller_tag) ? &child(*cerl) : cerl;
			int n = ALIGNNEXT(stpar, shape_align(shonepar));
			onepar = scan(par, at);

			if ((i != 0 && (onepar.propneeds & hasproccall) != 0) ||
			    onepar.fixneeds + (stpar >> 5) > maxfix) {
				/* +++ if we go over a certain number of param regs
				   they are forced to be on the stack so stpar>>5
				   is not the best estimate ,but sufficient*/
				/* stpar>>5 is the no of param regs used so far */
				/* if it isn't the first parameter, and it
				   calls a proc, identify it */
				/* it is ok for first param to have a proccall since we have
				   no loaded parameters to corrupt */
				cca(at, par);
				nds.propneeds |= usesproccall;
				nds = maxneeds(shapeneeds(sh(*(par))), nds);
				nds.maxargs = MAX(nds.maxargs, onepar.maxargs);
			} else {
				nds = maxneeds(onepar, nds);
			}

			if ((*cerl)->tag == caller_tag) {
				/* for caller_tag's we record where it will live */
				no(*cerl) = n;
				clear_coded_caller(*cerl);
			}

			n = n + shape_size(shonepar);
			stpar = ALIGNNEXT(n, 32);
			cerl = &next(*cerl);
		}

		nds.maxargs = MAX(nds.maxargs, stpar);
		/* scan the callees */
		nds = maxneeds(scan(&next(next(child(application))), at), nds);
		/* scan the postlude */
		plnds = scan(&next(next(next(child(application)))), at);

		if (plnds.propneeds & anyproccall) {
			application->props = 1;
			if (is_floating(sh(application)->tag) || valregable(sh(application))) {
				cca(at, ptr_position(application));
				plnds.propneeds |= usesproccall;
			}
		} else {
			application->props = 0;
			if (is_floating(sh(application)->tag) || valregable(sh(application))) {
				cca(at, ptr_position(application));
			}
		}

		nds = maxneeds(nds, plnds);
		nds.propneeds |= hasproccall;
		return nds;
	}

	case make_callee_list_tag: {
		exp cees = *e;
		exp *par = &child(cees);
		needs nds;
		long stpar = 0;
		int i;

		nds = zeroneeds;

		for (i = 0; i < no(cees); i++) {
			/* scan each callee and identify if necessary */
			needs onepar;
			shape shonepar = sh(*par);
			int n = ALIGNNEXT(stpar, shape_align(shonepar));
			onepar = scan(par, at);

			if ((onepar.propneeds & hasproccall) != 0 || onepar.fixneeds + 1 > maxfix) {
				/* if it calls a proc identify it */
				cca(at, par);
				nds.propneeds |= usesproccall;
				nds = maxneeds(shapeneeds(sh(*par)), nds);
				nds.maxargs = MAX(nds.maxargs, onepar.maxargs);
			} else {
				nds = maxneeds(onepar, nds);
			}

			n += shape_size(shonepar);
			stpar = ALIGNNEXT(n, 32);
			par = &next(*par);
		}

		no(cees) = stpar; /* The total no of bits needed for callees */
		max_callees = MAX(max_callees, stpar);
		return nds;
	}

	case make_dynamic_callee_tag: {
		exp cees = *e;
		exp *ptr = &child(cees);
		needs ndsp;
		needs nds;
		nds = zeroneeds;
		ndsp = scan(ptr, at);

		if (((ndsp.propneeds & hasproccall) != 0) ||
		    ndsp.fixneeds + 1 > maxfix) {
			cca(at, ptr);
			nds.propneeds |= usesproccall;
			nds = maxneeds(shapeneeds(sh(*(ptr))), nds);
			nds.maxargs =  MAX(nds.maxargs, ndsp.maxargs);
		} else {
			nds = ndsp;
		}

		ndsp = scan(&next(child(*e)), at);
		if (((ndsp.propneeds & hasproccall) != 0) ||
		    ndsp.fixneeds + 2 > maxfix) {
			cca(at, &next(child(cees)));
			nds.propneeds |= usesproccall;
			nds = maxneeds(shapeneeds(sh(next(child(*e)))), nds);
			nds.maxargs = MAX(nds.maxargs, ndsp.maxargs);
		} else {
			nds = maxneeds(ndsp, nds);
		}

		if (nds.fixneeds < 5) {
			nds.fixneeds = 5;
		}

		return nds;
	}

	case tail_call_tag: {
		needs ndsp;
		needs nds;
		exp *fn = &child(*e);
		ndsp = scan(fn, at);
		tail_call = true;

		if (((ndsp.propneeds & hasproccall) != 0) || ndsp.fixneeds + 1 > maxfix) {
			cca(at, fn);
			nds.propneeds |= usesproccall;
			nds = maxneeds(shapeneeds(sh(*fn)), nds);
			nds.maxargs = MAX(nds.maxargs, ndsp.maxargs);
		} else {
			nds = ndsp;
		}

		gen_call = true;
		ndsp     = scan(&next(child(*e)), at);
		nds      = maxneeds(nds, ndsp);
		return nds;
	}

	case same_callees_tag: {
		needs nds;
		nds = zeroneeds;
		nds.fixneeds = 4;
		max_callees = MAX(max_callees, callee_size);
		return nds;
	}

	case env_size_tag:
	case set_stack_limit_tag:
	case return_to_label_tag: {
		exp *arg = &child(*e);
		return scan(arg, at);
	}

	case general_env_offset_tag:
	case caller_name_tag:
		return shapeneeds(sh(*e));

	case formal_callee_tag:
		return zeroneeds;

	case caller_tag: {
		error(ERR_SERIOUS, "Shouldn't be scanning a caller_tag");
		return zeroneeds;
	}

	default:
		asm_comment("scan: bad nstare=%d", nstare);
		error(ERR_SERIOUS, "case not covered in needs scan");
		return zeroneeds;
	}
}

static int
scan_cond(exp *e, exp outer_id)
{
	exp ste = *e;
	exp first = child(ste);
	exp labst = next(first);
	exp second = next(child(labst));

	assert(ste->tag == cond_tag);

	/*
	 * cond is { ... test(L); ? ; goto X | L:make_top}
	 * if ? empty can replace by seq { ... not-test(X); make_top }
	 */
	if (second->tag == top_tag && sh(first)->tag == bothd && no(child(labst)) == 1
	    && first->tag == seq_tag && next(child(first))->tag == goto_tag)
	{
		exp l;

		for (l = child(child(first)); !l->last; l = next(l))
			;

		while (l->tag == seq_tag) {
			l = next(child(l));
		}

		if (l->tag == test_tag && pt(l) == labst) {
			settest_number(l, notbranch[test_number(l)]);
			pt(l) = pt(next(child(first)));
			next(child(first)) = second;
			next(second) = first;
			second->last = true;
			next(first) = next(ste);

			if (ste->last) {
				first->last = true;
			} else {
				first->last = false;
			}

			*e = first;
			return 1;
		} else {
			return 0;
		}
	}

	if (first->tag == seq_tag && second->tag == cond_tag
	    && no(child(labst)) == 1
	    && child(child(first))->tag == test_tag
	    && pt(child(child(first))) == labst
	    && child(second)->tag == seq_tag
	    && child(child(child(second)))->tag == test_tag)
	{
		/* cond is ( seq (test to L;....|
		 * L:cond(seq(test;...),...) ) ..... */

		exp test1 = child(child(first));
		exp test2 = child(child(child(second)));
		exp op11 = child(test1);
		exp op21 = next(op11);
		exp op12 = child(test2);
		exp op22 = next(op12);
		bool c1 = complex(op11);
		bool c2 = complex(op21);

		if (c1 && eq_exp(op11, op12)) {
			/* ....if first operands of tests are same, identify them */
			exp newid = getexp(sh(ste), next(ste), ste->last, op11, NULL,
			                   0, 2, ident_tag);
			exp tag1 = getexp(sh(op11), op21, 0, newid, NULL, 0, 0, name_tag);
			exp tag2 = getexp(sh(op12), op22, 0, newid, NULL, 0, 0, name_tag);

			pt(newid) = tag1;
			pt (tag1) = tag2;	/* uses of newid */
			next (op11) = ste;
			op11->last = false;/* body of newid */
			/* forget child test2 = child test1 */
			next(ste) = newid;
			ste->last = true;	/* father body = newid */
			child(test1) = tag1;
			child (test2) = tag2;	/* relace 1st operands of test */

			if (!complex(op21)) {
				/* if the second operand of 1st test is simple, then identification
				 * could go in a t-teg (!!!NB overloading of inlined flag!!!).... */
				setinlined(newid);
			}

			kill_exp(op12, op12);
			*(e) = newid;
			if (scan_cond(&next(child(labst)), newid) == 2 && complex(op22)) {
				/* ... however a further use of identification means that
				   the second operand of the second test must also be simple */
				clearinlined(newid);
			}

			return 1;
		} else if (c2 && eq_exp(op21, op22)) {
			/* ....if second operands of tests are
			 * same, identify them */

			exp newid = getexp(sh(ste), next(ste), ste->last, op21, NULL, 0, 2, ident_tag);
			exp tag1 = getexp(sh(op21), test1, 1, newid, NULL, 0, 0, name_tag);
			exp tag2 = getexp(sh(op22), test2, 1, newid, NULL, 0, 0, name_tag);

			pt(newid) = tag1;
			pt (tag1) = tag2;	/* uses of newid */
			next(op21) = ste;
			op21->last = false;
			/* body of newid */
			/* forget next child test2 = next child test1 */
			next(ste) = newid;
			ste->last = true;	/* father body = newid */
			next(op11) = tag1;
			next(op12) = tag2;

			if (!complex(op11)) {
				setinlined(newid);
			}

			kill_exp(op22, op22);
			/* relace 2nd operands of test */
			*(e) = newid;
			if (scan_cond(&next(child(labst)), newid) == 2 && complex(op12)) {
				clearinlined(newid);
			}
			return 1;
		} else if (op12->tag != name_tag
		           && op11->tag == name_tag
		           && child(op11) == outer_id
		           && eq_exp(child(outer_id), op12))
		{
			/* 1st param of test1 is already identified with 1st param of  test2 */
			exp tag = getexp(sh(op12), op22, 0, outer_id,
			                 pt(outer_id), 0, 0, name_tag);
			pt(outer_id) = tag;
			no(outer_id) += 1;

			if (complex(op21)) {
				clearinlined(outer_id);
			}

			/* update usage of ident */
			child(test2) = tag;
			kill_exp(op12, op12);
			if (scan_cond(&next(child(labst)), outer_id) == 2 && complex(op22)) {
				clearinlined(outer_id);
			}

			return 2;
		}
	}

	return 0;
}

static void
number_caller_parameter(exp param_id)
{
	exp init_exp = child(param_id);
	shape param_shape = sh(init_exp);
	long par_size = shape_size(param_shape);
	long par_stack_location = ALIGNNEXT(stparam, 32);

	assert(init_exp->tag == clear_tag);

	if (is_floating(param_shape->tag)) {
		if (floatparam <= FR_LAST_PARAM) {
			init_exp->props = floatparam;
			floatparam++;
		} else {
			init_exp->props = 0;/*passed by stack */
		}
	} else {
		if (fixparam <= end_param) {
			init_exp->props = fixparam;
		} else {
			init_exp->props = 0;/*passed by stack*/
		}
	}

	no(init_exp) = par_stack_location;
	stparam = ALIGNNEXT(par_stack_location + par_size, 32);
	fixparam = R_FIRST_PARAM + (stparam / 32);
}

static void
number_callee_parameter(exp callee_id)
{
	exp def = child(callee_id);
	shape callee_shape = sh(def);
	long size_of_callee = shape_size(callee_shape);
	long alignment_of_callee = shape_align(callee_shape);
	long n = ALIGNNEXT(callee_size , alignment_of_callee);

	no(def) = n;
	callee_size = ALIGNNEXT(n + size_of_callee, 32);
}

