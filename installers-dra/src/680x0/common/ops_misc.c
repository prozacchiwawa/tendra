/*
 * Copyright (c) 2002-2006 The TenDRA Project <http://www.tendra.org/>.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of The TenDRA Project nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific, prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS
 * IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * $Id$
 */

/*
    		 Crown Copyright (c) 1997

    This TenDRA(r) Computer Program is subject to Copyright
    owned by the United Kingdom Secretary of State for Defence
    acting through the Defence Evaluation and Research Agency
    (DERA).  It is made available to Recipients with a
    royalty-free licence for its use, reproduction, transfer
    to other parties and amendment for any purpose not excluding
    product development provided that any such use et cetera
    shall be deemed to be acceptance of the following conditions:-

        (1) Its Recipients shall ensure that this Notice is
        reproduced upon any copies or amended versions of it;

        (2) Any amended version of it shall be clearly marked to
        show both the nature of and the organisation responsible
        for the relevant amendment or amendments;

        (3) Its onward transfer from a recipient to another
        party shall be deemed to be that party's acceptance of
        these conditions;

        (4) DERA gives no warranty or assurance as to its
        quality or suitability for any purpose and DERA accepts
        no liability whatsoever in relation to any use to which
        it may be put.
*/
/*
			    VERSION INFORMATION
			    ===================

--------------------------------------------------------------------------
$Header: /u/g/release/CVSROOT/Source/src/installers/680x0/common/ops_misc.c,v 1.1.1.1 1998/01/17 15:55:49 release Exp $
--------------------------------------------------------------------------
$Log: ops_misc.c,v $
 * Revision 1.1.1.1  1998/01/17  15:55:49  release
 * First version to be checked into rolling release.
 *
Revision 1.5  1997/11/13 08:27:16  ma
All avs test passed (except add_to_ptr).

Revision 1.4  1997/11/10 15:38:09  ma
.

Revision 1.3  1997/11/09 14:22:51  ma
Now is_signed is used instead of issigned. Added clear for 64 bit shapes.

Revision 1.2  1997/10/29 10:22:27  ma
Replaced use_alloca with has_alloca.

Revision 1.1.1.1  1997/10/13 12:42:57  ma
First version.

Revision 1.6  1997/10/13 08:49:52  ma
Made all pl_tests for general proc & exception handling pass.

Revision 1.5  1997/09/25 06:45:28  ma
All general_proc tests passed

Revision 1.4  1997/06/24 10:56:07  ma
Added changes for "Plumhall Patch"

Revision 1.3  1997/06/18 10:09:43  ma
Checking in before merging with Input Baseline changes.

Revision 1.2  1997/04/20 11:30:36  ma
Introduced gcproc.c & general_proc.[ch].
Added cases for apply_general_proc next to apply_proc in all files.

Revision 1.1.1.1  1997/03/14 07:50:16  ma
Imported from DRA

 * Revision 1.1.1.1  1996/09/20  10:56:57  john
 *
 * Revision 1.3  1996/07/30  16:31:50  john
 * Removed offset conversion
 *
 * Revision 1.2  1996/07/05  14:24:52  john
 * Changes for spec 3.1
 *
 * Revision 1.1.1.1  1996/03/26  15:45:16  john
 *
 * Revision 1.4  94/06/29  14:24:51  14:24:51  ra (Robert Andrews)
 * Need to be more careful about bitfields in change_variety.
 *
 * Revision 1.3  94/02/21  16:02:15  16:02:15  ra (Robert Andrews)
 * Clear up a couple of int-long confusions.
 *
 * Revision 1.2  93/03/03  14:49:46  14:49:46  ra (Robert Andrews)
 * Added error treatment processing routine, jump_overflow.
 *
 * Revision 1.1  93/02/22  17:16:26  17:16:26  ra (Robert Andrews)
 * Initial revision
 *
--------------------------------------------------------------------------
*/


#include "error.h"

#include "config.h"
#include "common_types.h"
#include "assembler.h"
#include "basicread.h"
#include "check.h"
#include "exp.h"
#include "expmacs.h"
#include "externs.h"
#include "install_fns.h"
#include "shapemacs.h"
#include "tags.h"
#include "mach.h"
#include "mach_ins.h"
#include "where.h"
#include "mach_op.h"
#include "instr.h"
#include "codex.h"
#include "instrs.h"
#include "coder.h"
#include "tests.h"
#include "operations.h"
#include "evaluate.h"
#include "utility.h"
#include "translate.h"
#include "ops_shared.h"
#include "special_exps.h"

/************************************************************************
  SET_OVERFLOW
  If the expression e has a long_jump error treatment then
  the global variable overflow_jump is set the the corresponding label.
  If e has the error treatment trap overflow_jump is set to -1 instead.
  The previous value of overflow_jump is returned, so it can be restored.
 ************************************************************************/

int
set_overflow(exp e)
{
	int prev_overflow_jump = overflow_jump;

	if (! optop(e)) {
		if (pt(e)) {
			/* error jump on overflow */
			overflow_jump = no(son(pt(e)));
			overflow_jump = ptno(pt(son(pt(e))));
			overflow_jump = e->ptf.expr->sonf.expr->ptf.expr->ptf.l;

		} else {
			overflow_jump = -1 ; /* trap on overflow */
		}
	}

	return prev_overflow_jump;
}

/************************************************************************
  CLEAR_OVERFLOW
  Restore the global variable overflow_jump with a previous value.
 ************************************************************************/

void
clear_overflow(int prev_overflow_jump)
{
	overflow_jump = prev_overflow_jump;
}

/************************************************************************
  HAVE_OVERFLOW
  Used to test if overflow_jump has been set (we have an error treatment)
 ************************************************************************/

int
have_overflow(void)
{
	return overflow_jump;
}

/************************************************************************
  TRAP_INS
  Calls the error handler with ec as argument
 ************************************************************************/

void
trap_ins(int ec)
{
	push(slongsh, L32, mnw(ec));
	callins(0, get_error_handler());
}

/*
    OVERFLOW JUMP LABEL

    This is 0 to denote that overflows should be ignored.  Otherwise
    it gives the label to be jumped to.
*/

int overflow_jump = 0;

int err_continue = 0;

/************************************************************************
  TEST_OVERFLOW2

  If an error_treatment is specified and the previous instruction
  overflowed then either a trap or a jump is takken.

  The test condition is specified by br_ins
  ************************************************************************/

