/*
 * Automatically generated from the files:
 *	syntax.sid
 * and
 *	syntax.act
 * by:
 *	/home/arty/dev/tendra/tendra/tdfc2/../sid/obj/src/sid
 */

/* BEGINNING OF HEADER */

#define __SID_SPLIT
#line 87 "syntax.act"



/*
 * Copyright 2011, The TenDRA Project.
 * Copyright 1997-1998, United Kingdom Secretary of State for Defence.
 *
 * See doc/copyright/ for the full copyright terms.
 */

#include <stdio.h>

#include <shared/check.h>

#include <utility/config.h>
#include "c_types.h"

#include "exp_ops.h"
#include "hashid_ops.h"
#include "id_ops.h"
#include "type_ops.h"
#include <utility/error.h>
#include <utility/catalog.h>
#include <utility/buffer.h>
#include <utility/option.h>
#include <construct/access.h>
#include <construct/allocate.h>
#include <construct/assign.h>
#include <construct/basetype.h>
#include <construct/cast.h>
#include <construct/chktype.h>
#include <construct/class.h>
#include <parse/constant.h>
#include <construct/construct.h>
#include <construct/convert.h>
#include <construct/declare.h>
#include <construct/derive.h>
#include <output/dump.h>
#include <construct/exception.h>
#include <construct/expression.h>
#include <construct/function.h>
#include <parse/hash.h>
#include <construct/identifier.h>
#include <construct/initialise.h>
#include <construct/inttype.h>
#include <construct/label.h>
#include <parse/preproc.h>
#include <parse/lex.h>
#include <parse/literal.h>
#include <construct/member.h>
#include <construct/namespace.h>
#include <parse/parse.h>
#include <parse/pragma.h>
#include <parse/predict.h>
#include <parse/preproc.h>
#include <construct/redeclare.h>
#include <construct/rewrite.h>
#include <construct/statement.h>
#include <parse/symbols.h>
#include <construct/template.h>
#include <construct/tokdef.h>
#include <construct/token.h>
#include <construct/typeid.h>
#include <construct/variable.h>


/*
    These are the aliases for the compound types used in the parser.
*/

typedef LIST ( EXP ) SID_LIST_EXP ;


/*
    The function declarations are included at this point so that the
    type definitions are in scope.
*/

#include <syntax/syntax.h>


#line 96 "syntax3.c"

void
ZR820(BASE_TYPE *ZIbt, TYPE *ZIt1, CV_SPEC *ZIcv, DECL_SPEC *ZIds1)
{
	switch (CURRENT_TERMINAL) {
	default:
		{
			/* BEGINNING OF ACTION: declare_id_empty */
			{
#line 1179 "syntax.act"

    IGNORE empty_decl ( (*ZIds1), NULL_type, (*ZIbt), (*ZIt1), (*ZIcv), last_lex_token, 0 ) ;
    have_type_declaration = TYPE_DECL_NONE ;
    have_func_declarator = 0 ;
    have_type_specifier = 0 ;
#line 112 "syntax3.c"
			}
			/* END OF ACTION: declare_id_empty */
		}
		break;
	case 1: case 2: case 4: case 65:
	case 78:
		{
			TYPE ZIt;
			DECL_SPEC ZIds;

			/* BEGINNING OF ACTION: dspec_complete */
			{
#line 1140 "syntax.act"

    /* Complete a declaration specifier and a type */
    (ZIds) = complete_dspec ( (*ZIds1), (*ZIbt), (*ZIt1), (*ZIcv) ) ;
    (ZIt) = complete_pre_type ( (*ZIbt), (*ZIt1), (*ZIcv), 0 ) ;
    have_type_specifier = 0 ;
#line 131 "syntax3.c"
			}
			/* END OF ACTION: dspec_complete */
			ZRinit_Hdeclarator_Hlist (ZIds, *ZIbt, ZIt);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
		}
		break;
	case 356:
		return;
	}
	return;
ZL1:;
	SAVE_LEXER (356);
	return;
}

void
ZRfor_Hend_Hstatement(EXP *ZOe)
{
	EXP ZIe;

	switch (CURRENT_TERMINAL) {
	case 1: case 17: case 18: case 19:
	case 20: case 21: case 22: case 23:
	case 33: case 43: case 59: case 61:
	case 62: case 65: case 69: case 71:
	case 78: case 85: case 116: case 170:
	case 206: case 210: case 255: case 298:
	case 324: case 347:
		{
			EXP ZIa;
			EXP ZIb;

			ZRexpression (&ZIa);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: stmt_exp */
			{
#line 705 "syntax.act"

    (ZIb) = make_exp_stmt ( (ZIa) ) ;
#line 177 "syntax3.c"
			}
			/* END OF ACTION: stmt_exp */
			/* BEGINNING OF ACTION: bind_temporary */
			{
#line 824 "syntax.act"

    (ZIe) = bind_temporary ( (ZIb) ) ;
#line 185 "syntax3.c"
			}
			/* END OF ACTION: bind_temporary */
		}
		break;
	default:
		{
			/* BEGINNING OF ACTION: exp_none */
			{
#line 431 "syntax.act"

    (ZIe) = NULL_exp ;
#line 197 "syntax3.c"
			}
			/* END OF ACTION: exp_none */
		}
		break;
	case 356:
		return;
	}
	goto ZL0;
ZL1:;
	SAVE_LEXER (356);
	return;
ZL0:;
	*ZOe = ZIe;
}

void
ZRand_Hexpression(EXP *ZO911)
{
	EXP ZI911;

	if ((CURRENT_TERMINAL) == 356) {
		return;
	}
	{
		EXP ZIe;

		ZRmaxmin_Hexpression (&ZIe);
		ZR912 (ZIe, &ZI911);
		if ((CURRENT_TERMINAL) == 356) {
			RESTORE_LEXER;
			goto ZL1;
		}
	}
	goto ZL0;
ZL1:;
	SAVE_LEXER (356);
	return;
ZL0:;
	*ZO911 = ZI911;
}

void
parse_exp(EXP *ZOe)
{
	EXP ZIe;

	if ((CURRENT_TERMINAL) == 356) {
		return;
	}
	{
		ZRassignment_Hexpression (&ZIe);
		if ((CURRENT_TERMINAL) == 356) {
			RESTORE_LEXER;
			goto ZL1;
		}
	}
	goto ZL0;
ZL1:;
	{
		/* BEGINNING OF ACTION: error_syntax */
		{
#line 1349 "syntax.act"

    /* Syntax errors */
    ERROR err = ERR_lex_parse ( crt_token ) ;
    report ( crt_loc, err ) ;
    have_syntax_error = 1 ;
#line 265 "syntax3.c"
		}
		/* END OF ACTION: error_syntax */
		/* BEGINNING OF ACTION: exp_none */
		{
#line 431 "syntax.act"

    (ZIe) = NULL_exp ;
#line 273 "syntax3.c"
		}
		/* END OF ACTION: exp_none */
	}
ZL0:;
	*ZOe = ZIe;
}

void
ZRtype_Hqualifier(BASE_TYPE *ZObt, TYPE *ZOt, CV_SPEC *ZOcv)
{
	BASE_TYPE ZIbt;
	TYPE ZIt;
	CV_SPEC ZIcv;

	switch (CURRENT_TERMINAL) {
	case 98: case 124:
		{
			ZRcv_Hqualifier (&ZIcv);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: btype_none */
			{
#line 933 "syntax.act"
 (ZIbt) = btype_none ; 
#line 300 "syntax3.c"
			}
			/* END OF ACTION: btype_none */
			/* BEGINNING OF ACTION: type_none */
			{
#line 956 "syntax.act"

    (ZIt) = NULL_type ;
#line 308 "syntax3.c"
			}
			/* END OF ACTION: type_none */
		}
		break;
	case 2: case 25: case 97: case 102:
	case 104: case 106: case 110: case 111:
	case 114: case 115: case 118: case 121:
	case 122: case 123: case 156: case 185:
	case 286: case 299:
		{
			ZRtype_Hspecifier (&ZIbt, &ZIt);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: cv_none */
			{
#line 902 "syntax.act"
 (ZIcv) = cv_none ; 
#line 328 "syntax3.c"
			}
			/* END OF ACTION: cv_none */
		}
		break;
	case 356:
		return;
	default:
		goto ZL1;
	}
	goto ZL0;
ZL1:;
	SAVE_LEXER (356);
	return;
ZL0:;
	*ZObt = ZIbt;
	*ZOt = ZIt;
	*ZOcv = ZIcv;
}

void
ZR1079(IDENTIFIER *ZId, SID_LIST_EXP *ZOq)
{
	SID_LIST_EXP ZIq;

	switch (CURRENT_TERMINAL) {
	case 1: case 17: case 18: case 19:
	case 20: case 21: case 22: case 23:
	case 33: case 43: case 46: case 59:
	case 61: case 62: case 64: case 65:
	case 66: case 69: case 71: case 78:
	case 85: case 116: case 170: case 206:
	case 210: case 255:
		{
			ZRinitialiser_Hlist (*ZId, &ZIq);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
		}
		break;
	default:
		{
			/* BEGINNING OF ACTION: list_exp_null */
			{
#line 412 "syntax.act"

    (ZIq) = NULL_list ( EXP ) ;
#line 376 "syntax3.c"
			}
			/* END OF ACTION: list_exp_null */
		}
		break;
	case 356:
		return;
	}
	goto ZL0;
ZL1:;
	SAVE_LEXER (356);
	return;
ZL0:;
	*ZOq = ZIq;
}

void
ZR952(BASE_TYPE *ZI948, TYPE *ZI949, CV_SPEC *ZI950, DECL_SPEC *ZI951, EXP *ZOe)
{
	EXP ZIe;

	switch (CURRENT_TERMINAL) {
	case 77:
		{
			/* BEGINNING OF ACTION: declare_id_empty */
			{
#line 1179 "syntax.act"

    IGNORE empty_decl ( (*ZI951), NULL_type, (*ZI948), (*ZI949), (*ZI950), last_lex_token, 0 ) ;
    have_type_declaration = TYPE_DECL_NONE ;
    have_func_declarator = 0 ;
    have_type_specifier = 0 ;
#line 408 "syntax3.c"
			}
			/* END OF ACTION: declare_id_empty */
			ADVANCE_LEXER;
			/* BEGINNING OF ACTION: exp_none */
			{
#line 431 "syntax.act"

    (ZIe) = NULL_exp ;
#line 417 "syntax3.c"
			}
			/* END OF ACTION: exp_none */
		}
		break;
	case 1: case 2: case 4: case 65:
	case 78:
		{
			TYPE ZIt;
			DECL_SPEC ZIds;
			TYPE ZIs;
			IDENTIFIER ZIid;

			/* BEGINNING OF ACTION: dspec_complete */
			{
#line 1140 "syntax.act"

    /* Complete a declaration specifier and a type */
    (ZIds) = complete_dspec ( (*ZI951), (*ZI948), (*ZI949), (*ZI950) ) ;
    (ZIt) = complete_pre_type ( (*ZI948), (*ZI949), (*ZI950), 0 ) ;
    have_type_specifier = 0 ;
#line 438 "syntax3.c"
			}
			/* END OF ACTION: dspec_complete */
			ZRdeclarator (ZIt, &ZIs, &ZIid);
			ZR826 (ZI948, &ZIt, &ZIds, &ZIs, &ZIid);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: exp_none */
			{
#line 431 "syntax.act"

    (ZIe) = NULL_exp ;
#line 452 "syntax3.c"
			}
			/* END OF ACTION: exp_none */
		}
		break;
	case 356:
		return;
	default:
		goto ZL1;
	}
	goto ZL0;
ZL1:;
	SAVE_LEXER (356);
	return;
ZL0:;
	*ZOe = ZIe;
}

void
ZR826(BASE_TYPE *ZIbt, TYPE *ZIt, DECL_SPEC *ZIds, TYPE *ZIs, IDENTIFIER *ZIid)
{
	if ((CURRENT_TERMINAL) == 356) {
		return;
	}
	{
		IDENTIFIER ZId;
		int ZIb;
		EXP ZIa;

		/* BEGINNING OF ACTION: is_function */
		{
#line 1422 "syntax.act"

    (ZI0) = function_params ( (*ZIs) ) ;
#line 486 "syntax3.c"
		}
		/* END OF ACTION: is_function */
		if (!ZI0)
			goto ZL2;
		/* BEGINNING OF ACTION: define_func */
		{
#line 1173 "syntax.act"

    (ZId) = make_func_decl ( (*ZIds), (*ZIs), (*ZIid), 1 ) ;
    have_type_declaration = TYPE_DECL_NONE ;
    have_func_declarator = 0 ;
#line 498 "syntax3.c"
		}
		/* END OF ACTION: define_func */
		/* BEGINNING OF ACTION: function_begin */
		{
#line 864 "syntax.act"

    (ZIb) = in_class_defn ;
    in_class_defn = 0 ;
    in_function_defn++ ;
    really_in_function_defn++ ;
    begin_function ( (ZId) ) ;
#line 510 "syntax3.c"
		}
		/* END OF ACTION: function_begin */
		ZRfunction_Hbody (&ZIa);
		if ((CURRENT_TERMINAL) == 356) {
			RESTORE_LEXER;
			goto ZL1;
		}
		/* BEGINNING OF ACTION: function_end */
		{
#line 872 "syntax.act"

    IGNORE end_function ( (ZId), (ZIa) ) ;
    in_class_defn = (ZIb) ;
    in_function_defn-- ;
    really_in_function_defn-- ;
#line 526 "syntax3.c"
		}
		/* END OF ACTION: function_end */
		/* BEGINNING OF ACTION: rescan_token */
		{
#line 1470 "syntax.act"

    RESCAN_LEXER ;
#line 534 "syntax3.c"
		}
		/* END OF ACTION: rescan_token */
		goto ZL0;
	}
	UNREACHED;
ZL2:;
	switch (CURRENT_TERMINAL) {
	case 36: case 42: case 77:
		{
			IDENTIFIER ZId;
			EXP ZIa;

			/* BEGINNING OF ACTION: declare_id */
			{
#line 1154 "syntax.act"

    if ( in_weak_param ) {
	(ZId) = make_param_decl ( (*ZIds), (*ZIs), (*ZIid), CONTEXT_WEAK_PARAM ) ;
    } else if ( type_tag ( (*ZIs) ) == type_func_tag ) {
	check_weak_func ( (*ZIs), 0 ) ;
	(ZId) = make_func_decl ( (*ZIds), (*ZIs), (*ZIid), 0 ) ;
    } else {
	int def = predict_obj_defn () ;
	(ZId) = make_object_decl ( (*ZIds), (*ZIs), (*ZIid), def ) ;
    }
    if ( IS_id_type_alias ( (ZId) ) ) {
	BASE_TYPE bs = DEREF_btype ( id_type_alias_rep ( (ZId) ) ) ;
	bs |= (*ZIbt) ;
	COPY_btype ( id_type_alias_rep ( (ZId) ), bs ) ;
    }
    have_type_declaration = TYPE_DECL_NONE ;
    have_func_declarator = 0 ;
#line 567 "syntax3.c"
			}
			/* END OF ACTION: declare_id */
			ZRinitialiser_Hopt (ZId, &ZIa);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: initialise_id */
			{
#line 1260 "syntax.act"

    int def = init_object ( (ZId), (ZIa) ) ;
    if ( do_dump ) dump_declare ( (ZId), &decl_loc, def ) ;
#line 581 "syntax3.c"
			}
			/* END OF ACTION: initialise_id */
			/* BEGINNING OF INLINE: 827 */
			{
				switch (CURRENT_TERMINAL) {
				case 42:
					{
						ADVANCE_LEXER;
						ZRinit_Hdeclarator_Hlist (*ZIds, *ZIbt, *ZIt);
						if ((CURRENT_TERMINAL) == 356) {
							RESTORE_LEXER;
							goto ZL1;
						}
					}
					break;
				default:
					break;
				}
			}
			/* END OF INLINE: 827 */
			switch (CURRENT_TERMINAL) {
			case 77:
				break;
			default:
				goto ZL1;
			}
			ADVANCE_LEXER;
		}
		break;
	default:
		goto ZL1;
	}
	return;
ZL1:;
	SAVE_LEXER (356);
	return;
ZL0:;
}

