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
ZRrelational_Hexpression(EXP *ZO910)
{
	EXP ZI910;

	if ((CURRENT_TERMINAL) == 356) {
		return;
	}
	{
		EXP ZIe;

		ZRshift_Hexpression (&ZIe);
		ZR911 (ZIe, &ZI910);
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
	*ZO910 = ZI910;
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
#line 709 "syntax.act"

    (ZIb) = make_exp_stmt ( (ZIa) ) ;
#line 151 "syntax3.c"
			}
			/* END OF ACTION: stmt_exp */
			/* BEGINNING OF ACTION: bind_temporary */
			{
#line 828 "syntax.act"

    (ZIe) = bind_temporary ( (ZIb) ) ;
#line 159 "syntax3.c"
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
#line 171 "syntax3.c"
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
ZRand_Hexpression(EXP *ZO898)
{
	EXP ZI898;

	if ((CURRENT_TERMINAL) == 356) {
		return;
	}
	{
		EXP ZIe;

		ZRmaxmin_Hexpression (&ZIe);
		ZR899 (ZIe, &ZI898);
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
	*ZO898 = ZI898;
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
#line 1353 "syntax.act"

    /* Syntax errors */
    ERROR err = ERR_lex_parse ( crt_token ) ;
    report ( crt_loc, err ) ;
    have_syntax_error = 1 ;
#line 239 "syntax3.c"
		}
		/* END OF ACTION: error_syntax */
		/* BEGINNING OF ACTION: exp_none */
		{
#line 431 "syntax.act"

    (ZIe) = NULL_exp ;
#line 247 "syntax3.c"
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
#line 937 "syntax.act"
 (ZIbt) = btype_none ; 
#line 274 "syntax3.c"
			}
			/* END OF ACTION: btype_none */
			/* BEGINNING OF ACTION: type_none */
			{
#line 960 "syntax.act"

    (ZIt) = NULL_type ;
#line 282 "syntax3.c"
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
#line 906 "syntax.act"
 (ZIcv) = cv_none ; 
#line 302 "syntax3.c"
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
ZR1077(IDENTIFIER *ZId, SID_LIST_EXP *ZOq)
{
	SID_LIST_EXP ZIq;

	switch (CURRENT_TERMINAL) {
	case 1: case 17: case 18: case 19:
	case 20: case 21: case 22: case 23:
	case 33: case 43: case 46: case 59:
	case 61: case 62: case 64: case 65:
	case 69: case 71: case 78: case 85:
	case 116: case 170: case 206: case 210:
	case 255:
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
#line 350 "syntax3.c"
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
ZR950(BASE_TYPE *ZI946, TYPE *ZI947, CV_SPEC *ZI948, DECL_SPEC *ZI949, EXP *ZOe)
{
	EXP ZIe;

	switch (CURRENT_TERMINAL) {
	case 77:
		{
			/* BEGINNING OF ACTION: declare_id_empty */
			{
#line 1183 "syntax.act"

    IGNORE empty_decl ( (*ZI949), NULL_type, (*ZI946), (*ZI947), (*ZI948), last_lex_token, 0 ) ;
    have_type_declaration = TYPE_DECL_NONE ;
    have_func_declarator = 0 ;
    have_type_specifier = 0 ;
#line 382 "syntax3.c"
			}
			/* END OF ACTION: declare_id_empty */
			ADVANCE_LEXER;
			/* BEGINNING OF ACTION: exp_none */
			{
#line 431 "syntax.act"

    (ZIe) = NULL_exp ;
#line 391 "syntax3.c"
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
#line 1144 "syntax.act"

    /* Complete a declaration specifier and a type */
    (ZIds) = complete_dspec ( (*ZI949), (*ZI946), (*ZI947), (*ZI948) ) ;
    (ZIt) = complete_pre_type ( (*ZI946), (*ZI947), (*ZI948), 0 ) ;
    have_type_specifier = 0 ;
#line 412 "syntax3.c"
			}
			/* END OF ACTION: dspec_complete */
			ZRdeclarator (ZIt, &ZIs, &ZIid);
			ZR824 (ZI946, &ZIt, &ZIds, &ZIs, &ZIid);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: exp_none */
			{
#line 431 "syntax.act"

    (ZIe) = NULL_exp ;
#line 426 "syntax3.c"
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
ZR824(BASE_TYPE *ZIbt, TYPE *ZIt, DECL_SPEC *ZIds, TYPE *ZIs, IDENTIFIER *ZIid)
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
#line 1426 "syntax.act"

    (ZI0) = function_params ( (*ZIs) ) ;
#line 460 "syntax3.c"
		}
		/* END OF ACTION: is_function */
		if (!ZI0)
			goto ZL2;
		/* BEGINNING OF ACTION: define_func */
		{
#line 1177 "syntax.act"

    (ZId) = make_func_decl ( (*ZIds), (*ZIs), (*ZIid), 1 ) ;
    have_type_declaration = TYPE_DECL_NONE ;
    have_func_declarator = 0 ;
#line 472 "syntax3.c"
		}
		/* END OF ACTION: define_func */
		/* BEGINNING OF ACTION: function_begin */
		{
#line 868 "syntax.act"

    (ZIb) = in_class_defn ;
    in_class_defn = 0 ;
    in_function_defn++ ;
    really_in_function_defn++ ;
    begin_function ( (ZId) ) ;
#line 484 "syntax3.c"
		}
		/* END OF ACTION: function_begin */
		ZRfunction_Hbody (&ZIa);
		if ((CURRENT_TERMINAL) == 356) {
			RESTORE_LEXER;
			goto ZL1;
		}
		/* BEGINNING OF ACTION: function_end */
		{
#line 876 "syntax.act"

    IGNORE end_function ( (ZId), (ZIa) ) ;
    in_class_defn = (ZIb) ;
    in_function_defn-- ;
    really_in_function_defn-- ;
#line 500 "syntax3.c"
		}
		/* END OF ACTION: function_end */
		/* BEGINNING OF ACTION: rescan_token */
		{
#line 1474 "syntax.act"

    RESCAN_LEXER ;
#line 508 "syntax3.c"
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
#line 1158 "syntax.act"

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
#line 541 "syntax3.c"
			}
			/* END OF ACTION: declare_id */
			ZRinitialiser_Hopt (ZId, &ZIa);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: initialise_id */
			{
#line 1264 "syntax.act"

    int def = init_object ( (ZId), (ZIa) ) ;
    if ( do_dump ) dump_declare ( (ZId), &decl_loc, def ) ;
#line 555 "syntax3.c"
			}
			/* END OF ACTION: initialise_id */
			/* BEGINNING OF INLINE: 825 */
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
			/* END OF INLINE: 825 */
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
ZR1080(EXP *ZI1078, EXP *ZOe)
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

    CONS_exp ( (*ZI1078), (ZIq), (ZIp) ) ;
#line 622 "syntax3.c"
			}
			/* END OF ACTION: list_exp_cons */
			/* BEGINNING OF ACTION: exp_comma */
			{
#line 466 "syntax.act"

    (ZIe) = make_comma_exp ( (ZIp) ) ;
#line 630 "syntax3.c"
			}
			/* END OF ACTION: exp_comma */
		}
		break;
	default:
		{
			ZIe = *ZI1078;
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
#line 1353 "syntax.act"

    /* Syntax errors */
    ERROR err = ERR_lex_parse ( crt_token ) ;
    report ( crt_loc, err ) ;
    have_syntax_error = 1 ;
#line 677 "syntax3.c"
		}
		/* END OF ACTION: error_syntax */
		/* BEGINNING OF ACTION: exp_none */
		{
#line 431 "syntax.act"

    (ZIe) = NULL_exp ;
#line 685 "syntax3.c"
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
		/* BEGINNING OF INLINE: 831 */
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
		/* END OF INLINE: 831 */
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
#line 1386 "syntax.act"

    (ZIn1) = no_type_defns ;
#line 749 "syntax3.c"
		}
		/* END OF ACTION: no_type_defns */
		ZRtype_Hspecifier_Hseq (&ZIbt, &ZIp, &ZIcv);
		if ((CURRENT_TERMINAL) == 356) {
			RESTORE_LEXER;
			goto ZL1;
		}
		/* BEGINNING OF ACTION: type_complete */
		{
#line 996 "syntax.act"

    (ZIq) = complete_pre_type ( (ZIbt), (ZIp), (ZIcv), 1 ) ;
    have_type_specifier = 0 ;
#line 763 "syntax3.c"
		}
		/* END OF ACTION: type_complete */
		ZRabstract_Hdeclarator_Hopt (ZIq, &ZIt);
		if ((CURRENT_TERMINAL) == 356) {
			RESTORE_LEXER;
			goto ZL1;
		}
		/* BEGINNING OF ACTION: diff_type_defns */
		{
#line 1394 "syntax.act"

    (ZIn) = no_type_defns - (ZIn1) ;
#line 776 "syntax3.c"
		}
		/* END OF ACTION: diff_type_defns */
		/* BEGINNING OF ACTION: type_check */
		{
#line 1005 "syntax.act"

    object_type ( (ZIt), null_tag ) ;
#line 784 "syntax3.c"
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
#line 618 "syntax.act"

    (ZIe) = make_set_exp ( (ZIa) ) ;
#line 825 "syntax3.c"
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
#line 630 "syntax.act"

    (ZIe) = make_unused_exp ( (ZIa) ) ;
#line 859 "syntax3.c"
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
ZR955(EXP *ZI953, EXP *ZOe)
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

    (ZIe) = make_cond_exp ( (*ZI953), (ZIa), (ZIb) ) ;
#line 917 "syntax3.c"
			}
			/* END OF ACTION: exp_cond */
		}
		break;
	default:
		{
			ZIe = *ZI953;
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
#line 1125 "syntax.act"
 (ZIds) = dspec_none ; 
#line 965 "syntax3.c"
		}
		/* END OF ACTION: dspec_none */
		/* BEGINNING OF ACTION: type_none */
		{
#line 960 "syntax.act"

    (ZIt) = NULL_type ;
#line 973 "syntax3.c"
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
ZR1083(EXP *ZOe)
{
	EXP ZIe;

	switch (CURRENT_TERMINAL) {
	case 298:
		{
			EXP ZI1096;
			EXP ZI1097;

			ADVANCE_LEXER;
			ZRexpression (&ZI1096);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: exp_set */
			{
#line 618 "syntax.act"

    (ZI1097) = make_set_exp ( (ZI1096) ) ;
#line 1023 "syntax3.c"
			}
			/* END OF ACTION: exp_set */
			ZR1098 (&ZI1097, &ZIe);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
		}
		break;
	case 324:
		{
			EXP ZI1093;
			EXP ZI1094;

			ADVANCE_LEXER;
			ZRexpression (&ZI1093);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: exp_unused */
			{
#line 630 "syntax.act"

    (ZI1094) = make_unused_exp ( (ZI1093) ) ;
#line 1049 "syntax3.c"
			}
			/* END OF ACTION: exp_unused */
			ZR1098 (&ZI1094, &ZIe);
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
ZR1084(EXP *ZOe)
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
#line 655 "syntax.act"

    (ZIa) = begin_case_stmt ( (ZIc), 0 ) ;
#line 1095 "syntax3.c"
			}
			/* END OF ACTION: stmt_case_begin */
			/* BEGINNING OF ACTION: stmt_label_set */
			{
#line 772 "syntax.act"

    unreached_fall = 0 ;
#line 1103 "syntax3.c"
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
#line 659 "syntax.act"

    (ZIe) = end_case_stmt ( (ZIa), (ZIb) ) ;
#line 1123 "syntax3.c"
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
#line 693 "syntax.act"

    (ZIa) = begin_default_stmt ( 0 ) ;
#line 1139 "syntax3.c"
			}
			/* END OF ACTION: stmt_default_begin */
			/* BEGINNING OF ACTION: stmt_label_set */
			{
#line 772 "syntax.act"

    unreached_fall = 0 ;
#line 1147 "syntax3.c"
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
#line 697 "syntax.act"

    (ZIe) = end_default_stmt ( (ZIa), (ZIb) ) ;
#line 1167 "syntax3.c"
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
ZR703(BASE_TYPE *ZIb1, TYPE *ZIt1, CV_SPEC *ZIcv1, BASE_TYPE *ZObt, TYPE *ZOt, CV_SPEC *ZOcv)
{
	BASE_TYPE ZIbt;
	TYPE ZIt;
	CV_SPEC ZIcv;

	if ((CURRENT_TERMINAL) == 356) {
		return;
	}
	{
		BASE_TYPE ZI1033;
		TYPE ZI1034;
		CV_SPEC ZI1035;
		BASE_TYPE ZIb2;
		TYPE ZIt2;
		CV_SPEC ZIcv2;

		/* BEGINNING OF ACTION: is_type_specifier */
		{
#line 1455 "syntax.act"

    /* Resolve type-specifiers from other declarators */
    (ZI0) = predict_tspec ( 0 ) ;
#line 1209 "syntax3.c"
		}
		/* END OF ACTION: is_type_specifier */
		if (!ZI0)
			goto ZL2;
		ZRtype_Hqualifier (&ZI1033, &ZI1034, &ZI1035);
		if ((CURRENT_TERMINAL) == 356) {
			RESTORE_LEXER;
			goto ZL1;
		}
		/* BEGINNING OF ACTION: check_decl_specifier */
		{
#line 1478 "syntax.act"

    /* A type-name can be a declarator-id */
    if ( have_type_specifier && crt_lex_token == lex_type_Hname ) {
	crt_lex_token = lex_identifier ;
    }
#line 1227 "syntax3.c"
		}
		/* END OF ACTION: check_decl_specifier */
		ZR703 (&ZI1033, &ZI1034, &ZI1035, &ZIb2, &ZIt2, &ZIcv2);
		if ((CURRENT_TERMINAL) == 356) {
			RESTORE_LEXER;
			goto ZL1;
		}
		/* BEGINNING OF ACTION: btype_join */
		{
#line 939 "syntax.act"

    if ( (*ZIb1) & (ZIb2) ) {
	(ZIbt) = join_pre_types ( (*ZIb1), (ZIb2) ) ;
    } else {
	(ZIbt) = ( (*ZIb1) | (ZIb2) ) ;
    }
#line 1244 "syntax3.c"
		}
		/* END OF ACTION: btype_join */
		/* BEGINNING OF ACTION: type_join */
		{
#line 984 "syntax.act"

    /* Join two partial types */
    if ( IS_NULL_type ( (*ZIt1) ) ) {
	(ZIt) = (ZIt2) ;
    } else if ( IS_NULL_type ( (ZIt2) ) ) {
	(ZIt) = (*ZIt1) ;
    } else {
	report ( crt_loc, ERR_dcl_type_simple_many ( (*ZIt1), (ZIt2) ) ) ;
	(ZIt) = (ZIt2) ;
    }
#line 1260 "syntax3.c"
		}
		/* END OF ACTION: type_join */
		/* BEGINNING OF ACTION: cv_join */
		{
#line 910 "syntax.act"

    CV_SPEC c = ( (*ZIcv1) & (ZIcv2) ) ;
    if ( c ) report ( crt_loc, ERR_dcl_type_cv_dup ( c ) ) ;
    (ZIcv) = ( (*ZIcv1) | (ZIcv2) ) ;
#line 1270 "syntax3.c"
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
		ZR628 (&ZIq);
		if ((CURRENT_TERMINAL) == 356) {
			RESTORE_LEXER;
			goto ZL1;
		}
		/* BEGINNING OF ACTION: list_exp_cons */
		{
#line 416 "syntax.act"

    CONS_exp ( (ZIe), (ZIq), (ZIp) ) ;
#line 1319 "syntax3.c"
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
ZR1087(IDENTIFIER *ZIid, EXP *ZOe)
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
			EXP ZI1017;
			EXP ZI1018;
			EXP ZI1026;
			EXP ZIa;
			int ZIr;

			/* BEGINNING OF ACTION: exp_identifier */
			{
#line 504 "syntax.act"

    (ZI1017) = make_id_exp ( (*ZIid) ) ;
#line 1359 "syntax3.c"
			}
			/* END OF ACTION: exp_identifier */
			ZR927 (ZI1017, &ZI1018);
			ZR1013 (&ZI1018, &ZI1026);
			ZR1028 (&ZI1026, &ZIa);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: reach_check */
			{
#line 839 "syntax.act"

    (ZIr) = unreached_code ;
    if ( (ZIr) ) {
	if ( !unreached_last ) {
	    report ( crt_loc, ERR_stmt_stmt_unreach () ) ;
	    unreached_last = 1 ;
	}
    } else {
	unreached_last = 0 ;
    }
#line 1382 "syntax3.c"
			}
			/* END OF ACTION: reach_check */
			/* BEGINNING OF ACTION: stmt_exp */
			{
#line 709 "syntax.act"

    (ZIe) = make_exp_stmt ( (ZIa) ) ;
#line 1390 "syntax3.c"
			}
			/* END OF ACTION: stmt_exp */
			/* BEGINNING OF ACTION: stmt_label_clear */
			{
#line 776 "syntax.act"

    unreached_fall = 1 ;
#line 1398 "syntax3.c"
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
#line 780 "syntax.act"

    if ( unreached_code ) unreached_fall = 0 ;
#line 1420 "syntax3.c"
			}
			/* END OF ACTION: stmt_label_mod */
			/* BEGINNING OF ACTION: stmt_label_begin */
			{
#line 764 "syntax.act"

    (ZIa) = begin_label_stmt ( (*ZIid), lex_identifier ) ;
#line 1428 "syntax3.c"
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
#line 768 "syntax.act"

    (ZIe) = end_label_stmt ( (ZIa), (ZIb) ) ;
#line 1442 "syntax3.c"
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
ZRassignment_Hexpression(EXP *ZOe)
{
	EXP ZIe;

	switch (CURRENT_TERMINAL) {
	case 170:
		{
			int ZI1011;
			EXP ZI1012;

			ADVANCE_LEXER;
			/* BEGINNING OF ACTION: lex_alignof */
			{
#line 369 "syntax.act"
 (ZI1011) = lex_alignof ; 
#line 1476 "syntax3.c"
			}
			/* END OF ACTION: lex_alignof */
			ZRsizeof_Hexpression (ZI1011, &ZI1012);
			ZR1013 (&ZI1012, &ZIe);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
		}
		break;
	case 23:
		{
			EXP ZI1023;
			EXP ZI1024;

			/* BEGINNING OF EXTRACT: complex-exp */
			{
#line 329 "syntax.act"

    IDENTIFIER id = crt_token->pp_data.tok.id ;
    PPTOKEN *args = crt_token->pp_data.tok.args ;
    ZI1023 = parse_exp_token ( id, args ) ;
    RESCAN_LEXER ;
#line 1500 "syntax3.c"
			}
			/* END OF EXTRACT: complex-exp */
			ADVANCE_LEXER;
			ZR927 (ZI1023, &ZI1024);
			ZR1013 (&ZI1024, &ZIe);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
		}
		break;
	case 206:
		{
			EXP ZIa;
			EXP ZI966;
			EXP ZI965;
			EXP ZI964;
			EXP ZI963;
			EXP ZI962;
			EXP ZI961;
			EXP ZI960;
			EXP ZI959;
			EXP ZI958;
			EXP ZI957;
			EXP ZI956;
			EXP ZI953;

			ADVANCE_LEXER;
			ZRcast_Hexpression (&ZIa);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: exp_ignore */
			{
#line 508 "syntax.act"

    (ZI966) = make_cast_exp ( type_void, (ZIa), 0 ) ;
#line 1539 "syntax3.c"
			}
			/* END OF ACTION: exp_ignore */
			ZR923 (ZI966, &ZI965);
			ZR919 (ZI965, &ZI964);
			ZR915 (ZI964, &ZI963);
			ZR911 (ZI963, &ZI962);
			ZR907 (ZI962, &ZI961);
			ZR903 (ZI961, &ZI960);
			ZR899 (ZI960, &ZI959);
			ZR895 (ZI959, &ZI958);
			ZR891 (ZI958, &ZI957);
			ZR887 (ZI957, &ZI956);
			ZR883 (ZI956, &ZI953);
			ZR955 (&ZI953, &ZIe);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
		}
		break;
	case 210:
		{
			EXP ZI1020;
			EXP ZI1021;

			ADVANCE_LEXER;
			/* BEGINNING OF ACTION: exp_ellipsis */
			{
#line 486 "syntax.act"

    (ZI1020) = make_ellipsis_exp () ;
#line 1571 "syntax3.c"
			}
			/* END OF ACTION: exp_ellipsis */
			ZR927 (ZI1020, &ZI1021);
			ZR1013 (&ZI1021, &ZIe);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
		}
		break;
	case 1:
		{
			IDENTIFIER ZIid;
			EXP ZI1017;
			EXP ZI1018;

			/* BEGINNING OF EXTRACT: identifier */
			{
#line 195 "syntax.act"

    ZIid = crt_token->pp_data.id.use ;
#line 1593 "syntax3.c"
			}
			/* END OF EXTRACT: identifier */
			ADVANCE_LEXER;
			/* BEGINNING OF ACTION: exp_identifier */
			{
#line 504 "syntax.act"

    (ZI1017) = make_id_exp ( (ZIid) ) ;
#line 1602 "syntax3.c"
			}
			/* END OF ACTION: exp_identifier */
			ZR927 (ZI1017, &ZI1018);
			ZR1013 (&ZI1018, &ZIe);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
		}
		break;
	case 255:
		{
			EXP ZIa;
			EXP ZI966;
			EXP ZI965;
			EXP ZI964;
			EXP ZI963;
			EXP ZI962;
			EXP ZI961;
			EXP ZI960;
			EXP ZI959;
			EXP ZI958;
			EXP ZI957;
			EXP ZI956;
			EXP ZI953;

			ADVANCE_LEXER;
			ZRcast_Hexpression (&ZIa);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: exp_lit */
			{
#line 520 "syntax.act"

    (ZI966) = make_lit_exp ( (ZIa) ) ;
#line 1640 "syntax3.c"
			}
			/* END OF ACTION: exp_lit */
			ZR923 (ZI966, &ZI965);
			ZR919 (ZI965, &ZI964);
			ZR915 (ZI964, &ZI963);
			ZR911 (ZI963, &ZI962);
			ZR907 (ZI962, &ZI961);
			ZR903 (ZI961, &ZI960);
			ZR899 (ZI960, &ZI959);
			ZR895 (ZI959, &ZI958);
			ZR891 (ZI958, &ZI957);
			ZR887 (ZI957, &ZI956);
			ZR883 (ZI956, &ZI953);
			ZR955 (&ZI953, &ZIe);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
		}
		break;
	case 61:
		{
			EXP ZI992;
			EXP ZI993;

			ADVANCE_LEXER;
			ZRunary_Hexpression (&ZI992);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: exp_predec */
			{
#line 593 "syntax.act"

    (ZI993) = make_prefix_exp ( lex_minus_Hminus, (ZI992) ) ;
#line 1677 "syntax3.c"
			}
			/* END OF ACTION: exp_predec */
			ZR1013 (&ZI993, &ZIe);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
		}
		break;
	case 65:
		{
			ADVANCE_LEXER;
			ZR988 (&ZIe);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
		}
		break;
	case 71:
		{
			EXP ZI989;
			EXP ZI990;

			ADVANCE_LEXER;
			ZRunary_Hexpression (&ZI989);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: exp_preinc */
			{
#line 597 "syntax.act"

    (ZI990) = make_prefix_exp ( lex_plus_Hplus, (ZI989) ) ;
#line 1713 "syntax3.c"
			}
			/* END OF ACTION: exp_preinc */
			ZR1013 (&ZI990, &ZIe);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
		}
		break;
	case 116:
		{
			int ZI1008;
			EXP ZI1009;

			ADVANCE_LEXER;
			/* BEGINNING OF ACTION: lex_sizeof */
			{
#line 370 "syntax.act"
 (ZI1008) = lex_sizeof ; 
#line 1733 "syntax3.c"
			}
			/* END OF ACTION: lex_sizeof */
			ZRsizeof_Hexpression (ZI1008, &ZI1009);
			ZR1013 (&ZI1009, &ZIe);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
		}
		break;
	case 78:
		{
			EXP ZI995;
			EXP ZI996;

			ADVANCE_LEXER;
			ZRcast_Hexpression (&ZI995);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: exp_indir */
			{
#line 516 "syntax.act"

    (ZI996) = make_indir_exp ( (ZI995) ) ;
#line 1760 "syntax3.c"
			}
			/* END OF ACTION: exp_indir */
			ZR1013 (&ZI996, &ZIe);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
		}
		break;
	case 33:
		{
			EXP ZI998;
			EXP ZI999;

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
			ZRcast_Hexpression (&ZI998);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: exp_ref */
			{
#line 601 "syntax.act"

    (ZI999) = make_ref_exp ( (ZI998), 0 ) ;
#line 1798 "syntax3.c"
			}
			/* END OF ACTION: exp_ref */
			ZR1013 (&ZI999, &ZIe);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
		}
		break;
	case 17: case 18: case 19: case 20:
	case 21: case 22:
		{
			EXP ZI1014;
			EXP ZI1015;

			ZRliteral (&ZI1014);
			ZR927 (ZI1014, &ZI1015);
			ZR1013 (&ZI1015, &ZIe);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
		}
		break;
	case 62:
		{
			EXP ZI1001;
			EXP ZI1002;

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
			ZRcast_Hexpression (&ZI1001);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: exp_not */
			{
#line 564 "syntax.act"

    (ZI1002) = make_not_exp ( (ZI1001) ) ;
#line 1851 "syntax3.c"
			}
			/* END OF ACTION: exp_not */
			ZR1013 (&ZI1002, &ZIe);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
		}
		break;
	case 43: case 59: case 69: case 85:
		{
			int ZI1004;
			EXP ZI1005;
			EXP ZI1006;

			/* BEGINNING OF ACTION: lex_crt */
			{
#line 366 "syntax.act"
 (ZI1004) = crt_lex_token ; 
#line 1871 "syntax3.c"
			}
			/* END OF ACTION: lex_crt */
			ZRunary_Hoperator ();
			ZRcast_Hexpression (&ZI1005);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: exp_unary */
			{
#line 626 "syntax.act"

    (ZI1006) = make_uminus_exp ( (ZI1004), (ZI1005) ) ;
#line 1885 "syntax3.c"
			}
			/* END OF ACTION: exp_unary */
			ZR1013 (&ZI1006, &ZIe);
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
ZR1088(IDENTIFIER *ZIid, EXP *ZOe)
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
#line 1923 "syntax3.c"
			}
			/* END OF ACTION: exp_identifier */
			/* BEGINNING OF ACTION: stmt_exp */
			{
#line 709 "syntax.act"

    (ZIe) = make_exp_stmt ( (ZIa) ) ;
#line 1931 "syntax3.c"
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
#line 780 "syntax.act"

    if ( unreached_code ) unreached_fall = 0 ;
#line 1946 "syntax3.c"
			}
			/* END OF ACTION: stmt_label_mod */
			/* BEGINNING OF ACTION: stmt_label_begin */
			{
#line 764 "syntax.act"

    (ZIa) = begin_label_stmt ( (*ZIid), lex_identifier ) ;
#line 1954 "syntax3.c"
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
#line 768 "syntax.act"

    (ZIe) = end_label_stmt ( (ZIa), (ZIb) ) ;
#line 1968 "syntax3.c"
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
#line 2011 "syntax3.c"
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
ZRlogical_Hand_Hexpression(EXP *ZO886)
{
	EXP ZI886;

	if ((CURRENT_TERMINAL) == 356) {
		return;
	}
	{
		EXP ZIe;

		ZRinclusive_Hor_Hexpression (&ZIe);
		ZR887 (ZIe, &ZI886);
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
	*ZO886 = ZI886;
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
#line 839 "syntax.act"

    (ZIr) = unreached_code ;
    if ( (ZIr) ) {
	if ( !unreached_last ) {
	    report ( crt_loc, ERR_stmt_stmt_unreach () ) ;
	    unreached_last = 1 ;
	}
    } else {
	unreached_last = 0 ;
    }
#line 2077 "syntax3.c"
			}
			/* END OF ACTION: reach_check */
			/* BEGINNING OF ACTION: stmt_break */
			{
#line 651 "syntax.act"

    (ZIe) = make_break_stmt () ;
#line 2085 "syntax3.c"
			}
			/* END OF ACTION: stmt_break */
			/* BEGINNING OF ACTION: stmt_label_clear */
			{
#line 776 "syntax.act"

    unreached_fall = 1 ;
#line 2093 "syntax3.c"
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
#line 839 "syntax.act"

    (ZIr) = unreached_code ;
    if ( (ZIr) ) {
	if ( !unreached_last ) {
	    report ( crt_loc, ERR_stmt_stmt_unreach () ) ;
	    unreached_last = 1 ;
	}
    } else {
	unreached_last = 0 ;
    }
#line 2121 "syntax3.c"
			}
			/* END OF ACTION: reach_check */
			/* BEGINNING OF ACTION: stmt_continue */
			{
#line 684 "syntax.act"

    (ZIe) = make_continue_stmt () ;
#line 2129 "syntax3.c"
			}
			/* END OF ACTION: stmt_continue */
			/* BEGINNING OF ACTION: stmt_label_clear */
			{
#line 776 "syntax.act"

    unreached_fall = 1 ;
#line 2137 "syntax3.c"
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
#line 839 "syntax.act"

    (ZIr) = unreached_code ;
    if ( (ZIr) ) {
	if ( !unreached_last ) {
	    report ( crt_loc, ERR_stmt_stmt_unreach () ) ;
	    unreached_last = 1 ;
	}
    } else {
	unreached_last = 0 ;
    }
#line 2165 "syntax3.c"
			}
			/* END OF ACTION: reach_check */
			ZRjump_Hlabel (&ZIe);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: stmt_label_clear */
			{
#line 776 "syntax.act"

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
	case 113:
		{
			int ZIr;
			EXP ZIa;

			ADVANCE_LEXER;
			/* BEGINNING OF ACTION: reach_check */
			{
#line 839 "syntax.act"

    (ZIr) = unreached_code ;
    if ( (ZIr) ) {
	if ( !unreached_last ) {
	    report ( crt_loc, ERR_stmt_stmt_unreach () ) ;
	    unreached_last = 1 ;
	}
    } else {
	unreached_last = 0 ;
    }
#line 2207 "syntax3.c"
			}
			/* END OF ACTION: reach_check */
			ZR619 (&ZIa);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: stmt_return */
			{
#line 784 "syntax.act"

    (ZIe) = make_return_stmt ( (ZIa), lex_return ) ;
#line 2220 "syntax3.c"
			}
			/* END OF ACTION: stmt_return */
			/* BEGINNING OF ACTION: stmt_label_clear */
			{
#line 776 "syntax.act"

    unreached_fall = 1 ;
#line 2228 "syntax3.c"
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
ZRtarget_Hcondition_Hhead(EXP *ZO935, EXP *ZO936, int *ZO937)
{
	EXP ZI935;
	EXP ZI936;
	int ZI937;

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
#line 2275 "syntax3.c"
			}
			/* END OF EXTRACT: hash-if */
			break;
		default:
			goto ZL1;
		}
		ADVANCE_LEXER;
		/* BEGINNING OF ACTION: cond_hash_if */
		{
#line 1317 "syntax.act"

    EXP c = crt_hash_cond ;
    crt_hash_cond = make_if_cond ( (ZIc), c ) ;
    (ZIp) = c ;
#line 2290 "syntax3.c"
		}
		/* END OF ACTION: cond_hash_if */
		/* BEGINNING OF ACTION: reach_check */
		{
#line 839 "syntax.act"

    (ZIr) = unreached_code ;
    if ( (ZIr) ) {
	if ( !unreached_last ) {
	    report ( crt_loc, ERR_stmt_stmt_unreach () ) ;
	    unreached_last = 1 ;
	}
    } else {
	unreached_last = 0 ;
    }
#line 2306 "syntax3.c"
		}
		/* END OF ACTION: reach_check */
		ZRcompound_Hstatement (&ZIa);
		if ((CURRENT_TERMINAL) == 356) {
			RESTORE_LEXER;
			goto ZL1;
		}
		/* BEGINNING OF ACTION: reach_prev */
		{
#line 851 "syntax.act"
 unreached_prev = (ZIr) ; 
#line 2318 "syntax3.c"
		}
		/* END OF ACTION: reach_prev */
		/* BEGINNING OF ACTION: stmt_hash_if */
		{
#line 808 "syntax.act"

    (ZIe) = begin_hash_if_stmt ( (ZIc), (ZIa) ) ;
#line 2326 "syntax3.c"
		}
		/* END OF ACTION: stmt_hash_if */
		ZR938 (ZIe, ZIp, ZIr, &ZI935, &ZI936, &ZI937);
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
	*ZO935 = ZI935;
	*ZO936 = ZI936;
	*ZO937 = ZI937;
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
		/* BEGINNING OF INLINE: 637 */
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
#line 910 "syntax.act"

    CV_SPEC c = ( (ZIa) & (ZIb) ) ;
    if ( c ) report ( crt_loc, ERR_dcl_type_cv_dup ( c ) ) ;
    (ZIcv) = ( (ZIa) | (ZIb) ) ;
#line 2376 "syntax3.c"
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
		/* END OF INLINE: 637 */
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
#line 663 "syntax.act"

    (ZIc) = begin_compound_stmt ( 1 ) ;
#line 2421 "syntax3.c"
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
#line 671 "syntax.act"

    COPY_int ( exp_sequence_block ( (ZIc) ), 2 ) ;
    (ZId) = 1 ;
#line 2443 "syntax3.c"
		}
		/* END OF ACTION: stmt_compound_block */
		ZRstatement_Hseq_Hopt (ZIc, ZId, &ZIb);
		if ((CURRENT_TERMINAL) == 356) {
			RESTORE_LEXER;
			goto ZL1;
		}
		/* BEGINNING OF ACTION: stmt_return_fall */
		{
#line 788 "syntax.act"

    (ZIr) = fall_return_stmt () ;
#line 2456 "syntax3.c"
		}
		/* END OF ACTION: stmt_return_fall */
		/* BEGINNING OF ACTION: stmt_compound_add */
		{
#line 676 "syntax.act"

    (ZIa) = add_compound_stmt ( (ZIb), (ZIr) ) ;
#line 2464 "syntax3.c"
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
#line 680 "syntax.act"

    (ZIe) = end_compound_stmt ( (ZIa) ) ;
#line 2485 "syntax3.c"
		}
		/* END OF ACTION: stmt_compound_end */
		/* BEGINNING OF ACTION: rescan_token */
		{
#line 1474 "syntax.act"

    RESCAN_LEXER ;
#line 2493 "syntax3.c"
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
#line 906 "syntax.act"
 (ZIcv) = cv_none ; 
#line 2526 "syntax3.c"
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
ZR968(EXP *ZOe)
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
#line 1445 "syntax.act"

    /* Resolve type-ids from expressions */
    (ZI0) = predict_typeid ( 0 ) ;
#line 2561 "syntax3.c"
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
#line 2588 "syntax3.c"
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
			EXP ZI967;

			/* BEGINNING OF ACTION: exp_paren_begin */
			{
#line 572 "syntax.act"

    IGNORE incr_value ( OPT_VAL_paren_depth ) ;
#line 2612 "syntax3.c"
			}
			/* END OF ACTION: exp_paren_begin */
			ZRexpression (&ZIa);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: exp_paren_end */
			{
#line 576 "syntax.act"

    (ZI967) = make_paren_exp ( (ZIa) ) ;
    decr_value ( OPT_VAL_paren_depth ) ;
#line 2626 "syntax3.c"
			}
			/* END OF ACTION: exp_paren_end */
			switch (CURRENT_TERMINAL) {
			case 39:
				break;
			default:
				goto ZL1;
			}
			ADVANCE_LEXER;
			ZR927 (ZI967, &ZIe);
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
			ZR1099 ();
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
#line 907 "syntax.act"
 (ZIcv) = cv_const ; 
#line 2692 "syntax3.c"
			}
			/* END OF ACTION: cv_const */
		}
		break;
	case 124:
		{
			ADVANCE_LEXER;
			/* BEGINNING OF ACTION: cv_volatile */
			{
#line 908 "syntax.act"
 (ZIcv) = cv_volatile ; 
#line 2704 "syntax3.c"
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
ZR1098(EXP *ZI1097, EXP *ZOe)
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

    CONS_exp ( (*ZI1097), (ZIq), (ZIp) ) ;
#line 2746 "syntax3.c"
			}
			/* END OF ACTION: list_exp_cons */
			/* BEGINNING OF ACTION: exp_comma */
			{
#line 466 "syntax.act"

    (ZIa) = make_comma_exp ( (ZIp) ) ;
#line 2754 "syntax3.c"
			}
			/* END OF ACTION: exp_comma */
			/* BEGINNING OF ACTION: reach_check */
			{
#line 839 "syntax.act"

    (ZIr) = unreached_code ;
    if ( (ZIr) ) {
	if ( !unreached_last ) {
	    report ( crt_loc, ERR_stmt_stmt_unreach () ) ;
	    unreached_last = 1 ;
	}
    } else {
	unreached_last = 0 ;
    }
#line 2770 "syntax3.c"
			}
			/* END OF ACTION: reach_check */
			/* BEGINNING OF ACTION: stmt_exp */
			{
#line 709 "syntax.act"

    (ZIe) = make_exp_stmt ( (ZIa) ) ;
#line 2778 "syntax3.c"
			}
			/* END OF ACTION: stmt_exp */
			/* BEGINNING OF ACTION: stmt_label_clear */
			{
#line 776 "syntax.act"

    unreached_fall = 1 ;
#line 2786 "syntax3.c"
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
#line 839 "syntax.act"

    (ZIr) = unreached_code ;
    if ( (ZIr) ) {
	if ( !unreached_last ) {
	    report ( crt_loc, ERR_stmt_stmt_unreach () ) ;
	    unreached_last = 1 ;
	}
    } else {
	unreached_last = 0 ;
    }
#line 2823 "syntax3.c"
			}
			/* END OF ACTION: reach_check */
			/* BEGINNING OF ACTION: stmt_exp */
			{
#line 709 "syntax.act"

    (ZIe) = make_exp_stmt ( (*ZI1097) ) ;
#line 2831 "syntax3.c"
			}
			/* END OF ACTION: stmt_exp */
			/* BEGINNING OF ACTION: stmt_label_clear */
			{
#line 776 "syntax.act"

    unreached_fall = 1 ;
#line 2839 "syntax3.c"
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
ZR1099(void)
{
	switch (CURRENT_TERMINAL) {
	case 77:
		{
			ADVANCE_LEXER;
			/* BEGINNING OF ACTION: stmt_label_set */
			{
#line 772 "syntax.act"

    unreached_fall = 0 ;
#line 2869 "syntax3.c"
			}
			/* END OF ACTION: stmt_label_set */
		}
		break;
	default:
		{
			/* BEGINNING OF ACTION: stmt_label_set */
			{
#line 772 "syntax.act"

    unreached_fall = 0 ;
#line 2881 "syntax3.c"
			}
			/* END OF ACTION: stmt_label_set */
		}
		break;
	case 356:
		return;
	}
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
			TYPE ZI1039;
			IDENTIFIER ZI1040;
			TYPE ZI1036;
			IDENTIFIER ZIid;
			TYPE ZIt;

			ADVANCE_LEXER;
			ZRdeclarator_Haux (&ZI1039, &ZI1040);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: declarator_bad */
			{
#line 1236 "syntax.act"

    if ( IS_NULL_type ( (ZI1039) ) ) {
	report ( crt_loc, ERR_dcl_meaning_paren () ) ;
    }
#line 2963 "syntax3.c"
			}
			/* END OF ACTION: declarator_bad */
			switch (CURRENT_TERMINAL) {
			case 39:
				break;
			default:
				goto ZL1;
			}
			ADVANCE_LEXER;
			ZR873 (ZI1039, ZI1040, &ZI1036, &ZIid);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: type_inject */
			{
#line 1056 "syntax.act"

    (ZIt) = ( IS_NULL_type ( (ZI1036) ) ? (ZIp) : inject_pre_type ( (ZI1036), (ZIp), 1 ) ) ;
#line 2983 "syntax3.c"
			}
			/* END OF ACTION: type_inject */
			/* BEGINNING OF ACTION: declare_member */
			{
#line 1206 "syntax.act"

    IDENTIFIER id = make_member_decl ( dspec_none, (ZIt), (ZIid), 0 ) ;
    if ( do_dump ) dump_declare ( id, &decl_loc, 0 ) ;
    have_type_declaration = TYPE_DECL_NONE ;
    have_func_declarator = 0 ;
#line 2994 "syntax3.c"
			}
			/* END OF ACTION: declare_member */
		}
		break;
	case 1: case 2: case 4:
		{
			IDENTIFIER ZI1043;

			/* BEGINNING OF INLINE: any-identifier */
			{
				switch (CURRENT_TERMINAL) {
				case 1:
					{
						/* BEGINNING OF EXTRACT: identifier */
						{
#line 195 "syntax.act"

    ZI1043 = crt_token->pp_data.id.use ;
#line 3013 "syntax3.c"
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

    ZI1043 = crt_token->pp_data.id.use ;
#line 3026 "syntax3.c"
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

    ZI1043 = crt_token->pp_data.id.use ;
#line 3039 "syntax3.c"
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
			ZR1044 (&ZIp, &ZIq, &ZI1043);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
		}
		break;
	case 78:
		{
			TYPE ZI1037;
			TYPE ZI1038;
			IDENTIFIER ZIid;
			TYPE ZI1036;
			TYPE ZIt;

			ZRptr_Hoperator (&ZI1037);
			ZRdeclarator_Haux (&ZI1038, &ZIid);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: type_build */
			{
#line 1060 "syntax.act"

    (ZI1036) = ( IS_NULL_type ( (ZI1038) ) ? (ZI1037) : inject_pre_type ( (ZI1038), (ZI1037), 0 ) ) ;
#line 3076 "syntax3.c"
			}
			/* END OF ACTION: type_build */
			/* BEGINNING OF ACTION: type_inject */
			{
#line 1056 "syntax.act"

    (ZIt) = ( IS_NULL_type ( (ZI1036) ) ? (ZIp) : inject_pre_type ( (ZI1036), (ZIp), 1 ) ) ;
#line 3084 "syntax3.c"
			}
			/* END OF ACTION: type_inject */
			/* BEGINNING OF ACTION: declare_member */
			{
#line 1206 "syntax.act"

    IDENTIFIER id = make_member_decl ( dspec_none, (ZIt), (ZIid), 0 ) ;
    if ( do_dump ) dump_declare ( id, &decl_loc, 0 ) ;
    have_type_declaration = TYPE_DECL_NONE ;
    have_func_declarator = 0 ;
#line 3095 "syntax3.c"
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
#line 3112 "syntax3.c"
			}
			/* END OF ACTION: id_anon */
			/* BEGINNING OF ACTION: declarator_begin */
			{
#line 1231 "syntax.act"

    IDENTIFIER pid = underlying_id ( (ZIid) ) ;
    DEREF_loc ( id_loc ( pid ), decl_loc ) ;
#line 3121 "syntax3.c"
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
#line 1049 "syntax.act"

    /* Check for anonymous bitfields */
    HASHID nm = DEREF_hashid ( id_name ( (ZIid) ) ) ;
    int z = IS_hashid_anon ( nm ) ;
    (ZIt) = make_bitfield_type ( (ZIp), (ZIq), (ZIc), z ) ;
#line 3138 "syntax3.c"
			}
			/* END OF ACTION: type_bitfield_mem */
			/* BEGINNING OF ACTION: declare_bitfield */
			{
#line 1220 "syntax.act"

    IDENTIFIER id = make_member_decl ( dspec_none, (ZIt), (ZIid), 0 ) ;
    if ( do_dump ) dump_declare ( id, &decl_loc, 0 ) ;
    have_type_declaration = TYPE_DECL_NONE ;
    have_func_declarator = 0 ;
#line 3149 "syntax3.c"
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
			ZR1077 (&ZId, &ZIp);
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
#line 3226 "syntax3.c"
			}
			/* END OF ACTION: exp_aggregate */
		}
		break;
	case 46:
		{
			/* BEGINNING OF INLINE: 795 */
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
#line 3253 "syntax3.c"
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

    BUFFER field_buffer = { 0 };
    HASHID hid;
    IDENTIFIER the_id = (ZIid);

    field_buffer.posn = extend_buffer(&field_buffer, field_buffer.posn);
    hid = DEREF_hashid(id_name(the_id));
    IGNORE print_hashid(hid, 1, 0, &field_buffer, 0);
    fprintf(stderr, "exp_designated_with_identifier: %s\n", field_buffer.start);
    MAKE_exp_designated_name( type_void, the_id, (ZIa), (ZIe) );
#line 3286 "syntax3.c"
					}
					/* END OF ACTION: exp_designated_with_identifier */
				}
			}
			/* END OF INLINE: 795 */
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
ZRadditive_Hexpression(EXP *ZO918)
{
	EXP ZI918;

	if ((CURRENT_TERMINAL) == 356) {
		return;
	}
	{
		EXP ZIe;

		ZRmultiplicative_Hexpression (&ZIe);
		ZR919 (ZIe, &ZI918);
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
	*ZO918 = ZI918;
}

void
ZRdirect_Hdeclarator(TYPE *ZO871, IDENTIFIER *ZO872)
{
	TYPE ZI871;
	IDENTIFIER ZI872;

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
#line 1236 "syntax.act"

    if ( IS_NULL_type ( (ZIt) ) ) {
	report ( crt_loc, ERR_dcl_meaning_paren () ) ;
    }
#line 3358 "syntax3.c"
			}
			/* END OF ACTION: declarator_bad */
			switch (CURRENT_TERMINAL) {
			case 39:
				break;
			default:
				goto ZL1;
			}
			ADVANCE_LEXER;
			ZR873 (ZIt, ZIid, &ZI871, &ZI872);
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
#line 3390 "syntax3.c"
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
#line 3403 "syntax3.c"
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
#line 3416 "syntax3.c"
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
#line 960 "syntax.act"

    (ZIt) = NULL_type ;
#line 3432 "syntax3.c"
			}
			/* END OF ACTION: type_none */
			/* BEGINNING OF ACTION: declarator_begin */
			{
#line 1231 "syntax.act"

    IDENTIFIER pid = underlying_id ( (ZIid) ) ;
    DEREF_loc ( id_loc ( pid ), decl_loc ) ;
#line 3441 "syntax3.c"
			}
			/* END OF ACTION: declarator_begin */
			ZR873 (ZIt, ZIid, &ZI871, &ZI872);
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
	*ZO871 = ZI871;
	*ZO872 = ZI872;
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
#line 855 "syntax.act"
 (ZIx) = crt_condition ; 
#line 3487 "syntax3.c"
			}
			/* END OF ACTION: condition_get */
			/* BEGINNING OF ACTION: reach_check */
			{
#line 839 "syntax.act"

    (ZIr) = unreached_code ;
    if ( (ZIr) ) {
	if ( !unreached_last ) {
	    report ( crt_loc, ERR_stmt_stmt_unreach () ) ;
	    unreached_last = 1 ;
	}
    } else {
	unreached_last = 0 ;
    }
#line 3503 "syntax3.c"
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
#line 743 "syntax.act"

    (ZIa) = begin_if_stmt ( (ZIc) ) ;
#line 3517 "syntax3.c"
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
#line 663 "syntax.act"

    (ZIbs) = begin_compound_stmt ( 1 ) ;
#line 3532 "syntax3.c"
			}
			/* END OF ACTION: stmt_compound_begin */
			ZRscoped_Hstatement (ZIbs, &ZIb);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: reach_prev */
			{
#line 851 "syntax.act"
 unreached_prev = (ZIr) ; 
#line 3544 "syntax3.c"
			}
			/* END OF ACTION: reach_prev */
			/* BEGINNING OF ACTION: stmt_if_cont */
			{
#line 747 "syntax.act"

    (ZId) = cont_if_stmt ( (ZIa), (ZIb) ) ;
#line 3552 "syntax3.c"
			}
			/* END OF ACTION: stmt_if_cont */
			/* BEGINNING OF INLINE: 587 */
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
#line 755 "syntax.act"

    check_empty_stmt ( lex_else ) ;
#line 3572 "syntax3.c"
					}
					/* END OF ACTION: stmt_else */
					/* BEGINNING OF ACTION: stmt_compound_begin */
					{
#line 663 "syntax.act"

    (ZIfs) = begin_compound_stmt ( 1 ) ;
#line 3580 "syntax3.c"
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
#line 759 "syntax.act"

    report ( crt_loc, ERR_stmt_if_no_else () ) ;
    (ZIf) = NULL_exp ;
#line 3598 "syntax3.c"
					}
					/* END OF ACTION: stmt_no_else */
				}
			ZL2:;
			}
			/* END OF INLINE: 587 */
			/* BEGINNING OF ACTION: reach_prev */
			{
#line 851 "syntax.act"
 unreached_prev = (ZIr) ; 
#line 3609 "syntax3.c"
			}
			/* END OF ACTION: reach_prev */
			/* BEGINNING OF ACTION: stmt_if_end */
			{
#line 751 "syntax.act"

    (ZIe) = end_if_stmt ( (ZId), (ZIf) ) ;
#line 3617 "syntax3.c"
			}
			/* END OF ACTION: stmt_if_end */
			/* BEGINNING OF ACTION: condition_set */
			{
#line 856 "syntax.act"
 crt_condition = (ZIx) ; 
#line 3624 "syntax3.c"
			}
			/* END OF ACTION: condition_set */
			/* BEGINNING OF ACTION: stmt_label_clear */
			{
#line 776 "syntax.act"

    unreached_fall = 1 ;
#line 3632 "syntax3.c"
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
#line 839 "syntax.act"

    (ZIr) = unreached_code ;
    if ( (ZIr) ) {
	if ( !unreached_last ) {
	    report ( crt_loc, ERR_stmt_stmt_unreach () ) ;
	    unreached_last = 1 ;
	}
    } else {
	unreached_last = 0 ;
    }
#line 3660 "syntax3.c"
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
#line 792 "syntax.act"

    (ZIa) = begin_switch_stmt ( (ZIc) ) ;
#line 3680 "syntax3.c"
			}
			/* END OF ACTION: stmt_switch_begin */
			switch (CURRENT_TERMINAL) {
			case 39:
				break;
			default:
				goto ZL1;
			}
			ADVANCE_LEXER;
			/* BEGINNING OF INLINE: 590 */
			{
				switch (CURRENT_TERMINAL) {
				case 217:
					{
						ADVANCE_LEXER;
						/* BEGINNING OF ACTION: bool_true */
						{
#line 1415 "syntax.act"
 (ZIex) = 1 ; 
#line 3700 "syntax3.c"
						}
						/* END OF ACTION: bool_true */
					}
					break;
				default:
					{
						/* BEGINNING OF ACTION: bool_false */
						{
#line 1414 "syntax.act"
 (ZIex) = 0 ; 
#line 3711 "syntax3.c"
						}
						/* END OF ACTION: bool_false */
					}
					break;
				}
			}
			/* END OF INLINE: 590 */
			/* BEGINNING OF ACTION: stmt_compound_begin */
			{
#line 663 "syntax.act"

    (ZIbs) = begin_compound_stmt ( 1 ) ;
#line 3724 "syntax3.c"
			}
			/* END OF ACTION: stmt_compound_begin */
			ZRscoped_Hstatement (ZIbs, &ZIb);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: reach_prev */
			{
#line 851 "syntax.act"
 unreached_prev = (ZIr) ; 
#line 3736 "syntax3.c"
			}
			/* END OF ACTION: reach_prev */
			/* BEGINNING OF ACTION: stmt_switch_end */
			{
#line 796 "syntax.act"

    (ZIe) = end_switch_stmt ( (ZIa), (ZIb), (ZIex) ) ;
#line 3744 "syntax3.c"
			}
			/* END OF ACTION: stmt_switch_end */
			/* BEGINNING OF ACTION: stmt_label_clear */
			{
#line 776 "syntax.act"

    unreached_fall = 1 ;
#line 3752 "syntax3.c"
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
#line 776 "syntax.act"

    unreached_fall = 1 ;
#line 3769 "syntax3.c"
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
			EXP ZI1026;

			ZRassignment_Hexpression (&ZI1026);
			ZR1028 (&ZI1026, &ZIe);
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
#line 3827 "syntax3.c"
			}
			/* END OF ACTION: list_exp_cons */
			/* BEGINNING OF ACTION: exp_comma */
			{
#line 466 "syntax.act"

    (ZIe) = make_comma_exp ( (ZIp) ) ;
#line 3835 "syntax3.c"
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
#line 3857 "syntax3.c"
			}
			/* END OF ACTION: list_exp_cons */
			/* BEGINNING OF ACTION: exp_comma */
			{
#line 466 "syntax.act"

    (ZIe) = make_comma_exp ( (ZIp) ) ;
#line 3865 "syntax3.c"
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
#line 1435 "syntax.act"

    /* Resolve declaration-statements from expression-statements */
    int b = predict_decl () ;
    if ( b ) {
	if ( !(ZId) ) report ( crt_loc, ERR_stmt_dcl_start () ) ;
	in_declaration++ ;
    }
    (ZI0) = b ;
#line 3907 "syntax3.c"
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
#line 688 "syntax.act"

    in_declaration-- ;
    (ZIa) = NULL_exp ;
#line 3923 "syntax3.c"
		}
		/* END OF ACTION: stmt_decl */
		/* BEGINNING OF ACTION: stmt_label_clear */
		{
#line 776 "syntax.act"

    unreached_fall = 1 ;
#line 3931 "syntax3.c"
		}
		/* END OF ACTION: stmt_label_clear */
		/* BEGINNING OF ACTION: stmt_compound_add */
		{
#line 676 "syntax.act"

    (ZIb) = add_compound_stmt ( (ZIc), (ZIa) ) ;
#line 3939 "syntax3.c"
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
#line 676 "syntax.act"

    (ZIb) = add_compound_stmt ( (ZIc), (ZIa) ) ;
#line 3977 "syntax3.c"
			}
			/* END OF ACTION: stmt_compound_add */
			/* BEGINNING OF ACTION: bool_false */
			{
#line 1414 "syntax.act"
 (ZIdb) = 0 ; 
#line 3984 "syntax3.c"
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
#line 671 "syntax.act"

    COPY_int ( exp_sequence_block ( (ZIc) ), 2 ) ;
    (ZId) = 1 ;
#line 4037 "syntax3.c"
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
#line 676 "syntax.act"

    (ZIe) = add_compound_stmt ( (ZIc), (ZIa) ) ;
#line 4085 "syntax3.c"
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
ZR728(BASE_TYPE *ZIb1, TYPE *ZIt1, CV_SPEC *ZIcv1, DECL_SPEC *ZIds1, BASE_TYPE *ZObt, TYPE *ZOt, CV_SPEC *ZOcv, DECL_SPEC *ZOds)
{
	BASE_TYPE ZIbt;
	TYPE ZIt;
	CV_SPEC ZIcv;
	DECL_SPEC ZIds;

	if ((CURRENT_TERMINAL) == 356) {
		return;
	}
	{
		BASE_TYPE ZI1102;
		TYPE ZI1103;
		CV_SPEC ZI1104;
		DECL_SPEC ZI1105;
		BASE_TYPE ZIb2;
		TYPE ZIt2;
		CV_SPEC ZIcv2;
		DECL_SPEC ZIds2;

		/* BEGINNING OF ACTION: is_decl_specifier */
		{
#line 1430 "syntax.act"

    /* Resolve declaration-specifiers from other declarators */
    (ZI0) = predict_dspec ( 0 ) ;
#line 4178 "syntax3.c"
		}
		/* END OF ACTION: is_decl_specifier */
		if (!ZI0)
			goto ZL2;
		ZRdecl_Hspecifier (&ZI1102, &ZI1103, &ZI1104, &ZI1105);
		if ((CURRENT_TERMINAL) == 356) {
			RESTORE_LEXER;
			goto ZL1;
		}
		/* BEGINNING OF ACTION: check_decl_specifier */
		{
#line 1478 "syntax.act"

    /* A type-name can be a declarator-id */
    if ( have_type_specifier && crt_lex_token == lex_type_Hname ) {
	crt_lex_token = lex_identifier ;
    }
#line 4196 "syntax3.c"
		}
		/* END OF ACTION: check_decl_specifier */
		ZR728 (&ZI1102, &ZI1103, &ZI1104, &ZI1105, &ZIb2, &ZIt2, &ZIcv2, &ZIds2);
		if ((CURRENT_TERMINAL) == 356) {
			RESTORE_LEXER;
			goto ZL1;
		}
		/* BEGINNING OF ACTION: btype_join */
		{
#line 939 "syntax.act"

    if ( (*ZIb1) & (ZIb2) ) {
	(ZIbt) = join_pre_types ( (*ZIb1), (ZIb2) ) ;
    } else {
	(ZIbt) = ( (*ZIb1) | (ZIb2) ) ;
    }
#line 4213 "syntax3.c"
		}
		/* END OF ACTION: btype_join */
		/* BEGINNING OF ACTION: type_join */
		{
#line 984 "syntax.act"

    /* Join two partial types */
    if ( IS_NULL_type ( (*ZIt1) ) ) {
	(ZIt) = (ZIt2) ;
    } else if ( IS_NULL_type ( (ZIt2) ) ) {
	(ZIt) = (*ZIt1) ;
    } else {
	report ( crt_loc, ERR_dcl_type_simple_many ( (*ZIt1), (ZIt2) ) ) ;
	(ZIt) = (ZIt2) ;
    }
#line 4229 "syntax3.c"
		}
		/* END OF ACTION: type_join */
		/* BEGINNING OF ACTION: cv_join */
		{
#line 910 "syntax.act"

    CV_SPEC c = ( (*ZIcv1) & (ZIcv2) ) ;
    if ( c ) report ( crt_loc, ERR_dcl_type_cv_dup ( c ) ) ;
    (ZIcv) = ( (*ZIcv1) | (ZIcv2) ) ;
#line 4239 "syntax3.c"
		}
		/* END OF ACTION: cv_join */
		/* BEGINNING OF ACTION: dspec_join */
		{
#line 1133 "syntax.act"

    /* Combine two declaration specifiers */
    DECL_SPEC d = ( ( (*ZIds1) & (ZIds2) ) & dspec_duplicate ) ;
    if ( d ) report ( crt_loc, ERR_dcl_spec_dup ( d ) ) ;
    (ZIds) = ( (*ZIds1) | (ZIds2) ) ;
#line 4250 "syntax3.c"
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
ZRdirect_Habstract_Hdeclarator(TYPE *ZO864)
{
	TYPE ZI864;

	switch (CURRENT_TERMINAL) {
	case 65:
		{
			ADVANCE_LEXER;
			ZR1061 (&ZI864);
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
#line 4313 "syntax3.c"
			}
			/* END OF ACTION: id_none */
			/* BEGINNING OF ACTION: param_begin */
			{
#line 883 "syntax.act"

    func_type_defn ( 0 ) ;
    begin_param ( (ZIid) ) ;
    have_type_declaration = TYPE_DECL_NONE ;
    have_func_declarator = 0 ;
#line 4324 "syntax3.c"
			}
			/* END OF ACTION: param_begin */
			ZR754 (&ZIell);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: type_func_weak */
			{
#line 1028 "syntax.act"

    (ZIt) = make_func_type ( NULL_type, ( (ZIell) | FUNC_WEAK ), cv_c, empty_type_set ) ;
#line 4337 "syntax3.c"
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
#line 890 "syntax.act"

    end_param () ;
    have_type_declaration = TYPE_DECL_NONE ;
    have_func_declarator = 1 ;
#line 4354 "syntax3.c"
			}
			/* END OF ACTION: param_end */
			ZR865 (ZIt, &ZI864);
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
			ZR761 (&ZIe);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: type_array */
			{
#line 1040 "syntax.act"

    NAT n = make_array_dim ( (ZIe) ) ;
    MAKE_type_array ( cv_none, NULL_type, n, (ZIt) ) ;
#line 4393 "syntax3.c"
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
			ZR865 (ZIt, &ZI864);
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
	*ZO864 = ZI864;
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
#line 855 "syntax.act"
 (ZIx) = crt_condition ; 
#line 4450 "syntax3.c"
			}
			/* END OF ACTION: condition_get */
			/* BEGINNING OF ACTION: reach_check */
			{
#line 839 "syntax.act"

    (ZIr) = unreached_code ;
    if ( (ZIr) ) {
	if ( !unreached_last ) {
	    report ( crt_loc, ERR_stmt_stmt_unreach () ) ;
	    unreached_last = 1 ;
	}
    } else {
	unreached_last = 0 ;
    }
#line 4466 "syntax3.c"
			}
			/* END OF ACTION: reach_check */
			/* BEGINNING OF ACTION: stmt_do_begin */
			{
#line 701 "syntax.act"

    (ZIa) = begin_do_stmt () ;
#line 4474 "syntax3.c"
			}
			/* END OF ACTION: stmt_do_begin */
			/* BEGINNING OF ACTION: stmt_compound_begin */
			{
#line 663 "syntax.act"

    (ZIbs) = begin_compound_stmt ( 1 ) ;
#line 4482 "syntax3.c"
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
#line 828 "syntax.act"

    (ZIc) = bind_temporary ( (ZIc0) ) ;
#line 4513 "syntax3.c"
			}
			/* END OF ACTION: bind_temporary */
			/* BEGINNING OF ACTION: reach_prev */
			{
#line 851 "syntax.act"
 unreached_prev = (ZIr) ; 
#line 4520 "syntax3.c"
			}
			/* END OF ACTION: reach_prev */
			/* BEGINNING OF ACTION: stmt_do_end */
			{
#line 705 "syntax.act"

    (ZIe) = end_do_stmt ( (ZIa), (ZIb), (ZIc) ) ;
#line 4528 "syntax3.c"
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
#line 856 "syntax.act"
 crt_condition = (ZIx) ; 
#line 4542 "syntax3.c"
			}
			/* END OF ACTION: condition_set */
			/* BEGINNING OF ACTION: stmt_label_clear */
			{
#line 776 "syntax.act"

    unreached_fall = 1 ;
#line 4550 "syntax3.c"
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
#line 855 "syntax.act"
 (ZIx) = crt_condition ; 
#line 4578 "syntax3.c"
			}
			/* END OF ACTION: condition_get */
			/* BEGINNING OF ACTION: reach_check */
			{
#line 839 "syntax.act"

    (ZIr) = unreached_code ;
    if ( (ZIr) ) {
	if ( !unreached_last ) {
	    report ( crt_loc, ERR_stmt_stmt_unreach () ) ;
	    unreached_last = 1 ;
	}
    } else {
	unreached_last = 0 ;
    }
#line 4594 "syntax3.c"
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
#line 713 "syntax.act"

    (ZIf) = begin_for_stmt () ;
#line 4609 "syntax3.c"
			}
			/* END OF ACTION: stmt_for_begin */
			ZRfor_Hinit_Hstatement (&ZIa);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: stmt_for_init */
			{
#line 717 "syntax.act"

    (ZIg) = init_for_stmt ( (ZIf), &(ZIa) ) ;
#line 4622 "syntax3.c"
			}
			/* END OF ACTION: stmt_for_init */
			ZRfor_Hcond_Hstatement (&ZIc);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: stmt_compound_begin */
			{
#line 663 "syntax.act"

    (ZIds) = begin_compound_stmt ( 1 ) ;
#line 4635 "syntax3.c"
			}
			/* END OF ACTION: stmt_compound_begin */
			ZRfor_Hend_Hstatement (&ZIb);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: stmt_for_cond */
			{
#line 721 "syntax.act"

    (ZIh) = cond_for_stmt ( (ZIg), (ZIc), (ZIb) ) ;
#line 4648 "syntax3.c"
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
#line 667 "syntax.act"

    mark_compound_stmt ( (ZIds) ) ;
#line 4663 "syntax3.c"
			}
			/* END OF ACTION: stmt_compound_mark */
			ZRscoped_Hstatement (ZIds, &ZId);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: reach_prev */
			{
#line 851 "syntax.act"
 unreached_prev = (ZIr) ; 
#line 4675 "syntax3.c"
			}
			/* END OF ACTION: reach_prev */
			/* BEGINNING OF ACTION: stmt_for_end */
			{
#line 725 "syntax.act"

    (ZIe) = end_for_stmt ( (ZIh), (ZId) ) ;
#line 4683 "syntax3.c"
			}
			/* END OF ACTION: stmt_for_end */
			/* BEGINNING OF ACTION: condition_set */
			{
#line 856 "syntax.act"
 crt_condition = (ZIx) ; 
#line 4690 "syntax3.c"
			}
			/* END OF ACTION: condition_set */
			/* BEGINNING OF ACTION: stmt_label_clear */
			{
#line 776 "syntax.act"

    unreached_fall = 1 ;
#line 4698 "syntax3.c"
			}
			/* END OF ACTION: stmt_label_clear */
			/* BEGINNING OF ACTION: rescan_token */
			{
#line 1474 "syntax.act"

    RESCAN_LEXER ;
#line 4706 "syntax3.c"
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
#line 855 "syntax.act"
 (ZIx) = crt_condition ; 
#line 4726 "syntax3.c"
			}
			/* END OF ACTION: condition_get */
			/* BEGINNING OF ACTION: reach_check */
			{
#line 839 "syntax.act"

    (ZIr) = unreached_code ;
    if ( (ZIr) ) {
	if ( !unreached_last ) {
	    report ( crt_loc, ERR_stmt_stmt_unreach () ) ;
	    unreached_last = 1 ;
	}
    } else {
	unreached_last = 0 ;
    }
#line 4742 "syntax3.c"
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
#line 828 "syntax.act"

    (ZIc) = bind_temporary ( (ZIc0) ) ;
#line 4762 "syntax3.c"
			}
			/* END OF ACTION: bind_temporary */
			/* BEGINNING OF ACTION: stmt_while_begin */
			{
#line 800 "syntax.act"

    (ZIa) = begin_while_stmt ( (ZIc) ) ;
#line 4770 "syntax3.c"
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
#line 663 "syntax.act"

    (ZIbs) = begin_compound_stmt ( 1 ) ;
#line 4785 "syntax3.c"
			}
			/* END OF ACTION: stmt_compound_begin */
			ZRscoped_Hstatement (ZIbs, &ZIb);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: reach_prev */
			{
#line 851 "syntax.act"
 unreached_prev = (ZIr) ; 
#line 4797 "syntax3.c"
			}
			/* END OF ACTION: reach_prev */
			/* BEGINNING OF ACTION: stmt_while_end */
			{
#line 804 "syntax.act"

    (ZIe) = end_while_stmt ( (ZIa), (ZIb) ) ;
#line 4805 "syntax3.c"
			}
			/* END OF ACTION: stmt_while_end */
			/* BEGINNING OF ACTION: condition_set */
			{
#line 856 "syntax.act"
 crt_condition = (ZIx) ; 
#line 4812 "syntax3.c"
			}
			/* END OF ACTION: condition_set */
			/* BEGINNING OF ACTION: stmt_label_clear */
			{
#line 776 "syntax.act"

    unreached_fall = 1 ;
#line 4820 "syntax3.c"
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

void
ZRexpression_Hstatement(EXP *ZOe)
{
	EXP ZIe;

	switch (CURRENT_TERMINAL) {
	case 347:
		{
			ADVANCE_LEXER;
			ZR1083 (&ZIe);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
		}
		break;
	case 77:
		{
			ADVANCE_LEXER;
			/* BEGINNING OF ACTION: stmt_none */
			{
#line 647 "syntax.act"

    (ZIe) = NULL_exp ;
#line 4963 "syntax3.c"
			}
			/* END OF ACTION: stmt_none */
		}
		break;
	case 1: case 17: case 18: case 19:
	case 20: case 21: case 22: case 23:
	case 33: case 43: case 59: case 61:
	case 62: case 65: case 69: case 71:
	case 78: case 85: case 116: case 170:
	case 206: case 210: case 255:
		{
			EXP ZI1026;
			EXP ZIa;
			int ZIr;

			ZRassignment_Hexpression (&ZI1026);
			ZR1028 (&ZI1026, &ZIa);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: reach_check */
			{
#line 839 "syntax.act"

    (ZIr) = unreached_code ;
    if ( (ZIr) ) {
	if ( !unreached_last ) {
	    report ( crt_loc, ERR_stmt_stmt_unreach () ) ;
	    unreached_last = 1 ;
	}
    } else {
	unreached_last = 0 ;
    }
#line 4998 "syntax3.c"
			}
			/* END OF ACTION: reach_check */
			/* BEGINNING OF ACTION: stmt_exp */
			{
#line 709 "syntax.act"

    (ZIe) = make_exp_stmt ( (ZIa) ) ;
#line 5006 "syntax3.c"
			}
			/* END OF ACTION: stmt_exp */
			/* BEGINNING OF ACTION: stmt_label_clear */
			{
#line 776 "syntax.act"

    unreached_fall = 1 ;
#line 5014 "syntax3.c"
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
	case 298: case 324:
		{
			EXP ZI1078;
			EXP ZIa;
			int ZIr;

			ZRflow_Hexpression (&ZI1078);
			ZR1080 (&ZI1078, &ZIa);
			if ((CURRENT_TERMINAL) == 356) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: reach_check */
			{
#line 839 "syntax.act"

    (ZIr) = unreached_code ;
    if ( (ZIr) ) {
	if ( !unreached_last ) {
	    report ( crt_loc, ERR_stmt_stmt_unreach () ) ;
	    unreached_last = 1 ;
	}
    } else {
	unreached_last = 0 ;
    }
#line 5051 "syntax3.c"
			}
			/* END OF ACTION: reach_check */
			/* BEGINNING OF ACTION: stmt_exp */
			{
#line 709 "syntax.act"

    (ZIe) = make_exp_stmt ( (ZIa) ) ;
#line 5059 "syntax3.c"
			}
			/* END OF ACTION: stmt_exp */
			/* BEGINNING OF ACTION: stmt_label_clear */
			{
#line 776 "syntax.act"

    unreached_fall = 1 ;
#line 5067 "syntax3.c"
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

/* END OF FILE */
