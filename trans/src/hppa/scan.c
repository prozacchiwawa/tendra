/*
 * Copyright 2002-2012, The TenDRA Project.
 * Copyright 1997, United Kingdom Secretary of State for Defence.
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
#include <stdio.h>
#include <string.h>

#include <shared/bool.h>
#include <shared/check.h>
#include <shared/error.h>

#include <local/ash.h>
#include <local/exp.h>
#include <local/tag.h>

#include <tdf/shape.h>
#include <tdf/tag.h>

#include <reader/exp.h>

#include <construct/installtypes.h>
#include <construct/dec.h>
#include <construct/ash.h>
#include <construct/exp.h>
#include <construct/install_fns.h>

#include <flpt/flpt.h>

#include <utility/max.h>

#include <main/flags.h>
#include <main/print.h>

#include <refactor/const.h>
#include <reader/externs.h>
#include <refactor/optimise.h>
#include <refactor/refactor.h>

#include "procrec.h"
#include "bits.h"
#include "regable.h"
#include "tempdecs.h"
#include "special.h"
#include "reg.h"
#include "muldvrem.h"
#include "translate.h"
#include "proc.h"
#include "frames.h"
#include "scan.h"

/* used by scan to set initial parameter positions */
extern alignment long_to_al(unsigned long);
extern int specialopt(exp);

extern long notbranch[]; /* in makecode.c */
extern int nexps;

extern procrec *cpr;

static int stparam;  /* Size of parameter list in bits */
static int fixparam; /* Next available place for param */
static int fltparam;

static bool nonevis     = true;
static int callerfortr;

static bool gen_call;
static bool has_tail_call;

int maxfix, maxfloat; /* the maximum number of t-regs */


/* advance declaration of scan */
needs scan(exp *, exp **);

/*
 * needs defined in procrec.h.
 *
 * This is a structure which has two integers giving the number of
 * fixed and floating point registers required to contain live values in
 * the expression parameters. A further field prop is used for various
 * flags about certain forms of exp (mainly idents and procs).
 *
 * The maxargs field gives the maximum size in bits for the parameters of
 * all the procs called in the exp. The needs of a proc body are preserved
 * in the needs field of the procrec (see procrec.h).
 */

/* return ptrexp pointing to e */
static exp *
ptr_position(exp e)
{
	exp *res;
	exp dad = father(e);
	exp sib = child(dad);

	if (sib == e) {
		res = &child(dad);
	} else {
		while (next(sib) != e) {
			sib = next(sib);
		}

		res = &next(sib);
	}

	return res;
}

/*
 * This procedure effectively inserts a new declaration into an exp. This
 * is used to stop a procedure requiring more than the available number of
 * registers.
 */
static void
cca(exp **to, exp *x)
{
	if ((**to)->tag == diagnose_tag) {
		*to = &child(**to);
	}

	if (x == *to) {
		exp def = *x;

		/* replace by  Let tag = def In tag Ni */
		exp id = getexp(sh(def), next(def), def->last, def, NULL, 0, 1, ident_tag);
		exp tag = getexp(sh(def), id, 1, id, NULL, 0, 0, name_tag);

		pt(id)   = tag; /* use of tag */
		next(def) = tag; /* next(def) is body of Let = tag */
		def->last = false;

		*x = id;	 	/* replace pointer to x by Let */
	} else {
		/* replace by Let tag = def In ato/def = tag Ni */
		exp def = *x;
		exp ato = **to;
		exp id  = getexp(sh(ato), next(ato), ato->last, def, NULL, 0, 1, ident_tag);
		exp tag = getexp(sh(def), next(def), def->last, id, NULL, 0, 0, name_tag);

		pt(id)   = tag;	/* use of tag */
		next(def) = ato;	/* ato is body of Let */
		def->last = false;
		next(ato) = id;	/* its father is Let */
		ato->last = true;

		**to = id;	/* replace pointer to 'to' by Let */
		*x   = tag;		/* replace use of x by tag */
		*to  = &next(def); /* later replacement to same 'to' will be at * body of Let */
	}
}

static needs onefix    = { 1, 0, 0, 0 }; /* needs one fix pt reg */
static needs twofix    = { 2, 0, 0, 0 }; /* needs 2 fix pt regs */
static needs onefloat  = { 0, 1, 0, 0 }; /* needs 1 flt pt regs */
static needs zeroneeds = { 0, 0, 0, 0 }; /* has no needs */

#if 0
/* +++ optimise sharing of regs for idents */
/* check to see if any uses of id is initialiser to subvar dec */
static bool
subvar_use(exp uses)
{
	for (; uses != NULL; uses = pt(uses)) {
		if (uses->last && next(uses)->tag == cont_tag) {
			exp c = next(uses);

			if (!c->last && next(c)->last && next(next(c))->tag == ident_tag) {
				exp id = next(next(c));

				if ((id->props & subvar) != 0 && (id->props & inanyreg) != 0) {
					return 1;
				}
			}
		}
	}

	return 0;
}
#endif

/* this gives the needs for manipulating a value of shape s */
static needs
shapeneeds(shape s)
{
	if (is_floating(s->tag)) {
		return onefloat;
	} else {
		if (valregable(s)) {
			return onefix;
		} else {
			/* if the shape does not fit into a reg, needs two fixed regs for moving */
			return twofix;
		}
	}
}

/*
 * Transform a non-bit offset into a bit offset.
 * (borrowed from trans386)
 */
static void make_bitfield_offset
(exp e, exp pe, int spe, shape sha)
{
	exp omul;
	exp val8;

	if (e->tag == val_tag) {
		no(e) *= 8;
		return;
	}

	omul = getexp(sha, next(e), (int)(e->last), e, NULL, 0, 0, offset_mult_tag);
	val8 = getexp(slongsh, omul, 1, NULL, NULL, 0, 8, val_tag);
	e->last = false;
	setnext(e, val8);

	if (spe) {
		child(pe) = omul;
	} else {
		next(pe) = omul;
	}
}

/*
 * These are basicly the expressions which cannot be accessed by a simple
 * load or store instruction.
 */
static bool
complex(exp e)
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

/* change identity to variable definition */
static void
change_to_var(exp e)
{
	exp p = pt(e);
	shape ns;
	assert(e->tag == ident_tag && !isvar(e));
	setvar(e);
	setcaonly(e);
	ns = f_pointer(f_alignment(sh(child(e))));

	while (p != NULL) {
		exp * pos = ptr_position(p);
		exp ncont = getexp(sh(p), next(p), p->last, p, NULL, 0, 0, cont_tag);
		next(p) = ncont;
		p->last = true;
		sh(p) = ns;
		*pos = ncont;
		p = pt(p);
	}
}

/* replace uses of ident f (!= except) to uses of t */
static void
change_names(exp f, exp t, exp except)
{
	exp py = pt(f);
	assert(f->tag == ident_tag && t->tag == ident_tag && except->tag == name_tag);

	while (py != NULL) {
		exp ppy = pt(py);
		if (py != except) {
			child(py) = t; /* change f to t */
			pt(py) = pt(t);
			pt(t) = py;
			no(t)++;  /* maintain usage */
		}

		py = ppy;
	}
}

