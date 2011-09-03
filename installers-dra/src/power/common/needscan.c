/*
 * Copyright (c) 2002-2005 The TenDRA Project <http://www.tendra.org/>.
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
$Author: release $
$Date: 1998/02/04 15:49:01 $
$Revision: 1.2 $
$Log: needscan.c,v $
 * Revision 1.2  1998/02/04  15:49:01  release
 * Added OSF copyright message.
 *
 * Revision 1.1.1.1  1998/01/17  15:55:57  release
 * First version to be checked into rolling release.
 *
 * Revision 1.4  1996/10/14  17:31:56  pwe
 * include called callees in env_size
 *
 * Revision 1.3  1996/10/14  15:53:31  pwe
 * preserve regresult during postlude
 *
 * Revision 1.2  1996/10/04  16:03:03  pwe
 * add banners and mod for PWE ownership
 *
**********************************************************************/


/******************************************************************

		needs_scan.c

	Defines the scan through a program which reorganises it so that all
arguments of operations are suitable for later code-production. The procedure
scan evaluates the register requirements of an exp. The exps are produced
from the decoding process and the various exp -> exp transformations  in
the proc independent (common to other  translators)

******************************************************************/
#include "config.h"
#include "memtdf.h"
#include "codegen.h"

#include "myassert.h"
#include "comment.h"


#include "common_types.h"
#include "exptypes.h"
#include "exp.h"
#include "expmacs.h"
#include "tags.h"
#include "localtypes.h"
#include "procrectypes.h"
#include "bitsmacs.h"
#include "maxminmacs.h"
#include "regable.h"
#include "tempdecs.h"
#include "shapemacs.h"
#include "const.h"
#include "flpt.h"
#include "install_fns.h"
#include "externs.h"

#include "flags.h"

#include "machine.h"
#include "translat.h"
#include "check.h"
#include "makecode.h"
#include "needscan.h"
#include "me_fns.h"
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
#define LOADFROMSTORE(e)\
	(\
	!diagnose \
	&& \
	(\
	 (name(e) == cont_tag && (name(son(e))!= name_tag || INMEMIDENT(son(son(e)))))\
	  || \
	  (name(e) == name_tag && isglob(son(e))) /* load of TOC entry */ \
	)\
	)
int maxfix, maxfloat;		/* the maximum number of t-regs */
bool tail_call;
bool gen_call;
long callee_size;
long max_callees;
long no_of_returns;
int end_param;

/* The following are used for allocating param regs to t-regs */
static int freefixed;
static int freefloat;


static int stparam, fixparam, floatparam;


extern bool do_tlrecursion;

static bool nonevis = 1;
static bool rep_tag_scanned;

int scan_cond(exp *,exp);
needs scan(exp *, exp **);
static void number_caller_parameter(exp);
static void number_callee_parameter(exp);

/* declaration of scan.
   needs is defined in procrectypes.h.
   This is a structure which has two integers giving
   the number of fixed and floating point registers required to contain live values
   in the expression parameters. A further field prop is used for various
   flags about certain forms of exp (mainly idents and procs). The maxargs
   field gives the maximum size in bits for the parameters of all the procs
   called in the exp. The needs of a proc body are preserved in the needs field
   of the procrec (see procrectypes.h).
   */


/***************************************************************
		cca

This procedure effectively inserts a new declaration into an exp. This
is used to stop a procedure requiring more than the available number of
registers.
****************************************************************/

static void cca(exp * *to, exp * x)
{
  if (name((**to)) ==diagnose_tag)
  {
    *to = &son((**to));
  }
  if (x == (*to))
  {
    exp def = *(x);

    /* replace by  Let tg = def In tg Ni */
    exp id = getexp(sh(def), bro(def), last(def), def, nilexp,
		    0, 1, ident_tag);
    exp tg = getexp(sh(def), id, 1, id, nilexp,
		    0, 0, name_tag);

    pt(id) = tg;		/* use of tag */
    bro(def) = tg;		/* bro(def) is body of Let = tg */
    clearlast(def);
    *(x) = id;			/* replace pointer to x by Let */
    return;
  }
  else
  {				/* replace by Let tg = def In ato/def = tg Ni */
    exp def = *(x);
    exp ato = *(*to);
    exp id = getexp(sh(ato), bro(ato), last(ato), def, nilexp,
		    0, 1, ident_tag);
    exp tg = getexp(sh(def), bro(def), last(def), id, nilexp,
		    0, 0, name_tag);

    pt(id) = tg;		/* use of tg */
    bro(def) = ato;		/* ato is body of Let */
    clearlast(def);
    bro(ato) = id;		/* its father is Let */
    setlast(ato);
    *(*to) = id;		/* replace pointer to 'to' by Let */
    *(x) = tg;			/* replace use of x by tg */
    *to = &bro(def);		/* later replacement to same 'to' will be at
				 * body of Let */
    return;
  }
}

/* General useful needs */
static needs onefix = { 1, 0, 0, 0 };			/* needs one fix pt reg */
static needs twofix = { 2, 0, 0, 0 };			/* needs 2 fix pt regs */
static needs onefloat = { 0, 1, 0, 0 };			/* needs 1 flt pt regs */
static needs zeroneeds = { 0, 0, 0, 0 };		/* has no needs */


static needs shapeneeds(shape s)
{				/* this gives the needs for manipulating a
				 * value of shape s */
  if (is_floating(name(s)))
  {
    return onefloat;
  }
  else
  {
    if (valregable(s))
    {
      return onefix;
    }
    else
    {				/* if the shape does not fit into a reg, needs
				 * two fixed regs for moving ... */
      return twofix;
    }
  }
}


static bool complex(exp e)
{				/* these are basically the expressions which
				 * cannot be accessed by a simple load or
				 * store instruction */
  if (name(e) == name_tag ||
     (
       name(e) == cont_tag &&
       name(son(e)) == name_tag
       && isvar(son(son(e)))
      ) ||
      name(e) == val_tag ||
      name(e) == real_tag
     )
  {
    return 0;
  }
  else
  {
    return 1;
  }
}


needs commutative_scan(exp * e, exp * *at)
{
  /*
   * does the scan on commutative and associative operations and may perform
   * various transformations allowed by these properties
   */
  needs a1;
  needs a2;
  exp dad = *(e);
  exp *br = &son(*e);
  exp prev;
  bool dont_commute;

  ASSERT(father(son(*e)) == *e);

  /* scan the first operand - won't be a val_tag */
  a1 = scan(br, at);

  /*
   * if first operand is a proc, or ends with a load instruction,
   * it is not worth doing a commute to a later position
   */
  dont_commute = (a1.fixneeds >= maxfix || (a1.propsneeds & hasproccall))
		 || LOADFROMSTORE(*br);

  do
  {
    exp *prevbr;

    prevbr = br;
    prev = *(br);
    br = &bro(prev);
    a2 = scan(br, at);
    /* scan the next operand ... */
    if (name(*(br))!= val_tag)
    {
      prop pc;
      bool all_treg_needs;

      a1.floatneeds = max(a1.floatneeds, a2.floatneeds);
      pc = a2.propsneeds & hasproccall;
      all_treg_needs = (a2.fixneeds >= maxfix || pc != 0);

      if (!dont_commute
	   &&
	  (all_treg_needs || LOADFROMSTORE(*br))
	   &&
	  (a1.fixneeds < maxfix && (a1.propsneeds & hasproccall) == 0)
	)
      {
	/*
	 * ...its evaluation will call a proc, or ends with a load instruction,
	 * so put it first
	 */

	exp op1 = son(dad);
	exp cop = *(br);
	bool lcop = last(cop);

	bro(prev) = bro(cop);
	if (lcop)
	  setlast(prev);
	bro(cop) = op1;
	clearlast(cop);
	son(dad) = cop;
	br = (prev == op1)? &bro(cop): prevbr;
	dont_commute = 1;
	a1.fixneeds = max(a2.fixneeds, a1.fixneeds + 1);
	a1.propsneeds |= a2.propsneeds;
	a1.maxargs = max(a1.maxargs, a2.maxargs);
      }
      else if (all_treg_needs)
      {				/* ... its evaluation would disturb
				 * accumulated result, so replace it by a
				 * newly declared tag */
	cca(at, br);
	a1.fixneeds = max(a1.fixneeds, 2);
	a1.propsneeds = a1.propsneeds | morefix | (pc << 1);
	a1.maxargs = max(a1.maxargs, a2.maxargs);
      }
      else
      {
	/*
	 * ... its evaluation  will not disturb the accumulated result
	 */
	a1.fixneeds = max(a1.fixneeds, a2.fixneeds + 1);
	a1.propsneeds = a1.propsneeds | a2.propsneeds;
      }
    }
  } while (!last(*(br)));

  return a1;
}


