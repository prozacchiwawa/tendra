/*
    Copyright (c) 1993 Open Software Foundation, Inc.


    All Rights Reserved


    Permission to use, copy, modify, and distribute this software
    and its documentation for any purpose and without fee is hereby
    granted, provided that the above copyright notice appears in all
    copies and that both the copyright notice and this permission
    notice appear in supporting documentation.


    OSF DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING
    ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
    PARTICULAR PURPOSE.


    IN NO EVENT SHALL OSF BE LIABLE FOR ANY SPECIAL, INDIRECT, OR
    CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
    LOSS OF USE, DATA OR PROFITS, WHETHER IN ACTION OF CONTRACT,
    NEGLIGENCE, OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION
    WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
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



/**********************************************************************
$Author$
$Date$
$Revision$
$Log$
Revision 1.2  2002/01/27 10:28:11  asmodai
Fix spelling of separate and derived words.

Revision 1.1  2002/01/26 21:31:26  asmodai
Initial version of TenDRA 4.1.2.

 * Revision 1.2  1998/02/04  15:48:59  release
 * Added OSF copyright message.
 *
 * Revision 1.1.1.1  1998/01/17  15:55:57  release
 * First version to be checked into rolling release.
 *
 * Revision 1.2  1996/10/04  16:02:44  pwe
 * add banners and mod for PWE ownership
 *
**********************************************************************/


/**********************************************************************
		move.c

	The procedure move produces code to move a value from a to the
destination dest. This takes the form of a switch test on the parameter
a (type ans) which is either a reg, freg instore or bitad value. In
each of the three cases the ans field of the dest is similarly dealt
with to determine the necessary instructions for the move. Sizes and
alignment are taken from the ash field of the destination.

Delivers register used if 1-word destination is instore; otherwise NOREG.

**********************************************************************/
#include "config.h"
#include "memtdf.h"
#include "codegen.h"
#include "geninst.h"

#include "myassert.h"
#include "comment.h"
#include "proc.h"			/* for mem_temp() */
#include "maxminmacs.h"
#include "makecode.h"

#include "move.h"


#define	MAX_STEPS_INLINE_MOVE	12	/* 24 instructions */


#define NBITMASK(n)		((unsigned long)( (n)==32 ? ~0L : ((1<<(n))-1) ))


/*
 *	ins_sgn_pair[FALSE]		unsigned instruction
 *	ins_sgn_pair[TRUE]		signed instruction
 */
typedef Instruction_P ins_sgn_pair[2 /* FALSE..TRUE */ ];

static /* const */ ins_sgn_pair ld_ins_sz[] =
{
   /* 0 */	{&INSTRUCTION_I_NIL,    &INSTRUCTION_I_NIL},
   /* 8 */	{&INSTRUCTION_i_lbz,    &INSTRUCTION_i_lbz},	/* no signed byte load on POWER,
                                               * sign bit must be propagated after load */
   /* 16 */	{&INSTRUCTION_i_lhz,    &INSTRUCTION_i_lha},
   /* 24 */	{&INSTRUCTION_I_NIL,	&INSTRUCTION_I_NIL},
   /* 32 */	{&INSTRUCTION_i_l,	&INSTRUCTION_i_l},
   /* 40 */	{&INSTRUCTION_I_NIL,	&INSTRUCTION_I_NIL},
   /* 48 */	{&INSTRUCTION_I_NIL,	&INSTRUCTION_I_NIL},
   /* 56 */	{&INSTRUCTION_I_NIL,	&INSTRUCTION_I_NIL},
   /* 64 */	{&INSTRUCTION_I_NIL,	&INSTRUCTION_I_NIL}
};

static /* const */ ins_sgn_pair st_ins_sz[] =
{
   /* 0 */	{&INSTRUCTION_I_NIL,	&INSTRUCTION_I_NIL},
   /* 8 */	{&INSTRUCTION_i_stb,	&INSTRUCTION_i_stb},
   /* 16 */	{&INSTRUCTION_i_sth,	&INSTRUCTION_i_sth},
   /* 24 */	{&INSTRUCTION_I_NIL,	&INSTRUCTION_I_NIL},
   /* 32 */	{&INSTRUCTION_i_st,	&INSTRUCTION_i_st},
   /* 40 */	{&INSTRUCTION_I_NIL,	&INSTRUCTION_I_NIL},
   /* 48 */	{&INSTRUCTION_I_NIL,	&INSTRUCTION_I_NIL},
   /* 56 */	{&INSTRUCTION_I_NIL,	&INSTRUCTION_I_NIL},
   /* 64 */	{&INSTRUCTION_I_NIL,	&INSTRUCTION_I_NIL}
};



