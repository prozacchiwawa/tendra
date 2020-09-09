/*
 * Copyright 2002-2011, The TenDRA Project.
 * Copyright 1997, United Kingdom Secretary of State for Defence.
 *
 * See doc/copyright/ for the full copyright terms.
 */

/*
 * This file defines the routines which implement the TDF-to-TDF optimising
 * transformation which removes constant expressions from program fragments
 * (typically loops).
 *
 * The type maxconst returns information about the expression under
 * consideration. The field self is true if the expression as a whole is
 * constant within the program fragment under consideration. If the entire
 * expression is not constant (self is false) then the field cont is a list
 * of the sub-expressions which are constant within the specified region.
 *
 * The type maxconst is defined in consttypes.h
 *
 * The principal procedures defined here are mc_list, repeat_consts and
 * return_repeats. They are described below.
 *
 * Also used externally is intnl_to.
 */

#include <shared/bool.h>
#include <shared/check.h>
#include <shared/error.h>
#include <shared/xalloc.h>

#include <tdf/shape.h>
#include <tdf/tag.h>

#include <reader/exp.h>
#include <reader/externs.h>
#include <reader/basicread.h>

#include <construct/const.h>
#include <construct/exp.h>
#include <construct/install_fns.h>
#include <construct/installglob.h>
#include <construct/is_worth.h>
#include <construct/me_fns.h>

#include <flpt/flpt.h>

#include <main/flags.h>

#include <refactor/refactor.h>
#include <refactor/refactor_id.h>
#include <refactor/const.h>

#ifdef TDF_DIAG4
#include <diag4/dg_aux.h>
#endif

#define MAXUSE 16
#define VERYBIGUSAGE 100
#define MEMINC 64

/* the entire expression is constant */
static maxconst self_const = {
	true, NULL
},

/* no part of the expression is constant */
no_consts = {
	false, NULL
};

/* All variables initialised */

typedef struct _memlist {
	exp dec;
	int res;
	struct _memlist *next;
}   memlist;

/* no need to init mem and fmem */
static memlist *mem  = NULL; /* current list of frequent identifiers */
static memlist *fmem = NULL; /* list of free cells */

static prop cond_flag = 0;	/* pushed value */
/* 1 => inside cond(..);  2 => after test() in cond() */

static int arg_is_reff;		/* no init needed */
/* arg is reffield, so contents doesn't need guarding */

#define globmax 100
static int glob_index;
static exp glob_dest[globmax];

static int has_lj_dest;

exp get_repeats(void);

static int
find_glob(exp e)
{
	int i;

	for (i = 0; i < glob_index; i++) {
		if (glob_dest[i] == e) {
			return 1;
		}
	}

	return 0;
}

/*
 * ret_constlist returns the elements of a constants-list
 */
static void
ret_constlist(exp head)
{
	exp limit, t, n;

	if (head == NULL) {
		return;
	}

	limit = pt(head);
	t = child(head);
	retcell(head);
	while (t != limit) {
		n = next(t);
		retcell(t);
		t = n;
	}
	retcell(t);
}

/* declaration - max_const and mc_list are mutually recursive */
static maxconst max_const(exp, exp, int);

/*
 * mc_list examines a list of expressions, and for each of them
 * extracts the largest expressions which are constant within the
 * region of interest.
 *
 *  whole   the program region under consideration
 *  e       the first expression in the list. Expressions are
 *          linked via the next field.
 *  ass_ok  all assignments in this region are to simple unaliassed variables
 *  good    if this is true AND all the expressions in the list are
 *          constant then the value self_const is returned.
 */
static
maxconst mc_list(exp whole, exp e, int ass_ok, int good)
{
	exp t = e;
	int contin = true;
	maxconst mc, result;

	result.self = good;
	result.cont = NULL;

	do {
		/* NB - t may be killed within max_const (offset_mult) */
		/* so remember next one in list */
		exp next_t = next(t);
		if (t->last) {
			contin = false;
		}

		mc = max_const(whole, t, ass_ok);
		if (mc.self) {
			/* the whole of t is constant */
			/* make a list element */
			exp w = getexp(f_bottom, NULL, false, t, NULL, cond_flag, 0, 0);
			if (result.cont == NULL) {
				/* first item - start a list */
				result.cont = getexp(f_bottom, NULL, false, w, w, 0,  0, 0);
			} else {
				/* add this to list */
				next(pt(result.cont)) = w;
				pt(result.cont) = w;
			}
		} else {
			result.self = false; /* some part of e is not constant */
			if (mc.cont != NULL) {
				/* but t has constants in it */
				if (result.cont != NULL) {
					/* add them to list */
					next(pt(result.cont)) = child(mc.cont);
					pt(result.cont) = pt(mc.cont);
					retcell(mc.cont);
				} else {
					/* list was empty - start list */
					result.cont = mc.cont;
				}
			}
		}

		t = next_t;
	} while (contin);

	if (result.self) {
		ret_constlist(result.cont);
		return self_const;
	}

	return result;
}