void
test_overflow2(int br_ins)
{
	if (overflow_jump == -1) {
		ins0(bra2trap(br_ins));
	} else if (overflow_jump) {
		make_jump(br_ins, overflow_jump);
	}
}


/************************************************************************
  TEST_OVERFLOW

  If an error_treatment is specified and the previous instruction
  overflowed then either a trap or a jump is takken.

  This function finds the right test condition based on overflow_type
  ************************************************************************/

void
test_overflow(overflow_type typ)
{
	int instr;

	if (! have_overflow()) {
		return;
	}

	switch (typ) {
	case UNCONDITIONAL:
		instr = m_bra;
		break;
	case ON_OVERFLOW:
		instr = m_bvs;
		break;
	case ON_CARRY:
		instr = m_bcs;
		break;
	case ON_FP_OVERFLOW:
	case ON_FP_CARRY:
		ins2(m_fmovel, L32, L32, register(REG_FPSR), D0, 1);
		ins2h(m_andl, 0x00001c00, L32, D0, 1);
		instr = m_bne;
		break;
	case ON_FP_OPERAND_ERROR:
		ins2(m_fmovel, L32, L32, register(REG_FPSR), D0, 1);
		ins2h(m_andl, 0x00002000, L32, D0, 1);
		instr = m_bne;
		break;
	default:
		error(ERROR_SERIOUS, "invalid overflow test");
		return;
	}

	test_overflow2(instr);
}

/************************************************************************
  CHECKALLOC_STACK

  Checks if it is possible to allocate sz bytes on the stack.
  If it is not possible an exception is generated.
  else if do_alloc is TRUE, the allocation is done.

  ************************************************************************/

void
checkalloc_stack(where sz, int do_alloc)
{
	int erlab = next_lab();
	int cnlab = next_lab();
	make_comment("check for stack overflow ...");
	ins2(m_movl, 32, 32, SP, D0, 1);
	ins2(m_subl, 32, 32, sz, D0, 1);
	make_jump(m_bcs, erlab);
	ins2(m_cmpl, 32, 32, mw(get_stack_limit(), 0), D0, 0);
	make_jump(m_bcc, cnlab);
	make_label(erlab);
	trap_ins(f_stack_overflow);
	make_label(cnlab);
	if (do_alloc) {
		ins2(m_movl, 32, 32, D0, SP, 1);
	}
	make_comment("check for stack overflow done");
}

/*
    MARK D1 AS SPECIAL

    This flag is used to indicate that the D1 regsiter is being used
    as a special register and should be treated with care.
*/

bool D1_is_special = 0;


/*
    OUTPUT A CALL INSTRUCTION

    The procedure call given by fn is output.  A temporary A-register
    needs to be used when fn is not a simple procedure name.  The
    stack is then increased by longs to overwrite the procedure arguments.
*/

void
callins(long longs, exp fn)
{
	mach_op *op;
	exp s = son(fn), call_exp, fn_exp;
	bool simple_proc = 0;
	fn_exp = fn;

	/* Let's see if we have the procedure at compilation time */
	if (name(fn) == name_tag && ! isvar(s) && isglob(s)) {
		exp def = son ( s ) ; /* Definition of Identify construct */
		if (!def || name(def) == proc_tag ||
		    name(def) == general_proc_tag) {
			simple_proc = 1;
		}
	}

	/* If this is not a straight call, put the name into an A register */
	if (! simple_proc) {
		where w;
		w = zw(fn);
		if (whereis(w)!= Areg) {
			int r = next_tmp_reg();
			regsinproc |= regmsk(r);
			move(slongsh, w, register(r));
			fn_exp = register(r).wh_exp;
		}
	}
	/* Now output the call instruction */
	call_exp = getexp(proksh, nilexp, 0, fn_exp, nilexp, 0, L0, cont_tag);
	op = operand(L32, zw(call_exp));
	make_instr(m_call, op, null, ~save_msk);
	no_calls++;
	retcell(call_exp);
	dec_stack(-longs);
	have_cond = 0;
	return;
}

/************************************************************************
    OUTPUT A JMP INSTRUCTION

    The jump to the procedure given by fn is output.  A temporary A-register
    needs to be used when fn is not a simple procedure name.

 ************************************************************************/

void
jmpins(exp fn)
{
	mach_op *op;
	exp s = son(fn), jmp_exp, fn_exp;
	fn_exp = fn;
	/* If this is not a straight jmp, put the name into an A register */
	if (name(fn)!= name_tag || isvar(s) || !isglob(s)) {
		where w;
		w = zw(fn);
		if (whereis(w)!= Areg) {
			int r = next_tmp_reg();
			regsinproc |= regmsk(r);
			move(slongsh, w, register(r));
			fn_exp = register(r).wh_exp;
		}
	}
	/* Now output the jmp instruction */
	jmp_exp = getexp(proksh, nilexp, 0, fn_exp, nilexp, 0, L0, cont_tag);
	op = operand(L32, zw(jmp_exp));
	make_instr(m_jmp, op, null, ~save_msk);
	retcell(jmp_exp);
	have_cond = 0;
	return;
}



/*
    CONDITION CODES STATUS

    Many comparison instructions are unnecessary because the previous
    instruction has set the appropriate condition flags.  The flag
    have_cond deals with this.  A value of 0 indicates that we have
    no information on the flag values.  A value of 1 indicates that
    the last instruction set the flags appropriate to the where
    last_cond of size last_cond_sz.  A value of 2 is used immediately
    after a cmp instruction, the two arguments of the cmp being
    last_cond and last_cond2.  Finally a value of 3 is used immediately
    after certain move instructions to indicate that the flags are
    appropriate to either of the arguments, last_cond or last_cond_alt.
*/

bool have_cond = 0;
where last_cond;
where last_cond2;
where last_cond_alt;
long last_cond_sz;


/*
    COMPARE WITH ZERO

    The value a (of shape sha and size sz) is compared with 0.  The
    cases when have_cond is 1 or 3 are dealt with by this routine.
*/