/* the ld instruction for object sized bits, and sgned or not */
Instruction_P i_ld_sz PROTO_N ((bits,sgned)) PROTO_T (int bits X int sgned)
{
  ASSERT((bits&7)==0);
  ASSERT(bits<=64);
  ASSERT(ld_ins_sz[(bits)/8][sgned]!=I_NIL);
  return ld_ins_sz[(bits)/8][sgned];
}


/* the st instruction for object sized bits */
Instruction_P i_st_sz PROTO_N ((bits)) PROTO_T (int bits)
{
  ASSERT((bits&7)==0);
  ASSERT(bits<=64);
  ASSERT(st_ins_sz[(bits)/8][0]!=I_NIL);
  return st_ins_sz[(bits)/8][0];
}


/* load address represented by is into reg */
void ld_addr PROTO_N ((is,reg)) PROTO_T (instore is X int reg)
{
  COMMENT1("ld_addr: adval=%d", is.adval);

  if (is.adval)
  {
    if (IS_FIXREG(is.b.base))
    {
      rir_ins(i_a, is.b.base, is.b.offset, reg);
    }
    else
    {
      set_ins(is.b, reg);
    }
  }
  else
  {
    ld_ins(i_l, is.b, reg);
  }
}


/* get address represented by is into a reg */
int addr_reg PROTO_N ((is,regs)) PROTO_T (instore is X long regs)
{
  int r;

  COMMENT1("addr_reg: adval=%d", is.adval);

  if (is.adval && IS_FIXREG(is.b.base) && is.b.offset == 0)
  {
    /* simply return base reg */
    return is.b.base;
  }

  /* otherwise load address into reg */
  r = getreg(regs);
  ld_addr(is, r);
  return r;
}


/* store, sorting out temp reg required */
static void store PROTO_N ((st,r,is,regs)) PROTO_T (Instruction_P st X int r X instore is X long regs)
{
  if (is.adval)		/* is the value an address? */
  {
    if (IS_FIXREG(is.b.base))
    {
      st_ro_ins(st, r, is.b);comment(NIL);
    }
    else if (IMM_SIZE(is.b.offset))
    {
      if (freeregs(regs) >= 1)
      {
	/* load base address reg, then store using offset */
	int addr_reg = getreg(regs);
	baseoff b;
	
	b.base = is.b.base;
	b.offset = 0;
	set_ins(b, addr_reg);
	
	b.base = addr_reg;
	b.offset = is.b.offset;
	st_ro_ins(st, r, b);comment(NIL);
      }
      else
      {
	/* st_ins will do this correctly with R_TMP0 using an extra instruction */
	st_ins(st, r, is.b);
      }
    }
    else
    {
      /* st_ins would need 2 tmp regs which are not available */
      baseoff b;
      
      b.base = getreg(regs);
      b.offset = 0;

      /* address+offset into b.base */
      set_ins(is.b, b.base);

      /* store r to [b+0] */
      st_ro_ins(st, r, b);comment(NIL);
    }
  }
  else
  {
    baseoff b;

    ASSERT(r!=R_TMP0);
#if 0
    b.base = R_TMP0;
#else 
    b.base = getreg(regs);
#endif
    b.offset = 0;
    ld_ins(i_l, is.b, b.base);
    st_ro_ins(st, r, b);comment(NIL);
  }
}


#if 0
/*
 * Copy a large inmem object with a loop.
 * Compact code, but slower than loopmove2() so no longer used.
 */