/*
 * intnl_to returns true if part is contained in whole
 */
int
intnl_to(exp whole, exp part)
{
	exp q = part;

	while (q != whole && q != NULL && q->tag != hold_tag &&
	       q->tag != hold2_tag && (q->tag != ident_tag || !isglob(q))) {
		q = father(q);
	}

	return q == whole;
}

/* heavily used idents are kept in lookup list */

static int not_assigned_to(exp, exp);

/*
 * this replaces used_in with stronger test - see changes in assigns_alias
 */
static int
not_ass2(exp vardec, exp piece)
{
	exp t;
	exp q;
	exp upwards;

	t = pt(vardec);
	upwards = t;

	do {
		/* test each use of the identifier */
		q = t;
		while (q != NULL && q != piece && q != vardec &&
		       q->tag != rep_tag && (q->tag != ident_tag || !isglob(q))) {
			upwards = q;
			q = next(q);
		}

		if (q != NULL && q != piece && q->tag == rep_tag) {
			/* q has got to a repeat, so */
			/* scan up repeat_list structure for holder of piece */
			exp h = pt(q), hp = pt(piece);

			while (h != NULL && h != hp) {
				h = next(h);
			}

			if (h == hp) {
				/* q was within piece */
				q = piece;
				upwards = child(q);
				while (!upwards->last) {
					upwards = next(upwards);
				}
			} else {
				q = NULL;
			}
		}

		/* ascend from the use until we reach either vardec or piece */
		if (q == piece && upwards->last) {
			/* the use was in piece */
			if (isreallyass(t)) {
				return false;
			}

			if (!t->last && next(t)->last &&
			    (next(next(t))->tag == ass_tag || next(next(t))->tag == assvol_tag)) {
				/* the use was an assignment */
				return false;
			}

			if (!t->last && next(t)->last && next(next(t))->tag == ident_tag) {
				/* use in declaration */
				if (!isvar(next(next(t))) && !not_assigned_to(next(next(t)), next(t))) {
					return false;
				}
			} else {
				exp dad = father(t);

				if (dad->tag == addptr_tag && child(dad) == t) {
					/* use in subscript .... */
					if (!dad->last && next(dad)->last &&
					    (next(next(dad))->tag == ass_tag ||
					     next(next(dad))->tag == assvol_tag))
					{
						/* the use was an assignment */
						return false;
					}

					if (!dad->last && next(dad)->last &&
					    next(next(dad))->tag == ident_tag)
					{
						/* ... which is identified */
						if (!isvar(next(next(dad))) &&
						    !not_assigned_to(next(next(dad)), next(dad)))
						{
							return false;
						}
					}
				}
			}
		}

		t = pt(t);
	} while (t != NULL);

	return true;
}

static int
not_assigned_to(exp vardec, exp body)
{
	memlist *ptr;

	if (no(vardec) > VERYBIGUSAGE) {
		return false;
	}

	/*
	 * When a variable is used many times the result from not_ass2
	 * is saved in an ordered list to avoid n-squared run-times.
	 */

	/* default case - identifier not heavily used */
	if (no(vardec) <= MAXUSE) {
		return not_ass2(vardec, body);
	}

	/*
	 * Note: memory is cleared after each repeat is processed,
	 * so any in memory refer to the current repeat.
	 */

	/* is this declaration known? */
	for (ptr = mem; ptr != NULL && (ptr->dec) != vardec; ptr = ptr->next)
		;

	if (ptr == NULL) {
		memlist **pp = &mem;

		/* insert with heavier used decs first */
		while (*pp != NULL && no((*pp) ->dec) > no(vardec)) {
			pp = & ((*pp) ->next);
		}

		if (fmem == NULL) {
			/* add some cells onto the free list */
			memlist **fpp = &fmem;
			int i;

			*fpp = xcalloc(MEMINC, sizeof(memlist));
			for (i = 0; i < MEMINC; ++i) {
				(*fpp) ->next = (*fpp) + 1;
				fpp = & ((*fpp) ->next);
			}

			*fpp = NULL;
		}

		/* get a cell from the free list */
		ptr  = fmem;
		fmem = ptr->next;

		/* remember this vardec */
		ptr->dec = vardec;
		ptr->res = not_ass2(vardec, body);

		/* put cell into mem list */
		ptr->next = *pp;
		*pp = ptr;
	}

	return ptr->res;
}

/*
 * max_const extracts the largest expressions which are constant within
 * the region of interest.
 *
 *   whole   the program region under consideration
 *   e       the expression under consideration
 *   ass_ok  all assignments in this region are to simple unaliassed variables
 */
