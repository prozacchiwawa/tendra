/*
 * Copyright 2011, The TenDRA Project.
 * Copyright 1997, United Kingdom Secretary of State for Defence.
 *
 * See doc/copyright/ for the full copyright terms.
 */

#include <stdlib.h>

#include <shared/bool.h>
#include <shared/xalloc.h>

#include <local/ash.h>

#include <tdf/shape.h>
#include <tdf/tag.h>

#include <flpt/flpt.h>

#include <utility/int64.h>

#include "code_here.h"
#include "addr.h"
#include "inst_fmt.h"
#include "move.h"
#include "getregs.h"
#include "guard.h"
#include "bits.h"
#include "alpha_ins.h"
#include "reg_defs.h"
#include "ops.h"

/* corrects possible overflows of chars and shorts in reg r */
void
tidyshort(int r, shape s)
{
	if (s->tag == ucharhd) {
		operate_fmt_immediate (i_and, r, 255, r);
	} else if (s->tag == uwordhd) {
		operate_fmt_immediate (i_and, r, 0xffff, r);
	}
}

/*
 * given a list of expressions seq which contains one whose value is in
 * register reg, removes that exp from seq and delivers true;
 * otherwise delivers false
 */
static bool
regremoved(exp *seq, int reg)
{
	exp s = *seq;
	exp t = next(s);

	if (abs(regofval(s)) == reg) {
		*seq = t;
		return true;
	}

	for (;;) {
		if (abs(regofval(t)) == reg) {
			next(s) = next(t);
			if (t->last) {
				s->last = true;
			}

			return true;
		}

		if (t->last) {
			return false;
		}

		s = t;
		t = next(t);
	}
}

static void
do_comm(exp seq, space sp, int final, instruction rins)
{
	int   r = 0;
	space nsp;
	int   a1;
	int   a2;
	a1 = reg_operand (seq, sp);

	/* evaluate 1st operand into a1 */
	for (;;) {
		nsp = guardreg (a1, sp);
		seq = next (seq);
		if (seq->tag == val_tag) {/* next operand is a constant */
			if (seq->last) {
				if (isbigval(seq)) {
					int ov;
					flt64 res;
					INT64 ires;
					res = flt_to_f64(no(seq), is_signed(sh(seq)), &ov);
					ires = flt64_to_INT64(res);
					operate_fmt_big_immediate(rins, a1, ires, final);
				} else {
					operate_fmt_immediate (rins, a1, no(seq), final);
				}
				return;
			} else {
				if (r == 0) {
					r = getreg (sp.fixed);
				}
				if (isbigval(seq)) {
					int ov;
					flt64 res;
					INT64 ires;
					res = flt_to_f64(no(seq), is_signed(sh(seq)), &ov);
					ires = flt64_to_INT64(res);
					operate_fmt_big_immediate(rins, a1, ires, r);
				} else {
					operate_fmt_immediate(rins, a1, no(seq), r);
				}
			}
		} else {
			exp sq = seq;
			instruction ins = rins;
			a2 = reg_operand (sq, nsp);
			/* evaluate next operand */
			if (seq->last) {
				operate_fmt(ins, a1, a2, final);
				return;
			} else {
				if (r == 0) {
					r = getreg (sp.fixed);
				}
				operate_fmt (ins, a1, a2, r);
			}
		}
		a1 = r;
	}
}

/*
 * evaluate commutative operation rrins given by e into d,
 * using sp to get t-regs
 */
int
comm_op(exp e, space sp, where d, instruction rrins)
{
	instruction rins = rrins;

	switch (d.answhere.discrim) {
	case inreg: {
		int   dest = regalt(d.answhere);
		bool usesdest = regremoved(&child(e), dest);

		exp seq = child(e);
		if (dest == NO_REG) {
			dest = getreg(sp.fixed);
		}

		/* the destination is in a register; take care that
		we dont alter it before possible use as an operand .... */
		if (usesdest && seq->last) {
			/* ...it was used, but there is only one other operand */
			if (seq->tag == val_tag) {
				operate_fmt_immediate(rins, dest, no(seq), dest);
			} else {
				operate_fmt(rins, dest, reg_operand(seq, sp), dest);
			}
			tidyshort(dest, sh(e));
			return dest;
		} else if (usesdest) {	/* ... it was used so ... */
			int   r = getreg(sp.fixed);
			do_comm(seq, sp, r, rins);
			/* ... evaluate the remainder of the expression into r... */
			operate_fmt(rins, dest, r, dest);
			/* ... and do dest = dest rins r */
			tidyshort(dest, sh(e));
			return dest;
		} else {		/* ... it wasn't used */
			do_comm(seq, sp, dest, rins);
			tidyshort(dest, sh(e));
			return dest;
		}
	}

	default: {
		ans a;
		int   r = getreg(sp.fixed);
		space nsp;

		setregalt(a, r);
		do_comm(child(e), sp, r, rins);
		/* evaluate the expression into r ... */
		tidyshort(r, sh(e));
		nsp = guardreg(r, sp);
		move(a, d, nsp, 1);
		/* ... and move into a */
		return r;
	}
	}
}

/* evalate binary operation e with rins into dest */
int
non_comm_op(exp e, space sp, where dest, instruction rins)
{
	exp l = child(e);
	exp r = next(l);
	int   a1 = reg_operand(l, sp);
	space nsp;
	int   a2;
	instruction ins = rins;

	nsp = guardreg(a1, sp);
	a2 = reg_operand(r, nsp);

	/* regs a1 and a2 contain the operands */
	switch (dest.answhere.discrim) {
	case inreg: {
		int d = regalt(dest.answhere);

		if (d != NO_REG) {
			operate_fmt(ins, a1, a2, d);
			tidyshort(d, sh (e));
		}

		return d;
	}

	default: {
		ans a;
		int r1 = getreg(nsp.fixed);

		setregalt(a, r1);
		operate_fmt(ins, a1, a2, r1);
		tidyshort(r1, sh(e));
		nsp = guardreg(r1, sp);
		move(a, dest, nsp, 1);
		return r1;
	}
	}
}

/* evaluate floating dyadic operation e using ins into dest */
int
fop(exp e, space sp, where dest, instruction ins)
{
	exp l = child(e);
	exp r = next(l);
	int a1, a2;
	space nsp;

	if (IsRev(e)) {
		a2 = freg_operand(r, sp);
		nsp = guardfreg(a2, sp);
		a1 = freg_operand(l, nsp);
	} else {
		a1 = freg_operand(l, sp);
		nsp = guardfreg(a1, sp);
		a2 = freg_operand(r, nsp);
	}

	switch (dest.answhere.discrim) {
	case infreg: {
 		/* dest in register */
		freg fr;
		fr = fregalt(dest.answhere);
		float_op(ins, a1, a2, fr.fr);
		return fr.type == IEEE_double ? -(fr.fr + 32) : (fr.fr + 32);
	}

	default: {
		/* destination elsewhere */
		ans a;
		freg fr;
		int r1 = getfreg(nsp.flt);
		fr.fr   = r1;
		fr.type = dest.ashwhere.ashsize == 64 ? IEEE_double : IEEE_single;
		setfregalt (a, fr);

		float_op(ins, a1, a2, r1);
#if 1
		if (dest.answhere.discrim == insomefreg) {
			*dest.answhere.val.somefregans.fr = r1;
			dest.answhere.val.somefregans.type = fr.type;
		} else {
#endif
			move(a, dest, sp, 1);
#if 1
		}
#endif
		return fr.type == IEEE_double ? -(fr.fr + 32) : (fr.fr + 32);
	}
	}
}

