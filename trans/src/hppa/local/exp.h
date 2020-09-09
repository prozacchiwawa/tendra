/*
 * Copyright 2011, The TenDRA Project.
 * Copyright 1997, United Kingdom Secretary of State for Defence.
 *
 * See doc/copyright/ for the full copyright terms.
 */

#ifndef LOCAL_EXP_H
#define LOCAL_EXP_H

#include "bits.h"

#define setse_opt(x) (x)->props = (prop) ((x)->props | 0x400)
#define isse_opt(x) ((x)->props & 0x400)

#define setbyteuse(x)                   (x)->props = (prop) ((x)->props | 0x800)

#define frame_al_of_ptr(x) (x)->child.ald->al.frame
#define frame_al1_of_offset(x) (x)->child.ald->al.frame
#define includes_vcallees(x) ((x & 16) != 0)
#define l_or_cees(x) ((x &25) !=0)


/*
 * PROPERTIES OF IDENT CONSTRUCTS
*/

#define set_intnl_call( x )		pset ( x, 0x80 )
#define has_intnl_call( x )		ptst ( x, 0x80 )

#define is_fn_glob(e) ( e->tag==name_tag && child(e)->tag==ident_tag\
	&& ( child(child(e))==NULL || child(child(e))->tag==proc_tag ||\
         child(child(e))->tag==general_proc_tag ) )
#define IS_A_PROC(e) ( e->tag==proc_tag || e->tag==general_proc_tag )


#endif /* EXPMACS_INCLUDED */