void
parse_stmt(EXP *ZOe)
{
	EXP ZIe;

	if ((CURRENT_TERMINAL) == 356) {
		return;
	}
	{
		ZRstatement (&ZIe);
		if ((CURRENT_TERMINAL) == 356) {
			RESTORE_LEXER;
			goto ZL1;
		}
	}
	goto ZL0;
ZL1:;
	{
		/* BEGINNING OF ACTION: error_syntax */
		{
#line 1349 "syntax.act"

    /* Syntax errors */
    ERROR err = ERR_lex_parse ( crt_token ) ;
    report ( crt_loc, err ) ;
    have_syntax_error = 1 ;
#line 647 "syntax3.c"
		}
		/* END OF ACTION: error_syntax */
		/* BEGINNING OF ACTION: exp_none */
		{
#line 431 "syntax.act"

    (ZIe) = NULL_exp ;
#line 655 "syntax3.c"
		}
		/* END OF ACTION: exp_none */
	}
ZL0:;
	*ZOe = ZIe;
}

void
ZRmember_Hdeclarator_Hlist(TYPE ZIp, BASE_TYPE ZIq)
{
	if ((CURRENT_TERMINAL) == 356) {
		return;
	}
ZL2_member_Hdeclarator_Hlist:;
	{
		ZRmember_Hdeclarator (ZIp, ZIq);
		/* BEGINNING OF INLINE: 833 */
		{
			switch (CURRENT_TERMINAL) {
			case 42:
				{
					ADVANCE_LEXER;
					/* BEGINNING OF INLINE: member-declarator-list */
					goto ZL2_member_Hdeclarator_Hlist;
					/* END OF INLINE: member-declarator-list */
				}
				UNREACHED;
			case 356:
				RESTORE_LEXER;
				goto ZL1;
			default:
				break;
			}
		}
		/* END OF INLINE: 833 */
	}
	return;
ZL1:;
	SAVE_LEXER (356);
	return;
}

void
ZRtype_Hid(TYPE *ZOt, int *ZOn)
{
	TYPE ZIt;
	int ZIn;

	if ((CURRENT_TERMINAL) == 356) {
		return;
	}
	{
		int ZIn1;
		BASE_TYPE ZIbt;
		TYPE ZIp;
		CV_SPEC ZIcv;
		TYPE ZIq;

		/* BEGINNING OF ACTION: no_type_defns */
		{
#line 1382 "syntax.act"

    (ZIn1) = no_type_defns ;
#line 719 "syntax3.c"
		}
		/* END OF ACTION: no_type_defns */
		ZRtype_Hspecifier_Hseq (&ZIbt, &ZIp, &ZIcv);
		if ((CURRENT_TERMINAL) == 356) {
			RESTORE_LEXER;
			goto ZL1;
		}
		/* BEGINNING OF ACTION: type_complete */
		{
#line 992 "syntax.act"

    (ZIq) = complete_pre_type ( (ZIbt), (ZIp), (ZIcv), 1 ) ;
    have_type_specifier = 0 ;
#line 733 "syntax3.c"
		}
		/* END OF ACTION: type_complete */
		ZRabstract_Hdeclarator_Hopt (ZIq, &ZIt);
		if ((CURRENT_TERMINAL) == 356) {
			RESTORE_LEXER;
			goto ZL1;
		}
		/* BEGINNING OF ACTION: diff_type_defns */
		{
#line 1390 "syntax.act"

    (ZIn) = no_type_defns - (ZIn1) ;
#line 746 "syntax3.c"
		}
		/* END OF ACTION: diff_type_defns */
		/* BEGINNING OF ACTION: type_check */
		{
#line 1001 "syntax.act"

    object_type ( (ZIt), null_tag ) ;
#line 754 "syntax3.c"
		}
		/* END OF ACTION: type_check */
	}
	goto ZL0;
ZL1:;
	SAVE_LEXER (356);
	return;
ZL0:;
	*ZOt = ZIt;
	*ZOn = ZIn;
}

void
ZRflow_Hexpression(EXP *ZOe)
{
	EXP ZIe;

	switch (CURRENT_TERMINAL) {
	case 298:
		{
			EXP ZIa;

			ADVANCE_LEXER;
			switch (CURRENT_TERMINAL) {
			case 65:
				break;
			default:
				goto ZL1;
			}
			ADVANCE_LEXER;
			ZRexpression (&ZIa);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: exp_set */
			{
#line 614 "syntax.act"

    (ZIe) = make_set_exp ( (ZIa) ) ;
#line 795 "syntax3.c"
			}
			/* END OF ACTION: exp_set */
			switch (CURRENT_TERMINAL) {
			case 39:
				break;
			default:
				goto ZL1;
			}
			ADVANCE_LEXER;
		}
		break;
	case 324:
		{
			EXP ZIa;

			ADVANCE_LEXER;
			switch (CURRENT_TERMINAL) {
			case 65:
				break;
			default:
				goto ZL1;
			}
			ADVANCE_LEXER;
			ZRexpression (&ZIa);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: exp_unused */
			{
#line 626 "syntax.act"

    (ZIe) = make_unused_exp ( (ZIa) ) ;
#line 829 "syntax3.c"
			}
			/* END OF ACTION: exp_unused */
			switch (CURRENT_TERMINAL) {
			case 39:
				break;
			default:
				goto ZL1;
			}
			ADVANCE_LEXER;
		}
		break;
	case 356:
		return;
	default:
		goto ZL1;
	}
	goto ZL0;
ZL1:;
	SAVE_LEXER (356);
	return;
ZL0:;
	*ZOe = ZIe;
}

void
ZR1082(EXP *ZI1080, EXP *ZOe)
{
	EXP ZIe;

	switch (CURRENT_TERMINAL) {
	case 1: case 17: case 18: case 19:
	case 20: case 21: case 22: case 23:
	case 33: case 43: case 59: case 61:
	case 62: case 65: case 69: case 71:
	case 78: case 85: case 116: case 170:
	case 206: case 210: case 255: case 298:
	case 324: case 347:
		{
			SID_LIST_EXP ZIq;
			SID_LIST_EXP ZIp;

			ZRcomma_Hexpression_Htail (&ZIq);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: list_exp_cons */
			{
#line 416 "syntax.act"

    CONS_exp ( (*ZI1080), (ZIq), (ZIp) ) ;
#line 881 "syntax3.c"
			}
			/* END OF ACTION: list_exp_cons */
			/* BEGINNING OF ACTION: exp_comma */
			{
#line 466 "syntax.act"

    (ZIe) = make_comma_exp ( (ZIp) ) ;
#line 889 "syntax3.c"
			}
			/* END OF ACTION: exp_comma */
		}
		break;
	default:
		{
			ZIe = *ZI1080;
		}
		break;
	case 356:
		return;
	}
	goto ZL0;
ZL1:;
	SAVE_LEXER (356);
	return;
ZL0:;
	*ZOe = ZIe;
}

void
ZRdeclaration_Hcond_Hbody(void)
{
	if ((CURRENT_TERMINAL) == 356) {
		return;
	}
	{
		DECL_SPEC ZIds;
		TYPE ZIt;

		/* BEGINNING OF INLINE: open-brace */
		{
			{
				switch (CURRENT_TERMINAL) {
				case 64:
					break;
				default:
					goto ZL1;
				}
				ADVANCE_LEXER;
			}
		}
		/* END OF INLINE: open-brace */
		/* BEGINNING OF ACTION: dspec_none */
		{
#line 1121 "syntax.act"
 (ZIds) = dspec_none ; 
#line 937 "syntax3.c"
		}
		/* END OF ACTION: dspec_none */
		/* BEGINNING OF ACTION: type_none */
		{
#line 956 "syntax.act"

    (ZIt) = NULL_type ;
#line 945 "syntax3.c"
		}
		/* END OF ACTION: type_none */
		ZRdeclaration_Hseq_Hopt ();
		/* BEGINNING OF INLINE: close-brace */
		{
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
			{
				switch (CURRENT_TERMINAL) {
				case 38:
					break;
				default:
					goto ZL1;
				}
				ADVANCE_LEXER;
			}
		}
		/* END OF INLINE: close-brace */
	}
	return;
ZL1:;
	SAVE_LEXER (356);
	return;
}

void
ZR957(EXP *ZI955, EXP *ZOe)
{
	EXP ZIe;

	switch (CURRENT_TERMINAL) {
	case 72:
		{
			EXP ZIa;
			EXP ZIb;

			ADVANCE_LEXER;
			ZRexpression (&ZIa);
			switch (CURRENT_TERMINAL) {
			case 41:
				break;
			case 356:
				RESTORE_LEXER;
				goto ZL1;
			default:
				goto ZL1;
			}
			ADVANCE_LEXER;
			ZRconditional_Hexpression (&ZIb);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: exp_cond */
			{
#line 470 "syntax.act"

    (ZIe) = make_cond_exp ( (*ZI955), (ZIa), (ZIb) ) ;
#line 1006 "syntax3.c"
			}
			/* END OF ACTION: exp_cond */
		}
		break;
	default:
		{
			ZIe = *ZI955;
		}
		break;
	case 356:
		return;
	}
	goto ZL0;
ZL1:;
	SAVE_LEXER (356);
	return;
ZL0:;
	*ZOe = ZIe;
}

void
ZR1085(EXP *ZOe)
{
	EXP ZIe;

	switch (CURRENT_TERMINAL) {
	case 298:
		{
			EXP ZI1098;
			EXP ZI1099;

			ADVANCE_LEXER;
			ZRexpression (&ZI1098);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: exp_set */
			{
#line 614 "syntax.act"

    (ZI1099) = make_set_exp ( (ZI1098) ) ;
#line 1049 "syntax3.c"
			}
			/* END OF ACTION: exp_set */
			ZR1100 (&ZI1099, &ZIe);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
		}
		break;
	case 324:
		{
			EXP ZI1095;
			EXP ZI1096;

			ADVANCE_LEXER;
			ZRexpression (&ZI1095);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: exp_unused */
			{
#line 626 "syntax.act"

    (ZI1096) = make_unused_exp ( (ZI1095) ) ;
#line 1075 "syntax3.c"
			}
			/* END OF ACTION: exp_unused */
			ZR1100 (&ZI1096, &ZIe);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
		}
		break;
	case 356:
		return;
	default:
		goto ZL1;
	}
	goto ZL0;
ZL1:;
	SAVE_LEXER (356);
	return;
ZL0:;
	*ZOe = ZIe;
}

void
ZR704(BASE_TYPE *ZIb1, TYPE *ZIt1, CV_SPEC *ZIcv1, BASE_TYPE *ZObt, TYPE *ZOt, CV_SPEC *ZOcv)
{
	BASE_TYPE ZIbt;
	TYPE ZIt;
	CV_SPEC ZIcv;

	if ((CURRENT_TERMINAL) == 356) {
		return;
	}
	{
		BASE_TYPE ZI1035;
		TYPE ZI1036;
		CV_SPEC ZI1037;
		BASE_TYPE ZIb2;
		TYPE ZIt2;
		CV_SPEC ZIcv2;

		/* BEGINNING OF ACTION: is_type_specifier */
		{
#line 1451 "syntax.act"

    /* Resolve type-specifiers from other declarators */
    (ZI0) = predict_tspec ( 0 ) ;
#line 1122 "syntax3.c"
		}
		/* END OF ACTION: is_type_specifier */
		if (!ZI0)
			goto ZL2;
		ZRtype_Hqualifier (&ZI1035, &ZI1036, &ZI1037);
		if ((CURRENT_TERMINAL) == 356) {
			RESTORE_LEXER;
			goto ZL1;
		}
		/* BEGINNING OF ACTION: check_decl_specifier */
		{
#line 1474 "syntax.act"

    /* A type-name can be a declarator-id */
    if ( have_type_specifier && crt_lex_token == lex_type_Hname ) {
	crt_lex_token = lex_identifier ;
    }
#line 1140 "syntax3.c"
		}
		/* END OF ACTION: check_decl_specifier */
		ZR704 (&ZI1035, &ZI1036, &ZI1037, &ZIb2, &ZIt2, &ZIcv2);
		if ((CURRENT_TERMINAL) == 356) {
			RESTORE_LEXER;
			goto ZL1;
		}
		/* BEGINNING OF ACTION: btype_join */
		{
#line 935 "syntax.act"

    if ( (*ZIb1) & (ZIb2) ) {
	(ZIbt) = join_pre_types ( (*ZIb1), (ZIb2) ) ;
    } else {
	(ZIbt) = ( (*ZIb1) | (ZIb2) ) ;
    }
#line 1157 "syntax3.c"
		}
		/* END OF ACTION: btype_join */
		/* BEGINNING OF ACTION: type_join */
		{
#line 980 "syntax.act"

    /* Join two partial types */
    if ( IS_NULL_type ( (*ZIt1) ) ) {
	(ZIt) = (ZIt2) ;
    } else if ( IS_NULL_type ( (ZIt2) ) ) {
	(ZIt) = (*ZIt1) ;
    } else {
	report ( crt_loc, ERR_dcl_type_simple_many ( (*ZIt1), (ZIt2) ) ) ;
	(ZIt) = (ZIt2) ;
    }
#line 1173 "syntax3.c"
		}
		/* END OF ACTION: type_join */
		/* BEGINNING OF ACTION: cv_join */
		{
#line 906 "syntax.act"

    CV_SPEC c = ( (*ZIcv1) & (ZIcv2) ) ;
    if ( c ) report ( crt_loc, ERR_dcl_type_cv_dup ( c ) ) ;
    (ZIcv) = ( (*ZIcv1) | (ZIcv2) ) ;
#line 1183 "syntax3.c"
		}
		/* END OF ACTION: cv_join */
		goto ZL0;
	}
	UNREACHED;
ZL2:;
	switch (CURRENT_TERMINAL) {
	default:
		{
			ZIbt = *ZIb1;
			ZIt = *ZIt1;
			ZIcv = *ZIcv1;
		}
		break;
	}
	goto ZL0;
ZL1:;
	SAVE_LEXER (356);
	return;
ZL0:;
	*ZObt = ZIbt;
	*ZOt = ZIt;
	*ZOcv = ZIcv;
}