static maxconst
max_const(exp whole, exp e, int ass_ok)
{
	switch (e->tag) {
	case labst_tag:
		return mc_list(whole, next(child(e)), ass_ok, false);

	case contvol_tag:
	case case_tag:
	case goto_tag:
	case apply_general_tag:
	case tail_call_tag:
		return no_consts;

	case fdiv_tag: {
		maxconst mc;
		maxconst mct;

		mc = max_const(whole, next(child(e)), ass_ok);
		mct = mc_list(whole, child(e), ass_ok, optop(e));
		if (mct.self) {
			return mct;
		}

		if (mc.self && !strict_fl_div && optop(e)) {
			flpt f = new_flpt();
			exp funit;
			exp temp1;
			exp temp2;

			flt_copy(flptnos[fone_no], &flptnos[f]);
			funit = getexp(sh(e), NULL, 0, NULL, NULL, 0, f, real_tag);
			temp1 = me_b3(sh(e), funit, next(child(e)), fdiv_tag);
			temp2 = me_b3(sh(e), child(e), temp1, fmult_tag);

#ifdef TDF_DIAG4
			dgf(temp2) = dgf(e);
#endif
			replace(e, temp2, temp2);
			return max_const(whole, temp2, ass_ok);
		} else {
			return mct;
		}
	}

	case cond_tag: {
		prop old_cond_flag = cond_flag;
		maxconst mc;

		if (cond_flag == 0) {
			cond_flag = 1;
		}

		mc = mc_list(whole, child(e), ass_ok, false);
		cond_flag = old_cond_flag;

		return mc;
	}

	case test_tag: {
		maxconst mc;

		mc = mc_list(whole, child(e), ass_ok, false);
		if (cond_flag == 1) {
			cond_flag = 2;
		}

		return mc;
	}

	case val_tag:
	case proc_tag:
	case env_offset_tag:
	case general_env_offset_tag:
		return self_const;

	case name_tag:
		if (intnl_to(whole, child(e))) {
			/* internal const - may change */
			return no_consts;
		} else {
			/* external constant */
			return self_const;
		}

	case cont_tag:
		if ((child(e)->tag == name_tag) && isvar(child(child(e)))) {
			/* so e is extracting the contents of a variable */
			exp var = child(child(e));

			/*
			 * variable declared external to whole, and NEVER assigned to in whole
			 */
			if (!intnl_to(whole, var) && (not_assigned_to(var, whole)) && ass_ok) {
				if (iscaonly(var)) {
					return self_const;
				}
				if (isglob(var) && !find_glob(var)) {
					return self_const;
				}
			}

			return no_consts;
		} else {
			return mc_list(whole, child(e), ass_ok, ass_ok);
		}

	case plus_tag:
	case and_tag:
	case or_tag:
	case xor_tag:
	case mult_tag: {
		maxconst mc;

		mc = mc_list(whole, child(e), ass_ok, optop(e));

		if (mc.cont != NULL && pt(mc.cont) != child(mc.cont) && optop(e)) {
			/* more than 1 item in list */
			exp limit = pt(mc.cont), h = child(mc.cont), arg, this, last_h;
			int arg_count = 0;
			int tot_args = 1;

			for (this = child(e); !this->last; this = next(this)) {
				tot_args++;
			}

			/* remember for which operator these are arguments */
			/* NB - some items may not be args of this operator */
			while (h != NULL) {
				this = child(h);
				arg = child(e);
				while (arg != NULL && arg != this) {
					arg = (arg->last ? NULL : next(arg));
				}
				if (arg != NULL) {
					/* it's an argument of this operator */
					++arg_count;
					pt(h) = e;
					last_h = h;
				}
				h = (h == limit ? NULL : next(h));
			}

			/* remove reference to operator if only 1 arg is const */
			if (arg_count != tot_args && arg_count > 0) {
				SET(last_h);
				pt(last_h) = NULL;
			}
		}

		return mc;
	}

	case addptr_tag: {
		exp p;

		maxconst mc, mx;

		/* find the root pointer */
		for (p = child(e); p->tag == addptr_tag; p = child(p))
			;

		mc = max_const(whole, p, ass_ok);
		ret_constlist(mc.cont);

		if (mc.self) {
			/* root pointer is constant in this context */
			exp c_list = NULL, v_list = NULL, x, cph, *list;

			/* construct list of ALL constant parts */
			/* initial list element will hold const. ptr */
			cph = getexp(f_bottom, NULL, false, NULL, NULL, 0,  0, 0);
			mc.self = false;	/* assume, for moment */
			mc.cont = getexp(f_bottom, NULL, false, cph, cph, 0,  0, 0);

			/* return up the chain, testing the offsets */
			while (p != e) {
				mx = max_const(whole, next(p), ass_ok);
				p = next(p);		/* p is now the offset */

				/* add offset to appropriate list */
				list = (mx.self) ? &c_list : &v_list;
				*list = getexp(NULL, *list, 0, p, NULL, 0,  0, 0);

				if (mx.cont != NULL) {
					/* the offset is not constant, but PARTS of it are */

					/* remove any "negate(...)->tag" */
					exp lim = pt(mx.cont), h = child(mx.cont);
					while (h != NULL) {
						if (child(h)->tag == neg_tag && child(child(h))->tag == name_tag) {
							no(h) = -1;	/* set "done" flag */
						}

						h = (h == lim ? NULL : next(h));
					}

					/* add constant parts to mc */
					next(pt(mx.cont)) = child(mc.cont);
					child(mc.cont) = child(mx.cont);
					retcell(mx.cont);
				}

				p = next(p); /* p is now the next higher operation */
			}

			if (v_list == NULL) {
				/* whole addptr expression is constant */
				/* return c_list elements */
				while (c_list != NULL) {
					x = c_list;
					c_list = next(c_list);
					retcell(x);
				}

				ret_constlist(mc.cont);

				return self_const;
			}

			/*
			 * go down the chain of addptrs, rearranging offsets
			 *
			 * NB - assumes addptr is strictly diadic, so "last"
			 * flags are already correct
			 */

			/* put non-constant offsets at the higher levels */
			while (v_list != NULL) {
				/* put next offset in 2nd argument position */
				x = child(p);
				next(x) = child(v_list);
				next(next(x)) = p;
				p = x;		/* point to 1st argument */

				/* traverse v_list, returning elements */
				x = v_list;
				v_list = next(x);
				retcell(x);
			}

			/* the rest is constant - add it to mc.cont */
			child(cph) = p;

			/* and put constant offsets at the lower levels */
			while (c_list != NULL) {
				/* put next offset in 2nd argument position */
				x = child(p);
				next(x) = child(c_list);
				next(next(x)) = p;
				p = x;		/* point to 1st argument */

				/* traverse c_list, returning elements */
				x = c_list;
				c_list = next(x);
				retcell(x);
			}

			return mc;
		} else {
			return mc_list(whole, child(e), ass_ok, true);
		}
	}

	case offset_mult_tag: {
		exp arg1 = child(e);
		exp arg2 = next(arg1);

		maxconst mc1, mc2;
		shape ofsh = sh(e);
		mc1 = max_const(whole, arg1, ass_ok);
		mc2 = max_const(whole, arg2, ass_ok);

		if (mc1.self && mc2.self) {
			return self_const;
		}

		/*
		 * The offset is const, and arg1 has some constant parts so transform:
		 *   offset_mult((a*b),K)
		 * to:
		 *   offset_mult(a,offset_mult(b,K))
		 * rearranged so that the constant factors are grouped with K so that
		 * the largest possible structure can be extracted as constant
		 */
		if (mc2.self && mc1.cont != NULL) {
			exp klist = NULL, nklist = NULL;
			exp *ref;
			exp m_res;
			int j;

			ret_constlist(mc1.cont);
			if (arg1->tag == mult_tag) {
				exp m_arg = child(arg1);
				/* sort into const and varying args */
				while (m_arg != NULL) {
					mc1 = max_const(whole, m_arg, ass_ok);
					if (mc1.self) {
						/* add to constant operand list */
						klist = getexp(NULL, klist, false, m_arg, NULL,
						               0,  0, 0);
					} else {
						/* add to non-constant operand list */
						nklist = getexp(NULL, nklist, false, m_arg, NULL,
						                0,  0, 0);
						ret_constlist(mc1.cont);
					}
					if (m_arg->last) {
						m_arg = NULL;
					} else {
						m_arg = next(m_arg);
					}
				}

				/* build offset_mult chain with const parts innermost */
				m_res = copy(arg2);
				for (j = 0; j < 2; ++j) {
					exp *list = (j == 0) ? &klist : &nklist;
					/* use klist, and then nklist */
					while (*list != NULL) {
						exp z = *list;
						exp a1 = copy(child(z));
						exp offmul = getexp(ofsh, NULL, false, a1, NULL,
						                    0,  0, offset_mult_tag);
						setnext(a1, m_res);
						a1->last = false;
						setnext(m_res, offmul);
						m_res->last = true;
						m_res = hold_refactor(offmul);
						*list = next(z);
						retcell(z);
					}
				}

				/* insert m_res - kill left overs */
				ref = refto(father(e), e);
				if ((*ref)->last) {
					m_res->last = true;
				} else {
					m_res->last = false;
				}

				next(m_res) = next(*ref);
				*ref = m_res;
				kill_exp(e, e);
			} else {
				m_res = e;
			}

			return mc_list(whole, child(m_res), ass_ok, true);
		}

		/* default action */
		return mc_list(whole, child(e), ass_ok, true);
	}

	default:
		if (child(e) == NULL) {
			return self_const;
		} else {
			return mc_list(whole, child(e), ass_ok, is_a(e->tag) && optop(e));
		}
	}
}

