/*
 * Copyright 2011, The TenDRA Project.
 * Copyright 1996, United Kingdom Secretary of State for Defence.
 *
 * See doc/copyright/ for the full copyright terms.
 */

/*
 * Scans through the program and puts all the arguments of operations
 * into a suitable 68000 operand form.
 */

#include <stdlib.h>

#include <shared/bool.h>
#include <shared/check.h>

#include <local/exp.h>

#include <tdf/shape.h>
#include <tdf/tag.h>

#include <reader/exp.h>

#include <construct/exp.h>
#include <construct/install_fns.h>

#ifndef tdf3
#include "68k_globals.h"
#include "special_exps.h"
#endif

void scan(bool, exp, exp);

/*
 * MACROS TO SET OR GET THE SON OR NEXT
 */
#define assexp(I, P, V)	if (I) setchild(P, V); else setnext(P, V)
#define contexp(I, P)	((I) ? child(P): next(P))

/*
 * Transform a non-bit offset into a bit offset.
 * (borrowed from trans386)
 */
static void
make_bitfield_offset(exp e, exp pe, int spe, shape sha)
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
 * This routine inserts an identity declaration of x at to and replaces
 * x by a use of this identity.
 */
static void
cca(bool sto, exp to, bool sx, exp x)
{
	exp d, a, id, tag;
	d = contexp(sx, x);

#ifndef tdf3
	if (d->tag == caller_tag) {	/* position sensitive */
		cca(sto, to, 1, d);
		return;
	}
#endif

	d = contexp(sx, x);
	a = contexp(sto, to);

	id  = getexp(sh(a), next(a), a->last, d, NULL, 0, 1L, ident_tag);
	tag = getexp(sh(d), next(d), d->last, id, NULL, 0, 0L, name_tag);

	pt(id) = tag;
	d->last = false;

	if (d != a) {
		next(d) = a;
		next(a) = id;
		a->last = true;
		assexp(sto, to, id);
		assexp(sx, x, tag);
	} else {
		next(d) = tag;
		next(tag) = id;
		tag->last = true;
		d->last = false;
		assexp(sto, to, id);
	}
}

/*
 * Keeping the same to, cc scans along the next list e, applying cca to
 * introduce an identity declaration when doit is 1.  It keeps count as
 * the index position along the list in order to pass it to doit.  If it
 * uses cca it scans the resulting declaration, using the same to.  If it
 * doesn't, it scans the list element, still using the same to.  This keeps
 * all operations in the same order.
 */
static void
cc(bool sto, exp to, bool se, exp e, bool(*doit)(exp, int), int count)
{
	exp ec = contexp(se, e);

	if (ec->last) {
		if (doit(ec, count)) {
			cca(sto, to, se, e);
			ec = contexp(sto, to);
			scan(1, ec, child(ec));
		} else {
			scan(sto, to, ec);
		}
	} else {
		cc(sto, to, 0, ec, doit, count + 1);
		ec = contexp(se, e);
		if (doit(ec, count)) {
			cca(sto, to, se, e);
			ec = contexp(sto, to);
			scan(1, ec, child(ec));
		} else {
			scan(sto, to, ec);
		}
	}
}

/*
 * This routine is the same as cca, but forces the declaration into
 * a register.
 */
static void
ccp(bool sto, exp to, bool sx, exp x)
{
	exp xc = contexp(sx, x);
	exp toc;

	if (xc->tag != name_tag || !isusereg(child(xc))) {
		cca(sto, to, sx, x);
		toc = contexp(sto, to);
		setusereg(toc);
		scan(1, toc, child(toc));
	}
}

/*
 * IS THE EXP e AN OPERAND?
 */
static bool
is_opnd(exp e)
{
	switch (e->tag) {
	case name_tag: {
		exp s = child(e);
		return !isvar(s) && (child(child(e)) != NULL) &&
		       !isparam(child(child(e)));
	}

	case val_tag:
	case real_tag:
	case env_size_tag:
	case general_proc_tag:
	case proc_tag:
	case cont_tag:
	case string_tag:
	case null_tag:
		return 1;
	}

	return 0;
}

