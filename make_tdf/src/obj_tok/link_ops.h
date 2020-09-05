/*
 * Copyright 2011, The TenDRA Project.
 * Copyright 1997, United Kingdom Secretary of State for Defence.
 *
 * See doc/copyright/ for the full copyright terms.
 */

/*
    AUTOMATICALLY GENERATED FROM ALGEBRA tdf (VERSION 1.0)
    BY calculus (VERSION 1.3)
*/

#ifndef LINK_OPS_H_INCLUDED
#define LINK_OPS_H_INCLUDED

/* Operations for union LINKAGE */

#pragma token PROC(EXP : LINKAGE :) EXP : unsigned : TAG_link #
#pragma interface TAG_link


/* Operations for component name of union LINKAGE */

#pragma token PROC(EXP : LINKAGE :)\
    EXP : PTR(string) : link_name #
#pragma interface link_name

/* Operations for component type of union LINKAGE */

#pragma token PROC(EXP : LINKAGE :)\
    EXP : PTR(SORT) : link_type #
#pragma interface link_type

/* Operations for field basic of union LINKAGE */

#pragma token EXP const : unsigned : link_basic_tag #
#pragma token PROC(EXP : LINKAGE :) EXP : int : IS_link_basic #
#pragma interface link_basic_tag IS_link_basic

#pragma token PROC(\
	EXP : string :,\
	EXP : SORT :,\
	EXP lvalue : LINKAGE :\
    ) STATEMENT MAKE_link_basic #
#pragma interface MAKE_link_basic

#pragma token PROC(\
	EXP lvalue : string :,\
	EXP lvalue : SORT :,\
	EXP : LINKAGE :\
    ) STATEMENT DECONS_link_basic #
#pragma interface DECONS_link_basic

#pragma token PROC(\
	EXP : DESTROYER :,\
	EXP lvalue : string :,\
	EXP lvalue : SORT :,\
	EXP : LINKAGE :\
    ) STATEMENT DESTROY_link_basic #
#pragma interface DESTROY_link_basic


#endif