needs non_commutative_scan(exp * e, exp * *at)
{
  /*
   * scan non-commutative fix pt operation
   */
  needs l;
  needs r;
  prop pc;
  exp *arg = &son(*e);

  l = scan(arg, at);
  /* scan 1st operand */
  arg = &bro(*arg);
  r = scan(arg, at);
  /* scan second operand ... */
  l.floatneeds = max(l.floatneeds, r.floatneeds);
  pc = r.propsneeds & hasproccall;
  if (r.fixneeds < maxfix && pc == 0)
  {				/* ...it fits into registers */
    l.fixneeds = max(l.fixneeds, r.fixneeds + 1);
    l.propsneeds = l.propsneeds | r.propsneeds;
  }
  else
  {				/* ...it requires new declaration of second
				 * operand */
    cca(at, arg);
    l.fixneeds = max(l.fixneeds, 1);
    l.propsneeds = l.propsneeds | morefix | (pc << 1);
    l.maxargs = max(l.maxargs, r.maxargs);
  }
  return l;
}

static needs fpop(exp * e, exp * *at)
{
  /* scans diadic floating point operation  */
  needs l;
  needs r;
  exp op = *(e);
  prop pcr, pcl;
  exp *arg = &son(op);

  l = scan(arg, at);
  arg = &bro(*arg);
  r = scan(arg, at);
  l.fixneeds = max(l.fixneeds, r.fixneeds);
  pcr = r.propsneeds & hasproccall;
  pcl = l.propsneeds & hasproccall;

  if (r.floatneeds <= l.floatneeds && r.floatneeds < maxfloat && pcr == 0)
  {
    l.floatneeds = max(2, max(l.floatneeds, r.floatneeds + 1));
    l.propsneeds = l.propsneeds | r.propsneeds;
    ClearRev(op);
  }
  else if (pcl == 0 && l.floatneeds <= r.floatneeds && l.floatneeds < maxfloat)
  {
    l.floatneeds = max(2, max(r.floatneeds, l.floatneeds + 1));
    l.propsneeds = l.propsneeds | r.propsneeds;
    SetRev(op);
  }
  else if (r.floatneeds < maxfloat && pcr == 0)
  {
    l.floatneeds = max(2, max(l.floatneeds, r.floatneeds + 1));
    l.propsneeds = l.propsneeds | r.propsneeds;
    ClearRev(op);
  }
  else
  {
    cca(at, arg);
    ClearRev(op);
    l.floatneeds = max(l.floatneeds, 2);
    l.propsneeds = l.propsneeds | morefloat | (pcr << 1);
    l.maxargs = max(l.maxargs, r.maxargs);
  }
  return l;
}
/**********************************************************************
	maxneeds

Calculates a needs value. Each element of which is the maximum of the
corresponding elements in the two parameter needs
**********************************************************************/

static needs maxneeds(needs a, needs b)
{
  needs an;

  an.fixneeds = max(a.fixneeds, b.fixneeds);
  an.floatneeds = max(a.floatneeds, b.floatneeds);
  an.maxargs = max(a.maxargs, b.maxargs);
  an.propsneeds = a.propsneeds | b.propsneeds;
  return an;
}


/**********************************************************************
	maxsequence

**********************************************************************/

static needs maxtup(exp e, exp ** at)
{				/* calculates the needs of a tuple of
				 * expressions; any new declarations required
				 * by a component expression will replace the
				 * component expression */
  exp *stat = &son(e);
  needs an;

  an = zeroneeds;
  if (*stat==nilexp)
  {
    return an;
  }
  while (an = maxneeds(an, scan(stat, at)), !last(*stat))
  {
    stat = &bro(*stat);
  }
  return an;
}


static bool unchanged(exp usedname, exp ident)
{
  /*
   * finds if usedname is only used in cont operation or as result of ident
   * i.e. value of name is unchanged over its scope
   */
  exp uses = pt(usedname);

  while (uses != nilexp)
  {
    if (intnl_to(ident, uses))
    {
      if (!last(uses) || name(bro(uses))!= cont_tag)
      {
	exp z = uses;

	while (z != ident)
	{
	  if (!last(z) ||
	     (name(bro(z))!= seq_tag && name(bro(z))!= ident_tag))
	  {
	    return 0;
	  }
	  z = bro(z);
	}
      }
    }
    uses = pt(uses);
  }
  return 1;
}


static exp *ptr_position(exp e)
{
  exp *a;
  exp dad = father(e);

  if (son(dad) == e)
  {
    a = &son(dad);
  }
  else
  {
    exp sib = son(dad);

    while (bro(sib)!= e)
    {
      sib = bro(sib);
    }
    a = &bro(sib);
  }
  return a;
}



/*
 * The POWER convention for delivering a struct from a procedure is is have an
 * extra pointer parameter in the proc; this means that there always must be
 * space in the calling work-space for the result struct whether or not the
 * value is used e.g. as in f(x); or f(x).a etc. This proc is part of the
 * mechanism to determine whether it is necessary to insert a dummy
 * declaration to ensure that this space exists.
 */
static bool chase(exp sel, exp * e)
{
  /* distribute selection throughout compound expressions */
  bool b = 0;
  exp *one;

  switch (name(*e))
  {
  case ident_tag:
  case seq_tag:
  case rep_tag:
  case labst_tag:
    {
      b = chase(sel, &bro(son(*e)));
      break;
    }
  case solve_tag:
  case cond_tag:
    {
      one = &son(*e);
      for (;;)
      {
	b |= chase(sel, one);
	if (last(*one))
	  break;
	one = &bro(*one);
      }
      break;
    }
  case field_tag:
    {
      if (chase(*e, &son(*e)))
      {
	/* inner field has been distributed */
	exp stare = *e;
	exp ss = son(stare);

	if (!last(stare))
	  clearlast(ss);
	bro(ss) = bro(stare);
	sh(ss) = sh(stare);
	*e = ss;
	return chase(sel, e);
      }				/* ... continue to default */
    }
  default:
    {
      if (son(sel)!= *e)
      {				/* only change if not outer */
	exp stare = *e;
	exp newsel = getexp(sh(sel), bro(stare), last(stare), stare, nilexp,
			    props(sel), no(sel), name(sel));

	*e = newsel;
	bro(stare) = newsel;
	setlast(stare);
	b = 1;
      }
    }
  }
  if (b)
    sh(*e) = sh(sel);
  return b;
}



/********************************************************************
		scan

	This procedure works out register requirements of an exp. At each
call the fix field of the needs is the number of fixpnt registers required to contain live values to evaluate this expression. This never exceeds maxfix
because if it would have, a new declaration is introduced in the exp tree (similarly for floating regs and maxfloat). In these cases the prop field will
contain the bits morefix (or morefloat).
	Scan also works out various things concerned with proc calls.
The maxargs field contains the max size in bits of the space required for the
parameters of all the procedures called in the exp. An exp proc call
produces a hasproccall bit in the prop field, if this is transformed as part of
the definition of a new declaration the bit is replaced by a usesproccall. The
distinction is only used in unfolding nested proc calls; POWER requires this to
be done statically. The condition that a proc exp is a leaf (i.e no proc calls)
is that its prop contains neither bit.
	If an ident exp is suitable, scan marks the props of ident with
either inreg or infreg bits to indicate that a t reg may be used for this tag.

	A thorough understanding of needs along with other procedures
that do switch(name(exp)) requires a knowledge of the meaning of the fields
of the exp in each case.

********************************************************************/


