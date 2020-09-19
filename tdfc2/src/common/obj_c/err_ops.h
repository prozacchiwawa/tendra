/*
 * Copyright 2002-2011, The TenDRA Project.
 * Copyright 1997-1998, United Kingdom Secretary of State for Defence.
 *
 * See doc/copyright/ for the full copyright terms.
 */

/*
    AUTOMATICALLY GENERATED FROM ALGEBRA c_class (VERSION 1.1)
    BY calculus (VERSION 1.3)
*/

#ifndef ERR_OPS_H_INCLUDED
#define ERR_OPS_H_INCLUDED

/* Operations for union ERROR */

#define TAG_err(P)			(CHECK_NULL(P)->ag_tag)


/* Operations for component severity of union ERROR */

#define err_severity(P)			(CHECK_NULL(P) + 1)


/* Operations for field simple of union ERROR */

#define err_simple_tag			((unsigned)0)
#define IS_err_simple(P)		(CHECK_NULL(P)->ag_tag == 0)

#define err_simple_number(P)		(CHECK_TAG((P), 0) + 2)
#define err_simple_size(P)		(CHECK_TAG((P), 0) + 3)

#define MAKE_err_simple(severity_, number_, c_class_err)\
    {\
	c_class *x863_ = GEN_c_class(4, TYPEID_err);\
	x863_->ag_tag = 0;\
	COPY_int(x863_ + 1, (severity_));\
	COPY_int(x863_ + 2, (number_));\
	COPY_unsigned(x863_ + 3, 0);\
	(c_class_err) = x863_;\
    }

#define DECONS_err_simple(severity_, number_, size_, c_class_err)\
    {\
	c_class *x864_ = CHECK_TAG((c_class_err), 0);\
	(severity_) = DEREF_int(x864_ + 1);\
	(number_) = DEREF_int(x864_ + 2);\
	(size_) = DEREF_unsigned(x864_ + 3);\
    }

#define DESTROY_err_simple(destroyer_, severity_, number_, size_, c_class_err)\
    {\
	c_class *x865_ = CHECK_TAG((c_class_err), 0);\
	(severity_) = DEREF_int(x865_ + 1);\
	(number_) = DEREF_int(x865_ + 2);\
	(size_) = DEREF_unsigned(x865_ + 3);\
	(destroyer_)(x865_, (unsigned)4);\
    }


/* Operations for field compound of union ERROR */

#define err_compound_tag		((unsigned)1)
#define IS_err_compound(P)		(CHECK_NULL(P)->ag_tag == 1)

#define err_compound_head(P)		(CHECK_TAG((P), 1) + 2)
#define err_compound_tail(P)		(CHECK_TAG((P), 1) + 3)

#define MAKE_err_compound(severity_, head_, tail_, c_class_err)\
    {\
	c_class *x866_ = GEN_c_class(4, TYPEID_err);\
	x866_->ag_tag = 1;\
	COPY_int(x866_ + 1, (severity_));\
	COPY_err(x866_ + 2, (head_));\
	COPY_err(x866_ + 3, (tail_));\
	(c_class_err) = x866_;\
    }

#define DECONS_err_compound(severity_, head_, tail_, c_class_err)\
    {\
	c_class *x867_ = CHECK_TAG((c_class_err), 1);\
	(severity_) = DEREF_int(x867_ + 1);\
	(head_) = DEREF_err(x867_ + 2);\
	(tail_) = DEREF_err(x867_ + 3);\
    }

#define DESTROY_err_compound(destroyer_, severity_, head_, tail_, c_class_err)\
    {\
	c_class *x868_ = CHECK_TAG((c_class_err), 1);\
	(severity_) = DEREF_int(x868_ + 1);\
	(head_) = DEREF_err(x868_ + 2);\
	(tail_) = DEREF_err(x868_ + 3);\
	(destroyer_)(x868_, (unsigned)4);\
    }


#endif
