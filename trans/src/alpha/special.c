/* $Id$ */

/*
 * Copyright 2011, The TenDRA Project.
 * Copyright 1997, United Kingdom Secretary of State for Defence.
 *
 * See doc/copyright/ for the full copyright terms.
 */

/*
   special.c
   inlines some of the simpler standard library functions
*/

#include <string.h>

#include <shared/bool.h>
#include <shared/check.h>
#include <shared/error.h>
#include <shared/string.h>

#include <local/ash.h>

#include <tdf/shape.h>
#include <tdf/tag.h>

#include <reader/exp.h>

#include <construct/installtypes.h>
#include <construct/dec.h>
#include <construct/ash.h>

#include "addrtypes.h"
#include "move.h"
#include "pseudo.h"
#include "getregs.h"
#include "guard.h"
#include "code_here.h"
#include "inst_fmt.h"
#include "alpha_ins.h"
#include "labels.h"
#include "procrectypes.h"
#include "bitsmacs.h"
#include "reg_defs.h"
#include "special.h"


static int
specno( char *n)
{
	/* these are the procs I could do
				   something about */
  if (streq(n, "strlen"))
    return 2;
  return 0;
}

static  needs strlenneeds = {
  2, 0, uses2_bit, 0
};

needs
specialneeds(int i)
{
    /* these are the needs for their expansion */
  switch (i) {
    case 2: return strlenneeds;
    default: 
	error(ERR_INTERNAL, "not special fn");
  }
  UNREACHED;
}

int
specialfn(exp fn)
{
    /* could I treat this function load specially ? */
  if (fn->tag == name_tag && son(fn)->tag == ident_tag &&
      isglob (son (fn)) && son (son (fn)) == NULL) {
    char *extname = brog(son (fn)) -> name;
    return specno (extname);
  }
  return 0;
}


int
specialmake(int n, exp par, space sp, where dest, int exitlab)
{
  switch (n) {
    case  2: 		/* translate strlen in the obvious manner 
				*/
      {
	      where w;
	int   lstart = new_label ();
	int   r;
	int   r1;
	setregalt (w.answhere, AT);
	w.ashwhere = ashof (sh (par));
	code_here (par, sp, w);
	r = (dest.answhere.discrim == inreg) ? regalt (dest.answhere)
	  : getreg (sp.fixed);
	r1 = getreg (guardreg (r, sp).fixed);
	if (r == 0) {
	  r = 2;
	}
	      operate_fmt_immediate(i_addq,AT,1,r);
	      /*rri_ins (i_addq, r, 3, 1);*/
	set_label (lstart);

	UNUSED(r1);
/*	ls_ins (i_lb, r1, ind3);
	rri_ins (i_add, 3, 3, 1);*/
/*	condr_ins (i_bnez, r1, lstart);

	operate_fmt (i_sub, r, 3, r);
	setregalt (aa, r);
	move (aa, dest, sp, 1);*/
	return exitlab;
      }
    default: 
	error(ERR_INTERNAL, "not special");
  }
  return exitlab;
}