static void loopmove1
    PROTO_N ((iss,isd,bytes_per_step,no_steps,ld,st,regs))
    PROTO_T (instore iss X instore isd X int bytes_per_step X int no_steps X
	     Instruction_P ld X Instruction_P st X long regs)
{
  /*
   * Copy with loop.
   *
   * Currently generate:
   *
   *		!%srcptr and %destptr set
   *		lil	%cnt,bytes
   *	loop:
   *		ai.	%cnt,%cnt,-bytes_per_step
   *		lx	%tmp,[%srcptr+%cnt]
   *		stX	%tmp,[%destptr+%cnt]
   *		bnz	loop
   *
   * +++ unroll, and use two copy regs to separate ld and st using same reg
   * +++ use CR
   * +++ use lu/stu
   * +++ use lsi/stsi
   */

  int srcptr_reg;
  int destptr_reg;
  int cnt_reg;
  int copy_reg;
  int loop = new_label();

  COMMENT("loopmove1: loop move");

  /* moves of addresses not handled by this long move */
  ASSERT(!iss.adval);

  ASSERT(bytes_per_step <= 4);	/* only using 1 word regs */

  cnt_reg = getreg(regs);
  regs |= RMASK(cnt_reg);

  ASSERT(!iss.adval);
  iss.adval = 1;	/* we want address of value */
  srcptr_reg = addr_reg(iss, regs);
  regs |= RMASK(srcptr_reg);

  destptr_reg = addr_reg(isd, regs);
  regs |= RMASK(destptr_reg);

  copy_reg = R_TMP0;

  ld_const_ins(bytes_per_step * no_steps, cnt_reg);

  set_label(loop);

  rir_ins(i_a_cr, cnt_reg, -bytes_per_step, cnt_reg);
  ld_rr_ins(ld, srcptr_reg, cnt_reg, copy_reg);
  st_rr_ins(st, copy_reg, destptr_reg, cnt_reg);

  bc_ins(i_bnz, 0, loop,LIKELY_TO_JUMP);
  clear_reg(cnt_reg);
}
#endif


/*
 * Copy a large inmem object with a loop, using only 2 regs and R_TMP.
 */
static void loopmove2
    PROTO_N ((iss,isd,bytes_per_step,no_steps,ld,st,regs))
    PROTO_T (instore iss X instore isd X int bytes_per_step X int no_steps X
	     Instruction_P ld X Instruction_P st X long regs)
{
  /*
   * Copy with loop, need 2 regs and R_TMP.
   *
   * Currently generate:
   *
   *		!%srcptr and %destptr set in regs that will be changed
   *		lil	%tmp,steps
   *		mtctr	%tmp
   *		ai	%srcptr,%srcptr,-bytes_per_step [if needed]
   *		ai	%dstptr,%dstptr,-bytes_per_step [if needed]
   *	loop:
   *		lXu	%tmp,[%srcptr+bytes_per_step]
   *		stXu	%tmp,[%destptr+bytes_per_step]
   *		bdn	loop
   *
   * +++ do extra plain ld/st outside loop to avoid decrements
   * +++ use lsi/stsi
   */

  Instruction_P ldu, stu;
  int srcptr_reg;
  int destptr_reg;
  baseoff src_bo;
  baseoff dest_bo;
  int copy_reg;
  int loop = new_label();

  COMMENT("loopmove2: loop move");

  ASSERT(bytes_per_step <= 4);	/* only using 1 word regs */

  switch(bytes_per_step)
  {
  case 1:	ldu = i_lbzu; stu = i_stbu; break;
  case 2:	ldu = i_lhzu; stu = i_sthu; break;
  case 4:	ldu = i_lu; stu = i_stu; break;
  default:	fail("bad bytes_per_step in loopmove");
  }

  ld_const_ins(no_steps, R_TMP0);
  mt_ins(i_mtctr, R_TMP0);

  /* moves of addresses not handled by this long move */
  ASSERT(!iss.adval);
  iss.adval = 1;	/* we want address of value */
  iss.b.offset -= bytes_per_step;
  srcptr_reg = getreg(regs);
  regs |= RMASK(srcptr_reg);
  ld_addr(iss, srcptr_reg);

  destptr_reg = getreg(regs);
  regs |= RMASK(destptr_reg);
  if (isd.adval)
  {
    isd.b.offset -= bytes_per_step;
    ld_addr(isd, destptr_reg);
  }
  else
  {
    ld_addr(isd, destptr_reg);
    rir_ins(i_a, destptr_reg, -bytes_per_step, destptr_reg);
  }

  copy_reg = R_TMP0;

  set_label(loop);

  src_bo.base = srcptr_reg;
  src_bo.offset = bytes_per_step;
  ld_ro_ins(ldu, src_bo, copy_reg);comment(NIL);
  
  dest_bo.base = destptr_reg;
  dest_bo.offset = bytes_per_step;
  st_ro_ins(stu, copy_reg, dest_bo);comment(NIL);
  
  uncond_ins(i_bdn, loop);

  clear_reg(srcptr_reg);
  clear_reg(destptr_reg);
}