void
cmp_zero(shape sha, long sz, where a)
{
	long w;
	/* Check existing condition codes */
	if (have_cond == 1 && last_cond_sz == sz) {
		if (eq_where(last_cond, a)) {
			return;
		}
	}
	if (have_cond == 3 && last_cond_sz == sz) {
		if (eq_where(last_cond, a)) {
			return;
		}
		if (eq_where(last_cond_alt, a)) {
			return;
		}
	}
	w = whereis(a);
	if (w == Areg) {
		/* This does work, despite the manual */
		int instr = ins(sz, ml_tst);
		ins1(instr, sz, a, 0);
	} else if (w == Freg || (w == External && name(sha) == prokhd)) {
		/* Moving to D0 sets the flags */
		move(sha, a, D0);
	} else {
		if (sz == 64) {
			where w;
			w = a;
			ins1(m_tstl, 32, w, 0);
			w.wh_off += 32;
			ins1(m_tstl, 32, w, 0);
		} else {
			int instr = ins(sz, ml_tst);
			ins1(instr, sz, a, 0);
		}
	}
	/* Set new condition codes */
	set_cond(a, sz);
	return;
}


/*
    AUXILIARY COMPARISON ROUTINE

    The values a and b of size sz are compared.
*/

static bool
cmp_aux(long sz, where a, where b)
{
	where d;
	if (whereis(a) == Freg) {
		if (whereis(b) == Freg) {
			move(slongsh, a, D0);
			move(slongsh, b, D1);
			regsinproc |= regmsk(REG_D1);
			return cmp_aux(sz, D1, D0);
		}
		if (eq_where(b, D0)) {
			d = D1;
			regsinproc |= regmsk(REG_D1);
		} else {
			d = D0;
		}
		move(slongsh, a, d);
		return cmp_aux(sz, b, d);
	}
	if (whereis(b) == Freg) {
		if (eq_where(a, D0)) {
			d = D1;
			regsinproc |= regmsk(REG_D1);
		} else {
			d = D0;
		}
		move(slongsh, b, d);
		return cmp_aux(sz, a, d);
	}
	ins2_cmp(ins(sz, ml_cmp), sz, sz, a, b, 0);
	have_cond = 2;
	last_cond = a;
	last_cond2 = b;
	last_cond_sz = sz;
	return 1;
}


/*
    COMPARE WITH A CONSTANT

    The value a is compared with the constant value c, the type of the
    comparison being given by ntst.  The value returned by this routine
    has the same meaning as that returned by cmp.
*/

static bool
cmp_const(shape sha, long sz, where c, where a, long ntst)
{
	bool sw;
	long v = nw(c);
	if (is_offset(c.wh_exp)) {
		v /= 8;
	}
	if (v == 0) {
		if (!is_signed(sha) && ntst != tst_neq && ntst != tst_eq) {
			/* Force an actual comparison in these cases */
			have_cond = 0;
		}
		cmp_zero(sha, sz, a);
		return 1;
	}

	if (v < -128 || v > 127) {
		sw = cmp_aux(sz, c, a);
		return sw;
	}

	if (interfere(a, D0)) {
		sw = cmp_aux(sz, c, a);
		return sw;
	}

#ifdef REJECT
	if (!output_immediately) {
		mach_ins *p = current_ins;
		if (p && p->ins_no == m_moveq && p->op1->def.num == v) {
			sw = cmp_aux(sz, a, register(p->op2->def.num));
			last_cond2 = c;
			return !sw;
		}
	}
#endif

	move(slongsh, c, D0);
	sw = cmp_aux(sz, a, D0);
	last_cond2 = c;
	return !sw;
}


/*
    MAIN COMPARISON ROUTINE

    The values var and limit of shape sha are compared for the test
    indicated by ntst.  Depending on the addressing modes of var and
    limit we may do "cmp var,limit" or "cmp limit,var".  In the first
    case we return 1 and in the second 0.  The case when have_cond is
    2 is dealt with by this routine.
*/

bool
cmp(shape sha, where var, where limit, long ntst)
{
	bool sw;
	long sz = shape_size(sha);
	long rt = shtype(sha);

	long whv = whereis(var);
	long whl = whereis(limit);

#if 0
	if (name(sha) == ptrhd) {
		make_comment("HACK shape size");
		shape_size(sha) = 32;
		sz = 32;
	}
#endif
	if (rt == Freg) {
		/* Floating point comparisons are never swapped */
		where rv, rl;
		have_cond = 0;
		if (whv == Freg && last_use(var)) {
			rv = var;
		} else {
			if (eq_where(limit, FP0)) {
				rv = FP1;
				regsinproc |= regmsk(REG_FP1);
			} else {
				rv = FP0;
			}
		}
		if (whl == Freg && last_use(limit)) {
			rl = limit;
		} else {
			if (eq_where(rv, FP0)) {
				rl = FP1;
				regsinproc |= regmsk(REG_FP1);
			} else {
				rl = FP0;
			}
		}
		if (whv == Freg) {
			push_float(sz, var);
			pop_float(sz, rv);
		} else {
			move(sha, var, rv);
		}
		if (whl == Freg) {
			push_float(sz, limit);
			pop_float(sz, rl);
		} else {
			move(sha, limit, rl);
		}
		ins2_cmp(m_fcmpx, sz, sz, rl, rv, 0);
		return 1;
	}

	/* Check existing condition codes */
	if (have_cond == 2 && last_cond_sz == sz) {
		if (eq_where(last_cond, var) && eq_where(last_cond2, limit)) {
			return 0;
		}
		if (eq_where(last_cond, limit) && eq_where(last_cond2, var)) {
			return 1;
		}
	}

	if (whl == Value) {
		sw = cmp_const(sha, sz, limit, var, ntst);
		return sw;
	}

	if (whv == Value) {
		sw = cmp_const(sha, sz, var, limit, ntst);
		return !sw;
	}

	if (whl == Dreg || whl == Areg) {
		sw = cmp_aux(sz, var, limit);
		return !sw;
	}

	if (whv == Dreg || whv == Areg) {
		sw = cmp_aux(sz, limit, var);
		return sw;
	}

#if 0
	if (name(var.wh_exp) == name_tag && name(sha) == prokhd &&
	    ((son(son(var.wh_exp)) ==nilexp) ||
	     (name(son(son(var.wh_exp))) == proc_tag))) {
		exp proc_cont = getexp(sha, nilexp, 0, var.wh_exp, nilexp, 0,
				       0, cont_tag);
		var.wh_exp = proc_cont;
	}
#endif

	if (!interfere(var, D0)) {
		move(sha, limit, D0);
		sw = cmp_aux(sz, var, D0);
		last_cond2 = limit;
		return !sw;
	}

	if (!interfere(limit, D0)) {
		move(sha, var, D0);
		sw = cmp_aux(sz, limit, D0);
		last_cond2 = var;
		return sw;
	}

	move(sha, limit, D1);
	sw = cmp_aux(sz, var, D1);
	regsinproc |= regmsk(REG_D1);
	last_cond2 = limit;
	return !sw;
}