static int
scan_cond(exp * e, exp outer_id)
{
	exp ste = *e;
	exp first = child(ste);
	exp labst = next(first);
	exp second = next(child(labst));

	assert(ste->tag == cond_tag);

	if (second->tag == top_tag && sh(first)->tag == bothd && no(child(labst)) == 1
	    && first->tag == seq_tag && next(child(first))->tag == goto_tag) {
		/* cond is { ... test(L); ? ; goto X | L:make_top}
			if ? empty can replace by seq { ... not-test(X); make_top }
		*/

		exp l = child(child(first));
		while (!l->last) {
			l = next(l);
		}

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
		}

		return 0;
	}

	if (first->tag == seq_tag && second->tag == cond_tag
	    && no(child(labst)) == 1
	    && child(child(first))->tag == test_tag
	    && pt(child(child(first))) == labst
	    && child(second)->tag == seq_tag
	    && child(child(child(second)))->tag == test_tag)
	{
		/* cond is ( seq (test to L;....|
		   L:cond(seq(test;...),...) ) ..... */
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
			exp newid = getexp(sh(ste), next(ste), ste->last, op11, NULL, 0, 2, ident_tag);
			exp tag1 = getexp(sh(op11), op21, 0, newid, NULL, 0, 0, name_tag);
			exp tag2 = getexp(sh(op12), op22, 0, newid, NULL, 0, 0, name_tag);

			pt(newid)  = tag1;
			pt (tag1)  = tag2;	/* uses of newid */
			next (op11) = ste;
			op11->last = false;/* body of newid */
			/* forget child test2 = child test1 */
			next(ste) = newid;
			ste->last = true;	/* father body = newid */
			child(test1) = tag1;
			child(test2) = tag2;	/* relace 1st operands of test */

			if (!complex(op21)) {
				/*
				 * if the second operand of 1st test is simple, then identification
				 * could go in a t-teg (!!!NB overloading of inlined flag!!!)....
				 */
				setinlined(newid);
			}

			kill_exp(op12, op12);
			*e = newid;

			if (scan_cond(&next(child(labst)), newid) == 2 && complex(op22)) {
				/*
				 * ... however a further use of identification means that
				 * the second operand of the second test must also be simple
				 */
				clearinlined(newid);
			}

			return 1;
		} else if (c2 && eq_exp(op21, op22)) {
			/* ....if second operands of tests are same, identify them */

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
			*e = newid;

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
			exp tag = getexp(sh(op12), op22, 0, outer_id, pt(outer_id), 0, 0, name_tag);
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

/*
 * Does the scan on commutative and associative operations and may perform
 * various transformations allowed by these properties.
 */
needs
likeplus(exp *e, exp **at)
{
	needs a1;
	needs a2;
	prop pc;
	exp *br = &child(*e);
	exp prev;
	bool commuted = 0;
	exp dad = *(e);

	/* scan the first operand - won't be a val_tag */
	a1 = scan(br, at);

	do {
		exp *prevbr;
		prevbr = br;
		prev = *(br);
		br = &next(prev);
		a2 = scan(br, at);

		/* scan the next operand ... */
		if ((*br)->tag != val_tag) {
			a1.floatneeds = MAX(a1.floatneeds, a2.floatneeds);
			pc = a2.propneeds & hasproccall;
			if (a2.fixneeds < maxfix && pc == 0)
				/* ... its evaluation  will not disturb the accumulated result */
			{
				a1.fixneeds = MAX(a1.fixneeds, a2.fixneeds + 1);
				a1.propneeds = a1.propneeds | a2.propneeds;
			} else if (a1.fixneeds < maxfix && (a1.propneeds & hasproccall) == 0 && !commuted) {

				/*
				 * ...its evaluation will call a proc, so put it first
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
				op1->commuted = 1;
				cop->commuted = 1;
				commuted = 1;

				a1.fixneeds = MAX(a2.fixneeds, a1.fixneeds + 1);
				a1.propneeds |= a2.propneeds;
				a1.maxargs = MAX(a1.maxargs, a2.maxargs);
			} else {
				/*
				 * Evaluation would disturb accumulated result, so replace it by a
				 * newly declared tag.
				 */
				asm_comment("likeplus: insert ident");
				cca(at, br);
				a1.fixneeds = MAX(a1.fixneeds, 2);
				a1.propneeds = a1.propneeds | morefix | (pc << 1);
				a1.maxargs = MAX(a1.maxargs, a2.maxargs);
			}
		} else {
			asm_comment("likeplus: val_tag");
		}
	} while (!(*br)->last);

#if 1
	/* exception handling regs */
	if (!optop(*(e))) {
		if (a1.fixneeds < 4) {
			a1.fixneeds = 4;
		}
	}
#endif

	return a1;
}

/* scan non-commutative fix pt operation */
needs
likediv(exp *e, exp **at)
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
		/* fits into registers */
		l.fixneeds = MAX(l.fixneeds, r.fixneeds + 1);
		l.propneeds = l.propneeds | r.propneeds;
		asm_comment("likediv: r.fixneeds(%d) < maxfix(%d) && pc == 0", r.fixneeds, maxfix);
	} else {
		/* requires new declaration of second operand */
		cca(at, arg);
		l.fixneeds = MAX(l.fixneeds, 1);
		l.propneeds = l.propneeds | morefix | (pc << 1);
		l.maxargs = MAX(l.maxargs, r.maxargs);
		asm_comment("likediv: insert decl r.fixneeds=%d maxfix=%d", r.fixneeds, maxfix);
	}

#if 1
	/* exception handling regs (from mips) */
	if (!optop(*(e))) {
		if (l.fixneeds < 4) {
			l.fixneeds = 4;
		}
	}
#endif

	return l;
}

