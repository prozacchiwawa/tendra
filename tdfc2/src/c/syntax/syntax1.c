/*
 * Automatically generated from the files:
 *	syntax.sid
 * and
 *	syntax.act
 * by:
 *	sid
 */

/* BEGINNING OF HEADER */

#define __SID_SPLIT
#line 168 "syntax.act"


/* $Id$ */

/*
 * Copyright 2011, The TenDRA Project.
 * Copyright 1997-1998, United Kingdom Secretary of State for Defence.
 *
 * See doc/copyright/ for the full copyright terms.
 */

#include <stdio.h>

#include <shared/check.h>

#include "config.h"
#include "c_types.h"
#include "exp_ops.h"
#include "hashid_ops.h"
#include "id_ops.h"
#include "type_ops.h"
#include "error.h"
#include "catalog.h"
#include "option.h"
#include "access.h"
#include "allocate.h"
#include "assign.h"
#include "basetype.h"
#include "cast.h"
#include "chktype.h"
#include "class.h"
#include "constant.h"
#include "construct.h"
#include "convert.h"
#include "declare.h"
#include "derive.h"
#include "dump.h"
#include "exception.h"
#include "expression.h"
#include "function.h"
#include "hash.h"
#include "identifier.h"
#include "initialise.h"
#include "inttype.h"
#include "label.h"
#include "lex.h"
#include "literal.h"
#include "member.h"
#include "namespace.h"
#include "parse.h"
#include "pragma.h"
#include "predict.h"
#include "preproc.h"
#include "redeclare.h"
#include "rewrite.h"
#include "statement.h"
#include "symbols.h"
#include "template.h"
#include "tokdef.h"
#include "token.h"
#include "typeid.h"
#include "variable.h"


/*
    These are the aliases for the compound types used in the parser.
*/

typedef LIST ( EXP ) SID_LIST_EXP ;


/*
    The function declarations are included at this point so that the
    type definitions are in scope.
*/

#include "syntax.h"


#line 94 "syntax1.c"

/* BEGINNING OF STATIC VARIABLES */

int ZI0;

/* BEGINNING OF FUNCTION DEFINITIONS */

void
ZRbase_Htype_Hspecifier(BASE_TYPE *ZObt)
{
	BASE_TYPE ZIbt;

	switch (CURRENT_TERMINAL) {
	case 185:
		{
			ADVANCE_LEXER;
			/* BEGINNING OF ACTION: btype_bottom */
			{
#line 926 "syntax.act"
 (ZIbt) = btype_bottom ; 
#line 115 "syntax1.c"
			}
			/* END OF ACTION: btype_bottom */
		}
		break;
	case 97:
		{
			ADVANCE_LEXER;
			/* BEGINNING OF ACTION: btype_char */
			{
#line 914 "syntax.act"
 (ZIbt) = btype_char ; 
#line 127 "syntax1.c"
			}
			/* END OF ACTION: btype_char */
		}
		break;
	case 102:
		{
			ADVANCE_LEXER;
			/* BEGINNING OF ACTION: btype_double */
			{
#line 921 "syntax.act"
 (ZIbt) = btype_double ; 
#line 139 "syntax1.c"
			}
			/* END OF ACTION: btype_double */
		}
		break;
	case 106:
		{
			ADVANCE_LEXER;
			/* BEGINNING OF ACTION: btype_float */
			{
#line 920 "syntax.act"
 (ZIbt) = btype_float ; 
#line 151 "syntax1.c"
			}
			/* END OF ACTION: btype_float */
		}
		break;
	case 110:
		{
			ADVANCE_LEXER;
			/* BEGINNING OF ACTION: btype_int */
			{
#line 916 "syntax.act"
 (ZIbt) = btype_int ; 
#line 163 "syntax1.c"
			}
			/* END OF ACTION: btype_int */
		}
		break;
	case 111:
		{
			ADVANCE_LEXER;
			/* BEGINNING OF ACTION: btype_long */
			{
#line 917 "syntax.act"
 (ZIbt) = btype_long ; 
#line 175 "syntax1.c"
			}
			/* END OF ACTION: btype_long */
		}
		break;
	case 286:
		{
			ADVANCE_LEXER;
			/* BEGINNING OF ACTION: btype_ptrdiff_t */
			{
#line 924 "syntax.act"
 (ZIbt) = btype_ptrdiff_t ; 
#line 187 "syntax1.c"
			}
			/* END OF ACTION: btype_ptrdiff_t */
		}
		break;
	case 114:
		{
			ADVANCE_LEXER;
			/* BEGINNING OF ACTION: btype_short */
			{
#line 915 "syntax.act"
 (ZIbt) = btype_short ; 
#line 199 "syntax1.c"
			}
			/* END OF ACTION: btype_short */
		}
		break;
	case 115:
		{
			ADVANCE_LEXER;
			/* BEGINNING OF ACTION: btype_signed */
			{
#line 918 "syntax.act"
 (ZIbt) = btype_signed ; 
#line 211 "syntax1.c"
			}
			/* END OF ACTION: btype_signed */
		}
		break;
	case 299:
		{
			ADVANCE_LEXER;
			/* BEGINNING OF ACTION: btype_size_t */
			{
#line 923 "syntax.act"
 (ZIbt) = btype_size_t ; 
#line 223 "syntax1.c"
			}
			/* END OF ACTION: btype_size_t */
		}
		break;
	case 122:
		{
			ADVANCE_LEXER;
			/* BEGINNING OF ACTION: btype_unsigned */
			{
#line 919 "syntax.act"
 (ZIbt) = btype_unsigned ; 
#line 235 "syntax1.c"
			}
			/* END OF ACTION: btype_unsigned */
		}
		break;
	case 123:
		{
			ADVANCE_LEXER;
			/* BEGINNING OF ACTION: btype_void */
			{
#line 925 "syntax.act"
 (ZIbt) = btype_void ; 
#line 247 "syntax1.c"
			}
			/* END OF ACTION: btype_void */
		}
		break;
	case 156:
		{
			ADVANCE_LEXER;
			/* BEGINNING OF ACTION: btype_wchar_t */
			{
#line 922 "syntax.act"
 (ZIbt) = btype_wchar_t ; 
#line 259 "syntax1.c"
			}
			/* END OF ACTION: btype_wchar_t */
		}
		break;
	case 355:
		return;
	default:
		goto ZL1;
	}
	goto ZL0;
ZL1:;
	SAVE_LEXER (355);
	return;
ZL0:;
	*ZObt = ZIbt;
}

void
ZR892(EXP ZI890, EXP *ZO891)
{
	EXP ZI891;

ZL2_892:;
	switch (CURRENT_TERMINAL) {
	case 80:
		{
			EXP ZIb;
			EXP ZIe;

			/* BEGINNING OF INLINE: xor */
			{
				{
					switch (CURRENT_TERMINAL) {
					case 80:
						break;
					default:
						goto ZL1;
					}
					ADVANCE_LEXER;
				}
			}
			/* END OF INLINE: xor */
			ZRand_Hexpression (&ZIb);
			if ((CURRENT_TERMINAL) == 355) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: exp_xor */
			{
#line 624 "syntax.act"

    (ZIe) = make_xor_exp ( (ZI890), (ZIb) ) ;
#line 312 "syntax1.c"
			}
			/* END OF ACTION: exp_xor */
			/* BEGINNING OF INLINE: 892 */
			ZI890 = ZIe;
			goto ZL2_892;
			/* END OF INLINE: 892 */
		}
		UNREACHED;
	default:
		{
			ZI891 = ZI890;
		}
		break;
	case 355:
		return;
	}
	goto ZL0;
ZL1:;
	SAVE_LEXER (355);
	return;
ZL0:;
	*ZO891 = ZI891;
}

void
ZRdecl_Hspecifier_Hseq(BASE_TYPE *ZObt, TYPE *ZOt, CV_SPEC *ZOcv, DECL_SPEC *ZOds)
{
	BASE_TYPE ZIbt;
	TYPE ZIt;
	CV_SPEC ZIcv;
	DECL_SPEC ZIds;

	if ((CURRENT_TERMINAL) == 355) {
		return;
	}
	{
		BASE_TYPE ZIb1;
		TYPE ZIt1;
		CV_SPEC ZIcv1;
		DECL_SPEC ZIds1;

		ZRdecl_Hspecifier (&ZIb1, &ZIt1, &ZIcv1, &ZIds1);
		if ((CURRENT_TERMINAL) == 355) {
			RESTORE_LEXER;
			goto ZL1;
		}
		/* BEGINNING OF ACTION: check_decl_specifier */
		{
#line 1473 "syntax.act"

    /* A type-name can be a declarator-id */
    if ( have_type_specifier && crt_lex_token == lex_type_Hname ) {
	crt_lex_token = lex_identifier ;
    }
#line 367 "syntax1.c"
		}
		/* END OF ACTION: check_decl_specifier */
		/* BEGINNING OF INLINE: 719 */
		{
			switch (CURRENT_TERMINAL) {
			case 2: case 25: case 94: case 97:
			case 98: case 102: case 104: case 105:
			case 106: case 110: case 111: case 112:
			case 114: case 115: case 117: case 118:
			case 120: case 121: case 122: case 123:
			case 124: case 137: case 156: case 185:
			case 286: case 299:
				{
					BASE_TYPE ZIb2;
					TYPE ZIt2;
					CV_SPEC ZIcv2;
					DECL_SPEC ZIds2;

					ZRdecl_Hspecifier_Hseq (&ZIb2, &ZIt2, &ZIcv2, &ZIds2);
					if ((CURRENT_TERMINAL) == 355) {
						RESTORE_LEXER;
						goto ZL1;
					}
					/* BEGINNING OF ACTION: btype_join */
					{
#line 930 "syntax.act"

    if ( (ZIb1) & (ZIb2) ) {
	(ZIbt) = join_pre_types ( (ZIb1), (ZIb2) ) ;
    } else {
	(ZIbt) = ( (ZIb1) | (ZIb2) ) ;
    }
#line 400 "syntax1.c"
					}
					/* END OF ACTION: btype_join */
					/* BEGINNING OF ACTION: type_join */
					{
#line 976 "syntax.act"

    /* Join two partial types */
    if ( IS_NULL_type ( (ZIt1) ) ) {
	(ZIt) = (ZIt2) ;
    } else if ( IS_NULL_type ( (ZIt2) ) ) {
	(ZIt) = (ZIt1) ;
    } else {
	report ( crt_loc, ERR_dcl_type_simple_many ( (ZIt1), (ZIt2) ) ) ;
	(ZIt) = (ZIt2) ;
    }
#line 416 "syntax1.c"
					}
					/* END OF ACTION: type_join */
					/* BEGINNING OF ACTION: cv_join */
					{
#line 901 "syntax.act"

    CV_SPEC c = ( (ZIcv1) & (ZIcv2) ) ;
    if ( c ) report ( crt_loc, ERR_dcl_type_cv_dup ( c ) ) ;
    (ZIcv) = ( (ZIcv1) | (ZIcv2) ) ;
#line 426 "syntax1.c"
					}
					/* END OF ACTION: cv_join */
					/* BEGINNING OF ACTION: dspec_join */
					{
#line 1125 "syntax.act"

    /* Combine two declaration specifiers */
    DECL_SPEC d = ( ( (ZIds1) & (ZIds2) ) & dspec_duplicate ) ;
    if ( d ) report ( crt_loc, ERR_dcl_spec_dup ( d ) ) ;
    (ZIds) = ( (ZIds1) | (ZIds2) ) ;
#line 437 "syntax1.c"
					}
					/* END OF ACTION: dspec_join */
				}
				break;
			default:
				{
					ZIbt = ZIb1;
					ZIt = ZIt1;
					ZIcv = ZIcv1;
					ZIds = ZIds1;
				}
				break;
			}
		}
		/* END OF INLINE: 719 */
	}
	goto ZL0;
ZL1:;
	SAVE_LEXER (355);
	return;
ZL0:;
	*ZObt = ZIbt;
	*ZOt = ZIt;
	*ZOcv = ZIcv;
	*ZOds = ZIds;
}

