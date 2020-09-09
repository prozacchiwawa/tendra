/*
 * Copyright 2002-2011, The TenDRA Project.
 * Copyright 1997, United Kingdom Secretary of State for Defence.
 *
 * See doc/copyright/ for the full copyright terms.
 */

#include <stddef.h>

#include <shared/bool.h>

#include <tdf/shape.h>
#include <tdf/tag.h>

#include <reader/exp.h>
#include <reader/externs.h>

#include <refactor/query.h>
#include <refactor/refactor.h>

bool
is_tester(exp e, int eq)
{
	if (e->tag == test_tag || e->tag == testbit_tag) {
		if (!eq || test_number(e) == f_equal) {
			return true;
		}
	}

	return false;
}

/*
 * looks for things like
 *     (a ~ b) ? a : b
 * puts test in t - can make use of delay-slot
 */
bool
is_maxlike(exp x, exp *t)
{
	exp op1, op2, z, l, w;
	exp xC, xCC, xCCC, xCCCC;
	exp yCC;

	if (x->tag != cond_tag) {
		return false;
	}

	xC = child(x);

	if (xC->tag != seq_tag) {
		return false;
	}

	xCC = child(xC);
	xCCC = child(xCC);

	*t = xCCC;

	if (xCCC->tag != test_tag) {
		return false;
	}

	l = pt(*t);

	xCCCC = child(xCCC);

	op1 = xCCCC;

	if (!(!is_floating(sh(op1)->tag))) {
		return false;
	}

	if (xCCCC->last) {
		return false;
	}

	xCCCC = next(xCCCC);
	op2 = xCCCC;
	if (!xCCCC->last) {
		return false;
	}

	if (!xCCC->last) {
		return false;
	}

	if (xCC->last) {
		return false;
	}

	xCC = next(xCC);

	z = xCC;

	if (!(comp_eq_exp(z, op1, NULL, NULL))) {
		return false;
	}

	if (!xCC->last) {
		return false;
	}

	if (xC->last) {
		return false;
	}

	xC = next(xC);
	if (l != xC) {
		return false;
	}

	yCC = child(xC);

	z = yCC;

	if (!(no(z) == 1)) {
		return false;
	}

	if (yCC->last) {
		return false;
	}

	yCC = next(yCC);
	w = yCC;

	if (!(comp_eq_exp(w, op2, NULL, NULL))) {
		return false;
	}

	if (!yCC->last) {
		return false;
	}

	if (!xC->last) {
		return false;
	}

	return true;
}

/*
 * looks for things like
 *     (a ~ b) ? b : a
 * puts test in t - can make use of delay-slot
 */
bool
is_minlike(exp x, exp *t)
{
	exp op1, op2, z, l, w;
	exp xC, xCC, xCCC, xCCCC;
	exp yCC;

	if (x->tag != cond_tag) {
		return false;
	}

	xC = child(x);

	if (xC->tag != seq_tag) {
		return false;
	}

	xCC = child(xC);
	xCCC = child(xCC);

	*t = xCCC;

	if (xCCC->tag != test_tag) {
		return false;
	}

	l = pt(*t);

	xCCCC = child(xCCC);

	op1 = xCCCC;

	if (!(!is_floating(sh(op1)->tag))) {
		return false;
	}

	if (xCCCC->last) {
		return false;
	}

	xCCCC = next(xCCCC);
	op2 = xCCCC;

	if (!xCCCC->last) {
		return false;
	}

	if (!xCCC->last) {
		return false;
	}

	if (xCC->last) {
		return false;
	}

	xCC = next(xCC);
	z = xCC;

	if (!(comp_eq_exp(z, op2, NULL, NULL))) {
		return false;
	}

	if (!xCC->last) {
		return false;
	}

	if (xC->last) {
		return false;
	}

	xC = next(xC);
	if (l != xC) {
		return false;
	}

	yCC = child(xC);

	z = yCC;

	if (!(no(z) == 1)) {
		return false;
	}

	if (yCC->last) {
		return false;
	}

	yCC = next(yCC);
	w = yCC;

	if (!(comp_eq_exp(w, op1, NULL, NULL))) {
		return false;
	}

	if (!yCC->last) {
		return false;
	}

	if (!xC->last) {
		return false;
	}

	return true;
}