/*
 * Copy a large inmem object with unrolled loop, using 3 regs and R_TMP.
 */
static void loopmove3
    PROTO_N ((iss,isd,bytes_per_step,no_steps,ld,st,regs))
    PROTO_T (instore iss X instore isd X int bytes_per_step X int no_steps X
	     Instruction_P ld X Instruction_P st X long regs)
{
  /*
   * Copy with unrolled loop, need 3 regs and R_TMP.
   *
   * Currently generate:
   *
   *		!%srcptr and %destptr set in regs that will be changed
   *		ai	%srcptr,%srcptr,-bytes_per_step [if needed]
   *		ai	%dstptr,%dstptr,-bytes_per_step [if needed]
   *		lXu	%tmp2,[%srcptr+bytes_per_step]	[if needed]
   *		lil	%tmp1,half_steps
   *		mtctr	%tmp1
   *		stXu	%tmp2,[%destptr+bytes_per_step]	[if needed]
   *	loop:
   *		lXu	%tmp1,[%srcptr+bytes_per_step]
   *		lXu	%tmp2,[%srcptr+bytes_per_step]
   *		stXu	%tmp1,[%destptr+bytes_per_step]
   *		stXu	%tmp2,[%destptr+bytes_per_step]
   *		bdn	loop
   *
   * +++ use lsi/stsi
   */

  int half_no_steps = no_steps/2;
  Instruction_P ldu, stu;
  int srcptr_reg;
  int destptr_reg;
  baseoff src_bo;
  baseoff dest_bo;
  int copy1_reg;
  int copy2_reg;
  bool decr_destptr_reg;
  int loop = new_label();

  COMMENT("loopmove3: loop move");

  ASSERT(bytes_per_step <= 4);	/* only using 1 word regs */
  ASSERT(half_no_steps>=1);

  switch(bytes_per_step)
  {
  case 1:	ldu = i_lbzu; stu = i_stbu; break;
  case 2:	ldu = i_lhzu; stu = i_sthu; break;
  case 4:	ldu = i_lu; stu = i_stu; break;
  default:	fail("bad bytes_per_step in loopmove");
  }

  /* moves of addresses not handled by this long move */
  ASSERT(!iss.adval);
  iss.adval = 1;	/* we want address of value */
  iss.b.offset -= bytes_per_step;
  srcptr_reg = getreg(regs);
  regs |= RMASK(srcptr_reg);
  ld_addr(iss, srcptr_reg);

  destptr_reg = getreg(regs);
  regs |= RMASK(destptr_reg);
  if (isd.adval)
  {
    if (2*half_no_steps == no_steps)
    {
      /* no opportunity to avoid decr later */
      isd.b.offset -= bytes_per_step;
      decr_destptr_reg = 0;
    }
    else
    {
      decr_destptr_reg = 1;
    }
    ld_addr(isd, destptr_reg);
  }
  else
  {
    ld_addr(isd, destptr_reg);
    decr_destptr_reg = 1;
  }

  copy1_reg = R_TMP0;
  copy2_reg = getreg(regs);
  regs |= RMASK(copy2_reg);

  src_bo.base = srcptr_reg;
  src_bo.offset = bytes_per_step;

  dest_bo.base = destptr_reg;
  dest_bo.offset = bytes_per_step;

  if (2*half_no_steps < no_steps)
    ld_ro_ins(ldu, src_bo, copy2_reg);comment(NIL);
  
  ld_const_ins(half_no_steps, copy1_reg);
  mt_ins(i_mtctr, copy1_reg);

  if (2*half_no_steps < no_steps)
  {
    if (decr_destptr_reg)
    {
      /* no need to do the decr, use plain st not stu for first step */
      dest_bo.offset -= bytes_per_step;
      st_ro_ins(st, copy2_reg, dest_bo);comment(NIL);
      dest_bo.offset += bytes_per_step;
    }
    else
    {
      st_ro_ins(stu, copy2_reg, dest_bo);comment(NIL);
    }
  }
  else
  {
    if (decr_destptr_reg)
      rir_ins(i_a, destptr_reg, -bytes_per_step, destptr_reg);
  }

  set_label(loop);

  ld_ro_ins(ldu, src_bo, copy1_reg);comment(NIL);
  ld_ro_ins(ldu, src_bo, copy2_reg);comment(NIL);
  
  st_ro_ins(stu, copy1_reg, dest_bo);comment(NIL);
  st_ro_ins(stu, copy2_reg, dest_bo);comment(NIL);
  
  uncond_ins(i_bdn, loop);

  clear_reg(srcptr_reg);
  clear_reg(destptr_reg);
}


