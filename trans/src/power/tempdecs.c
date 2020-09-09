/*
 * Copyright 2002-2012, The TenDRA Project.
 * Copyright 1997, United Kingdom Secretary of State for Defence.
 * Copyright 1993, Open Software Foundation, Inc.
 *
 * See doc/copyright/ for the full copyright terms.
 */

#include <assert.h>

#include <shared/bool.h>
#include <shared/check.h>
#include <shared/error.h>

#include <local/cpu.h>
#include <local/ash.h>

#include <tdf/shape.h>
#include <tdf/tag.h>

#include <reader/exp.h>

#include <construct/installtypes.h>
#include <construct/dec.h>
#include <construct/ash.h>
#include <construct/exp.h>

#include <main/flags.h>

#include <refactor/const.h>

#include "procrec.h"
#include "bits.h"
#include "regable.h"
#include "tempdecs.h"
#include "reg.h"
#include "stack.h"

/* to go in a switch as in case CASE_APPLYLIKE: */
#define	CASE_APPLYLIKE	apply_tag: case round_tag:case apply_general_tag


static int nouses;
static bool useinpar;
static int param_uses(exp);
static int locate_param(exp);

bool APPLYLIKE(exp e)
{
  if (e->tag ==apply_tag)
    return 1;
  if (e->tag ==apply_general_tag)
    return 1;
  if (e->tag ==round_tag)
    if (sh(e)->tag == ulonghd || cpu != CPU_POWERPC)
      return 1;
  return 0;
}
/* RETURNS_R_RESULT returns 1 if the exp returns R_RESULT when evaluated */
bool RETURNS_R_RESULT(exp e)
{
  if (e->tag ==apply_tag && valregable(sh(e)))
  {
    return 1;
  }
  if (e->tag ==apply_general_tag && valregable(sh(e)))
  {
    return 1;
  }
  if (e->tag ==round_tag)
  {
    if (sh(e)->tag ==ulonghd || cpu != CPU_POWERPC)
      return 1;
  }
  return 0;
}
/* RETURNS_FR_RESULT returns 1 if the exp returns FR_RESULT when evaluated */
bool RETURNS_FR_RESULT(exp e)
{
  if (e->tag ==apply_tag && is_floating(sh(e)->tag))
  {
    return 1;
  }
  if (e->tag ==apply_general_tag && is_floating(sh(e)->tag))
  {
    return 1;
  }
  return 0;
}


static int
trace_uses(exp e, exp id)
{
  /*
   * reduces nouses for each non-assignment use of id encountered in e; sets
   * useinpar if use in actual parameter (or function) posn terminates with 0 on
   * applications or jumps terminates with 2 on assignment to id otherwise
   * delivers 1
   * 0 is returned if trace_uses runs into a dead end
   * 2 is returned if trace_uses runs into another assignment
   * 1 is returned if still searching ok so as soon as 0 or 2 is returned
   * the recursion ends quickly
   */

  if (APPLYLIKE(e))
  {
    /* u is nouses before we start to scan the parameters */
    int u = nouses;
    int p = 1;
    exp l = child(e);

    while (p == 1)
    {
      p = trace_uses(l, id);
      if (u != nouses || p == 2)
      {
	/* We found a use of the ident or we found an assignment to it */
	useinpar = 1;
      }

      if (p == 0)
	nouses = u;
      if (l->last)
	break;
      l = next(l);
    }
    return 0;
  }

  switch (e->tag)
  {
   case caller_name_tag:
   case env_offset_tag:
   case general_env_offset_tag:
    /* Don't want to look at children of these tags */
    return 1;
   case name_tag:
    {
      nouses -= (child(e) == id);
      return 1;
    }

   case ident_tag:
    {
      exp f = child(e);
      exp s = next(f);
      int a;

      if ((e->props & defer_bit)!= 0)
      {
	exp t = f;

	f = s;
	s = t;
      }
      a = trace_uses(f, id);
      if (a != 1)
	return a;
      return trace_uses(s, id);
    }
   case case_tag:
    {
      trace_uses(child(e), id);
      return 0;
    }

  case labst_tag:
    return 0;

   case cond_tag:
    {
      int el;

      /* Cond tags are not treated like the default since we know
	 that the first argument will be coded first */
      el = trace_uses(child(e),id);
      if (el != 1)
      {
	return el;
      }
      return 0;
    }
   case seq_tag:
    {
      exp s = child(child(e));

      for (;;)
      {
	int el = trace_uses(s, id);

	if (el != 1)
	  return el;
	if (s->last)
	  return trace_uses(next(child(e)), id);
	s = next(s);
      }
    }

   case ass_tag:
    {
      if (isvar(id) && child(e)->tag == name_tag && child(child(e)) == id)
      {
	trace_uses(next(child(e)), id);
	return 2;
      }
      else if (APPLYLIKE(next(child(e))))
      {
	return trace_uses(next(child(e)), id);
      }
      /* else cont to next case */
    }
   default:
    {
      exp s = child(e);
      int nu = nouses;
      int bad_arguments = 0;
      /* A bad argument is one which contains an assignment or something to stop flow */
      int good_arguments = 0;
      /* A good_argument is one which contains one or more uses of id, but doesn't have
	 any assignments or things to stop flow */
      int ret_value = 0;

      if (s==NULL)
      {
	/*no arguments */
	return 1;
      }
      for (;;)
      {
	int monitor_uses;
	int el;
	monitor_uses = nouses;
	el = trace_uses(s, id);
	if (el==1  && nouses < monitor_uses)
	{
	  /* argument with uses of ident*/
	  good_arguments ++;
	}
	if (el != 1)
	{
	  /* An argument corrupts the flow */
	  bad_arguments++;
	  ret_value = el;
	}
	if (s->last)
	  break;
	s = next(s);
      }
      if (bad_arguments==0)
      {
	return 1;
	/* No problems */
      }

      if (bad_arguments==1 && good_arguments==0)
      {
	/* one bad one */
	/* all the rest don't use it */
	return ret_value;
      }
      nouses = nu;
      return ret_value;
    }
  }
}