void
ZRexpression_Hlist(SID_LIST_EXP *ZOp)
{
	SID_LIST_EXP ZIp;

	if ((CURRENT_TERMINAL) == 356) {
		return;
	}
	{
		EXP ZIe;
		SID_LIST_EXP ZIq;

		ZRassignment_Hexpression (&ZIe);
		ZR629 (&ZIq);
		if ((CURRENT_TERMINAL) == 356) {
			RESTORE_LEXER;
			goto ZL1;
		}
		/* BEGINNING OF ACTION: list_exp_cons */
		{
#line 416 "syntax.act"

    CONS_exp ( (ZIe), (ZIq), (ZIp) ) ;
#line 1232 "syntax3.c"
		}
		/* END OF ACTION: list_exp_cons */
	}
	goto ZL0;
ZL1:;
	SAVE_LEXER (356);
	return;
ZL0:;
	*ZOp = ZIp;
}

void
ZR1086(EXP *ZOe)
{
	EXP ZIe;

	switch (CURRENT_TERMINAL) {
	case 96:
		{
			EXP ZIc;
			EXP ZIa;
			EXP ZIb;

			ADVANCE_LEXER;
			ZRconstant_Hexpression (&ZIc);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: stmt_case_begin */
			{
#line 651 "syntax.act"

    (ZIa) = begin_case_stmt ( (ZIc), 0 ) ;
#line 1267 "syntax3.c"
			}
			/* END OF ACTION: stmt_case_begin */
			/* BEGINNING OF ACTION: stmt_label_set */
			{
#line 768 "syntax.act"

    unreached_fall = 0 ;
#line 1275 "syntax3.c"
			}
			/* END OF ACTION: stmt_label_set */
			switch (CURRENT_TERMINAL) {
			case 41:
				break;
			default:
				goto ZL1;
			}
			ADVANCE_LEXER;
			ZRstatement (&ZIb);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: stmt_case_end */
			{
#line 655 "syntax.act"

    (ZIe) = end_case_stmt ( (ZIa), (ZIb) ) ;
#line 1295 "syntax3.c"
			}
			/* END OF ACTION: stmt_case_end */
		}
		break;
	case 100:
		{
			EXP ZIa;
			EXP ZIb;

			ADVANCE_LEXER;
			/* BEGINNING OF ACTION: stmt_default_begin */
			{
#line 689 "syntax.act"

    (ZIa) = begin_default_stmt ( 0 ) ;
#line 1311 "syntax3.c"
			}
			/* END OF ACTION: stmt_default_begin */
			/* BEGINNING OF ACTION: stmt_label_set */
			{
#line 768 "syntax.act"

    unreached_fall = 0 ;
#line 1319 "syntax3.c"
			}
			/* END OF ACTION: stmt_label_set */
			switch (CURRENT_TERMINAL) {
			case 41:
				break;
			default:
				goto ZL1;
			}
			ADVANCE_LEXER;
			ZRstatement (&ZIb);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: stmt_default_end */
			{
#line 693 "syntax.act"

    (ZIe) = end_default_stmt ( (ZIa), (ZIb) ) ;
#line 1339 "syntax3.c"
			}
			/* END OF ACTION: stmt_default_end */
		}
		break;
	case 356:
		return;
	default:
		goto ZL1;
	}
	goto ZL0;
ZL1:;
	SAVE_LEXER (356);
	return;
ZL0:;
	*ZOe = ZIe;
}

void
ZRassignment_Hexpression(EXP *ZOe)
{
	EXP ZIe;

	switch (CURRENT_TERMINAL) {
	case 170:
		{
			int ZI1013;
			EXP ZI1014;

			ADVANCE_LEXER;
			/* BEGINNING OF ACTION: lex_alignof */
			{
#line 369 "syntax.act"
 (ZI1013) = lex_alignof ; 
#line 1373 "syntax3.c"
			}
			/* END OF ACTION: lex_alignof */
			ZRsizeof_Hexpression (ZI1013, &ZI1014);
			ZR1015 (&ZI1014, &ZIe);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
		}
		break;
	case 23:
		{
			EXP ZI1025;
			EXP ZI1026;

			/* BEGINNING OF EXTRACT: complex-exp */
			{
#line 329 "syntax.act"

    IDENTIFIER id = crt_token->pp_data.tok.id ;
    PPTOKEN *args = crt_token->pp_data.tok.args ;
    ZI1025 = parse_exp_token ( id, args ) ;
    RESCAN_LEXER ;
#line 1397 "syntax3.c"
			}
			/* END OF EXTRACT: complex-exp */
			ADVANCE_LEXER;
			ZR940 (ZI1025, &ZI1026);
			ZR1015 (&ZI1026, &ZIe);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
		}
		break;
	case 206:
		{
			EXP ZIa;
			EXP ZI968;
			EXP ZI967;
			EXP ZI966;
			EXP ZI965;
			EXP ZI964;
			EXP ZI963;
			EXP ZI962;
			EXP ZI961;
			EXP ZI960;
			EXP ZI959;
			EXP ZI958;
			EXP ZI955;

			ADVANCE_LEXER;
			ZRcast_Hexpression (&ZIa);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: exp_ignore */
			{
#line 508 "syntax.act"

    (ZI968) = make_cast_exp ( type_void, (ZIa), 0 ) ;
#line 1436 "syntax3.c"
			}
			/* END OF ACTION: exp_ignore */
			ZR936 (ZI968, &ZI967);
			ZR932 (ZI967, &ZI966);
			ZR928 (ZI966, &ZI965);
			ZR924 (ZI965, &ZI964);
			ZR920 (ZI964, &ZI963);
			ZR916 (ZI963, &ZI962);
			ZR912 (ZI962, &ZI961);
			ZR908 (ZI961, &ZI960);
			ZR904 (ZI960, &ZI959);
			ZR900 (ZI959, &ZI958);
			ZR896 (ZI958, &ZI955);
			ZR957 (&ZI955, &ZIe);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
		}
		break;
	case 210:
		{
			EXP ZI1022;
			EXP ZI1023;

			ADVANCE_LEXER;
			/* BEGINNING OF ACTION: exp_ellipsis */
			{
#line 486 "syntax.act"

    (ZI1022) = make_ellipsis_exp () ;
#line 1468 "syntax3.c"
			}
			/* END OF ACTION: exp_ellipsis */
			ZR940 (ZI1022, &ZI1023);
			ZR1015 (&ZI1023, &ZIe);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
		}
		break;
	case 1:
		{
			IDENTIFIER ZIid;
			EXP ZI1019;
			EXP ZI1020;

			/* BEGINNING OF EXTRACT: identifier */
			{
#line 195 "syntax.act"

    ZIid = crt_token->pp_data.id.use ;
#line 1490 "syntax3.c"
			}
			/* END OF EXTRACT: identifier */
			ADVANCE_LEXER;
			/* BEGINNING OF ACTION: exp_identifier */
			{
#line 504 "syntax.act"

    (ZI1019) = make_id_exp ( (ZIid) ) ;
#line 1499 "syntax3.c"
			}
			/* END OF ACTION: exp_identifier */
			ZR940 (ZI1019, &ZI1020);
			ZR1015 (&ZI1020, &ZIe);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
		}
		break;
	case 255:
		{
			EXP ZIa;
			EXP ZI968;
			EXP ZI967;
			EXP ZI966;
			EXP ZI965;
			EXP ZI964;
			EXP ZI963;
			EXP ZI962;
			EXP ZI961;
			EXP ZI960;
			EXP ZI959;
			EXP ZI958;
			EXP ZI955;

			ADVANCE_LEXER;
			ZRcast_Hexpression (&ZIa);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: exp_lit */
			{
#line 520 "syntax.act"

    (ZI968) = make_lit_exp ( (ZIa) ) ;
#line 1537 "syntax3.c"
			}
			/* END OF ACTION: exp_lit */
			ZR936 (ZI968, &ZI967);
			ZR932 (ZI967, &ZI966);
			ZR928 (ZI966, &ZI965);
			ZR924 (ZI965, &ZI964);
			ZR920 (ZI964, &ZI963);
			ZR916 (ZI963, &ZI962);
			ZR912 (ZI962, &ZI961);
			ZR908 (ZI961, &ZI960);
			ZR904 (ZI960, &ZI959);
			ZR900 (ZI959, &ZI958);
			ZR896 (ZI958, &ZI955);
			ZR957 (&ZI955, &ZIe);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
		}
		break;
	case 61:
		{
			EXP ZI994;
			EXP ZI995;

			ADVANCE_LEXER;
			ZRunary_Hexpression (&ZI994);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: exp_predec */
			{
#line 589 "syntax.act"

    (ZI995) = make_prefix_exp ( lex_minus_Hminus, (ZI994) ) ;
#line 1574 "syntax3.c"
			}
			/* END OF ACTION: exp_predec */
			ZR1015 (&ZI995, &ZIe);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
		}
		break;
	case 65:
		{
			ADVANCE_LEXER;
			ZR990 (&ZIe);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
		}
		break;
	case 71:
		{
			EXP ZI991;
			EXP ZI992;

			ADVANCE_LEXER;
			ZRunary_Hexpression (&ZI991);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: exp_preinc */
			{
#line 593 "syntax.act"

    (ZI992) = make_prefix_exp ( lex_plus_Hplus, (ZI991) ) ;
#line 1610 "syntax3.c"
			}
			/* END OF ACTION: exp_preinc */
			ZR1015 (&ZI992, &ZIe);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
		}
		break;
	case 116:
		{
			int ZI1010;
			EXP ZI1011;

			ADVANCE_LEXER;
			/* BEGINNING OF ACTION: lex_sizeof */
			{
#line 370 "syntax.act"
 (ZI1010) = lex_sizeof ; 
#line 1630 "syntax3.c"
			}
			/* END OF ACTION: lex_sizeof */
			ZRsizeof_Hexpression (ZI1010, &ZI1011);
			ZR1015 (&ZI1011, &ZIe);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
		}
		break;
	case 78:
		{
			EXP ZI997;
			EXP ZI998;

			ADVANCE_LEXER;
			ZRcast_Hexpression (&ZI997);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: exp_indir */
			{
#line 516 "syntax.act"

    (ZI998) = make_indir_exp ( (ZI997) ) ;
#line 1657 "syntax3.c"
			}
			/* END OF ACTION: exp_indir */
			ZR1015 (&ZI998, &ZIe);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
		}
		break;
	case 33:
		{
			EXP ZI1000;
			EXP ZI1001;

			/* BEGINNING OF INLINE: and */
			{
				{
					switch (CURRENT_TERMINAL) {
					case 33:
						break;
					default:
						goto ZL1;
					}
					ADVANCE_LEXER;
				}
			}
			/* END OF INLINE: and */
			ZRcast_Hexpression (&ZI1000);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: exp_ref */
			{
#line 597 "syntax.act"

    (ZI1001) = make_ref_exp ( (ZI1000), 0 ) ;
#line 1695 "syntax3.c"
			}
			/* END OF ACTION: exp_ref */
			ZR1015 (&ZI1001, &ZIe);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
		}
		break;
	case 17: case 18: case 19: case 20:
	case 21: case 22:
		{
			EXP ZI1016;
			EXP ZI1017;

			ZRliteral (&ZI1016);
			ZR940 (ZI1016, &ZI1017);
			ZR1015 (&ZI1017, &ZIe);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
		}
		break;
	case 62:
		{
			EXP ZI1003;
			EXP ZI1004;

			/* BEGINNING OF INLINE: not */
			{
				{
					switch (CURRENT_TERMINAL) {
					case 62:
						break;
					default:
						goto ZL1;
					}
					ADVANCE_LEXER;
				}
			}
			/* END OF INLINE: not */
			ZRcast_Hexpression (&ZI1003);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: exp_not */
			{
#line 560 "syntax.act"

    (ZI1004) = make_not_exp ( (ZI1003) ) ;
#line 1748 "syntax3.c"
			}
			/* END OF ACTION: exp_not */
			ZR1015 (&ZI1004, &ZIe);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
		}
		break;
	case 43: case 59: case 69: case 85:
		{
			int ZI1006;
			EXP ZI1007;
			EXP ZI1008;

			/* BEGINNING OF ACTION: lex_crt */
			{
#line 366 "syntax.act"
 (ZI1006) = crt_lex_token ; 
#line 1768 "syntax3.c"
			}
			/* END OF ACTION: lex_crt */
			ZRunary_Hoperator ();
			ZRcast_Hexpression (&ZI1007);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: exp_unary */
			{
#line 622 "syntax.act"

    (ZI1008) = make_uminus_exp ( (ZI1006), (ZI1007) ) ;
#line 1782 "syntax3.c"
			}
			/* END OF ACTION: exp_unary */
			ZR1015 (&ZI1008, &ZIe);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
		}
		break;
	case 356:
		return;
	default:
		goto ZL1;
	}
	goto ZL0;
ZL1:;
	SAVE_LEXER (356);
	return;
ZL0:;
	*ZOe = ZIe;
}

void
ZRexpression_Hlist_Hopt(SID_LIST_EXP *ZOp)
{
	SID_LIST_EXP ZIp;

	switch (CURRENT_TERMINAL) {
	case 1: case 17: case 18: case 19:
	case 20: case 21: case 22: case 23:
	case 33: case 43: case 59: case 61:
	case 62: case 65: case 69: case 71:
	case 78: case 85: case 116: case 170:
	case 206: case 210: case 255:
		{
			ZRexpression_Hlist (&ZIp);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
		}
		break;
	default:
		{
			/* BEGINNING OF ACTION: list_exp_null */
			{
#line 412 "syntax.act"

    (ZIp) = NULL_list ( EXP ) ;
#line 1832 "syntax3.c"
			}
			/* END OF ACTION: list_exp_null */
		}
		break;
	case 356:
		return;
	}
	goto ZL0;
ZL1:;
	SAVE_LEXER (356);
	return;
ZL0:;
	*ZOp = ZIp;
}

void
ZR1089(IDENTIFIER *ZIid, EXP *ZOe)
{
	EXP ZIe;

	switch (CURRENT_TERMINAL) {
	case 33: case 34: case 35: case 36:
	case 42: case 44: case 45: case 46:
	case 48: case 49: case 50: case 53:
	case 54: case 55: case 56: case 57:
	case 58: case 59: case 60: case 61:
	case 63: case 65: case 66: case 67:
	case 68: case 69: case 70: case 71:
	case 72: case 73: case 74: case 75:
	case 76: case 77: case 78: case 79:
	case 80: case 81: case 86: case 87:
		{
			EXP ZI1019;
			EXP ZI1020;
			EXP ZI1028;
			EXP ZIa;
			int ZIr;

			/* BEGINNING OF ACTION: exp_identifier */
			{
#line 504 "syntax.act"

    (ZI1019) = make_id_exp ( (*ZIid) ) ;
#line 1876 "syntax3.c"
			}
			/* END OF ACTION: exp_identifier */
			ZR940 (ZI1019, &ZI1020);
			ZR1015 (&ZI1020, &ZI1028);
			ZR1030 (&ZI1028, &ZIa);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: reach_check */
			{
#line 835 "syntax.act"

    (ZIr) = unreached_code ;
    if ( (ZIr) ) {
	if ( !unreached_last ) {
	    report ( crt_loc, ERR_stmt_stmt_unreach () ) ;
	    unreached_last = 1 ;
	}
    } else {
	unreached_last = 0 ;
    }
#line 1899 "syntax3.c"
			}
			/* END OF ACTION: reach_check */
			/* BEGINNING OF ACTION: stmt_exp */
			{
#line 705 "syntax.act"

    (ZIe) = make_exp_stmt ( (ZIa) ) ;
#line 1907 "syntax3.c"
			}
			/* END OF ACTION: stmt_exp */
			/* BEGINNING OF ACTION: stmt_label_clear */
			{
#line 772 "syntax.act"

    unreached_fall = 1 ;
#line 1915 "syntax3.c"
			}
			/* END OF ACTION: stmt_label_clear */
			switch (CURRENT_TERMINAL) {
			case 77:
				break;
			default:
				goto ZL1;
			}
			ADVANCE_LEXER;
		}
		break;
	case 41:
		{
			EXP ZIa;
			EXP ZIb;

			/* BEGINNING OF ACTION: stmt_label_mod */
			{
#line 776 "syntax.act"

    if ( unreached_code ) unreached_fall = 0 ;
#line 1937 "syntax3.c"
			}
			/* END OF ACTION: stmt_label_mod */
			/* BEGINNING OF ACTION: stmt_label_begin */
			{
#line 760 "syntax.act"

    (ZIa) = begin_label_stmt ( (*ZIid), lex_identifier ) ;
#line 1945 "syntax3.c"
			}
			/* END OF ACTION: stmt_label_begin */
			ADVANCE_LEXER;
			ZRstatement (&ZIb);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: stmt_label_end */
			{
#line 764 "syntax.act"

    (ZIe) = end_label_stmt ( (ZIa), (ZIb) ) ;
#line 1959 "syntax3.c"
			}
			/* END OF ACTION: stmt_label_end */
		}
		break;
	case 356:
		return;
	default:
		goto ZL1;
	}
	goto ZL0;
ZL1:;
	SAVE_LEXER (356);
	return;
ZL0:;
	*ZOe = ZIe;
}