/*
 * do_this_k replaces simple and compound constants in list by uses
 * of a newly declared constant.
 *
 *   kdec    declaration of this new constant
 *   patn    pattern to look for
 *           NB where safe_eval has NOT been used, patn is child(kdec)
 *   list    list of constant expresion holders
 *   limit   last constant holder in list
 */
static void
do_this_k(exp kdec, exp patn, exp list, exp limit)
{
	exp t = list;
	exp arglist = NULL, ap;
	int nargs = 0;

	if (pt(list) != NULL) {
		/* build required argument list */
		exp p = child(patn);
		while (p != NULL) {
			exp arg_h = getexp(NULL, arglist, 0, p, NULL, 0, 0, 0);
			arglist = arg_h;
			++nargs;
			p = (p->last ? NULL : next(p));
		}
	}

	for (;;) {
		if (no(t) == 0) {
			if (pt(t) == NULL && eq_exp(child(t), patn)) {
				/* simple correspondence */
				exp e = child(t);
				exp f = father(e);
				exp tagt = getexp(sh(e), next(e), (int)(e->last),
				                  kdec, pt(kdec), 0,  0, name_tag);
				pt(kdec) = tagt;
				++no(kdec);
#ifdef TDF_DIAG4
				if (diag != DIAG_NONE) {
					dg_extracted(tagt, *(refto(f, e)));
				}
#endif
				*(refto(f, e)) = tagt;
				no(t) = -1;		/* dealt with */
				kill_exp(child(t), child(t));
			} else if (pt(t) != NULL && pt(t)->tag == patn->tag) {
				/* try for complex match - at least the operator is correct */
				/* check errtreat ??? */
				int scan2 = true;
				int matched = 0;
				exp t2 = t, op = pt(t);

				while (matched >= 0 && scan2) {
					if (no(t2) == 0 && pt(t2) == op) {
						/* find match in argument list */
						ap = arglist;

						while (ap != NULL &&
						       (pt(ap) != NULL || !eq_exp(child(t2), child(ap)))) {
							ap = next(ap);
						}

						if (ap == NULL) {
							matched = -1;
						} else {
							pt(ap) = t2;
							++matched;
						}
					}

					if (t2 == limit) {
						scan2 = false;
					} else {
						t2 = next(t2);
					}
				}

				if (matched == nargs) {
					exp prev_arg = NULL, oparg = child(op), cc;
					int last_arg;

					cc = getexp(sh(child(kdec)), op, 1, kdec, pt(kdec), 0, 0, name_tag);
					pt(kdec) = cc;
					++no(kdec);

					while (oparg != NULL) {
						last_arg = (int)oparg->last;

						for (ap = arglist; ap != NULL && child(pt(ap)) != oparg; ap = next(ap))
							;

						if (ap == NULL) {
							/* this is one of the other args of op */
							if (prev_arg == NULL) {
								child(op) = oparg;
							} else {
								next(prev_arg) = oparg;
							}

							oparg->last = false;
							prev_arg = oparg;
						}

						oparg = (last_arg ? NULL : next(oparg));
					}

					/* now add combined constant */
					next(prev_arg) = cc;

					/* mark those dealt with & clear arglist */
					ap = arglist;
					while (ap != NULL) {
						exp deadarg = child(pt(ap));
						no(pt(ap)) = -1;
						child(pt(ap)) = NULL;
						pt(ap) = NULL;
						kill_exp(deadarg, deadarg);
						ap = next(ap);
					}
				}
			}
		}

		if (t == limit) {
			break;
		}

		t = next(t);
	}

	/* return arglist */
	while (arglist != NULL) {
		ap = next(arglist);
		retcell(arglist);
		arglist = ap;
	}
}