/*
    OUTPUT A PUSH INSTRUCTION

    The value wh of shape sha and size sz is pushed onto the stack.
*/

void
push(shape sha, long sz, where wh)
{
	long s;
	mach_op *op1, *op2;
	bool real_push = 1;
	if (sz != 32) {
		if (is_signed(sha) && (whereis(wh) == Dreg)) {
			change_var_sh(slongsh, sha, wh, wh);
			push(slongsh, L32, wh);
		} else {
			change_var_sh(slongsh, sha, wh, D0);
			push(slongsh, L32, D0);
		}
		have_cond = 0;
		return;
	}
	if (stack_change) {
		stack_change -= 32;
		real_push = 0;
		if (stack_direction) {
			update_stack();
		}
		s = stack_change;
		stack_change = 0;
	}
	op1 = operand(sz, wh);
	if (real_push) {
		op2 = make_dec_sp();
	} else {
		op2 = make_indirect(REG_SP, s / 8);
	}
	make_instr(m_movl, op1, op2, 0);
	have_cond = 0;
	if (real_push) {
		stack_size -= 32;
	} else {
		stack_change = s;
	}
	return;
}


/*
    PUSH A FLOATING POINT REGISTER

    The floating-point register wh of size sz is pushed onto the stack.
*/

void
push_float(long sz, where wh)
{
	mach_op *op1 = operand(sz, wh);
	mach_op *op2 = make_dec_sp();
	int instr = insf(sz, ml_fmove);
	make_instr(instr, op1, op2, 0);
	stack_size -= sz;
	have_cond = 0;
	return;
}


/*
    OUTPUT A POP OPERATION

    A value of shape sha and size sz is popped from the stack into wh.
*/

void
pop(shape sha, long sz, where wh)
{
	mach_op *op1, *op2;
	if (sz != 32) {
		if (whereis(wh) == Dreg) {
			pop(slongsh, L32, wh);
			change_var_sh(sha, slongsh, wh, wh);
		} else {
			pop(slongsh, L32, D0);
			change_var_sh(sha, slongsh, D0, wh);
		}
		have_cond = 0;
		return;
	}
	op1 = make_inc_sp();
	op2 = operand(sz, wh);
	make_instr(m_movl, op1, op2, 0);
	have_cond = 0;
	stack_size += sz;
	return;
}


/*
    POP A FLOATING POINT REGISTER

    A value of size sz is popped from the stack into the floating-point
    register wh.
*/

void
pop_float(long sz, where wh)
{
	mach_op *op1 = make_inc_sp();
	mach_op *op2 = operand(sz, wh);
	int instr = insf(sz, ml_fmove);
	make_instr(instr, op1, op2, 0);
	have_cond = 0;
	stack_size += sz;
	return;
}


/*
    MOVE AN ADDRESS INTO A TEMPORARY REGISTER

    The effective address of wh is loaded into a temporary register and
    the register number is returned.  By default, register r is used,
    but if try is true we see if we can do better.
*/

static int
tmp_mova(where wh, int r, bool try)
{
	tmp_reg_prefer = r;
	mova(wh, register(r));
	if (try && !output_immediately && current_ins) {
		int i = current_ins->ins_no;
		if (i == m_lea || i == m_movl) {
			mach_op *op1 = current_ins->op1;
			mach_op *op2 = current_ins->op2;
			if (op2->type == MACH_REG && op2->def.num == r) {
				int t = r;
				if (i == m_lea) {
					if (op1->type == MACH_CONT) {
						op1 = op1->of;
						if (op1->type == MACH_REG &&
						    op1->plus == null) {
							t = op1->def.num;
						}
					}
				} else {
					if (op1->type == MACH_REG) {
						t = op1->def.num;
					}
				}
				if (t != r) {
					current_ins->ins_no = m_ignore_ins;
					op2->def.num = t;
					r = t;
				}
			}
		}
	}
	regsinproc |= regmsk(r);
	return r;
}


/*
    MOVE A CONSTANT VALUE

    The constant value c is assigned to the where to (of shape sha and
    size sz).
*/

void
move_const(shape sha, long sz, long c, where to)
{
	int instr;
	int whto = whereis(to);

	if (c == 0) {
		/* Clearing is a special case */
		if (whto == Dreg) {
			ins2n(m_moveq, 0, L32, to, 1);
			set_cond(to, sz);
			return;
		}
		if (whto == Areg) {
			ins2(m_subl, L32, L32, to, to, 1);
			have_cond = 0;
			return;
		}
		if (sz == 64) {
			where w;
			w = to;
			ins1(m_clrl, 32, w, 0);
			w.wh_off += 32;
			ins1(m_clrl, 32, w, 0);
		} else {
			instr = ins(sz, ml_clr);
			ins1(instr, sz, to, 1);
			set_cond(to, sz);
		}
		return;
	}

	instr = ins(sz, ml_mov);

	if (sz == 8) {
		c &= 0xff;
	}
	if (sz == 16) {
		c &= 0xffff;
	}
	if (c >= -128 && c <= 127) {
		/* Look for quick moves */
		if (whto == Dreg) {
			ins2n(m_moveq, c, L32, to, 1);
			set_cond(to, sz);
			return;
		} else {
			ins2n(m_moveq, c, L32, D0, 1);
			if (whto == Areg) {
				instr = m_movl;
			}
			ins2(instr, sz, sz, D0, to, 1);
			if (whto == Areg) {
				have_cond = 0;
			} else {
				set_cond(to, sz);
			}
			return;
		}
	}

	if (whto == Areg && sz == 8) {
		ins2n(instr, c, sz, D0, 1);
		ins2(m_movl, L32, L32, D0, to, 1);
	} else {
		ins2n(instr, c, sz, to, 1);
	}
	if (whto == Areg) {
		have_cond = 0;
	} else {
		set_cond(to, sz);
	}
	return;
}


/*
    MOVE FROM A FLOATING-POINT REGISTER

    The value in the floating-point register from (of size sz) is moved
    into to.
*/

