/*
 * Copyright 2011, The TenDRA Project.
 * Copyright 1997, United Kingdom Secretary of State for Defence.
 * Copyright 1993, Open Software Foundation, Inc.
 *
 * See doc/copyright/ for the full copyright terms.
 */

#include <assert.h>

#include <shared/bool.h>
#include <shared/check.h>
#include <shared/error.h>
#include <shared/xalloc.h>

#include <local/exp.h>

#include <tdf/tag.h>

#include <reader/exp.h>

#include <reader/externs.h>
#include <reader/basicread.h>
#include <reader/main_reads.h>
#include <reader/readglob.h>

#include <construct/installglob.h>
#include <construct/exp.h>

#include <flpt/flpt.h>

#include <main/flags.h>
#include <main/print.h>

#include <refactor/optimise.h>

#include "reg.h"
#include "code_here.h"
#include "tempdecs.h"
#include "weights.h"
#include "procrec.h"
#include "regalloc.h"
#include "make_code.h"
#include "eval.h"
#include "scan.h"
#include "stabs_diag3.h"
#include "translate.h"
#include "stack.h"
#include "frames.h"
#include "stack.h"

long frame_offset(exp id)
{
  exp init_exp = child(id);
  exp p;
  procrec *pr;
  long n = no (id);
  long off = n>>6;

  assert(id->tag == ident_tag);
  for (p = father(id); !IS_A_PROC(p); p = father(p));

  pr = & procrecs[no(p)];
  
	
  if (isparam(id))
  {
    if( init_exp->tag==formal_callee_tag)
    {
      /* Callee parameter accessed through R_FP */
      return (no(init_exp)>>3) + EXTRA_CALLEE_BYTES;
    }
    else
    {
      /* Caller parameter accessed through R_TP */
      return (no(init_exp)>>3) + STACK_ARG_AREA;
    }
  }
  else if (init_exp->tag==caller_name_tag)
  {
    error(ERR_SERIOUS, "Taking env_offset of an identified caller within a postlude");
    return 0;
  }
  else
  {
    /* Local */
    return ( off + (pr->locals_offset>>3) - (pr->frame_size>>3) );
  }
}


void set_up_frame_pointer(procrec * pr, exp e)
{
  long pprops = pr->needsproc.propneeds;
  /* Initialize a few of the fields */
  pr->alloca_proc = proc_has_alloca(e);
  pr->leaf_proc = ( ( pprops & anyproccall ) == 0);
  pr->has_fp = 0;
  pr->has_saved_sp = 0;
  pr->save_all_sregs = 0;
  pr->has_vcallees = 0;
  pr->has_no_vcallers = 0;
  pr->has_tp = 0;
  
  /* 
   * This choosing of what is needed within the proc is very important
   * For a procedure which is not a leaf proc and has had someone loading
   * the value of a label, and someone has grabbed the current_env
   * It means, that they could potentially long jump into this proc.
   * In this case we must save all the s-regs, and also we must insist
   * on a frame pointer.
   * The reason for this, is that there is no way of telling when coding
   * up the long jump whether the proc it is long jumping into has a
   * frame pointer or not.
   * So to make a convention we say that it must have a frame pointer,
   * since this is the only way we can code up the long jump.(by loading fp )
   *
   * In the event of us having a procedure which could be long jumped
   * back to, and the proc has calls to alloca, then the stack pointer can
   * not be restored from the frame pointer, by simply subtracting the 
   * p_frame_size, so it is necessary to save the last value of the stack pointer
   * at a designated place on the stack relative to the frame pointer.
   */
  if( !(pr->leaf_proc) && proc_has_lv(e) && proc_uses_crt_env(e) )
  {
    /* This means someone could call long jump back to this proc */
    pr->save_all_sregs = 1;
    pr->has_fp = 1;
  }
  if ( pr->alloca_proc )
  {
    /* alloca procedures require a frame pointer */
    pr->has_fp = 1;
  }
  if ( pr->alloca_proc && pr->save_all_sregs )
  {
    /* alloca proc which could be long jumped to */
    pr->has_saved_sp = 1;
  }

  if (e->tag==general_proc_tag)
  {
    /* All general_proc_tag's have a frame pointer */
    pr->has_tp = 1;/* +++ is this only really needed for vcallees */
    pr->has_fp = 1;
    pr->has_vcallees = proc_has_vcallees(e);
    pr->has_no_vcallers = !proc_has_vcallers(e);
    /* If has_no_vcallers==1 => Not a varargs
       however has_no_vcallers==0 does not imply varargs it tells us nothing*/
  }
  if (gen_call)
  {
    /* It could be a normal proc with a tail call */
    pr->has_fp = 1;
  }
  pr->no_of_returns = no_of_returns;  
  pr->callee_size = callee_size;
  if (max_callees < 0)
    pr->max_callee_bytes = 0;
  else
    pr->max_callee_bytes = ALIGNNEXT ((max_callees >> 3) + EXTRA_CALLEE_BYTES, 8);
}