/*
 * CHECK THE POINTER ARGUMENT OF AN ADDPTR
 */
static void
ap_arg1(bool sto, exp to, bool sa, exp a, bool b)
{
	exp ac = contexp(sa, a);

	if (!b && ac->tag == cont_tag && child(ac)->tag == name_tag &&
	    isvar(child(child(ac)))) {
		return;
	}

	if (!b && ac->tag == name_tag) {
		return;
	}

	/* The pointer has to go into a register */
	ccp(sto, to, sa, a);
}

/*
 * CHECK THE INTEGER ARGUMENT OF AN ADDPTR
 */
static void
ap_argsc(bool sto, exp to, bool se, exp e, int sz, bool b)
{
	exp ec = contexp(se, e);
	exp p = child(ec);
	exp a = next(p);
	exp temp;

	/* Check for multiplication by constant scale factor */
	if (a->tag == offset_mult_tag && next(child(a))->tag == val_tag) {
		long k = no(next(child(a)));
		if ((k == 8 || k == 16 || k == 32 || k == 64) && k == sz) {
			ccp(sto, to, 1, a);
			ap_arg1(sto, to, 1, ec, b);
			return;
		}

	}

	if (sz == 8) {
		ccp(sto, to, 0, child(ec));
		ap_arg1(sto, to, 1, ec, b);
		return;
	}

	if (b) {
		ccp(sto, to, se, e);
		return;
	}

	cca(sto, to, se, e);
	temp = contexp(sto, to);
	scan(1, temp, child(temp));
}

/*
 * CHECK THE ARGUMENT OF A CONT OR THE DESTINATION OF AN ASSIGN
 */
static void
cont_arg(bool sto, exp to, exp e, shape sa)
{
	unsigned char n = child(e)->tag;
	if (n == name_tag) {
		return;
	}

	if (n == cont_tag) {
		exp s = child(child(e));
		if (s->tag == name_tag &&
		    (isvar(child(s)) || isglob(child(s)) || isusereg(child(s)))) {
			return;
		}

		if (s->tag == reff_tag && child(s)->tag == name_tag &&
		    (isvar(child(child(s))) || isglob(child(child(s))) ||
		     isusereg(child(child(s))))) {
			return;
		}

		ccp(sto, to, 1, e);
		return;
	}

	if (n == reff_tag) {
		exp s = child(e);
		if (child(s)->tag == name_tag && isusereg(child(child(s)))) {
			return;
		}

		if (child(s)->tag == addptr_tag) {
			ap_argsc(sto, to, 1, s, shape_size(sa), 1);
			return;
		}

		ccp(sto, to, 1, s);
		return;
	}

	if (n == addptr_tag) {
		ap_argsc(sto, to, 1, e, shape_size(sa), 0);
		return;
	}

	ccp(sto, to, 1, e);
}

/*
 * DOIT ROUTINE, IS t NOT AN OPERAND?
 */
static bool
notopnd(exp t, int i)
{
	return i >= 0 && !is_opnd(t);
}

#ifndef tdf3
static int scan_for_alloca(exp);

static int
scan_alloc_args(exp s)
{
	if (scan_for_alloca(s)) {
		return 1;
	}

	if (s->last) {
		return 0;
	}

	return scan_alloc_args(next(s));
}

static int
scan_for_alloca(exp t)
{
	switch (t->tag) {
	case local_free_all_tag:
	case local_free_tag:
	case last_local_tag:
	case alloca_tag:
	case make_lv_tag:
		return 1;

	case case_tag:
		return scan_for_alloca(child(t));

	case labst_tag:
		return scan_for_alloca(next(child(t)));

	case env_offset_tag:
	case string_tag:
	case name_tag:
		return 0;

	case apply_general_tag:
		if call_is_untidy(t) {
			return 1;
		}

		return scan_alloc_args(child(t));

	default:
		if (child(t) == NULL) {
			return 0;
		}

		return scan_alloc_args(child(t));
	}
}

static bool
no_alloca(exp t, int i)
{
	UNUSED(i);
	return scan_for_alloca(t);
}

#endif

/*
 * APPLY cc, DOING IT WITH OPERANDS
 */
