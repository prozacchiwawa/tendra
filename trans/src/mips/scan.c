/*
 * Copyright 2011-2012, The TenDRA Project.
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
#include <string.h>

#include <shared/bool.h>
#include <shared/check.h>

#include <local/ash.h>
#include <local/exp.h>
#include <local/tag.h>

#include <tdf/shape.h>
#include <tdf/tag.h>

#include <reader/exp.h>
#include <reader/externs.h>
#include <reader/basicread.h>
#include <reader/externs.h>

#include <construct/installtypes.h>
#include <construct/dec.h>
#include <construct/ash.h>
#include <construct/exp.h>
#include <construct/install_fns.h>
#include <construct/me_fns.h>

#include <flpt/flpt.h>

#include <utility/max.h>

#include <main/flags.h>

#include <refactor/const.h>
#include <refactor/optimise.h>
#include <refactor/refactor.h>
#include <refactor/query.h>

#include "procrec.h"
#include "bits.h"
#include "regable.h"
#include "tempdecs.h"
#include "special.h"
#include "frames.h"
#include "main.h"
#include "scan.h"

static void
tidy_ident(exp e);

static int maxfix, maxfloat; /* the maximum number of t-regs */
static int stparam, fixparam, floatparam;
/* used by scan to set initial parameter positions */

extern long notbranch[6];

static bool nonevis     = true;
static int callerfortr;

static bool gen_call;

/*
 * declaration of scan.
 *
 * needs is defined in procrectypes.h.
 *
 * This is a structure which has two integers giving the number of fixed
 * and floating point registers required to contain live values in the
 * expression parameters. A further field prop is used for various flags
 * about certain forms of exp (mainly idents and procs). The maxargs field
 * gives the maximum size in bits for the parameters of all the procs
 * called in the exp. The needs of a proc body are preserved in the
 * needs field of the procrec (see procrectypes.h).
 */

/*
 * This procedure effectively inserts a new declaration into an exp.
 * This is used to stop a procedure requiring more than the available
 * number of registers.
 */
static void
cca(exp **to, exp *x)
{
	if (x == (*to)) {
		exp def = *x;
		/* replace by  Let tag = def In tag Ni */
		exp id = getexp (sh (def), next(def), def->last, def, NULL, 0, 1, ident_tag);
		exp tag = getexp (sh (def), id, 1, id, NULL, 0, 0, name_tag);
		pt (id) = tag;		/* use of tag */
		next(def) = tag;		/* next(def) is body of Let = tag */
		def->last = false;
		*x = id;		/* replace pointer to x by Let */
	} else {
		/* replace by Let tag = def In ato/def = tag Ni */
		exp def = *x;
		exp ato = **to;
		exp id  = getexp (sh (ato), next(ato), ato->last, def, NULL, 0, 1, ident_tag);
		exp tag = getexp (sh (def), next(def), def->last, id, NULL, 0, 0, name_tag);
		pt (id) = tag;		/* use of tag */
		next(def) = ato;		/* ato is body of Let */
		def->last = false;
		next(ato) = id;		/* its father is Let */
		ato->last = true;
		**to = id;		/* replace pointer to 'to' by Let */
		*x = tag;		/* replace use of x by tag */

		if (def->tag == ident_tag) {
			tidy_ident(id);
			*to = ptr_position(ato);
			no(id) = 100; /*it has already been scanned */
		} else {
			*to = & next(child(id));
		}		/* later replacement to same 'to' will be
				   at body of Let */
	}
}

static needs onefix    = { 1, 0, 0, 0 }; /* needs one fix pt reg */
static needs twofix    = { 2, 0, 0, 0 }; /* needs 2 fix pt regs */
static needs threefix  = { 3, 0, 0, 0 }; /* needs 3 fix pt regs */
static needs fourfix   = { 4, 0, 0, 0 }; /* needs 4 fix pt regs */
static needs onefloat  = { 0, 1, 0, 0 }; /* needs 1 flt pt regs */
static needs zeroneeds = { 0, 0, 0, 0 }; /* has no needs */

static needs
shapeneeds(shape s)
{
	/* this gives the needs for manipulating a value of shape s */
	if (is_floating(s->tag)) {
		return onefloat;
	} else {
		if (valregable (s)) {
			return onefix;
		} else {
			/*
			 * If the shape does not fit into a reg,
			 * it may need up to four fixed regs for moving
			 */
			int al = shape_align(s);

			if (al == 1) {
				return threefix;
			}

			if (al == 8) {
				if (shape_size(s) < 16 ) {
					return twofix;
				}
				return fourfix;
			} else {
				int unitmv = MIN(al, 32);
				if (shape_size(s) / unitmv < inlineassign) {
					return twofix;
				} else {
					return fourfix;
				}
			}
		}
	}
}

/*
 * These are basically the expressions which cannot be accessed
 * by a simple load or store instruction
 */
static bool
complex(exp e)
{
	if (e->tag == name_tag) {
		return 0;
	}

	if (e->tag == cont_tag && child(e)->tag == name_tag && isvar (child(child(e)))) {
		return 0;
	}

	if (e->tag == val_tag || e->tag == real_tag || e->tag == null_tag) {
		return 0;
	}

	return 1;
}