void set_up_frame_info(procrec * pr, exp e)
{
  int r;
  long maxargs;
  
  UNUSED(e);

  /*
   * Initialize more fields
   */
  maxargs = pr->needsproc.maxargs;
  pr->locals_space = pr->spacereqproc.stack;      
  if (do_profile)
  {
    pr->leaf_proc = 0;
  }
  if (pr->leaf_proc)
  {
    asm_comment("leaf_proc");
    assert(maxargs==0);
  }
  else
  {
    assert(maxargs >=0);

    if (maxargs < STACK_MIN_MAXARGS*8)
    {
      maxargs = STACK_MIN_MAXARGS*8;	/* at least reg param dump for calls */
    }
  }
  
  /* put on 64 bit boundaries */
  maxargs = ALIGNNEXT(maxargs, 64);
  pr->locals_space = ALIGNNEXT(pr->locals_space, 64);
  
  
  /* find lowest s-reg used for i_stm and i_lm */
  pr->sreg_first_save = R_NO_REG;
  for (r = R_FIRST; r <= R_LAST; r++)
  {
    if ((pr->spacereqproc.fixdump & RMASK(r)) != 0)
    {
      pr->sreg_first_save = r;
      break;
    }
  }
  
  
  asm_comment("gpr use mask = %#lx, lowest = %d", (unsigned long) pr->spacereqproc.fixdump, pr->sreg_first_save);
  assert(pr->sreg_first_save==R_NO_REG || IS_SREG(pr->sreg_first_save));
  
  
  
  /* find lowest float s-reg used */
  pr->sfreg_first_save = FR_NO_REG;
  
  for (r = FR_FIRST; r <= FR_LAST; r++)
  {
    if ((pr->spacereqproc.fltdump & RMASK(r)) != 0)
    {
      pr->sfreg_first_save = r;
      break;
    }
  }
  
  
  asm_comment("fpr use mask = %#lx, lowest = %d", (unsigned long) pr->spacereqproc.fltdump, pr->sfreg_first_save);
  assert(pr->sfreg_first_save==FR_NO_REG || IS_FLT_SREG(pr->sfreg_first_save));
  if (pr->leaf_proc)
  {
    pr->locals_offset = 0;
  }
  else
  {
    pr->locals_offset = STACK_LINK_AREA_SIZE*8 + maxargs;
  }
  pr->frame_size= pr->locals_offset + pr->locals_space;
  /* GRAB TWO WORDS FOR THE SAVED SP */
  if (pr->has_saved_sp)
  {
    pr->frame_size += 64;
    /* Two extra words */
  }
  
  /* grab space for fixed s-reg dump area */
  if (pr->sreg_first_save != R_NO_REG)
  {
    pr->frame_size += ALIGNNEXT((R_LAST-pr->sreg_first_save+1)*32, 64);
  }
  
  /* grab space for float s-reg dump area */
  if (pr->sfreg_first_save != FR_NO_REG)
  {
    pr->frame_size += (FR_LAST-pr->sfreg_first_save+1)*64;
  }
  pr->params_offset = pr->frame_size + STACK_ARG_AREA*8;
  pr->maxargs=maxargs;
}