needs scan(exp * e, exp * *at)
{
  /*
   * e is the expression to be scanned, at is the place to put any new decs .
   * NB order of recursive calls with same at is critical
   */
  exp ste = *(e);
  int nstare = name(ste);
  static long exp_num = 0;		/* count of exps we evaluate */

  exp_num++;
  ASSERT(*e != nilexp);


  switch (nstare)
  {
   case 0:
    {/* zero_tag */
      return zeroneeds;
    };

   case compound_tag:
   case nof_tag:
   case concatnof_tag:
   case ncopies_tag:
    {
      needs nl;
      bool cantdo;
      exp dad;

      if (nstare==nof_tag && son(ste) ==nilexp)
	return zeroneeds;

      if (name(ste) == ncopies_tag && name(son(ste))!= name_tag
	  && name(son(ste))!= val_tag)
      {
	nl = scan(&son(*e), at);
	cca(at, &son(*e));
      }
      else
      {
	nl = maxtup(*(e), at);
      }
      ste = *e;
      dad = father(ste);
      if (name(dad) == compound_tag ||
	  name(dad) == nof_tag ||
	  name(dad) == concatnof_tag)
      {
	cantdo = 0;
      }
      else if (last(ste))
      {
	if (name(bro(ste)) == ass_tag)
	{
	  exp a = son(bro(ste));

	  cantdo = (name(a)!= name_tag || !isvar(son(a)));
	}
	else
	{
	  cantdo = 1;
	}
      }
      else if (last(bro(ste)))
      {
	cantdo = (name(bro(bro(ste)))!= ident_tag);
      }
      else
      {
	cantdo = 1;
      }


      if (cantdo)
      {
	/* can only deal with tuples in simple assignment or identity */
	int prps = (nl.propsneeds & hasproccall) << 1;

	cca(at, ptr_position(ste));
	nl = shapeneeds(sh(*(e)));
	nl.propsneeds |= morefix;
	nl.propsneeds |= prps;
      }

      nl.fixneeds = max(nl.fixneeds,2);

      return nl;
    };

  case cond_tag:
    {
      if (scan_cond(e, nilexp)!=0)
      {
	return scan(e, at);
      }                   /* Else goto next case */
    }
    /*FALLTHROUGH*/
   case rep_tag:
   case solve_tag:
    {
      exp *stat;
      exp *statat;
      needs an;

      stat = &son(*e);
      statat = stat;
      an = zeroneeds;
      rep_tag_scanned=0;
      /*
       * Simply scan each argument
       * The arguments are effectively independent pieces
       * of code for these constructions
       */
      /***********************************************************/
      /*    _        _________                                   */
      /*   |_|----->|        _|                                  */
      /*  /         |    _  |_|                                  */
      /* e          |___|_|___|                                  */
      /*               / |                                       */
      /*              /  |                                       */
      /*          stat(1)|       stat(2)       stat(3)           */
      /*             ____v____  /  _________  /  _________       */
      /*            |        _|/  |        _|/  |        _|      */
      /*            |    _  |_|-->|    _  |_|-->|    _  |_|-->   */
      /*            |___|_|___|   |___|_|___|   |___|_|___|      */
      /*                                                         */
      /***********************************************************/
      while (an = maxneeds(an, scan(stat, &statat)), !last(*(stat)))
      {
	stat = &bro(*stat);
	statat = stat;
      }
      if (name(*e) ==rep_tag)
      {
	if (rep_tag_scanned==0)
	{
	  setinnermost(*e);
	}
	else
	{
	  clearinnermost(*e);
	}
      }
      rep_tag_scanned=1;
      if ((an.propsneeds & usesproccall)!= 0)
      {
	an.propsneeds |= hasproccall;
      }
      return an;
    }

  case labst_tag:
    {
      exp *stat;
      exp *statat;
      needs an;
      int expn = exp_num;

      ASSERT(!last(son(*e)));
      ASSERT(last(bro(son(*e))));

      /****************************************/
      /*    _     _________                   */
      /*   |_|-->| labst   |                  */
      /*  /      |    _    |                  */
      /* e       |___|_|___|   stat           */
      /*              |       /               */
      /*          ____v____  /  ________      */
      /*         | clear  _|/  |       _|     */
      /*         |       |_|-->|      |_|     */
      /*         |_________|   |________|     */
      /*                                      */
      /****************************************/
      stat = &bro(son(*e));
      statat = stat;
      an = scan(stat, &statat);

      if ((an.propsneeds & usesproccall)!= 0)
      {
	an.propsneeds |= hasproccall;
      }

      /*
       * ptno(son()) is set to exp number so that make_code can estimate
       * distances of conditional branches, which is limited on POWER.
       */
      ptno(son(*e)) = expn;

      return an;
    }


/*********************************************************************
  ident_tag

shape of exp is body,
son is def, brother of son is body,
ptr of ident exp is chain of uses
*********************************************************************/
   case ident_tag:
    {
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
      if (pt(stare) == nilexp)
      {
	/* no uses, should have caonly flag set (not already set for params) */
	setcaonly(stare);
      }
#endif
      if (isvar(stare) && (!iscaonly(stare) || all_variables_visible))
      {
	setvis(stare);
      }

      if (isparam(stare))
      {
	if (name(son(stare))!=formal_callee_tag)
	{
	  number_caller_parameter(stare);
	}
	else
	{
	  number_callee_parameter(stare);
	}
      }

      nonevis &= !isvis(stare);

      /* Scan the body of the ident */
      arg = &bro(son(stare));
      bdy = scan(arg, &arg);
      /* Scan the def of the ident */
      arg = &son(stare);
      defneeds = scan(arg, &arg);
      ASSERT(stare == *e);

      nonevis = old_nonevis;
      t = son(stare);
      s = bro(t);
      fxregble = fixregable(stare);
      flregble = floatregable(stare);
      uses_R_RESULT = (bdy.propsneeds & uses_R_RESULT_bit)!=0;
      uses_FR_RESULT = (bdy.propsneeds & uses_FR_RESULT_bit)!=0;
/*****************************************************************************/
      if (name(son(stare)) ==caller_name_tag)
      {
	/*
	 * IDENT is a caller in postlude
	 */
	no(stare) = R_NO_REG;
	/* At present all callers in postludes are only allowed on the stack*/
	/* This is because of the problems created by nested postludes */
      }
/*****************************************************************************/
      else if (isparam(stare) && name(son(stare)) ==formal_callee_tag)
      {
	/*
	 * IDENT is a callee parameter
	 */
	no(stare) = R_NO_REG;
      }
/*****************************************************************************/
      else if (isparam(stare) && name(son(stare))!=formal_callee_tag)
      {
	/*
	 * IDENT is a caller parameter
	 */
	if (do_profile)
	{
	  /* mcount is called after the last param is dealt with */
	  /* So we must put all params on the stack or in s-regs */
	  bdy.propsneeds |= hasproccall;
	}
	no(stare) =R_NO_REG;
      }
/*****************************************************************************/
      else
      {
	/*
	 * IDENT is a normal ident
	 */
	ASSERT(!isparam(*e));


	if (!isvis(*e) &&
	   (bdy.propsneeds & anyproccall) ==0 &&
	   (
	    (uses_R_RESULT==0 && fxregble) ||
	    (uses_FR_RESULT==0 && flregble)
	    ) &&
	   (
	     name(t) == apply_tag || /* Let a := f()*/
	    (name(s) == seq_tag && name(bro(son(s))) == res_tag &&
	      name(son(bro(son(s)))) == cont_tag && isvar(stare) &&
	      name(son(son(bro(son(s))))) == name_tag &&
	      son(son(son(bro(son(s))))) == stare
	      )			/* Let a := ..; return cont a */
	    )
	   )
	{
	  /* Ident suitable for res reg */
	  if (fxregble)
	  {
	    props(stare) |= inreg_bits;
	    no(stare) = R_RESULT;
	    bdy.propsneeds |= uses_R_RESULT_bit;
	  }
	  else
	  {
	    props(stare) |= infreg_bits;
	    no(stare) = FR_RESULT;
	    bdy.propsneeds |= uses_FR_RESULT_bit;
	  }
	}

#if 1
	else if (!isvar(*e) && !isparam(*e) &&
		((name(t) == reff_tag && name(son(t)) == cont_tag &&
		   name(son(son(t))) == name_tag && isvar(son(son(son(t))))
		   && !isvis(son(son(son(t)))) && !isglob(son(son(son(t))))
		   && unchanged(son(son(son(t))), stare)
	  /*
	   * reff cont variable-not assigned to in scope
	   */
		  ) ||
		 (name(t) == cont_tag && name(son(t)) == name_tag &&
		   isvar(son(son(t))) && !isvis(son(son(t))) && !isglob(son(son(t)))
		   && unchanged(son(son(t)), stare)
	  /*
	   * cont variable - not assigned to in scope
	   */
		  )
		 )
	 )
	{
	  props(stare) |= defer_bit;
	  /* dont take space for this dec */
	}
#endif
#if 0	/* dont undo large const in loop optimisation */
	else if (!isvar(stare) &&
		(isusereg(stare) == 0)
		 && (name(t) == name_tag || name(t) == val_tag))
	{
	  props(stare) |= defer_bit;
	  /* dont take space for this dec */
	}
#endif
#if 1
	else if (!isvar(stare)
		 && name(t) == name_tag
		 && !isvar(son(t))
		 && !isvis(son(t))
		 && (props(son(t)) & inanyreg)
		)
	{
	  /*
	   * dont take space for this constant dec,
	   * initialiser is another simple constant ident
	   * (eg from double nested loop optimisation)
	   */
	  props(stare) |= defer_bit;
	}
#endif
	/* All the parameters have been scanned at this point so
	   maxfix gives the total no of free t-regs */
	else if (fxregble &&
		 bdy.fixneeds < maxfix &&
		(bdy.propsneeds & morefix) == 0 &&
		((bdy.propsneeds & anyproccall) ==0
#if 0
		  ||
		  tempdec(stare,((bdy.propsneeds & morefix) ==0) && bdy.fixneeds<2)
#endif
		 )
		)
	{
	  if ((props(stare) & notparreg) ==0)
	  {
	    no(stare) = R_NO_REG;
	    props(stare) |= inreg_bits;
	    if (uses_R_RESULT==1)
	    {
	      props(stare) |= notresreg;
	    }
	    bdy.fixneeds += 1;
	  }
	}
	else if (flregble &&
		 bdy.floatneeds < maxfloat &&
		(bdy.propsneeds & morefloat) == 0 &&
		((bdy.propsneeds & anyproccall) ==0
#if 0
		  ||
		  tempdec(stare,((bdy.propsneeds & morefloat) == 0 &&
				     bdy.floatneeds < 1))
#endif
		 )
		)
	{
	  if ((props(stare) & notparreg) == 0)
	  {
	    /* Ident suitable for float t-reg */
	    no(stare) = FR_NO_REG;
	    props(stare) |= infreg_bits;
	    if (uses_FR_RESULT==1)
	    {
	      props(stare) |= notresreg;
	    }
	    bdy.floatneeds += 1;
	  }
	}
	else
	{
	  /*
	   * allocate either on stack or saved reg, decided later by
	   * regalloc()
	   */
	  no(stare) = R_NO_REG;
	}
      }
      bdy = maxneeds(bdy, defneeds);
      if ((bdy.propsneeds & usesproccall)!= 0)
      {
	bdy.propsneeds |= hasproccall;
      }
      return bdy;
    }

/*********************************************************************
	sequence

shape of exp is shape of end of sequence
son is sequence holder, son of this is list of voided statements.
*********************************************************************/

  case seq_tag:
    {
      exp *arg = &bro(son(*e));
      needs an;
      exp *stat;
      exp * atsc = &son(son(*e));
      for (;;)
      {
	exp sc = *atsc;
	if (name(sc) == cond_tag && name(sh(son(sc))) ==bothd
	    && name(bro(son(bro(son(sc))))) == top_tag)
	{
	  /* sc is cond(... goto | make_top); can replace
	     make_top by next exp in sequence */
	  exp lbst = bro(son(sc));
	  exp mkt = bro(son(lbst));
	  exp ne = (last(sc))? bro(son(*e)): bro(sc);
	  exp bne = bro(ne);
	  bool lne = last(ne);
	  if (name(ne)!= cond_tag)
	  {
	    /* only worthwhile eliding if ne is a cond */
	    if (last(sc))break;
	    atsc = &bro(sc);
	    continue;
	  }
	  sh(sc) = sh(ne);
	  bro(ne) = lbst; setlast(ne);
	  bro(son(lbst)) = ne;
	  /* sc is now cond( ... goto | next cond exp) */
	  if (!last(sc))
	  { /* not last in seq - swallow next*/
	    bro(sc) = bne;
	    if (lne) { setlast(sc); } else { clearlast(sc);}
	    no(son(*e))--; /* one less statement */
	  }
	  else if (no(son(*e))!= 1)
	  { /* last but not only - replace by
	       make_top and put cond in res posn */
	    bro(mkt) = bro(sc); setlast(mkt);
	    *atsc = mkt;
	    bro(sc) = bne;
	    if (lne) { setlast(sc); } else { clearlast(sc);}
	    *arg = sc;
	    sc = mkt;
	  }
	  else
	  { /* whole sequence can be replace by cond */
	    bro(sc) = bro(*e);
	    if (last(*e)) { setlast(sc); } else {clearlast(sc); }
	    *e = sc;
	    return scan(e, at);
	  }

	}
	if (last(sc))break;
	atsc = &bro(sc);
      }
      an = scan(arg, &arg);
      stat = &son(son(*e));

      arg = stat;
      for (;;)
      {
	needs stneeds;

	stneeds = scan(stat, &arg);
	/* initial statements voided */
	an = maxneeds(an, stneeds);
	if (last(*(stat)))
	{
	  if ((an.propsneeds & usesproccall)!= 0)
	  {
	    an.propsneeds |= hasproccall;
	  }
	  return an;
	}
	stat = &bro(*stat);
	arg = stat;
      }

    };

/********************************************************************
	goto

shape is bottom
son is exp for value jumped with
ptr is labelled exp
*********************************************************************/

   case goto_tag:
    /* By popular request the  infamous trap_tag */
   case trap_tag:
    {
      return zeroneeds;
    };
   case ass_tag:
   case assvol_tag:
    {
      exp *lhs = &son(*e);
      exp *rhs = &bro(*lhs);
      needs nr;
      ash a;

      nr = scan(rhs, at);
      /* scan source */

      a = ashof(sh(*(rhs)));
      if (!valregable(sh(*rhs)))
      {
	/* complicated memory move */
	if (!(a.ashsize <= 32 && a.ashsize == a.ashalign))
	  nr.fixneeds += 2;	/* memory block copy */
      }

      if (name(*(lhs)) == name_tag &&
	 ((isvar(son(*(lhs))) && !isglob(son(*(lhs)))) ||
	  ((nr.propsneeds & (hasproccall | morefix)) == 0
	    && nr.fixneeds+1 < maxfix
	   )
	  )
	)
      {
	/* simple destination */
	if (isvar(son(*(lhs))) && isglob(son(*(lhs))))
	  nr.fixneeds += 1;		/* for TOC access */
	return nr;
      }
      else
      {
	needs nl;
	prop prps = (nr.propsneeds & hasproccall) << 1;

	nl = scan(lhs, at);
	/* scan destination */
	if (APPLYLIKE(*(rhs)) &&
	    nstare == ass_tag &&
	   (nl.propsneeds & (anyproccall|uses_R_RESULT_bit|uses_FR_RESULT_bit)) ==0
	   )
	{
	  /*
	   * source is proc call, so assign result reg directly
	   */
	 ;
	}
	else if (nr.fixneeds >= maxfix || prps != 0)
	{
	  /*
	   * source and destination regs overlap, so identify source
	   */
	  cca(at, rhs);
	  nl = shapeneeds(sh(*(rhs)));
	  nl.propsneeds |= morefix;
	  nl.propsneeds &= ~(prps >> 1);
	  nl.propsneeds |= prps;
	}
	nr.fixneeds += 1;
	return maxneeds(nl, nr);
      }
    };
   case untidy_return_tag:
   case res_tag:
    {
      needs x;
      shape s;
      exp *arg = &son(*e);
      s = sh(*(arg));
      props(*e) = 0;		/* clear possibility of tlrecirsion; may be
				 * set later */
      no_of_returns++;
      x = scan(arg, at);	/* scan result exp ... */

      if (shape_size(s)!= 0)
      {
	/* ...not a void result */
	x.propsneeds |= has_result_bit;

	if (is_floating(name(s)))
	{
	  /* ... floating pt result */
	  x.propsneeds |= realresult_bit;
	  if (name(s)!= shrealhd)
	  {
	    x.propsneeds |= longrealresult_bit;
	  }
	}
	else
	{
	  if (!valregable(s))
	  {
	    ASSERT(redo_structfns==0);
	    x.propsneeds |= long_result_bit;
	  }
	}
      }

#if 0				/* +++ if we can avoid leaf proc conflict */
      if ((x.propsneeds & (long_result_bit | anyproccall | uses_res_reg_bit)) == 0)
      {
	r = son(*(e));
	if (name(r) == ident_tag && isvar(r) &&
	    name(ss = bro(son(r))) == seq_tag &&
	    name(t = bro(son(ss))) == cont_tag &&
	    name(son(t)) == name_tag && son(son(t)) == r)
	{

	  /*
	   * result is tag allocated into result reg - see ident_tag:
	   */
	  if ((props(r) & inreg_bits)!= 0)
	  {
	    x.fixneeds--;
	  }
	  else if ((props(r) & infreg_bits)!= 0)
	  {
	    x.floatneeds--;
	  }
	  else
	  {
	    props(r) |= (is_floating(name(s)))? infreg_bits : inreg_bits;
	  }
	  x.propsneeds |= uses_res_reg_bit;
	  no(r) = R_USE_RES_REG;/* identification  uses result reg in body */
	}
      }
#endif
      return x;
    };

  case apply_tag:
    {
      exp application = *(e);
      exp fn = son(application);
      exp *par = &bro(fn);
      exp *fnexp = &son(*e);
      int parsize = 0;
      needs nds;
      bool regresult = reg_result(sh(application));
      int i;



      nds = scan(fnexp, at);
      /* scan the function exp ... */
      if ((nds.propsneeds & hasproccall)!= 0)
      {
	/* .... it must be identified */
	cca(at, fnexp);
	nds.propsneeds &= ~hasproccall;
	nds.propsneeds |= usesproccall;
	fn = son(application);
	par = &bro(fn);
      }


      for (i = 1; !last(fn); ++i)
      {				/* scan parameters in turn ... */
	needs onepar;
	shape shpar = sh(*par);
	int par_regs_used;
	int move_to_stack_regs;

	par_regs_used = (parsize >> 5);

	if (par_regs_used >= (end_param-R_FIRST_PARAM+1))
	{
	  par_regs_used = (end_param-R_FIRST_PARAM+1);
	  move_to_stack_regs = 1;	/* can no longer move to param reg, need spare */
	}
	else
	{
	  move_to_stack_regs = 0;
	}

	onepar = scan(par, at);

	FULLCOMMENT4("scan: apply_tag: i=%d parn=%d pars=%d mover=%d",
		i, onepar.fixneeds, par_regs_used, move_to_stack_regs);

	if (((i != 1 || regresult) && (onepar.propsneeds & hasproccall)!= 0))
	{
	  /* if it isn't the first parameter, and it calls a proc, identify it */
	  FULLCOMMENT("scan apply_tag: cca bring forward apply");
	  cca(at, par);
	  nds.propsneeds |= usesproccall;
	  nds = maxneeds(shapeneeds(sh(*(par))), nds);
	  nds.maxargs = max(nds.maxargs, onepar.maxargs);
	}
	else if ((i != 1 && (onepar.propsneeds & hasproccall)!= 0) ||
		 onepar.fixneeds >= 6 /* +++ remove safety net */ ||
		 onepar.fixneeds + move_to_stack_regs + par_regs_used > maxfix)
	{
	  /* requires to many regs to evaluate, identify it */
	  FULLCOMMENT("scan apply_tag: cca, param has too large fixneeds");
	  cca(at, par);
	  nds.propsneeds |= morefix;
	  nds = maxneeds(shapeneeds(sh(*(par))), nds);
	  nds.maxargs = max(nds.maxargs, onepar.maxargs);
	}
	else
	{
	  nds = maxneeds(onepar, nds);
	}
	parsize = ALIGNNEXT(parsize, shape_align(shpar));
	parsize = ALIGNNEXT(parsize + shape_size(shpar), 32);

	if (last(*(par)))
	{
	  break;
	};
	par = &bro(*par);
      }

      if (!regresult && name(father(application))!= ass_tag)
      {
	/* find space for non reg result */
	FULLCOMMENT("scan apply_tag: cca space for non reg result");
	cca(at, ptr_position(application));
	nds.propsneeds |= usesproccall;
      }
      else
      {
	nds.propsneeds |= hasproccall;
      }
      nds.maxargs = max(nds.maxargs, parsize);
      return nds;
    };

  case null_tag:
    {
      /* many int optimisations keyed off val_tag so represent null as val_tag */
      name(*e) = val_tag;
      no(*e) = 0;		/* null represented as 0 */
      return shapeneeds(sh(*e));
    }

  case val_tag:
    {
      exp s = sh(*e);

      if (name(s) == offsethd && al2(s) >= 8)
      {
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
    {
      return shapeneeds(sh(*(e)));
    };

    case clear_tag:
    case top_tag:
    case prof_tag:
    case local_free_all_tag:
    {
      return zeroneeds;
    };

   case neg_tag:
   case case_tag:
   case not_tag:
   case offset_negate_tag:
   case diagnose_tag:
   case goto_lv_tag:
   case alloca_tag:
    {
      return scan(&son(*e), at);
    }
   case abs_tag:
    if (architecture!=RS6000_CODE)
    {
      /* We cant use abs instruction so we convert */
      shape int_shpe = sh(*e);
      exp arg = son(*e);
      exp id = me_startid(int_shpe,arg,0);
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

      CLEAR__TAG = getexp(f_top, nilexp, 0, nilexp, nilexp,0, 0, clear_tag);
      LABST__TAG = me_b3(int_shpe,CLEAR__TAG,me_obtain(id),labst_tag);

      VAL__TAG = me_shint(int_shpe,0);
      TEST__TAG = me_q1(no_nat_option,f_less_than,&LABST__TAG,
			me_obtain(id),VAL__TAG,test_tag);
      NEG__TAG = me_u3(int_shpe,me_obtain(id),neg_tag);
      pt(NEG__TAG) = pt(ABS__TAG);
      props(NEG__TAG) =props(ABS__TAG);

      ZERO__TAG = me_u3(f_top,TEST__TAG,0);
      SEQ__TAG = me_b3(int_shpe,ZERO__TAG,NEG__TAG,seq_tag);
      COND__TAG = me_b3(int_shpe,SEQ__TAG,LABST__TAG,cond_tag);
      id = me_complete_id(id,COND__TAG);
      if (last(*e))
      {
	setlast(id);
      }
      else
      {
	clearlast(id);
      }
      bro(id) = bro(*e);
      *e = id;
      return scan(e,at);
    }



   case fneg_tag:
   case fabs_tag:
   case chfl_tag:
    {
      needs nds;

      nds = scan(&son(*e), at);
      return nds;
    }
   case bitf_to_int_tag:
   case int_to_bitf_tag:
    {
      exp *arg = &son(*e);

      return scan(arg, at);
    }

  case round_tag:
    {
      needs s;
      exp *arg = &son(*e);
      shape sres = sh(*e);
      if (shape_size(sres)!=32)
      {
	exp ch = getexp(sres,bro(*e),last(*e),*e,pt(*e),props(*e),0,chvar_tag);
	bro(*e) =ch;setlast(*e);
	sh(*e) =slongsh;
	*e=ch;
	return scan(e,at);
      }
      s = scan(arg,at);
      s.fixneeds = max(s.fixneeds,1);
      s.floatneeds = max(s.floatneeds,2);
      return s;
    };
   case shl_tag:
   case shr_tag:
   case long_jump_tag:
    {
      exp *lhs = &son(*e);
      exp *rhs = &bro(*lhs);
      needs nr;
      needs nl;
      prop prps;

      nr = scan(rhs, at);
      nl = scan(lhs, at);
      rhs = &bro(*lhs);
      prps = (nr.propsneeds & hasproccall) << 1;
      if (nr.fixneeds >= maxfix || prps != 0)
      {

	/*
	 * if reg requirements overlap, identify second operand
	 */
	cca(at, rhs);
	nl = shapeneeds(sh(*(rhs)));
	nl.propsneeds |= morefix;
	nl.propsneeds &= ~(prps >> 1);
	nl.propsneeds |= prps;
      }
      nr.fixneeds += 1;
      return maxneeds(nl, nr);

    };


  case test_tag:
    {
      exp stare = *(e);
      exp l = son(stare);
      exp r = bro(l);

      if (!last(stare) && name(bro(stare)) == test_tag &&
	  no(stare) == no(bro(stare)) &&
	  props(stare) == props(bro(stare)) &&
	  eq_exp(l, son(bro(stare))) && eq_exp(r, bro(son(bro(stare))))
	)
      {				/* same test following in seq list - remove
				 * second test */
	if (last(bro(stare)))
	  setlast(stare);
	bro(stare) = bro(bro(stare));
      }

      if (last(stare) && name(bro(stare)) == 0	/* seq holder */
	  && name(bro(bro(stare))) == test_tag &&
	  name(bro(bro(bro(stare)))) == seq_tag &&
	  no(stare) == no(bro(bro(stare))) &&
	  props(stare) == props(bro(bro(stare))) &&
	  eq_exp(l, son(bro(bro(stare))))
	  && eq_exp(r, bro(son(bro(bro(stare)))))
	)
      {				/* same test following in seq res - void
				 * second test */
	setname(bro(bro(stare)), top_tag);
	son(bro(bro(stare))) = nilexp;
	pt(bro(bro(stare))) = nilexp;
      }

      /*
       * commute tests if
       * a) lhs a const (to support convention of const on rhs); or
       * b) rhs is a load from mem, move to lhs in hope of reducing
       *    load-use delays
       */
      if (
	  (
	    test_number(stare) ==TEST_NE||test_number(stare) ==TEST_EQ||
	    !is_floating(name(sh(l)))
	   )
	   &&
	  (
	    (name(l) == val_tag)
	     ||
	    (LOADFROMSTORE(r) && !LOADFROMSTORE(l))
	  )
	)
      {
	/* commute */
	bro(l) = stare;
	setlast(l);
	bro(r) = l;
	clearlast(r);
	son(stare) = r;
	r = l;
	l = son(stare);
	settest_number(stare,cbranch(test_number(stare)));
      }

      if (is_floating(name(sh(l))))
      {
	return fpop(e, at);
      }
      else if (name(r) == val_tag && no(r) == 1
	       && (test_number(stare) == TEST_GE ||
		   test_number(stare) == TEST_LT)
	      )
      {
	/* The only reason for this optimisation is that it increases
	   the chance of using the Record bit */
	no(r) = 0;
	if (test_number(stare) == TEST_GE)
	{
	  /* Branch >=1 is same as >0 */
	  settest_number(stare,TEST_GT);
	}
	else
	{
	  /* Branch <1 is same as <= 0 */
	  settest_number(stare,TEST_LE);
	}
      }
      return non_commutative_scan(e, at);
    }
   case movecont_tag:
    {
     exp * d  = &son(*e);
     exp * s  = &bro(*d);
     exp * sz = &bro(*s);
     needs nd;
     needs ns;
     needs nsz;
     prop prps;
     nd  = scan(d, at);
     ns  = scan(s, at);
     nsz = scan(sz, at);
     prps = (ns.propsneeds & hasproccall) << 1;
     if (ns.fixneeds >= maxfix || prps != 0) {
       /* if reg requirements overlap, identify
	  second operand */
       cca(at, s);
       ns = shapeneeds(sh(*(s)));
       ns.propsneeds |= morefix;
       ns.propsneeds &= ~(prps >> 1);
       ns.propsneeds |= prps;
     }
     nd.fixneeds += 1;
     nd = maxneeds(nd, ns);
     prps= (nsz.propsneeds & hasproccall) << 1;
     if (nd.fixneeds +nsz.fixneeds >= maxfix || prps != 0) {
       /* if reg requirements overlap, identify
	  last operand */
       cca(at, sz);
       nsz = shapeneeds(sh(*(sz)));
       nsz.propsneeds |= morefix;
       nsz.propsneeds &= ~(prps >> 1);
       nsz.propsneeds |= prps;
     }
     nd.fixneeds+=1;
     nd = maxneeds(nd,nsz);
     if (nd.fixneeds < 4)nd.fixneeds = 3;
     return nd;
   }



   case plus_tag:
    {				/* replace any operands which are neg(..) by -
				 * if poss */
      exp sum = *(e);
      exp list = son(sum);
      bool someneg = 0;
      bool allneg = 1;

      /* check BUGP13 [corruption by extract_consts()] is fixed */
      /* check father set correctly */
      ASSERT(father(son(sum)) ==sum);

      for (; optop(sum);)
      {
	if (name(list) == neg_tag)
	  someneg = 1;
	else
	  allneg = 0;
	if (last(list))
	  break;
	list = bro(list);
      }

      if (someneg)
      {				/* there are some neg() operands */
	if (allneg)
	{
	  /* transform -..-... to -(..+.. +...) */
	  exp x;

	  /*
	   * Build a new list form operand of neg_tags, which will
	   * become plus_tag operands.
	   */
	  x = son(sum);
	  list = son(x);
	  for (;;)
	  {
	    /*
	     * 'x' moves along neg_tag's lists
	     * 'list' moves along sons of neg_tag's lists, building a new list
	     * eventually new list is made son of plus_tag
	     */

	    ASSERT(list == son(x));

	    bro(list) = son(bro(x));
	    if (!last(x))
	    {
	      clearlast(list);
	      list = bro(list);
	      x = bro(x);
	    }
	    else
	    {

	      setlast(list);
	      bro(list) = sum;		/* set father to be */
	      son(sum) = son(son(sum)); /* set new sons of plus_tag */
	      break;
	    }
	  }

	  /*
	   * create new neg_tag to replace plus_tag,
	   * old plus_tag being the operand of the new neg_tag.
	   */
	  x = getexp(sh(sum), bro(sum), last(sum), sum, nilexp,
		     0, 0, neg_tag);

	  setlast(sum);

	  /* set father of sum, new neg_tag exp */
	  bro(sum) = x;

	  /* check father of sum is correct */
	  ASSERT(father(son(sum)) ==sum);

	  *(e) = x;
	}			/* end allneg */
	else
	{
	  /* transform to  ((..(..+..) - ..) -..) */
	  int n = 0;
	  exp brosum = bro(sum);
	  bool lastsum = last(sum);
	  exp x = son(sum);
	  exp newsum = sum;

	  list = nilexp;
	  for (;;)
	  {
	    exp nxt = bro(x);
	    bool final = last(x);

	    if (name(x) == neg_tag)
	    {
	      bro(son(x)) = list;
	      list = son(x);
	    }
	    else
	    {
	      bro(x) = newsum;
	      newsum = x;
	      if ((n++) == 0)
		setlast(newsum);
	      else
		clearlast(newsum);
	    }
	    if (final)
	      break;
	    x = nxt;
	  }

	  if (n > 1)
	  {
	    son(sum) = newsum;
	    newsum = sum;	/* use existing exp for add operations */
	  }
	  for (;;)
	  {			/* introduce - operations */
	    exp nxt = bro(list);

	    bro(newsum) = list;
	    clearlast(newsum);
	    x = getexp(sh(sum), nilexp, 0, newsum, nilexp, 0, 0, minus_tag);

	    bro(list) = x;
	    setlast(list);
	    newsum = x;
	    if ((list = nxt) == nilexp)
	      break;
	  }
	  bro(newsum) = brosum;
	  if (lastsum)
	  {
	    setlast(newsum);
	  }
	  else
	  {
	    clearlast(newsum);
	  }
	  *(e) = newsum;
	}			/* end else allneg */

	/* check father set correctly */
	ASSERT(father(son(*e)) ==*e);

	return scan(e, at);

      }				/* end someneg - else continue to next case */
    }
   case addptr_tag:
    {
      exp p = son(*e);
      exp d = bro(p);
      int fal = frame_al_of_ptr(sh(p));
      if (fal!=0 && i_reckon_its_a_general_proc(fal))
      {
	int oal = frame_al1_of_offset(sh(d));
/*	if( ((oal-1)&oal) != 0)
	{
	  fail("can't cope with mixed frame offsets yet");
	}*/
	if (!l_or_cees(oal))
	{
	  /* callers are referenced through R_TP */
	  /* to get this we use locptr to access through R_FP(current_env)*/
	  exp ne = getexp(sh(p), d, 0, p, nilexp, 0, 0,locptr_tag);
	  bro(p) = ne; setlast(p);
	  son(*e) = ne;
	}
      }
      /* ... and continue */
    }


   case local_free_tag:
   case mult_tag:
   case and_tag:
   case or_tag:
   case xor_tag:
    {
      return commutative_scan(e, at);
    };
   case reff_tag:
   case offset_pad_tag:
   case chvar_tag:
   case locptr_tag:
    {
      exp *arg = &son(*e);

      return maxneeds(scan(arg, at),
		      shapeneeds(sh(*(e))));
    };

  case float_tag:
    {
      needs nds;
      exp *arg = &son(*e);

      nds = maxneeds(scan(arg, at), shapeneeds(sh(*(e))));
      if (name(sh(son(*(e)))) == ulonghd)
      {
	if (nds.floatneeds < 2)
	  nds.floatneeds = 2;
      }
      return nds;
    }
   case cont_tag:
   case contvol_tag:
    {
      exp *arg = &son(*e);
      needs nds;

      nds = maxneeds(scan(arg, at), shapeneeds(sh(*(e))));
      nds.fixneeds = max(nds.fixneeds, 2);
      return nds;
    };
   case offset_mult_tag: case offset_div_tag: {
     exp op1 = son(*e);
     exp op2 = bro(op1);
     shape s = sh(op2);
     if (name(op2) ==val_tag  && name(s) ==offsethd
	 && al2(s) >= 8) {
       int n = no(op2) /8;
       if (n == 1) {
	 /* offset is one  byte */
	 bro(op1) = bro(*e);
	 if (last(*e)) { setlast(op1); } else {clearlast(op1); }
	 *e = op1;
	 return scan(e, at);
       }
       else
	 if (name(*e) == offset_mult_tag && n > 1 && (n& (n-1)) == 0)
	   if (name(op1) == and_tag
	      && name(son(op1)) == shr_tag &&
	      name(bro(son(op1))) ==val_tag) {
	     exp shexp = son(op1);
	     exp ac = bro(shexp);
	     exp shop1 = son(shexp);
	     exp shop2 = bro(shop1);
	     int na = no(ac);
	     if ((na& (na+1)) ==0 && name(shop2) ==val_tag) {
	       int pn = 0;
	       int ns = no(shop2);
	       int i = n;
	       while (i>1) { i >>= 1; pn++; }

	       if (ns > pn)
	       {
		 /* can do transform:
		    (((shop1>>ns) & na) * n) =>
		    shop1>>(ns-pn) & (na*n)
		      */
		 no(shop2) = ns-pn;
		 no(ac) = na*n;
		 bro(op1) = bro(*e);
		 if (last(*e))
		 {
		   setlast(op1);
		 }
		 else
		 {
		   clearlast(op1);
		 }
		 *e = op1;
		 return scan(e, at);
	       }
	     }
	   }
	   else
	   {
	     /* will do this by literal shift */
	     no(op2) = n;
	     return scan(&son(*e), at);
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
    {
      return non_commutative_scan(e, at);
    };
   case offset_add_tag:
    {
      exp l = son(*e);
      exp r = bro(l);
      if (name(l) == val_tag)
      {
	sh(l) = sh(r);   /* both offsets will be treated the same */
	son(*e) = r; clearlast(r);
	bro(r) = l; setlast(l); bro(l) = *e;
	/* ... and put val last */
      }
      else
      {
	if (al2(sh(l)) >=8 && al2(sh(r)) <8)
	{
	  return non_commutative_scan(e, at);
	}
      }

      setname(*e, plus_tag);

      return commutative_scan(e,at);
    }
   case offset_subtract_tag: {
	exp l = son(*e);
	exp r = bro(l);
	if (name(r) ==val_tag) {
		sh(r) = sh(l);   /* both offsets will be treated the same */
	}
	else
	if (al2(sh(r)) >=8 && al2(sh(l)) <8) {
	        return non_commutative_scan(e, at);
	}

	setname(*e, minus_tag);
	return non_commutative_scan(e, at);
    }

  case fdiv_tag:
  case fplus_tag:
  case fminus_tag:
  case fmult_tag:
    {
      exp op = *(e);
      exp a2 = bro(son(op));

      if (!last(a2))
      {				/* + and * can have >2 parameters - make them
				 * diadic - can do better a+exp => let x = exp
				 * in a+x */
	exp opn = getexp(sh(op), op, 0, a2, nilexp, 0, 0, name(op));

	/* dont need to transfer error treatment - nans */
	exp nd = getexp(sh(op), bro(op), last(op), opn, nilexp, 0, 1,
			ident_tag);
	exp id = getexp(sh(op), op, 1, nd, nilexp, 0, 0, name_tag);

	pt(nd) = id;
	bro(son(op)) = id;
	setlast(op);
	bro(op) = nd;
	while (!last(a2))
	  a2 = bro(a2);
	bro(a2) = opn;
	*(e) = nd;
	return scan(e, at);
      }
      return fpop(e, at);
    };
   case field_tag:
    {
      needs str;
      exp *arg = &son(*e);

      if (chase(*e, arg))
      {				/* field has been distributed */
	exp stare = *e;
	exp ss = son(stare);

	if (!last(stare))
	  clearlast(ss);
	bro(ss) = bro(stare);
	sh(ss) = sh(stare);
	*e = ss;
	return scan(e, at);
      }
      str = scan(arg, at);
      return maxneeds(str, shapeneeds(sh(*(e))));
    };

/*********************************************************************
	load_proc


number is number of proc (useful for indexing)
*********************************************************************/
   case proc_tag:
   case general_proc_tag:
    {
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

      if (name(*e) ==general_proc_tag)
      {
	end_param = GENERAL_PROC_PARAM_REGS + R_FIRST_PARAM - 1;
      }
      else
      {
	end_param = PROC_PARAM_REGS + R_FIRST_PARAM - 1;
      }

      nonevis = 1;
      gen_call = 0;
      tail_call = 0;
      bexp = &son(*e);
      bat = bexp;
      body = scan(bexp, &bat);
#ifdef DO_DYNAMIC_INITIALISATION
      if (proc_is_main(*e))
      {
        /* we need a call to __main */
        body.propsneeds |= usesproccall;
      }
#endif
      return body;
    }
/********************************************************************
 |  TDF SPECIFICATION 4.0 ADDITIONS       |
 ******************************************
 The new tags introduced for the move from spec 3.0 to spec 4.0 are
 general_proc
 apply_general
 make_callee_list
 make_dynamic_callee
 tail_call
 same_callees
 untidy_return
 set_stack_limit
 env_size
 general_env_offset
 caller_name
 formal_callee
 caller
********************************************************************/
   case apply_general_tag:
    {
      exp application = *(e);
      exp *fn = &son(application);
      exp cers = bro(*fn);
      exp *cerl = &son(cers);
      long stpar = 0;
      needs nds;
      needs plnds;
      int i;

      gen_call = 1;

      /* scan the function */
      nds = scan(fn, at);

      if ((nds.propsneeds & hasproccall)!= 0)
      {
	/* .... it must be identified */
	cca(at, fn);
	nds.propsneeds &= ~hasproccall;
	nds.propsneeds |= usesproccall;
	fn = &son(application);
      }
      /* scan the callers */
      for (i=0; i<no(cers); i++)
      {
	needs onepar;
	shape shonepar = sh(*cerl);
	exp * par = (name(*cerl) ==caller_tag)?&son(*cerl):cerl;
	int n = ALIGNNEXT(stpar, shape_align(shonepar));
	onepar = scan(par,at);
	if ((i != 0 && (onepar.propsneeds & hasproccall)!= 0) ||
	    onepar.fixneeds+ (stpar>>5) > maxfix)
	{
	  /* +++ if we go over a certain number of param regs
	     they are forced to be on the stack so stpar>>5
	     is not the best estimate ,but sufficient*/
	  /* stpar>>5 is the no of param regs used so far */
	  /* if it isn't the first parameter, and it
	     calls a proc, identify it */
	  /* it is ok for first param to have a proccall since we have
	     no loaded parameters to corrupt */
	  cca(at, par);
	  nds.propsneeds |= usesproccall;
	  nds = maxneeds(shapeneeds(sh(*(par))), nds);
	  nds.maxargs = max(nds.maxargs, onepar.maxargs);
	}
	else
	{
	  nds = maxneeds(onepar, nds);
	}
	if (name(*cerl) ==caller_tag)
	{
	  /* for caller_tag's we record where it will live */
	  no(*cerl) = n;
	  clear_coded_caller(*cerl);
	}
	n = n + shape_size(shonepar);
	stpar = ALIGNNEXT(n,32);
	cerl = &bro(*cerl);
      }
      nds.maxargs = max(nds.maxargs, stpar);
      /* scan the callees */
      nds = maxneeds(scan(&bro(bro(son(application))), at), nds);
      /* scan the postlude */
      plnds = scan(&bro(bro(bro(son(application)))), at);
      if (plnds.propsneeds & anyproccall)
      {
	props(application) =1;
	if (is_floating(name(sh(application))) || valregable(sh(application)))
	{
	  cca(at, ptr_position(application));
	  plnds.propsneeds |= usesproccall;
	}
      }
      else
      {
	props(application) =0;
	if (is_floating(name(sh(application))) || valregable(sh(application)))
	{
	  cca(at, ptr_position(application));
	}
      }
      nds = maxneeds(nds, plnds);
      nds.propsneeds |= hasproccall;
      return nds;
    }
/********************************************************************/
   case make_callee_list_tag:
    {
      exp cees = *e;
      exp *par = &son(cees);
      needs nds;
      long stpar = 0;
      int i;
      nds = zeroneeds;
      for (i=0;i<no(cees);i++)
      {
	/* scan each callee and identify if necessary */
	needs onepar;
	shape shonepar = sh(*par);
	int n = ALIGNNEXT(stpar,shape_align(shonepar));
	onepar = scan(par,at);
	if ((onepar.propsneeds & hasproccall)!=0 || onepar.fixneeds+1>maxfix)
	{
	  /* if it calls a proc identify it */
	  cca(at,par);
	  nds.propsneeds |=usesproccall;
	  nds = maxneeds(shapeneeds(sh(*par)),nds);
	  nds.maxargs = max(nds.maxargs,onepar.maxargs);
	}
	else
	{
	  nds = maxneeds(onepar,nds);
	}
	n += shape_size(shonepar);
	stpar = ALIGNNEXT(n,32);
	par = &bro(*par);
      }
      no(cees)=stpar; /* The total no of bits needed for callees */
      max_callees = max(max_callees, stpar);
      return nds;
    }

/********************************************************************/
   case make_dynamic_callee_tag:
    {
      exp cees = *e;
      exp *ptr = &son(cees);
      needs ndsp;
      needs nds;
      nds = zeroneeds;
      ndsp = scan(ptr, at);
      if (((ndsp.propsneeds & hasproccall)!= 0) ||
	  ndsp.fixneeds+1 > maxfix)
      {
	cca(at, ptr);
	nds.propsneeds |= usesproccall;
	nds = maxneeds(shapeneeds(sh(*(ptr))), nds);
	nds.maxargs =  max(nds.maxargs, ndsp.maxargs);
      }
      else
      {
	nds = ndsp;
      }
      ndsp = scan(&bro(son(*e)), at);
      if (((ndsp.propsneeds & hasproccall)!= 0) ||
	  ndsp.fixneeds+2 > maxfix)
      {
	cca(at, &bro(son(cees)));
	nds.propsneeds |= usesproccall;
	nds = maxneeds(shapeneeds(sh(bro(son(*e)))), nds);
	nds.maxargs = max(nds.maxargs, ndsp.maxargs);
      }
      else
      {
	nds = maxneeds(ndsp, nds);
      }
      if (nds.fixneeds<5)nds.fixneeds = 5;
      return nds;
    }
/********************************************************************/
   case tail_call_tag:
    {
      needs ndsp;
      needs nds;
      exp *fn = &son(*e);
      ndsp = scan(fn,at);
      tail_call = 1;
      if (((ndsp.propsneeds & hasproccall)!=0) || ndsp.fixneeds+1>maxfix)
      {
	cca(at,fn);
	nds.propsneeds |= usesproccall;
	nds = maxneeds(shapeneeds(sh(*fn)),nds);
	nds.maxargs = max(nds.maxargs,ndsp.maxargs);
      }
      else
      {
	nds = ndsp;
      }
      gen_call = 1;
      ndsp = scan(&bro(son(*e)),at);
      nds = maxneeds(nds,ndsp);
      return nds;
    }
/********************************************************************/
   case same_callees_tag:
    {
      needs nds;
      nds = zeroneeds;
      nds.fixneeds = 4;
      max_callees = max(max_callees, callee_size);
      return nds;
    }
/********************************************************************/
   case env_size_tag:
   case set_stack_limit_tag:
   case return_to_label_tag:
    {
      exp *arg = &son(*e);
      return scan(arg,at);
    }
/********************************************************************/
   case general_env_offset_tag:
   case caller_name_tag:
    {
      return shapeneeds(sh(*e));
    }
/********************************************************************/
   case formal_callee_tag:
    {
      return zeroneeds;
    }
/********************************************************************/
   case caller_tag:
    {
      fail("Shouldn't be scanning a caller_tag");
      return zeroneeds;
    }
/********************************************************************/
   default:
    {
      FULLCOMMENT1("scan: bad nstare=%d", nstare);
      fail("case not covered in needs scan");
      return zeroneeds;
    }
  }
}
int scan_cond(exp * e, exp outer_id)
{
  exp ste = *e;
  exp first = son(ste);
  exp labst = bro(first);
  exp second = bro(son(labst));

  ASSERT(name(ste) ==cond_tag);

  if (name(second) ==top_tag && name(sh(first)) ==bothd && no(son(labst)) ==1
      && name(first) ==seq_tag && name(bro(son(first))) == goto_tag) {
    /* cond is { ... test(L); ? ; goto X | L:make_top}
       if ? empty can replace by seq { ... not-test(X); make_top }
       */
    exp l = son(son(first));
    while (!last(l)) { l = bro(l); }
    while (name(l) ==seq_tag) { l = bro(son(l)); }
    if (name(l) ==test_tag && pt(l) ==labst) {
      settest_number(l, notbranch[test_number(l)]);
      pt(l) = pt(bro(son(first)));
      bro(son(first)) = second;
      bro(second) = first; setlast(second);
      bro(first) = bro(ste);
      if (last(ste)) { setlast(first);} else { clearlast(first); }
      *e = first;
      return 1;
    }
    else return 0;
  }


  if (name(first) == seq_tag && name(second) == cond_tag
      && no(son(labst)) == 1
      && name(son(son(first))) == test_tag
      && pt(son(son(first))) == labst
      && name(son(second)) == seq_tag
      && name(son(son(son(second)))) == test_tag) {
    /* cond is ( seq (test to L;....|
       L:cond(seq(test;...),...) ) ..... */
    exp test1 = son(son(first));
    exp test2 = son(son(son(second)));
    exp op11 = son(test1);
    exp op21 = bro(op11);
    exp op12 = son(test2);
    exp op22 = bro(op12);
    bool c1 = complex(op11);
    bool c2 = complex(op21);

    if (c1 && eq_exp(op11, op12)) {
      /* ....if first operands of tests are
	 same, identify them */
      exp newid = getexp(sh(ste), bro(ste), last(ste), op11, nilexp,
			  0, 2, ident_tag);
      exp tg1 = getexp(sh(op11), op21, 0, newid, nilexp, 0, 0, name_tag);
      exp tg2 = getexp(sh(op12), op22, 0, newid, nilexp, 0, 0, name_tag);

      pt(newid) = tg1;
      pt (tg1) = tg2;	/* uses of newid */
      bro (op11) = ste; clearlast (op11);/* body of newid */
      /* forget son test2 = son test1 */
      bro(ste) = newid;
      setlast (ste);	/* father body = newid */
      son(test1) = tg1;
      son (test2) = tg2;	/* relace 1st operands of test */
      if (!complex(op21)) {
	/* if the second operand of 1st test is simple, then identification
	   could go in a t-teg (!!!NB overloading of inlined flag!!!).... */
	setinlined(newid);
      }
      kill_exp(op12, op12);
      *(e) = newid;
      if (scan_cond(&bro(son(labst)), newid) == 2 && complex(op22)) {
	/* ... however a further use of identification means that
	   the second operand of the second test must also be simple */
	clearinlined(newid);
      }
      return 1;
    }
    else
      if (c2 && eq_exp(op21, op22)) {
	/* ....if second operands of tests are
	   same, identify them */

	exp newid = getexp(sh(ste), bro(ste), last(ste), op21,
			    nilexp, 0, 2, ident_tag);
	exp tg1 = getexp(sh(op21), test1, 1,
			  newid, nilexp, 0, 0, name_tag);
	exp tg2 = getexp(sh(op22), test2, 1, newid, nilexp,
			  0, 0, name_tag);

	pt(newid) = tg1;
	pt (tg1) = tg2;	/* uses of newid */
	bro(op21) = ste; clearlast(op21);
	/* body of newid */
	/* forget bro son test2 = bro son test1 */
	bro(ste) = newid;
	setlast (ste);	/* father body = newid */
	bro(op11) = tg1;
	bro(op12) = tg2;
	if (!complex(op11)) { setinlined(newid); }
	kill_exp(op22, op22);
	/* relace 2nd operands of test */
	*(e) = newid;
	if (scan_cond(&bro(son(labst)), newid) == 2 && complex(op12)) {
	  clearinlined(newid);
	}
	return 1;
      }
      else
	if (name(op12)!= name_tag
	    && name(op11) == name_tag
	    && son(op11) == outer_id
	    && eq_exp(son(outer_id), op12)
	    ) {		/* 1st param of test1 is already identified with
			   1st param of  test2 */
	  exp tg = getexp(sh(op12), op22, 0, outer_id,
			   pt(outer_id), 0, 0, name_tag);
	  pt(outer_id) = tg;
	  no(outer_id) += 1;
	  if (complex(op21)) { clearinlined(outer_id); }
	  /* update usage of ident */
	  son(test2) = tg;
	  kill_exp(op12, op12);
	  if (scan_cond(&bro(son(labst)), outer_id) == 2 && complex(op22)) {
	    clearinlined(outer_id);
	  }
	  return 2;
	}
  }
  return 0;
}
static void number_caller_parameter(exp param_id)
{
  exp init_exp = son(param_id);
  shape param_shape = sh(init_exp);
  long par_size = shape_size(param_shape);
  long par_stack_location = ALIGNNEXT(stparam,32);

  ASSERT(name(init_exp) ==clear_tag);

  if (is_floating(name(param_shape)))
  {
    if (floatparam <= FR_LAST_PARAM)
    {
      props(init_exp) = floatparam;
      floatparam++;
    }
    else
    {
      props(init_exp) = 0;/*passed by stack */
    }
  }
  else
  {
    if (fixparam <= end_param)
    {
      props(init_exp) = fixparam;
    }
    else
    {
      props(init_exp) = 0;/*passed by stack*/
    }
  }
  no(init_exp) = par_stack_location;
  stparam = ALIGNNEXT(par_stack_location + par_size, 32);
  fixparam = R_FIRST_PARAM + (stparam / 32);
  return;
}


static void number_callee_parameter(exp callee_id)
{
  exp def = son(callee_id);
  shape callee_shape = sh(def);
  long size_of_callee = shape_size(callee_shape);
  long alignment_of_callee = shape_align(callee_shape);
  long n = ALIGNNEXT(callee_size , alignment_of_callee);

  no(def) = n;
  callee_size = ALIGNNEXT(n + size_of_callee , 32);
  return;
}