void
ZRlogical_Hand_Hexpression(EXP *ZO899)
{
	EXP ZI899;

	if ((CURRENT_TERMINAL) == 356) {
		return;
	}
	{
		EXP ZIe;

		ZRinclusive_Hor_Hexpression (&ZIe);
		ZR900 (ZIe, &ZI899);
		if ((CURRENT_TERMINAL) == 356) {
			RESTORE_LEXER;
			goto ZL1;
		}
	}
	goto ZL0;
ZL1:;
	SAVE_LEXER (356);
	return;
ZL0:;
	*ZO899 = ZI899;
}

void
ZRjump_Hstatement(EXP *ZOe)
{
	EXP ZIe;

	switch (CURRENT_TERMINAL) {
	case 95:
		{
			int ZIr;

			ADVANCE_LEXER;
			/* BEGINNING OF ACTION: reach_check */
			{
#line 835 "syntax.act"

    (ZIr) = unreached_code ;
    if ( (ZIr) ) {
	if ( !unreached_last ) {
	    report ( crt_loc, ERR_stmt_stmt_unreach () ) ;
	    unreached_last = 1 ;
	}
    } else {
	unreached_last = 0 ;
    }
#line 2027 "syntax3.c"
			}
			/* END OF ACTION: reach_check */
			/* BEGINNING OF ACTION: stmt_break */
			{
#line 647 "syntax.act"

    (ZIe) = make_break_stmt () ;
#line 2035 "syntax3.c"
			}
			/* END OF ACTION: stmt_break */
			/* BEGINNING OF ACTION: stmt_label_clear */
			{
#line 772 "syntax.act"

    unreached_fall = 1 ;
#line 2043 "syntax3.c"
			}
			/* END OF ACTION: stmt_label_clear */
			ZRsemicolon_Hx ();
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
		}
		break;
	case 99:
		{
			int ZIr;

			ADVANCE_LEXER;
			/* BEGINNING OF ACTION: reach_check */
			{
#line 835 "syntax.act"

    (ZIr) = unreached_code ;
    if ( (ZIr) ) {
	if ( !unreached_last ) {
	    report ( crt_loc, ERR_stmt_stmt_unreach () ) ;
	    unreached_last = 1 ;
	}
    } else {
	unreached_last = 0 ;
    }
#line 2071 "syntax3.c"
			}
			/* END OF ACTION: reach_check */
			/* BEGINNING OF ACTION: stmt_continue */
			{
#line 680 "syntax.act"

    (ZIe) = make_continue_stmt () ;
#line 2079 "syntax3.c"
			}
			/* END OF ACTION: stmt_continue */
			/* BEGINNING OF ACTION: stmt_label_clear */
			{
#line 772 "syntax.act"

    unreached_fall = 1 ;
#line 2087 "syntax3.c"
			}
			/* END OF ACTION: stmt_label_clear */
			ZRsemicolon_Hx ();
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
		}
		break;
	case 108:
		{
			int ZIr;

			ADVANCE_LEXER;
			/* BEGINNING OF ACTION: reach_check */
			{
#line 835 "syntax.act"

    (ZIr) = unreached_code ;
    if ( (ZIr) ) {
	if ( !unreached_last ) {
	    report ( crt_loc, ERR_stmt_stmt_unreach () ) ;
	    unreached_last = 1 ;
	}
    } else {
	unreached_last = 0 ;
    }
#line 2115 "syntax3.c"
			}
			/* END OF ACTION: reach_check */
			ZRjump_Hlabel (&ZIe);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: stmt_label_clear */
			{
#line 772 "syntax.act"

    unreached_fall = 1 ;
#line 2128 "syntax3.c"
			}
			/* END OF ACTION: stmt_label_clear */
			ZRsemicolon_Hx ();
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
		}
		break;
	case 113:
		{
			int ZIr;
			EXP ZIa;

			ADVANCE_LEXER;
			/* BEGINNING OF ACTION: reach_check */
			{
#line 835 "syntax.act"

    (ZIr) = unreached_code ;
    if ( (ZIr) ) {
	if ( !unreached_last ) {
	    report ( crt_loc, ERR_stmt_stmt_unreach () ) ;
	    unreached_last = 1 ;
	}
    } else {
	unreached_last = 0 ;
    }
#line 2157 "syntax3.c"
			}
			/* END OF ACTION: reach_check */
			ZR620 (&ZIa);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: stmt_return */
			{
#line 780 "syntax.act"

    (ZIe) = make_return_stmt ( (ZIa), lex_return ) ;
#line 2170 "syntax3.c"
			}
			/* END OF ACTION: stmt_return */
			/* BEGINNING OF ACTION: stmt_label_clear */
			{
#line 772 "syntax.act"

    unreached_fall = 1 ;
#line 2178 "syntax3.c"
			}
			/* END OF ACTION: stmt_label_clear */
			ZRsemicolon_Hx ();
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
		}
		break;
	case 356:
		return;
	default:
		goto ZL1;
	}
	goto ZL0;
ZL1:;
	SAVE_LEXER (356);
	return;
ZL0:;
	*ZOe = ZIe;
}

void
ZR1090(IDENTIFIER *ZIid, EXP *ZOe)
{
	EXP ZIe;

	switch (CURRENT_TERMINAL) {
	default:
		{
			EXP ZIa;

			/* BEGINNING OF ACTION: exp_identifier */
			{
#line 504 "syntax.act"

    (ZIa) = make_id_exp ( (*ZIid) ) ;
#line 2216 "syntax3.c"
			}
			/* END OF ACTION: exp_identifier */
			/* BEGINNING OF ACTION: stmt_exp */
			{
#line 705 "syntax.act"

    (ZIe) = make_exp_stmt ( (ZIa) ) ;
#line 2224 "syntax3.c"
			}
			/* END OF ACTION: stmt_exp */
		}
		break;
	case 41:
		{
			EXP ZIa;
			EXP ZIb;

			/* BEGINNING OF ACTION: stmt_label_mod */
			{
#line 776 "syntax.act"

    if ( unreached_code ) unreached_fall = 0 ;
#line 2239 "syntax3.c"
			}
			/* END OF ACTION: stmt_label_mod */
			/* BEGINNING OF ACTION: stmt_label_begin */
			{
#line 760 "syntax.act"

    (ZIa) = begin_label_stmt ( (*ZIid), lex_identifier ) ;
#line 2247 "syntax3.c"
			}
			/* END OF ACTION: stmt_label_begin */
			ADVANCE_LEXER;
			ZRstatement (&ZIb);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: stmt_label_end */
			{
#line 764 "syntax.act"

    (ZIe) = end_label_stmt ( (ZIa), (ZIb) ) ;
#line 2261 "syntax3.c"
			}
			/* END OF ACTION: stmt_label_end */
		}
		break;
	case 356:
		return;
	}
	goto ZL0;
ZL1:;
	SAVE_LEXER (356);
	return;
ZL0:;
	*ZOe = ZIe;
}

void
ZRtarget_Hcondition_Hhead(EXP *ZO889, EXP *ZO890, int *ZO891)
{
	EXP ZI889;
	EXP ZI890;
	int ZI891;

	if ((CURRENT_TERMINAL) == 356) {
		return;
	}
	{
		EXP ZIc;
		EXP ZIp;
		int ZIr;
		EXP ZIa;
		EXP ZIe;

		switch (CURRENT_TERMINAL) {
		case 26:
			/* BEGINNING OF EXTRACT: hash-if */
			{
#line 313 "syntax.act"

    ZIc = crt_token->pp_data.exp ;
#line 2301 "syntax3.c"
			}
			/* END OF EXTRACT: hash-if */
			break;
		default:
			goto ZL1;
		}
		ADVANCE_LEXER;
		/* BEGINNING OF ACTION: cond_hash_if */
		{
#line 1313 "syntax.act"

    EXP c = crt_hash_cond ;
    crt_hash_cond = make_if_cond ( (ZIc), c ) ;
    (ZIp) = c ;
#line 2316 "syntax3.c"
		}
		/* END OF ACTION: cond_hash_if */
		/* BEGINNING OF ACTION: reach_check */
		{
#line 835 "syntax.act"

    (ZIr) = unreached_code ;
    if ( (ZIr) ) {
	if ( !unreached_last ) {
	    report ( crt_loc, ERR_stmt_stmt_unreach () ) ;
	    unreached_last = 1 ;
	}
    } else {
	unreached_last = 0 ;
    }
#line 2332 "syntax3.c"
		}
		/* END OF ACTION: reach_check */
		ZRcompound_Hstatement (&ZIa);
		if ((CURRENT_TERMINAL) == 356) {
			RESTORE_LEXER;
			goto ZL1;
		}
		/* BEGINNING OF ACTION: reach_prev */
		{
#line 847 "syntax.act"
 unreached_prev = (ZIr) ; 
#line 2344 "syntax3.c"
		}
		/* END OF ACTION: reach_prev */
		/* BEGINNING OF ACTION: stmt_hash_if */
		{
#line 804 "syntax.act"

    (ZIe) = begin_hash_if_stmt ( (ZIc), (ZIa) ) ;
#line 2352 "syntax3.c"
		}
		/* END OF ACTION: stmt_hash_if */
		ZR892 (ZIe, ZIp, ZIr, &ZI889, &ZI890, &ZI891);
		if ((CURRENT_TERMINAL) == 356) {
			RESTORE_LEXER;
			goto ZL1;
		}
	}
	goto ZL0;
ZL1:;
	SAVE_LEXER (356);
	return;
ZL0:;
	*ZO889 = ZI889;
	*ZO890 = ZI890;
	*ZO891 = ZI891;
}

void
ZRcv_Hqualifier_Hseq(CV_SPEC *ZOcv)
{
	CV_SPEC ZIcv;

	if ((CURRENT_TERMINAL) == 356) {
		return;
	}
	{
		CV_SPEC ZIa;

		ZRcv_Hqualifier (&ZIa);
		/* BEGINNING OF INLINE: 638 */
		{
			switch (CURRENT_TERMINAL) {
			case 98: case 124:
				{
					CV_SPEC ZIb;

					ZRcv_Hqualifier_Hseq (&ZIb);
					if ((CURRENT_TERMINAL) == 356) {
						RESTORE_LEXER;
						goto ZL1;
					}
					/* BEGINNING OF ACTION: cv_join */
					{
#line 906 "syntax.act"

    CV_SPEC c = ( (ZIa) & (ZIb) ) ;
    if ( c ) report ( crt_loc, ERR_dcl_type_cv_dup ( c ) ) ;
    (ZIcv) = ( (ZIa) | (ZIb) ) ;
#line 2402 "syntax3.c"
					}
					/* END OF ACTION: cv_join */
				}
				break;
			default:
				{
					ZIcv = ZIa;
				}
				break;
			case 356:
				RESTORE_LEXER;
				goto ZL1;
			}
		}
		/* END OF INLINE: 638 */
	}
	goto ZL0;
ZL1:;
	SAVE_LEXER (356);
	return;
ZL0:;
	*ZOcv = ZIcv;
}

void
ZRfunction_Hbody(EXP *ZOe)
{
	EXP ZIe;

	if ((CURRENT_TERMINAL) == 356) {
		return;
	}
	{
		EXP ZIc;
		int ZId;
		EXP ZIb;
		EXP ZIr;
		EXP ZIa;

		/* BEGINNING OF ACTION: stmt_compound_begin */
		{
#line 659 "syntax.act"

    (ZIc) = begin_compound_stmt ( 1 ) ;
#line 2447 "syntax3.c"
		}
		/* END OF ACTION: stmt_compound_begin */
		/* BEGINNING OF INLINE: open-brace */
		{
			{
				switch (CURRENT_TERMINAL) {
				case 64:
					break;
				default:
					goto ZL1;
				}
				ADVANCE_LEXER;
			}
		}
		/* END OF INLINE: open-brace */
		/* BEGINNING OF ACTION: stmt_compound_block */
		{
#line 667 "syntax.act"

    COPY_int ( exp_sequence_block ( (ZIc) ), 2 ) ;
    (ZId) = 1 ;
#line 2469 "syntax3.c"
		}
		/* END OF ACTION: stmt_compound_block */
		ZRstatement_Hseq_Hopt (ZIc, ZId, &ZIb);
		if ((CURRENT_TERMINAL) == 356) {
			RESTORE_LEXER;
			goto ZL1;
		}
		/* BEGINNING OF ACTION: stmt_return_fall */
		{
#line 784 "syntax.act"

    (ZIr) = fall_return_stmt () ;
#line 2482 "syntax3.c"
		}
		/* END OF ACTION: stmt_return_fall */
		/* BEGINNING OF ACTION: stmt_compound_add */
		{
#line 672 "syntax.act"

    (ZIa) = add_compound_stmt ( (ZIb), (ZIr) ) ;
#line 2490 "syntax3.c"
		}
		/* END OF ACTION: stmt_compound_add */
		/* BEGINNING OF INLINE: close-brace */
		{
			{
				switch (CURRENT_TERMINAL) {
				case 38:
					break;
				default:
					goto ZL1;
				}
				ADVANCE_LEXER;
			}
		}
		/* END OF INLINE: close-brace */
		/* BEGINNING OF ACTION: stmt_compound_end */
		{
#line 676 "syntax.act"

    (ZIe) = end_compound_stmt ( (ZIa) ) ;
#line 2511 "syntax3.c"
		}
		/* END OF ACTION: stmt_compound_end */
		/* BEGINNING OF ACTION: rescan_token */
		{
#line 1470 "syntax.act"

    RESCAN_LEXER ;
#line 2519 "syntax3.c"
		}
		/* END OF ACTION: rescan_token */
	}
	goto ZL0;
ZL1:;
	SAVE_LEXER (356);
	return;
ZL0:;
	*ZOe = ZIe;
}