/*
 * insert run-time checks on this argument - see safe_eval
 *
 *   e	argument to be tested
 *   esc	label: jump to this if e is:
 *			pointer and nil
 *			numeric and zero
 */
static exp
safe_arg(exp e, exp esc)
{
	exp decl = getexp(sh(e), NULL, 0, e, NULL, 0,  0, ident_tag);
	exp v1, v2, z, s, konst, tst;

	/* make the unsafe value for this shape */
	switch (sh(e)->tag) {
	case ptrhd:
		konst = me_null(sh(e), ptr_null, null_tag);
		break;

	case scharhd:
	case ucharhd:
	case swordhd:
	case uwordhd:
	case slonghd:
	case ulonghd:
	case s64hd:
	case u64hd:
		konst = getexp(sh(e), NULL, 0, NULL, NULL, 0,  0, val_tag);
		break;

	case shrealhd:
	case realhd:
	case doublehd: {
		flpt f = new_flpt();
		int i;
		for (i = 0; i < MANT_SIZE; ++i) {
			(flptnos[f].mant)[i] = 0;
		}
		flptnos[f].exp = 0;
		flptnos[f].sign = 0;
		konst = getexp(sh(e), NULL, 0, NULL, NULL, 0, f, real_tag);
		break;
	}

	case offsethd:
		konst = f_offset_zero(f_alignment(sh(e)));
		break;

	default:
		SET(konst);
		error(ERR_INTERNAL, "impossible shape found");
	}

	v1 = getexp(sh(e), NULL, 0, decl, pt(decl), 0,  0, name_tag);
	pt(decl) = v1;
	++no(decl);

	v2 = getexp(sh(e), NULL, 1, decl, pt(decl), 0,  0, name_tag);
	pt(decl) = v2;
	++no(decl);

	tst = getexp(f_top, NULL, 0, v1, esc, 0, 0, test_tag);
	settest_number(tst, f_not_equal);
	++no(child(esc));
	setnext(v1, konst);
	tst = hc(tst, konst);

	z = getexp(f_top, v2, 0, tst, NULL, 0,  0, 0);
	setnext(tst, z);
	tst->last = true;

	s = getexp(sh(e), decl, 1, z, NULL, 0,  0, seq_tag);
	setnext(e, s);
	e->last = false;
	s = hc(s, v2);

	return hc(decl, s);
}