/*
 * Memory to memory move.
 * If copy of object left in a fixed point reg, return reg, otherwise NOREG.
 */
static int moveinstore PROTO_N ((iss,isd,size,al,regs,sgned)) PROTO_T (instore iss X instore isd X int size X int al X long regs X bool sgned)
{
  int bits;
  int bits_per_step;
  int bytes_per_step;
  int no_steps;
  Instruction_P st;
  Instruction_P ld;
  bool unalign = al < 32;

  if (iss.b.base == isd.b.base 
      && iss.b.offset == isd.b.offset 
      && iss.adval==0 
      && isd.adval==1)
  {
    return NOREG;
  }
  
  /* we are limited by 32 bit regs */
  bits_per_step = min(al, 32);

  bytes_per_step = bits_per_step / 8;

  /*
   * .ashsize gives precise size in bits, not as rounded up as if
   * object is an array element. So we round up bits to convenient
   * size, less than alignment.
   */
  bits = (size + bits_per_step - 1) & ~(bits_per_step - 1);

  no_steps = (bits + bits_per_step - 1) / bits_per_step;

  COMMENT2("moveinstore: mem to mem size,align=%d,%d",
	   size, al);
  COMMENT4("moveinstore: mem to mem bits=%d align=%d, bytes_per_step=%d no_steps=%d",
	   bits, al, bytes_per_step, no_steps);

  if ((al % 8) != 0 || (bits % 8) != 0)
  {
    fail("moveinstore: bits mem to mem move");
    return NOREG;
  }

  /*
   * we are assuming the following, eg 8 bit object cannot have 32 bit
   * alignment
   */
  ASSERT((bits % al) == 0);

  ASSERT(bytes_per_step > 0 && bytes_per_step <= 4);
  ASSERT(no_steps > 0);
  ASSERT((no_steps * bytes_per_step) == (bits / 8));

  /* multi step objects by unsigned move, single step special cased below */
  ld = i_ld_sz(bits_per_step, 0 /* unsgned */);
  st = i_st_sz(bits_per_step);

  /* +++ use fp reg for float, except not passed free fp regs */
  /* +++ use actual alignment which may be better than nominal alignment */

  if (no_steps <= MAX_STEPS_INLINE_MOVE)
  {
    /* move in line */

    if (no_steps == 1)
    {
      int r = getreg(regs);	/* register for holding values
				 * transferred */

      if (iss.adval)
      {
	/* generate address of source */
	if (IS_FIXREG(iss.b.base))
	{
	  if (iss.b.offset == 0)
	  {
	    COMMENT("moveinstore: using adval base reg directly");
	    r = iss.b.base;
	  }
	  else
	  {
	    rir_ins(i_a, iss.b.base, iss.b.offset, r);
	  }
	}
	else
	  set_ins(iss.b, r);
      }
      else
      {
	/* load source */
	ld_ins(i_ld_sz(bits_per_step, sgned), iss.b, r);
	if (sgned && bits_per_step == 8)
	{
	  /* POWER has no load signed byte instruction, so propagate sign */
	  adjust_to_size(ulonghd,r,scharhd,r,NO_ERROR_JUMP);
	}
      }

      store(st, r, isd, regs);

      return (unalign) ? NOREG : r;
    }
    else
    {
      /*
       * Move using 2 regs ensuring load delay slot not occupied.
       */
      int ld_steps = no_steps;
      int st_steps = no_steps;

      int r1, r2;	/* regs used to copy object */

      COMMENT("moveinstore: inline move");

      ASSERT(ld_steps >= 2);

      /* moves of addresses not handled by this long move */
      ASSERT(!iss.adval);

      ASSERT(bits_per_step <= 32);	/* only using byte regs */

      r1 = getreg(regs);
      regs |= RMASK(r1);

      r2 = getreg(regs);
      regs |= RMASK(r2);

      if (!IS_FIXREG(iss.b.base))
      {
	/* load source ptr in reg, note R_TMP0 possibly in use for dest */

	int pr = getreg(regs);

	regs |= RMASK(pr);

	COMMENT("moveinstore: load ptr to source");

	set_ins(iss.b, pr);
	iss.b.base = pr;
	iss.b.offset = 0;
      }

      if (!isd.adval)
      {
	int pr = getreg(regs);

	regs |= RMASK(pr);

	COMMENT("moveinstore: dest !adval");
	ld_ins(i_l, isd.b, pr);
	isd.b.base = pr;
	isd.b.offset = 0;
      }
      else if (!IS_FIXREG(isd.b.base))
      {
	int pr = getreg(regs);

	regs |= RMASK(pr);

	COMMENT("moveinstore: load ptr to dest");

	set_ins(isd.b, pr);
	isd.b.base = pr;
	isd.b.offset = 0;
      }

      /* first, pre-load both regs */
      ld_ro_ins(ld, iss.b, r1);comment("moveinstore initial store->reg1");
      ld_steps--;
      iss.b.offset += bytes_per_step;

      ld_ro_ins(ld, iss.b, r2);comment("moveinstore initial store->reg2");
      ld_steps--;
      iss.b.offset += bytes_per_step;

      /*
       * now generate overlapping sequence with ld rX separated from
       * following st rX
       *
       *	st	r1
       *	ld	r1
       *	st	r2
       *	ld	r2
       *
       * while there's still data
       */
      while (st_steps > 0)
      {
	/* st r1 */
	st_ro_ins(st, r1, isd.b);comment("moveinstore reg1->store");
	st_steps--;
	isd.b.offset += bytes_per_step;

	/* ld r1 */
	if (ld_steps > 0)
	{
	  ld_ro_ins(ld, iss.b, r1);comment("moveinstore store->reg1");
	  ld_steps--;
	  iss.b.offset += bytes_per_step;
	}

	/* st r2 */
	if (st_steps > 0)
	{
	  st_ro_ins(st, r2, isd.b);comment("moveinstore reg2->store");
	  st_steps--;
	  isd.b.offset += bytes_per_step;
	}

	/* ld r2 */
	if (ld_steps > 0)
	{
	  ld_ro_ins(ld, iss.b, r2);comment("moveinstore store->reg2");
	  ld_steps--;
	  iss.b.offset += bytes_per_step;
	}
      }

      COMMENT("moveinstore: end inline move");

      ASSERT(ld_steps == 0);

      return NOREG;
    }
  }			/* inline end */
  else
  {
    /*
     * Copy using a loop.
     * scan() has only reseved 2 regs (no more available sometimes),
     * but if more happen to be free use them for an unrolled loop.
     */

    if (freeregs(regs) < 3)
      loopmove2(iss, isd, bytes_per_step, no_steps, ld, st, regs);
    else
      loopmove3(iss, isd, bytes_per_step, no_steps, ld, st, regs);

    return NOREG;
  }
}