static void
all_opnd(bool sto, exp to, exp e)
{
#if 0
	if (!next(child(e))->last) {
		/* Operation has more than two parameters.  Make it diadic */
		exp opn = getexp(sh(e), e, 0, next(child(e)), NULL, 0, 0, e->tag);
		exp nd = getexp(sh(e), next(e), e->last, opn, NULL, 0, 1, ident_tag);
		exp id = getexp(sh(e), e, 1, nd, NULL, 0, 0, name_tag);

		pt(nd) = id;
		next(child(e)) = id;
		e->last = true;
		next(e) = nd;

		while (!next(child(e))->last) {
			next(child(e)) = next(next(child(e)));
		}

		next(next(child(e))) = opn;
		e = nd;
		scan(sto, e, e);
	}
#endif

	cc(sto, to, 1, e, notopnd, 1);
}

/*
 * IS e ASSIGNABLE?
 */
static bool
is_assable(exp e)
{
	long sz;
	unsigned char n = e->tag;

	if (is_a(n)) {
		return 1;
	}

	if (n != apply_tag && n != apply_general_tag) {
		return 0;
	}

	n = sh(e)->tag;
	sz = shape_size(sh(e));

	return n <= ulonghd || (n == ptrhd && sz == 32);
}

/*
 * DOIT ROUTINE, IS t NOT ASSIGNABLE?
 */
static bool
notass(exp t, int i)
{
	return i >= 0 && !is_assable(t);
}

/*
 * APPLY cc, DOING IT WITH ASSIGNABLES
 */
static void
all_assable(bool sto, exp to, exp e)
{
	cc(sto, to, 1, e, notass, 1);
}

/*
 * IS e DIRECTLY ADDRESSABLE?
 */
static bool
is_direct(exp e)
{
	unsigned char s = e->tag;

	return (s == name_tag && !isglob(child(e)) && !isvar(child(e))) ||
	       (s == cont_tag && child(e)->tag == name_tag &&
	        !isglob(child(child(e))) && isvar(child(child(e))));
}

/*
 * IS e INDIRECTLY ADDRESSABLE?
 */
static bool
is_indable(exp e)
{
	unsigned char s = e->tag;

	if (s == name_tag) {
		return 1;
	}

	if (s == cont_tag) {
		unsigned char t = child(e)->tag;
		return (t == name_tag && isvar(child(child(e)))) ||
		       (t == cont_tag && child(child(e))->tag == name_tag &&
		        isvar(child(child(child(e))))) ||
		       (t == reff_tag && is_direct(child(child(e))));
	}

	return (s == reff_tag && is_direct(child(e))) || s == addptr_tag;
}

#ifndef tdf3
/*
    MAKES child(e) INDIRECTLY ADDRESSABLE
*/
static void
indable_child(bool sto, exp to, exp e)
{
	if (!is_indable(child(e))) {
		exp ec;

		cca(sto, to, 1, e);
		ec = contexp(sto, to);
		scan(1, ec, child(ec));
	} else {
		scan(sto, to, child(e));
	}
}

#endif

/*
 * APPLY scan TO A NEXT LIST
 */
static void
scanargs(bool st, exp e)
{
	exp t = e;
	exp temp;

	while (temp = contexp(st, t), scan(st, t, temp),
	       temp = contexp(st, t), !temp->last) {
		t = contexp(st, t);
		st = 0;
	}
}

/*
 * DOIT ROUTINE FOR APPLY
 */
#if 0
static bool
apdo(exp t, int i)
{
	/* The first argument needs special treatment */
	if (i == 1) {
		return !is_indable(t);
	}
	return 0;
}
#endif

/*
 * DOIT ROUTINE FOR PLUS
 */
static bool
plusdo(exp t, int i)
{
	/* Can't negate first argument */
	if (i == 1) {
		return !is_opnd(t);
	}

	/* But can negate the rest */
	if (t->tag == neg_tag) {
		return 0;
	}

	return !is_opnd(t);
}

/*
 * DOIT ROUTINE FOR MULT
 */
static bool
multdo(exp t, int i)
{
	return i >= 0 && !is_o(t->tag);
}