/*
 * safe_eval	ensure that the evaluation of e cannot fail
 *
 * Insert run-time checks into the evaluation of this expression - this is only
 * used when a constant is extracted from inside a conditional inside a loop.
 * Where this happens, the extraction of the constant and its unconditional
 * evaluation outside the loop can result in program failure when the program
 * would not otherwise have failed.
 *
 * This should be called with "escape_route" as NULL - this marks the
 * outermost call of safe_eval, and causes the contruction of a label for
 * the code to escape to if a "dangerous" value is encountered during
 * evaluation of the constant. If at the end of the outermost call the label
 * has been used, then there is a possibility of failure during evaluation
 * and code is generated to supply a "safe" value when the label is reached.
 *
 * dangerous operations are:
 *	contents of NIL
 *	reffield of NIL (indirectly, when its contents are taken)
 *	division by zero (any variety; includes mod and rem)
 * Note that checking the result of reffield for NIL is a waste of time
 * since any offset from NIL will make the result different from NIL.
 *
 *   e		expression being evaluated
 *   escape_route	label: jump to this if evaluation would fail
 */
static exp
safe_eval(exp e, exp escape_route)
{
	exp esc_lab, res;

	if (escape_route == NULL) {
		/* this is outermost call - construct escape label */
		exp z   = getexp(f_top, NULL, 0, NULL, NULL, 0, 0, clear_tag);
		esc_lab = getexp(sh(e), NULL, 0, z, NULL, 0, 0, labst_tag);
	} else {
		esc_lab = escape_route;
	}

	switch (e->tag) {
	case ident_tag:
	case cond_tag:
	case rep_tag:
	case solve_tag:
	case case_tag:
		error(ERR_INTERNAL, "unanticipated component");
		return NULL;

	case name_tag:
	case env_offset_tag:
	case general_env_offset_tag:
	case real_tag:
		res = copy(e);
		break;

	case div0_tag:
	case div1_tag:
	case div2_tag:
	case fdiv_tag:
	case mod_tag:
	case rem2_tag:
	case rem0_tag:
	case offset_div_tag:
	case offset_div_by_int_tag: {
		exp arg1 = safe_eval(child(e), esc_lab);
		exp arg2 = safe_eval(next(child(e)), esc_lab);

		res = copyexp(e);
		setchild(res, arg1);
		arg2 = safe_arg(arg2, esc_lab);
		setnext(arg1, arg2);
		arg1->last = false;
		res = hc(res, arg2);
		break;
	}

	case cont_tag: {
		exp arg = child(e);

		if (arg->tag == name_tag &&
		    (isglob(child(arg)) || isvar(child(arg)))) {
			res = copy(e);
		} else {
			arg = safe_eval(arg, esc_lab);
			if (!arg_is_reff) {
				arg = safe_arg(arg, esc_lab);
			}
			res = copyexp(e);
			setchild(res, arg);
			res = hc(res, arg);
		}
		break;
	}

	case reff_tag: {
		exp arg = child(e);

		if (arg->tag == name_tag && isglob(child(arg))) {
			res = copy(e);
		} else {
			arg = safe_eval(arg, esc_lab);
			if (!arg_is_reff) {
				arg = safe_arg(arg, esc_lab);
			}
			res = copyexp(e);
			setchild(res, arg);
			res = hc(res, arg);
		}
		break;
	}

	default: {
		exp k = copyexp(e);
		exp arg = child(e);
		exp p;

		if (arg == NULL) {
			res = k;
			break;
		}

		p = safe_eval(arg, esc_lab);
		setchild(k, p);
		while (!arg->last) {
			exp safe = safe_eval(next(arg), esc_lab);
			setnext(p, safe);
			p->last = false;
			p = next(p);
			arg = next(arg);
		}
		res = hc(k, p);
		break;
	}
	}

	arg_is_reff = (e->tag == reff_tag);
	if (escape_route != NULL) {
		/* this was an inner call */
		return res;
	}

	if (no(child(esc_lab)) == 0) {
		/* the escape route is not used - inherently safe */
		retcell(child(esc_lab));
		retcell(esc_lab);
		return res;
	} else {
		/* the escape route was used - construct conditional */
		exp cond = getexp(sh(e), NULL, 0, res, NULL, 0,  0, cond_tag);
		exp safe;

		safe = getexp(sh(e), NULL, 1, NULL, NULL, 0,  0, clear_tag);
		setnext(child(esc_lab), safe);
		IGNORE hc(esc_lab, safe);
		setnext(res, esc_lab);
		res->last = false;
		IGNORE hc(cond, esc_lab);
		return cond;
	}
}