static void
move_from_freg(long sz, where from, where to)
{
	int instr = insf(sz, ml_fmove);
	switch (whereis(to)) {
	case Dreg:
		ins2(m_fmoves, sz, sz, from, to, 1);
		have_cond = 0;
		return;
	case Freg:
		ins2(m_fmovex, sz, sz, from, to, 1);
		have_cond = 0;
		return;
	case RegPair: {
		exp te = to.wh_exp;
		if (sz != 64) {
			error(ERROR_SERIOUS, "Wrong floating variety");
		}
		push_float(sz, from);
		pop(slongsh, L32, zw(son(te)));
		pop(slongsh, L32, zw(bro(te)));
		have_cond = 0;
		return;
	}
	default:
		ins2(instr, sz, sz, from, to, 1);
		have_cond = 0;
		return;
	}
}


/*
    MOVE TO A FLOATING-POINT REGISTER

    The value in from (of size sz) is moved into the floating-point
    register to.
*/

static void
move_to_freg(long sz, where from, where to)
{
	int instr = insf(sz, ml_fmove);
	switch (whereis(from)) {
	case Dreg:
		ins2(m_fmoves, sz, sz, from, to, 1);
		have_cond = 0;
		return;
	case Areg:
		move(slongsh, from, D0);
		ins2(m_fmoves, sz, sz, D0, to, 1);
		have_cond = 0;
		return;
	case Freg:
		ins2(m_fmovex, sz, sz, from, to, 1);
		have_cond = 0;
		return;
	case RegPair: {
		exp fe = from.wh_exp;
		if (sz != 64) {
			error(ERROR_SERIOUS, "Wrong floating variety");
		}
		push(slongsh, L32, zw(bro(fe)));
		push(slongsh, L32, zw(son(fe)));
		pop_float(sz, to);
		have_cond = 0;
		return;
	}
	default:
		ins2(instr, sz, sz, from, to, 1);
		have_cond = 0;
		return;
	}
}


/*
    TEST AN EXTERNAL FOR SIMPLE CONTENTS/ASSIGN

    The expression e of external storage type is checked for simple
    operand type.
*/

static bool
ca_extern(exp e)
{
	char n = name(e);
	if (n != cont_tag && n != ass_tag) {
		return 0;
	}
	return name(son(e)) == name_tag ? 1 : 0;
}


/*
    MOVE LARGE OBJECTS

    sz bits are copied from from to to.  down can be 0 (start at the
    top), 1 (start at the bottom) or 2 (don't care).
*/

void
move_bytes(long sz, where from, where to, int down)
{
	long off;
	int instr;

	exp fe = from.wh_exp;
	exp te = to.wh_exp;
	long fof = from.wh_off;
	long tof = to.wh_off;

	long whfrom = whereis(from);
	long whto = whereis(to);

	/* Set up move types */
	int r1 = REG_A0;
	int r2 = REG_A1;
	int s1 = 0;
	int s2 = 0;

	if (whfrom == External && ca_extern(fe)) {
		s1 = 3;
	}
	if (name(te) == apply_tag || name(te) == apply_general_tag ||
	    name(te) == tail_call_tag) {
		s2 = 1;
	}
	if (whto == External && ca_extern(te)) {
		s2 = 3;
	}

	if (whfrom == Variable || whfrom == Parameter || whfrom == RegInd) {
		s1 = 3;
	}
	if (whto == Variable || whto == Parameter || whto == RegInd) {
		s2 = 3;
	}
	if (whfrom == RegPair) {
		s1 = 4;
	}
	if (whto == RegPair) {
		s2 = 4;
	}

	if (sz > 12 * 32 && s2 != 1 && down != 1) {
		mach_op *op1, *op2;
		long lab = next_lab();
		long longs = (sz / 32);
		sz -= 32 * longs;
		r1 = REG_A0;
		r2 = REG_A1;
		s1 = 0;
		s2 = 0;
		tmp_mova(from, r1, 0);
		tmp_mova(to, r2, 0);
		move(slongsh, mnw(longs - 1), D0);
		make_label(lab);
		op1 = make_postinc(r1);
		op2 = make_postinc(r2);
		make_instr(m_movl, op1, op2, regmsk(r1) | regmsk(r2));
		op1 = make_register(REG_D0);
		op2 = make_lab_data(lab, 0);
		make_instr(m_dbf, op1, op2, regmsk(REG_D0));
	} else {
		if (s1 == 0) {
			int r = tmp_mova(from, r1, 1);
			if (r != r1) {
				if (s2 == 0) {
					r2 = tmp_mova(to, r1, 1);
				}
				r1 = r;
			} else {
				if (s2 == 0) {
					r2 = tmp_mova(to, r2, 1);
				}
			}
		} else {
			if (s2 == 0) {
				r2 = tmp_mova(to, REG_A1, 1);
			}
		}
	}

	off = 0;
	while (sz) {
		mach_op *op1, *op2;
		long b = ((sz >= 32)? 32 :((sz >= 16)? 16 : 8));
		sz -= b;
		if (down != 0) {
			off = sz;
		}
		instr = ins(b, ml_mov);
		switch (s1) {
		case 0:
			op1 = make_indirect(r1, off / 8);
			break;
		case 2:
			op1 = make_lab_ind(r1, off / 8);
			break;
		case 3:
			op1 = operand(L32, mw(fe, fof + off));
			break;
		case 4:
			op1 = operand(L32, zw(sz ? bro(fe) : son(fe)));
			break;
		}
		switch (s2) {
		case 0:
			op2 = make_indirect(r2, off / 8);
			break;
		case 1:
			op2 = make_dec_sp();
			break;
		case 3:
			op2 = operand(L32, mw(te, tof + off));
			break;
		case 4: {
			op2 = operand(L32, zw(sz ? bro(te) : son(te)));
			break;
		}
		}
		make_instr(instr, op1, op2, 0);
		if (s2 == 1) {
			stack_size -= b;
		}
		off += b;
	}
	have_cond = 0;
	return;
}


/*
    MAIN MOVE ROUTINE

    A value of shape sha is moved from from into to.  There are several
    main subcases : floating-point values, values of sizes 8, 16 and 32,
    and all other cases.
*/