/*
 * DOIT ROUTINE FOR AND
 */
static bool
anddo(exp t, int i)
{
	UNUSED(i);

#if 0
	/* Can't negate first argument */
	if (i == 1) {
		return !is_o(t->tag);
	}

	/* But can negate the rest */
	if (t->tag == not_tag) {
		return 0;
	}
#endif

	return !is_o(t->tag);
}

/*
 * DOIT ROUTINE FOR XOR
 */
static bool
notado(exp t, int i)
{
	return i >= 0 && !is_o(t->tag);
}

void
scan(bool sto, exp to, exp e)
{
	switch (e->tag) {
	case cond_tag:
	case rep_tag:
	case compound_tag:
	case solve_tag:
	case concatnof_tag:
	case nof_tag:
	case diagnose_tag:
#ifndef tdf3
	case caller_tag:
		if (child(e) == NULL) {
			/* empty make_nof */
			return;
		}

		scanargs(1, e);
		return;
#else
		scanargs(1, e);
		return;
#endif

	case labst_tag:
		scan(0, child(e), next(child(e)));
		return;

	case ident_tag:
		scan(0, child(e), next(child(e)));
		scan(1, e, child(e));
		return;

	case seq_tag:
		scanargs(1, child(e));
		scan(0, child(e), next(child(e)));
		return;

#if 0
	case diag_tag:
	case cscope_tag:
	case fscope_tag:
		scanargs(1, e);
		return;
#endif

	case local_free_tag:
	case long_jump_tag:
	case ncopies_tag:
		all_assable(sto, to, e);
		return;

	case alloca_tag:
		all_opnd(sto, to, e);
		return;

#ifndef tdf3
	case set_stack_limit_tag: {
		exp lim = get_stack_limit();
		setnext(lim, child(e));
		setchild(e, lim);
		e->tag = ass_tag;
		scan(sto, to, e);
		return;
	}
#endif

	case offset_add_tag:
	case offset_subtract_tag:
		if ((al2(sh(child(e))) == 1) && (al2(sh(next(child(e)))) != 1)) {
			make_bitfield_offset(next(child(e)), child(e), 0, sh(e));
		}

		if ((al2(sh(child(e))) != 1) && (al2(sh(next(child(e)))) == 1)) {
			make_bitfield_offset(child(e), e, 1, sh(e));
		}

	case test_tag:
	case absbool_tag:
	case testbit_tag:
	case make_stack_limit_tag:
	case minus_tag:
	case subptr_tag:
	case div0_tag:
	case div1_tag:
	case div2_tag:
	case shl_tag:
	case shr_tag:
	case rem0_tag:
	case mod_tag:
	case rem2_tag:
	case round_tag:
	case max_tag:
	case offset_max_tag:
	case min_tag:
	case offset_div_by_int_tag:
	case offset_negate_tag:
	case offset_pad_tag:
	case minptr_tag:
	case fplus_tag:
	case fminus_tag:
	case fmult_tag:
	case fdiv_tag:
	case fneg_tag:
	case fabs_tag:
	case chfl_tag:
	case float_tag:
	case offset_mult_tag:
	case offset_div_tag:
	case movecont_tag:
		all_opnd(sto, to, e);
		return;

	case not_tag:
	case neg_tag:
	case abs_tag:
	case chvar_tag:
		all_opnd(sto, to, e);
		return;

	case bitf_to_int_tag:
	case int_to_bitf_tag:
		all_opnd(sto, to, e);
		return;

	case ass_tag:
	case assvol_tag: {
		exp toc;

		/* Change assvol into ass */
		if (e->tag == assvol_tag) {
			e->tag = ass_tag;
		}

		if (!is_assable(next(child(e)))) {
			cca(sto, to, 0, child(e));
			toc = contexp(sto, to);
			scan(1, toc, child(toc));
		} else {
			scan(sto, to, next(child(e)));
		}

		cont_arg(sto, to, e, sh(next(child(e))));
		return;
	}

#ifndef tdf3
	case tail_call_tag: {
		exp cees = next(child(e));
		cur_proc_has_tail_call    = true;
		cur_proc_use_same_callees = (cees->tag == same_callees_tag);

		if (child(cees) != NULL) {
			cc(sto, to, 1, cees, no_alloca, 1);
		}

		indable_child(sto, to, e);

		return;
	}

	case apply_general_tag: {
		exp cees = next(next(child(e)));
		exp p_post = cees;	/* next(p_post) is postlude */

		cur_proc_use_same_callees = (cees->tag == same_callees_tag);

		while (next(p_post)->tag == ident_tag &&
		       child(next(p_post))->tag == caller_name_tag) {
			p_post = child(next(p_post));
		}

		scan(0, p_post, next(p_post));
		if (child(cees) != NULL) {
			scanargs(1, cees);
		}

		if (no(next(child(e))) != 0) {
			scanargs(1, next(child(e)));
		}

		if (!is_indable(child(e))) {
			exp ec;
			cca(sto, to, 1, e);
			ec = contexp(sto, to);
			scan(1, ec, child(ec));
		} else {
			scan(sto, to, child(e));
		}

		return;
	}
#endif

	case apply_tag:
		scanargs(1, e);

		FALL_THROUGH;

	case goto_lv_tag:
		if (!is_indable(child(e))) {
			exp ec;
			cca(sto, to, 1, e);
			ec = contexp(sto, to);
			scan(1, ec, child(ec));
		} else {
			scan(sto, to, child(e));
		}

		return;

#ifndef tdf3
	case untidy_return_tag:
#endif
	case res_tag: {
		long sz;

		if (child(e)->tag == apply_tag ||
		    child(e)->tag == apply_general_tag) {
			scan(sto, to, child(e));
			return;
		}

		sz = shape_size(sh(child(e)));

		if (sz <= 64) {
			all_assable(sto, to, e);
			return;
		}

		all_opnd(sto, to, e);
		return;
	}

	case case_tag: {
		exp toc;

		if (!is_opnd(child(e))) {
			cca(sto, to, 1, e);
			toc = contexp(sto, to);
			scan(1, toc, child(toc));
		} else {
			scan(sto, to, child(e));
		}

		return;
	}

	case plus_tag:
		if (child(e)->tag == neg_tag &&
		    next(child(e))->tag == val_tag) {
			scan(sto, to, child(e));
			return;
		}

		cc(sto, to, 1, e, plusdo, 1);
		return;

	case addptr_tag: {
		exp a = next(child(e));

		if (a->tag == offset_mult_tag &&
		    next(child(a))->tag == val_tag) {
			long k = no(next(child(a))) / 8;
			if (k == 1 || k == 2 || k == 4 || k == 8) {
				ccp(sto, to, 1, a);
				ap_arg1(sto, to, 1, e, 0);
				return;
			}
		}

		ccp(sto, to, 0, child(e));
		ap_arg1(sto, to, 1, e, 0);
		return;
	}

	case mult_tag:
		cc(sto, to, 1, e, multdo, 1);
		return;

	case and_tag:
		cc(sto, to, 1, e, anddo, 1);
		return;

	case or_tag:
	case xor_tag:
		cc(sto, to, 1, e, notado, 1);
		return;

	case cont_tag:
	case contvol_tag:
		/* Change contvol into cont */
		if (e->tag == contvol_tag) {
			e->tag = cont_tag;
		}

		cont_arg(sto, to, e, sh(e));
		return;

	case field_tag:
		if (!is_o(child(e)->tag) || e->tag == cont_tag) {
			exp temp;
			cca(sto, to, 1, e);
			temp = contexp(sto, to);
			scan(1, temp, child(temp));
		} else {
			scan(sto, to, child(e));
		}
		return;

	case reff_tag: {
		exp s = child(e);

		if (s->tag == name_tag || (s->tag == cont_tag &&
		                            child(s)->tag == name_tag)) {
			return;
		}

		ccp(sto, to, 1, e);
		return;
	}

	case general_proc_tag:
	case proc_tag:
		scan(1, e, child(e));
		return;

#if 0
	case val_tag:
		if (sh(e)->tag == offsethd && al2(sh(e)) >= 8) {
			no(e) = no(e) >> 3;
		}
		return;
#endif

	default:
		return;
	}
}

