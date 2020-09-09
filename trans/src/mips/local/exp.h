/*
 * Copyright 2011, The TenDRA Project.
 * Copyright 1997, United Kingdom Secretary of State for Defence.
 *
 * See doc/copyright/ for the full copyright terms.
 */

#ifndef LOCAL_EXP_H
#define LOCAL_EXP_H

#define frame_al_of_ptr(x) (x)->child.ald->al.frame
#define frame_al1_of_offset(x) (x)->child.ald->al.frame
#define includes_vcallees(x) ((x & 16) != 0)
#define l_or_cees(x) ((x &25) !=0)

/* properties of IDENT */

/* Set in weights and used in coder. At this time no copying */
#define set_intnl_call(x) (x)->props |= 0x80
#define has_intnl_call(x) ((x)->props & 0x80)

/* properties of STRING construction */
#define string_char_size(e) (e)->props

#endif