void
ZRcv_Hqualifier_Hseq_Hopt(CV_SPEC *ZOcv)
{
	CV_SPEC ZIcv;

	switch (CURRENT_TERMINAL) {
	case 98: case 124:
		{
			ZRcv_Hqualifier_Hseq (&ZIcv);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
		}
		break;
	default:
		{
			/* BEGINNING OF ACTION: cv_none */
			{
#line 902 "syntax.act"
 (ZIcv) = cv_none ; 
#line 2552 "syntax3.c"
			}
			/* END OF ACTION: cv_none */
		}
		break;
	case 356:
		return;
	}
	goto ZL0;
ZL1:;
	SAVE_LEXER (356);
	return;
ZL0:;
	*ZOcv = ZIcv;
}

void
ZR970(EXP *ZOe)
{
	EXP ZIe;

	if ((CURRENT_TERMINAL) == 356) {
		return;
	}
	{
		TYPE ZIt;
		int ZIn;
		EXP ZIa;

		/* BEGINNING OF ACTION: is_type_id_false */
		{
#line 1441 "syntax.act"

    /* Resolve type-ids from expressions */
    (ZI0) = predict_typeid ( 0 ) ;
#line 2587 "syntax3.c"
		}
		/* END OF ACTION: is_type_id_false */
		if (!ZI0)
			goto ZL2;
		ZRtype_Hid (&ZIt, &ZIn);
		switch (CURRENT_TERMINAL) {
		case 39:
			break;
		case 356:
			RESTORE_LEXER;
			goto ZL1;
		default:
			goto ZL1;
		}
		ADVANCE_LEXER;
		ZRcast_Hexpression (&ZIa);
		if ((CURRENT_TERMINAL) == 356) {
			RESTORE_LEXER;
			goto ZL1;
		}
		/* BEGINNING OF ACTION: exp_cast */
		{
#line 461 "syntax.act"

    /* n is the number of type definitions in t */
    (ZIe) = make_cast_exp ( (ZIt), (ZIa), (ZIn) ) ;
#line 2614 "syntax3.c"
		}
		/* END OF ACTION: exp_cast */
		goto ZL0;
	}
	UNREACHED;
ZL2:;
	switch (CURRENT_TERMINAL) {
	case 1: case 17: case 18: case 19:
	case 20: case 21: case 22: case 23:
	case 33: case 43: case 59: case 61:
	case 62: case 65: case 69: case 71:
	case 78: case 85: case 116: case 170:
	case 206: case 210: case 255: case 298:
	case 324: case 347:
		{
			EXP ZIa;
			EXP ZI969;

			/* BEGINNING OF ACTION: exp_paren_begin */
			{
#line 568 "syntax.act"

    IGNORE incr_value ( OPT_VAL_paren_depth ) ;
#line 2638 "syntax3.c"
			}
			/* END OF ACTION: exp_paren_begin */
			ZRexpression (&ZIa);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: exp_paren_end */
			{
#line 572 "syntax.act"

    (ZI969) = make_paren_exp ( (ZIa) ) ;
    decr_value ( OPT_VAL_paren_depth ) ;
#line 2652 "syntax3.c"
			}
			/* END OF ACTION: exp_paren_end */
			switch (CURRENT_TERMINAL) {
			case 39:
				break;
			default:
				goto ZL1;
			}
			ADVANCE_LEXER;
			ZR940 (ZI969, &ZIe);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
		}
		break;
	default:
		goto ZL1;
	}
	goto ZL0;
ZL1:;
	SAVE_LEXER (356);
	return;
ZL0:;
	*ZOe = ZIe;
}

void
ZRfall_Hcheck(void)
{
	switch (CURRENT_TERMINAL) {
	case 223:
		{
			ADVANCE_LEXER;
			ZR1101 ();
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
		}
		break;
	case 356:
		return;
	default:
		break;
	}
	return;
ZL1:;
	SAVE_LEXER (356);
	return;
}

void
ZRcv_Hqualifier(CV_SPEC *ZOcv)
{
	CV_SPEC ZIcv;

	switch (CURRENT_TERMINAL) {
	case 98:
		{
			ADVANCE_LEXER;
			/* BEGINNING OF ACTION: cv_const */
			{
#line 903 "syntax.act"
 (ZIcv) = cv_const ; 
#line 2718 "syntax3.c"
			}
			/* END OF ACTION: cv_const */
		}
		break;
	case 124:
		{
			ADVANCE_LEXER;
			/* BEGINNING OF ACTION: cv_volatile */
			{
#line 904 "syntax.act"
 (ZIcv) = cv_volatile ; 
#line 2730 "syntax3.c"
			}
			/* END OF ACTION: cv_volatile */
		}
		break;
	case 356:
		return;
	default:
		goto ZL1;
	}
	goto ZL0;
ZL1:;
	SAVE_LEXER (356);
	return;
ZL0:;
	*ZOcv = ZIcv;
}

void
ZRunary_Hoperator(void)
{
	switch (CURRENT_TERMINAL) {
	case 85:
		{
			ADVANCE_LEXER;
		}
		break;
	case 59:
		{
			ADVANCE_LEXER;
		}
		break;
	case 69:
		{
			ADVANCE_LEXER;
		}
		break;
	case 43:
		{
			/* BEGINNING OF INLINE: compl */
			{
				{
					switch (CURRENT_TERMINAL) {
					case 43:
						break;
					default:
						goto ZL1;
					}
					ADVANCE_LEXER;
				}
			}
			/* END OF INLINE: compl */
		}
		break;
	case 356:
		return;
	default:
		goto ZL1;
	}
	return;
ZL1:;
	SAVE_LEXER (356);
	return;
}

void
ZRmember_Hdeclarator(TYPE ZIp, BASE_TYPE ZIq)
{
	switch (CURRENT_TERMINAL) {
	case 65:
		{
			TYPE ZI1041;
			IDENTIFIER ZI1042;
			TYPE ZI1038;
			IDENTIFIER ZIid;
			TYPE ZIt;

			ADVANCE_LEXER;
			ZRdeclarator_Haux (&ZI1041, &ZI1042);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: declarator_bad */
			{
#line 1232 "syntax.act"

    if ( IS_NULL_type ( (ZI1041) ) ) {
	report ( crt_loc, ERR_dcl_meaning_paren () ) ;
    }
#line 2820 "syntax3.c"
			}
			/* END OF ACTION: declarator_bad */
			switch (CURRENT_TERMINAL) {
			case 39:
				break;
			default:
				goto ZL1;
			}
			ADVANCE_LEXER;
			ZR875 (ZI1041, ZI1042, &ZI1038, &ZIid);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: type_inject */
			{
#line 1052 "syntax.act"

    (ZIt) = ( IS_NULL_type ( (ZI1038) ) ? (ZIp) : inject_pre_type ( (ZI1038), (ZIp), 1 ) ) ;
#line 2840 "syntax3.c"
			}
			/* END OF ACTION: type_inject */
			/* BEGINNING OF ACTION: declare_member */
			{
#line 1202 "syntax.act"

    IDENTIFIER id = make_member_decl ( dspec_none, (ZIt), (ZIid), 0 ) ;
    if ( do_dump ) dump_declare ( id, &decl_loc, 0 ) ;
    have_type_declaration = TYPE_DECL_NONE ;
    have_func_declarator = 0 ;
#line 2851 "syntax3.c"
			}
			/* END OF ACTION: declare_member */
		}
		break;
	case 1: case 2: case 4:
		{
			IDENTIFIER ZI1045;

			/* BEGINNING OF INLINE: any-identifier */
			{
				switch (CURRENT_TERMINAL) {
				case 1:
					{
						/* BEGINNING OF EXTRACT: identifier */
						{
#line 195 "syntax.act"

    ZI1045 = crt_token->pp_data.id.use ;
#line 2870 "syntax3.c"
						}
						/* END OF EXTRACT: identifier */
						ADVANCE_LEXER;
					}
					break;
				case 4:
					{
						/* BEGINNING OF EXTRACT: statement-name */
						{
#line 207 "syntax.act"

    ZI1045 = crt_token->pp_data.id.use ;
#line 2883 "syntax3.c"
						}
						/* END OF EXTRACT: statement-name */
						ADVANCE_LEXER;
					}
					break;
				case 2:
					{
						/* BEGINNING OF EXTRACT: type-name */
						{
#line 199 "syntax.act"

    ZI1045 = crt_token->pp_data.id.use ;
#line 2896 "syntax3.c"
						}
						/* END OF EXTRACT: type-name */
						ADVANCE_LEXER;
					}
					break;
				default:
					goto ZL1;
				}
			}
			/* END OF INLINE: any-identifier */
			ZR1046 (&ZIp, &ZIq, &ZI1045);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
		}
		break;
	case 78:
		{
			TYPE ZI1039;
			TYPE ZI1040;
			IDENTIFIER ZIid;
			TYPE ZI1038;
			TYPE ZIt;

			ZRptr_Hoperator (&ZI1039);
			ZRdeclarator_Haux (&ZI1040, &ZIid);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: type_build */
			{
#line 1056 "syntax.act"

    (ZI1038) = ( IS_NULL_type ( (ZI1040) ) ? (ZI1039) : inject_pre_type ( (ZI1040), (ZI1039), 0 ) ) ;
#line 2933 "syntax3.c"
			}
			/* END OF ACTION: type_build */
			/* BEGINNING OF ACTION: type_inject */
			{
#line 1052 "syntax.act"

    (ZIt) = ( IS_NULL_type ( (ZI1038) ) ? (ZIp) : inject_pre_type ( (ZI1038), (ZIp), 1 ) ) ;
#line 2941 "syntax3.c"
			}
			/* END OF ACTION: type_inject */
			/* BEGINNING OF ACTION: declare_member */
			{
#line 1202 "syntax.act"

    IDENTIFIER id = make_member_decl ( dspec_none, (ZIt), (ZIid), 0 ) ;
    if ( do_dump ) dump_declare ( id, &decl_loc, 0 ) ;
    have_type_declaration = TYPE_DECL_NONE ;
    have_func_declarator = 0 ;
#line 2952 "syntax3.c"
			}
			/* END OF ACTION: declare_member */
		}
		break;
	case 41:
		{
			IDENTIFIER ZIid;
			EXP ZIc;
			TYPE ZIt;

			/* BEGINNING OF ACTION: id_anon */
			{
#line 386 "syntax.act"

    HASHID nm = lookup_anon () ;
    (ZIid) = DEREF_id ( hashid_id ( nm ) ) ;
#line 2969 "syntax3.c"
			}
			/* END OF ACTION: id_anon */
			/* BEGINNING OF ACTION: declarator_begin */
			{
#line 1227 "syntax.act"

    IDENTIFIER pid = underlying_id ( (ZIid) ) ;
    DEREF_loc ( id_loc ( pid ), decl_loc ) ;
#line 2978 "syntax3.c"
			}
			/* END OF ACTION: declarator_begin */
			ADVANCE_LEXER;
			ZRconstant_Hexpression (&ZIc);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: type_bitfield_mem */
			{
#line 1045 "syntax.act"

    /* Check for anonymous bitfields */
    HASHID nm = DEREF_hashid ( id_name ( (ZIid) ) ) ;
    int z = IS_hashid_anon ( nm ) ;
    (ZIt) = make_bitfield_type ( (ZIp), (ZIq), (ZIc), z ) ;
#line 2995 "syntax3.c"
			}
			/* END OF ACTION: type_bitfield_mem */
			/* BEGINNING OF ACTION: declare_bitfield */
			{
#line 1216 "syntax.act"

    IDENTIFIER id = make_member_decl ( dspec_none, (ZIt), (ZIid), 0 ) ;
    if ( do_dump ) dump_declare ( id, &decl_loc, 0 ) ;
    have_type_declaration = TYPE_DECL_NONE ;
    have_func_declarator = 0 ;
#line 3006 "syntax3.c"
			}
			/* END OF ACTION: declare_bitfield */
		}
		break;
	case 356:
		return;
	default:
		goto ZL1;
	}
	return;
ZL1:;
	SAVE_LEXER (356);
	return;
}

void
ZR1100(EXP *ZI1099, EXP *ZOe)
{
	EXP ZIe;

	switch (CURRENT_TERMINAL) {
	case 348:
		{
			SID_LIST_EXP ZIq;
			SID_LIST_EXP ZIp;
			EXP ZIa;
			int ZIr;

			ADVANCE_LEXER;
			ZRcomma_Hexpression_Htail (&ZIq);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: list_exp_cons */
			{
#line 416 "syntax.act"

    CONS_exp ( (*ZI1099), (ZIq), (ZIp) ) ;
#line 3046 "syntax3.c"
			}
			/* END OF ACTION: list_exp_cons */
			/* BEGINNING OF ACTION: exp_comma */
			{
#line 466 "syntax.act"

    (ZIa) = make_comma_exp ( (ZIp) ) ;
#line 3054 "syntax3.c"
			}
			/* END OF ACTION: exp_comma */
			/* BEGINNING OF ACTION: reach_check */
			{
#line 835 "syntax.act"

    (ZIr) = unreached_code ;
    if ( (ZIr) ) {
	if ( !unreached_last ) {
	    report ( crt_loc, ERR_stmt_stmt_unreach () ) ;
	    unreached_last = 1 ;
	}
    } else {
	unreached_last = 0 ;
    }
#line 3070 "syntax3.c"
			}
			/* END OF ACTION: reach_check */
			/* BEGINNING OF ACTION: stmt_exp */
			{
#line 705 "syntax.act"

    (ZIe) = make_exp_stmt ( (ZIa) ) ;
#line 3078 "syntax3.c"
			}
			/* END OF ACTION: stmt_exp */
			/* BEGINNING OF ACTION: stmt_label_clear */
			{
#line 772 "syntax.act"

    unreached_fall = 1 ;
#line 3086 "syntax3.c"
			}
			/* END OF ACTION: stmt_label_clear */
			switch (CURRENT_TERMINAL) {
			case 77:
				break;
			default:
				goto ZL1;
			}
			ADVANCE_LEXER;
		}
		break;
	case 77:
		{
			int ZIr;

			ADVANCE_LEXER;
			switch (CURRENT_TERMINAL) {
			case 348:
				break;
			default:
				goto ZL1;
			}
			ADVANCE_LEXER;
			/* BEGINNING OF ACTION: reach_check */
			{
#line 835 "syntax.act"

    (ZIr) = unreached_code ;
    if ( (ZIr) ) {
	if ( !unreached_last ) {
	    report ( crt_loc, ERR_stmt_stmt_unreach () ) ;
	    unreached_last = 1 ;
	}
    } else {
	unreached_last = 0 ;
    }
#line 3123 "syntax3.c"
			}
			/* END OF ACTION: reach_check */
			/* BEGINNING OF ACTION: stmt_exp */
			{
#line 705 "syntax.act"

    (ZIe) = make_exp_stmt ( (*ZI1099) ) ;
#line 3131 "syntax3.c"
			}
			/* END OF ACTION: stmt_exp */
			/* BEGINNING OF ACTION: stmt_label_clear */
			{
#line 772 "syntax.act"

    unreached_fall = 1 ;
#line 3139 "syntax3.c"
			}
			/* END OF ACTION: stmt_label_clear */
		}
		break;
	case 356:
		return;
	default:
		goto ZL1;
	}
	goto ZL0;
ZL1:;
	SAVE_LEXER (356);
	return;
ZL0:;
	*ZOe = ZIe;
}