void
move(shape sha, where from, where to)
{
	int instr;
	long sz = shape_size(sha);
	long rt = shtype(sha);
	where from1, from2;

	exp fe = from.wh_exp;
	exp te = to.wh_exp;
	long fof = from.wh_off;
	long tof = to.wh_off;

	long whfrom = whereis(from);
	long whto = whereis(to);

	if (sz == 0 || eq_where(from, to) || eq_where(to,zero)) {
		return;
	}
	sz = round(sz, shape_align(sha));

	if (name(sha) == bitfhd && sz != 8 && sz != 16) {
		sz = 32;
	}

	if (rt == Freg || whfrom == Freg || whto == Freg) {
		if (name(fe) == real_tag) {
			whfrom = Value;
		}
		if (name(te) == apply_tag || name(te) == apply_general_tag ||
		    name(te) == tail_call_tag) {
			switch (whfrom) {
			case Dreg:
			case Areg:
				from1 = from;
				break;
			case Freg:
				push_float(sz, from);
				return;
			case Value: {
				long *p = realrep(fe);
				if (p) {
					from1 = mnw(p[0]);
					if (sz > 32) {
						from2 = mnw(p[1]);
					}
				} else {
					long lb = next_lab();
					exp t = simple_exp(internal_tag);
					make_constant(lb, fe);
					no(t) = lb;
					from1 = mw(t, fof);
					from2 = mw(t, fof + 32);
				}
				break;
			}
			case RegPair:
				from1 = zw(son(fe));
				from2 = zw(bro(fe));
				break;
			case Variable:
				from1 = mw(fe, fof);
				if (sz > 32) {
					from2 = mw(fe, fof + 32);
				}
				break;
			case External:
				if (ca_extern(fe)) {
					from1 = mw(fe, fof);
					if (sz > 32)from2 = mw(fe, fof + 32);
				} else {
					tmp_mova(from, REG_A0, 0);
					from1 = A0_p;
					if (sz > 32)from2 = mw(A0_p.wh_exp, 32);
				}
				break;
			default:
				tmp_mova(from, REG_A0, 0);
				from1 = A0_p;
				if (sz > 32) {
					from2 = mw(A0_p.wh_exp, 32);
				}
				break;
			}
			if (sz > 32) {
				move(slongsh, from2, to);
			}
			move(slongsh, from1, to);
			have_cond = 0;
			return;
		}
		if (whfrom == Freg) {
			move_from_freg(sz, from, to);
			return;
		}
		if (whto == Freg) {
			move_to_freg(sz, from, to);
			return;
		}
		if (whfrom == Value) {
			if (sz == 32) {
				long *p = realrep(fe);
				if (p) {
					from1 = mnw(p[0]);
					ins2(m_movl, L32, L32, from1, to, 1);
				} else {
					ins2(m_movl, L32, L32, from, to, 1);
				}
				have_cond = 0;
				return;
			} else {
				long *p = realrep(fe);
				if (p) {
					from1 = mnw(p[0]);
					from2 = mnw(p[1]);
				} else {
					long lb = next_lab();
					exp t = simple_exp(internal_tag);
					make_constant(lb, fe);
					no(t) = lb;
					from1 = mw(t, fof);
					from2 = mw(t, fof + 32);
				}
				if (whto == RegPair) {
					ins2(m_movl, L32, L32, from1,
					     zw(son(te)), 1);
					ins2(m_movl, L32, L32, from2,
					     zw(bro(te)), 1);
					have_cond = 0;
					return;
				}
				ins2(m_movl, L32, L32, from2, mw(te, tof + 32),
				     1);
				ins2(m_movl, L32, L32, from1, to, 1);
				have_cond = 0;
				return;
			}
		}
		if (whfrom == RegPair) {
			if (sz != 64) {
				error(ERROR_SERIOUS, "Wrong floating variety");
			}
			ins2(m_movl, L32, L32, zw(bro(fe)),
			     mw(te, tof + 32), 1);
			ins2(m_movl, L32, L32, zw(son(fe)), to, 1);
			have_cond = 0;
			return;
		}
		if (whto == RegPair) {
			if (sz != 64) {
				error(ERROR_SERIOUS, "Wrong floating variety");
			}
			ins2(m_movl, L32, L32, from, zw(son(te)), 1);
			ins2(m_movl, L32, L32, mw(fe, fof + 32),
			     zw(bro(te)), 1);
			have_cond = 0;
			return;
		}
		/* Fall through otherwise */
	}

	/* Move things of size 8, 16 or 32 */
	if (sz <= 32 && sz != 24) {
		if (name(te) == apply_tag || name(te) == apply_general_tag ||
		    name(te) == tail_call_tag) {
			if (whfrom == Value) {
				mach_op *op1, *op2;
				long v = nw(from);
				if (is_offset(from.wh_exp)) {
					v /= 8;
				}
				if (v == 0 && stack_change == 0) {
					op1 = make_dec_sp();
					make_instr(m_clrl, op1, null, 0);
					have_cond = 0;
					stack_size -= 32;
					return;
				}
				if (v >= -128 && v <= 127) {
					long s = stack_change;
					stack_change = 0;
					op1 = make_value(v);
					op2 = make_register(REG_D0);
					make_instr(m_moveq, op1, op2,
						   regmsk(REG_D0));
					stack_change = s;
					push(sha, L32, D0);
					return;
				}
				if (stack_change) {
					push(sha, L32, from);
					return;
				}
				op1 = make_int_data(v);
				make_instr(m_pea, op1, null, 0);
				have_cond = 0;
				stack_size -= 32;
				return;
			}
			push(sha, sz, from);
			return;
		}

		if (name(fe) == null_tag) {
			move_const(sha, sz, L0, to);
			return;
		}

		if (whfrom == Value) {
			long v = nw(from);
			if (is_offset(from.wh_exp)) {
				v /= 8;
			}
			move_const(sha, sz, v, to);
			return;
		}

		if (sz == 8) {
			if (whfrom == Areg) {
				move(slongsh, from, D0);
				move(sha, D0, to);
				return;
			}
			if (whto == Areg) {
				move(sha, from, D0);
				move(slongsh, D0, to);
				return;
			}
		}

		if (whfrom == Other && whto == Other) {
			move(sha, from, D0);
			move(sha, D0, to);
			return;
		}
# if 0
		if ((name(sha) == prokhd) && (whfrom == External) &&
		    (whto == Dreg)) {
			/* We need the contents of this address */
			move(sha,from,A0);
			move(sha,A0_p,D0);
			move(sha,D0,to);
			return;
		}
#endif
		instr = ins(sz, ml_mov);
		ins2(instr, sz, sz, from, to, 1);
		if (whto == Areg) {
			have_cond = 0;
		} else {
			set_cond(to, sz);
			if (whfrom == Dreg || whfrom == Areg) {
				set_cond_alt(from);
			}
		}
		return;
	}

	if (name(fe) == null_tag) {
		move_const(sha, sz, L0, to);
		return;
	}

	/* Other cases are dealt with by move_bytes */
	move_bytes(sz, from, to, 2);
	return;
}


