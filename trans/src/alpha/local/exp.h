/*
 * Copyright 2011, The TenDRA Project.
 * Copyright 1997, United Kingdom Secretary of State for Defence.
 *
 * See doc/copyright/ for the full copyright terms.
 */


#ifndef LOCAL_EXP_H
#define LOCAL_EXP_H

/* properties of IDENT */

/* Set in weights and used in coder. At this time no copying */
#define set_intnl_call(x) (x)->props |= 0x80
#define has_intnl_call(x) ((x)->props & 0x80)

/* properties of MAKE_PROC construction */
#define set_proc_has_float(x) (x)->props |= 0x100
#define proc_has_float(x) ((x)->props & 0x100)

/* properties of NAME construction */
#define set_vararg(x)	(x)->props |= 0x08
#define is_vararg(x)    ((x)->props & 0x08)

/* properties of STRING construction */
#define string_char_size(e) (e)->props

#if 0
#define proc_has_gen_call(e) (e->props & 0x4000)
#define set_proc_has_gen_call(e) (e)->props |= 0x4000
#endif
#define set_has_c_vararg(x)	(x)->props|=0x400
#define clear_has_c_vararg(x) (x)->props &= ~0x400
#define has_c_vararg(x)		((x)->props & 0x400)
#define set_has_machine_division(x) (x)->props |= 0x2000
#define has_machine_division(x) ((x)->props & 0x2000)
#define has_float(x)	((x)->props & 0x1000)
#define set_has_float(x) (x)->props |= 0x1000

#define frame_al_of_ptr(x) (x)->child.ald->al.frame
#define frame_al1_of_offset(x) (x)->child.ald->al.frame
#define includes_vcallees(x) ((x & 16) != 0)
#define l_or_cees(x) ((x & 25) != 0)

#endif