void
ZR1101(void)
{
	switch (CURRENT_TERMINAL) {
	case 77:
		{
			ADVANCE_LEXER;
			/* BEGINNING OF ACTION: stmt_label_set */
			{
#line 768 "syntax.act"

    unreached_fall = 0 ;
#line 3169 "syntax3.c"
			}
			/* END OF ACTION: stmt_label_set */
		}
		break;
	default:
		{
			/* BEGINNING OF ACTION: stmt_label_set */
			{
#line 768 "syntax.act"

    unreached_fall = 0 ;
#line 3181 "syntax3.c"
			}
			/* END OF ACTION: stmt_label_set */
		}
		break;
	case 356:
		return;
	}
}

void
ZRinitialiser_Hclause(IDENTIFIER ZId, EXP *ZOe)
{
	EXP ZIe;

	switch (CURRENT_TERMINAL) {
	case 1: case 17: case 18: case 19:
	case 20: case 21: case 22: case 23:
	case 33: case 43: case 59: case 61:
	case 62: case 65: case 69: case 71:
	case 78: case 85: case 116: case 170:
	case 206: case 210: case 255:
		{
			ZRinitialiser_Hexpression (&ZIe);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
		}
		break;
	case 64:
		{
			SID_LIST_EXP ZIp;

			/* BEGINNING OF INLINE: open-brace */
			{
				{
					switch (CURRENT_TERMINAL) {
					case 64:
						break;
					default:
						goto ZL1;
					}
					ADVANCE_LEXER;
				}
			}
			/* END OF INLINE: open-brace */
			ZR1079 (&ZId, &ZIp);
			/* BEGINNING OF INLINE: close-brace */
			{
				if ((CURRENT_TERMINAL) == 356) {
					RESTORE_LEXER;
					goto ZL1;
				}
				{
					switch (CURRENT_TERMINAL) {
					case 38:
						break;
					default:
						goto ZL1;
					}
					ADVANCE_LEXER;
				}
			}
			/* END OF INLINE: close-brace */
			/* BEGINNING OF ACTION: exp_aggregate */
			{
#line 435 "syntax.act"

    /* The expression type is a dummy */
    MAKE_exp_aggregate ( type_void, (ZIp), NULL_list ( OFFSET ), (ZIe) ) ;
#line 3252 "syntax3.c"
			}
			/* END OF ACTION: exp_aggregate */
		}
		break;
	case 46:
		{
			/* BEGINNING OF INLINE: 796 */
			{
				{
					IDENTIFIER ZIid;
					EXP ZIa;

					switch (CURRENT_TERMINAL) {
					case 46:
						break;
					default:
						goto ZL1;
					}
					ADVANCE_LEXER;
					switch (CURRENT_TERMINAL) {
					case 1:
						/* BEGINNING OF EXTRACT: identifier */
						{
#line 195 "syntax.act"

    ZIid = crt_token->pp_data.id.use ;
#line 3279 "syntax3.c"
						}
						/* END OF EXTRACT: identifier */
						break;
					default:
						goto ZL1;
					}
					ADVANCE_LEXER;
					switch (CURRENT_TERMINAL) {
					case 36:
						break;
					default:
						goto ZL1;
					}
					ADVANCE_LEXER;
					ZRinitialiser_Hclause (ZId, &ZIa);
					if ((CURRENT_TERMINAL) == 356) {
						RESTORE_LEXER;
						goto ZL1;
					}
					/* BEGINNING OF ACTION: exp_designated_with_identifier */
					{
#line 528 "syntax.act"

    MAKE_exp_designated_name( type_void, (ZIid), (ZIa), (ZIe) );
#line 3304 "syntax3.c"
					}
					/* END OF ACTION: exp_designated_with_identifier */
				}
			}
			/* END OF INLINE: 796 */
		}
		break;
	case 66:
		{
			/* BEGINNING OF INLINE: 797 */
			{
				{
					EXP ZIa;
					EXP ZIb;

					/* BEGINNING OF INLINE: open-square */
					{
						{
							switch (CURRENT_TERMINAL) {
							case 66:
								break;
							default:
								goto ZL1;
							}
							ADVANCE_LEXER;
						}
					}
					/* END OF INLINE: open-square */
					ZRconstant_Hexpression (&ZIa);
					/* BEGINNING OF INLINE: close-square */
					{
						if ((CURRENT_TERMINAL) == 356) {
							RESTORE_LEXER;
							goto ZL1;
						}
						{
							switch (CURRENT_TERMINAL) {
							case 40:
								break;
							default:
								goto ZL1;
							}
							ADVANCE_LEXER;
						}
					}
					/* END OF INLINE: close-square */
					ZRinitialiser_Hclause (ZId, &ZIb);
					if ((CURRENT_TERMINAL) == 356) {
						RESTORE_LEXER;
						goto ZL1;
					}
					/* BEGINNING OF ACTION: exp_designated_with_subscript */
					{
#line 532 "syntax.act"

    MAKE_exp_designated_subscript( type_void, (ZIa), (ZIb), (ZIe) );
#line 3361 "syntax3.c"
					}
					/* END OF ACTION: exp_designated_with_subscript */
				}
			}
			/* END OF INLINE: 797 */
		}
		break;
	case 356:
		return;
	default:
		goto ZL1;
	}
	goto ZL0;
ZL1:;
	SAVE_LEXER (356);
	return;
ZL0:;
	*ZOe = ZIe;
}

void
ZRadditive_Hexpression(EXP *ZO931)
{
	EXP ZI931;

	if ((CURRENT_TERMINAL) == 356) {
		return;
	}
	{
		EXP ZIe;

		ZRmultiplicative_Hexpression (&ZIe);
		ZR932 (ZIe, &ZI931);
		if ((CURRENT_TERMINAL) == 356) {
			RESTORE_LEXER;
			goto ZL1;
		}
	}
	goto ZL0;
ZL1:;
	SAVE_LEXER (356);
	return;
ZL0:;
	*ZO931 = ZI931;
}

void
ZRdirect_Hdeclarator(TYPE *ZO873, IDENTIFIER *ZO874)
{
	TYPE ZI873;
	IDENTIFIER ZI874;

	switch (CURRENT_TERMINAL) {
	case 65:
		{
			TYPE ZIt;
			IDENTIFIER ZIid;

			ADVANCE_LEXER;
			ZRdeclarator_Haux (&ZIt, &ZIid);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: declarator_bad */
			{
#line 1232 "syntax.act"

    if ( IS_NULL_type ( (ZIt) ) ) {
	report ( crt_loc, ERR_dcl_meaning_paren () ) ;
    }
#line 3433 "syntax3.c"
			}
			/* END OF ACTION: declarator_bad */
			switch (CURRENT_TERMINAL) {
			case 39:
				break;
			default:
				goto ZL1;
			}
			ADVANCE_LEXER;
			ZR875 (ZIt, ZIid, &ZI873, &ZI874);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
		}
		break;
	case 1: case 2: case 4:
		{
			IDENTIFIER ZIid;
			TYPE ZIt;

			/* BEGINNING OF INLINE: any-identifier */
			{
				switch (CURRENT_TERMINAL) {
				case 1:
					{
						/* BEGINNING OF EXTRACT: identifier */
						{
#line 195 "syntax.act"

    ZIid = crt_token->pp_data.id.use ;
#line 3465 "syntax3.c"
						}
						/* END OF EXTRACT: identifier */
						ADVANCE_LEXER;
					}
					break;
				case 4:
					{
						/* BEGINNING OF EXTRACT: statement-name */
						{
#line 207 "syntax.act"

    ZIid = crt_token->pp_data.id.use ;
#line 3478 "syntax3.c"
						}
						/* END OF EXTRACT: statement-name */
						ADVANCE_LEXER;
					}
					break;
				case 2:
					{
						/* BEGINNING OF EXTRACT: type-name */
						{
#line 199 "syntax.act"

    ZIid = crt_token->pp_data.id.use ;
#line 3491 "syntax3.c"
						}
						/* END OF EXTRACT: type-name */
						ADVANCE_LEXER;
					}
					break;
				default:
					goto ZL1;
				}
			}
			/* END OF INLINE: any-identifier */
			/* BEGINNING OF ACTION: type_none */
			{
#line 956 "syntax.act"

    (ZIt) = NULL_type ;
#line 3507 "syntax3.c"
			}
			/* END OF ACTION: type_none */
			/* BEGINNING OF ACTION: declarator_begin */
			{
#line 1227 "syntax.act"

    IDENTIFIER pid = underlying_id ( (ZIid) ) ;
    DEREF_loc ( id_loc ( pid ), decl_loc ) ;
#line 3516 "syntax3.c"
			}
			/* END OF ACTION: declarator_begin */
			ZR875 (ZIt, ZIid, &ZI873, &ZI874);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
		}
		break;
	case 356:
		return;
	default:
		goto ZL1;
	}
	goto ZL0;
ZL1:;
	SAVE_LEXER (356);
	return;
ZL0:;
	*ZO873 = ZI873;
	*ZO874 = ZI874;
}

void
ZRselection_Hstatement(EXP *ZOe)
{
	EXP ZIe;

	switch (CURRENT_TERMINAL) {
	case 109:
		{
			unsigned ZIx;
			int ZIr;
			EXP ZIc;
			EXP ZIa;
			EXP ZIbs;
			EXP ZIb;
			EXP ZId;
			EXP ZIf;

			ADVANCE_LEXER;
			/* BEGINNING OF ACTION: condition_get */
			{
#line 851 "syntax.act"
 (ZIx) = crt_condition ; 
#line 3562 "syntax3.c"
			}
			/* END OF ACTION: condition_get */
			/* BEGINNING OF ACTION: reach_check */
			{
#line 835 "syntax.act"

    (ZIr) = unreached_code ;
    if ( (ZIr) ) {
	if ( !unreached_last ) {
	    report ( crt_loc, ERR_stmt_stmt_unreach () ) ;
	    unreached_last = 1 ;
	}
    } else {
	unreached_last = 0 ;
    }
#line 3578 "syntax3.c"
			}
			/* END OF ACTION: reach_check */
			ZRopen_Hround_Hx ();
			ZRexpression (&ZIc);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: stmt_if_begin */
			{
#line 739 "syntax.act"

    (ZIa) = begin_if_stmt ( (ZIc) ) ;
#line 3592 "syntax3.c"
			}
			/* END OF ACTION: stmt_if_begin */
			switch (CURRENT_TERMINAL) {
			case 39:
				break;
			default:
				goto ZL1;
			}
			ADVANCE_LEXER;
			/* BEGINNING OF ACTION: stmt_compound_begin */
			{
#line 659 "syntax.act"

    (ZIbs) = begin_compound_stmt ( 1 ) ;
#line 3607 "syntax3.c"
			}
			/* END OF ACTION: stmt_compound_begin */
			ZRscoped_Hstatement (ZIbs, &ZIb);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: reach_prev */
			{
#line 847 "syntax.act"
 unreached_prev = (ZIr) ; 
#line 3619 "syntax3.c"
			}
			/* END OF ACTION: reach_prev */
			/* BEGINNING OF ACTION: stmt_if_cont */
			{
#line 743 "syntax.act"

    (ZId) = cont_if_stmt ( (ZIa), (ZIb) ) ;
#line 3627 "syntax3.c"
			}
			/* END OF ACTION: stmt_if_cont */
			/* BEGINNING OF INLINE: 588 */
			{
				{
					EXP ZIfs;

					switch (CURRENT_TERMINAL) {
					case 103:
						break;
					default:
						goto ZL3;
					}
					ADVANCE_LEXER;
					/* BEGINNING OF ACTION: stmt_else */
					{
#line 751 "syntax.act"

    check_empty_stmt ( lex_else ) ;
#line 3647 "syntax3.c"
					}
					/* END OF ACTION: stmt_else */
					/* BEGINNING OF ACTION: stmt_compound_begin */
					{
#line 659 "syntax.act"

    (ZIfs) = begin_compound_stmt ( 1 ) ;
#line 3655 "syntax3.c"
					}
					/* END OF ACTION: stmt_compound_begin */
					ZRscoped_Hstatement (ZIfs, &ZIf);
					if ((CURRENT_TERMINAL) == 356) {
						RESTORE_LEXER;
						goto ZL3;
					}
				}
				goto ZL2;
			ZL3:;
				{
					/* BEGINNING OF ACTION: stmt_no_else */
					{
#line 755 "syntax.act"

    report ( crt_loc, ERR_stmt_if_no_else () ) ;
    (ZIf) = NULL_exp ;
#line 3673 "syntax3.c"
					}
					/* END OF ACTION: stmt_no_else */
				}
			ZL2:;
			}
			/* END OF INLINE: 588 */
			/* BEGINNING OF ACTION: reach_prev */
			{
#line 847 "syntax.act"
 unreached_prev = (ZIr) ; 
#line 3684 "syntax3.c"
			}
			/* END OF ACTION: reach_prev */
			/* BEGINNING OF ACTION: stmt_if_end */
			{
#line 747 "syntax.act"

    (ZIe) = end_if_stmt ( (ZId), (ZIf) ) ;
#line 3692 "syntax3.c"
			}
			/* END OF ACTION: stmt_if_end */
			/* BEGINNING OF ACTION: condition_set */
			{
#line 852 "syntax.act"
 crt_condition = (ZIx) ; 
#line 3699 "syntax3.c"
			}
			/* END OF ACTION: condition_set */
			/* BEGINNING OF ACTION: stmt_label_clear */
			{
#line 772 "syntax.act"

    unreached_fall = 1 ;
#line 3707 "syntax3.c"
			}
			/* END OF ACTION: stmt_label_clear */
		}
		break;
	case 119:
		{
			int ZIr;
			EXP ZIc;
			EXP ZIa;
			int ZIex;
			EXP ZIbs;
			EXP ZIb;

			ADVANCE_LEXER;
			/* BEGINNING OF ACTION: reach_check */
			{
#line 835 "syntax.act"

    (ZIr) = unreached_code ;
    if ( (ZIr) ) {
	if ( !unreached_last ) {
	    report ( crt_loc, ERR_stmt_stmt_unreach () ) ;
	    unreached_last = 1 ;
	}
    } else {
	unreached_last = 0 ;
    }
#line 3735 "syntax3.c"
			}
			/* END OF ACTION: reach_check */
			switch (CURRENT_TERMINAL) {
			case 65:
				break;
			default:
				goto ZL1;
			}
			ADVANCE_LEXER;
			ZRexpression (&ZIc);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: stmt_switch_begin */
			{
#line 788 "syntax.act"

    (ZIa) = begin_switch_stmt ( (ZIc) ) ;
#line 3755 "syntax3.c"
			}
			/* END OF ACTION: stmt_switch_begin */
			switch (CURRENT_TERMINAL) {
			case 39:
				break;
			default:
				goto ZL1;
			}
			ADVANCE_LEXER;
			/* BEGINNING OF INLINE: 591 */
			{
				switch (CURRENT_TERMINAL) {
				case 217:
					{
						ADVANCE_LEXER;
						/* BEGINNING OF ACTION: bool_true */
						{
#line 1411 "syntax.act"
 (ZIex) = 1 ; 
#line 3775 "syntax3.c"
						}
						/* END OF ACTION: bool_true */
					}
					break;
				default:
					{
						/* BEGINNING OF ACTION: bool_false */
						{
#line 1410 "syntax.act"
 (ZIex) = 0 ; 
#line 3786 "syntax3.c"
						}
						/* END OF ACTION: bool_false */
					}
					break;
				}
			}
			/* END OF INLINE: 591 */
			/* BEGINNING OF ACTION: stmt_compound_begin */
			{
#line 659 "syntax.act"

    (ZIbs) = begin_compound_stmt ( 1 ) ;
#line 3799 "syntax3.c"
			}
			/* END OF ACTION: stmt_compound_begin */
			ZRscoped_Hstatement (ZIbs, &ZIb);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: reach_prev */
			{
#line 847 "syntax.act"
 unreached_prev = (ZIr) ; 
#line 3811 "syntax3.c"
			}
			/* END OF ACTION: reach_prev */
			/* BEGINNING OF ACTION: stmt_switch_end */
			{
#line 792 "syntax.act"

    (ZIe) = end_switch_stmt ( (ZIa), (ZIb), (ZIex) ) ;
#line 3819 "syntax3.c"
			}
			/* END OF ACTION: stmt_switch_end */
			/* BEGINNING OF ACTION: stmt_label_clear */
			{
#line 772 "syntax.act"

    unreached_fall = 1 ;
#line 3827 "syntax3.c"
			}
			/* END OF ACTION: stmt_label_clear */
		}
		break;
	case 26:
		{
			ZRtarget_Hcondition (&ZIe);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: stmt_label_clear */
			{
#line 772 "syntax.act"

    unreached_fall = 1 ;
#line 3844 "syntax3.c"
			}
			/* END OF ACTION: stmt_label_clear */
		}
		break;
	case 356:
		return;
	default:
		goto ZL1;
	}
	goto ZL0;
ZL1:;
	SAVE_LEXER (356);
	return;
ZL0:;
	*ZOe = ZIe;
}