static needs
fpop(exp *e, exp **at)
{
	/* scans diadic floating point operation  */
	needs l;
	needs r;
	exp op = *e;
	prop pcr, pcl;
	exp *arg = &child(op);

	l = scan(arg, at);
	arg = &next(*arg);
	r = scan(arg, at);
	l.fixneeds = MAX(l.fixneeds, r.fixneeds);
	pcr = r.propneeds & hasproccall;
	pcl = l.propneeds & hasproccall;

	if (!optop(*e)) {
		if (l.fixneeds < 2) {
			l.fixneeds = 2;
		}
	}

	if ((has & HAS_LONG_DOUBLE) && sh(child(op))->tag == doublehd) {
		ClearRev(op);
		arg = &child(op);

		if (!is_o((*arg)->tag) || pcl) {
			cca(at, arg);
		}

		arg = &next(child(op));
		if (!is_o((*arg)->tag) || pcr) {
			cca(at, arg);
		}

		l.floatneeds = MAX(l.floatneeds, r.floatneeds);
		l.maxargs = MAX(l.maxargs, r.maxargs);
		l.propneeds |= hasproccall;

		return l;
	}

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
maxtup(exp e, exp ** at)
{
	exp *stat = &child(e);
	needs an;

	an = zeroneeds;
	while (an = maxneeds(an, scan(stat, at)), !(*stat)->last) {
		stat = &next(*stat);
	}

	return an;
}

/*
 * Finds if usedname is only used in cont operation or as result of ident
 * i.e. value of name is unchanged over its scope
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
				if (!z->last ||
				    (next(z)->tag != seq_tag && next(z)->tag != ident_tag)) {
					return 0;
				}
			}
		}
	}

	return 1;
}

/*
 * The HPPA convention for delivering a struct from a procedure is is have an
 * extra pointer parameter in the proc; this means that there always must be
 * space in the calling work-space for the result struct whether or not the
 * value is used e.g. as in f(x); or f(x).a etc.
 *
 * This proc is part of the mechanism to determine whether it is necessary to
 * insert a dummy declaration to ensure that this space exists.
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
	case labst_tag:
		b = chase(sel, &next(child(*e)));
		break;

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

	case field_tag:
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

	default:
		if ((child(sel) != *e) && (sh(*e)->tag != bothd)) {
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
 * This procedure works out register requirements of an exp. At each
 * call the fix field of the needs is the number of fixpnt registers required
 * to contain live values to evaluate this expression. This never exceeds maxfix
 * because if it would have, a new declaration is introduced in the exp tree
 * (similarly for floating regs and maxfloat). In these cases the prop field
 * will contain the bits morefix (or morefloat).
 *
 * Scan also works out various things concerned with proc calls.
 * The maxargs field contains the max size in bits of the space required for the
 * parameters of all the procedures called in the exp. An exp proc call produces
 * a hasproccall bit in the prop field, if this is transformed as part of the
 * definition of a new declaration the bit is replaced by a usesproccall.
 * The distinction is only used in unfolding nested proc calls; SPARC requires
 * this to be done statically. The condition that a proc exp is a leaf
 * (i.e no proc calls) is that its prop contains neither bit.
 *
 * If an ident exp is suitable, scan marks the props of ident with either inreg
 * or infreg bits to indicate that a t reg may be used for this tag.
 *
 * A thorough understanding of needs along with other procedures
 * that do switch (exp->tag) requires a knowledge of the meaning of the fields
 * of the exp in each case.
 */
needs
scan(exp *e, exp **at)
{
	/*
	 * e is the expression to be scanned, at is the place to put any new decs .
	 * NB order of recursive calls with same at is critical
	 */
	exp ste = *e;
	int nstare = ste->tag;

	nexps++;

	/* ignore diagnostic information */
#if 0
	while (nstare == diag_tag || nstare == cscope_tag || nstare == fscope_tag) {
		e = &child(ste);
		ste = *(e);
		nstare = ste->tag;
	}
#endif

	asm_comment("scan: %d", nstare);

	switch (nstare) {
	case 0:
		UNREACHED;
		return zeroneeds;

	case compound_tag:
		return maxtup(ste, at);

	case nof_tag:
	case concatnof_tag:
	case ncopies_tag: {
		needs nl;
		bool cantdo;
		exp dad;

#if 1
		if (nstare == nof_tag && child(ste) == NULL) {
			return zeroneeds;
		}
#endif

		if (ste->tag == ncopies_tag && child(ste)->tag != name_tag
		    && child(ste)->tag != val_tag) {
			nl = scan(&child(*e), at);
			cca(at, &child(*e));
		} else {
			nl = maxtup(*(e), at);
		}

		ste = *e;
		dad = father(ste);

		if (dad->tag == compound_tag || dad->tag == nof_tag
		    || dad->tag == concatnof_tag) {
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

		/* can only deal with tuples in simple assignment or identity */
		if (cantdo) {
			int prps = (nl.propneeds & hasproccall) << 1;
			cca(at, ptr_position(ste));
			nl = shapeneeds(sh(*(e)));
			nl.propneeds |= morefix;
			nl.propneeds |= prps;
		}

		if (nl.fixneeds < 2) {
			nl.fixneeds = 2;
		}

		return nl;
	}

	case cond_tag: {
#if 1
		if (scan_cond(e, NULL) != 0) {
			return scan(e, at);
		}
		/* else goto next case */
#else

		exp first = child(ste);
		exp labst = next(first);
		exp second = next(child(labst));
		exp t, f, v;

		if (first->tag == seq_tag && second->tag == cond_tag &&
		    child(child(first))->tag == test_tag && pt(child(child(first))) == labst
		    && child(second)->tag == seq_tag
		    && child(child(child(second)))->tag == test_tag)
		{
			/* cond is ( seq (test to L;....),
			* L:cond(seq(test;...),...) ) ..... */
			exp test1 = child(child(first));
			exp test2 = child(child(child(second)));
			bool c1 = complex(child(test1));
			bool c2 = complex(next(child(test1)));

			if (c1 && eq_exp(child(test1), child(test2)))
				/* ....if first operands of tests are same, identify them */
			{
				exp o21 = next(child(test1));
				exp o22 = next(child(test2));
				exp newid = getexp(sh(ste), next(ste), ste->last, child(test1), NULL,
				                   0, 2, ident_tag);
				exp tag1 = getexp(sh(child(test1)), o21, 0, newid, NULL, 0, 0, name_tag);
				exp tag2 = getexp(sh(child(test2)), o22, 0, newid, NULL, 0, 0, name_tag);

				pt(newid) = tag1;
				pt(tag1)  = tag2;	/* uses of newid */
				next(child(test1)) = ste;
				child(test1)->last = false; /* body of newid */
				/* forget child test2 = child test1 */
				next(ste) = newid;
				ste->last = true;		/* father body = newid */
				child(test1) = tag1;
				child(test2) = tag2;	/* relace 1st operands of test */
				*e = newid;

				return scan(e, at);
			} else if (c2 && eq_exp(next(child(test1)), next(child(test2))))
				/* ....if second operands of tests are same, identify them */
			{
				exp o21 = next(next(child(test1)));
				exp o22 = next(next(child(test2)));
				exp newid = getexp(sh(ste), next(ste), ste->last, next(child(test1)),
				                   NULL, 0, 2, ident_tag);
				exp tag1 = getexp(sh(next(child(test1))), o21, 1,
				                  newid, NULL, 0, 0, name_tag);
				exp tag2 = getexp(sh(next(child(test2))), o22, 1, newid, NULL,
				                  0, 0, name_tag);

				pt(newid) = tag1;
				pt(tag1)  = tag2;	/* uses of newid */
				next(next(child(test1))) = ste;
				next(child(test1))->last = false;
				/* body of newid */
				/* forget next child test2 = next child test1 */
				next(ste) = newid;
				ste->last = true;		/* father body = newid */
				next(child(test1)) = tag1;
				next(child(test2)) = tag2;
				/* relace 2st operands of test */
				*e = newid;

				return scan(e, at);
			} else if (child(test2)->tag != name_tag
			           && child(test1)->tag == name_tag &&
			           child(child(test1))->tag == ident_tag && !isvar(child(child(test1)))
			           && !isglob(child(child(test1)))
			           && eq_exp(child(child(child(test1))), child(test2)))
				/* 1st param of test1 is identified with 1st param of  test2 */
			{
				exp tag = getexp(sh(child(test1)), next(child(test2)), 0, child(child(test1)),
				                 pt(child(child(test1))), 0, 0, name_tag);

				pt(child(child(test1))) = tag;
				no(child(child(test1))) += 1;
				/* update usage of ident */
				child(test2) = tag;
				/* and then drop into next case */
				asm_comment("scan() cond_tag: case 3");
			}
		}
		/* else goto next case */
		asm_comment("scan() cond_tag: case 4");
#endif

		FALL_THROUGH;
	}

	case labst_tag:
	case rep_tag:
	case solve_tag: {
		exp *stat;
		exp *statat;
		needs an;

		stat = &child(*e);
		statat = stat;

		an = zeroneeds;
		while (an = maxneeds(an, scan(stat, &statat)), !(*stat)->last) {
			stat = &next(*stat);
			statat = stat;
		}

		if ((an.propneeds & usesproccall) != 0) {
			an.propneeds |= hasproccall;
		}

		return an;
	}

	case ident_tag: {
		needs bdy;
		needs def;
		exp stare = *e;
		exp *arg = &next(child(stare));
		exp t = pt(stare), s;
		shape shdef = sh(child(stare));
		bool fxregble;
		bool flregble;
		bool old_nonevis = nonevis;

		if (pt(stare) == NULL) {
			/* no uses, should have caonly flag and no var flag */
			setcaonly(stare);
			clearvar(stare);
		}

		if (diag != DIAG_NONE && (shdef->tag != bitfhd)) {
			setvis(stare);
		}

		/* if (!iscaonly(stare) || all_variables_visible)*/
		if (isvar(stare) && (!iscaonly(stare) || all_variables_visible)) {
			setvis(stare);
		}

		if (isparam(stare)) {
			exp def = child(stare);
			shape shdef = sh(def);
			long sz = shape_size(shdef);
			if (child(stare)->tag != formal_callee_tag) {
				assert(def->tag == clear_tag);
				/* round up bytes and halfwords to full words */
				if (sz > 0 && sz < 32) {
					sz = 32;
				}
				/* round up small (i.e. < 64 byte) structues to two words */
				else if (sz > 32 && sz < 64) {
					sz = 64;
				}

				/* parameters must be properly aligned */
				stparam = rounder(stparam + sz, sz);

				fixparam = ARG0 + (stparam >> 5) - 1;
				fltparam = 3 * (3 + (stparam >> 5)) + 1;

				if (valregable(shdef) && (shape_size(shdef) == 8 ||
				                          shape_size(shdef) == 16))
					/* Right align bytes and halfwords. */
				{
					no(def) = stparam - 32 + shape_size(shdef);
				} else {
					no(def) = stparam;
				}

				if (is_floating(shdef->tag)) {
					if (fltparam < (3 * 8) + 1) {
						/* floating paramter passed in registers */
						def->props = fltparam;
						maxfloat--;
					} else {
						/* floating parameter passed by stack */
						def->props = 0;
					}
				} else {
					if (fixparam < ARG3 + 1) {
						/* param reg(s) free for the param */
						def->props = fixparam;
						if (shdef->tag != cpdhd && shdef->tag != nofhd) {
							maxfix--;
						}
					} else {
						def->props = 0;    /* Pass by stack */
					}
				}
			} else {
				long alp = shape_align(shdef);
				long n = rounder(callee_sz, alp);
				no(def) = n;
				callee_sz = rounder(n + sz, 32);
			}
		}

		nonevis &= !isvis(stare);

		/* scan the body of the identity */
		bdy = scan(arg, &arg);

		/* scan the body-scope */
		arg = &child(stare);
		def = scan(arg, &arg);
		/* scan the initialisation of tag */

		nonevis = old_nonevis;
		t = child(stare);
		s = next(t);
		fxregble = fixregable(stare);
		flregble = floatregable(stare);

		if (isparam(stare)) {
			if (child(stare)->tag == formal_callee_tag) {
				/* IDENT is a callee parameter. */
				no(stare) = R_NO_REG;
			} else {
				bool is_aggregate = (shdef->tag == cpdhd || shdef->tag == nofhd ||
				                     shdef->tag == s64hd || shdef->tag == u64hd);
				if (!is_aggregate && !isvis(stare) &&
				    !isoutpar(stare) &&
				    ((bdy.propneeds & anyproccall) == 0 ||
				     (!has_tail_call &&
				      tempdec(stare, (fxregble && bdy.fixneeds <= 2) ||
				              (flregble && bdy.floatneeds <= 1))))) {
					int x = child(stare)->props; /* param reg it comes in, or else 0 */

					if (x != 0 && (stare->props & notparreg) == 0) {
						/*
						 * use input param for leaf proc, unless it is R_RESULT=ARG0
						 * and we plan to use that specially
						 */
						no(stare) = x;
						if (flregble) {
							/* stare->props |= infreg_bits; */
							no(stare) = 0;
						} else {
							stare->props |= inreg_bits;
						}
					} else if (fxregble && bdy.fixneeds < maxfix && (bdy.propneeds & morefix) == 0) {
						no(stare) = 0;
						stare->props |= inreg_bits;
						bdy.fixneeds += 1;
					} else if (flregble && bdy.floatneeds < maxfloat && 0 && (bdy.propneeds & morefloat) == 0) {
						no(stare) = 0;
						stare->props |= infreg_bits;
						bdy.floatneeds += 1;
					} else {
						no(stare) = R_NO_REG;
					}
				} else {
					no(stare) = R_NO_REG;
				}
			}
		} else {
			/* NON PARAMETER */
			assert(!isparam(*e)); /* handled above */
			if (!isvis(*e) && !isparam(*e) &&
			    (bdy.propneeds & (anyproccall | uses_res_reg_bit)) == 0
			    && (fxregble /*|| flregble*/) &&
			    (t->tag == apply_tag ||
			     (s->tag == seq_tag && next(child(s))->tag == res_tag &&
			      child(next(child(s)))->tag == cont_tag && isvar(stare) &&
			      child(child(next(child(s))))->tag == name_tag &&
			      child(child(child(next(child(s))))) == stare)))
			{
				/* Let a := ..; return cont a */
				/* +++ integrate this with the block above, otherwise UNREACHED */
				/*
				 * Put tag in result reg if definition is call of proc,
				 * or body ends with return tag, provided result is not used other wise.
				 */
				asm_comment("scan: ident_tag(%p): use result reg", (void *) stare);
				stare->props |= (fxregble) ? inreg_bits : infreg_bits;

				if (fxregble) {
					stare->props |= inreg_bits;
					bdy.fixneeds++;
				} else {
					stare->props |= infreg_bits;
				}

				bdy.propneeds |= uses_res_reg_bit;
				no(stare) = R_USE_RES_REG;	/* identification  uses result reg in body */
			} else if (isenvoff(stare)) {
				/* MUST go on stack */
				no(stare) = R_NO_REG;
			} else if (!isvar(*e) && !isparam(*e) &&
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
			             && unchanged(child(child(t)), stare))))
			             /*
			              * cont variable - not assigned to in scope
			              */
			{

				asm_comment("scan: ident_tag(%p): dont take space for this dec", (void *) stare);
				stare->props |= defer_bit;
				/* dont take space for this dec */
			} else if (!isvar(stare) &&
			           ((stare->props & 0x10 /* forced in const */ ) == 0)
			           && (t->tag == name_tag || t->tag == val_tag)) {
				asm_comment("scan: ident_tag(%p): dont take space for this dec (#2)", (void *) stare);
				stare->props |= defer_bit;
				/* dont take space for this dec */
			} else if (fxregble && bdy.fixneeds < maxfix &&
			           (bdy.propneeds & morefix) == 0 &&
			           ((bdy.propneeds & anyproccall) == 0
			            || tempdec(stare, ((bdy.propneeds & morefix) == 0 &&
			                               bdy.fixneeds < maxfix_tregs - 2)))) {
				/*
				 * put this tag in some  fixpt t-reg - which will be decided in
				 * make_code
				 */

				asm_comment("scan: ident_tag(%p): use fixpt t-reg", (void *) stare);
				asm_comment("	bdy.fixneeds=%d def.fixneeds=%d", bdy.fixneeds, def.fixneeds);

				stare->props |= inreg_bits;
				no(stare) = 0;
				bdy.fixneeds += 1;
			} else if (bdy.floatneeds < maxfloat && (bdy.propneeds & morefloat) == 0
			           && flregble &&
			           ((bdy.propneeds & anyproccall) == 0
			            || tempdec(stare, ((bdy.propneeds & morefloat) == 0 &&
			                               bdy.floatneeds < MAXFLOAT_TREGS - 1)))) {

				/*
				 * put this tag in some  float t-reg - which will be decided in
				 * make_code
				 */
				asm_comment("scan: ident_tag(%p): use float t-reg", (void *) stare);
				stare->props |= infreg_bits;
				no(stare) = 0;
				bdy.floatneeds += 1;
			} else {
				asm_comment("scan: ident_tag(%p): use stack or saved reg", (void *) stare);
				no(stare) = R_NO_REG;

				/*
				 * allocate either on stack or saved reg
				 */
			}
		}

		bdy = maxneeds(bdy, def);
		if ((bdy.propneeds & usesproccall) != 0) {
			bdy.propneeds |= hasproccall;
		}

		return bdy;
	}

	/*
	 * shape of exp is shape of end of sequence
	 * child is sequence holder, child of this is list of voided statements.
	 */
	case seq_tag: {
		exp *arg = &next(child(*e));
		needs an;
		exp *stat;

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

	case trap_tag:
	case goto_tag:
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

		if (nstare != ass_tag || a.ashsize != a.ashalign || a.ashalign == 1) {
			/* struct/union assign */
			if (!(a.ashsize <= 32 && a.ashsize == a.ashalign)) {
				nr.fixneeds += 2;    /* memory block copy */
			}
		}

		if ((*lhs)->tag == name_tag &&
		    (isvar(child(*(lhs))) &&
		     ((nr.propneeds & (hasproccall | morefix)) == 0
		      && nr.fixneeds < maxfix)))
		{
			/* simple destination */
			return nr;
		} else {
			needs nl;
			prop prps = (nr.propneeds & hasproccall) << 1;

			nl = scan(lhs, at);

			if ((*rhs)->tag == apply_tag && nstare == ass_tag &&
			    (nl.propneeds & (uses_res_reg_bit | anyproccall)) == 0) {
				/* source is proc call, so assign result reg directly */
				/* SKIP */ ;
			} else if (nr.fixneeds >= maxfix || prps != 0) {
				/* source and destination regs overlap, so identify source */
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
		ash a;
		needs x;
		shape s;
		exp *arg = &child(*e);
		/* exp r,ss,t; */
		s = sh(*(arg));
		a = ashof(s);
		(*e)->props = 0;		/* clear possibility of tlrecirsion; may be set later */
		x = scan(arg, at);

		/* scan result exp ... */
		if (is_floating(s->tag) && a.ashsize <= 64) { /* ... floating pt result */
			x.propneeds |= realresult_bit;
			if (s->tag != shrealhd) {
				x.propneeds |= longrealresult_bit;
			}
			asm_comment("scan res_tag: long real/real result");
		} else {
			if (!valregable(s) && !(child(*e)->tag == top_tag)) { /* .... result does not fit into reg */
				x.propneeds |= long_result_bit;
				asm_comment("scan res_tag: struct/union result");
			}
		}

		asm_comment("scan res_tag: result size %ld", a.ashsize);
		if (a.ashsize != 0 && (*arg)->tag != clear_tag) { /* not a void result */
			x.propneeds |= has_result_bit;
		}

		return x;
	}

	/* spec 3.1 procedure stuff */

	case apply_general_tag: {
		exp application = *(e);
		exp *fn = &child(application);
		exp cers = next(*fn);
		exp *cerl = &child(cers);
		long stpar = 0;

		needs nds;
		needs plnds;
		int i;

		gen_call = true;

		nds = scan(fn, at);
		if ((nds.propneeds & hasproccall) != 0) {
			/* .... it must be identified */
			cca(at, fn);
			nds.propneeds &= ~hasproccall;
			nds.propneeds |= usesproccall;
			fn = &child(application);
		}

		for (i = 0; i < no(cers); i++) {
			needs onepar;
			shape shonepar = sh(*cerl);
			exp * par = (*cerl)->tag == caller_tag ? &child(*cerl) : cerl;
			int n = rounder(stpar, shape_align(shonepar));
			onepar = scan(par, at);

			if ((i != 0 && (onepar.propneeds & hasproccall) != 0) ||
			    onepar.fixneeds + (stpar >> 5) > maxfix) {
				/* if it isn't the first parameter, and it
				   calls a proc, identify it */
				cca(at, par);
				nds.propneeds |= usesproccall;
				nds = maxneeds(shapeneeds(sh(*(par))), nds);
				nds.maxargs = MAX(nds.maxargs, onepar.maxargs);
			} else {
				nds = maxneeds(onepar, nds);
			}

			if ((*cerl)->tag == caller_tag) {
				no(*cerl) = n;
			}

			n = n + shape_size(shonepar);
			stpar = rounder(n, 32);
			cerl = &next(*cerl);
		}

		nds.maxargs = MAX(nds.maxargs, stpar);
		nds = maxneeds(scan(&next(next(child(application))), at), nds);

		plnds = scan(&next(next(next(child(application)))), at);

		if ((plnds.propneeds & (anyproccall | uses2_bit)) != 0) {
			application->props = 1;
			if (is_floating(sh(application)->tag) || valregable(sh(application))) {
				cca(at, ptr_position(application));
				plnds.propneeds |= usesproccall;
			}
		} else {
			application->props = 0;
		}

		nds = maxneeds(nds, plnds);
		nds.propneeds |= hasproccall;
		return nds;
	}

	case make_callee_list_tag: {
		exp cees = *e;
		exp * par = &child(cees);
		needs nds;
		long stpar = 0;
		int i;
		nds = zeroneeds;

		for (i = 0; i < no(cees); i++) {
			needs onepar;
			shape shonepar = sh(*par);
			int n = rounder(stpar, shape_align(shonepar));
			onepar = scan(par, at);

			if (((onepar.propneeds & hasproccall) != 0) ||
			    onepar.fixneeds + 1 > maxfix) {
				/* if it calls a proc, identify it */
				cca(at, par);
				nds.propneeds |= usesproccall;
				nds = maxneeds(shapeneeds(sh(*(par))), nds);
				nds.maxargs = MAX(nds.maxargs, onepar.maxargs);
			} else {
				nds = maxneeds(onepar, nds);
			}

			n = n + shape_size(shonepar);
			stpar = rounder(n, 32);
			par = &next(*par);
		}

		no(cees) = stpar;
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

	case same_callees_tag: {
		needs nds;

		nds = zeroneeds;
		nds.fixneeds = 4;

		return nds;
	}

	case tail_call_tag: {
		needs ndsp;
		needs nds;

		exp *fn = &child(*e);
		ndsp =  scan(fn, at);
		if (((ndsp.propneeds & hasproccall) != 0) ||
		    ndsp.fixneeds + 1 > maxfix) {
			cca(at, fn);
			nds.propneeds |= usesproccall;
			nds = maxneeds(shapeneeds(sh(*(fn))), nds);
			nds.maxargs =  MAX(nds.maxargs, ndsp.maxargs);
		} else {
			nds = ndsp;
		}

		gen_call      = true;
		has_tail_call = true;

		ndsp = scan(&next(child(*e)), at);
		nds = maxneeds(nds, ndsp);

		return nds;
	}

	case env_size_tag: {
		exp *arg = &child(*e);
		return scan(arg, at);
	}

	case apply_tag: {
		exp application = *(e);
		exp fn = child(application);
		exp *par = &next(fn);
		exp *fnexp = &child(*e);
		int parsize = 0;
		needs nds;
		bool tlrecpos = nonevis && callerfortr;
		int i;
		bool notinreg = !((has & HAS_LONG_DOUBLE) ? (sh(application)->tag == shrealhd ||
		                        sh(application)->tag == realhd)
		                : is_floating(sh(application)->tag)
		                || valregable(sh(application)));

		bool long_result_space_needed = notinreg && !(sh(*e)->tag == tophd);

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

		if (fn->tag != name_tag ||
		    (child(child(fn)) != NULL && child(child(fn))->tag != proc_tag)) {
			tlrecpos = 0;
		}

		for (i = 1; !fn->last; ++i) {
			/* scan parameters in turn ... */
			needs onepar;
			shape shpar = sh(*par);

			onepar = scan(par, at);

			if ((i != 1 && (onepar.propneeds & hasproccall) != 0) ||
			    onepar.fixneeds + (parsize >> 5) > maxfix) {

				/*
				 * if it isn't the first parameter, and it calls a proc, identify it
				 */
				asm_comment("apply_tag: identifying parameter %d (1..) containing proc call", i);
				cca(at, par);
				nds.propneeds |= usesproccall;
				nds = maxneeds(shapeneeds(sh(*(par))), nds);
				nds.maxargs = MAX(nds.maxargs, onepar.maxargs);
			} else {
				nds = maxneeds(onepar, nds);
			}

			parsize = rounder(parsize + shape_size(shpar), 32);
			/* round up bytes and halfwords */

			parsize = rounder(parsize, shape_align(shpar));
			/* parameters must be properly aligned on the stack */

			if ((!valregable(shpar) && !is_floating(shpar->tag)) || parsize > 128) {
				tlrecpos = 0;
			}

			if ((*par)->last) {
				break;
			}

			par = &next(*par);
		}

		if (specialopt(fn)) {
			nds.propneeds |= dont_optimise;    /* eg vfork */
		}

		if ((i = specialfn(fn)) > 0) {
			/* eg strlen */
			nds = maxneeds(specialneeds(i), nds);
			assert("specialfn - specialneeds");
			return nds;
		}

		if (tlrecpos) {
			exp dad = father(application);

			if (dad->tag == res_tag) {
				dad->props = 1;	/* do a tl recursion */
			}
		}

		if (long_result_space_needed) {
			/* find space for tuple result */
			asm_comment("apply_tag: identifying notinreg result");
			assert((*ptr_position(application))->tag == apply_tag);
			cca(at, ptr_position(application));
			nds.propneeds |= usesproccall;
		} else {
			nds.propneeds |= hasproccall;
		}

		nds.maxargs  = MAX(nds.maxargs, parsize);
		nds.fixneeds = MAX(nds.fixneeds, 7); /* clobber %o0..%o5,%07 */

		return nds;
	}

	case val_tag: {
		exp s = sh(*e);

		if (s->tag == offsethd && al2(s) >= 8) {
			/* express disps in bytes */
			no(*e) = no(*e) >> 3;
		}

		asm_comment("val_tag %d no=%ld", s->tag, no(*e));
		/* ... and continue */
	}

	case name_tag:
	case null_tag:
	case real_tag:
	case string_tag:
	case env_offset_tag:
	case general_env_offset_tag:
	case current_env_tag:
	case make_lv_tag:
	case last_local_tag:
	case caller_name_tag:
	case give_stack_limit_tag:
		/*  case next_frame_tag:  */
		if (nstare == last_local_tag) {
			cpr->Has_ll = 1;
		}

		return shapeneeds(sh(*(e)));

	case clear_tag:
	case top_tag:
	case local_free_all_tag:
	case prof_tag:
	case formal_callee_tag:
		return zeroneeds;

	case set_stack_limit_tag:
	case abs_tag:
	case neg_tag:
	case case_tag:
	case not_tag:
	case goto_lv_tag: /* for COBOL */
	case offset_negate_tag:
	case diagnose_tag:
	case local_free_tag:
#if 1
	case return_to_label_tag:
#endif
	{
		exp *arg = &child(*e);
		return scan(arg, at);
	}

	case fneg_tag:
	case fabs_tag:
	case chfl_tag: {
		needs nds;
		exp *pste;
		nds = scan(&child(*e), at);
		pste = ptr_position(ste);

		if (!optop(*pste) && nds.fixneeds < 2) {
			nds.fixneeds = 2;
		}

		if ((has & HAS_LONG_DOUBLE)) {
			exp op = *pste;
			if (sh(op)->tag == doublehd ||
			    sh(child(op))->tag == doublehd) {
				if (!is_o(child(op)->tag) || (nds.propneeds & hasproccall)) {
					cca(at, &child(op));
				}
				nds.propneeds |= hasproccall;
			}
		}

		return nds;
	}

	case bitf_to_int_tag: {
		exp *arg = &child(*e);
		needs nds;
		exp stararg;
		exp stare;
		int sizeb;

		nds = scan(arg, at);
		stararg = *(arg);
		stare = *(e);
		sizeb = ashof(sh(stararg)).ashsize;

		if ((stararg->tag == name_tag &&
		     ((sizeb == 8 && (no(stararg) & 7) == 0)
		      || (sizeb == 16 && (no(stararg) & 15) == 0)
		      || (sizeb == 32 && (no(stararg) & 31) == 0))
		    ) || (stararg->tag == cont_tag &&
		          ((child(stararg)->tag != name_tag && child(stararg)->tag != reff_tag)
		           || (sizeb == 8 && (no(child(stararg)) & 7) == 0)
		           || (sizeb == 16 && (no(child(stararg)) & 15) == 0)
		           || (sizeb == 32 && (no(child(stararg)) & 31) == 0))))
			/*
			 * these bitsint(trimnof(X)) could be implemented by lb or lh
			 * instructions ...
			 */
		{
			int sgned = is_signed(sh(stare));
			shape ns = (sizeb == 8) ? ((sgned) ? scharsh : ucharsh)
			           : (sizeb == 16) ? ((sgned) ? swordsh : uwordsh)
			           : sh(stare);

			/* can use short loads instead of bits extractions */
			if (stararg->tag == cont_tag) {
				/* make the ptr shape consistent */
				sh(child(stararg)) = f_pointer(long_to_al(shape_align(ns)));
			}

			sh(stararg) = ns;
			stare->tag = chvar_tag;
		}

		return nds;
	}

	case int_to_bitf_tag: {
		exp *arg = &child(*e);
		return scan(arg, at);
	}

	case round_tag: {
		needs s;
		exp *arg;
		exp *pste;
		int rm = (int)round_number(*e);
		arg = &child(*e);
		s = scan(arg, at);
		pste = ptr_position(ste);
		s.fixneeds = MAX(s.fixneeds, 2);

		if (rm < 3 || sh(*pste)->tag == ulonghd) {
			s.floatneeds = MAX(s.floatneeds, 3);
		} else {
			s.floatneeds = MAX(s.floatneeds, 2);
		}

		if ((has & HAS_LONG_DOUBLE)) {
			exp op = *pste;

			if (sh(child(op))->tag == doublehd) {
				if (!is_o(child(op)->tag) || (s.propneeds & hasproccall)) {
					cca(at, &child(op));
				}

				s.propneeds |= hasproccall;
			}
		}

		return s;
	}

	case shl_tag:
	case shr_tag:
	case long_jump_tag: {
		exp *lhs = &child(*e);
		exp *rhs = &next(*lhs);
		needs nl, nr;
		prop prps;

		if ((*rhs)->tag == val_tag) {
			return scan(lhs, at);
		}

		nr = scan(rhs, at);
		nl = scan(lhs, at);
		rhs = &next(*lhs);
		prps = (nr.propneeds & hasproccall) << 1;

		/* if reg requirements overlap, identify second operand */
		if (nr.fixneeds >= maxfix || prps != 0) {
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
		exp stare = *e;
		exp l = child(stare);
		exp r = next(l);

		/* same test following in seq list - remove second test */
		if (!stare->last && next(stare)->tag == test_tag &&
		    no(stare) == no(next(stare)) &&
		    test_number(stare) == test_number(next(stare)) &&
		    eq_exp(l, child(next(stare))) && eq_exp(r, next(child(next(stare)))))
		{
			if (next(stare)->last) {
				stare->last = true;
			}

			next(stare) = next(next(stare));
		}

		/* same test following in seq res - void second test */
		if (stare->last && next(stare)->tag == 0	/* seq holder */
		    && next(next(stare))->tag == test_tag &&
		    next(next(next(stare)))->tag == seq_tag &&
		    no(stare) == no(next(next(stare))) &&
		    test_number(stare) == test_number(next(next(stare))) &&
		    eq_exp(l, child(next(next(stare))))
		    && eq_exp(r, next(child(next(next(stare))))))
		{
			next(next(stare))->tag = top_tag;
			child(next(next(stare))) = NULL;
			pt(next(next(stare))) = NULL;
		}

		/* commute  const = x */
		if (l->tag == val_tag && (stare->props == 5 || stare->props == 6)) {
			next(l) = stare;
			l->last = true;
			next(r) = l;
			r->last = false;
			child(stare) = r;

			r = l;
			l = child(stare);
		}

		/* equality of bits against +ve consts doesnt need sign adjustment */
		if (l->tag == bitf_to_int_tag && r->tag == val_tag &&
		    (stare->props == 5 || stare->props == 6) &&
		    (child(l)->tag == cont_tag || child(l)->tag == name_tag))
		{
			long n = no(r);

			switch (sh(l)->tag) {
			case scharhd:
				if (n >= 0 && n <= 127) {
					sh(l) = ucharsh;
				}

				break;

			case swordhd:
				if (n >= 0 && n <= 0xffff) {
					sh(l) = uwordsh;
				}

				break;

			default:
				;
			}
		} else if (is_floating(sh(l)->tag)) {
			return fpop(e, at);
		} else if (r->tag == val_tag && no(r) == 1
		           && (stare->props == 3 || stare->props == 2)) {
			no(r) = 0;

			if (stare->props == 3) {
				stare->props = 4;	/* branch >=1 -> branch > 0 */
			} else {
				stare->props = 1;	/* branch <1 -> branch <= 0 */
			}
		}

		return likediv(e, at);
	}

	case plus_tag: {
		/* replace any operands which are neg(..) by -, if poss */
		exp sum = *e;
		exp list = child(sum);
		bool someneg = 0;
		bool allneg = 1;

		for ( ; optop(sum);) {
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

		asm_comment("scan case plus_tag,addptr_tag: allneg=%d someneg=%d", allneg, someneg);

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

					next(list) = child(next(x));
					if (!x->last) {
						list->last = false;
						list = next(list);
						x = next(x);
					} else {
						list->last = true;
						next(list) = sum;		/* set father to be */
						child(sum)  = child(child(sum)); /* set new children of plus_tag */
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

				*e = x;
			}			/* end allneg */
			else {
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

			return scan(e, at);
		}
		/* end someneg - else continue to next case */

		FALL_THROUGH;
	}

	case and_tag:
	case or_tag:
	case xor_tag:
		return likeplus(e, at);

	case addptr_tag: {
		exp p = child(*e);
		exp d = next(p);
		int fal = frame_al_of_ptr(sh(p));

		if (fal != 0) {
			int oal = frame_al1_of_offset(sh(d));

#if 0
			if (((oal - 1) &oal) != 0) {
				error(ERR_INTERNAL, "can't cope with mixed frame offsets yet");
			}
#endif

			if (includes_vcallees(fal) && ((oal & 20) != 0)) {
				/* oal = callees and callers are offset relative to
				FP when there are variable callees */
				exp ne = getexp(sh(p), d, 0, p, NULL, 0, 0, locptr_tag);
				next(p) = ne;
				p->last = true;
				child(*e) = ne;
			}
		}

		/* ... and continue */
	}

	case minus_tag:
	case subptr_tag:
	case minptr_tag:
		return likediv(e, at);

	case reff_tag:
	case float_tag:
	case offset_pad_tag:
	case locptr_tag:
	case chvar_tag: {
		exp *arg = &child(*e);
		exp *pste;

		needs nds;
		nds = shapeneeds(sh(*e));
		nds = maxneeds(scan(arg, at), nds);
		pste = ptr_position(ste);

		if ((has & HAS_LONG_DOUBLE)) {
			exp op = *pste;
			if (sh(op)->tag == doublehd) {
				nds.propneeds |= hasproccall;
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

mult_tag_case:

	case mult_tag:
		return multneeds(e, at);

	case offset_mult_tag:
	case offset_div_tag: {
		exp op2 = next(child(*e));
		shape s = sh(op2);

		if (op2->tag == val_tag && no(op2) == 8 &&
		    s->tag == offsethd && al2(s) >= 8) {
			/* offset is one byte */
			exp op1 = child(*e);
			next(op1) = next(*e);
			if ((*e)->last) {
				op1->last = true;
			} else {
				op1->last = false;
			}
			*e = op1;
			return scan(e, at);
		}

		if (nstare == offset_mult_tag) {
			goto mult_tag_case;
		}

		FALL_THROUGH;
	}

#if 0
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
			} else if (*e->tag == offset_mult_tag && n > 1 && (n & (n - 1)) == 0)
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
							/* can do transform:
							   (((shop1>>ns) & na) * n) =>
							   shop1>>(ns-pn) & (na*n)
							     */
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

		if (nstare == offset_mult_tag) {
			goto mult_tag_case;
		}
	}
#endif

	case div0_tag:
	case div2_tag:
	case offset_div_by_int_tag:
		return divneeds(e, at);

	case offset_add_tag:
		if ((al2(sh(child(*e))) == 1) && (al2(sh(next(child(*e)))) != 1)) {
			make_bitfield_offset(next(child(*e)), child(*e), 0, sh(*e));
		}

		if ((al2(sh(child(*e))) != 1) && (al2(sh(next(child(*e)))) == 1)) {
			make_bitfield_offset(child(*e), *e, 1, sh(*e));
		}

	case offset_subtract_tag:
	case component_tag:
		return likediv(e, at);

	case make_stack_limit_tag: {
		needs nd;

		nd = likediv(e, at);
		nd.fixneeds = MAX(nd.fixneeds, 2);

		return nd;
	}

	case offset_max_tag:
	case max_tag:
	case min_tag: {
		needs nd;

		nd = likediv(e, at);
		nd.fixneeds = MAX(nd.fixneeds, 3);

		return nd;
	}

	case rem0_tag:
	case rem2_tag:
		return remneeds(e, at);

	case div1_tag:
		if (is_signed(sh(*e)) == 0) {
			(*e)->tag = div2_tag;
		}

		return divneeds(e, at);

	case mod_tag:
		if (is_signed(sh(*e)) == 0) {
			(*e)->tag = rem2_tag;
		}

		return remneeds(e, at);

	case fdiv_tag:
#if 0
	{
		exp z = *e;
		exp a2 = next(child(z));

		if (a2->tag == real_tag) {	/* replace X/const by X*const^-1 */
			flt inverse;
			flt unitflt;

			unitflt = flptnos[fone_no];
			if (flt_div(unitflt, flptnos[no(a2)], &inverse) == OKAY) {
				/* a/const => a* (1.0/const) */
				int f = new_flpt();

				flptnos[f] = inverse;
				no(a2) = f;
				z->tag = fmult_tag;
			}
		}

		FALL_THROUGH;
	}
#endif

	case fplus_tag:
	case fminus_tag:
	case fmult_tag: {
		exp op = *e;
		exp a2 = next(child(op));

		/*
		 * + and * can have >2 parameters - make them diadic - can do better
		 * a+exp => let x = exp in a+x
		 */
		if (!a2->last) {
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

		/* field has been distributed */
		if (chase(*e, arg)) {
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
		exp stare = *(e);

		/* set number temp t-regs that can be used in proc */
		maxfix = maxfix_tregs;
		maxfloat = MAXFLOAT_TREGS;

		assert(~optim & OPTIM_TAIL);

		callerfortr = optim & OPTIM_TAIL && !proc_has_setjmp(stare) &&
		              !proc_has_alloca(stare) && !proc_has_lv(stare) &&
		              !proc_uses_crt_env(stare);

		gen_call      = (stare->tag == general_proc_tag);
		has_tail_call = false;

		callee_sz = 0;
		stparam = 0;
		fixparam = ARG0;
		nonevis = true;

		bexp = &child(*e);
		bat = bexp;
		body = scan(bexp, &bat);
		/* scan the body of the proc */

		/* should never require this in reg in C */
		return body;
	}

	case absbool_tag:
		assert(~has & HAS_SETCC);
		error(ERR_SERIOUS, "absbool_tag not covered in needs scan");
		UNREACHED;

	case alloca_tag: {
		needs nds;

		cpr->Has_checkalloc = checkalloc(*e);
		nds = scan(&child(*e), at);
		if (nds.fixneeds < 2) {
			nds.fixneeds = 2;
		}

		hppabuiltin |= (1 << 5);
		return nds;
	}

	case movecont_tag: {
		exp *d  = &child(*e);
		exp *s  = &next(*d);
		exp *sz = &next(*s);

		needs nd;
		needs ns;
		needs nsz;
		prop prps;
		nd = scan(d, at);
		ns = scan(s, at);
		nsz = scan(sz, at);
		prps = (ns.propneeds & hasproccall) << 1;

		if (ns.fixneeds >= maxfix || prps != 0) {
			/* if reg requirements overlap, identify second operand */
			cca(at, d);
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
			cca(at, d);
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

	case testbit_tag:
		/* not in MIPS */
		error(ERR_SERIOUS, "testbit_tag not covered in needs scan");
		UNREACHED;

	/*Know to NOT be covered:
	cscope_tag		(ignored)
	 diag_tag		(ignored)
	 fscope_tag		(ignored)
	 local_free_all_tag
	 local_free_tag
	 offset_pad_tag
	 testbit_tag
	 */

	default: {
		char s[64];

		asm_comment("scan: bad nstare=%d", nstare);
		sprintf(s, "case %d not covered in needs scan", nstare);
		error(ERR_SERIOUS, s);

		return zeroneeds;
	}
	}
}

/* commented out return_to_label_tag and make_stack_limit_tag for puposes
 * of debugging until proper 4.0 libraries are built. */