/*
    MOVE ADDRESS ROUTINE

    The effective address of from is loaded into to.
*/

void
mova(where from, where to)
{
	int r;
	exp fe = from.wh_exp;
	char nf = name(fe);
	char nt = name(to.wh_exp);

	if (nf == reff_tag) {
		exp s = son(from.wh_exp);
		mova(mw(s, nw(from)), to);
		return;
	}

	if (nt == apply_tag || nt == apply_general_tag ||
	    nt == tail_call_tag) {
		exp s = son(from.wh_exp);
		if (nf == cont_tag) {
			ins1(m_pea, L32, zw(s), 0);
		} else {
			ins1(m_pea, L32, from, 0);
		}
		stack_size -= 32;
		have_cond = 0;
		return;
	}

	switch (nf) {
	case val_tag:
		move(slongsh, from, to);
		return;
	case cont_tag:
	case ass_tag: {
		exp s = son(from.wh_exp);
		if (from.wh_off == 0 && name(s) == name_tag) {
			exp ss = son(s);
			if (!isvar(ss) && !isglob(ss)) {
				move(slongsh, zw(s), to);
				return;
			}
		}
		break;
	}
	}

	if (whereis(to) == Areg) {
		/*
		   if (nf == name_tag && isvar (son (fe))) {
		   	move (slongsh, from, to);
		   	return;
		   }
		 */
		if (nf == name_tag && !isvar(son(fe)) &&
		    ptno(son(fe)) == reg_pl) {
			add(slongsh, mw(fe, 0), mw(zeroe, from.wh_off / 8),
			    to);
		} else {
			ins2(m_lea, L32, L32, from, to, 1);
			have_cond = 0;
		}
		return;
	}


	r = next_tmp_reg();
	regsinproc |= regmsk(r);
	ins2(m_lea, L32, L32, from, register(r), 1);
	have_cond = 0;
	tmp_reg_status = 1;
	move(slongsh, register(r), to);
	return;
}



long
range_max(shape shp)
{
	switch (name(shp)) {
	case scharhd:
		return 0x7f;
	case swordhd:
		return 0x7fff;
	case slonghd:
		return 0x7fffffff;
	case ucharhd:
		return 0xff;
	case uwordhd:
		return 0xffff;
	case ulonghd:
		return 0xffffffff;
	default:
		fprintf(stderr, "Illegal shape in comparison");
	}
	return 0;
}

long
range_min(shape shp)
{
	switch (name(shp)) {
	case scharhd:
		return -0x80;
	case swordhd:
		return -0x8000;
	case slonghd:
		return -0x80000000;
	case ucharhd:
	case uwordhd:
	case ulonghd:
		return 0;
	default:
		fprintf(stderr, "Illegal shape in comparison");
	}
	return 0;
}


/*
    AUXILIARY CHANGE VARIETY ROUTINE

    The value from of shape shf is converted to a value of shape sht and
    moved into to.
*/

