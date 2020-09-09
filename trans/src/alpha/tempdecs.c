/*
 * Copyright 2011-2012, The TenDRA Project.
 * Copyright 1997, United Kingdom Secretary of State for Defence.
 *
 * See doc/copyright/ for the full copyright terms.
 */

/*  
  tempdec.c - is the value in the declaration required over  
  proc calls ?  If it isn't, declaration can be allocated 
  in t-reg, rather than s-reg.
*/

#include <assert.h>
#include <stddef.h>

#include <shared/bool.h>

#include <local/exp.h>

#include <tdf/tag.h>

#include <reader/exp.h>

#include <construct/exp.h>

#include <main/flags.h>

#include <refactor/const.h>

#include "bits.h"
#include "tempdecs.h"

static int nouses;
static bool useinpar;

/* 
   reduces nouses for each non-assignment use of id encountered 
   in e; sets useinpar if use in actual parameter posn 
   terminates with 0 on applications or jumps terminates with 2 
   on assignment to id otherwise delivers 1
*/
static int
trace_uses(exp e, exp id)
{
  assert(e!=(exp)0);
  assert(id!=(exp)0);
  switch (e->tag) {
    case name_tag: {
      nouses -=(child(e)==id);
      return 1;
    }	
    case apply_general_tag :
    case apply_tag :{
    int u = nouses;
    int p = 1;
    exp l = child(e);
    while( p==1 ) {
      p = trace_uses(l, id);	    	
      if (u!=nouses || p==2) { useinpar=1; }
      if (p==0) nouses = u;
      if (l->last) break;
      l = next(l);
    }
    return 0;
    }	
    
  case ident_tag: {
    exp f = child(e);
    exp s = next(f);
    int a;
    if ( ( e->props & defer_bit) != 0 ) {
      exp t = f;
      f = s;
      s = t;
    }
    a = trace_uses(f, id);
    if (a !=1) return a;
    return trace_uses(s, id);
  }	
    
    
  case case_tag:  {
    trace_uses(child(e), id);
    return 0;
  }
    
  case labst_tag: return 0;
     
  case seq_tag: {
     exp s = child(child(e));
     for(;;) {
       int el = trace_uses(s, id);
       if (el!=1 ) return el;
       if (s->last) return trace_uses(next(child(e)),id);
       s = next(s);		
     }
   }
  case ass_tag: {
    if (isvar(id) && child(e)->tag==name_tag && child(child(e))==id) {
      trace_uses(next(child(e)),id);
      return 2;
    }
    else{
      int nu = nouses;
      if (trace_uses(child(e),id) != 1 ||
	  trace_uses(next(child(e)), id) !=1 ){
	nouses = nu;
	return 0;
      }
      return 1;
    }
  }
  case goto_lv_tag:
  {
    int nu = nouses;
    if(trace_uses(child(e),id) != 1){
      nouses = nu;
    }
    return 0;
  }
  case test_tag:{
    int nu = nouses;
    if((trace_uses(child(e),id) != 1) || (trace_uses(next(child(e)),id)!= 1)){
      nouses = nu;
    }	
    return 0;
   }
   case solve_tag:case cond_tag: {
     return trace_uses(child(e),id);
   }
   case goto_tag:case rep_tag:
     return 0;
   case current_env_tag:
     return 0;
   default: {
     exp s = child(e);
     int nu = nouses; /* s list can be done in any order ...*/
     if (s == NULL) return 1;
     for(;;) {
       int el = trace_uses(s, id);
       if (el!=1 ) { /* ... so reset nouses if any terminate */
	 nouses = nu; 
	 return el; 
       }
       if (s->last) return 1;
       s = next(s);
     }
   }
    
  }
}


static void
after_a(exp a, exp id)
{
  /* apply trace_uses to dynamic successors of a */
  exp dad ;
  exp l;
 tailrec: dad = father(a);
  if (nouses == 0) return;
  if (dad->tag==cond_tag || dad->tag==rep_tag || dad->tag == res_tag
      || dad->tag == solve_tag || dad->tag == labst_tag
      || dad->tag == case_tag || dad->tag== goto_lv_tag
      || dad->tag == test_tag ||  dad->tag == apply_tag) {
    /* dont try too hard ! */
    while (dad->tag != apply_tag && dad !=id) dad = father(dad);
    if (dad->tag == apply_tag) { useinpar =1;}
    return;
  }
  
  
  for (l=a; !l->last; l=next(l)) {
    int u = trace_uses(next(l), id);
    if (u!=1|| nouses==0) return;
  }
  a = dad;
  if (dad!=id) goto tailrec;	     
}



static bool
simple_seq(exp e, exp id)
{
  exp dad = father(e);
  for(;;) {
    if (dad == id) return 1;
    if (dad->tag==seq_tag || dad->tag==0
	|| dad->tag == ident_tag) { dad = father(dad);}
    else return 0;
  }
}

bool
tempdec(exp e, bool enoughs)
{
  /* e is a local declaration; 'enoughs'
     is a misnomer to say whether there are
     t-regs available delivers 1 if e can be
     allocated into t-reg or par reg */
  exp p;
  if (~optim & OPTIM_TEMPDEC) return 0;
  nouses = 0;
  useinpar = 0;
  if (isvar(e) ) {
    for (p=pt(e); p!=NULL; p =pt(p)) {
      /* find no of uses which are not assignments to id ... */
      if (!p->last && next(p)->last && next(next(p))->tag == ass_tag ) {
    (void) simple_seq;
/*	if (!simple_seq(next(next(p)), e) ) return 0;*/
	/* ... in simple sequence */		 
	continue;
      }
      nouses++;
    }
  }
  else nouses = no(e);
  
  /* trace simple successors to assignmnts or init to id to find 
     if all uses occur before unpredictable change of control 
     (or another assignment to id) */
  if (child(e)->tag != clear_tag || isparam(e)) { after_a(child(e), e); }
  if (isvar(e)) {
    for (p=pt(e); p!=NULL; p =pt(p)) {
      if (!p->last && next(p)->last && next(next(p))->tag == ass_tag ) {	
	after_a(next(next(p)), e);
      }
    }
  }

  if (nouses ==0 &&(enoughs || !useinpar) ) {
    if (useinpar) e->props |= notparreg; 
    /* don't allocate this into par reg */
    return 1;
  }
  return 0;
}


		



