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

#ifndef INST_OPS_H_INCLUDED
#define INST_OPS_H_INCLUDED

/* Operations for union INSTANCE */

#define TAG_inst(P)			(CHECK_NULL(P)->ag_tag)


/* Operations for component form of union INSTANCE */

#define inst_form(P)			(CHECK_NULL(P) + 1)


/* Operations for component alias of union INSTANCE */

#define inst_alias(P)			(CHECK_NULL(P) + 2)


/* Operations for component next of union INSTANCE */

#define inst_next(P)			(CHECK_NULL(P) + 3)


/* Operations for field templ of union INSTANCE */

#define inst_templ_tag			((unsigned)0)
#define IS_inst_templ(P)		(CHECK_NULL(P)->ag_tag == 0)

#define inst_templ_id(P)		(CHECK_TAG((P), 0) + 4)
#define inst_templ_spec(P)		(CHECK_TAG((P), 0) + 5)
#define inst_templ_access(P)		(CHECK_TAG((P), 0) + 6)
#define inst_templ_mode(P)		(CHECK_TAG((P), 0) + 7)
#define inst_templ_mems(P)		(CHECK_TAG((P), 0) + 8)
#define inst_templ_prev(P)		(CHECK_TAG((P), 0) + 9)

#define MAKE_inst_templ(form_, next_, id_, access_, prev_, c_class_inst)\
    {\
	c_class *x857_ = GEN_c_class(10, TYPEID_inst);\
	x857_->ag_tag = 0;\
	COPY_type(x857_ + 1, (form_));\
	COPY_inst(x857_ + 2, x857_);\
	COPY_inst(x857_ + 3, (next_));\
	COPY_id(x857_ + 4, (id_));\
	COPY_type(x857_ + 5, NULL_type);\
	COPY_dspec(x857_ + 6, (access_));\
	COPY_pptok(x857_ + 7, NULL);\
	COPY_list(x857_ + 8, NULL_list ( IDENTIFIER ));\
	COPY_inst(x857_ + 9, (prev_));\
	(c_class_inst) = x857_;\
    }

#define DECONS_inst_templ(form_, alias_, next_, id_, spec_, access_, mode_, mems_, prev_, c_class_inst)\
    {\
	c_class *x858_ = CHECK_TAG((c_class_inst), 0);\
	(form_) = DEREF_type(x858_ + 1);\
	(alias_) = DEREF_inst(x858_ + 2);\
	(next_) = DEREF_inst(x858_ + 3);\
	(id_) = DEREF_id(x858_ + 4);\
	(spec_) = DEREF_type(x858_ + 5);\
	(access_) = DEREF_dspec(x858_ + 6);\
	(mode_) = DEREF_pptok(x858_ + 7);\
	(mems_) = DEREF_list(x858_ + 8);\
	(prev_) = DEREF_inst(x858_ + 9);\
    }

#define DESTROY_inst_templ(destroyer_, form_, alias_, next_, id_, spec_, access_, mode_, mems_, prev_, c_class_inst)\
    {\
	c_class *x859_ = CHECK_TAG((c_class_inst), 0);\
	(form_) = DEREF_type(x859_ + 1);\
	(alias_) = DEREF_inst(x859_ + 2);\
	(next_) = DEREF_inst(x859_ + 3);\
	(id_) = DEREF_id(x859_ + 4);\
	(spec_) = DEREF_type(x859_ + 5);\
	(access_) = DEREF_dspec(x859_ + 6);\
	(mode_) = DEREF_pptok(x859_ + 7);\
	(mems_) = DEREF_list(x859_ + 8);\
	(prev_) = DEREF_inst(x859_ + 9);\
	(destroyer_)(x859_, (unsigned)10);\
    }


/* Operations for field token of union INSTANCE */

#define inst_token_tag			((unsigned)1)
#define IS_inst_token(P)		(CHECK_NULL(P)->ag_tag == 1)

#define inst_token_no(P)		(CHECK_TAG((P), 1) + 4)

#define MAKE_inst_token(form_, next_, c_class_inst)\
    {\
	c_class *x860_ = GEN_c_class(5, TYPEID_inst);\
	x860_->ag_tag = 1;\
	COPY_type(x860_ + 1, (form_));\
	COPY_inst(x860_ + 2, x860_);\
	COPY_inst(x860_ + 3, (next_));\
	COPY_ulong(x860_ + 4, LINK_NONE);\
	(c_class_inst) = x860_;\
    }

#define DECONS_inst_token(form_, alias_, next_, no_, c_class_inst)\
    {\
	c_class *x861_ = CHECK_TAG((c_class_inst), 1);\
	(form_) = DEREF_type(x861_ + 1);\
	(alias_) = DEREF_inst(x861_ + 2);\
	(next_) = DEREF_inst(x861_ + 3);\
	(no_) = DEREF_ulong(x861_ + 4);\
    }

#define DESTROY_inst_token(destroyer_, form_, alias_, next_, no_, c_class_inst)\
    {\
	c_class *x862_ = CHECK_TAG((c_class_inst), 1);\
	(form_) = DEREF_type(x862_ + 1);\
	(alias_) = DEREF_inst(x862_ + 2);\
	(next_) = DEREF_inst(x862_ + 3);\
	(no_) = DEREF_ulong(x862_ + 4);\
	(destroyer_)(x862_, (unsigned)5);\
    }


#endif