void
ZRexpression(EXP *ZOe)
{
	EXP ZIe;

	switch (CURRENT_TERMINAL) {
	case 1: case 17: case 18: case 19:
	case 20: case 21: case 22: case 23:
	case 33: case 43: case 59: case 61:
	case 62: case 65: case 69: case 71:
	case 78: case 85: case 116: case 170:
	case 206: case 210: case 255:
		{
			EXP ZI1028;

			ZRassignment_Hexpression (&ZI1028);
			ZR1030 (&ZI1028, &ZIe);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
		}
		break;
	case 298: case 324:
		{
			EXP ZIa;
			SID_LIST_EXP ZIq;
			SID_LIST_EXP ZIp;

			ZRflow_Hexpression (&ZIa);
			ZRcomma_Hexpression_Htail (&ZIq);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: list_exp_cons */
			{
#line 416 "syntax.act"

    CONS_exp ( (ZIa), (ZIq), (ZIp) ) ;
#line 3902 "syntax3.c"
			}
			/* END OF ACTION: list_exp_cons */
			/* BEGINNING OF ACTION: exp_comma */
			{
#line 466 "syntax.act"

    (ZIe) = make_comma_exp ( (ZIp) ) ;
#line 3910 "syntax3.c"
			}
			/* END OF ACTION: exp_comma */
		}
		break;
	case 347:
		{
			EXP ZIa;
			SID_LIST_EXP ZIq;
			SID_LIST_EXP ZIp;

			ZRinset_Hflow_Hexpression (&ZIa);
			ZRcomma_Hexpression_Htail (&ZIq);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: list_exp_cons */
			{
#line 416 "syntax.act"

    CONS_exp ( (ZIa), (ZIq), (ZIp) ) ;
#line 3932 "syntax3.c"
			}
			/* END OF ACTION: list_exp_cons */
			/* BEGINNING OF ACTION: exp_comma */
			{
#line 466 "syntax.act"

    (ZIe) = make_comma_exp ( (ZIp) ) ;
#line 3940 "syntax3.c"
			}
			/* END OF ACTION: exp_comma */
		}
		break;
	case 356:
		return;
	default:
		goto ZL1;
	}
	goto ZL0;
ZL1:;
	SAVE_LEXER (356);
	return;
ZL0:;
	*ZOe = ZIe;
}

void
ZRstatement_Hseq_Hopt(EXP ZIc, int ZId, EXP *ZOe)
{
	EXP ZIe;

	if ((CURRENT_TERMINAL) == 356) {
		return;
	}
ZL2_statement_Hseq_Hopt:;
	{
		EXP ZIa;
		EXP ZIb;

		/* BEGINNING OF ACTION: is_decl_statement */
		{
#line 1431 "syntax.act"

    /* Resolve declaration-statements from expression-statements */
    int b = predict_decl () ;
    if ( b ) {
	if ( !(ZId) ) report ( crt_loc, ERR_stmt_dcl_start () ) ;
	in_declaration++ ;
    }
    (ZI0) = b ;
#line 3982 "syntax3.c"
		}
		/* END OF ACTION: is_decl_statement */
		if (!ZI0)
			goto ZL3;
		ZRdeclaration ();
		if ((CURRENT_TERMINAL) == 356) {
			RESTORE_LEXER;
			goto ZL1;
		}
		/* BEGINNING OF ACTION: stmt_decl */
		{
#line 684 "syntax.act"

    in_declaration-- ;
    (ZIa) = NULL_exp ;
#line 3998 "syntax3.c"
		}
		/* END OF ACTION: stmt_decl */
		/* BEGINNING OF ACTION: stmt_label_clear */
		{
#line 772 "syntax.act"

    unreached_fall = 1 ;
#line 4006 "syntax3.c"
		}
		/* END OF ACTION: stmt_label_clear */
		/* BEGINNING OF ACTION: stmt_compound_add */
		{
#line 672 "syntax.act"

    (ZIb) = add_compound_stmt ( (ZIc), (ZIa) ) ;
#line 4014 "syntax3.c"
		}
		/* END OF ACTION: stmt_compound_add */
		/* BEGINNING OF INLINE: statement-seq-opt */
		ZIc = ZIb;
		goto ZL2_statement_Hseq_Hopt;
		/* END OF INLINE: statement-seq-opt */
	}
	UNREACHED;
ZL3:;
	switch (CURRENT_TERMINAL) {
	case 1: case 2: case 4: case 17:
	case 18: case 19: case 20: case 21:
	case 22: case 23: case 24: case 26:
	case 33: case 43: case 59: case 61:
	case 62: case 64: case 65: case 69:
	case 71: case 77: case 78: case 85:
	case 95: case 96: case 99: case 100:
	case 101: case 107: case 108: case 109:
	case 113: case 116: case 119: case 125:
	case 126: case 170: case 206: case 210:
	case 223: case 255: case 289: case 298:
	case 323: case 324: case 347:
		{
			EXP ZIa;
			EXP ZIb;
			int ZIdb;

			ZRstatement (&ZIa);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: stmt_compound_add */
			{
#line 672 "syntax.act"

    (ZIb) = add_compound_stmt ( (ZIc), (ZIa) ) ;
#line 4052 "syntax3.c"
			}
			/* END OF ACTION: stmt_compound_add */
			/* BEGINNING OF ACTION: bool_false */
			{
#line 1410 "syntax.act"
 (ZIdb) = 0 ; 
#line 4059 "syntax3.c"
			}
			/* END OF ACTION: bool_false */
			/* BEGINNING OF INLINE: statement-seq-opt */
			ZIc = ZIb;
			ZId = ZIdb;
			goto ZL2_statement_Hseq_Hopt;
			/* END OF INLINE: statement-seq-opt */
		}
		UNREACHED;
	default:
		{
			ZIe = ZIc;
		}
		break;
	}
	goto ZL0;
ZL1:;
	SAVE_LEXER (356);
	return;
ZL0:;
	*ZOe = ZIe;
}

void
ZRscoped_Hstmt_Hbody(EXP ZIc, EXP *ZOe)
{
	EXP ZIe;

	switch (CURRENT_TERMINAL) {
	case 64:
		{
			int ZId;

			/* BEGINNING OF INLINE: open-brace */
			{
				{
					switch (CURRENT_TERMINAL) {
					case 64:
						break;
					default:
						goto ZL1;
					}
					ADVANCE_LEXER;
				}
			}
			/* END OF INLINE: open-brace */
			/* BEGINNING OF ACTION: stmt_compound_block */
			{
#line 667 "syntax.act"

    COPY_int ( exp_sequence_block ( (ZIc) ), 2 ) ;
    (ZId) = 1 ;
#line 4112 "syntax3.c"
			}
			/* END OF ACTION: stmt_compound_block */
			ZRstatement_Hseq_Hopt (ZIc, ZId, &ZIe);
			/* BEGINNING OF INLINE: close-brace */
			{
				if ((CURRENT_TERMINAL) == 356) {
					RESTORE_LEXER;
					goto ZL1;
				}
				{
					switch (CURRENT_TERMINAL) {
					case 38:
						break;
					default:
						goto ZL1;
					}
					ADVANCE_LEXER;
				}
			}
			/* END OF INLINE: close-brace */
		}
		break;
	case 1: case 2: case 4: case 17:
	case 18: case 19: case 20: case 21:
	case 22: case 23: case 24: case 26:
	case 33: case 43: case 59: case 61:
	case 62: case 65: case 69: case 71:
	case 77: case 78: case 85: case 95:
	case 96: case 99: case 100: case 101:
	case 107: case 108: case 109: case 113:
	case 116: case 119: case 125: case 126:
	case 170: case 206: case 210: case 223:
	case 255: case 289: case 298: case 323:
	case 324: case 347:
		{
			EXP ZIa;

			ZRsimple_Hstatement (&ZIa);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: stmt_compound_add */
			{
#line 672 "syntax.act"

    (ZIe) = add_compound_stmt ( (ZIc), (ZIa) ) ;
#line 4160 "syntax3.c"
			}
			/* END OF ACTION: stmt_compound_add */
		}
		break;
	case 356:
		return;
	default:
		goto ZL1;
	}
	goto ZL0;
ZL1:;
	SAVE_LEXER (356);
	return;
ZL0:;
	*ZOe = ZIe;
}

void
ZRstatement(EXP *ZOe)
{
	EXP ZIe;

	switch (CURRENT_TERMINAL) {
	case 64:
		{
			ZRcompound_Hstatement (&ZIe);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
		}
		break;
	case 1: case 2: case 4: case 17:
	case 18: case 19: case 20: case 21:
	case 22: case 23: case 24: case 26:
	case 33: case 43: case 59: case 61:
	case 62: case 65: case 69: case 71:
	case 77: case 78: case 85: case 95:
	case 96: case 99: case 100: case 101:
	case 107: case 108: case 109: case 113:
	case 116: case 119: case 125: case 126:
	case 170: case 206: case 210: case 223:
	case 255: case 289: case 298: case 323:
	case 324: case 347:
		{
			ZRsimple_Hstatement (&ZIe);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
		}
		break;
	case 356:
		return;
	default:
		goto ZL1;
	}
	goto ZL0;
ZL1:;
	SAVE_LEXER (356);
	return;
ZL0:;
	*ZOe = ZIe;
}

void
ZR729(BASE_TYPE *ZIb1, TYPE *ZIt1, CV_SPEC *ZIcv1, DECL_SPEC *ZIds1, BASE_TYPE *ZObt, TYPE *ZOt, CV_SPEC *ZOcv, DECL_SPEC *ZOds)
{
	BASE_TYPE ZIbt;
	TYPE ZIt;
	CV_SPEC ZIcv;
	DECL_SPEC ZIds;

	if ((CURRENT_TERMINAL) == 356) {
		return;
	}
	{
		BASE_TYPE ZI1104;
		TYPE ZI1105;
		CV_SPEC ZI1106;
		DECL_SPEC ZI1107;
		BASE_TYPE ZIb2;
		TYPE ZIt2;
		CV_SPEC ZIcv2;
		DECL_SPEC ZIds2;

		/* BEGINNING OF ACTION: is_decl_specifier */
		{
#line 1426 "syntax.act"

    /* Resolve declaration-specifiers from other declarators */
    (ZI0) = predict_dspec ( 0 ) ;
#line 4253 "syntax3.c"
		}
		/* END OF ACTION: is_decl_specifier */
		if (!ZI0)
			goto ZL2;
		ZRdecl_Hspecifier (&ZI1104, &ZI1105, &ZI1106, &ZI1107);
		if ((CURRENT_TERMINAL) == 356) {
			RESTORE_LEXER;
			goto ZL1;
		}
		/* BEGINNING OF ACTION: check_decl_specifier */
		{
#line 1474 "syntax.act"

    /* A type-name can be a declarator-id */
    if ( have_type_specifier && crt_lex_token == lex_type_Hname ) {
	crt_lex_token = lex_identifier ;
    }
#line 4271 "syntax3.c"
		}
		/* END OF ACTION: check_decl_specifier */
		ZR729 (&ZI1104, &ZI1105, &ZI1106, &ZI1107, &ZIb2, &ZIt2, &ZIcv2, &ZIds2);
		if ((CURRENT_TERMINAL) == 356) {
			RESTORE_LEXER;
			goto ZL1;
		}
		/* BEGINNING OF ACTION: btype_join */
		{
#line 935 "syntax.act"

    if ( (*ZIb1) & (ZIb2) ) {
	(ZIbt) = join_pre_types ( (*ZIb1), (ZIb2) ) ;
    } else {
	(ZIbt) = ( (*ZIb1) | (ZIb2) ) ;
    }
#line 4288 "syntax3.c"
		}
		/* END OF ACTION: btype_join */
		/* BEGINNING OF ACTION: type_join */
		{
#line 980 "syntax.act"

    /* Join two partial types */
    if ( IS_NULL_type ( (*ZIt1) ) ) {
	(ZIt) = (ZIt2) ;
    } else if ( IS_NULL_type ( (ZIt2) ) ) {
	(ZIt) = (*ZIt1) ;
    } else {
	report ( crt_loc, ERR_dcl_type_simple_many ( (*ZIt1), (ZIt2) ) ) ;
	(ZIt) = (ZIt2) ;
    }
#line 4304 "syntax3.c"
		}
		/* END OF ACTION: type_join */
		/* BEGINNING OF ACTION: cv_join */
		{
#line 906 "syntax.act"

    CV_SPEC c = ( (*ZIcv1) & (ZIcv2) ) ;
    if ( c ) report ( crt_loc, ERR_dcl_type_cv_dup ( c ) ) ;
    (ZIcv) = ( (*ZIcv1) | (ZIcv2) ) ;
#line 4314 "syntax3.c"
		}
		/* END OF ACTION: cv_join */
		/* BEGINNING OF ACTION: dspec_join */
		{
#line 1129 "syntax.act"

    /* Combine two declaration specifiers */
    DECL_SPEC d = ( ( (*ZIds1) & (ZIds2) ) & dspec_duplicate ) ;
    if ( d ) report ( crt_loc, ERR_dcl_spec_dup ( d ) ) ;
    (ZIds) = ( (*ZIds1) | (ZIds2) ) ;
#line 4325 "syntax3.c"
		}
		/* END OF ACTION: dspec_join */
		goto ZL0;
	}
	UNREACHED;
ZL2:;
	switch (CURRENT_TERMINAL) {
	default:
		{
			ZIbt = *ZIb1;
			ZIt = *ZIt1;
			ZIcv = *ZIcv1;
			ZIds = *ZIds1;
		}
		break;
	}
	goto ZL0;
ZL1:;
	SAVE_LEXER (356);
	return;
ZL0:;
	*ZObt = ZIbt;
	*ZOt = ZIt;
	*ZOcv = ZIcv;
	*ZOds = ZIds;
}