static void
look_for_caonly(exp e)
{
	if (e->tag == name_tag) {
		if (isvar(child(e))) {
			clearcaonly(child(e));
		}
		return;
	}

	if (e->tag == addptr_tag) {
		look_for_caonly(child(e));
	}

	if (e->tag == seq_tag || e->tag == ident_tag) {
		look_for_caonly(next(child(e)));
	}
}

/*
 *  issn         loop is child(rf) else next(rf)
 *  rf           EXP holding loop
 *  list_head    exp containing list of constant expressions
 *               this must not be empty
 */
static int
extract_consts(int issn, exp rf, exp list_head)
{
	exp val;
	int changed = 0;		/* result; will be true if we make a change */
	exp t = child(list_head);	/* first in list */
	exp limit = pt (list_head);	/* last in list */
	int contin = true;

	do {
		if (issn) {
			val = child(rf);
		} else {
			val = next(rf);
		}

		if (no(t) != 0) {
			/* this has been dealt with previously - just * check for end */
			contin = (t != limit);
		} else {
			/* this has not been absorbed by a previous constant */

			exp e;
			int force = 0;

			if (pt(t) == NULL) {
				/* simple constant - no nexts */
				exp f;
				e = child(t);
				f = father(e);

				/* ?????????????????? */
				if (!e->last && next(e)->last && (f->tag == ident_tag) && !isvar(f)) {
					/*
					 * This is an in-register constant declaration so remove the
					 * force register bit from f so that it becomes a simple renaming
					 */
					clearusereg(f);

					/*
					 * ...and set the force register bit for the outer declaration.
					 */
					force = 1;
				}
#ifdef TDF_DIAG4
				e = copy_dg_separate(e); /* original may remain in use */
#else
				e = copy(e);
#endif
				/* so child(t) can be killed or used in declaration */
			} else {
				/* the next few consts are args of the same operator */
				exp op = pt(t), new_c, prev = NULL, c_arg = NULL, t2 = t;
				int scan = true;

				new_c = copyexp(op);

				while (scan) {
					if (no(t2) == 0 && pt(t2) == op) {
#ifdef TDF_DIAG4
						c_arg = copy_dg_separate(child(t2)); /* original may remain in use */
#else
						c_arg = copy(child(t2));
#endif
						if (prev == NULL) {
							child(new_c) = c_arg;
						} else {
							next(prev) = c_arg;
							prev->last = false;
						}

						prev = c_arg;
					}

					if (t2 == limit) {
						scan = false;
					} else {
						t2 = next(t2);
					}
				}

				e = hc(new_c, c_arg);
			}

			if (is_worth(e)) {
				/* declare new constant */
				exp konst;
				exp newdec;
				int kill_e = false;

#ifdef TDF_DIAG4
				if (diag != DIAG_NONE) {
					strip_dg_context(e);
				}
#endif

				if (t->props > 1) {
					/*
					 * This const. is in a conditional in the loop; ensure that extraction
					 * from loop does not cause a failure.
					 */
					kill_e = true;
					konst = safe_eval(e, NULL);
				} else {
					konst = e;
				}

				newdec = getexp(sh(val), next(val),
					(int) (val->last), konst, NULL, 0,  0, ident_tag);
				if (has_lj_dest) {
					setvis(newdec);
				}

				if (force && isvis(father(e))) {
					setvis(newdec);
				} else {
#ifdef TRANS_MIPS
					setusereg(newdec);
#else
					if (force) {
						setusereg(newdec);
					}
#endif
				}

				if (sh(konst)->tag == ptrhd) {
					look_for_caonly(konst);
				}

				next(konst) = val;
				konst->last = false;

				next(val) = newdec;
				val->last = true;

				if (issn) {
					child(rf) = newdec;
				} else {
					next(rf) = newdec;
				}

#ifdef TDF_DIAG4
				if (diag != DIAG_NONE) {
					make_optim_dg(DGD_EXTRACT, newdec);
				}
#endif
				do_this_k(newdec, e, t, limit);
				if (kill_e) {
					kill_exp(e, e);
				}

				changed = 1; /* have made a change */
			} else {
				kill_exp(e, e);
			}
		}

		if (t == limit) {
			/* that was the last in the list */
			contin = false;
		} else {
			exp n = next(t);
			retcell(t);
			t = n;
		}
	} while (contin);

	retcell(t);

	retcell(list_head);
	return changed;
}

static int
named_dest(exp dest)
{
	switch (dest->tag) {
	case name_tag:
		if (!isvar(child(dest))) {
			if (child(child(dest)) != NULL) {
				return named_dest(child(child(dest)));
			}

			return false;
		}

		if (iscaonly(child(dest))) {
			return true;
		}

		if (isglob(child(dest))) {
			if (find_glob(child(dest))) {
				return true;
			}

			if (glob_index == globmax) {
				return false;
			}

			glob_dest[glob_index++] = child(dest);

			return true;
		}

		return false;

	case addptr_tag:
	case reff_tag:
		/* Should we look at next child to see if it contains an assignment ??? */
		return false;

	default:
		return false;
	}
}