static void
after_a(exp a, exp id)
{
  /* apply trace_uses to dynamic successors of a */
  exp dad;
  exp l;

tailrecurse:
  dad = father(a);
  if (nouses == 0)
    return;
  if (dad->tag == cond_tag || dad->tag == rep_tag
      || dad->tag == solve_tag || dad->tag == labst_tag
      || dad->tag == case_tag || dad->tag == goto_tag
      || dad->tag == test_tag || APPLYLIKE(dad))
  {
    /* dont try too hard ! */
    while (APPLYLIKE(dad) && dad != id)
      dad = father(dad);
    if (APPLYLIKE(dad))
    {
      useinpar = 1;
    }
    return;
  }


  for (l = a; !l->last; l = next(l))
  {
    int u = trace_uses(next(l), id);

    if (u != 1 || nouses == 0)
      return;
  }
  a = dad;
  if (dad != id)
    goto tailrecurse;
}

static bool
simple_seq(exp e, exp id)
{
	UNUSED(e);
	UNUSED(id);

#if 0
  exp dad = father(e);

  for (;;)
  {
    if (dad == id)
      return 1;
    if (dad->tag == seq_tag || dad->tag == 0
	|| dad->tag == ident_tag)
    {
      dad = father(dad);
    }
    else
      return 0;
  }
#else
  return 1;
#endif
}

/*
 * e is a local declaration; 'enoughs' is a misnomer to say whether there
 * are t-regs available delivers 1 if e can be allocated into t-reg or par
 * reg
 */
static int
tempdec(exp e, bool enoughs)
{
  exp p;

  if (~optim & OPTIM_TEMPDEC)
    return 0;

  nouses = 0;
  useinpar = 0;

  if (isvar(e))
  {
    for (p = pt(e); p != NULL; p = pt(p))
    {
      /* find no of uses which are not assignments to id ... */
      if (!p->last && next(p)->last && next(next(p))->tag == ass_tag)
      {
	if (!simple_seq(next(next(p)), e))
	  return 0;
	continue;
      }
      nouses++;
    }
  }
  else
    nouses = no(e);

  /*
   * trace simple successors to assignmnts or init to id to find if all uses
   * occur before unpredictable change of control (or another assignment to
   * id)
   */

  if (child(e)->tag!= clear_tag || isparam(e))
  {
    after_a(child(e), e);
  }

  if (isvar(e))
  {
    for (p = pt(e); p != NULL; p = pt(p))
    {
      if (!p->last && next(p)->last && next(next(p))->tag == ass_tag)
      {
	after_a(next(next(p)), e);
      }
    }
  }

  if (nouses == 0 && (enoughs || !useinpar))
  {
    if (useinpar)
    {
      /* See if it can be allocated into a parameter register */
      e->props |= notparreg;
      if (isparam(e))
      {
	return param_uses(e);
      }
      else
	return 100;
    }
    return 100;
  }
  return 0;
}
static int param_uses(exp id)
{
  exp p;
  assert(isparam(id));
  assert(useinpar);
  assert(nouses==0);
  /* We found all the uses of the ident and we found one of them in a parameter list */

  for (p=pt(id); p!=NULL;p = pt(p))
  {
    if (APPLYLIKE(father(p)))
    {
      return locate_param(p);
    }
  }
  /* not a simple use in a parameter list */
  return 100;
}
static int locate_param(exp e)
{
  exp f = father(e);
  bool is_float = is_floating(sh(e)->tag);
  exp par;


  assert(APPLYLIKE(f));
  switch (f->tag)
  {
   case apply_general_tag:
    par =  child(next(child(f)));
    break;
   case apply_tag:
    par = next(child(f));
    break;
   case round_tag:
    par = child(f);
    break;
   default:
    return 0;
  }
  {
    int fxparam = R_FIRST_PARAM;
    int flparam = FR_FIRST_PARAM;
    int stparam = 0;

    for (;;)
    {
      int par_size = shape_size(sh(par));

      if (par==e)
      {
	/* We have found it */
	if (is_float)
	{
	  if (flparam>FR_LAST_PARAM)
	    return 0;
	  else
	    return flparam;
	}
	else
	{
	  if (fxparam>end_param)
	    return 0;
	  else
	    return fxparam;
	}
      }
      stparam = ALIGNNEXT(stparam + par_size,32);
      fxparam = (stparam/32) + R_FIRST_PARAM;
      if (is_floating(sh(par)->tag))
      {
	flparam++;
      }
      if (par->last)
	break;
      par = next(par);
    }
    return 0;
  }
}