void
ZRdirect_Habstract_Hdeclarator(TYPE *ZO866)
{
	TYPE ZI866;

	switch (CURRENT_TERMINAL) {
	case 65:
		{
			ADVANCE_LEXER;
			ZR1063 (&ZI866);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
		}
		break;
	case 333:
		{
			IDENTIFIER ZIid;
			int ZIell;
			TYPE ZIt;

			ADVANCE_LEXER;
			switch (CURRENT_TERMINAL) {
			case 65:
				break;
			default:
				goto ZL1;
			}
			ADVANCE_LEXER;
			/* BEGINNING OF ACTION: id_none */
			{
#line 382 "syntax.act"

    (ZIid) = NULL_id ;
#line 4388 "syntax3.c"
			}
			/* END OF ACTION: id_none */
			/* BEGINNING OF ACTION: param_begin */
			{
#line 879 "syntax.act"

    func_type_defn ( 0 ) ;
    begin_param ( (ZIid) ) ;
    have_type_declaration = TYPE_DECL_NONE ;
    have_func_declarator = 0 ;
#line 4399 "syntax3.c"
			}
			/* END OF ACTION: param_begin */
			ZR755 (&ZIell);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: type_func_weak */
			{
#line 1024 "syntax.act"

    (ZIt) = make_func_type ( NULL_type, ( (ZIell) | FUNC_WEAK ), cv_c, empty_type_set ) ;
#line 4412 "syntax3.c"
			}
			/* END OF ACTION: type_func_weak */
			switch (CURRENT_TERMINAL) {
			case 39:
				break;
			default:
				goto ZL1;
			}
			ADVANCE_LEXER;
			/* BEGINNING OF ACTION: param_end */
			{
#line 886 "syntax.act"

    end_param () ;
    have_type_declaration = TYPE_DECL_NONE ;
    have_func_declarator = 1 ;
#line 4429 "syntax3.c"
			}
			/* END OF ACTION: param_end */
			ZR867 (ZIt, &ZI866);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
		}
		break;
	case 66:
		{
			EXP ZIe;
			TYPE ZIt;

			/* BEGINNING OF INLINE: open-square */
			{
				{
					switch (CURRENT_TERMINAL) {
					case 66:
						break;
					default:
						goto ZL1;
					}
					ADVANCE_LEXER;
				}
			}
			/* END OF INLINE: open-square */
			ZR754 (&ZIe);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: type_array */
			{
#line 1036 "syntax.act"

    NAT n = make_array_dim ( (ZIe) ) ;
    MAKE_type_array ( cv_none, NULL_type, n, (ZIt) ) ;
#line 4468 "syntax3.c"
			}
			/* END OF ACTION: type_array */
			/* BEGINNING OF INLINE: close-square */
			{
				{
					switch (CURRENT_TERMINAL) {
					case 40:
						break;
					default:
						goto ZL1;
					}
					ADVANCE_LEXER;
				}
			}
			/* END OF INLINE: close-square */
			ZR867 (ZIt, &ZI866);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
		}
		break;
	case 356:
		return;
	default:
		goto ZL1;
	}
	goto ZL0;
ZL1:;
	SAVE_LEXER (356);
	return;
ZL0:;
	*ZO866 = ZI866;
}

void
ZRiteration_Hstatement(EXP *ZOe)
{
	EXP ZIe;

	switch (CURRENT_TERMINAL) {
	case 101:
		{
			unsigned ZIx;
			int ZIr;
			EXP ZIa;
			EXP ZIbs;
			EXP ZIb;
			EXP ZIc0;
			EXP ZIc;

			ADVANCE_LEXER;
			/* BEGINNING OF ACTION: condition_get */
			{
#line 851 "syntax.act"
 (ZIx) = crt_condition ; 
#line 4525 "syntax3.c"
			}
			/* END OF ACTION: condition_get */
			/* BEGINNING OF ACTION: reach_check */
			{
#line 835 "syntax.act"

    (ZIr) = unreached_code ;
    if ( (ZIr) ) {
	if ( !unreached_last ) {
	    report ( crt_loc, ERR_stmt_stmt_unreach () ) ;
	    unreached_last = 1 ;
	}
    } else {
	unreached_last = 0 ;
    }
#line 4541 "syntax3.c"
			}
			/* END OF ACTION: reach_check */
			/* BEGINNING OF ACTION: stmt_do_begin */
			{
#line 697 "syntax.act"

    (ZIa) = begin_do_stmt () ;
#line 4549 "syntax3.c"
			}
			/* END OF ACTION: stmt_do_begin */
			/* BEGINNING OF ACTION: stmt_compound_begin */
			{
#line 659 "syntax.act"

    (ZIbs) = begin_compound_stmt ( 1 ) ;
#line 4557 "syntax3.c"
			}
			/* END OF ACTION: stmt_compound_begin */
			ZRscoped_Hstatement (ZIbs, &ZIb);
			switch (CURRENT_TERMINAL) {
			case 125:
				break;
			case 356:
				RESTORE_LEXER;
				goto ZL1;
			default:
				goto ZL1;
			}
			ADVANCE_LEXER;
			switch (CURRENT_TERMINAL) {
			case 65:
				break;
			default:
				goto ZL1;
			}
			ADVANCE_LEXER;
			ZRexpression (&ZIc0);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: bind_temporary */
			{
#line 824 "syntax.act"

    (ZIc) = bind_temporary ( (ZIc0) ) ;
#line 4588 "syntax3.c"
			}
			/* END OF ACTION: bind_temporary */
			/* BEGINNING OF ACTION: reach_prev */
			{
#line 847 "syntax.act"
 unreached_prev = (ZIr) ; 
#line 4595 "syntax3.c"
			}
			/* END OF ACTION: reach_prev */
			/* BEGINNING OF ACTION: stmt_do_end */
			{
#line 701 "syntax.act"

    (ZIe) = end_do_stmt ( (ZIa), (ZIb), (ZIc) ) ;
#line 4603 "syntax3.c"
			}
			/* END OF ACTION: stmt_do_end */
			switch (CURRENT_TERMINAL) {
			case 39:
				break;
			default:
				goto ZL1;
			}
			ADVANCE_LEXER;
			/* BEGINNING OF ACTION: condition_set */
			{
#line 852 "syntax.act"
 crt_condition = (ZIx) ; 
#line 4617 "syntax3.c"
			}
			/* END OF ACTION: condition_set */
			/* BEGINNING OF ACTION: stmt_label_clear */
			{
#line 772 "syntax.act"

    unreached_fall = 1 ;
#line 4625 "syntax3.c"
			}
			/* END OF ACTION: stmt_label_clear */
			ZRsemicolon_Hx ();
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
		}
		break;
	case 107:
		{
			unsigned ZIx;
			int ZIr;
			EXP ZIf;
			EXP ZIa;
			EXP ZIg;
			EXP ZIc;
			EXP ZIds;
			EXP ZIb;
			EXP ZIh;
			EXP ZId;

			ADVANCE_LEXER;
			/* BEGINNING OF ACTION: condition_get */
			{
#line 851 "syntax.act"
 (ZIx) = crt_condition ; 
#line 4653 "syntax3.c"
			}
			/* END OF ACTION: condition_get */
			/* BEGINNING OF ACTION: reach_check */
			{
#line 835 "syntax.act"

    (ZIr) = unreached_code ;
    if ( (ZIr) ) {
	if ( !unreached_last ) {
	    report ( crt_loc, ERR_stmt_stmt_unreach () ) ;
	    unreached_last = 1 ;
	}
    } else {
	unreached_last = 0 ;
    }
#line 4669 "syntax3.c"
			}
			/* END OF ACTION: reach_check */
			switch (CURRENT_TERMINAL) {
			case 65:
				break;
			default:
				goto ZL1;
			}
			ADVANCE_LEXER;
			/* BEGINNING OF ACTION: stmt_for_begin */
			{
#line 709 "syntax.act"

    (ZIf) = begin_for_stmt () ;
#line 4684 "syntax3.c"
			}
			/* END OF ACTION: stmt_for_begin */
			ZRfor_Hinit_Hstatement (&ZIa);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: stmt_for_init */
			{
#line 713 "syntax.act"

    (ZIg) = init_for_stmt ( (ZIf), &(ZIa) ) ;
#line 4697 "syntax3.c"
			}
			/* END OF ACTION: stmt_for_init */
			ZRfor_Hcond_Hstatement (&ZIc);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: stmt_compound_begin */
			{
#line 659 "syntax.act"

    (ZIds) = begin_compound_stmt ( 1 ) ;
#line 4710 "syntax3.c"
			}
			/* END OF ACTION: stmt_compound_begin */
			ZRfor_Hend_Hstatement (&ZIb);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: stmt_for_cond */
			{
#line 717 "syntax.act"

    (ZIh) = cond_for_stmt ( (ZIg), (ZIc), (ZIb) ) ;
#line 4723 "syntax3.c"
			}
			/* END OF ACTION: stmt_for_cond */
			switch (CURRENT_TERMINAL) {
			case 39:
				break;
			default:
				goto ZL1;
			}
			ADVANCE_LEXER;
			/* BEGINNING OF ACTION: stmt_compound_mark */
			{
#line 663 "syntax.act"

    mark_compound_stmt ( (ZIds) ) ;
#line 4738 "syntax3.c"
			}
			/* END OF ACTION: stmt_compound_mark */
			ZRscoped_Hstatement (ZIds, &ZId);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: reach_prev */
			{
#line 847 "syntax.act"
 unreached_prev = (ZIr) ; 
#line 4750 "syntax3.c"
			}
			/* END OF ACTION: reach_prev */
			/* BEGINNING OF ACTION: stmt_for_end */
			{
#line 721 "syntax.act"

    (ZIe) = end_for_stmt ( (ZIh), (ZId) ) ;
#line 4758 "syntax3.c"
			}
			/* END OF ACTION: stmt_for_end */
			/* BEGINNING OF ACTION: condition_set */
			{
#line 852 "syntax.act"
 crt_condition = (ZIx) ; 
#line 4765 "syntax3.c"
			}
			/* END OF ACTION: condition_set */
			/* BEGINNING OF ACTION: stmt_label_clear */
			{
#line 772 "syntax.act"

    unreached_fall = 1 ;
#line 4773 "syntax3.c"
			}
			/* END OF ACTION: stmt_label_clear */
			/* BEGINNING OF ACTION: rescan_token */
			{
#line 1470 "syntax.act"

    RESCAN_LEXER ;
#line 4781 "syntax3.c"
			}
			/* END OF ACTION: rescan_token */
		}
		break;
	case 125:
		{
			unsigned ZIx;
			int ZIr;
			EXP ZIc0;
			EXP ZIc;
			EXP ZIa;
			EXP ZIbs;
			EXP ZIb;

			ADVANCE_LEXER;
			/* BEGINNING OF ACTION: condition_get */
			{
#line 851 "syntax.act"
 (ZIx) = crt_condition ; 
#line 4801 "syntax3.c"
			}
			/* END OF ACTION: condition_get */
			/* BEGINNING OF ACTION: reach_check */
			{
#line 835 "syntax.act"

    (ZIr) = unreached_code ;
    if ( (ZIr) ) {
	if ( !unreached_last ) {
	    report ( crt_loc, ERR_stmt_stmt_unreach () ) ;
	    unreached_last = 1 ;
	}
    } else {
	unreached_last = 0 ;
    }
#line 4817 "syntax3.c"
			}
			/* END OF ACTION: reach_check */
			switch (CURRENT_TERMINAL) {
			case 65:
				break;
			default:
				goto ZL1;
			}
			ADVANCE_LEXER;
			ZRexpression (&ZIc0);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: bind_temporary */
			{
#line 824 "syntax.act"

    (ZIc) = bind_temporary ( (ZIc0) ) ;
#line 4837 "syntax3.c"
			}
			/* END OF ACTION: bind_temporary */
			/* BEGINNING OF ACTION: stmt_while_begin */
			{
#line 796 "syntax.act"

    (ZIa) = begin_while_stmt ( (ZIc) ) ;
#line 4845 "syntax3.c"
			}
			/* END OF ACTION: stmt_while_begin */
			switch (CURRENT_TERMINAL) {
			case 39:
				break;
			default:
				goto ZL1;
			}
			ADVANCE_LEXER;
			/* BEGINNING OF ACTION: stmt_compound_begin */
			{
#line 659 "syntax.act"

    (ZIbs) = begin_compound_stmt ( 1 ) ;
#line 4860 "syntax3.c"
			}
			/* END OF ACTION: stmt_compound_begin */
			ZRscoped_Hstatement (ZIbs, &ZIb);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: reach_prev */
			{
#line 847 "syntax.act"
 unreached_prev = (ZIr) ; 
#line 4872 "syntax3.c"
			}
			/* END OF ACTION: reach_prev */
			/* BEGINNING OF ACTION: stmt_while_end */
			{
#line 800 "syntax.act"

    (ZIe) = end_while_stmt ( (ZIa), (ZIb) ) ;
#line 4880 "syntax3.c"
			}
			/* END OF ACTION: stmt_while_end */
			/* BEGINNING OF ACTION: condition_set */
			{
#line 852 "syntax.act"
 crt_condition = (ZIx) ; 
#line 4887 "syntax3.c"
			}
			/* END OF ACTION: condition_set */
			/* BEGINNING OF ACTION: stmt_label_clear */
			{
#line 772 "syntax.act"

    unreached_fall = 1 ;
#line 4895 "syntax3.c"
			}
			/* END OF ACTION: stmt_label_clear */
		}
		break;
	case 356:
		return;
	default:
		goto ZL1;
	}
	goto ZL0;
ZL1:;
	SAVE_LEXER (356);
	return;
ZL0:;
	*ZOe = ZIe;
}

void
ZRassignment_Hoperator(void)
{
	switch (CURRENT_TERMINAL) {
	case 45:
		{
			ADVANCE_LEXER;
		}
		break;
	case 58:
		{
			ADVANCE_LEXER;
		}
		break;
	case 60:
		{
			ADVANCE_LEXER;
		}
		break;
	case 70:
		{
			ADVANCE_LEXER;
		}
		break;
	case 74:
		{
			ADVANCE_LEXER;
		}
		break;
	case 76:
		{
			ADVANCE_LEXER;
		}
		break;
	case 79:
		{
			ADVANCE_LEXER;
		}
		break;
	case 34:
		{
			/* BEGINNING OF INLINE: and-eq */
			{
				{
					switch (CURRENT_TERMINAL) {
					case 34:
						break;
					default:
						goto ZL1;
					}
					ADVANCE_LEXER;
				}
			}
			/* END OF INLINE: and-eq */
		}
		break;
	case 68:
		{
			/* BEGINNING OF INLINE: or-eq */
			{
				{
					switch (CURRENT_TERMINAL) {
					case 68:
						break;
					default:
						goto ZL1;
					}
					ADVANCE_LEXER;
				}
			}
			/* END OF INLINE: or-eq */
		}
		break;
	case 81:
		{
			/* BEGINNING OF INLINE: xor-eq */
			{
				{
					switch (CURRENT_TERMINAL) {
					case 81:
						break;
					default:
						goto ZL1;
					}
					ADVANCE_LEXER;
				}
			}
			/* END OF INLINE: xor-eq */
		}
		break;
	case 356:
		return;
	default:
		goto ZL1;
	}
	return;
ZL1:;
	SAVE_LEXER (356);
	return;
}

/* END OF FILE */