/*
 * Scans e - returns true if any aliased variables are assigned to.
 */
static int
assigns_alias(exp e)
{
	switch (e->tag) {
	case assvol_tag:
	case ass_tag: {
		exp dest = child(e);

		if (!named_dest(child(e))) {
			/* LHS may be aliassed */
			return true;
		} else {
			/* check RHS for assignments */
			return assigns_alias(next(dest));
		}
	}

	case name_tag:
	case env_offset_tag:
	case general_env_offset_tag:
		return false;

	case case_tag:
		return assigns_alias(child(e)); /* NB - must only look at first child */

	case bfass_tag:
	case apply_tag:
		return true; /* pessimist! */

	default: {
		int aa = false;
		exp s = child(e);

		while ((s != NULL) && !aa) {
			aa = assigns_alias(s);
			if (aa || s->last) {
				s = NULL;
			} else {
				s = next(s);
			}
		}

		return aa;
	}
	}
}

/*
 * Scans e - returns true if any label may be long jump destination
 */
static int
scan_for_lv(exp e)
{
	switch (e->tag) {
	case make_lv_tag:
		return true;

	case name_tag:
	case env_offset_tag:
	case general_env_offset_tag:
		return false;

	default: {
		int aa = false;
		exp s = child(e);

		while ((s != NULL) && !aa) {
			aa = scan_for_lv(s);
			if (aa || s->last) {
				s = NULL;
			} else {
				s = next(s);
			}
		}

		return aa;
	}
	}
}

/*
 * Calls extract_consts on each element of the list of repeat loops.
 */
void
repeat_consts(void)
{
	exp reps;

	for (reps = get_repeats(); reps != NULL; reps = pt(reps)) {
		exp loop;
		exp sts;
		int no_alias;
		maxconst mx;
		exp consts;
		exp rr;
		int sn;
		exp fa;
		memlist **mptr;

		if (child(reps) == NULL) {
			continue;
		}

		if (child(reps)->tag != rep_tag) {
			continue;
		}

		if (no(reps) >= max_loop_depth) {
			continue;
		}

		loop = child(reps);
		sts = next(child(loop));

		/* put old identifier memory list into its free list */
		mptr = &mem;

		glob_index = 0;
		no_alias = !assigns_alias(sts);
		while (*mptr != NULL) {
			mptr = & ((*mptr) ->next);
		}

		*mptr = fmem;
		fmem  = mem;
		mem   = NULL;

		mx = mc_list(loop, sts, no_alias, false);

		consts = mx.cont;
		/* NB - false forces a list to be produced */

		if (no_alias) {
			set_noalias (reps);	/* preserve for forall processing */
		}

		if (consts == NULL) {
			continue;
		}

		fa = father(loop);

		if (child(fa) == loop) {
			sn = 1;
			rr = fa;
		} else {
			sn = 0;
			rr = child(fa);
			while (next(rr) != loop && !rr->last) {
				rr = next(rr);
			}
		}

		if (sn || next(rr) == loop) {
			while (fa->tag != proc_tag && fa->tag != general_proc_tag &&
				   fa->tag != hold_tag && fa->tag != hold2_tag) {
				fa = father(fa);
			}

			if (fa->tag == hold_tag || fa->tag == hold2_tag) {
				continue;
			}

			if (proc_uses_crt_env(fa)) {
				has_lj_dest = scan_for_lv(sts);
			} else {
				has_lj_dest = 0;
			}

			IGNORE extract_consts(sn, rr, consts);
		}
	}
}

/*
 * Calculates maximum distance of every repeat from a leaf node
 * (this allows repeat processing to be restricted to inner loops)
 *
 * Returns the repeat_list
 */
exp
get_repeats(void)
{
	if (repeat_list != NULL && !is_dist(repeat_list)) {
		exp reps;

		for (reps = repeat_list; reps != NULL; reps = pt(reps)) {
			int dist;
			exp sup;

			if (no(reps) != 0) {
				continue;
			}

			/* this is a leaf node */
			/* no(x) is used in dexp to count directly nested loops */
			dist = 0;
			sup = reps;

			do {
				set_dist (sup);	/* no(x) is now max dist to leaf */
				no(sup) = dist;
				if (child(sup) != NULL && child(sup)->tag == rep_tag) {
					++dist;		/* only repeats are significant */
				}
				sup = next(sup);	/* go to enclosing repeat */
			} while (sup != NULL && (!is_dist(sup) || no(sup) < dist));
		}
	}

	return repeat_list;
}

/*
 * Returns the storage used by repeat_list.
 */
void
return_repeats(void)
{
	exp reps, next;

	for (reps = repeat_list; reps != NULL; reps = next) {
		next = pt(reps);
		retcell(reps);
	}

	repeat_list = NULL;
}

