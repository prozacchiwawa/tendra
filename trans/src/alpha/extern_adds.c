/*
 * Copyright 2011, The TenDRA Project.
 * Copyright 1997, United Kingdom Secretary of State for Defence.
 *
 * See doc/copyright/ for the full copyright terms.
 */

#include <assert.h>
#include <stddef.h>

#include <shared/bool.h>

#include <local/exp.h>

#include <tdf/shape.h>
#include <tdf/tag.h>

#include <reader/externs.h>

#include <construct/exp.h>

#include "extern_adds.h"

extern exp * ptr_position(exp e);

/* replaces uses of extern in procs by local identification of address;

   initially replace usage chain by
     name:0; pt: usage chain in proc P; child: P; props: !=0 if in loop
	no: no of usages in proc; next: last in usage chain;
    in usages[no of procs]
    Use this to determine if and where to identify global to a proc local

*/

exp * usages;

static bool enter_parents
(exp e)
{
  exp dad =e;
  bool inloop = 0;
  assert(e->tag ==name_tag);
  for (;;) {
    dad = father(dad);
    if (dad->tag ==rep_tag) {
      inloop=1;
    }
    else if (dad->tag ==proc_tag) {
      exp nu = usages[no(dad)];
      if (nu == NULL) {
	nu = getexp(sh(e), e, 1, dad, NULL, 0,0, 0);
	usages[no(dad)] = nu;
      }
      nu->props |= inloop;
      pt(e) = pt(nu);
				/* remember pt(e) before entry */
      pt(nu) = e;
      no(nu) ++;
      return 1;
    }
    else if (dad->tag == ident_tag && isglob(dad)) {
      return 0;
    }
    else if (dad->tag == 102 || dad->tag == hold_tag) {
      /* thou shalt use descriptive names for constants (102 ?) */

      /* could be leftover from exp token expansion with no pars */
      return 0;
    }
  }
}



void global_usages
(exp id, int nop)
{
  exp plist, nextpl;
  int i;
  assert(id->tag ==ident_tag && isglob(id) && child(id) ==NULL);
  if (no(id) ==0) return;
  for (i=0; i<nop; i++) {
    usages[i] = NULL;
  }
  plist = pt(id);
  nextpl = pt(plist);
  pt(id) = NULL;
  no(id) = 0;
  for (;;) {
    if (!enter_parents(plist)) {
      pt(plist) = pt(id);
      pt(id) = plist;
      no(id) ++;
    }
    if ((plist = nextpl) == NULL)break;
    nextpl = pt(plist);
  }
  for (i=0; i<nop; i++) {
    exp ui = usages[i];
    if (ui != NULL) {
      if (ui->props!= 0 ) {
	/* id is used enough in proc i -
	   so identify it locally */
	exp * pi;
	shape sname = f_pointer(f_alignment(sh(id)));
	for (pi= &child(child(ui));;) {
	  if ((*pi)->tag == ident_tag && isparam(*pi)) {
	    pi = &next(child(*pi));
	  }
	  else if ((*pi)->tag == diagnose_tag || (*pi)->tag == prof_tag) {
	    pi = &child(*pi);
	  }
	  else {
	    /* invent new def to identify global ... */
	    exp nl = getexp(sname,
			    *pi, 0, id, pt(id), 0, 0, name_tag);
	    exp ndef = getexp(sh(*pi), next(*pi),(*pi)->last,
			      nl, NULL, 0x10/*don't defer */,
			      0, ident_tag);
	    exp lu = pt(ui);
	    (*pi)->last = true; next(*pi) = ndef;
	    pt(id) = nl; no(id) ++;
	    setcaonly(ndef);
	    *pi = ndef;
	    /*... and replace uses of global by ndef */
	    while (lu != NULL) {
	      exp nlu = pt(lu);
	      if (no(lu)!=0) {
		exp * plu = ptr_position(lu);
		exp nrf = getexp(sh(lu), next(lu), lu->last,
				 lu, NULL, 0, no(lu), reff_tag);
		sh(lu) = sname;
		no(lu) = 0;
		next(lu) = nrf; lu->last = true;
		*plu = nrf;
	      }
	      child(lu) = ndef;
	      pt(lu) = pt(ndef);
	      pt(ndef) = lu; no(ndef) ++;
	      lu = nlu;
	    }
	    break;
	  }
	}
      }
      else {
	/* restore normal usage chain */
	pt(next(ui)) = pt(id);
	pt(id) = pt(ui);
	no(id) += no(ui);
      }
      retcell(ui);
    }
  }
}