void
ZRdeclarator_Htail(IDENTIFIER ZIid, TYPE *ZOt)
{
	TYPE ZIt;

	switch (CURRENT_TERMINAL) {
	case 65:
		{
			TYPE ZIs;

			ADVANCE_LEXER;
			/* BEGINNING OF ACTION: param_begin */
			{
#line 875 "syntax.act"

    func_type_defn ( 0 ) ;
    begin_param ( (ZIid) ) ;
    have_type_declaration = TYPE_DECL_NONE ;
    have_func_declarator = 0 ;
#line 484 "syntax1.c"
			}
			/* END OF ACTION: param_begin */
			/* BEGINNING OF INLINE: 748 */
			{
				switch (CURRENT_TERMINAL) {
				case 2: case 25: case 47: case 94:
				case 97: case 98: case 102: case 104:
				case 105: case 106: case 110: case 111:
				case 112: case 114: case 115: case 117:
				case 118: case 120: case 121: case 122:
				case 123: case 124: case 137: case 156:
				case 185: case 286: case 299:
					{
						int ZIell;

						ZRparameter_Hdeclaration_Hlist (&ZIell);
						if ((CURRENT_TERMINAL) == 355) {
							RESTORE_LEXER;
							goto ZL1;
						}
						/* BEGINNING OF ACTION: type_func */
						{
#line 1015 "syntax.act"

    (ZIs) = make_func_type ( NULL_type, (ZIell), cv_c, empty_type_set ) ;
#line 510 "syntax1.c"
						}
						/* END OF ACTION: type_func */
						switch (CURRENT_TERMINAL) {
						case 39:
							break;
						default:
							goto ZL1;
						}
						ADVANCE_LEXER;
					}
					break;
				case 1: case 4:
					{
						ZRparameter_Hid_Hlist ();
						if ((CURRENT_TERMINAL) == 355) {
							RESTORE_LEXER;
							goto ZL1;
						}
						/* BEGINNING OF ACTION: type_func_old */
						{
#line 1023 "syntax.act"

    (ZIs) = make_func_type ( NULL_type, FUNC_PARAMS, cv_c, empty_type_set ) ;
#line 534 "syntax1.c"
						}
						/* END OF ACTION: type_func_old */
						switch (CURRENT_TERMINAL) {
						case 39:
							break;
						default:
							goto ZL1;
						}
						ADVANCE_LEXER;
					}
					break;
				case 39:
					{
						/* BEGINNING OF ACTION: type_func_none */
						{
#line 1027 "syntax.act"

    (ZIs) = make_func_type ( NULL_type, FUNC_NO_PARAMS, cv_c, empty_type_set ) ;
#line 553 "syntax1.c"
						}
						/* END OF ACTION: type_func_none */
						ADVANCE_LEXER;
					}
					break;
				default:
					goto ZL1;
				}
			}
			/* END OF INLINE: 748 */
			ZIt = ZIs;
			/* BEGINNING OF ACTION: param_end */
			{
#line 884 "syntax.act"

    end_param () ;
    have_type_declaration = TYPE_DECL_NONE ;
    have_func_declarator = 1 ;
#line 572 "syntax1.c"
			}
			/* END OF ACTION: param_end */
		}
		break;
	case 332:
		{
			int ZIell;

			ADVANCE_LEXER;
			switch (CURRENT_TERMINAL) {
			case 65:
				break;
			default:
				goto ZL1;
			}
			ADVANCE_LEXER;
			/* BEGINNING OF ACTION: param_begin */
			{
#line 875 "syntax.act"

    func_type_defn ( 0 ) ;
    begin_param ( (ZIid) ) ;
    have_type_declaration = TYPE_DECL_NONE ;
    have_func_declarator = 0 ;
#line 597 "syntax1.c"
			}
			/* END OF ACTION: param_begin */
			ZR760 (&ZIell);
			if ((CURRENT_TERMINAL) == 355) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: type_func_weak */
			{
#line 1019 "syntax.act"

    (ZIt) = make_func_type ( NULL_type, ( (ZIell) | FUNC_WEAK ), cv_c, empty_type_set ) ;
#line 610 "syntax1.c"
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
#line 884 "syntax.act"

    end_param () ;
    have_type_declaration = TYPE_DECL_NONE ;
    have_func_declarator = 1 ;
#line 627 "syntax1.c"
			}
			/* END OF ACTION: param_end */
		}
		break;
	case 66:
		{
			EXP ZIe;

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
			ZR759 (&ZIe);
			if ((CURRENT_TERMINAL) == 355) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: type_array */
			{
#line 1031 "syntax.act"

    NAT n = make_array_dim ( (ZIe) ) ;
    MAKE_type_array ( cv_none, NULL_type, n, (ZIt) ) ;
#line 660 "syntax1.c"
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
		}
		break;
	case 355:
		return;
	default:
		goto ZL1;
	}
	goto ZL0;
ZL1:;
	SAVE_LEXER (355);
	return;
ZL0:;
	*ZOt = ZIt;
}

void
ZRparameter_Hdeclaration(IDENTIFIER *ZOd)
{
	IDENTIFIER ZId;

	if ((CURRENT_TERMINAL) == 355) {
		return;
	}
	{
		BASE_TYPE ZIbt;
		TYPE ZIt1;
		CV_SPEC ZIcv;
		DECL_SPEC ZIds1;
		TYPE ZIt2;
		DECL_SPEC ZIds;
		TYPE ZIt;
		IDENTIFIER ZIid;

		ZRdecl_Hspecifier_Hseq (&ZIbt, &ZIt1, &ZIcv, &ZIds1);
		if ((CURRENT_TERMINAL) == 355) {
			RESTORE_LEXER;
			goto ZL1;
		}
		/* BEGINNING OF ACTION: dspec_complete */
		{
#line 1136 "syntax.act"

    /* Complete a declaration specifier and a type */
    (ZIds) = complete_dspec ( (ZIds1), (ZIbt), (ZIt1), (ZIcv) ) ;
    (ZIt2) = complete_pre_type ( (ZIbt), (ZIt1), (ZIcv), 0 ) ;
    have_type_specifier = 0 ;
#line 722 "syntax1.c"
		}
		/* END OF ACTION: dspec_complete */
		ZRparameter_Hdeclarator_Hopt (ZIt2, &ZIt, &ZIid);
		if ((CURRENT_TERMINAL) == 355) {
			RESTORE_LEXER;
			goto ZL1;
		}
		/* BEGINNING OF ACTION: declare_param */
		{
#line 1185 "syntax.act"

    (ZId) = make_param_decl ( (ZIds), (ZIt), (ZIid), CONTEXT_PARAMETER ) ;
    have_type_declaration = TYPE_DECL_NONE ;
    have_func_declarator = 0 ;
#line 737 "syntax1.c"
		}
		/* END OF ACTION: declare_param */
	}
	goto ZL0;
ZL1:;
	SAVE_LEXER (355);
	return;
ZL0:;
	*ZOd = ZId;
}

void
ZR896(EXP ZI894, EXP *ZO895)
{
	EXP ZI895;

ZL2_896:;
	switch (CURRENT_TERMINAL) {
	case 33:
		{
			EXP ZIb;
			EXP ZIe;

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
			ZRmaxmin_Hexpression (&ZIb);
			if ((CURRENT_TERMINAL) == 355) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: exp_and */
			{
#line 442 "syntax.act"

    (ZIe) = make_and_exp ( (ZI894), (ZIb) ) ;
#line 784 "syntax1.c"
			}
			/* END OF ACTION: exp_and */
			/* BEGINNING OF INLINE: 896 */
			ZI894 = ZIe;
			goto ZL2_896;
			/* END OF INLINE: 896 */
		}
		UNREACHED;
	default:
		{
			ZI895 = ZI894;
		}
		break;
	case 355:
		return;
	}
	goto ZL0;
ZL1:;
	SAVE_LEXER (355);
	return;
ZL0:;
	*ZO895 = ZI895;
}

void
ZRtarget_Hcondition(EXP *ZOe)
{
	EXP ZIe;

	if ((CURRENT_TERMINAL) == 355) {
		return;
	}
	{
		EXP ZIa;
		EXP ZIp;
		int ZIr;
		EXP ZIb;

		ZRtarget_Hcondition_Hhead (&ZIa, &ZIp, &ZIr);
		/* BEGINNING OF INLINE: 572 */
		{
			switch (CURRENT_TERMINAL) {
			case 28:
				{
					int ZIs;

					ADVANCE_LEXER;
					/* BEGINNING OF ACTION: cond_hash_else */
					{
#line 1320 "syntax.act"

    crt_hash_cond = make_else_cond ( crt_hash_cond ) ;
#line 837 "syntax1.c"
					}
					/* END OF ACTION: cond_hash_else */
					/* BEGINNING OF ACTION: reach_check */
					{
#line 830 "syntax.act"

    (ZIs) = unreached_code ;
    if ( (ZIs) ) {
	if ( !unreached_last ) {
	    report ( crt_loc, ERR_stmt_stmt_unreach () ) ;
	    unreached_last = 1 ;
	}
    } else {
	unreached_last = 0 ;
    }
#line 853 "syntax1.c"
					}
					/* END OF ACTION: reach_check */
					ZRcompound_Hstatement (&ZIb);
					if ((CURRENT_TERMINAL) == 355) {
						RESTORE_LEXER;
						goto ZL1;
					}
				}
				break;
			default:
				{
					/* BEGINNING OF ACTION: stmt_none */
					{
#line 638 "syntax.act"

    (ZIb) = NULL_exp ;
#line 870 "syntax1.c"
					}
					/* END OF ACTION: stmt_none */
				}
				break;
			case 355:
				RESTORE_LEXER;
				goto ZL1;
			}
		}
		/* END OF INLINE: 572 */
		/* BEGINNING OF ACTION: cond_hash_endif */
		{
#line 1323 "syntax.act"

    crt_hash_cond = (ZIp) ;
#line 886 "syntax1.c"
		}
		/* END OF ACTION: cond_hash_endif */
		switch (CURRENT_TERMINAL) {
		case 29:
			break;
		default:
			goto ZL1;
		}
		ADVANCE_LEXER;
		/* BEGINNING OF ACTION: reach_prev */
		{
#line 841 "syntax.act"
 unreached_prev = (ZIr) ; 
#line 900 "syntax1.c"
		}
		/* END OF ACTION: reach_prev */
		/* BEGINNING OF ACTION: stmt_hash_endif */
		{
#line 807 "syntax.act"

    (ZIe) = end_hash_if_stmt ( (ZIa), (ZIb) ) ;
#line 908 "syntax1.c"
		}
		/* END OF ACTION: stmt_hash_endif */
	}
	goto ZL0;
ZL1:;
	SAVE_LEXER (355);
	return;
ZL0:;
	*ZOe = ZIe;
}

void
ZR1025(EXP *ZI1023, EXP *ZOe)
{
	EXP ZIe;

	switch (CURRENT_TERMINAL) {
	case 42:
		{
			SID_LIST_EXP ZIq;
			SID_LIST_EXP ZIp;

			ADVANCE_LEXER;
			ZRcomma_Hexpression_Htail (&ZIq);
			if ((CURRENT_TERMINAL) == 355) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: list_exp_cons */
			{
#line 418 "syntax.act"

    CONS_exp ( (*ZI1023), (ZIq), (ZIp) ) ;
#line 942 "syntax1.c"
			}
			/* END OF ACTION: list_exp_cons */
			/* BEGINNING OF ACTION: exp_comma */
			{
#line 468 "syntax.act"

    (ZIe) = make_comma_exp ( (ZIp) ) ;
#line 950 "syntax1.c"
			}
			/* END OF ACTION: exp_comma */
		}
		break;
	default:
		{
			ZIe = *ZI1023;
		}
		break;
	case 355:
		return;
	}
	goto ZL0;
ZL1:;
	SAVE_LEXER (355);
	return;
ZL0:;
	*ZOe = ZIe;
}

void
ZRclass_Hspecifier(TYPE *ZOt)
{
	TYPE ZIt;

	if ((CURRENT_TERMINAL) == 355) {
		return;
	}
	{
		BASE_TYPE ZIkey;

		ZRclass_Hkey (&ZIkey);
		/* BEGINNING OF INLINE: 673 */
		{
			switch (CURRENT_TERMINAL) {
			case 1: case 2: case 4:
				{
					IDENTIFIER ZIid;

					/* BEGINNING OF INLINE: any-identifier */
					{
						switch (CURRENT_TERMINAL) {
						case 1:
							{
								/* BEGINNING OF EXTRACT: identifier */
								{
#line 197 "syntax.act"

    ZIid = crt_token->pp_data.id.use ;
#line 1000 "syntax1.c"
								}
								/* END OF EXTRACT: identifier */
								ADVANCE_LEXER;
							}
							break;
						case 4:
							{
								/* BEGINNING OF EXTRACT: statement-name */
								{
#line 209 "syntax.act"

    ZIid = crt_token->pp_data.id.use ;
#line 1013 "syntax1.c"
								}
								/* END OF EXTRACT: statement-name */
								ADVANCE_LEXER;
							}
							break;
						case 2:
							{
								/* BEGINNING OF EXTRACT: type-name */
								{
#line 201 "syntax.act"

    ZIid = crt_token->pp_data.id.use ;
#line 1026 "syntax1.c"
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
					ZR1028 (&ZIkey, &ZIid, &ZIt);
					if ((CURRENT_TERMINAL) == 355) {
						RESTORE_LEXER;
						goto ZL1;
					}
				}
				break;
			case 64:
				{
					IDENTIFIER ZIid;
					IDENTIFIER ZIp;
					int ZIf;
					IDENTIFIER ZItid;

					/* BEGINNING OF ACTION: id_anon */
					{
#line 389 "syntax.act"

    HASHID nm = lookup_anon () ;
    (ZIid) = DEREF_id ( hashid_id ( nm ) ) ;
#line 1057 "syntax1.c"
					}
					/* END OF ACTION: id_anon */
					/* BEGINNING OF ACTION: type_class_begin */
					{
#line 1077 "syntax.act"

    (ZIp) = begin_class_defn ( (ZIid), (ZIkey), cinfo_none, NULL_type ) ;
    (ZIf) = in_function_defn ;
    in_function_defn = 0 ;
    in_class_defn++ ;
    really_in_class_defn++ ;
    no_type_defns++ ;
    end_base_class ( crt_class, 1 ) ;
#line 1071 "syntax1.c"
					}
					/* END OF ACTION: type_class_begin */
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
					ZRmember_Hspecification_Hopt ();
					/* BEGINNING OF INLINE: close-brace */
					{
						if ((CURRENT_TERMINAL) == 355) {
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
					/* BEGINNING OF ACTION: type_class_end */
					{
#line 1087 "syntax.act"

    (ZItid) = end_class_defn ( (ZIp) ) ;
    in_function_defn = (ZIf) ;
    in_class_defn-- ;
    really_in_class_defn-- ;
#line 1113 "syntax1.c"
					}
					/* END OF ACTION: type_class_end */
					/* BEGINNING OF ACTION: type_name */
					{
#line 960 "syntax.act"

    MAKE_type_pre ( cv_none, btype_alias, qual_none, (ZIt) ) ;
    COPY_id ( type_name ( (ZIt) ), (ZItid) ) ;
    have_type_specifier = 1 ;
#line 1123 "syntax1.c"
					}
					/* END OF ACTION: type_name */
					/* BEGINNING OF ACTION: rescan_token */
					{
#line 1466 "syntax.act"

    RESCAN_LEXER ;
#line 1131 "syntax1.c"
					}
					/* END OF ACTION: rescan_token */
				}
				break;
			case 355:
				RESTORE_LEXER;
				goto ZL1;
			default:
				goto ZL1;
			}
		}
		/* END OF INLINE: 673 */
	}
	goto ZL0;
ZL1:;
	SAVE_LEXER (355);
	return;
ZL0:;
	*ZOt = ZIt;
}

void
ZR1026(EXP *ZIa, SID_LIST_EXP *ZOp)
{
	SID_LIST_EXP ZIp;

	switch (CURRENT_TERMINAL) {
	case 42:
		{
			SID_LIST_EXP ZIq;

			ADVANCE_LEXER;
			ZRcomma_Hexpression_Htail (&ZIq);
			if ((CURRENT_TERMINAL) == 355) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: list_exp_cons */
			{
#line 418 "syntax.act"

    CONS_exp ( (*ZIa), (ZIq), (ZIp) ) ;
#line 1174 "syntax1.c"
			}
			/* END OF ACTION: list_exp_cons */
		}
		break;
	default:
		{
			SID_LIST_EXP ZIq;

			/* BEGINNING OF ACTION: list_exp_null */
			{
#line 414 "syntax.act"

    (ZIq) = NULL_list ( EXP ) ;
#line 1188 "syntax1.c"
			}
			/* END OF ACTION: list_exp_null */
			/* BEGINNING OF ACTION: list_exp_cons */
			{
#line 418 "syntax.act"

    CONS_exp ( (*ZIa), (ZIq), (ZIp) ) ;
#line 1196 "syntax1.c"
			}
			/* END OF ACTION: list_exp_cons */
		}
		break;
	case 355:
		return;
	}
	goto ZL0;
ZL1:;
	SAVE_LEXER (355);
	return;
ZL0:;
	*ZOp = ZIp;
}

void
ZRfield_Hid_Hexpression(NAMESPACE ZIns, IDENTIFIER *ZOid)
{
	IDENTIFIER ZIid;

	if ((CURRENT_TERMINAL) == 355) {
		return;
	}
	{
		IDENTIFIER ZIuid;

		/* BEGINNING OF INLINE: any-identifier */
		{
			switch (CURRENT_TERMINAL) {
			case 1:
				{
					/* BEGINNING OF EXTRACT: identifier */
					{
#line 197 "syntax.act"

    ZIuid = crt_token->pp_data.id.use ;
#line 1233 "syntax1.c"
					}
					/* END OF EXTRACT: identifier */
					ADVANCE_LEXER;
				}
				break;
			case 4:
				{
					/* BEGINNING OF EXTRACT: statement-name */
					{
#line 209 "syntax.act"

    ZIuid = crt_token->pp_data.id.use ;
#line 1246 "syntax1.c"
					}
					/* END OF EXTRACT: statement-name */
					ADVANCE_LEXER;
				}
				break;
			case 2:
				{
					/* BEGINNING OF EXTRACT: type-name */
					{
#line 201 "syntax.act"

    ZIuid = crt_token->pp_data.id.use ;
#line 1259 "syntax1.c"
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
		/* BEGINNING OF ACTION: rescan_member */
		{
#line 401 "syntax.act"

    HASHID nm = DEREF_hashid ( id_name ( (ZIuid) ) ) ;
    (ZIid) = find_qual_id ( (ZIns), nm, 1, 0 ) ;
#line 1276 "syntax1.c"
		}
		/* END OF ACTION: rescan_member */
	}
	goto ZL0;
ZL1:;
	SAVE_LEXER (355);
	return;
ZL0:;
	*ZOid = ZIid;
}

void
ZRdecl_Hspecifier(BASE_TYPE *ZObt, TYPE *ZOt, CV_SPEC *ZOcv, DECL_SPEC *ZOds)
{
	BASE_TYPE ZIbt;
	TYPE ZIt;
	CV_SPEC ZIcv;
	DECL_SPEC ZIds;

	switch (CURRENT_TERMINAL) {
	case 98: case 124:
		{
			ZRcv_Hqualifier (&ZIcv);
			if ((CURRENT_TERMINAL) == 355) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: btype_none */
			{
#line 927 "syntax.act"
 (ZIbt) = btype_none ; 
#line 1308 "syntax1.c"
			}
			/* END OF ACTION: btype_none */
			/* BEGINNING OF ACTION: type_none */
			{
#line 951 "syntax.act"

    (ZIt) = NULL_type ;
#line 1316 "syntax1.c"
			}
			/* END OF ACTION: type_none */
			/* BEGINNING OF ACTION: dspec_none */
			{
#line 1115 "syntax.act"
 (ZIds) = dspec_none ; 
#line 1323 "syntax1.c"
			}
			/* END OF ACTION: dspec_none */
		}
		break;
	case 94: case 105: case 112: case 117:
	case 120: case 137:
		{
			ZRstorage_Hclass_Hspecifier (&ZIds);
			if ((CURRENT_TERMINAL) == 355) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: dspec_check */
			{
#line 1131 "syntax.act"

    if ( have_type_specifier ) report ( crt_loc, ERR_dcl_spec_order ( (ZIds) ) ) ;
#line 1341 "syntax1.c"
			}
			/* END OF ACTION: dspec_check */
			/* BEGINNING OF ACTION: btype_none */
			{
#line 927 "syntax.act"
 (ZIbt) = btype_none ; 
#line 1348 "syntax1.c"
			}
			/* END OF ACTION: btype_none */
			/* BEGINNING OF ACTION: type_none */
			{
#line 951 "syntax.act"

    (ZIt) = NULL_type ;
#line 1356 "syntax1.c"
			}
			/* END OF ACTION: type_none */
			/* BEGINNING OF ACTION: cv_none */
			{
#line 896 "syntax.act"
 (ZIcv) = cv_none ; 
#line 1363 "syntax1.c"
			}
			/* END OF ACTION: cv_none */
		}
		break;
	case 2: case 25: case 97: case 102:
	case 104: case 106: case 110: case 111:
	case 114: case 115: case 118: case 121:
	case 122: case 123: case 156: case 185:
	case 286: case 299:
		{
			ZRtype_Hspecifier (&ZIbt, &ZIt);
			if ((CURRENT_TERMINAL) == 355) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: cv_none */
			{
#line 896 "syntax.act"
 (ZIcv) = cv_none ; 
#line 1383 "syntax1.c"
			}
			/* END OF ACTION: cv_none */
			/* BEGINNING OF ACTION: dspec_none */
			{
#line 1115 "syntax.act"
 (ZIds) = dspec_none ; 
#line 1390 "syntax1.c"
			}
			/* END OF ACTION: dspec_none */
		}
		break;
	case 355:
		return;
	default:
		goto ZL1;
	}
	goto ZL0;
ZL1:;
	SAVE_LEXER (355);
	return;
ZL0:;
	*ZObt = ZIbt;
	*ZOt = ZIt;
	*ZOcv = ZIcv;
	*ZOds = ZIds;
}

void
parse_init(IDENTIFIER ZId, EXP *ZOe)
{
	EXP ZIe;

	if ((CURRENT_TERMINAL) == 355) {
		return;
	}
	{
		ZRinitialiser_Hclause (ZId, &ZIe);
		if ((CURRENT_TERMINAL) == 355) {
			RESTORE_LEXER;
			goto ZL1;
		}
	}
	goto ZL0;
ZL1:;
	{
		/* BEGINNING OF ACTION: error_syntax */
		{
#line 1348 "syntax.act"

    /* Syntax errors */
    ERROR err = ERR_lex_parse ( crt_token ) ;
    report ( crt_loc, err ) ;
    have_syntax_error = 1 ;
#line 1437 "syntax1.c"
		}
		/* END OF ACTION: error_syntax */
		/* BEGINNING OF ACTION: exp_none */
		{
#line 433 "syntax.act"

    (ZIe) = NULL_exp ;
#line 1445 "syntax1.c"
		}
		/* END OF ACTION: exp_none */
	}
ZL0:;
	*ZOe = ZIe;
}

void
ZR1027(EXP *ZOe)
{
	EXP ZIe;

	switch (CURRENT_TERMINAL) {
	case 298:
		{
			EXP ZIa;

			ADVANCE_LEXER;
			ZRexpression (&ZIa);
			if ((CURRENT_TERMINAL) == 355) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: exp_set */
			{
#line 608 "syntax.act"

    (ZIe) = make_set_exp ( (ZIa) ) ;
#line 1474 "syntax1.c"
			}
			/* END OF ACTION: exp_set */
			switch (CURRENT_TERMINAL) {
			case 347:
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
			ZRexpression (&ZIa);
			if ((CURRENT_TERMINAL) == 355) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: exp_unused */
			{
#line 620 "syntax.act"

    (ZIe) = make_unused_exp ( (ZIa) ) ;
#line 1501 "syntax1.c"
			}
			/* END OF ACTION: exp_unused */
			switch (CURRENT_TERMINAL) {
			case 347:
				break;
			default:
				goto ZL1;
			}
			ADVANCE_LEXER;
		}
		break;
	case 355:
		return;
	default:
		goto ZL1;
	}
	goto ZL0;
ZL1:;
	SAVE_LEXER (355);
	return;
ZL0:;
	*ZOe = ZIe;
}

void
ZR900(EXP ZI898, EXP *ZO899)
{
	EXP ZI899;

ZL2_900:;
	switch (CURRENT_TERMINAL) {
	case 86: case 87:
		{
			int ZIop;
			EXP ZIb;
			EXP ZIe;

			/* BEGINNING OF ACTION: lex_crt */
			{
#line 367 "syntax.act"
 (ZIop) = crt_lex_token ; 
#line 1543 "syntax1.c"
			}
			/* END OF ACTION: lex_crt */
			/* BEGINNING OF INLINE: maxmin-operator */
			{
				switch (CURRENT_TERMINAL) {
				case 86:
					{
						ADVANCE_LEXER;
					}
					break;
				case 87:
					{
						ADVANCE_LEXER;
					}
					break;
				default:
					goto ZL1;
				}
			}
			/* END OF INLINE: maxmin-operator */
			ZRequality_Hexpression (&ZIb);
			if ((CURRENT_TERMINAL) == 355) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: exp_maxmin */
			{
#line 542 "syntax.act"

    (ZIe) = make_mult_exp ( (ZIop), (ZI898), (ZIb) ) ;
#line 1574 "syntax1.c"
			}
			/* END OF ACTION: exp_maxmin */
			/* BEGINNING OF INLINE: 900 */
			ZI898 = ZIe;
			goto ZL2_900;
			/* END OF INLINE: 900 */
		}
		UNREACHED;
	default:
		{
			ZI899 = ZI898;
		}
		break;
	case 355:
		return;
	}
	goto ZL0;
ZL1:;
	SAVE_LEXER (355);
	return;
ZL0:;
	*ZO899 = ZI899;
}

void
ZRdeclaration_Hseq_Hopt(void)
{
	if ((CURRENT_TERMINAL) == 355) {
		return;
	}
ZL2_declaration_Hseq_Hopt:;
	{
		BASE_TYPE ZI939;
		TYPE ZI940;
		CV_SPEC ZI941;
		DECL_SPEC ZI942;
		BASE_TYPE ZI943;
		TYPE ZI944;
		CV_SPEC ZI945;
		DECL_SPEC ZI946;
		EXP ZIe;

		/* BEGINNING OF ACTION: is_decl_specifier */
		{
#line 1422 "syntax.act"

    /* Resolve declaration-specifiers from other declarators */
    (ZI0) = predict_dspec ( 0 ) ;
#line 1623 "syntax1.c"
		}
		/* END OF ACTION: is_decl_specifier */
		if (!ZI0)
			goto ZL3;
		ZRdecl_Hspecifier (&ZI939, &ZI940, &ZI941, &ZI942);
		if ((CURRENT_TERMINAL) == 355) {
			RESTORE_LEXER;
			goto ZL1;
		}
		/* BEGINNING OF ACTION: check_decl_specifier */
		{
#line 1473 "syntax.act"

    /* A type-name can be a declarator-id */
    if ( have_type_specifier && crt_lex_token == lex_type_Hname ) {
	crt_lex_token = lex_identifier ;
    }
#line 1641 "syntax1.c"
		}
		/* END OF ACTION: check_decl_specifier */
		ZR726 (&ZI939, &ZI940, &ZI941, &ZI942, &ZI943, &ZI944, &ZI945, &ZI946);
		ZR947 (&ZI943, &ZI944, &ZI945, &ZI946, &ZIe);
		if ((CURRENT_TERMINAL) == 355) {
			RESTORE_LEXER;
			goto ZL1;
		}
		/* BEGINNING OF ACTION: declare_extern */
		{
#line 1237 "syntax.act"

    external_declaration ( (ZIe), 1 ) ;
#line 1655 "syntax1.c"
		}
		/* END OF ACTION: declare_extern */
		/* BEGINNING OF INLINE: declaration-seq-opt */
		goto ZL2_declaration_Hseq_Hopt;
		/* END OF INLINE: declaration-seq-opt */
	}
	UNREACHED;
ZL3:;
	switch (CURRENT_TERMINAL) {
	case 77:
		{
			ADVANCE_LEXER;
			/* BEGINNING OF ACTION: declare_empty */
			{
#line 1182 "syntax.act"

    report ( crt_loc, ERR_dcl_dcl_semicolon () ) ;
#line 1673 "syntax1.c"
			}
			/* END OF ACTION: declare_empty */
			/* BEGINNING OF INLINE: declaration-seq-opt */
			goto ZL2_declaration_Hseq_Hopt;
			/* END OF INLINE: declaration-seq-opt */
		}
		UNREACHED;
	case 126:
		{
			EXP ZIe;

			ZRasm_Hdefinition (&ZIe);
			if ((CURRENT_TERMINAL) == 355) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: declare_extern */
			{
#line 1237 "syntax.act"

    external_declaration ( (ZIe), 1 ) ;
#line 1695 "syntax1.c"
			}
			/* END OF ACTION: declare_extern */
			/* BEGINNING OF INLINE: declaration-seq-opt */
			goto ZL2_declaration_Hseq_Hopt;
			/* END OF INLINE: declaration-seq-opt */
		}
		UNREACHED;
	case 26: case 30:
		{
			ZRdeclaration_Hcond ();
			/* BEGINNING OF INLINE: declaration-seq-opt */
			if ((CURRENT_TERMINAL) == 355) {
				RESTORE_LEXER;
				goto ZL1;
			} else {
				goto ZL2_declaration_Hseq_Hopt;
			}
			/* END OF INLINE: declaration-seq-opt */
		}
		UNREACHED;
	case 1: case 2: case 4: case 65:
	case 78:
		{
			BASE_TYPE ZIbt;
			TYPE ZIt1;
			CV_SPEC ZIcv;
			DECL_SPEC ZIds1;
			TYPE ZIt;
			DECL_SPEC ZIds;
			TYPE ZIs;
			IDENTIFIER ZIid;
			EXP ZIe;

			/* BEGINNING OF ACTION: btype_none */
			{
#line 927 "syntax.act"
 (ZIbt) = btype_none ; 
#line 1733 "syntax1.c"
			}
			/* END OF ACTION: btype_none */
			/* BEGINNING OF ACTION: type_none */
			{
#line 951 "syntax.act"

    (ZIt1) = NULL_type ;
#line 1741 "syntax1.c"
			}
			/* END OF ACTION: type_none */
			/* BEGINNING OF ACTION: cv_none */
			{
#line 896 "syntax.act"
 (ZIcv) = cv_none ; 
#line 1748 "syntax1.c"
			}
			/* END OF ACTION: cv_none */
			/* BEGINNING OF ACTION: dspec_none */
			{
#line 1115 "syntax.act"
 (ZIds1) = dspec_none ; 
#line 1755 "syntax1.c"
			}
			/* END OF ACTION: dspec_none */
			/* BEGINNING OF ACTION: dspec_complete */
			{
#line 1136 "syntax.act"

    /* Complete a declaration specifier and a type */
    (ZIds) = complete_dspec ( (ZIds1), (ZIbt), (ZIt1), (ZIcv) ) ;
    (ZIt) = complete_pre_type ( (ZIbt), (ZIt1), (ZIcv), 0 ) ;
    have_type_specifier = 0 ;
#line 1766 "syntax1.c"
			}
			/* END OF ACTION: dspec_complete */
			ZRdeclarator (ZIt, &ZIs, &ZIid);
			ZR821 (&ZIbt, &ZIt, &ZIds, &ZIs, &ZIid);
			if ((CURRENT_TERMINAL) == 355) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: exp_none */
			{
#line 433 "syntax.act"

    (ZIe) = NULL_exp ;
#line 1780 "syntax1.c"
			}
			/* END OF ACTION: exp_none */
			/* BEGINNING OF ACTION: declare_extern */
			{
#line 1237 "syntax.act"

    external_declaration ( (ZIe), 1 ) ;
#line 1788 "syntax1.c"
			}
			/* END OF ACTION: declare_extern */
			/* BEGINNING OF INLINE: declaration-seq-opt */
			goto ZL2_declaration_Hseq_Hopt;
			/* END OF INLINE: declaration-seq-opt */
		}
		UNREACHED;
	default:
		break;
	}
	return;
ZL1:;
	SAVE_LEXER (355);
	return;
}

void
ZR1028(BASE_TYPE *ZIkey, IDENTIFIER *ZIid, TYPE *ZOt)
{
	TYPE ZIt;

	switch (CURRENT_TERMINAL) {
	case 64:
		{
			IDENTIFIER ZIp;
			int ZIf;
			IDENTIFIER ZItid;

			/* BEGINNING OF ACTION: type_class_begin */
			{
#line 1077 "syntax.act"

    (ZIp) = begin_class_defn ( (*ZIid), (*ZIkey), cinfo_none, NULL_type ) ;
    (ZIf) = in_function_defn ;
    in_function_defn = 0 ;
    in_class_defn++ ;
    really_in_class_defn++ ;
    no_type_defns++ ;
    end_base_class ( crt_class, 1 ) ;
#line 1828 "syntax1.c"
			}
			/* END OF ACTION: type_class_begin */
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
			ZRmember_Hspecification_Hopt ();
			/* BEGINNING OF INLINE: close-brace */
			{
				if ((CURRENT_TERMINAL) == 355) {
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
			/* BEGINNING OF ACTION: type_class_end */
			{
#line 1087 "syntax.act"

    (ZItid) = end_class_defn ( (ZIp) ) ;
    in_function_defn = (ZIf) ;
    in_class_defn-- ;
    really_in_class_defn-- ;
#line 1870 "syntax1.c"
			}
			/* END OF ACTION: type_class_end */
			/* BEGINNING OF ACTION: type_name */
			{
#line 960 "syntax.act"

    MAKE_type_pre ( cv_none, btype_alias, qual_none, (ZIt) ) ;
    COPY_id ( type_name ( (ZIt) ), (ZItid) ) ;
    have_type_specifier = 1 ;
#line 1880 "syntax1.c"
			}
			/* END OF ACTION: type_name */
			/* BEGINNING OF ACTION: rescan_token */
			{
#line 1466 "syntax.act"

    RESCAN_LEXER ;
#line 1888 "syntax1.c"
			}
			/* END OF ACTION: rescan_token */
		}
		break;
	default:
		{
			/* BEGINNING OF ACTION: type_elaborate */
			{
#line 966 "syntax.act"

    MAKE_type_pre ( cv_none, (*ZIkey), qual_none, (ZIt) ) ;
    COPY_id ( type_name ( (ZIt) ), (*ZIid) ) ;
    if ( have_type_declaration == TYPE_DECL_NONE ) {
	have_type_declaration = TYPE_DECL_ELABORATE ;
    }
    have_type_specifier = 1 ;
#line 1905 "syntax1.c"
			}
			/* END OF ACTION: type_elaborate */
		}
		break;
	case 355:
		return;
	}
	goto ZL0;
ZL1:;
	SAVE_LEXER (355);
	return;
ZL0:;
	*ZOt = ZIt;
}

void
ZRabstract_Hdeclarator_Htail(TYPE *ZOt)
{
	TYPE ZIt;

	switch (CURRENT_TERMINAL) {
	case 65:
		{
			IDENTIFIER ZIid;
			TYPE ZIs;

			ADVANCE_LEXER;
			/* BEGINNING OF ACTION: id_none */
			{
#line 384 "syntax.act"

    (ZIid) = NULL_id ;
#line 1938 "syntax1.c"
			}
			/* END OF ACTION: id_none */
			/* BEGINNING OF ACTION: param_begin */
			{
#line 875 "syntax.act"

    func_type_defn ( 0 ) ;
    begin_param ( (ZIid) ) ;
    have_type_declaration = TYPE_DECL_NONE ;
    have_func_declarator = 0 ;
#line 1949 "syntax1.c"
			}
			/* END OF ACTION: param_begin */
			ZR757 (&ZIs);
			switch (CURRENT_TERMINAL) {
			case 39:
				break;
			case 355:
				RESTORE_LEXER;
				goto ZL1;
			default:
				goto ZL1;
			}
			ADVANCE_LEXER;
			ZIt = ZIs;
			/* BEGINNING OF ACTION: param_end */
			{
#line 884 "syntax.act"

    end_param () ;
    have_type_declaration = TYPE_DECL_NONE ;
    have_func_declarator = 1 ;
#line 1971 "syntax1.c"
			}
			/* END OF ACTION: param_end */
		}
		break;
	case 332:
		{
			IDENTIFIER ZIid;
			int ZIell;

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
#line 384 "syntax.act"

    (ZIid) = NULL_id ;
#line 1994 "syntax1.c"
			}
			/* END OF ACTION: id_none */
			/* BEGINNING OF ACTION: param_begin */
			{
#line 875 "syntax.act"

    func_type_defn ( 0 ) ;
    begin_param ( (ZIid) ) ;
    have_type_declaration = TYPE_DECL_NONE ;
    have_func_declarator = 0 ;
#line 2005 "syntax1.c"
			}
			/* END OF ACTION: param_begin */
			ZR760 (&ZIell);
			if ((CURRENT_TERMINAL) == 355) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: type_func_weak */
			{
#line 1019 "syntax.act"

    (ZIt) = make_func_type ( NULL_type, ( (ZIell) | FUNC_WEAK ), cv_c, empty_type_set ) ;
#line 2018 "syntax1.c"
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
#line 884 "syntax.act"

    end_param () ;
    have_type_declaration = TYPE_DECL_NONE ;
    have_func_declarator = 1 ;
#line 2035 "syntax1.c"
			}
			/* END OF ACTION: param_end */
		}
		break;
	case 66:
		{
			EXP ZIe;

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
			ZR759 (&ZIe);
			if ((CURRENT_TERMINAL) == 355) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: type_array */
			{
#line 1031 "syntax.act"

    NAT n = make_array_dim ( (ZIe) ) ;
    MAKE_type_array ( cv_none, NULL_type, n, (ZIt) ) ;
#line 2068 "syntax1.c"
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
		}
		break;
	case 355:
		return;
	default:
		goto ZL1;
	}
	goto ZL0;
ZL1:;
	SAVE_LEXER (355);
	return;
ZL0:;
	*ZOt = ZIt;
}

void
ZR1029(BASE_TYPE *ZIbt, TYPE *ZIp, CV_SPEC *ZIcv)
{
	switch (CURRENT_TERMINAL) {
	case 77:
		{
			/* BEGINNING OF ACTION: declare_member_empty */
			{
#line 1204 "syntax.act"

    IGNORE empty_decl ( dspec_none, NULL_type, (*ZIbt), (*ZIp), (*ZIcv), last_lex_token, 1 ) ;
    have_type_declaration = TYPE_DECL_NONE ;
    have_func_declarator = 0 ;
    have_type_specifier = 0 ;
#line 2113 "syntax1.c"
			}
			/* END OF ACTION: declare_member_empty */
			ADVANCE_LEXER;
		}
		break;
	case 1: case 2: case 4: case 41:
	case 65: case 78:
		{
			TYPE ZIt;

			/* BEGINNING OF ACTION: type_complete */
			{
#line 987 "syntax.act"

    (ZIt) = complete_pre_type ( (*ZIbt), (*ZIp), (*ZIcv), 1 ) ;
    have_type_specifier = 0 ;
#line 2130 "syntax1.c"
			}
			/* END OF ACTION: type_complete */
			ZRmember_Hdeclarator (ZIt, *ZIbt);
			/* BEGINNING OF INLINE: 830 */
			{
				switch (CURRENT_TERMINAL) {
				case 42:
					{
						ADVANCE_LEXER;
						ZRmember_Hdeclarator_Hlist (ZIt, *ZIbt);
						switch (CURRENT_TERMINAL) {
						case 77:
							break;
						case 355:
							RESTORE_LEXER;
							goto ZL1;
						default:
							goto ZL1;
						}
						ADVANCE_LEXER;
					}
					break;
				case 77:
					{
						ADVANCE_LEXER;
					}
					break;
				case 355:
					RESTORE_LEXER;
					goto ZL1;
				default:
					goto ZL1;
				}
			}
			/* END OF INLINE: 830 */
		}
		break;
	case 355:
		return;
	default:
		goto ZL1;
	}
	return;
ZL1:;
	SAVE_LEXER (355);
	return;
}

void
ZRscoped_Hstatement(EXP ZIc, EXP *ZOe)
{
	EXP ZIe;

	if ((CURRENT_TERMINAL) == 355) {
		return;
	}
	{
		EXP ZIa;

		ZRscoped_Hstmt_Hbody (ZIc, &ZIa);
		if ((CURRENT_TERMINAL) == 355) {
			RESTORE_LEXER;
			goto ZL1;
		}
		/* BEGINNING OF ACTION: stmt_compound_end */
		{
#line 671 "syntax.act"

    (ZIe) = end_compound_stmt ( (ZIa) ) ;
#line 2200 "syntax1.c"
		}
		/* END OF ACTION: stmt_compound_end */
		/* BEGINNING OF ACTION: rescan_token */
		{
#line 1466 "syntax.act"

    RESCAN_LEXER ;
#line 2208 "syntax1.c"
		}
		/* END OF ACTION: rescan_token */
	}
	goto ZL0;
ZL1:;
	{
		/* BEGINNING OF ACTION: error_syntax */
		{
#line 1348 "syntax.act"

    /* Syntax errors */
    ERROR err = ERR_lex_parse ( crt_token ) ;
    report ( crt_loc, err ) ;
    have_syntax_error = 1 ;
#line 2223 "syntax1.c"
		}
		/* END OF ACTION: error_syntax */
		/* BEGINNING OF ACTION: stmt_compound_end */
		{
#line 671 "syntax.act"

    (ZIe) = end_compound_stmt ( (ZIc) ) ;
#line 2231 "syntax1.c"
		}
		/* END OF ACTION: stmt_compound_end */
		/* BEGINNING OF ACTION: rescan_token */
		{
#line 1466 "syntax.act"

    RESCAN_LEXER ;
#line 2239 "syntax1.c"
		}
		/* END OF ACTION: rescan_token */
	}
ZL0:;
	*ZOe = ZIe;
}

void
ZRinit_Hdeclarator(DECL_SPEC ZIds, BASE_TYPE ZIbt, TYPE ZIp)
{
	if ((CURRENT_TERMINAL) == 355) {
		return;
	}
	{
		TYPE ZIt;
		IDENTIFIER ZIid;
		IDENTIFIER ZId;
		EXP ZIe;

		ZRdeclarator (ZIp, &ZIt, &ZIid);
		if ((CURRENT_TERMINAL) == 355) {
			RESTORE_LEXER;
			goto ZL1;
		}
		/* BEGINNING OF ACTION: declare_id */
		{
#line 1150 "syntax.act"

    if ( in_weak_param ) {
	(ZId) = make_param_decl ( (ZIds), (ZIt), (ZIid), CONTEXT_WEAK_PARAM ) ;
    } else if ( type_tag ( (ZIt) ) == type_func_tag ) {
	check_weak_func ( (ZIt), 0 ) ;
	(ZId) = make_func_decl ( (ZIds), (ZIt), (ZIid), 0 ) ;
    } else {
	int def = predict_obj_defn () ;
	(ZId) = make_object_decl ( (ZIds), (ZIt), (ZIid), def ) ;
    }
    if ( IS_id_type_alias ( (ZId) ) ) {
	BASE_TYPE bs = DEREF_btype ( id_type_alias_rep ( (ZId) ) ) ;
	bs |= (ZIbt) ;
	COPY_btype ( id_type_alias_rep ( (ZId) ), bs ) ;
    }
    have_type_declaration = TYPE_DECL_NONE ;
    have_func_declarator = 0 ;
#line 2284 "syntax1.c"
		}
		/* END OF ACTION: declare_id */
		ZRinitialiser_Hopt (ZId, &ZIe);
		if ((CURRENT_TERMINAL) == 355) {
			RESTORE_LEXER;
			goto ZL1;
		}
		/* BEGINNING OF ACTION: initialise_id */
		{
#line 1255 "syntax.act"

    int def = init_object ( (ZId), (ZIe) ) ;
    if ( do_dump ) dump_declare ( (ZId), &decl_loc, def ) ;
#line 2298 "syntax1.c"
		}
		/* END OF ACTION: initialise_id */
	}
	return;
ZL1:;
	SAVE_LEXER (355);
	return;
}

void
ZR904(EXP ZI902, EXP *ZO903)
{
	EXP ZI903;

ZL2_904:;
	switch (CURRENT_TERMINAL) {
	case 48: case 63:
		{
			int ZIop;
			EXP ZIb;
			EXP ZIe;

			/* BEGINNING OF ACTION: lex_crt */
			{
#line 367 "syntax.act"
 (ZIop) = crt_lex_token ; 
#line 2325 "syntax1.c"
			}
			/* END OF ACTION: lex_crt */
			ZRequality_Hoperator ();
			ZRrelational_Hexpression (&ZIb);
			if ((CURRENT_TERMINAL) == 355) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: exp_equality */
			{
#line 493 "syntax.act"

    /* op will be in its primary form */
    (ZIe) = make_equality_exp ( (ZIop), (ZI902), (ZIb) ) ;
#line 2340 "syntax1.c"
			}
			/* END OF ACTION: exp_equality */
			/* BEGINNING OF INLINE: 904 */
			ZI902 = ZIe;
			goto ZL2_904;
			/* END OF INLINE: 904 */
		}
		UNREACHED;
	default:
		{
			ZI903 = ZI902;
		}
		break;
	case 355:
		return;
	}
	goto ZL0;
ZL1:;
	SAVE_LEXER (355);
	return;
ZL0:;
	*ZO903 = ZI903;
}

void
ZRdeclaration(void)
{
	if ((CURRENT_TERMINAL) == 355) {
		return;
	}
	{
		BASE_TYPE ZIb1;
		TYPE ZI1097;
		CV_SPEC ZIcv1;
		DECL_SPEC ZI1098;
		BASE_TYPE ZIbt;
		TYPE ZIt1;
		CV_SPEC ZIcv;
		DECL_SPEC ZIds1;

		/* BEGINNING OF ACTION: is_decl_specifier */
		{
#line 1422 "syntax.act"

    /* Resolve declaration-specifiers from other declarators */
    (ZI0) = predict_dspec ( 0 ) ;
#line 2387 "syntax1.c"
		}
		/* END OF ACTION: is_decl_specifier */
		if (!ZI0)
			goto ZL1;
		ZRdecl_Hspecifier (&ZIb1, &ZI1097, &ZIcv1, &ZI1098);
		if ((CURRENT_TERMINAL) == 355) {
			RESTORE_LEXER;
			goto ZL1;
		}
		/* BEGINNING OF ACTION: check_decl_specifier */
		{
#line 1473 "syntax.act"

    /* A type-name can be a declarator-id */
    if ( have_type_specifier && crt_lex_token == lex_type_Hname ) {
	crt_lex_token = lex_identifier ;
    }
#line 2405 "syntax1.c"
		}
		/* END OF ACTION: check_decl_specifier */
		ZR726 (&ZIb1, &ZI1097, &ZIcv1, &ZI1098, &ZIbt, &ZIt1, &ZIcv, &ZIds1);
		ZR815 (&ZIbt, &ZIt1, &ZIcv, &ZIds1);
		switch (CURRENT_TERMINAL) {
		case 77:
			break;
		case 355:
			RESTORE_LEXER;
			goto ZL1;
		default:
			goto ZL1;
		}
		ADVANCE_LEXER;
	}
	return;
ZL1:;
	SAVE_LEXER (355);
	return;
}

void
ZRprimary_Hexpression(EXP *ZOe)
{
	EXP ZIe;

	switch (CURRENT_TERMINAL) {
	case 23:
		{
			/* BEGINNING OF EXTRACT: complex-exp */
			{
#line 333 "syntax.act"

    IDENTIFIER id = crt_token->pp_data.tok.id ;
    PPTOKEN *args = crt_token->pp_data.tok.args ;
    ZIe = parse_exp_token ( id, args ) ;
    RESCAN_LEXER ;
#line 2443 "syntax1.c"
			}
			/* END OF EXTRACT: complex-exp */
			ADVANCE_LEXER;
		}
		break;
	case 210:
		{
			ADVANCE_LEXER;
			/* BEGINNING OF ACTION: exp_ellipsis */
			{
#line 488 "syntax.act"

    (ZIe) = make_ellipsis_exp () ;
#line 2457 "syntax1.c"
			}
			/* END OF ACTION: exp_ellipsis */
		}
		break;
	case 1:
		{
			IDENTIFIER ZIid;

			/* BEGINNING OF EXTRACT: identifier */
			{
#line 197 "syntax.act"

    ZIid = crt_token->pp_data.id.use ;
#line 2471 "syntax1.c"
			}
			/* END OF EXTRACT: identifier */
			ADVANCE_LEXER;
			/* BEGINNING OF ACTION: exp_identifier */
			{
#line 506 "syntax.act"

    (ZIe) = make_id_exp ( (ZIid) ) ;
#line 2480 "syntax1.c"
			}
			/* END OF ACTION: exp_identifier */
		}
		break;
	case 65:
		{
			EXP ZIa;

			ADVANCE_LEXER;
			/* BEGINNING OF ACTION: exp_paren_begin */
			{
#line 563 "syntax.act"

    IGNORE incr_value ( OPT_VAL_paren_depth ) ;
#line 2495 "syntax1.c"
			}
			/* END OF ACTION: exp_paren_begin */
			ZRexpression (&ZIa);
			if ((CURRENT_TERMINAL) == 355) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: exp_paren_end */
			{
#line 566 "syntax.act"

    (ZIe) = make_paren_exp ( (ZIa) ) ;
    decr_value ( OPT_VAL_paren_depth ) ;
#line 2509 "syntax1.c"
			}
			/* END OF ACTION: exp_paren_end */
			switch (CURRENT_TERMINAL) {
			case 39:
				break;
			default:
				goto ZL1;
			}
			ADVANCE_LEXER;
		}
		break;
	case 17: case 18: case 19: case 20:
	case 21: case 22:
		{
			ZRliteral (&ZIe);
			if ((CURRENT_TERMINAL) == 355) {
				RESTORE_LEXER;
				goto ZL1;
			}
		}
		break;
	case 355:
		return;
	default:
		goto ZL1;
	}
	goto ZL0;
ZL1:;
	SAVE_LEXER (355);
	return;
ZL0:;
	*ZOe = ZIe;
}

void
ZRcontrol_Hstatement(EXP *ZOe)
{
	EXP ZIe;

	switch (CURRENT_TERMINAL) {
	case 289:
		{
			EXP ZIa;

			ADVANCE_LEXER;
			/* BEGINNING OF ACTION: reach_set */
			{
#line 842 "syntax.act"
 unreached_code = 0 ; 
#line 2559 "syntax1.c"
			}
			/* END OF ACTION: reach_set */
			ZRstatement (&ZIa);
			if ((CURRENT_TERMINAL) == 355) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: stmt_reach */
			{
#line 811 "syntax.act"

    (ZIe) = make_reach_stmt ( (ZIa), 1 ) ;
#line 2572 "syntax1.c"
			}
			/* END OF ACTION: stmt_reach */
		}
		break;
	case 323:
		{
			EXP ZIa;

			ADVANCE_LEXER;
			/* BEGINNING OF ACTION: reach_unset */
			{
#line 843 "syntax.act"
 unreached_code = 1 ; 
#line 2586 "syntax1.c"
			}
			/* END OF ACTION: reach_unset */
			ZRstatement (&ZIa);
			if ((CURRENT_TERMINAL) == 355) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: stmt_unreach */
			{
#line 815 "syntax.act"

    (ZIe) = make_reach_stmt ( (ZIa), 0 ) ;
#line 2599 "syntax1.c"
			}
			/* END OF ACTION: stmt_unreach */
		}
		break;
	case 355:
		return;
	default:
		goto ZL1;
	}
	goto ZL0;
ZL1:;
	SAVE_LEXER (355);
	return;
ZL0:;
	*ZOe = ZIe;
}

void
ZR908(EXP ZI906, EXP *ZO907)
{
	EXP ZI907;

ZL2_908:;
	switch (CURRENT_TERMINAL) {
	case 49: case 50: case 53: case 54:
		{
			int ZIop;
			EXP ZIb;
			EXP ZIe;

			/* BEGINNING OF ACTION: lex_crt */
			{
#line 367 "syntax.act"
 (ZIop) = crt_lex_token ; 
#line 2634 "syntax1.c"
			}
			/* END OF ACTION: lex_crt */
			/* BEGINNING OF INLINE: relational-operator */
			{
				switch (CURRENT_TERMINAL) {
				case 49:
					{
						ADVANCE_LEXER;
					}
					break;
				case 50:
					{
						ADVANCE_LEXER;
					}
					break;
				case 53:
					{
						ADVANCE_LEXER;
					}
					break;
				case 54:
					{
						ADVANCE_LEXER;
					}
					break;
				default:
					goto ZL1;
				}
			}
			/* END OF INLINE: relational-operator */
			ZRshift_Hexpression (&ZIb);
			if ((CURRENT_TERMINAL) == 355) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: exp_relation */
			{
#line 596 "syntax.act"

    /* op will be in its primary form */
    (ZIe) = make_relation_exp ( (ZIop), (ZI906), (ZIb) ) ;
#line 2676 "syntax1.c"
			}
			/* END OF ACTION: exp_relation */
			/* BEGINNING OF INLINE: 908 */
			ZI906 = ZIe;
			goto ZL2_908;
			/* END OF INLINE: 908 */
		}
		UNREACHED;
	default:
		{
			ZI907 = ZI906;
		}
		break;
	case 355:
		return;
	}
	goto ZL0;
ZL1:;
	SAVE_LEXER (355);
	return;
ZL0:;
	*ZO907 = ZI907;
}

void
ZRclass_Hkey(BASE_TYPE *ZOkey)
{
	BASE_TYPE ZIkey;

	switch (CURRENT_TERMINAL) {
	case 118:
		{
			ADVANCE_LEXER;
			/* BEGINNING OF ACTION: key_struct */
			{
#line 1063 "syntax.act"
 (ZIkey) = btype_struct ; 
#line 2714 "syntax1.c"
			}
			/* END OF ACTION: key_struct */
		}
		break;
	case 121:
		{
			ADVANCE_LEXER;
			/* BEGINNING OF ACTION: key_union */
			{
#line 1064 "syntax.act"
 (ZIkey) = btype_union ; 
#line 2726 "syntax1.c"
			}
			/* END OF ACTION: key_union */
		}
		break;
	case 355:
		return;
	default:
		goto ZL1;
	}
	goto ZL0;
ZL1:;
	SAVE_LEXER (355);
	return;
ZL0:;
	*ZOkey = ZIkey;
}

void
ZRinitialiser_Hlist(IDENTIFIER ZId, SID_LIST_EXP *ZOp)
{
	SID_LIST_EXP ZIp;

	if ((CURRENT_TERMINAL) == 355) {
		return;
	}
	{
		EXP ZIb;
		EXP ZIa;
		SID_LIST_EXP ZIq;

		ZRinitialiser_Hclause (ZId, &ZIb);
		if ((CURRENT_TERMINAL) == 355) {
			RESTORE_LEXER;
			goto ZL1;
		}
		/* BEGINNING OF ACTION: exp_location */
		{
#line 526 "syntax.act"

    MAKE_exp_location ( type_void, crt_loc, (ZIb), (ZIa) ) ;
#line 2767 "syntax1.c"
		}
		/* END OF ACTION: exp_location */
		/* BEGINNING OF INLINE: 792 */
		{
			switch (CURRENT_TERMINAL) {
			case 42:
				{
					ADVANCE_LEXER;
					ZR1074 (&ZId, &ZIq);
					if ((CURRENT_TERMINAL) == 355) {
						RESTORE_LEXER;
						goto ZL1;
					}
				}
				break;
			default:
				{
					/* BEGINNING OF ACTION: list_exp_null */
					{
#line 414 "syntax.act"

    (ZIq) = NULL_list ( EXP ) ;
#line 2790 "syntax1.c"
					}
					/* END OF ACTION: list_exp_null */
				}
				break;
			}
		}
		/* END OF INLINE: 792 */
		/* BEGINNING OF ACTION: list_exp_cons */
		{
#line 418 "syntax.act"

    CONS_exp ( (ZIa), (ZIq), (ZIp) ) ;
#line 2803 "syntax1.c"
		}
		/* END OF ACTION: list_exp_cons */
	}
	goto ZL0;
ZL1:;
	SAVE_LEXER (355);
	return;
ZL0:;
	*ZOp = ZIp;
}

void
parse_tok_type(TYPE *ZOt)
{
	TYPE ZIt;

	if ((CURRENT_TERMINAL) == 355) {
		return;
	}
	{
		BASE_TYPE ZIbt;
		TYPE ZIp;
		CV_SPEC ZIcv;
		TYPE ZIq;

		ZRtype_Hspecifier_Hseq (&ZIbt, &ZIp, &ZIcv);
		if ((CURRENT_TERMINAL) == 355) {
			RESTORE_LEXER;
			goto ZL1;
		}
		/* BEGINNING OF ACTION: type_complete */
		{
#line 987 "syntax.act"

    (ZIq) = complete_pre_type ( (ZIbt), (ZIp), (ZIcv), 1 ) ;
    have_type_specifier = 0 ;
#line 2840 "syntax1.c"
		}
		/* END OF ACTION: type_complete */
		ZRabstract_Hdeclarator_Hopt (ZIq, &ZIt);
		if ((CURRENT_TERMINAL) == 355) {
			RESTORE_LEXER;
			goto ZL1;
		}
	}
	goto ZL0;
ZL1:;
	SAVE_LEXER (355);
	return;
ZL0:;
	*ZOt = ZIt;
}

void
ZRequality_Hoperator(void)
{
	switch (CURRENT_TERMINAL) {
	case 48:
		{
			ADVANCE_LEXER;
		}
		break;
	case 63:
		{
			/* BEGINNING OF INLINE: not-eq */
			{
				{
					switch (CURRENT_TERMINAL) {
					case 63:
						break;
					default:
						goto ZL1;
					}
					ADVANCE_LEXER;
				}
			}
			/* END OF INLINE: not-eq */
		}
		break;
	case 355:
		return;
	default:
		goto ZL1;
	}
	return;
ZL1:;
	SAVE_LEXER (355);
	return;
}

void
ZRmaxmin_Hexpression(EXP *ZO899)
{
	EXP ZI899;

	if ((CURRENT_TERMINAL) == 355) {
		return;
	}
	{
		EXP ZIe;

		ZRequality_Hexpression (&ZIe);
		ZR900 (ZIe, &ZI899);
		if ((CURRENT_TERMINAL) == 355) {
			RESTORE_LEXER;
			goto ZL1;
		}
	}
	goto ZL0;
ZL1:;
	SAVE_LEXER (355);
	return;
ZL0:;
	*ZO899 = ZI899;
}

void
ZRinset_Hflow_Hexpression(EXP *ZOe)
{
	EXP ZIe;

	if ((CURRENT_TERMINAL) == 355) {
		return;
	}
	{
		switch (CURRENT_TERMINAL) {
		case 346:
			break;
		default:
			goto ZL1;
		}
		ADVANCE_LEXER;
		ZR1027 (&ZIe);
		if ((CURRENT_TERMINAL) == 355) {
			RESTORE_LEXER;
			goto ZL1;
		}
	}
	goto ZL0;
ZL1:;
	SAVE_LEXER (355);
	return;
ZL0:;
	*ZOe = ZIe;
}

void
ZR912(EXP ZI910, EXP *ZO911)
{
	EXP ZI911;

ZL2_912:;
	switch (CURRENT_TERMINAL) {
	case 57:
		{
			EXP ZIb;
			EXP ZIe;

			ADVANCE_LEXER;
			ZRadditive_Hexpression (&ZIb);
			if ((CURRENT_TERMINAL) == 355) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: exp_lshift */
			{
#line 538 "syntax.act"

    (ZIe) = make_shift_exp ( lex_lshift, (ZI910), (ZIb) ) ;
#line 2973 "syntax1.c"
			}
			/* END OF ACTION: exp_lshift */
			/* BEGINNING OF INLINE: 912 */
			ZI910 = ZIe;
			goto ZL2_912;
			/* END OF INLINE: 912 */
		}
		UNREACHED;
	case 75:
		{
			EXP ZIb;
			EXP ZIe;

			ADVANCE_LEXER;
			ZRadditive_Hexpression (&ZIb);
			if ((CURRENT_TERMINAL) == 355) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: exp_rshift */
			{
#line 604 "syntax.act"

    (ZIe) = make_shift_exp ( lex_rshift, (ZI910), (ZIb) ) ;
#line 2998 "syntax1.c"
			}
			/* END OF ACTION: exp_rshift */
			/* BEGINNING OF INLINE: 912 */
			ZI910 = ZIe;
			goto ZL2_912;
			/* END OF INLINE: 912 */
		}
		UNREACHED;
	default:
		{
			ZI911 = ZI910;
		}
		break;
	case 355:
		return;
	}
	goto ZL0;
ZL1:;
	SAVE_LEXER (355);
	return;
ZL0:;
	*ZO911 = ZI911;
}

void
parse_type_param(IDENTIFIER *ZOd)
{
	IDENTIFIER ZId;

	if ((CURRENT_TERMINAL) == 355) {
		return;
	}
	{
		/* BEGINNING OF ACTION: error_syntax */
		{
#line 1348 "syntax.act"

    /* Syntax errors */
    ERROR err = ERR_lex_parse ( crt_token ) ;
    report ( crt_loc, err ) ;
    have_syntax_error = 1 ;
#line 3040 "syntax1.c"
		}
		/* END OF ACTION: error_syntax */
		/* BEGINNING OF ACTION: decl_none */
		{
#line 1241 "syntax.act"

    (ZId) = NULL_id ;
#line 3048 "syntax1.c"
		}
		/* END OF ACTION: decl_none */
	}
	*ZOd = ZId;
}

void
ZR1041(TYPE *ZIp, BASE_TYPE *ZIq, IDENTIFIER *ZI1040)
{
	switch (CURRENT_TERMINAL) {
	case 41:
		{
			EXP ZIc;
			TYPE ZIt;

			/* BEGINNING OF ACTION: declarator_begin */
			{
#line 1222 "syntax.act"

    IDENTIFIER pid = underlying_id ( (*ZI1040) ) ;
    DEREF_loc ( id_loc ( pid ), decl_loc ) ;
#line 3070 "syntax1.c"
			}
			/* END OF ACTION: declarator_begin */
			ADVANCE_LEXER;
			ZRconstant_Hexpression (&ZIc);
			if ((CURRENT_TERMINAL) == 355) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: type_bitfield_mem */
			{
#line 1041 "syntax.act"

    /* Check for anonymous bitfields */
    HASHID nm = DEREF_hashid ( id_name ( (*ZI1040) ) ) ;
    int z = IS_hashid_anon ( nm ) ;
    (ZIt) = make_bitfield_type ( (*ZIp), (*ZIq), (ZIc), z ) ;
#line 3087 "syntax1.c"
			}
			/* END OF ACTION: type_bitfield_mem */
			/* BEGINNING OF ACTION: declare_bitfield */
			{
#line 1211 "syntax.act"

    IDENTIFIER id = make_member_decl ( dspec_none, (ZIt), (*ZI1040), 0 ) ;
    if ( do_dump ) dump_declare ( id, &decl_loc, 0 ) ;
    have_type_declaration = TYPE_DECL_NONE ;
    have_func_declarator = 0 ;
#line 3098 "syntax1.c"
			}
			/* END OF ACTION: declare_bitfield */
		}
		break;
	default:
		{
			TYPE ZI1039;
			TYPE ZI1033;
			IDENTIFIER ZIid;
			TYPE ZIt;

			/* BEGINNING OF ACTION: type_none */
			{
#line 951 "syntax.act"

    (ZI1039) = NULL_type ;
#line 3115 "syntax1.c"
			}
			/* END OF ACTION: type_none */
			/* BEGINNING OF ACTION: declarator_begin */
			{
#line 1222 "syntax.act"

    IDENTIFIER pid = underlying_id ( (*ZI1040) ) ;
    DEREF_loc ( id_loc ( pid ), decl_loc ) ;
#line 3124 "syntax1.c"
			}
			/* END OF ACTION: declarator_begin */
			ZR870 (ZI1039, *ZI1040, &ZI1033, &ZIid);
			if ((CURRENT_TERMINAL) == 355) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: type_inject */
			{
#line 1047 "syntax.act"

    (ZIt) = ( IS_NULL_type ( (ZI1033) ) ? (*ZIp) : inject_pre_type ( (ZI1033), (*ZIp), 1 ) ) ;
#line 3137 "syntax1.c"
			}
			/* END OF ACTION: type_inject */
			/* BEGINNING OF ACTION: declare_member */
			{
#line 1197 "syntax.act"

    IDENTIFIER id = make_member_decl ( dspec_none, (ZIt), (ZIid), 0 ) ;
    if ( do_dump ) dump_declare ( id, &decl_loc, 0 ) ;
    have_type_declaration = TYPE_DECL_NONE ;
    have_func_declarator = 0 ;
#line 3148 "syntax1.c"
			}
			/* END OF ACTION: declare_member */
		}
		break;
	case 355:
		return;
	}
	return;
ZL1:;
	SAVE_LEXER (355);
	return;
}

void
ZRpostfix_Hexpression(EXP *ZO923)
{
	EXP ZI923;

	if ((CURRENT_TERMINAL) == 355) {
		return;
	}
	{
		EXP ZIe;

		ZRprimary_Hexpression (&ZIe);
		ZR924 (ZIe, &ZI923);
		if ((CURRENT_TERMINAL) == 355) {
			RESTORE_LEXER;
			goto ZL1;
		}
	}
	goto ZL0;
ZL1:;
	SAVE_LEXER (355);
	return;
ZL0:;
	*ZO923 = ZI923;
}

void
ZRsimple_Hstatement(EXP *ZOe)
{
	EXP ZIe;

	switch (CURRENT_TERMINAL) {
	case 170:
		{
			int ZI1008;
			EXP ZI1009;
			EXP ZI1023;
			EXP ZIa;
			int ZIr;

			ADVANCE_LEXER;
			/* BEGINNING OF ACTION: lex_alignof */
			{
#line 370 "syntax.act"
 (ZI1008) = lex_alignof ; 
#line 3207 "syntax1.c"
			}
			/* END OF ACTION: lex_alignof */
			ZRsizeof_Hexpression (ZI1008, &ZI1009);
			ZR1013 (&ZI1009, &ZI1023);
			ZR1025 (&ZI1023, &ZIa);
			if ((CURRENT_TERMINAL) == 355) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: reach_check */
			{
#line 830 "syntax.act"

    (ZIr) = unreached_code ;
    if ( (ZIr) ) {
	if ( !unreached_last ) {
	    report ( crt_loc, ERR_stmt_stmt_unreach () ) ;
	    unreached_last = 1 ;
	}
    } else {
	unreached_last = 0 ;
    }
#line 3230 "syntax1.c"
			}
			/* END OF ACTION: reach_check */
			/* BEGINNING OF ACTION: stmt_exp */
			{
#line 700 "syntax.act"

    (ZIe) = make_exp_stmt ( (ZIa) ) ;
#line 3238 "syntax1.c"
			}
			/* END OF ACTION: stmt_exp */
			/* BEGINNING OF ACTION: stmt_label_clear */
			{
#line 768 "syntax.act"

    unreached_fall = 1 ;
#line 3246 "syntax1.c"
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
	case 23:
		{
			EXP ZI1020;
			EXP ZI1021;
			EXP ZI1023;
			EXP ZIa;
			int ZIr;

			/* BEGINNING OF EXTRACT: complex-exp */
			{
#line 333 "syntax.act"

    IDENTIFIER id = crt_token->pp_data.tok.id ;
    PPTOKEN *args = crt_token->pp_data.tok.args ;
    ZI1020 = parse_exp_token ( id, args ) ;
    RESCAN_LEXER ;
#line 3274 "syntax1.c"
			}
			/* END OF EXTRACT: complex-exp */
			ADVANCE_LEXER;
			ZR924 (ZI1020, &ZI1021);
			ZR1013 (&ZI1021, &ZI1023);
			ZR1025 (&ZI1023, &ZIa);
			if ((CURRENT_TERMINAL) == 355) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: reach_check */
			{
#line 830 "syntax.act"

    (ZIr) = unreached_code ;
    if ( (ZIr) ) {
	if ( !unreached_last ) {
	    report ( crt_loc, ERR_stmt_stmt_unreach () ) ;
	    unreached_last = 1 ;
	}
    } else {
	unreached_last = 0 ;
    }
#line 3298 "syntax1.c"
			}
			/* END OF ACTION: reach_check */
			/* BEGINNING OF ACTION: stmt_exp */
			{
#line 700 "syntax.act"

    (ZIe) = make_exp_stmt ( (ZIa) ) ;
#line 3306 "syntax1.c"
			}
			/* END OF ACTION: stmt_exp */
			/* BEGINNING OF ACTION: stmt_label_clear */
			{
#line 768 "syntax.act"

    unreached_fall = 1 ;
#line 3314 "syntax1.c"
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
	case 24:
		{
			EXP ZIa;

			/* BEGINNING OF EXTRACT: complex-stmt */
			{
#line 340 "syntax.act"

    IDENTIFIER id = crt_token->pp_data.tok.id ;
    PPTOKEN *args = crt_token->pp_data.tok.args ;
    ZIa = parse_exp_token ( id, args ) ;
    RESCAN_LEXER ;
#line 3338 "syntax1.c"
			}
			/* END OF EXTRACT: complex-stmt */
			ADVANCE_LEXER;
			/* BEGINNING OF ACTION: stmt_exp */
			{
#line 700 "syntax.act"

    (ZIe) = make_exp_stmt ( (ZIa) ) ;
#line 3347 "syntax1.c"
			}
			/* END OF ACTION: stmt_exp */
		}
		break;
	case 206:
		{
			EXP ZI1083;
			EXP ZI963;
			EXP ZI962;
			EXP ZI961;
			EXP ZI960;
			EXP ZI959;
			EXP ZI958;
			EXP ZI957;
			EXP ZI956;
			EXP ZI955;
			EXP ZI954;
			EXP ZI953;
			EXP ZI950;
			EXP ZI1023;
			EXP ZIa;
			int ZIr;

			ADVANCE_LEXER;
			ZRcast_Hexpression (&ZI1083);
			if ((CURRENT_TERMINAL) == 355) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: exp_ignore */
			{
#line 510 "syntax.act"

    (ZI963) = make_cast_exp ( type_void, (ZI1083), 0 ) ;
#line 3382 "syntax1.c"
			}
			/* END OF ACTION: exp_ignore */
			ZR920 (ZI963, &ZI962);
			ZR916 (ZI962, &ZI961);
			ZR912 (ZI961, &ZI960);
			ZR908 (ZI960, &ZI959);
			ZR904 (ZI959, &ZI958);
			ZR900 (ZI958, &ZI957);
			ZR896 (ZI957, &ZI956);
			ZR892 (ZI956, &ZI955);
			ZR888 (ZI955, &ZI954);
			ZR884 (ZI954, &ZI953);
			ZR880 (ZI953, &ZI950);
			ZR952 (&ZI950, &ZI1023);
			ZR1025 (&ZI1023, &ZIa);
			if ((CURRENT_TERMINAL) == 355) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: reach_check */
			{
#line 830 "syntax.act"

    (ZIr) = unreached_code ;
    if ( (ZIr) ) {
	if ( !unreached_last ) {
	    report ( crt_loc, ERR_stmt_stmt_unreach () ) ;
	    unreached_last = 1 ;
	}
    } else {
	unreached_last = 0 ;
    }
#line 3415 "syntax1.c"
			}
			/* END OF ACTION: reach_check */
			/* BEGINNING OF ACTION: stmt_exp */
			{
#line 700 "syntax.act"

    (ZIe) = make_exp_stmt ( (ZIa) ) ;
#line 3423 "syntax1.c"
			}
			/* END OF ACTION: stmt_exp */
			/* BEGINNING OF ACTION: stmt_label_clear */
			{
#line 768 "syntax.act"

    unreached_fall = 1 ;
#line 3431 "syntax1.c"
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
	case 210:
		{
			EXP ZI1017;
			EXP ZI1018;
			EXP ZI1023;
			EXP ZIa;
			int ZIr;

			ADVANCE_LEXER;
			/* BEGINNING OF ACTION: exp_ellipsis */
			{
#line 488 "syntax.act"

    (ZI1017) = make_ellipsis_exp () ;
#line 3457 "syntax1.c"
			}
			/* END OF ACTION: exp_ellipsis */
			ZR924 (ZI1017, &ZI1018);
			ZR1013 (&ZI1018, &ZI1023);
			ZR1025 (&ZI1023, &ZIa);
			if ((CURRENT_TERMINAL) == 355) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: reach_check */
			{
#line 830 "syntax.act"

    (ZIr) = unreached_code ;
    if ( (ZIr) ) {
	if ( !unreached_last ) {
	    report ( crt_loc, ERR_stmt_stmt_unreach () ) ;
	    unreached_last = 1 ;
	}
    } else {
	unreached_last = 0 ;
    }
#line 3480 "syntax1.c"
			}
			/* END OF ACTION: reach_check */
			/* BEGINNING OF ACTION: stmt_exp */
			{
#line 700 "syntax.act"

    (ZIe) = make_exp_stmt ( (ZIa) ) ;
#line 3488 "syntax1.c"
			}
			/* END OF ACTION: stmt_exp */
			/* BEGINNING OF ACTION: stmt_label_clear */
			{
#line 768 "syntax.act"

    unreached_fall = 1 ;
#line 3496 "syntax1.c"
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
	case 1:
		{
			IDENTIFIER ZIid;

			/* BEGINNING OF EXTRACT: identifier */
			{
#line 197 "syntax.act"

    ZIid = crt_token->pp_data.id.use ;
#line 3517 "syntax1.c"
			}
			/* END OF EXTRACT: identifier */
			ADVANCE_LEXER;
			ZR1084 (&ZIid, &ZIe);
			if ((CURRENT_TERMINAL) == 355) {
				RESTORE_LEXER;
				goto ZL1;
			}
		}
		break;
	case 346:
		{
			ADVANCE_LEXER;
			ZR1080 (&ZIe);
			if ((CURRENT_TERMINAL) == 355) {
				RESTORE_LEXER;
				goto ZL1;
			}
		}
		break;
	case 255:
		{
			EXP ZI1082;
			EXP ZI963;
			EXP ZI962;
			EXP ZI961;
			EXP ZI960;
			EXP ZI959;
			EXP ZI958;
			EXP ZI957;
			EXP ZI956;
			EXP ZI955;
			EXP ZI954;
			EXP ZI953;
			EXP ZI950;
			EXP ZI1023;
			EXP ZIa;
			int ZIr;

			ADVANCE_LEXER;
			ZRcast_Hexpression (&ZI1082);
			if ((CURRENT_TERMINAL) == 355) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: exp_lit */
			{
#line 522 "syntax.act"

    (ZI963) = make_lit_exp ( (ZI1082) ) ;
#line 3568 "syntax1.c"
			}
			/* END OF ACTION: exp_lit */
			ZR920 (ZI963, &ZI962);
			ZR916 (ZI962, &ZI961);
			ZR912 (ZI961, &ZI960);
			ZR908 (ZI960, &ZI959);
			ZR904 (ZI959, &ZI958);
			ZR900 (ZI958, &ZI957);
			ZR896 (ZI957, &ZI956);
			ZR892 (ZI956, &ZI955);
			ZR888 (ZI955, &ZI954);
			ZR884 (ZI954, &ZI953);
			ZR880 (ZI953, &ZI950);
			ZR952 (&ZI950, &ZI1023);
			ZR1025 (&ZI1023, &ZIa);
			if ((CURRENT_TERMINAL) == 355) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: reach_check */
			{
#line 830 "syntax.act"

    (ZIr) = unreached_code ;
    if ( (ZIr) ) {
	if ( !unreached_last ) {
	    report ( crt_loc, ERR_stmt_stmt_unreach () ) ;
	    unreached_last = 1 ;
	}
    } else {
	unreached_last = 0 ;
    }
#line 3601 "syntax1.c"
			}
			/* END OF ACTION: reach_check */
			/* BEGINNING OF ACTION: stmt_exp */
			{
#line 700 "syntax.act"

    (ZIe) = make_exp_stmt ( (ZIa) ) ;
#line 3609 "syntax1.c"
			}
			/* END OF ACTION: stmt_exp */
			/* BEGINNING OF ACTION: stmt_label_clear */
			{
#line 768 "syntax.act"

    unreached_fall = 1 ;
#line 3617 "syntax1.c"
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
	case 61:
		{
			EXP ZI989;
			EXP ZI990;
			EXP ZI1023;
			EXP ZIa;
			int ZIr;

			ADVANCE_LEXER;
			ZRunary_Hexpression (&ZI989);
			if ((CURRENT_TERMINAL) == 355) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: exp_predec */
			{
#line 583 "syntax.act"

    (ZI990) = make_prefix_exp ( lex_minus_Hminus, (ZI989) ) ;
#line 3648 "syntax1.c"
			}
			/* END OF ACTION: exp_predec */
			ZR1013 (&ZI990, &ZI1023);
			ZR1025 (&ZI1023, &ZIa);
			if ((CURRENT_TERMINAL) == 355) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: reach_check */
			{
#line 830 "syntax.act"

    (ZIr) = unreached_code ;
    if ( (ZIr) ) {
	if ( !unreached_last ) {
	    report ( crt_loc, ERR_stmt_stmt_unreach () ) ;
	    unreached_last = 1 ;
	}
    } else {
	unreached_last = 0 ;
    }
#line 3670 "syntax1.c"
			}
			/* END OF ACTION: reach_check */
			/* BEGINNING OF ACTION: stmt_exp */
			{
#line 700 "syntax.act"

    (ZIe) = make_exp_stmt ( (ZIa) ) ;
#line 3678 "syntax1.c"
			}
			/* END OF ACTION: stmt_exp */
			/* BEGINNING OF ACTION: stmt_label_clear */
			{
#line 768 "syntax.act"

    unreached_fall = 1 ;
#line 3686 "syntax1.c"
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
	case 65:
		{
			EXP ZI1023;
			EXP ZIa;
			int ZIr;

			ADVANCE_LEXER;
			ZR985 (&ZI1023);
			ZR1025 (&ZI1023, &ZIa);
			if ((CURRENT_TERMINAL) == 355) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: reach_check */
			{
#line 830 "syntax.act"

    (ZIr) = unreached_code ;
    if ( (ZIr) ) {
	if ( !unreached_last ) {
	    report ( crt_loc, ERR_stmt_stmt_unreach () ) ;
	    unreached_last = 1 ;
	}
    } else {
	unreached_last = 0 ;
    }
#line 3724 "syntax1.c"
			}
			/* END OF ACTION: reach_check */
			/* BEGINNING OF ACTION: stmt_exp */
			{
#line 700 "syntax.act"

    (ZIe) = make_exp_stmt ( (ZIa) ) ;
#line 3732 "syntax1.c"
			}
			/* END OF ACTION: stmt_exp */
			/* BEGINNING OF ACTION: stmt_label_clear */
			{
#line 768 "syntax.act"

    unreached_fall = 1 ;
#line 3740 "syntax1.c"
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
	case 71:
		{
			EXP ZI986;
			EXP ZI987;
			EXP ZI1023;
			EXP ZIa;
			int ZIr;

			ADVANCE_LEXER;
			ZRunary_Hexpression (&ZI986);
			if ((CURRENT_TERMINAL) == 355) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: exp_preinc */
			{
#line 587 "syntax.act"

    (ZI987) = make_prefix_exp ( lex_plus_Hplus, (ZI986) ) ;
#line 3771 "syntax1.c"
			}
			/* END OF ACTION: exp_preinc */
			ZR1013 (&ZI987, &ZI1023);
			ZR1025 (&ZI1023, &ZIa);
			if ((CURRENT_TERMINAL) == 355) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: reach_check */
			{
#line 830 "syntax.act"

    (ZIr) = unreached_code ;
    if ( (ZIr) ) {
	if ( !unreached_last ) {
	    report ( crt_loc, ERR_stmt_stmt_unreach () ) ;
	    unreached_last = 1 ;
	}
    } else {
	unreached_last = 0 ;
    }
#line 3793 "syntax1.c"
			}
			/* END OF ACTION: reach_check */
			/* BEGINNING OF ACTION: stmt_exp */
			{
#line 700 "syntax.act"

    (ZIe) = make_exp_stmt ( (ZIa) ) ;
#line 3801 "syntax1.c"
			}
			/* END OF ACTION: stmt_exp */
			/* BEGINNING OF ACTION: stmt_label_clear */
			{
#line 768 "syntax.act"

    unreached_fall = 1 ;
#line 3809 "syntax1.c"
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
			ADVANCE_LEXER;
			/* BEGINNING OF ACTION: stmt_none */
			{
#line 638 "syntax.act"

    (ZIe) = NULL_exp ;
#line 3829 "syntax1.c"
			}
			/* END OF ACTION: stmt_none */
		}
		break;
	case 116:
		{
			int ZI1005;
			EXP ZI1006;
			EXP ZI1023;
			EXP ZIa;
			int ZIr;

			ADVANCE_LEXER;
			/* BEGINNING OF ACTION: lex_sizeof */
			{
#line 371 "syntax.act"
 (ZI1005) = lex_sizeof ; 
#line 3847 "syntax1.c"
			}
			/* END OF ACTION: lex_sizeof */
			ZRsizeof_Hexpression (ZI1005, &ZI1006);
			ZR1013 (&ZI1006, &ZI1023);
			ZR1025 (&ZI1023, &ZIa);
			if ((CURRENT_TERMINAL) == 355) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: reach_check */
			{
#line 830 "syntax.act"

    (ZIr) = unreached_code ;
    if ( (ZIr) ) {
	if ( !unreached_last ) {
	    report ( crt_loc, ERR_stmt_stmt_unreach () ) ;
	    unreached_last = 1 ;
	}
    } else {
	unreached_last = 0 ;
    }
#line 3870 "syntax1.c"
			}
			/* END OF ACTION: reach_check */
			/* BEGINNING OF ACTION: stmt_exp */
			{
#line 700 "syntax.act"

    (ZIe) = make_exp_stmt ( (ZIa) ) ;
#line 3878 "syntax1.c"
			}
			/* END OF ACTION: stmt_exp */
			/* BEGINNING OF ACTION: stmt_label_clear */
			{
#line 768 "syntax.act"

    unreached_fall = 1 ;
#line 3886 "syntax1.c"
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
	case 78:
		{
			EXP ZI992;
			EXP ZI993;
			EXP ZI1023;
			EXP ZIa;
			int ZIr;

			ADVANCE_LEXER;
			ZRcast_Hexpression (&ZI992);
			if ((CURRENT_TERMINAL) == 355) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: exp_indir */
			{
#line 518 "syntax.act"

    (ZI993) = make_indir_exp ( (ZI992) ) ;
#line 3917 "syntax1.c"
			}
			/* END OF ACTION: exp_indir */
			ZR1013 (&ZI993, &ZI1023);
			ZR1025 (&ZI1023, &ZIa);
			if ((CURRENT_TERMINAL) == 355) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: reach_check */
			{
#line 830 "syntax.act"

    (ZIr) = unreached_code ;
    if ( (ZIr) ) {
	if ( !unreached_last ) {
	    report ( crt_loc, ERR_stmt_stmt_unreach () ) ;
	    unreached_last = 1 ;
	}
    } else {
	unreached_last = 0 ;
    }
#line 3939 "syntax1.c"
			}
			/* END OF ACTION: reach_check */
			/* BEGINNING OF ACTION: stmt_exp */
			{
#line 700 "syntax.act"

    (ZIe) = make_exp_stmt ( (ZIa) ) ;
#line 3947 "syntax1.c"
			}
			/* END OF ACTION: stmt_exp */
			/* BEGINNING OF ACTION: stmt_label_clear */
			{
#line 768 "syntax.act"

    unreached_fall = 1 ;
#line 3955 "syntax1.c"
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
	case 4:
		{
			IDENTIFIER ZIid;

			/* BEGINNING OF EXTRACT: statement-name */
			{
#line 209 "syntax.act"

    ZIid = crt_token->pp_data.id.use ;
#line 3976 "syntax1.c"
			}
			/* END OF EXTRACT: statement-name */
			ADVANCE_LEXER;
			ZR1085 (&ZIid, &ZIe);
			if ((CURRENT_TERMINAL) == 355) {
				RESTORE_LEXER;
				goto ZL1;
			}
		}
		break;
	case 2:
		{
			IDENTIFIER ZIid;
			EXP ZIa;
			EXP ZIb;

			/* BEGINNING OF EXTRACT: type-name */
			{
#line 201 "syntax.act"

    ZIid = crt_token->pp_data.id.use ;
#line 3998 "syntax1.c"
			}
			/* END OF EXTRACT: type-name */
			ADVANCE_LEXER;
			/* BEGINNING OF ACTION: stmt_label_mod */
			{
#line 772 "syntax.act"

    if ( unreached_code ) unreached_fall = 0 ;
#line 4007 "syntax1.c"
			}
			/* END OF ACTION: stmt_label_mod */
			/* BEGINNING OF ACTION: stmt_label_begin */
			{
#line 755 "syntax.act"

    (ZIa) = begin_label_stmt ( (ZIid), lex_identifier ) ;
#line 4015 "syntax1.c"
			}
			/* END OF ACTION: stmt_label_begin */
			switch (CURRENT_TERMINAL) {
			case 41:
				break;
			default:
				goto ZL1;
			}
			ADVANCE_LEXER;
			ZRstatement (&ZIb);
			if ((CURRENT_TERMINAL) == 355) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: stmt_label_end */
			{
#line 759 "syntax.act"

    (ZIe) = end_label_stmt ( (ZIa), (ZIb) ) ;
#line 4035 "syntax1.c"
			}
			/* END OF ACTION: stmt_label_end */
		}
		break;
	case 33:
		{
			EXP ZI995;
			EXP ZI996;
			EXP ZI1023;
			EXP ZIa;
			int ZIr;

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
			ZRcast_Hexpression (&ZI995);
			if ((CURRENT_TERMINAL) == 355) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: exp_ref */
			{
#line 591 "syntax.act"

    (ZI996) = make_ref_exp ( (ZI995), 0 ) ;
#line 4071 "syntax1.c"
			}
			/* END OF ACTION: exp_ref */
			ZR1013 (&ZI996, &ZI1023);
			ZR1025 (&ZI1023, &ZIa);
			if ((CURRENT_TERMINAL) == 355) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: reach_check */
			{
#line 830 "syntax.act"

    (ZIr) = unreached_code ;
    if ( (ZIr) ) {
	if ( !unreached_last ) {
	    report ( crt_loc, ERR_stmt_stmt_unreach () ) ;
	    unreached_last = 1 ;
	}
    } else {
	unreached_last = 0 ;
    }
#line 4093 "syntax1.c"
			}
			/* END OF ACTION: reach_check */
			/* BEGINNING OF ACTION: stmt_exp */
			{
#line 700 "syntax.act"

    (ZIe) = make_exp_stmt ( (ZIa) ) ;
#line 4101 "syntax1.c"
			}
			/* END OF ACTION: stmt_exp */
			/* BEGINNING OF ACTION: stmt_label_clear */
			{
#line 768 "syntax.act"

    unreached_fall = 1 ;
#line 4109 "syntax1.c"
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
	case 126:
		{
			ZRasm_Hdefinition (&ZIe);
			if ((CURRENT_TERMINAL) == 355) {
				RESTORE_LEXER;
				goto ZL1;
			}
		}
		break;
	case 289: case 323:
		{
			ZRcontrol_Hstatement (&ZIe);
			if ((CURRENT_TERMINAL) == 355) {
				RESTORE_LEXER;
				goto ZL1;
			}
		}
		break;
	case 96: case 100: case 223:
		{
			ZRfall_Hcheck ();
			ZR1081 (&ZIe);
			if ((CURRENT_TERMINAL) == 355) {
				RESTORE_LEXER;
				goto ZL1;
			}
		}
		break;
	case 298: case 324:
		{
			EXP ZI1075;
			EXP ZIa;
			int ZIr;

			ZRflow_Hexpression (&ZI1075);
			ZR1077 (&ZI1075, &ZIa);
			if ((CURRENT_TERMINAL) == 355) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: reach_check */
			{
#line 830 "syntax.act"

    (ZIr) = unreached_code ;
    if ( (ZIr) ) {
	if ( !unreached_last ) {
	    report ( crt_loc, ERR_stmt_stmt_unreach () ) ;
	    unreached_last = 1 ;
	}
    } else {
	unreached_last = 0 ;
    }
#line 4174 "syntax1.c"
			}
			/* END OF ACTION: reach_check */
			/* BEGINNING OF ACTION: stmt_exp */
			{
#line 700 "syntax.act"

    (ZIe) = make_exp_stmt ( (ZIa) ) ;
#line 4182 "syntax1.c"
			}
			/* END OF ACTION: stmt_exp */
			/* BEGINNING OF ACTION: stmt_label_clear */
			{
#line 768 "syntax.act"

    unreached_fall = 1 ;
#line 4190 "syntax1.c"
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
	case 101: case 107: case 125:
		{
			ZRiteration_Hstatement (&ZIe);
			if ((CURRENT_TERMINAL) == 355) {
				RESTORE_LEXER;
				goto ZL1;
			}
		}
		break;
	case 95: case 99: case 108: case 113:
		{
			ZRjump_Hstatement (&ZIe);
			if ((CURRENT_TERMINAL) == 355) {
				RESTORE_LEXER;
				goto ZL1;
			}
		}
		break;
	case 17: case 18: case 19: case 20:
	case 21: case 22:
		{
			EXP ZI1011;
			EXP ZI1012;
			EXP ZI1023;
			EXP ZIa;
			int ZIr;

			ZRliteral (&ZI1011);
			ZR924 (ZI1011, &ZI1012);
			ZR1013 (&ZI1012, &ZI1023);
			ZR1025 (&ZI1023, &ZIa);
			if ((CURRENT_TERMINAL) == 355) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: reach_check */
			{
#line 830 "syntax.act"

    (ZIr) = unreached_code ;
    if ( (ZIr) ) {
	if ( !unreached_last ) {
	    report ( crt_loc, ERR_stmt_stmt_unreach () ) ;
	    unreached_last = 1 ;
	}
    } else {
	unreached_last = 0 ;
    }
#line 4250 "syntax1.c"
			}
			/* END OF ACTION: reach_check */
			/* BEGINNING OF ACTION: stmt_exp */
			{
#line 700 "syntax.act"

    (ZIe) = make_exp_stmt ( (ZIa) ) ;
#line 4258 "syntax1.c"
			}
			/* END OF ACTION: stmt_exp */
			/* BEGINNING OF ACTION: stmt_label_clear */
			{
#line 768 "syntax.act"

    unreached_fall = 1 ;
#line 4266 "syntax1.c"
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
	case 62:
		{
			EXP ZI998;
			EXP ZI999;
			EXP ZI1023;
			EXP ZIa;
			int ZIr;

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
			ZRcast_Hexpression (&ZI998);
			if ((CURRENT_TERMINAL) == 355) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: exp_not */
			{
#line 554 "syntax.act"

    (ZI999) = make_not_exp ( (ZI998) ) ;
#line 4309 "syntax1.c"
			}
			/* END OF ACTION: exp_not */
			ZR1013 (&ZI999, &ZI1023);
			ZR1025 (&ZI1023, &ZIa);
			if ((CURRENT_TERMINAL) == 355) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: reach_check */
			{
#line 830 "syntax.act"

    (ZIr) = unreached_code ;
    if ( (ZIr) ) {
	if ( !unreached_last ) {
	    report ( crt_loc, ERR_stmt_stmt_unreach () ) ;
	    unreached_last = 1 ;
	}
    } else {
	unreached_last = 0 ;
    }
#line 4331 "syntax1.c"
			}
			/* END OF ACTION: reach_check */
			/* BEGINNING OF ACTION: stmt_exp */
			{
#line 700 "syntax.act"

    (ZIe) = make_exp_stmt ( (ZIa) ) ;
#line 4339 "syntax1.c"
			}
			/* END OF ACTION: stmt_exp */
			/* BEGINNING OF ACTION: stmt_label_clear */
			{
#line 768 "syntax.act"

    unreached_fall = 1 ;
#line 4347 "syntax1.c"
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
	case 26: case 109: case 119:
		{
			ZRselection_Hstatement (&ZIe);
			if ((CURRENT_TERMINAL) == 355) {
				RESTORE_LEXER;
				goto ZL1;
			}
		}
		break;
	case 43: case 59: case 69: case 85:
		{
			int ZI1001;
			EXP ZI1002;
			EXP ZI1003;
			EXP ZI1023;
			EXP ZIa;
			int ZIr;

			/* BEGINNING OF ACTION: lex_crt */
			{
#line 367 "syntax.act"
 (ZI1001) = crt_lex_token ; 
#line 4381 "syntax1.c"
			}
			/* END OF ACTION: lex_crt */
			ZRunary_Hoperator ();
			ZRcast_Hexpression (&ZI1002);
			if ((CURRENT_TERMINAL) == 355) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: exp_unary */
			{
#line 616 "syntax.act"

    (ZI1003) = make_uminus_exp ( (ZI1001), (ZI1002) ) ;
#line 4395 "syntax1.c"
			}
			/* END OF ACTION: exp_unary */
			ZR1013 (&ZI1003, &ZI1023);
			ZR1025 (&ZI1023, &ZIa);
			if ((CURRENT_TERMINAL) == 355) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: reach_check */
			{
#line 830 "syntax.act"

    (ZIr) = unreached_code ;
    if ( (ZIr) ) {
	if ( !unreached_last ) {
	    report ( crt_loc, ERR_stmt_stmt_unreach () ) ;
	    unreached_last = 1 ;
	}
    } else {
	unreached_last = 0 ;
    }
#line 4417 "syntax1.c"
			}
			/* END OF ACTION: reach_check */
			/* BEGINNING OF ACTION: stmt_exp */
			{
#line 700 "syntax.act"

    (ZIe) = make_exp_stmt ( (ZIa) ) ;
#line 4425 "syntax1.c"
			}
			/* END OF ACTION: stmt_exp */
			/* BEGINNING OF ACTION: stmt_label_clear */
			{
#line 768 "syntax.act"

    unreached_fall = 1 ;
#line 4433 "syntax1.c"
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
	case 355:
		return;
	default:
		goto ZL1;
	}
	goto ZL0;
ZL1:;
	SAVE_LEXER (355);
	return;
ZL0:;
	*ZOe = ZIe;
}

void
ZRmember_Hspecification_Hopt(void)
{
	if ((CURRENT_TERMINAL) == 355) {
		return;
	}
ZL2_member_Hspecification_Hopt:;
	{
		BASE_TYPE ZIb1;
		TYPE ZIt1;
		CV_SPEC ZIcv1;
		BASE_TYPE ZIbt;
		TYPE ZIp;
		CV_SPEC ZIcv;

		/* BEGINNING OF ACTION: is_type_specifier */
		{
#line 1447 "syntax.act"

    /* Resolve type-specifiers from other declarators */
    (ZI0) = predict_tspec ( 0 ) ;
#line 4479 "syntax1.c"
		}
		/* END OF ACTION: is_type_specifier */
		if (!ZI0)
			goto ZL3;
		ZRtype_Hqualifier (&ZIb1, &ZIt1, &ZIcv1);
		if ((CURRENT_TERMINAL) == 355) {
			RESTORE_LEXER;
			goto ZL1;
		}
		/* BEGINNING OF ACTION: check_decl_specifier */
		{
#line 1473 "syntax.act"

    /* A type-name can be a declarator-id */
    if ( have_type_specifier && crt_lex_token == lex_type_Hname ) {
	crt_lex_token = lex_identifier ;
    }
#line 4497 "syntax1.c"
		}
		/* END OF ACTION: check_decl_specifier */
		ZR701 (&ZIb1, &ZIt1, &ZIcv1, &ZIbt, &ZIp, &ZIcv);
		ZR1029 (&ZIbt, &ZIp, &ZIcv);
		/* BEGINNING OF INLINE: member-specification-opt */
		if ((CURRENT_TERMINAL) == 355) {
			RESTORE_LEXER;
			goto ZL1;
		} else {
			goto ZL2_member_Hspecification_Hopt;
		}
		/* END OF INLINE: member-specification-opt */
	}
	UNREACHED;
ZL3:;
	return;
ZL1:;
	SAVE_LEXER (355);
	return;
}

void
ZR916(EXP ZI914, EXP *ZO915)
{
	EXP ZI915;

ZL2_916:;
	switch (CURRENT_TERMINAL) {
	case 59:
		{
			EXP ZIb;
			EXP ZIe;

			ADVANCE_LEXER;
			ZRmultiplicative_Hexpression (&ZIb);
			if ((CURRENT_TERMINAL) == 355) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: exp_minus */
			{
#line 546 "syntax.act"

    (ZIe) = make_minus_exp ( (ZI914), (ZIb) ) ;
#line 4542 "syntax1.c"
			}
			/* END OF ACTION: exp_minus */
			/* BEGINNING OF INLINE: 916 */
			ZI914 = ZIe;
			goto ZL2_916;
			/* END OF INLINE: 916 */
		}
		UNREACHED;
	case 69:
		{
			EXP ZIb;
			EXP ZIe;

			ADVANCE_LEXER;
			ZRmultiplicative_Hexpression (&ZIb);
			if ((CURRENT_TERMINAL) == 355) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: exp_plus */
			{
#line 571 "syntax.act"

    (ZIe) = make_plus_exp ( (ZI914), (ZIb) ) ;
#line 4567 "syntax1.c"
			}
			/* END OF ACTION: exp_plus */
			/* BEGINNING OF INLINE: 916 */
			ZI914 = ZIe;
			goto ZL2_916;
			/* END OF INLINE: 916 */
		}
		UNREACHED;
	default:
		{
			ZI915 = ZI914;
		}
		break;
	case 355:
		return;
	}
	goto ZL0;
ZL1:;
	SAVE_LEXER (355);
	return;
ZL0:;
	*ZO915 = ZI915;
}

void
ZRsizeof_Hexpression(int ZIop, EXP *ZOe)
{
	EXP ZIe;

	if ((CURRENT_TERMINAL) == 355) {
		return;
	}
	{
		int ZIn1;
		int ZIm1;
		EXP ZIc;

		/* BEGINNING OF ACTION: sizeof_begin */
		{
#line 1390 "syntax.act"

    suppress_usage++ ;
#line 4610 "syntax1.c"
		}
		/* END OF ACTION: sizeof_begin */
		/* BEGINNING OF ACTION: no_side_effects */
		{
#line 1373 "syntax.act"

    (ZIn1) = no_side_effects ;
#line 4618 "syntax1.c"
		}
		/* END OF ACTION: no_side_effects */
		/* BEGINNING OF ACTION: no_type_defns */
		{
#line 1377 "syntax.act"

    (ZIm1) = no_type_defns ;
#line 4626 "syntax1.c"
		}
		/* END OF ACTION: no_type_defns */
		/* BEGINNING OF INLINE: 465 */
		{
			switch (CURRENT_TERMINAL) {
			case 170:
				{
					int ZI1067;
					EXP ZIa;
					int ZIn2;
					int ZIm2;
					TYPE ZIt;

					ADVANCE_LEXER;
					/* BEGINNING OF ACTION: lex_alignof */
					{
#line 370 "syntax.act"
 (ZI1067) = lex_alignof ; 
#line 4645 "syntax1.c"
					}
					/* END OF ACTION: lex_alignof */
					ZRsizeof_Hexpression (ZI1067, &ZIa);
					if ((CURRENT_TERMINAL) == 355) {
						RESTORE_LEXER;
						goto ZL1;
					}
					/* BEGINNING OF ACTION: diff_side_effects */
					{
#line 1381 "syntax.act"

    (ZIn2) = no_side_effects - (ZIn1) ;
#line 4658 "syntax1.c"
					}
					/* END OF ACTION: diff_side_effects */
					/* BEGINNING OF ACTION: diff_type_defns */
					{
#line 1385 "syntax.act"

    (ZIm2) = no_type_defns - (ZIm1) ;
#line 4666 "syntax1.c"
					}
					/* END OF ACTION: diff_type_defns */
					/* BEGINNING OF ACTION: type_of */
					{
#line 992 "syntax.act"

    (ZIt) = typeof_exp ( &(ZIa), (ZIn2), (ZIop) ) ;
#line 4674 "syntax1.c"
					}
					/* END OF ACTION: type_of */
					/* BEGINNING OF ACTION: exp_sizeof */
					{
#line 612 "syntax.act"

    (ZIc) = make_sizeof_exp ( (ZIt), (ZIa), (ZIm2), (ZIop) ) ;
#line 4682 "syntax1.c"
					}
					/* END OF ACTION: exp_sizeof */
				}
				break;
			case 23:
				{
					EXP ZI1103;
					EXP ZIa;
					int ZIn2;
					int ZIm2;
					TYPE ZIt;

					/* BEGINNING OF EXTRACT: complex-exp */
					{
#line 333 "syntax.act"

    IDENTIFIER id = crt_token->pp_data.tok.id ;
    PPTOKEN *args = crt_token->pp_data.tok.args ;
    ZI1103 = parse_exp_token ( id, args ) ;
    RESCAN_LEXER ;
#line 4703 "syntax1.c"
					}
					/* END OF EXTRACT: complex-exp */
					ADVANCE_LEXER;
					ZR924 (ZI1103, &ZIa);
					if ((CURRENT_TERMINAL) == 355) {
						RESTORE_LEXER;
						goto ZL1;
					}
					/* BEGINNING OF ACTION: diff_side_effects */
					{
#line 1381 "syntax.act"

    (ZIn2) = no_side_effects - (ZIn1) ;
#line 4717 "syntax1.c"
					}
					/* END OF ACTION: diff_side_effects */
					/* BEGINNING OF ACTION: diff_type_defns */
					{
#line 1385 "syntax.act"

    (ZIm2) = no_type_defns - (ZIm1) ;
#line 4725 "syntax1.c"
					}
					/* END OF ACTION: diff_type_defns */
					/* BEGINNING OF ACTION: type_of */
					{
#line 992 "syntax.act"

    (ZIt) = typeof_exp ( &(ZIa), (ZIn2), (ZIop) ) ;
#line 4733 "syntax1.c"
					}
					/* END OF ACTION: type_of */
					/* BEGINNING OF ACTION: exp_sizeof */
					{
#line 612 "syntax.act"

    (ZIc) = make_sizeof_exp ( (ZIt), (ZIa), (ZIm2), (ZIop) ) ;
#line 4741 "syntax1.c"
					}
					/* END OF ACTION: exp_sizeof */
				}
				break;
			case 210:
				{
					EXP ZI1104;
					EXP ZIa;
					int ZIn2;
					int ZIm2;
					TYPE ZIt;

					ADVANCE_LEXER;
					/* BEGINNING OF ACTION: exp_ellipsis */
					{
#line 488 "syntax.act"

    (ZI1104) = make_ellipsis_exp () ;
#line 4760 "syntax1.c"
					}
					/* END OF ACTION: exp_ellipsis */
					ZR924 (ZI1104, &ZIa);
					if ((CURRENT_TERMINAL) == 355) {
						RESTORE_LEXER;
						goto ZL1;
					}
					/* BEGINNING OF ACTION: diff_side_effects */
					{
#line 1381 "syntax.act"

    (ZIn2) = no_side_effects - (ZIn1) ;
#line 4773 "syntax1.c"
					}
					/* END OF ACTION: diff_side_effects */
					/* BEGINNING OF ACTION: diff_type_defns */
					{
#line 1385 "syntax.act"

    (ZIm2) = no_type_defns - (ZIm1) ;
#line 4781 "syntax1.c"
					}
					/* END OF ACTION: diff_type_defns */
					/* BEGINNING OF ACTION: type_of */
					{
#line 992 "syntax.act"

    (ZIt) = typeof_exp ( &(ZIa), (ZIn2), (ZIop) ) ;
#line 4789 "syntax1.c"
					}
					/* END OF ACTION: type_of */
					/* BEGINNING OF ACTION: exp_sizeof */
					{
#line 612 "syntax.act"

    (ZIc) = make_sizeof_exp ( (ZIt), (ZIa), (ZIm2), (ZIop) ) ;
#line 4797 "syntax1.c"
					}
					/* END OF ACTION: exp_sizeof */
				}
				break;
			case 1:
				{
					IDENTIFIER ZIid;
					EXP ZI1105;
					EXP ZIa;
					int ZIn2;
					int ZIm2;
					TYPE ZIt;

					/* BEGINNING OF EXTRACT: identifier */
					{
#line 197 "syntax.act"

    ZIid = crt_token->pp_data.id.use ;
#line 4816 "syntax1.c"
					}
					/* END OF EXTRACT: identifier */
					ADVANCE_LEXER;
					/* BEGINNING OF ACTION: exp_identifier */
					{
#line 506 "syntax.act"

    (ZI1105) = make_id_exp ( (ZIid) ) ;
#line 4825 "syntax1.c"
					}
					/* END OF ACTION: exp_identifier */
					ZR924 (ZI1105, &ZIa);
					if ((CURRENT_TERMINAL) == 355) {
						RESTORE_LEXER;
						goto ZL1;
					}
					/* BEGINNING OF ACTION: diff_side_effects */
					{
#line 1381 "syntax.act"

    (ZIn2) = no_side_effects - (ZIn1) ;
#line 4838 "syntax1.c"
					}
					/* END OF ACTION: diff_side_effects */
					/* BEGINNING OF ACTION: diff_type_defns */
					{
#line 1385 "syntax.act"

    (ZIm2) = no_type_defns - (ZIm1) ;
#line 4846 "syntax1.c"
					}
					/* END OF ACTION: diff_type_defns */
					/* BEGINNING OF ACTION: type_of */
					{
#line 992 "syntax.act"

    (ZIt) = typeof_exp ( &(ZIa), (ZIn2), (ZIop) ) ;
#line 4854 "syntax1.c"
					}
					/* END OF ACTION: type_of */
					/* BEGINNING OF ACTION: exp_sizeof */
					{
#line 612 "syntax.act"

    (ZIc) = make_sizeof_exp ( (ZIt), (ZIa), (ZIm2), (ZIop) ) ;
#line 4862 "syntax1.c"
					}
					/* END OF ACTION: exp_sizeof */
				}
				break;
			case 61:
				{
					EXP ZI1060;
					EXP ZIa;
					int ZIn2;
					int ZIm2;
					TYPE ZIt;

					ADVANCE_LEXER;
					ZRunary_Hexpression (&ZI1060);
					if ((CURRENT_TERMINAL) == 355) {
						RESTORE_LEXER;
						goto ZL1;
					}
					/* BEGINNING OF ACTION: exp_predec */
					{
#line 583 "syntax.act"

    (ZIa) = make_prefix_exp ( lex_minus_Hminus, (ZI1060) ) ;
#line 4886 "syntax1.c"
					}
					/* END OF ACTION: exp_predec */
					/* BEGINNING OF ACTION: diff_side_effects */
					{
#line 1381 "syntax.act"

    (ZIn2) = no_side_effects - (ZIn1) ;
#line 4894 "syntax1.c"
					}
					/* END OF ACTION: diff_side_effects */
					/* BEGINNING OF ACTION: diff_type_defns */
					{
#line 1385 "syntax.act"

    (ZIm2) = no_type_defns - (ZIm1) ;
#line 4902 "syntax1.c"
					}
					/* END OF ACTION: diff_type_defns */
					/* BEGINNING OF ACTION: type_of */
					{
#line 992 "syntax.act"

    (ZIt) = typeof_exp ( &(ZIa), (ZIn2), (ZIop) ) ;
#line 4910 "syntax1.c"
					}
					/* END OF ACTION: type_of */
					/* BEGINNING OF ACTION: exp_sizeof */
					{
#line 612 "syntax.act"

    (ZIc) = make_sizeof_exp ( (ZIt), (ZIa), (ZIm2), (ZIop) ) ;
#line 4918 "syntax1.c"
					}
					/* END OF ACTION: exp_sizeof */
				}
				break;
			case 65:
				{
					ADVANCE_LEXER;
					ZR1069 (&ZIop, &ZIn1, &ZIm1, &ZIc);
					if ((CURRENT_TERMINAL) == 355) {
						RESTORE_LEXER;
						goto ZL1;
					}
				}
				break;
			case 71:
				{
					EXP ZI1059;
					EXP ZIa;
					int ZIn2;
					int ZIm2;
					TYPE ZIt;

					ADVANCE_LEXER;
					ZRunary_Hexpression (&ZI1059);
					if ((CURRENT_TERMINAL) == 355) {
						RESTORE_LEXER;
						goto ZL1;
					}
					/* BEGINNING OF ACTION: exp_preinc */
					{
#line 587 "syntax.act"

    (ZIa) = make_prefix_exp ( lex_plus_Hplus, (ZI1059) ) ;
#line 4952 "syntax1.c"
					}
					/* END OF ACTION: exp_preinc */
					/* BEGINNING OF ACTION: diff_side_effects */
					{
#line 1381 "syntax.act"

    (ZIn2) = no_side_effects - (ZIn1) ;
#line 4960 "syntax1.c"
					}
					/* END OF ACTION: diff_side_effects */
					/* BEGINNING OF ACTION: diff_type_defns */
					{
#line 1385 "syntax.act"

    (ZIm2) = no_type_defns - (ZIm1) ;
#line 4968 "syntax1.c"
					}
					/* END OF ACTION: diff_type_defns */
					/* BEGINNING OF ACTION: type_of */
					{
#line 992 "syntax.act"

    (ZIt) = typeof_exp ( &(ZIa), (ZIn2), (ZIop) ) ;
#line 4976 "syntax1.c"
					}
					/* END OF ACTION: type_of */
					/* BEGINNING OF ACTION: exp_sizeof */
					{
#line 612 "syntax.act"

    (ZIc) = make_sizeof_exp ( (ZIt), (ZIa), (ZIm2), (ZIop) ) ;
#line 4984 "syntax1.c"
					}
					/* END OF ACTION: exp_sizeof */
				}
				break;
			case 116:
				{
					int ZI1066;
					EXP ZIa;
					int ZIn2;
					int ZIm2;
					TYPE ZIt;

					ADVANCE_LEXER;
					/* BEGINNING OF ACTION: lex_sizeof */
					{
#line 371 "syntax.act"
 (ZI1066) = lex_sizeof ; 
#line 5002 "syntax1.c"
					}
					/* END OF ACTION: lex_sizeof */
					ZRsizeof_Hexpression (ZI1066, &ZIa);
					if ((CURRENT_TERMINAL) == 355) {
						RESTORE_LEXER;
						goto ZL1;
					}
					/* BEGINNING OF ACTION: diff_side_effects */
					{
#line 1381 "syntax.act"

    (ZIn2) = no_side_effects - (ZIn1) ;
#line 5015 "syntax1.c"
					}
					/* END OF ACTION: diff_side_effects */
					/* BEGINNING OF ACTION: diff_type_defns */
					{
#line 1385 "syntax.act"

    (ZIm2) = no_type_defns - (ZIm1) ;
#line 5023 "syntax1.c"
					}
					/* END OF ACTION: diff_type_defns */
					/* BEGINNING OF ACTION: type_of */
					{
#line 992 "syntax.act"

    (ZIt) = typeof_exp ( &(ZIa), (ZIn2), (ZIop) ) ;
#line 5031 "syntax1.c"
					}
					/* END OF ACTION: type_of */
					/* BEGINNING OF ACTION: exp_sizeof */
					{
#line 612 "syntax.act"

    (ZIc) = make_sizeof_exp ( (ZIt), (ZIa), (ZIm2), (ZIop) ) ;
#line 5039 "syntax1.c"
					}
					/* END OF ACTION: exp_sizeof */
				}
				break;
			case 78:
				{
					EXP ZI1061;
					EXP ZIa;
					int ZIn2;
					int ZIm2;
					TYPE ZIt;

					ADVANCE_LEXER;
					ZRcast_Hexpression (&ZI1061);
					if ((CURRENT_TERMINAL) == 355) {
						RESTORE_LEXER;
						goto ZL1;
					}
					/* BEGINNING OF ACTION: exp_indir */
					{
#line 518 "syntax.act"

    (ZIa) = make_indir_exp ( (ZI1061) ) ;
#line 5063 "syntax1.c"
					}
					/* END OF ACTION: exp_indir */
					/* BEGINNING OF ACTION: diff_side_effects */
					{
#line 1381 "syntax.act"

    (ZIn2) = no_side_effects - (ZIn1) ;
#line 5071 "syntax1.c"
					}
					/* END OF ACTION: diff_side_effects */
					/* BEGINNING OF ACTION: diff_type_defns */
					{
#line 1385 "syntax.act"

    (ZIm2) = no_type_defns - (ZIm1) ;
#line 5079 "syntax1.c"
					}
					/* END OF ACTION: diff_type_defns */
					/* BEGINNING OF ACTION: type_of */
					{
#line 992 "syntax.act"

    (ZIt) = typeof_exp ( &(ZIa), (ZIn2), (ZIop) ) ;
#line 5087 "syntax1.c"
					}
					/* END OF ACTION: type_of */
					/* BEGINNING OF ACTION: exp_sizeof */
					{
#line 612 "syntax.act"

    (ZIc) = make_sizeof_exp ( (ZIt), (ZIa), (ZIm2), (ZIop) ) ;
#line 5095 "syntax1.c"
					}
					/* END OF ACTION: exp_sizeof */
				}
				break;
			case 33:
				{
					EXP ZI1062;
					EXP ZIa;
					int ZIn2;
					int ZIm2;
					TYPE ZIt;

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
					ZRcast_Hexpression (&ZI1062);
					if ((CURRENT_TERMINAL) == 355) {
						RESTORE_LEXER;
						goto ZL1;
					}
					/* BEGINNING OF ACTION: exp_ref */
					{
#line 591 "syntax.act"

    (ZIa) = make_ref_exp ( (ZI1062), 0 ) ;
#line 5131 "syntax1.c"
					}
					/* END OF ACTION: exp_ref */
					/* BEGINNING OF ACTION: diff_side_effects */
					{
#line 1381 "syntax.act"

    (ZIn2) = no_side_effects - (ZIn1) ;
#line 5139 "syntax1.c"
					}
					/* END OF ACTION: diff_side_effects */
					/* BEGINNING OF ACTION: diff_type_defns */
					{
#line 1385 "syntax.act"

    (ZIm2) = no_type_defns - (ZIm1) ;
#line 5147 "syntax1.c"
					}
					/* END OF ACTION: diff_type_defns */
					/* BEGINNING OF ACTION: type_of */
					{
#line 992 "syntax.act"

    (ZIt) = typeof_exp ( &(ZIa), (ZIn2), (ZIop) ) ;
#line 5155 "syntax1.c"
					}
					/* END OF ACTION: type_of */
					/* BEGINNING OF ACTION: exp_sizeof */
					{
#line 612 "syntax.act"

    (ZIc) = make_sizeof_exp ( (ZIt), (ZIa), (ZIm2), (ZIop) ) ;
#line 5163 "syntax1.c"
					}
					/* END OF ACTION: exp_sizeof */
				}
				break;
			case 17: case 18: case 19: case 20:
			case 21: case 22:
				{
					EXP ZI1106;
					EXP ZIa;
					int ZIn2;
					int ZIm2;
					TYPE ZIt;

					ZRliteral (&ZI1106);
					ZR924 (ZI1106, &ZIa);
					if ((CURRENT_TERMINAL) == 355) {
						RESTORE_LEXER;
						goto ZL1;
					}
					/* BEGINNING OF ACTION: diff_side_effects */
					{
#line 1381 "syntax.act"

    (ZIn2) = no_side_effects - (ZIn1) ;
#line 5188 "syntax1.c"
					}
					/* END OF ACTION: diff_side_effects */
					/* BEGINNING OF ACTION: diff_type_defns */
					{
#line 1385 "syntax.act"

    (ZIm2) = no_type_defns - (ZIm1) ;
#line 5196 "syntax1.c"
					}
					/* END OF ACTION: diff_type_defns */
					/* BEGINNING OF ACTION: type_of */
					{
#line 992 "syntax.act"

    (ZIt) = typeof_exp ( &(ZIa), (ZIn2), (ZIop) ) ;
#line 5204 "syntax1.c"
					}
					/* END OF ACTION: type_of */
					/* BEGINNING OF ACTION: exp_sizeof */
					{
#line 612 "syntax.act"

    (ZIc) = make_sizeof_exp ( (ZIt), (ZIa), (ZIm2), (ZIop) ) ;
#line 5212 "syntax1.c"
					}
					/* END OF ACTION: exp_sizeof */
				}
				break;
			case 62:
				{
					EXP ZI1063;
					EXP ZIa;
					int ZIn2;
					int ZIm2;
					TYPE ZIt;

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
					ZRcast_Hexpression (&ZI1063);
					if ((CURRENT_TERMINAL) == 355) {
						RESTORE_LEXER;
						goto ZL1;
					}
					/* BEGINNING OF ACTION: exp_not */
					{
#line 554 "syntax.act"

    (ZIa) = make_not_exp ( (ZI1063) ) ;
#line 5248 "syntax1.c"
					}
					/* END OF ACTION: exp_not */
					/* BEGINNING OF ACTION: diff_side_effects */
					{
#line 1381 "syntax.act"

    (ZIn2) = no_side_effects - (ZIn1) ;
#line 5256 "syntax1.c"
					}
					/* END OF ACTION: diff_side_effects */
					/* BEGINNING OF ACTION: diff_type_defns */
					{
#line 1385 "syntax.act"

    (ZIm2) = no_type_defns - (ZIm1) ;
#line 5264 "syntax1.c"
					}
					/* END OF ACTION: diff_type_defns */
					/* BEGINNING OF ACTION: type_of */
					{
#line 992 "syntax.act"

    (ZIt) = typeof_exp ( &(ZIa), (ZIn2), (ZIop) ) ;
#line 5272 "syntax1.c"
					}
					/* END OF ACTION: type_of */
					/* BEGINNING OF ACTION: exp_sizeof */
					{
#line 612 "syntax.act"

    (ZIc) = make_sizeof_exp ( (ZIt), (ZIa), (ZIm2), (ZIop) ) ;
#line 5280 "syntax1.c"
					}
					/* END OF ACTION: exp_sizeof */
				}
				break;
			case 43: case 59: case 69: case 85:
				{
					int ZI1064;
					EXP ZI1065;
					EXP ZIa;
					int ZIn2;
					int ZIm2;
					TYPE ZIt;

					/* BEGINNING OF ACTION: lex_crt */
					{
#line 367 "syntax.act"
 (ZI1064) = crt_lex_token ; 
#line 5298 "syntax1.c"
					}
					/* END OF ACTION: lex_crt */
					ZRunary_Hoperator ();
					ZRcast_Hexpression (&ZI1065);
					if ((CURRENT_TERMINAL) == 355) {
						RESTORE_LEXER;
						goto ZL1;
					}
					/* BEGINNING OF ACTION: exp_unary */
					{
#line 616 "syntax.act"

    (ZIa) = make_uminus_exp ( (ZI1064), (ZI1065) ) ;
#line 5312 "syntax1.c"
					}
					/* END OF ACTION: exp_unary */
					/* BEGINNING OF ACTION: diff_side_effects */
					{
#line 1381 "syntax.act"

    (ZIn2) = no_side_effects - (ZIn1) ;
#line 5320 "syntax1.c"
					}
					/* END OF ACTION: diff_side_effects */
					/* BEGINNING OF ACTION: diff_type_defns */
					{
#line 1385 "syntax.act"

    (ZIm2) = no_type_defns - (ZIm1) ;
#line 5328 "syntax1.c"
					}
					/* END OF ACTION: diff_type_defns */
					/* BEGINNING OF ACTION: type_of */
					{
#line 992 "syntax.act"

    (ZIt) = typeof_exp ( &(ZIa), (ZIn2), (ZIop) ) ;
#line 5336 "syntax1.c"
					}
					/* END OF ACTION: type_of */
					/* BEGINNING OF ACTION: exp_sizeof */
					{
#line 612 "syntax.act"

    (ZIc) = make_sizeof_exp ( (ZIt), (ZIa), (ZIm2), (ZIop) ) ;
#line 5344 "syntax1.c"
					}
					/* END OF ACTION: exp_sizeof */
				}
				break;
			default:
				goto ZL1;
			}
		}
		/* END OF INLINE: 465 */
		/* BEGINNING OF ACTION: sizeof_end */
		{
#line 1394 "syntax.act"

    suppress_usage-- ;
#line 5359 "syntax1.c"
		}
		/* END OF ACTION: sizeof_end */
		ZIe = ZIc;
	}
	goto ZL0;
ZL1:;
	SAVE_LEXER (355);
	return;
ZL0:;
	*ZOe = ZIe;
}

/* END OF FILE */