/*
 * looks for things like
 *     (a~0) ? a:-a
 */
bool
is_abslike(exp x, exp *t)
{
	exp op, l, z, w;
	exp xC, xCC, xCCC, xCCCC;
	exp yCC, yCCC;

	if (x->tag != cond_tag) {
		return false;
	}

	xC = child(x);

	if (xC->tag != seq_tag) {
		return false;
	}

	xCC = child(xC);
	xCCC = child(xCC);

	*t = xCCC;
	if (xCCC->tag != test_tag) {
		return false;
	}

	l = pt(*t);

	xCCCC = child(xCCC);
	op = xCCCC;

	if (xCCCC->last) {
		return false;
	}

	xCCCC = next(xCCCC);
	if (xCCCC->tag != val_tag || no(xCCCC) != 0) {
		return false;
	}

	if (!xCCCC->last) {
		return false;
	}

	if (!xCCC->last) {
		return false;
	}

	if (xCC->last) {
		return false;
	}

	xCC = next(xCC);
	z = xCC;
	if (!(comp_eq_exp(z, op, NULL, NULL))) {
		return false;
	}

	if (!xCC->last) {
		return false;
	}

	if (xC->last) {
		return false;
	}

	xC = next(xC);
	if (l != xC) {
		return false;
	}

	yCC = child(xC);
	z = yCC;

	if (!(no(z) == 1)) {
		return false;
	}

	if (yCC->last) {
		return false;
	}

	yCC = next(yCC);
	if (yCC->tag != neg_tag) {
		return false;
	}

	yCCC = child(yCC);
	w = yCCC;

	if (!(comp_eq_exp(op, w, NULL, NULL))) {
		return false;
	}

	if (!yCCC->last) {
		return false;
	}

	if (!yCC->last) {
		return false;
	}

	if (!xC->last) {
		return false;
	}

	return true;
}

bool
is_fabslike(exp x, exp *t)
{
	exp op, l, z, w;
	exp xC, xCC, xCCC, xCCCC;
	exp yCC, yCCC;

	if (x->tag != cond_tag) {
		return false;
	}

	xC = child(x);

	if (xC->tag != seq_tag) {
		return false;
	}

	xCC = child(xC);
	xCCC = child(xCC);
	*t = xCCC;

	if (xCCC->tag != test_tag) {
		return false;
	}

	l = pt(*t);

	xCCCC = child(xCCC);
	op = xCCCC;

	if (xCCCC->last) {
		return false;
	}

	xCCCC = next(xCCCC);
	if (xCCCC->tag != val_tag || no(xCCCC) != 0) {
		return false;
	}

	if (!xCCCC->last) {
		return false;
	}

	if (!xCCC->last) {
		return false;
	}

	if (xCC->last) {
		return false;
	}

	xCC = next(xCC);
	z = xCC;

	if (!(eq_exp(z, op))) {
		return false;
	}

	if (!xCC->last) {
		return false;
	}

	if (xC->last) {
		return false;
	}

	xC = next(xC);
	if (l != xC) {
		return false;
	}

	yCC = child(xC);
	z = yCC;

	if (!(no(z) == 1)) {
		return false;
	}

	if (yCC->last) {
		return false;
	}

	yCC = next(yCC);
	if (yCC->tag != fneg_tag) {
		return false;
	}

	yCCC = child(yCC);
	w = yCCC;

	if (!(eq_exp(op, w))) {
		return false;
	}

	if (!yCCC->last) {
		return false;
	}

	if (!yCC->last) {
		return false;
	}

	if (!xC->last) {
		return false;
	}

	return true;
}

bool
is_condassign(exp e, exp *to_test, exp *to_ass)
{
	exp arg1 = child(e);
	exp arg2 = next(arg1);
	exp z;
	exp st;
	exp ass;
	exp val;

	if (arg1->tag != seq_tag) {
		return false;
	}

	z = child(arg1);
	st = child(z);
	ass = next(z);

	if (no(child(arg2)) != 1 || next(child(arg2))->tag != top_tag) {
		return false;
	}

	if (st->tag != test_tag && st->tag != testbit_tag) {
		return false;
	}

	if (!st->last) {
		return false;
	}

	if (ass->tag != ass_tag) {
		return false;
	}

	if (child(ass)->tag != name_tag || !isvar(child(child(ass)))) {
		return false;
	}

	val = next(child(ass));
	*to_test = st;
	*to_ass = ass;

	if (val->tag == val_tag) {
		return true;
	}

	return false;
}