/*
 * Generate code to move 'a' to 'dest'.
 * If copy of object left in a fixed point reg, return reg, otherwise NOREG.
 * ans a;
 * a.discrim is an enumeration
 *  it can be inreg
 *            infreg
 *            notinreg
 *            bitad
 *            insomereg
 *            insomefreg
 * a.val is a union anstu
 *            this can be int regans;			 register number 
 *                        freg fregans;
 *                        instore instoreans;
 *                        instore bitadans;
 *                        somefreg somefregans;	      not yet used
 *                        somereg someregans;
 * dest.answhere is an ans
 * dest.ashwhere is an ash
 *               dest.ashwhere.ashsize is the minimum size of bits in a shape
 *               dest.ashwhere.ashalign is the required alignment in bits
 *
 * regs is a long with the bits masked out for which registers you cannot use
 */
int move PROTO_N ((a,dest,regs,sgned)) PROTO_T (ans a X where dest X long regs X bool sgned)
{
  int al = dest.ashwhere.ashalign; /* al is the alignment of the destination */
  int size = dest.ashwhere.ashsize;
  if(size==0)
    return NOREG;
  
  FULLCOMMENT4("move: %d -> %d, dest ashsize,ashalign = %d,%d",
	       a.discrim, dest.answhere.discrim, dest.ashwhere.ashsize, dest.ashwhere.ashalign);
#if 0
  ASSERT((dest.answhere.discrim == inreg && dest.answhere.val.regans == R_0)	/* nowhere */
	 || dest.ashwhere.ashsize > 0);	/* unitialised dest.ashwhere */
#endif

start:

  /* Switch on the source */
  switch (a.discrim)
  {
   case insomereg:
   case insomefreg:
    {
      fail("move: source somereg not specified");
      return NOREG;
    }
   case inreg:
    /* source in fixed point register */
    {
      int r = regalt(a);

      switch (dest.answhere.discrim)
      {
      case inreg:
	/* source and dest in fixed register */
	{
	  int rd = regalt(dest.answhere);

	  if (rd != R_0 /* nowhere */ && rd != r)
	  {
	    /* move reg r to reg rd */
	    mov_rr_ins(r, rd);comment(NIL);
	  }
	  return NOREG;
	}			/* end inreg dest */

       case insomereg:
	/* source and dest in fixed register */
	{
	  int *sr = someregalt(dest.answhere);

	  if (*sr != -1)
	  {
	    fail("move: somereg already set");
	  }
	  *sr = r;
	  return NOREG;
	}

       case infreg:
	/* dest in floating point register */
	{
	  freg fr;

	  fr = fregalt(dest.answhere);
	  st_ro_ins(i_st, r, mem_temp(0));comment(NIL);
	  if (fr.dble)
	  {
	    st_ro_ins(i_st, r + 1, mem_temp(4));comment(NIL);
	    ldf_ro_ins(i_lfd, mem_temp(0), fr.fr);
	  }
	  else
	  {
	    ldf_ro_ins(i_lfs, mem_temp(0), fr.fr);
	  }
	  return NOREG;
	}			/* end infreg dest */

       case notinreg:
	/* dest instore */
	{
	  Instruction_P st;

	  if(al==1)
	  {
	    if(size<=8)
	    {
	      al = 8;
	    }
	    else if(size<=16)
	    {
	      al = 16;
	    }
	    else
	    {
	      al = 32;
	    }
	  }
	  st = i_st_sz(al);
	  store(st, r, insalt(dest.answhere), regs);

	  return r;
	}			/* end notinreg dest */
       default:
	break;
      }				/* end switch dest */
    }				/* end inreg a */

   case infreg:
    /* source in floating point register */
    {
      freg fr;

      fr = fregalt(a);
      switch (dest.answhere.discrim)
      {
      case inreg:
	/* dest in fixed point register */
	{
	  int rd = regalt(dest.answhere);

	  if (rd != 0)
	  {
	    /* store and load to move to fixed reg */
	    if (fr.dble)
	    {
	      stf_ins(i_stfd, fr.fr, mem_temp(0));
	      ld_ro_ins(i_l, mem_temp(0), rd);comment(NIL);
	      ld_ro_ins(i_l, mem_temp(4), rd + 1);comment(NIL);
	    }
	    else
	    {
	      stf_ins(i_stfs, fr.fr, mem_temp(0));
	      ld_ro_ins(i_l, mem_temp(0), rd);comment(NIL);
	    }
	  }
	  return NOREG;
	}			/* end inreg dest */

      case insomereg:
	/* source in flt reg, can choose dest reg */
	{
	  int * sr = someregalt(dest.answhere);

	  if (*sr != -1)
	  {
	    fail("move: somereg already set");
	  }
	  *sr = getreg(regs);
	  setregalt(dest.answhere, *sr);
	  goto start;
	}

      case infreg:
	/* source and dest in floating point registers */
	{
	  freg frd;	frd = fregalt(dest.answhere);	/* for XLC compiler bug */

	  if (fr.fr != frd.fr)
	    rrf_ins(i_fmr, fr.fr, frd.fr);

	  return NOREG;
	}			/* end infreg dest */

      case notinreg:
	/* source in flt reg, dest instore */
	{
	  Instruction_P st = (fr.dble) ? i_stfd : i_stfs;
	  instore is;

	  if ((dest.ashwhere.ashsize == 64 && !fr.dble) ||
	      (dest.ashwhere.ashsize == 32 && fr.dble))
	  {
	    fail("inconsistent sizes");
	  }
	  is = insalt(dest.answhere);
	  if (is.adval)
	  {
	    if (fr.dble)
	    {
	      stf_ins(i_stfd, fr.fr, is.b);
	    }
	    else
	    {
	      stf_ins(i_stfs, fr.fr, is.b);
	    }
	  }
	  else
	  {
	    baseoff b;

	    b.base = getreg(regs);
	    b.offset = 0;
	    ld_ro_ins(i_l, is.b, b.base);comment(NIL);
	    stf_ro_ins(st, fr.fr, b);
	  };

	  return ((fr.dble) ? -(fr.fr + 32) : (fr.fr + 32));
	}			/* end notinreg dest */
       default:
	break;
      }				/* end switch dest */
    }				/* end infreg a */

  case notinreg:
    /* source instore */
    {
      /* get into register and repeat */
      instore iss;

      iss = insalt(a);

      if (iss.adval)
	COMMENT("move: source adval");

      if (iss.adval && iss.b.offset == 0 && IS_FIXREG(iss.b.base))
      {
	/* address of [base_reg+0] is base_reg */
	setregalt(a, iss.b.base);
	goto start;
      }
      if(al==1)
      {
	if(size<=8)
	{
	  al = 8;
	}
	else if(size<=16)
	{
	  al = 16;
	}
	else
	{
	  al = 32;
	}
      }
      
      /* determine which load instruction to use from al and adval */

      switch (dest.answhere.discrim)
      {
      case insomereg:
	/* source instore, can choose dest reg */
	{
	  int *sr = someregalt(dest.answhere);

	  if (*sr != -1)
	  {
	    fail("move: somereg already set");
	  }
	  *sr = getreg(regs);
	  setregalt(dest.answhere, *sr);
	  /* and continue to next case */
	}

      case inreg:
	/* source instore, dest in fixpnt reg */
	{
	  int rd = regalt(dest.answhere);

	  if (rd != R_0 /* nowhere */ )
	  {
	    if (iss.adval)
	    {
	      /* generate address of source */
	      if (IS_FIXREG(iss.b.base))
		rir_ins(i_a, iss.b.base, iss.b.offset, rd);
	      else
		set_ins(iss.b, rd);
	    }
	    else
	    {
	      /* load source */
	      ld_ins(i_ld_sz(al, sgned), iss.b, rd);
	      if (sgned && al == 8)
	      {
		/* POWER has no load signed byte instruction, so propagate sign */
		/* +++ word aligned byte: load word then sra 24 */
		/* +++ halfword aligned byte: lha then sra 8 */
		/* +++ 0 offset: lsi 1 byte then sra 24 */
		adjust_to_size(ulonghd,rd,scharhd,rd,NO_ERROR_JUMP);
	      }
	    }
	  }
	  return NOREG;
	}			/* end inreg dest */

      case infreg:
	/* source instore, dest in floating pnt reg */
	{
	  freg frd;

	  frd = fregalt(dest.answhere);

	  ASSERT(!iss.adval);	/* address should never go to float reg */
	  /* allow doubles not to be double aligned in mem, ie param */
	  if (frd.dble)
	  {
	    ldf_ins(i_lfd, iss.b, frd.fr);
	  }
	  else
	  {
	    ldf_ins(i_lfs, iss.b, frd.fr);
	  }
	  return NOREG;
	}			/* end infreg dest */
      case notinreg:
	/* source and dest instore */
	{
	  return moveinstore(iss, insalt(dest.answhere), dest.ashwhere.ashsize, al, regs, sgned);
	}
       default:
	break;
      }				/* end switch dest */
    }				/* end notinreg a */
  }				/* end switch a */

  fail("move not handled");
  return NOREG;
  /*NOTREACHED*/
}