void
change_var_sh(shape sht, shape shf, where from, where to)
{
	int instr;

	long szf = shape_size(shf);
	long szt = shape_size(sht);
	bool sgf = is_signed(shf);
	bool sgt = is_signed(sht);

	long whf = whereis(from);
	long wht = whereis(to);

	if (have_overflow()) {
		if (whf == Value) {
			if (((nw(from) < 0) && !is_signed(sht)) ||
			    ((nw(from)) < 0 &&
			     (is_signed(sht) && name(shf) ==ulonghd))) {
				test_overflow(UNCONDITIONAL);
			}
			if (is_signed(sht)) {
				if ((nw(from) < range_min(sht)) ||
				    (nw(from) > range_max(sht))) {
					test_overflow(UNCONDITIONAL);
				}
			} else {
				if ((nw(from) < (unsigned)range_min(sht)) ||
				    (nw(from) > (unsigned)range_max(sht))) {
					test_overflow(UNCONDITIONAL);
				}
			}
		}
	}

	if (whf == Value) {
		long v = dochvar(nw(from), sht);
		move(sht, mnw(v), to);
		return;
	}

	if (name(sht) == bitfhd) {
		sgt = is_signed(sht);
		switch (szt) {
		case 8:
			sht = (sgt ? scharsh : ucharsh);
			break;
		case 16:
			sht = (sgt ? swordsh : uwordsh);
			break;
		default:
			szt = L32;
			sht = (sgt ? slongsh : ulongsh);
			break;
		}
	}

	if (name(shf) == bitfhd) {
		sgf = is_signed(shf);
		switch (szf) {
		case 8:
			shf = (sgf ? scharsh : ucharsh);
			break;
		case 16:
			shf = (sgf ? swordsh : uwordsh);
			break;
		default:
			szf = L32;
			shf = (sgf ? slongsh : ulongsh);
			break;
		}
	}

	if (have_overflow()) {
		bool sw;
		int br_ins;

		/*move(shf,from,D0);*/

		if (is_signed(shf) && !is_signed(sht)) {
			/* if signed -> unsigned, test lt 0.  */

			exp zero_exp = getexp(shf, nilexp, 0, nilexp, nilexp,
					      0, 0, val_tag);
			sw = cmp(shf, from, zw(zero_exp), tst_ls);
			br_ins = branch_ins(tst_ls, sw, 1,
					    is_floating(name(shf)));
			test_overflow2(br_ins);
			kill_exp(zero_exp, zero_exp);
		}

		if (is_signed(sht) && (name(shf) == ulonghd)) {
			/*
			 * Treat the unsigned value as signed and check .lt.
			 * zero.
			 */
			int br_ins;
			exp zero_exp = getexp(slongsh, nilexp, 0, nilexp,
					      nilexp, 0, 0, val_tag);
			sw = cmp(slongsh, from, zw(zero_exp), tst_ls);
			br_ins = branch_ins(tst_ls, sw, 1,
					    is_floating(name(shf)));
			test_overflow2(br_ins);
			kill_exp(zero_exp, zero_exp);
		}

		if(name(sht) <= name(shf)) {
			/* shortening variety */
			exp max_val = getexp(sht, nilexp, 0, nilexp, nilexp, 0,
					     range_max(sht), val_tag);
			exp min_val = getexp(sht, nilexp, 0, nilexp, nilexp, 0,
					     range_min(sht), val_tag);

			int br_ins;

			if (whf != Dreg) {
				move(shf, from, D0);
				from = D0;
				whf = Dreg;
			}

			/*
			 * If value is a char or word we must sign-extend it,
			 * as the checks are done using long arithmetic.
			 */
			if (is_signed(shf) && (szf < 32)) {
				ins1((szf == 16) ? m_extl : m_extbl, 32, from,
				     1);
			}

			sw = cmp(is_signed(sht) ?slongsh : ulongsh, from,
				 zw(max_val), tst_gr);
			br_ins = branch_ins(tst_gr, sw, is_signed(sht),
					    is_floating(name(sht)));
			test_overflow2(br_ins);
			sw = cmp(is_signed(sht) ? slongsh : ulongsh, from,
				 zw(min_val), tst_ls);
			br_ins = branch_ins(tst_ls, sw, is_signed(sht),
					    is_floating(name(sht)));
			test_overflow2(br_ins);

			kill_exp(max_val, max_val);
			kill_exp(min_val, min_val);
		}
	}

	if (szt <= szf) {
		if (whf == Parameter) {
			where adj;
			adj = mw(from.wh_exp, from.wh_off + szf - szt);
			move(sht, adj, to);
			return;
		}
		if (szt == szf || whf == Dreg) {
			move(sht, from, to);
			return;
		}
		if (wht == Dreg) {
			move(shf, from, to);
			return;
		}
		move(shf, from, D0);
		move(sht, D0, to);
		return;
	}

	if (sgf && sgt && szf == 16 && szt == 32) {
		/*
		 * The instruction "mov.w <ea>, %an" automatically sign
		 * extends.
		 */
		if (wht == Areg) {
			move(shf, from, to);
			return;
		}
		if (wht != Dreg) {
			int r = next_tmp_reg();
			move(shf, from, register(r));
			tmp_reg_status = 1;
			move(sht, register(r), to);
			regsinproc |= regmsk(r);
			return;
		}
	}

	if (sgf) {
		bool d;
		where dest;
		if (wht == Dreg) {
			dest = to;
			move(shf, from, dest);
			d = 0;
		} else {
			if (whf == Dreg) {
				/* Extension is non-intrusive */
				dest = from;
			} else {
				dest = D0;
				move(shf, from, dest);
			}
			d = 1;
		}
		if (szf == 8) {
			instr = (szt == 16 ? m_extw : m_extbl);
		} else {
			instr = m_extl;
		}
		ins1(instr, szt, dest, 1);
		set_cond(dest, szt);
		if (d) {
			move(sht, dest, to);
		}
	} else {
		if (wht == Dreg) {
			if (eq_where(to, from)) {
				long v = (szf == 8 ? 0xff : 0xffff);
				if (!eq_where(to, D0)) {
					and(slongsh, mnw(v), to, to);
				}
				return;
			}
		}
		move(slongsh, zero, D0);
		move(shf, from, D0);
		move(sht, D0, to);
		return;
	}
	return;
}


/*
    MAIN CHANGE VARIETY ROUTINE

    The value from is converted to a value of shape sha and moved into to.
*/

void
change_var(shape sha, where from, where to)
{
	shape shf = sh(from.wh_exp);
	change_var_sh(sha, shf, from, to);
	return;
}

/*
    FIND APPROPRIATE BRANCH INSTRUCTION TYPE

    This routine returns the appropriate branch instruction for test number
    test_no, which should be switched if sw is 0.  sf indicates whether
    a floating-point instruction should be used.  If not, sg indicates
    whether a signed or unsigned instruction should be used.
*/

int
branch_ins(long test_no, int sw, int sg, int sf)
{
	int r = test_no;
	if (!sw) {
		switch (r) {
		case tst_le:
			r = tst_ge;
			break;
		case tst_ls:
			r = tst_gr;
			break;
		case tst_ge:
			r = tst_le;
			break;
		case tst_gr:
			r = tst_ls;
			break;
		case tst_ngr:
			r = tst_nls;
			break;
		case tst_nge:
			r = tst_nle;
			break;
		case tst_nls:
			r = tst_ngr;
			break;
		case tst_nle:
			r = tst_nge;
			break;
		}
	}
	switch (r) {
	case tst_eq:
		/* Equal */
		return sf ? m_fbeq : m_beq;
	case tst_neq:
		/* Not equal */
		return sf ? m_fbne : m_bne;
	case tst_le:
		/* Less than or equals */
		if (sf) {
			return m_fble;
		}
		return sg ? m_ble : m_bls;
	case tst_ls:
		/* Less than */
		if (sf) {
			return m_fblt;
		}
		return sg ? m_blt : m_bcs;
	case tst_ge:
		/* Greater than or equals */
		if (sf) {
			return m_fbge;
		}
		return sg ? m_bge : m_bcc;
	case tst_gr:
		/* Greater than */
		if (sf) {
			return m_fbgt;
		}
		return sg ? m_bgt : m_bhi;
	case tst_ngr:
		/* Not greater than */
		if (sf) {
			return m_fbngt;
		}
		return sg ? m_ble : m_bls;
	case tst_nge:
		/* Not greater than or equals */
		if (sf) {
			return m_fbnge;
		}
		return sg ? m_blt : m_bcs;
	case tst_nls:
		/* Not less than */
		if (sf) {
			return m_fbnlt;
		}
		return sg ? m_bge : m_bcc;
	case tst_nle:
		/* Not less than or equals */
		if (sf) {
			return m_fbnle;
		}
		return sg ? m_bgt : m_bhi;
	}
	error(ERROR_SERIOUS, "Illegal test");
	return m_dont_know;
}


/*
    OUTPUT CONDITIONAL JUMP

    A jump to the label indicated by jr is output.  test_no, sw, sg and sf
    have the same meanings as in branch_ins.
*/

void
branch(long test_no, exp jr, int sg, int sw, int sf)
{
	make_jump(branch_ins(test_no, sw, sg, sf), ptno(jr));
	return;
}