/*
 * A rather more complicated equivalence of expressions.
 * This allows sequences and conditionals with tests
 * which only jump to nearest conditional outlab;
 * initial call : comp_eq_exp(a,b,NULL,NULL)
 */

static bool
comp_eq_explist(exp a, exp b, exp laba, exp labb)
{
	if (a == NULL) {
		return b == NULL;
	}

	if (b == NULL || !comp_eq_exp(a, b, laba, labb)) {
		return 0;
	}

	if (a->last) {
		return b->last;
	}

	if (b->last) {
		return 0;
	}

	return comp_eq_explist(next(a), next(b), laba, labb);
}

bool
comp_eq_exp(exp a, exp b, exp laba, exp labb)
{
	if (a->tag != b->tag || !eq_shape(sh(a), sh(b))) {
		return 0;
	}

	if (a->tag == seq_tag) {
		return(comp_eq_explist(child(child(a)), child(child(b)), laba, labb) &&
		       comp_eq_exp(next(child(a)), next(child(b)), laba, labb));
	}

	if (a->tag == cond_tag) {
		exp fa = child(a);
		exp fb = child(b);

		return(comp_eq_exp(fa, fb, next(fa), next(fb)) &&
		       comp_eq_exp(next(child(next(fa))), next(child(next(fb))), laba, labb));
	}

	if (a->tag == test_tag) {
		return(pt(a) == laba && pt(b) == labb && a->props == b->props &&
		       comp_eq_explist(child(a), child(b), laba, labb));
	}

	if (a->tag == name_tag) {
		return child(a) == child(b) && no(a) == no(b);
	}

	return is_a(a->tag) && no(a) == no(b) &&
	       comp_eq_explist(child(a), child(b), laba, labb);
}

/*
 * Pattern in oddtest.pat
 * this is intended to be the pattern for:
 *     if (test) fexp 1 else fexp -1
 * to transform to:
 *     fexp ( (absbool(test) <<1) -1))
 * where f is any sequence of unary operators including identity
 */

static bool
oddunary(exp x, exp y, exp *v)
{
	exp z;
	exp xC;

	*v = x;
	if (x->tag != val_tag) {
		goto flab1;
	}

	if (!(y->tag == val_tag && ((no(x) == 1 && no(y) == -1) || (no(x) == -1 && no(y) == 1) ))) {
		goto flab1;
	}

	return true;

flab1:

	if (x->tag != y->tag) {
		return false;
	}

	xC = child(x);
	z = xC;

	if (!(z != NULL && z->last && child(y) != NULL && oddunary(z, child(y), v))) {
		return false;
	}

	if (!xC->last) {
		return false;
	}

	return true;
}

bool
oddtest(exp x, exp *t, exp *f, exp *v)
{
	exp l, z, g;
	exp xC, xCC, xCCC;
	exp yCC;

	if (x->tag != cond_tag) {
		return false;
	}

	xC = child(x);

	if (xC->tag != seq_tag) {
		return false;
	}

	xCC = child(xC);
	xCCC = child(xCC);
	*t = xCCC;

	if (xCCC->tag != test_tag) {
		return false;
	}

	l = pt(*t);
	if (!xCCC->last) {
		return false;
	}

	if (xCC->last) {
		return false;
	}

	xCC = next(xCC);
	*f = xCC;

	if (!xCC->last) {
		return false;
	}

	if (xC->last) {
		return false;
	}

	xC = next(xC);
	if (l != xC) {
		return false;
	}

	yCC = child(xC);

	z = yCC;

	if (!(no(z) == 1)) {
		return false;
	}

	if (yCC->last) {
		return false;
	}

	yCC = next(yCC);
	g = yCC;

	if (!(oddunary(*f, g, v))) {
		return false;
	}

	if (!yCC->last) {
		return false;
	}

	if (!xC->last) {
		return false;
	}

	return true;
}

