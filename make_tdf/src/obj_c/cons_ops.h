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

#ifndef CONS_OPS_H_INCLUDED
#define CONS_OPS_H_INCLUDED

/* Operations for union CONSTRUCT */

#define TAG_cons(P)			((unsigned) 0)


/* Operations for component name of union CONSTRUCT */

#define cons_name(P)			(CHECK_NULL(P) + 0)


/* Operations for component encode of union CONSTRUCT */

#define cons_encode(P)			(CHECK_NULL(P) + 1)


/* Operations for component res of union CONSTRUCT */

#define cons_res(P)			(CHECK_NULL(P) + 2)


/* Operations for component pars of union CONSTRUCT */

#define cons_pars(P)			(CHECK_NULL(P) + 3)


/* Operations for component kind of union CONSTRUCT */

#define cons_kind(P)			(CHECK_NULL(P) + 4)


/* Operations for field basic of union CONSTRUCT */

#define cons_basic_tag			((unsigned)0)
#define IS_cons_basic(P)		1


#define MAKE_cons_basic(name_, encode_, res_, pars_, kind_, tdf_cons)\
    {\
	tdf *x68_ = GEN_tdf(5, TYPEID_cons);\
	COPY_string(x68_ + 0, (name_));\
	COPY_unsigned(x68_ + 1, (encode_));\
	COPY_sort(x68_ + 2, (res_));\
	COPY_list(x68_ + 3, (pars_));\
	COPY_unsigned(x68_ + 4, (kind_));\
	(tdf_cons) = x68_;\
    }

#define DECONS_cons_basic(name_, encode_, res_, pars_, kind_, tdf_cons)\
    {\
	tdf *x69_ = (tdf_cons);\
	(name_) = DEREF_string(x69_ + 0);\
	(encode_) = DEREF_unsigned(x69_ + 1);\
	(res_) = DEREF_sort(x69_ + 2);\
	(pars_) = DEREF_list(x69_ + 3);\
	(kind_) = DEREF_unsigned(x69_ + 4);\
    }

#define DESTROY_cons_basic(destroyer_, name_, encode_, res_, pars_, kind_, tdf_cons)\
    {\
	tdf *x70_ = (tdf_cons);\
	(name_) = DEREF_string(x70_ + 0);\
	(encode_) = DEREF_unsigned(x70_ + 1);\
	(res_) = DEREF_sort(x70_ + 2);\
	(pars_) = DEREF_list(x70_ + 3);\
	(kind_) = DEREF_unsigned(x70_ + 4);\
	(destroyer_)(x70_, (unsigned)5);\
    }


#endif