static int
scan_cond(exp *e, exp outer_id)
{
	exp ste = *e;
	exp first  = child(ste);
	exp labst  = next(first);
	exp second = next(child(labst));

	assert(ste->tag == cond_tag);

	if (second->tag == top_tag && sh(first)->tag == bothd && no(child(labst)) == 1
	    && first->tag == seq_tag && next(child(first))->tag == goto_tag) {
		/* cond is { ... test(L); ? ; goto X | L:make_top}
			if ? empty can replace by seq { ... not-test(X); make_top }
		*/

		exp l = child(child(first));
		while(!l->last) {
			l = next(l);
		}

		while(l->tag == seq_tag) {
			l = next(child(l));
		}

		if (l->tag == test_tag && pt(l) == labst) {
			settest_number(l, notbranch[test_number(l) - 1]);
			pt(l) = pt(next(child(first)));
			next(child(first)) = second;
			next(second) = first;
			second->last = true;
			next(first) = next(ste);

			if(ste->last) {
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
		/* cond is (seq(test to L;....|
		   L:cond(seq(test;...),...) ) ..... */
		exp test1 = child(child(first));
		exp test2 = child(child(child(second)));
		exp op11 = child(test1);
		exp op21 = next(op11);
		exp op12 = child(test2);
		exp op22 = next(op12);
		bool c1 = complex(op11);
		bool c2 = complex(op21);

		if (c1 && eq_exp (op11, op12)) {
			/* ....if first operands of tests are same, identify them */
			exp newid = getexp (sh (ste), next(ste), ste->last, op11, NULL,
			                    0, 2, ident_tag);
			exp tag1 = getexp (sh (op11), op21, 0, newid, NULL, 0, 0, name_tag);
			exp tag2 = getexp (sh (op12), op22, 0, newid, NULL, 0, 0, name_tag);

			pt(newid) = tag1;
			pt(tag1)  = tag2;	/* uses of newid */
			next(op11) = ste;
			op11->last = false;/* body of newid */
			/* forget child test2 = child test1 */
			next(ste) = newid;
			ste->last = true;	/* father body = newid */
			child(test1) = tag1;
			child(test2) = tag2;	/* relace 1st operands of test */

			if (!complex(op21) ) {
				/* if the second operand of 1st test is simple, then identification
				could go in a t-teg (!!!NB overloading of inlined flag!!!).... */
				setinlined(newid);
			}

			kill_exp(op12, op12);
			*e = newid;
			if (scan_cond(&next(child(labst)), newid) == 2 && complex(op22)) {
				/* ... however a further use of identification means that
				   the second operand of the second test must also be simple */
				clearinlined(newid);
			}

			return 1;
		} else if (c2 && eq_exp (op21, op22)) {
			/* ....if second operands of tests are same, identify them */

			exp newid = getexp (sh (ste), next(ste), ste->last, op21, NULL, 0, 2, ident_tag);
			exp tag1 = getexp (sh (op21), test1, 1, newid, NULL, 0, 0, name_tag);
			exp tag2 = getexp (sh (op22), test2, 1, newid, NULL, 0, 0, name_tag);

			pt (newid) = tag1;
			pt (tag1)  = tag2;	/* uses of newid */
			next(op21) = ste;
			op21->last = false;
			/* body of newid */
			/* forget next child test2 = next child test1 */
			next(ste) = newid;
			ste->last = true;	/* father body = newid */
			next(op11) = tag1;
			next(op12) = tag2;

			if (!complex(op11) ) {
				setinlined(newid);
			}

			kill_exp(op22, op22);
			/* relace 2nd operands of test */
			* (e) = newid;
			if (scan_cond (&next(child(labst)), newid) == 2 && complex(op12) ) {
				clearinlined(newid);
			}

			return 1;
		} else if (op12->tag != name_tag
		           && op11->tag == name_tag
		           && child(op11) == outer_id
		           && eq_exp (child(outer_id), op12))
		{
			/* 1st param of test1 is already identified with 1st param of  test2 */
			exp tag = getexp (sh (op12), op22, 0, outer_id,
			                  pt (outer_id), 0, 0, name_tag);
			pt (outer_id) = tag;
			no (outer_id) += 1;

			if (complex(op21) ) {
				clearinlined(outer_id);
			}

			/* update usage of ident */
			child(test2) = tag;
			kill_exp(op12, op12);

			if (scan_cond (&next(child(labst)), outer_id) == 2 && complex(op22)) {
				clearinlined(outer_id);
			}

			return 2;
		}
	}

	return 0;
}

/*
 * does the scan on commutative and
 * associative operations and may perform
 * various transformations allowed by these properties
 */
static needs
likeplus(exp *e, exp **at)
{
	needs a1;
	needs a2;
	prop pc;
	exp *br  = &child(*e);
	exp prev;
	bool commuted = 0;
	exp dad = *e;

	a1 = scan (br, at);
	/* scan the first operand - won't be a val_tag */

	do {
		exp *prevbr;
		prevbr = br;
		prev = * (br);
		br = &next(prev);
		a2 = scan (br, at); /* scan the next operand ... */

		if ((*br)->tag != val_tag) {
			a1.floatneeds = MAX(a1.floatneeds, a2.floatneeds);
			pc = a2.propneeds & hasproccall;
			if (a2.fixneeds < maxfix && pc == 0) {
				/* ... its evaluation  will not disturb
				 the accumulated result */
				if (a2.fixneeds <= a1.fixneeds || commuted ||
				    a1.fixneeds >= maxfix || (a1.propneeds & hasproccall) != 0) {
					a1.fixneeds = MAX(a1.fixneeds, a2.fixneeds + 1);
				} else {
					/* ... needs more regs so put it first to reduce
							 register usage */
					exp op1 = child(dad);
					exp cop = * (br);
					bool lcop = cop->last;
					next(prev) = next(cop);

					if (lcop) {
						prev->last = true;
					}

					next(cop) = op1;
					cop->last = false;
					child(dad) = cop;
					br = (prev == op1) ? &next(cop) : prevbr;
					a1.fixneeds = MAX(a2.fixneeds, a1.fixneeds + 1);
				}

				a1.propneeds |= a2.propneeds;
			} else if (a1.fixneeds < maxfix &&
			           (a1.propneeds & hasproccall) == 0 && !commuted) {
				/* ...its evaluation will call a proc, so
				   put it first  */
				exp dad = * (e);
				exp op1 = child(dad);
				exp cop = * (br);
				bool lcop = cop->last;
				next(prev) = next(cop);

				if (lcop) {
					prev->last = true;
				}

				next(cop) = op1;
				cop->last = false;
				child(dad) = cop;
				br = (prev == op1) ? &next(cop) : prevbr;
				commuted = 1;
				a1.fixneeds = MAX(a2.fixneeds, a1.fixneeds + 1);
				a1.propneeds |= a2.propneeds;
				a1.maxargs = MAX(a1.maxargs, a2.maxargs);
			} else {
				/* ... its evaluation would disturb
					   accumulated result, so replace it by a
					   newly declared tag */
				cca (at, br);
				a1.fixneeds = MAX(a1.fixneeds, 2);
				a1.propneeds = a1.propneeds | morefix | (pc << 1);
				a1.maxargs = MAX(a1.maxargs, a2.maxargs);
			}
		}
	} while (!(*br)->last);

	if (!optop(*e)) {
		if (a1.fixneeds < 4) {
			a1.fixneeds = 4;
		}
	}

	return a1;
}

/* scan non-commutative fix pt operation */
static needs
likediv(exp *e, exp ** at)
{
	needs l;
	needs r;
	prop pc;
	exp *arg = &child(*e);

	l = scan(arg, at); /* scan 1st operand */
	arg = &next(*arg);
	r = scan (arg, at);
	/* scan second operand ... */
	l.floatneeds = MAX(l.floatneeds, r.floatneeds);
	pc = r.propneeds & hasproccall;

	if (r.fixneeds < maxfix && pc == 0) {/* ...it fits into registers */
		l.fixneeds = MAX(l.fixneeds, r.fixneeds + 1);
		l.propneeds = l.propneeds | r.propneeds;
	} else {
		/* ...it requires new declaration of second operand */
		cca (at, arg);
		l.fixneeds = MAX(l.fixneeds, 1);
		l.propneeds = l.propneeds | morefix | (pc << 1);
		l.maxargs = MAX(l.maxargs, r.maxargs);
	}

	if ((*e)->tag != test_tag && (!optop(*e) || (*e)->tag == div1_tag || (*e)->tag == mod_tag)) {
		if (l.fixneeds < 4) {
			l.fixneeds = 4;
		}
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
	bool withert = !(optop(*e));

	l = scan (arg, at);
	arg = &next(*arg);
	r = scan (arg, at);
	l.fixneeds = MAX(l.fixneeds, r.fixneeds);
	pcr = r.propneeds & hasproccall;
	pcl = l.propneeds & hasproccall;

	if (r.floatneeds <= l.floatneeds && r.floatneeds < maxfloat && pcr == 0) {
		l.floatneeds = MAX(2, MAX(l.floatneeds, r.floatneeds + 1));
		l.propneeds = l.propneeds | r.propneeds;
		ClearRev(op);
	} else if (pcl == 0 && l.floatneeds <= r.floatneeds && l.floatneeds < maxfloat ) {
		l.floatneeds = MAX(2, MAX(r.floatneeds, l.floatneeds + 1));
		l.propneeds = l.propneeds | r.propneeds;
		SetRev(op);
	} else if (r.floatneeds < maxfloat && pcr == 0) {
		l.floatneeds = MAX(2, MAX(l.floatneeds, r.floatneeds + 1));
		l.propneeds = l.propneeds | r.propneeds;
		ClearRev(op);
	} else {
		cca (at, arg);
		ClearRev(op);
		l.floatneeds = MAX(l.floatneeds, 2);
		l.propneeds = l.propneeds | morefloat | (pcr << 1);
		l.maxargs = MAX(l.maxargs, r.maxargs);
	}

	if (withert && l.fixneeds < 2) {
		l.fixneeds = 2;
	}

	return l;
}

/*
 * Calculates a needs value. Each element of which is the maximum of the
 * corresponding elements in the two parameter needs
 */
static needs
maxneeds(needs a, needs b)
{
	needs an;

	an.fixneeds   = MAX(a.fixneeds,   b.fixneeds);
	an.floatneeds = MAX(a.floatneeds, b.floatneeds);
	an.maxargs    = MAX(a.maxargs,    b.maxargs);
	an.propneeds = a.propneeds | b.propneeds;

	return an;
}

/*
 * calculates the needs of a tuple of
 * expressions; any new declarations
 * required by a component expression will
 * replace the component expression
 */
static needs
maxtup(exp e, exp ** at)
{
	exp * stat = &child(e);
	needs an;

	an = zeroneeds;
	if (*stat == NULL) {
		return an;
	}

	while (an = maxneeds (an, scan (stat, at)), !(*stat)->last) {
		stat = &next(*stat);
	}

	return an;
}

/*
 * finds if usedname is only used in cont
 * operation or as result of ident i.e.
 * value of name is unchanged over its scope
 */
static bool
unchanged(exp usedname, exp ident)
{
	exp uses;

	for (uses = pt(usedname); uses != NULL; uses = pt(uses)) {
		if (!intnl_to (ident, uses)) {
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
 * check if e  is (let a = 0 in
 * cond(inttest(L)=result; a=1 | L:top); a ni )
 * This will be compiled later using set instructions instead of branches
 */
static exp
absbool(exp id /* declaration */ )
{
	if (isvar (id) && child(id)->tag == val_tag && no (child(id)) == 0
	    && no (id) == 2 /* name initially 0 only used twice */ ) {
		exp bdy = next(child(id));
		if (bdy->tag == seq_tag && next(child(bdy))->tag == cont_tag &&
		    child(next(child(bdy)))->tag == name_tag &&
		    child(child(next(child(bdy)))) == id
		    /* one use is result  of sequence body */ )
		{
			exp c = child(child(bdy));
			if (c->last && c->tag == cond_tag /* seq is cond=c; id */ ) {
				exp first = child(c);
				exp second = next(child(c));
				if (no (child(second)) == 1 /* only one jump to else */ &&
				    next(child(second))->tag == top_tag
				    && first->tag == seq_tag /* cond is (seq= first | L: top) */ )
				{
					exp s = child(child(first));
					exp r = next(child(first));

					if (r->tag == ass_tag && child(r)->tag == name_tag &&
					    child(child(r)) == id && next(child(r))->tag == val_tag &&
					    no (next(child(r))) == 1 /* last of seq is id = 1 */ &&
					    s->last && s->tag == test_tag && pt (s) == second
					    && !is_floating(sh(child(s))->tag))
					{
					    /**t of seq is int test jumping to
					       second */
						return s;
					}
				}
			}
		}
	}

	return 0;
}

exp *
ptr_position(exp e)
{
	exp * a;
	exp dad = father(e);

	if (child(dad) == e) {
		a = &child(dad);
	} else {
		exp sib;

		for (sib = child(dad); next(sib) != e; sib = next(sib))
			;

		a = &next(sib);
	}

	return a;
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

/*
 * replace Var/Id x = Var y = e1 in { e2; contents(y)} in e3;
 * by Var x = e1 in { e2/(y=>x); e3}
 * replace Var/Id x = Id y = e1 in {e2; y} in e3
 * by Var/Id x = e1 in { e2/y=>(cont)x; e3}
 */
static void
tidy_ident(exp e)
{
	exp init;
	exp bdyinit;
	exp idy;
	exp e1;
	exp e3;

	assert(e->tag == ident_tag);
	init = child(e);
	e3 = next(init);

	if ( init->tag != ident_tag || isparam(e)) {
		return ;
	}

	tidy_ident(init);
	e1 = child(init);
	bdyinit = next(e1);

	if (!isvar(init)) {
		if (bdyinit->tag == seq_tag) {
			exp idy = next(child(bdyinit));
			exp nexte3;
			bool laste3;

			if (idy->tag != name_tag || child(idy) != init ||
			    no(idy) != 0 ||
			    shape_size(sh(idy)) != shape_size(sh(e1)) ||
			    shape_align(sh(idy)) != shape_align(sh(e1)) ) {
				return;
			}

			if (isvar(e)) {
				change_to_var(init);
			}

			change_names(init, e, idy);

			nexte3 = next(e3);
			laste3 = e3->last;
			next(child(bdyinit)) = e3;
			next(e3) = bdyinit;
			bdyinit->last = true;
			/* bdyinit is now { e2/(y=>x); e3} */
			next(bdyinit) = nexte3;

			if (laste3) {
				bdyinit->last = true;
			} else {
				bdyinit->last = false;
			}

			child(e) = e1;  /* next(e1) is bdyinit */
			return;
		} else if (bdyinit->tag == name_tag && (idy = child(bdyinit)) == init
		           && no(bdyinit) == 0 &&
		           shape_size(sh(idy)) == shape_size(sh(e1)) &&
		           shape_align(sh(idy)) == shape_align(sh(e1)) ) {
			/* form is Var/Id x = Id y = e1 in y in e3
				=> Var x = e1 in e3 */
			next(e1) = e3;
			child(e) = e1;
		}

		return;
	} else {
		if (bdyinit->tag == seq_tag) {
			exp cy = next(child(bdyinit));
			exp nexte3;
			bool laste3;

			if (cy->tag != cont_tag) {
				return;
			}

			idy = child(cy);
			if (idy->tag != name_tag || no(idy) != 0 ||
			    child(idy) != init ||
			    shape_size(sh(cy)) != shape_size(sh(e1)) ||
			    shape_align(sh(cy)) != shape_align(sh(e1)) ) {
				return;
			}

			if (!isvar(e)) {
				change_to_var(e);
				e3 = next(init);

				if (isvis(init)) {
					setvis(e);
				}

				if (!iscaonly(init)) {
					clearcaonly(e);
				}
			}

			change_names(init, e, idy);

			nexte3 = next(e3);
			laste3 = e3->last;
			next(child(bdyinit)) = e3;
			next(e3) = bdyinit;
			bdyinit->last = true;
			/* bdyinit is now { e2/(y=>x); e3} */
			next(bdyinit) = nexte3;

			if (laste3) {
				bdyinit->last = true;
			} else {
				bdyinit->last = false;
			}

			child(e) = e1;  /* next(e1) is bdyinit */
			return;
		} else if (bdyinit->tag == cont_tag) {
			exp cy =  next(child(bdyinit));
			idy = child(cy);

			if (idy->tag != name_tag || no(idy) != 0 ||
			    child(idy) != init ||
			    shape_size(sh(cy)) != shape_size(sh(e1)) ||
			    shape_align(sh(cy)) != shape_align(sh(e1)) ) {
				return;
			}

			/* form is Var x = Var y = e1 in cont(y) in e3
				=> Var x = e1 in e3 */
			if (!isvar(e)) {
				change_to_var(e);
				e3 = next(init);
				if (isvis(init)) {
					setvis(e);
				}
				if (!iscaonly(init)) {
					clearcaonly(e);
				}
			}

			next(e1) = e3;
			child(e)  = e1;
		}

		return;
	}
}

/* distribute selection throughout compound expressions */
static bool
chase(exp sel, exp * e)
{
	bool b = 0;
	switch ((*e)->tag) {
	case ident_tag:
	case seq_tag:
	case rep_tag:
	case labst_tag:
		b = chase(sel, &next(child(*e)));
		break;

	case solve_tag:
	case cond_tag: {
		exp *one;

		for (one = &child(*e); ; one = &next(*one)) {
			b |= chase(sel, one);
			if ((*one)->last) {
				break;
			}
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
			sh (ss) = sh (stare);
			*e = ss;

			return chase(sel, e);
		}
		/* ... continue to default */

	default:
		if (child(sel) != *e && sh(*e)->tag != bothd) {
			/* only change if not outer */
			exp stare = *e;
			exp newsel = getexp (sh (sel), next(stare), stare->last, stare, NULL,
			                     sel->props, no (sel), sel->tag);
			*e =  newsel;
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

static bool
use_not_rep(exp e)
{
	exp u;

	if (no(e) != 1) {
		return 0;
	}

	u = father(pt(e));
	while (u != e) {
		if (u->tag == rep_tag || u->tag == solve_tag) {
			return 0;
		} else {
			u = father(u);
		}
	}

	return 1;
}

/*
 * This procedure works out register requirements of an exp. At each call
 * the fix field of the needs is the number of fixpnt registers required to
 * contain live values to evaluate this expression. This never exceeds
 * maxfix because if it would have, a new declaration is introduced in the
 * exp tree (similarly for floating regs and maxfloat). In these cases the
 * prop field will contain the bits morefix (or morefloat).
 *
 * Scan also works out various things concerned with proc calls.  The
 * maxargs field contains the max size in bits of the space required for
 * the parameters of all the procedures called in the exp. An exp proc call
 * produces a hasproccall bit in the prop field, if this is transformed as
 * part of the definition of a new declaration the bit is replaced by a
 * usesproccall. The distinction is only used in unfolding nested proc
 * calls; MIPS requires this to be done statically. The condition that a
 * proc exp is a leaf (i.e no proc calls) is that its prop contains neither
 * bit.
 *
 * If an ident exp is suitable, scan marks the props of ident with
 * either inreg or infreg bits to indicate that a t reg may be used for
 * this tag.
 *
 * A thorough understanding of needs along with other procedures
 * that do switch (exp->tag) requires a knowledge of the meaning of the
 * fields of the exp in each case.
 *
 * e is the expression to be scanned, at
 * is the place to put any new decs. NB order of recursive
 * calls with same at is critical
 */
needs
scan(exp *e, exp **at)
{
	exp ste = *e;
	int nstare = ste->tag;

	/*
	  while
	    (nstare == diag_tag || nstare == cscope_tag || nstare == fscope_tag) {
	    e = &child(ste);
	    ste = * (e);
	    nstare = ste->tag;
	  }
	*/

	switch (nstare) {
	case 0:
		return zeroneeds;

	case compound_tag:
	case nof_tag:
	case concatnof_tag:
	case ncopies_tag: {
		needs nl;
		bool cantdo;
		exp dad;

		if (ste->tag == ncopies_tag && child(ste)->tag != name_tag
		    && child(ste)->tag != val_tag ) {
			nl = scan(&child(*e), at);
			cca(at, &child(*e));
		} else {
			nl = maxtup(*(e), at);
		}

		dad = father(ste);
		if (dad->tag == compound_tag || dad->tag == nof_tag
		    || dad->tag == concatnof_tag) {
			cantdo = 0;
		} else if (ste->last ) {
			if (next(ste)->tag == ass_tag ) {
				exp a = child(next(ste));
				cantdo = (a->tag != name_tag || !isvar(child(a)) );
			} else {
				cantdo = 1;
			}
		} else if (next(ste)->last) {
			cantdo = (next(next(ste))->tag != ident_tag) ;
		} else {
			cantdo = 1;
		}

		if (cantdo)  {
			/* can only deal with tuples in simple assignment or identity */
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
		exp t, f, v;

		if (oddtest(ste, & t, &f, &v) ) {
			/* transform to f((absbool(t) <<1)-1)  */
			exp bc = next(ste);
			bool lc = ste->last;

			exp ab  = getexp(sh(v), NULL, 0, t, NULL, 0, 0, absbool_tag);
			exp shl = getexp(sh(v), NULL, 0, ab, NULL, 0, 0, shl_tag);
			exp v1  = getexp(sh(v), shl, 1, NULL, NULL, 0, 1, val_tag);
			exp p   = getexp(sh(v), NULL, 1, shl, NULL, 0, 0, plus_tag);
			exp vm1 = getexp(sh(v), p, 1, NULL, NULL, 0, -1, val_tag);

			next(ab) = v1;
			next(shl) = vm1;
			next(t) = ab;
			t->last = true;

			if (no(v) == -1) {
				settest_number(t, notbranch[test_number(t) - 1]);
			}

			if (f == v) {
				*e = p;
			} else {
				child(next(v)) = p;
				next(p) = next(v);
				*e = f;
			}

			next(*e) = bc;
			if (lc) {
				(*e)->last = true;
			} else {
				(*e)->last = false;
			}

			return scan(e, at);
		}

		if (is_maxlike(ste, &t) ) {
			child(ste) = t;
			next(t) = ste;
			t->last = true;
			ste->tag = maxlike_tag;
			return scan(&child(ste), at);
		}

		if (is_minlike(ste, &t) ) {
			child(ste) = t;
			next(t) = ste;
			t->last = true;
			ste->tag = minlike_tag;
			return scan(&child(ste), at);
		}

		if (is_abslike(ste, &t) ) {
			child(ste) = t;
			next(t) = ste;
			t->last = true;
			ste->tag = abslike_tag;
			return scan(&child(ste), at);
		}

		if (is_fabslike(ste, &t) ) {
			child(ste) = child(t);
			next(child(t)) = ste;
			child(t)->last = true;
			ste->tag = fabs_tag;
			return scan(&child(ste), at);
		}

		if (scan_cond(e, NULL) != 0) {
			return scan(e, at);
		}
		/* else goto next case */
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

	case ident_tag:
		tidy_ident(*e); /* remove nugatory idents in initialisation */

	{
		needs bdy;
		needs def;
		exp stare = *e;
		exp *arg = &next(child(stare));
		exp t = pt (stare), s;
		bool fxregble;
		bool flregble;
		bool old_nonevis = nonevis;
		exp ab;

		if (isvar(stare) && !iscaonly(stare)) {
			setvis(stare);
		}

		if (!isvar (stare)  && !isvis(stare) && !isparam(stare)
		    && ((stare->props & 0x10) != 0)
		    && use_not_rep(stare))
		{
			/* remove declaration */
			/*only one use */
			exp u = pt(stare);
			exp init = child(stare);
			exp bdy = next(init);
			exp * posu = ptr_position(u);
			next(init) = next(u);

			if (u->last) {
				init->last = true;
			} else {
				init->last = false;
			}

			*posu = init;
			next(bdy) = next(stare);
			if (stare->last) {
				bdy->last = true;
			} else {
				bdy->last = false;
			}

			*e = bdy;
			return scan(e, at);
		}

		if (isparam(stare)
		    && child(stare)->tag != formal_callee_tag) {
			exp def = child(stare);
			shape shdef = sh(def);
			long n = rounder(stparam, shape_align(shdef));
			long sizep = shape_size(shdef);

			assert(def->tag == clear_tag);
			if (is_floating(shdef->tag)) {
				if (n + sizep <= 128 ) {
					def->props = floatparam++;
					maxfloat--;
				}
			} else if (sizep <= 32  && stparam <= 96  ) {
				def->props = fixparam;
				maxfix--;
			} else {
				def->props = 0;
			}

			if (endian == ENDIAN_BIG && sizep < 32 && valregable(shdef)) {
				/* characters are promoted to ints */
				int dn = (sizep == 8) ? 24 : 16;
				exp pu = pt(stare);
				while (pu != NULL) {
					no(pu) += dn;
					pu = pt(pu);
				}
			}

			no(def) = n;
			stparam = rounder(n + sizep, 32 );
			fixparam = 4 + (stparam >> 5);

			if (!is_floating(shdef->tag) && !valregable(shdef)) {
				setvis(stare);
			}

			/* now def->props = pos parreg and no(def) = par stack address */
		} else if (isparam(stare) && child(stare)->tag == formal_callee_tag) {
			exp def = child(stare);
			shape shdef = sh(def);
			long sizep = shape_size(shdef);
			long alp = shape_align(shdef);
			long n = rounder(callee_size, alp);
			no(def) = n;
			callee_size = rounder(n + sizep, 32);

		}

		nonevis &= !isvis(stare);

		bdy = scan (arg, &arg);
		/* scan the body-scope */
		arg = &child(stare);
		def = scan (arg, &arg);
		/* scan the initialisation of tag */

		nonevis = old_nonevis;
		t = child(stare);
		s = next(t);
		fxregble = fixregable (stare);
		flregble = floatregable (stare);

		if (isparam(stare)) {
			if (!isvis(stare) && !isoutpar(stare)
			    && child(stare)->tag != formal_callee_tag &&
			    (bdy.propneeds & anyproccall) == 0)
			{
				/* leave pars in par regs or put in t-regs
				   !! WHAT ABOUT TEMP DECS !!
				*/
				int x = child(stare)->props;

				if (x != 0) {
					no(stare) = x;
					if (flregble) {
						stare->props |= infreg_bits;
					} else {
						stare->props |= inreg_bits;
					}
				} else if (fxregble &&
				           bdy.fixneeds < maxfix && (bdy.propneeds & morefix) == 0 ) {
					no(stare) = 0;
					stare->props |= inreg_bits;
					bdy.fixneeds += 1;
				} else if (flregble &&
				           bdy.floatneeds < maxfloat &&
				           (bdy.propneeds & morefloat) == 0 ) {
					no(stare) = 0;
					stare->props |= infreg_bits;
					bdy.floatneeds += 1;
				} else {
					no(stare) = 100;
				}
			} else {
				no(stare) = 100;
			}
		} else {
			if ((ab = absbool (stare)) != NULL) {
				/* form is (let a = 0 in cond(test(L)=ab;
				   a=1 | L:top) ni replace declaration by
				   ABS */
				next(ab) = stare;
				ab->last = true; /* father => *e */
				child(stare) = ab;
				pt(stare) = NULL;
				pt(ab) = NULL;
				stare->tag = absbool_tag;
				return maxneeds(bdy, def);
			}

			if (!isvis (*e) && !isparam(*e) &&
			    (bdy.propneeds & (anyproccall | uses2_bit)) == 0
			    && (fxregble || flregble) &&
			    (t->tag == apply_tag ||
			     (s->tag == seq_tag && next(child(s))->tag == res_tag &&
			      child(next(child(s)))->tag == cont_tag && isvar (stare) &&
			      child(child(next(child(s))))->tag == name_tag &&
			      child(child(child(next(child(s))))) == stare)))
            	   /* Let a := ..; return cont a */
			{
				/* put tag in result reg if definition is
				   call of proc, or body ends with return
				   tag, provided result is not used other
				   wise */
				stare->props |= (fxregble) ? inreg_bits : infreg_bits;
				bdy.propneeds |= uses2_bit;
				no (stare) = 101;   /* identification  uses result reg in body */
				if (fxregble) {
					bdy.fixneeds += 1;
				}
			} else if (!isvar (*e) && !isparam(*e) &&
				 ((t->tag == reff_tag && child(t)->tag == cont_tag &&
				   child(child(t))->tag == name_tag && isvar (child(child(child(t))))
				   && !isvis (child(child(child(t)))) && !isglob (child(child(child(t))))
				   && unchanged (child(child(child(t))), stare)
				   /* reff cont variable-not assigned to in
					  scope */
				  ) ||
				  (t->tag == cont_tag && child(t)->tag == name_tag &&
				   isvar (child(child(t))) && !isvis (child(child(t))) && !isglob (child(child(t)))
				   && unchanged (child(child(t)), stare))))
				   /* cont variable - not assigned to in scope */
			{
				stare->props |= defer_bit;
				/* dont take space for this dec */
			} else if (!isvar (stare)  && !isvis(stare) &&
			           ((stare->props & 0x10 /* forced in const */ ) == 0
			            && (t->tag == name_tag || t->tag == val_tag ))) {
				stare->props |= defer_bit;
				/* dont take space for this dec */
			} else if ( fxregble && bdy.fixneeds < maxfix
				&& ( isinlined(stare) ||
					 ((bdy.propneeds & morefix) == 0 &&
					  ((bdy.propneeds & anyproccall) == 0
					   || tempdec(stare, ((bdy.propneeds & morefix) == 0 &&
									   bdy.fixneeds < 10))))))
			{
				/* put this tag in some  fixpt t-reg -
				   which will be decided  in make_code */
				stare->props |= inreg_bits;
				no (stare) = 0;
				bdy.fixneeds += 1;
			} else if ( flregble && bdy.floatneeds < maxfloat
				&& ( isinlined(stare) ||
					 ((bdy.propneeds & morefloat) == 0
					  && ((bdy.propneeds & anyproccall) == 0
						  || tempdec (stare, ((bdy.propneeds & morefloat) == 0 &&
									  bdy.floatneeds < 6)))))) {
				/* put this tag in some  float t-reg -
				   which will be decided  in make_code */
				stare->props |= infreg_bits;
				no (stare) = 0;
				bdy.floatneeds += 1;
			} else {
				if (fxregble && (bdy.propneeds & anyproccall) == 0) {
					SetPossParReg(stare);
				}
				no (stare) = 100;
				/* allocate either on stack or saved reg
				*/
			}
		}

		bdy = maxneeds (bdy, def);
		if ((bdy.propneeds & usesproccall) != 0) {
			bdy.propneeds |= hasproccall;
		}

		return bdy;
	}

	case seq_tag: {
		exp *stat;
		exp *arg  = &next(child(*e));
		exp *atsc = &child(child(*e));
		needs an;

		for(;;) {
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

				/* only worthwhile eliding if ne is a cond */
				if (ne->tag != cond_tag ) {
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
				if (!sc->last) { /* not last in seq - swallow next*/
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
				} else { /* whole sequence can be replace by cond */
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

		an = scan (arg, &arg);
		stat = &child(child(*e));

		arg = stat;
		for (;;) {
			needs stneeds;
			stneeds = scan (stat, &arg);
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
	case trap_tag:
		return zeroneeds;

	case ass_tag:
	case assvol_tag: {
		exp *lhs = &child(*e);
		exp *rhs = &next(*lhs);
		needs nr;

		nr = scan (rhs, at);
		/* scan source */

		if ((*lhs)->tag == name_tag && (no(*lhs) < 8 * 32768 && no(*lhs) >= -8 * 32768) &&
		    (isvar (child(* (lhs))) &&  /* can do better for regable rhs*/
		     ((nr.propneeds & (hasproccall | morefix)) == 0
		      && nr.fixneeds < maxfix)))
		{
			/* simple destination */
			return nr;
		} else {
			needs nl;
			prop prps = (nr.propneeds & hasproccall) << 1;
			nl = scan (lhs, at); /* scan destination */

			if ((*rhs)->tag == apply_tag && nstare == ass_tag &&
			    (nl.propneeds & (uses2_bit | anyproccall)) == 0) {
				/* source is proc call, so assign result
				   reg directly */
				;
			} else if (nr.fixneeds >= maxfix || prps != 0) {
				/* source and destination regs overlap, so
				   identify source */
				cca (at, rhs);
				nl = shapeneeds (sh (* (rhs)));
				nl.propneeds |= morefix;
				nl.propneeds &= ~(prps >> 1);
				nl.propneeds |= prps;
			}

			nr.fixneeds += 1;
			return maxneeds (nl, nr);
		}
	}

	case res_tag:
	case untidy_return_tag: {
		ash a;
		needs x;
		shape s;
		exp *arg = &child(*e);

		s = sh(*(arg));
		a = ashof(s);
		(*e)->props = 0; /* clear possibility of tlrecirsion; may be set later */
		x = scan(arg, at); /* scan result exp ... */

		if (is_floating(s->tag)) {
			/* ... floating pt result */
			x.propneeds |= realresult_bit;
			if (s->tag != shrealhd) {
				x.propneeds |= longrealresult_bit;
			}
		} else {
			if (!valregable (s)) {
				/* .... result does not fit into reg */
				x.propneeds |= long_result_bit;
			}
		}

		if (a.ashsize != 0) {
			/* ...not a void result */
			x.propneeds |= has_result_bit;
		}

		return x;
	}

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
			cca (at, fn);
			nds.propneeds &= ~hasproccall;
			nds.propneeds |= usesproccall;
			fn = &child(application);
		}

		for (i = 0; i < no(cers); i++) {
			needs onepar;
			shape shonepar = sh(*cerl);
			exp *par = (*cerl)->tag == caller_tag ? &child(*cerl) : cerl;
			int n = rounder(stpar, shape_align(shonepar));
			onepar = scan(par, at);

			if ((i != 0 && (onepar.propneeds & hasproccall) != 0) ||
			    onepar.fixneeds + (stpar >> 5) > maxfix) {
				/* if it isn't the first parameter, and it
				   calls a proc, identify it */
				cca (at, par);
				nds.propneeds |= usesproccall;
				nds = maxneeds (shapeneeds(sh(*par)), nds);
				nds.maxargs = MAX(nds.maxargs, onepar.maxargs);
			} else {
				nds = maxneeds (onepar, nds);
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
			if (is_floating(sh(application)->tag)
			    || valregable(sh(application)))
			{
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
			    onepar.fixneeds + 1 > maxfix)
			{
				/* if it calls a proc, identify it */
				cca (at, par);
				nds.propneeds |= usesproccall;
				nds = maxneeds (shapeneeds (sh (* (par))), nds);
				nds.maxargs = MAX(nds.maxargs, onepar.maxargs);
			} else {
				nds = maxneeds (onepar, nds);
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
			cca (at, ptr);
			nds.propneeds |= usesproccall;
			nds = maxneeds (shapeneeds (sh (* (ptr))), nds);
			nds.maxargs =  MAX(nds.maxargs, ndsp.maxargs);
		} else {
			nds = ndsp;
		}

		ndsp = scan(&next(child(*e)), at);
		if (((ndsp.propneeds & hasproccall) != 0) ||
		    ndsp.fixneeds + 2 > maxfix) {
			cca (at, &next(child(cees)));
			nds.propneeds |= usesproccall;
			nds = maxneeds (shapeneeds (sh (next(child(*e)))), nds);
			nds.maxargs = MAX(nds.maxargs, ndsp.maxargs);
		} else {
			nds = maxneeds (ndsp, nds);
		}

		if (nds.fixneeds < 8) {
			nds.fixneeds = 8;
		}

		return nds;
	}

	case same_callees_tag: {
		needs nds;

		nds = zeroneeds;
		nds.fixneeds = 6;

		return nds;
	}

	case tail_call_tag: {
		needs ndsp;
		needs nds;
		exp *fn = &child(*e);
		ndsp = scan(fn, at);

		if (((ndsp.propneeds & hasproccall) != 0) ||
		    ndsp.fixneeds + 1 > maxfix) {
			cca (at, fn);
			nds.propneeds |= usesproccall;
			nds = maxneeds (shapeneeds (sh (* (fn))), nds);
			nds.maxargs =  MAX(nds.maxargs, ndsp.maxargs);
		} else {
			nds = ndsp;
		}

		gen_call = true;

		ndsp = scan(&next(child(*e)), at);
		nds = maxneeds(nds, ndsp);

		if (nds.fixneeds < 5) {
			nds.fixneeds = 5;
		}

		return nds;
	}

	case apply_tag: {
		exp application = *e;
		exp fn = child(application);
		exp *par = &next(fn);
		exp *fnexp = &child(*e);
		int parsize = 0;
		needs nds;
		bool tlrecpos = nonevis && callerfortr;
		int i;

		if (endian == ENDIAN_BIG && !fn->last) {
			/* ABI says that all int pars <32 bits are promoted to int*/
			for(;;) {
				shape s = sh(*par);
				if (shape_size(s) < 32 && valregable(s)) {
					exp cv = getexp((is_signed(s)) ? slongsh : ulongsh, next(*par),
					                (*par)->last, *par, NULL, 0, 0, chvar_tag);
					next(*par) = cv;
					(*par)->last = true;
					*par = cv;
				}

				if ((*par)->last) {
					break;
				}

				par = &next(*par);
			}

			par = &next(fn);
		}

		nds = scan (fnexp, at);
		/* scan the function exp ... */
		if ((nds.propneeds & hasproccall) != 0) {
			/* .... it must be identified */
			cca (at, fnexp);
			nds.propneeds &= ~hasproccall;
			nds.propneeds |= usesproccall;
			fn = child(application);
			par = &next(fn);
		}

		if (fn->tag != name_tag ||
		    (child(child(fn)) != NULL && child(child(fn))->tag != proc_tag) ) {
			tlrecpos = 0;
		}

		/* scan parameters in turn ... */
		for (i = 1; !fn->last; ++i) {
			needs onepar;
			shape shpar = sh(*par);
			onepar = scan (par, at);

			parsize = rounder(parsize, shape_align(shpar));
			onepar.fixneeds += (parsize >> 5);

			if ((i != 1 && (onepar.propneeds & hasproccall) != 0) ||
			    onepar.fixneeds > maxfix) {
				/* if it isn't the first parameter, and it
				   calls a proc, identify it */
				cca (at, par);
				nds.propneeds |= usesproccall;
				if (onepar.fixneeds != 0) {
					nds.propneeds |= morefix;
				}

				if (onepar.floatneeds != 0) {
					nds.propneeds |= morefloat;
				}

				nds = maxneeds (shapeneeds (sh (* (par))), nds);
				nds.maxargs = MAX(nds.maxargs, onepar.maxargs);
			} else {
				nds = maxneeds (onepar, nds);
			}

			parsize = rounder(parsize + shape_size(shpar), 32);
			if ((!valregable(shpar) && !is_floating(shpar->tag)) || parsize > 128) {
				tlrecpos = 0;
			}

			if ((*par)->last) {
				break;
			}

			par = &next(*par);
		}

		if ((i = specialfn (fn)) > 0) {
			nds = maxneeds (specialneeds (i), nds);
			return nds;
		}

		if (tlrecpos) {
			exp dad = father(application);
			if (dad->tag == res_tag) {
				dad->props = 1; /* do a tl recursion*/
			}
		}

		nds.propneeds |= hasproccall;
		nds.maxargs = MAX(nds.maxargs, parsize);

		return nds;
	}

	case val_tag: {
		exp s = sh(*e);

		if (s->tag == offsethd && al2(s) >= 8) {
			/* express disps in bytes */
			no(*e) = no(*e) >> 3;
		}

		return shapeneeds(sh(*e));
	}

	case name_tag: {
		int n = no(*e);
		shape vs = sh(*e);

		if (n >= 8 * 32768 || n < -8 * 32786) { /* offset too big for assembler */
			shape  s = f_offset(al1_of(vs), al1_of(vs));
			exp v = me_shint(s, n);
			exp nm = me_obtain(child(*e));
			exp ao = me_b3(vs, nm, v, addptr_tag);
			next(ao) = next(*e);

			if ((*e)->last) {
				ao->last = true;
			} else {
				ao->last = false;
			}

			*e = ao;
			kill_exp(ste, ste);
			return scan(e, at);
		}

		return shapeneeds (sh (* (e)));
	}

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
	case env_size_tag:
		return shapeneeds(sh(*e));

	case prof_tag:
		(*e)->tag = top_tag;
		return zeroneeds;

	case clear_tag:
	case top_tag:
	case local_free_all_tag:
	case formal_callee_tag:
		return zeroneeds;

	case case_tag: {
		needs nds;

		nds = scan(&child(*e), at);
		if (nds.fixneeds < 3) {
			nds.fixneeds = 3;
		}

		return nds;
	}

	case set_stack_limit_tag:
	case neg_tag:
	case not_tag:
	case abs_tag:
	case offset_negate_tag:
	case absbool_tag:
	case return_to_label_tag:
	case diagnose_tag:
	case goto_lv_tag: {
		exp *arg = &child(*e);
		return scan (arg, at);
	}

	case fneg_tag:
	case fabs_tag:
	case chfl_tag: {
		needs nds;

		nds = scan(&child(*e), at);
		if (!optop(*e) && nds.fixneeds < 2) {
			nds.fixneeds = 2;
		}

		return nds;
	}

	case alloca_tag: {
		needs nds;

		nds = scan(&child(*e), at);
		if (nds.fixneeds < 2) {
			nds.fixneeds = 2;
		}

		return nds;
	}

	case bitf_to_int_tag: {
		exp *arg = &child(*e);
		needs nds;
		exp stararg;
		exp stare;
		int sizeb;

		nds = scan (arg, at);
		stararg = *(arg);
		stare = * (e);
		sizeb = ashof (sh (stararg)).ashsize;

		if ((stararg->tag == name_tag &&
		     ((sizeb == 8 && (no (stararg) & 7) == 0)
		      || (sizeb == 16 && (no (stararg) & 15) == 0)
		      || (sizeb == 32 && (no(stararg) & 31) == 0))
		    ) || (stararg->tag == cont_tag &&
		          ((child(stararg)->tag != name_tag && child(stararg)->tag != reff_tag)
		           || (sizeb == 8 && (no (child(stararg)) & 7) == 0)
		           || (sizeb == 16 && (no (child(stararg)) & 15) == 0)
		           || (sizeb == 32 && (no(child(stararg)) & 31) == 0))))
		{
			bool sgned = is_signed(sh(stare));
			shape ns = (sizeb == 8) ? ( (sgned) ? scharsh : ucharsh)
			           : (sizeb == 16) ? ((sgned) ? swordsh : uwordsh)
			           : sh(stare);
			/*  can use short loads instead of bits extractions*/
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
		return scan (arg, at);
	}

	case round_tag: {
		needs s;
		exp *arg = &child(*e);
		shape sres = sh(*e);

		if (shape_size(sres) != 32 ) {
			exp ch = getexp(sres, next(*e), (*e)->last, *e, pt(*e), (*e)->props,
			                0, chvar_tag);
			next(*e) = ch;
			(*e)->last = true;
			sh(*e) = slongsh;

			*e = ch;
			return scan(e, at);
		}

		s = scan (arg, at);
		s.fixneeds = MAX(s.fixneeds, (optop(*e)) ? 2 : 5);
		s.floatneeds = MAX(s.floatneeds, 2);

		return s;
	}

	case shl_tag:
	case shr_tag:
	case long_jump_tag: {
		needs nr;
		needs nl;
		prop prps;

		exp *lhs = &child(*e);
		exp *rhs = &next(*lhs);

		if ((*rhs)->tag == val_tag) {
			return scan (lhs, at);
		}

		nr = scan(rhs, at);
		nl = scan(lhs, at);
		rhs = &next(*lhs);
		prps = (nr.propneeds & hasproccall) << 1;

		/* if reg requirements overlap, identify second operand */
		if (nr.fixneeds >= maxfix || prps != 0) {
			cca (at, rhs);
			nl = shapeneeds (sh (* (rhs)));
			nl.propneeds |= morefix;
			nl.propneeds &= ~(prps >> 1);
			nl.propneeds |= prps;
		}

		nr.fixneeds += 1;
		return maxneeds (nl, nr);
	}

	case test_tag: {
		exp stare = *(e);
		exp l = child(stare);
		exp r = next(l);
		exp dad = father(stare);
		bool xlike = (dad->tag == maxlike_tag || dad->tag == minlike_tag || dad->tag == abslike_tag);
		/* don't do various optimisations if xlike */

		if (!stare->last && next(stare)->tag == test_tag &&
		    no (stare) == no (next(stare)) &&
		    stare->props == next(stare)->props &&
		    eq_exp (l, child(next(stare))) && eq_exp (r, next(child(next(stare)))))
		{
			/* same test following in seq list - remove second test */
			if (next(stare)->last) {
				stare->last = true;
			}

			next(stare) = next(next(stare));
		}

		if (stare->last && next(stare)->tag == 0/* seq holder */
		    && next(next(stare))->tag == test_tag &&
		    next(next(next(stare)))->tag == seq_tag &&
		    no (stare) == no (next(next(stare))) &&
		    stare->props == next(next(stare))->props &&
		    eq_exp (l, child(next(next(stare))))
		    && eq_exp (r, next(child(next(next(stare))))))
		{
			/* same test following in seq res - void second test */
			next(next(stare))->tag = top_tag;
			child(next(next(stare))) = NULL;
			pt (next(next(stare))) = NULL;
		}

		if (!xlike && l->tag == val_tag && (stare->props == 5 || stare->props == 6)) {
			/* commute  const = x */
			next(l) = stare;
			l->last = true;
			next(r) = l;
			r->last = false;
			child(stare) = r;
			r = l;
			l = child(stare);
		}

		if (!xlike && r->tag == val_tag && (stare->props == 5 || stare->props == 6) &&
		    no (r) == 0 &&
		    l->tag == and_tag && next(child(l))->tag == val_tag &&
		    (no (next(child(l))) & (no (next(child(l))) - 1)) == 0)
		{
			/* zero test  x & 2^n   -> neg test (x shl (31-n)) */
			long n = no (next(child(l)));
			int  x;

			for (x = 0; n > 0; x++) {
				n = n << 1;
			}

			if (x == 0) {
				/* no shift required */
				next(child(l)) = r; /* zero there */
				child(stare) = child(l);/* x */
			} else {
				l->tag = shl_tag;
				no (next(child(l))) = x;
			}

			stare->props -= 3;	/* test for neg */
			sh (child(stare)) = slongsh;
		}

		if (l->tag == bitf_to_int_tag && r->tag == val_tag &&
		    (stare->props == 5 || stare->props == 6) &&
		    (child(l)->tag == cont_tag || child(l)->tag == name_tag))
		{
			/* equality of bits against +ve consts
			   doesnt need sign adjustment */
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
			return fpop (e, at);
		} else if (!xlike && r->tag == val_tag && no (r) == 1
		           && (stare->props == 3 || stare->props == 2)) {
			no (r) = 0;
			if (stare->props == 3) {
				stare->props = 4;/* branch >=1 -> branch > 0 */
			} else {
				stare->props = 1;/* branch <1 -> branch <= 0 */
			}
		}

		return likediv(e, at);
	}

	case movecont_tag: {
		exp *d  = &child(*e);
		exp *s  = &next(*d);
		exp *sz = &next(*s);

		needs nd;
		needs ns;
		needs nsz;

		prop prps ;
		nd  = scan(d, at);
		ns  = scan(s, at);
		nsz = scan(sz, at);
		prps = (ns.propneeds & hasproccall) << 1;

		if (ns.fixneeds >= maxfix || prps != 0) {
			/* if reg requirements overlap, identify second operand */
			cca (at, s);
			ns = shapeneeds (sh (* (s)));
			ns.propneeds |= morefix;
			ns.propneeds &= ~(prps >> 1);
			ns.propneeds |= prps;
		}

		nd.fixneeds += 1;
		nd = maxneeds (nd, ns);
		prps = (nsz.propneeds & hasproccall) << 1;

		if (nd.fixneeds + nsz.fixneeds >= maxfix || prps != 0) {
			/* if reg requirements overlap, identify
			   last operand */
			cca (at, sz);
			nsz = shapeneeds (sh (* (sz)));
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
		/* replace any operands which are neg(..) by - ,if poss */
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

		if (someneg) {
			/* there are some neg() operands */
			if (allneg) {
				/* transform -..-... to -(..+.. +...) */
				exp x = child(sum);
				list = child(x);

				for (;;) {
					if (!x->last) {
						next(list) = child(next(x));
						list->last = false;
						list = next(list);
						x = next(x);
					} else {
						next(list) = sum;
						list->last = true;
						child(sum) = child(child(sum));
						/* use existing exp */
						break;
					}
				}

				x = getexp (sh (sum), next(sum), sum->last, sum, NULL,
				            0, 0, neg_tag);
				sum->last = true;
				next(sum) = x; /* set father of sum to be negate */
				*e = x;
			} else {
				/* transform to  ((..(..+..) - ..) -..) */
				int   n = 0;
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
			/* end else allneg */

			return scan(e, at);
		}
		/* end someneg  */
		return likeplus(e, at);
	}

	case addptr_tag: {
		exp p = child(*e);
		exp d = next(p);
		int fal = frame_al_of_ptr(sh(p));

		if (fal != 0) {
			int oal = frame_al1_of_offset(sh(d));

			/*
			if( ((oal-1)&oal) != 0) {
				error(ERR_INTERNAL, "can't cope with mixed frame offsets yet");
			}

			if ((oal & fal)==0) {
				error(ERR_INTERNAL, "frame-ptr and offset incompatible");
			}
			*/

			if ( includes_vcallees(fal) && l_or_cees(oal)) {
				/* if oal = locals or callees relative to local reg
				   else relative to fp */
				exp ne = getexp(sh(p), d, 0, p, NULL, 0, 0,
				                locptr_tag);
				next(p) = ne;
				p->last = true;
				child(*e) = ne;
			}
		}

		/* ... and continue */
	}

	case mult_tag:
	case and_tag:
	case or_tag:
	case xor_tag:
		return likeplus(e, at);

	case reff_tag:
	case chvar_tag:
	case offset_pad_tag:
	case locptr_tag: {
		exp *arg = &child(*e);
		return maxneeds(scan(arg, at), shapeneeds(sh(*e)));
	}

	case float_tag:  {
		needs nds;
		exp *arg = &child(*e);
		nds = maxneeds (scan (arg, at), shapeneeds (sh (* (e))));

		if (sh(child(*e))->tag == ulonghd) {
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

		nds = maxneeds(scan(arg, at), shapeneeds(sh(*e)));
		nds.fixneeds = MAX(nds.fixneeds, 2);

		return nds;
	}

	case offset_mult_tag:
	case offset_div_tag: {
		exp op1 = child(*e);
		exp op2 = next(op1);
		shape s = sh(op2);

		if (op2->tag == val_tag  && s->tag == offsethd
		    && al2(s) >= 8) {
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
				    next(child(op1))->tag == val_tag )
				{
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
				} else { /* will do this by literal shift */
					no(op2) = n;
					return scan(&child(*e), at);
				}
			}
		}

		return likediv (e, at);
	}

	case offset_add_tag: {
		exp l = child(*e);
		exp r = next(l);

		if (l->tag == val_tag) {
			sh(l) = sh(r); /* both offsets will be treated the same */
			child(*e) = r;
			r->last = false;
			next(r) = l;
			l->last = true;
			next(l) = *e;
			/* ... and put val last */
		} else if (al2(sh(l)) >= 8 && al2(sh(r)) < 8) {
			return likediv (e, at);
		}

		(*e)->tag = plus_tag;
		return likeplus(e, at);
	}

	case offset_subtract_tag: {
		exp l = child(*e);
		exp r = next(l);

		if (r->tag == val_tag) {
			sh(r) = sh(l); /* both offsets will be treated the same */
		} else if ( al2(sh(r)) >= 8 && al2(sh(l)) < 8) {
			return likediv (e, at);
		}

		(*e)->tag = minus_tag;
		return likediv (e, at);
	}

	case div0_tag:
	case rem0_tag:
	case div2_tag:
	case minus_tag:
	case subptr_tag:
	case minptr_tag:
	case make_stack_limit_tag:
	case rem2_tag:
	case offset_div_by_int_tag:
	case component_tag:
		return likediv (e, at);

	case offset_max_tag:
	case max_tag:
	case min_tag:
	case local_free_tag: {
		needs nd;

		nd = likediv(e, at);
		nd.fixneeds = MAX(nd.fixneeds, 3);

		return nd;
	}

	case div1_tag:
		if (!is_signed(sh(*e))) {
			(*e)->tag = div2_tag;
		}

		return likediv(e, at);

	case mod_tag:
		if (!is_signed(sh(*e))) {
			(*e)->tag = rem2_tag;
		}

		return likediv(e, at);

	case fdiv_tag:
	/* THIS IS TOO ACCURATE FOR PLATFORM!
	{
		exp z = * (e);
		exp a2 = next(child(z));

		if (a2->tag == real_tag) { replace X/const by X*const^-1
		  flt inverse;
		  flt unitflt;
		  str2flt("1.0", &unitflt, NULL);

		  if (flt_div (unitflt, flptnos[no (a2)], &inverse) == OKAY) {
					  a/const => a* (1.0/const)
		    int f = new_flpt ();
		    flptnos[f] = inverse;
		    no (a2) = f;
		    z->tag = fmult_tag;
		  }
		}
	}
	*/
		/* and continue to next case */

	case fplus_tag:
	case fminus_tag:
	case fmult_tag: {
		exp op = *(e);
		exp a2 = next(child(op));

		/* + and * can have >2 parameters
		  - make them diadic - can do better
		  a+exp => let x = exp in a+x */
		if (!a2->last ) {
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
			*(e) = nd;
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
			sh(ss)  = sh(stare);

			*e = ss;
			return scan (e, at);
		}

		str = scan (arg, at);
		return maxneeds (str, shapeneeds (sh (* (e))));
	}

	case proc_tag:
	case general_proc_tag: {
		exp *bexp;
		exp *bat;
		needs body;
		exp stare = *e;

		callerfortr = optim & OPTIM_TAIL && !proc_has_setjmp(stare) && !proc_has_alloca(stare) &&
		              !proc_has_lv(stare) && !proc_uses_crt_env(stare);

		maxfix = PIC_code ? 15 : 16;
		/* ie $2-$15, $24 & $25 if not PIC */
		maxfloat = 10;
		gen_call = (stare->tag == general_proc_tag);
		callee_size = 0;
		stparam = 0;
		fixparam = 4;
		floatparam = 6;
		nonevis = true;

		bexp = & child(*e);
		bat = bexp;
		body = scan (bexp, &bat);

		/* scan the body of the proc */
		setframe_flags(*e, (body.propneeds & anyproccall) == 0);
		if (gen_call || Has_fp) {
			callee_size += 128;
		}

		/* should never require this in reg in C */
		return body;
	}

	default:
		printf("case %d not covered in needs scan\n", (*e)->tag);
		/* NB should call failer */
		return zeroneeds;
	}
}

