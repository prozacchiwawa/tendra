/*
 * Automatically generated from the files:
 *	syntax.sid
 * and
 *	syntax.act
 * by:
 *	/home/arty/dev/tendra/tendra/obj.antigone-regen/bin/sid
 */

/* BEGINNING OF HEADER */

#line 50 "syntax.act"



/*
 * Copyright 2011, The TenDRA Project.
 * Copyright 1997, United Kingdom Secretary of State for Defence.
 *
 * See doc/copyright/ for the full copyright terms.
 */

#include <string.h>

#include <shared/check.h>
#include <shared/error.h>
#include <shared/xalloc.h>
#include <shared/string.h>

#include "obj_c/errors.h"
#include "obj_c/entry_ops.h"
#include "obj_c/map_ops.h"
#include "obj_c/msg_ops.h"
#include "obj_c/name_ops.h"
#include "obj_c/param_ops.h"

#include "lex.h"
#include "process.h"
#include "syntax.h"


/*
    These types give the implementation of the types used in the syntax.
*/

typedef LIST ( ENTRY ) LIST_ENTRY ;
typedef LIST ( MAP ) LIST_MAP ;
typedef LIST ( MESSAGE ) LIST_MESSAGE ;
typedef LIST ( NAME ) LIST_NAME ;
typedef LIST ( PARAM ) LIST_PARAM ;
typedef LIST ( PROPERTY ) LIST_PROPERTY ;


/*
    This variable is used to keep count of the position in a name or
    parameter list.
*/

static int counter = 0 ;


/*
    This routine searches the name list p for an identifier matching id.
    The null name is returned if no matching name is found.
*/

static NAME find_name
    ( LIST ( NAME ) p, string id )
{
    while ( !IS_NULL_list ( p ) ) {
	NAME a = DEREF_name ( HEAD_list ( p ) ) ;
	string nm = DEREF_string ( name_id ( a ) ) ;
	if ( streq ( nm, id ) ) return ( a ) ;
	p = TAIL_list ( p ) ;
    }
    return ( NULL_name ) ;
}


/*
    This routine searches the parameter list p for an identifier matching
    id.  The null parameter is returned if no matching parameter is found.
*/

static PARAM find_param
    ( LIST ( PARAM ) p, string id )
{
    while ( !IS_NULL_list ( p ) ) {
	PARAM a = DEREF_param ( HEAD_list ( p ) ) ;
	string nm = DEREF_string ( param_name ( a ) ) ;
	if ( streq ( nm, id ) ) return ( a ) ;
	p = TAIL_list ( p ) ;
    }
    return ( NULL_param ) ;
}


#line 99 "syntax.c"

/* BEGINNING OF FUNCTION DECLARATIONS */

static void ZRparam_Hlist(LIST_PARAM *);
static void ZRname_Hlist(LIST_NAME *, LIST_NAME *, LIST_NAME *);
static void ZRname_Hseq(LIST_NAME *, LIST_NAME *, LIST_NAME *);
static void ZRprops_Hlist(LIST_PROPERTY *);
static void ZRmap_Hlist(LIST_PARAM, LIST_MAP *);
static void ZRentry(ENTRY *);
extern void read_errors(void);
static void ZRmessage_Hlist(LIST_PARAM, LIST_MESSAGE *);
static void ZRsignature(LIST_PARAM *);
static void ZRentries_Hlist(LIST_ENTRY *);

/* BEGINNING OF STATIC VARIABLES */


/* BEGINNING OF FUNCTION DEFINITIONS */

static void
ZRparam_Hlist(LIST_PARAM *ZOp)
{
	LIST_PARAM ZIp;

	if ((CURRENT_TERMINAL) == 26) {
		return;
	}
	{
		string ZIa;
		TYPE ZIt;
		string ZIb;
		PARAM ZIc;
		LIST_PARAM ZIq;

		switch (CURRENT_TERMINAL) {
		case 0:
			/* BEGINNING OF EXTRACT: identifier */
			{
#line 161 "syntax.act"

    ZIa = xstrdup ( token_buff ) ;
#line 141 "syntax.c"
			}
			/* END OF EXTRACT: identifier */
			break;
		default:
			goto ZL1;
		}
		ADVANCE_LEXER;
		/* BEGINNING OF ACTION: find_type */
		{
#line 289 "syntax.act"

    NAME n = find_name ( all_types, (ZIa) ) ;
    if ( IS_NULL_name ( n ) ) {
	error ( ERR_SERIOUS, "Type '%s' not defined", (ZIa) ) ;
	MAKE_name_basic ( (ZIa), 0, n ) ;
    }
    (ZIt) = n ;
#line 159 "syntax.c"
		}
		/* END OF ACTION: find_type */
		switch (CURRENT_TERMINAL) {
		case 16:
			break;
		default:
			goto ZL1;
		}
		ADVANCE_LEXER;
		switch (CURRENT_TERMINAL) {
		case 0:
			/* BEGINNING OF EXTRACT: identifier */
			{
#line 161 "syntax.act"

    ZIb = xstrdup ( token_buff ) ;
#line 176 "syntax.c"
			}
			/* END OF EXTRACT: identifier */
			break;
		default:
			goto ZL1;
		}
		ADVANCE_LEXER;
		/* BEGINNING OF ACTION: make_param */
		{
#line 336 "syntax.act"

    MAKE_param_basic ( (ZIt), (ZIb), counter, (ZIc) ) ;
    counter++ ;
#line 190 "syntax.c"
		}
		/* END OF ACTION: make_param */
		/* BEGINNING OF INLINE: 79 */
		{
			switch (CURRENT_TERMINAL) {
			case 17:
				{
					ADVANCE_LEXER;
					ZRparam_Hlist (&ZIq);
					if ((CURRENT_TERMINAL) == 26) {
						RESTORE_LEXER;
						goto ZL1;
					}
				}
				break;
			default:
				{
					/* BEGINNING OF ACTION: empty_param_list */
					{
#line 225 "syntax.act"

    (ZIq) = NULL_list ( PARAM ) ;
#line 213 "syntax.c"
					}
					/* END OF ACTION: empty_param_list */
				}
				break;
			}
		}
		/* END OF INLINE: 79 */
		/* BEGINNING OF ACTION: cons_param_list */
		{
#line 229 "syntax.act"

    string id = DEREF_string ( param_name ( (ZIc) ) ) ;
    PARAM b = find_param ( (ZIq), id ) ;
    if ( !IS_NULL_param ( b ) ) {
	error ( ERR_SERIOUS, "Parameter '%s' defined twice", id ) ;
    }
    CONS_param ( (ZIc), (ZIq), (ZIp) ) ;
#line 231 "syntax.c"
		}
		/* END OF ACTION: cons_param_list */
	}
	goto ZL0;
ZL1:;
	SAVE_LEXER (26);
	return;
ZL0:;
	*ZOp = ZIp;
}

static void
ZRname_Hlist(LIST_NAME *ZOp, LIST_NAME *ZOq, LIST_NAME *ZOr)
{
	LIST_NAME ZIp;
	LIST_NAME ZIq;
	LIST_NAME ZIr;

	switch (CURRENT_TERMINAL) {
	case 0:
		{
			ZRname_Hseq (&ZIp, &ZIq, &ZIr);
			if ((CURRENT_TERMINAL) == 26) {
				RESTORE_LEXER;
				goto ZL1;
			}
		}
		break;
	default:
		{
			/* BEGINNING OF ACTION: empty_name_list */
			{
#line 246 "syntax.act"

    (ZIp) = NULL_list ( NAME ) ;
#line 267 "syntax.c"
			}
			/* END OF ACTION: empty_name_list */
			/* BEGINNING OF ACTION: empty_name_list */
			{
#line 246 "syntax.act"

    (ZIq) = NULL_list ( NAME ) ;
#line 275 "syntax.c"
			}
			/* END OF ACTION: empty_name_list */
			/* BEGINNING OF ACTION: empty_name_list */
			{
#line 246 "syntax.act"

    (ZIr) = NULL_list ( NAME ) ;
#line 283 "syntax.c"
			}
			/* END OF ACTION: empty_name_list */
		}
		break;
	case 26:
		return;
	}
	goto ZL0;
ZL1:;
	SAVE_LEXER (26);
	return;
ZL0:;
	*ZOp = ZIp;
	*ZOq = ZIq;
	*ZOr = ZIr;
}

static void
ZRname_Hseq(LIST_NAME *ZOp, LIST_NAME *ZOq, LIST_NAME *ZOr)
{
	LIST_NAME ZIp;
	LIST_NAME ZIq;
	LIST_NAME ZIr;

	if ((CURRENT_TERMINAL) == 26) {
		return;
	}
	{
		string ZIa;
		NAME ZIn;
		NAME ZIm;
		NAME ZIk;
		LIST_NAME ZIp1;
		LIST_NAME ZIq1;
		LIST_NAME ZIr1;

		switch (CURRENT_TERMINAL) {
		case 0:
			/* BEGINNING OF EXTRACT: identifier */
			{
#line 161 "syntax.act"

    ZIa = xstrdup ( token_buff ) ;
#line 327 "syntax.c"
			}
			/* END OF EXTRACT: identifier */
			break;
		default:
			goto ZL1;
		}
		ADVANCE_LEXER;
		/* BEGINNING OF ACTION: make_name */
		{
#line 327 "syntax.act"

    MAKE_name_basic ( (ZIa), counter, (ZIn) ) ;
    counter++ ;
#line 341 "syntax.c"
		}
		/* END OF ACTION: make_name */
		/* BEGINNING OF INLINE: 109 */
		{
			switch (CURRENT_TERMINAL) {
			case 18:
				{
					string ZIb;
					NAME ZIk1;

					ADVANCE_LEXER;
					switch (CURRENT_TERMINAL) {
					case 0:
						/* BEGINNING OF EXTRACT: identifier */
						{
#line 161 "syntax.act"

    ZIb = xstrdup ( token_buff ) ;
#line 360 "syntax.c"
						}
						/* END OF EXTRACT: identifier */
						break;
					default:
						goto ZL1;
					}
					ADVANCE_LEXER;
					/* BEGINNING OF ACTION: make_name_aux */
					{
#line 332 "syntax.act"

    MAKE_name_basic ( (ZIb), 0, (ZIm) ) ;
#line 373 "syntax.c"
					}
					/* END OF ACTION: make_name_aux */
					/* BEGINNING OF INLINE: 110 */
					{
						switch (CURRENT_TERMINAL) {
						case 19:
							{
								string ZIc;

								ADVANCE_LEXER;
								switch (CURRENT_TERMINAL) {
								case 0:
									/* BEGINNING OF EXTRACT: identifier */
									{
#line 161 "syntax.act"

    ZIc = xstrdup ( token_buff ) ;
#line 391 "syntax.c"
									}
									/* END OF EXTRACT: identifier */
									break;
								default:
									goto ZL1;
								}
								ADVANCE_LEXER;
								/* BEGINNING OF ACTION: make_name_aux */
								{
#line 332 "syntax.act"

    MAKE_name_basic ( (ZIc), 0, (ZIk1) ) ;
#line 404 "syntax.c"
								}
								/* END OF ACTION: make_name_aux */
							}
							break;
						default:
							{
								ZIk1 = ZIm;
							}
							break;
						}
					}
					/* END OF INLINE: 110 */
					ZIk = ZIk1;
				}
				break;
			default:
				{
					ZIm = ZIn;
					ZIk = ZIn;
				}
				break;
			}
		}
		/* END OF INLINE: 109 */
		/* BEGINNING OF INLINE: 116 */
		{
			switch (CURRENT_TERMINAL) {
			case 17:
				{
					ADVANCE_LEXER;
					ZRname_Hseq (&ZIp1, &ZIq1, &ZIr1);
					if ((CURRENT_TERMINAL) == 26) {
						RESTORE_LEXER;
						goto ZL1;
					}
				}
				break;
			default:
				{
					/* BEGINNING OF ACTION: empty_name_list */
					{
#line 246 "syntax.act"

    (ZIp1) = NULL_list ( NAME ) ;
#line 449 "syntax.c"
					}
					/* END OF ACTION: empty_name_list */
					/* BEGINNING OF ACTION: empty_name_list */
					{
#line 246 "syntax.act"

    (ZIq1) = NULL_list ( NAME ) ;
#line 457 "syntax.c"
					}
					/* END OF ACTION: empty_name_list */
					/* BEGINNING OF ACTION: empty_name_list */
					{
#line 246 "syntax.act"

    (ZIr1) = NULL_list ( NAME ) ;
#line 465 "syntax.c"
					}
					/* END OF ACTION: empty_name_list */
				}
				break;
			}
		}
		/* END OF INLINE: 116 */
		/* BEGINNING OF ACTION: cons_name_list */
		{
#line 250 "syntax.act"

    string id = DEREF_string ( name_id ( (ZIn) ) ) ;
    NAME b = find_name ( (ZIp1), id ) ;
    if ( !IS_NULL_name ( b ) ) {
	error ( ERR_SERIOUS, "Name '%s' given twice in list", id ) ;
    }
    CONS_name ( (ZIn), (ZIp1), (ZIp) ) ;
#line 483 "syntax.c"
		}
		/* END OF ACTION: cons_name_list */
		/* BEGINNING OF ACTION: join_name_list */
		{
#line 259 "syntax.act"

    CONS_name ( (ZIm), (ZIq1), (ZIq) ) ;
#line 491 "syntax.c"
		}
		/* END OF ACTION: join_name_list */
		/* BEGINNING OF ACTION: join_name_list */
		{
#line 259 "syntax.act"

    CONS_name ( (ZIk), (ZIr1), (ZIr) ) ;
#line 499 "syntax.c"
		}
		/* END OF ACTION: join_name_list */
	}
	goto ZL0;
ZL1:;
	SAVE_LEXER (26);
	return;
ZL0:;
	*ZOp = ZIp;
	*ZOq = ZIq;
	*ZOr = ZIr;
}

static void
ZRprops_Hlist(LIST_PROPERTY *ZOp)
{
	LIST_PROPERTY ZIp;

	if ((CURRENT_TERMINAL) == 26) {
		return;
	}
	{
		string ZIa;
		PROPERTY ZIb;
		LIST_PROPERTY ZIq;

		switch (CURRENT_TERMINAL) {
		case 0:
			/* BEGINNING OF EXTRACT: identifier */
			{
#line 161 "syntax.act"

    ZIa = xstrdup ( token_buff ) ;
#line 533 "syntax.c"
			}
			/* END OF EXTRACT: identifier */
			break;
		default:
			goto ZL1;
		}
		ADVANCE_LEXER;
		/* BEGINNING OF ACTION: find_props */
		{
#line 280 "syntax.act"

    NAME n = find_name ( all_props, (ZIa) ) ;
    if ( IS_NULL_name ( n ) ) {
	error ( ERR_SERIOUS, "Property '%s' not defined", (ZIa) ) ;
	MAKE_name_basic ( (ZIa), 0, n ) ;
    }
    (ZIb) = n ;
#line 551 "syntax.c"
		}
		/* END OF ACTION: find_props */
		/* BEGINNING OF INLINE: 71 */
		{
			switch (CURRENT_TERMINAL) {
			case 17:
				{
					ADVANCE_LEXER;
					ZRprops_Hlist (&ZIq);
					if ((CURRENT_TERMINAL) == 26) {
						RESTORE_LEXER;
						goto ZL1;
					}
				}
				break;
			default:
				{
					/* BEGINNING OF ACTION: empty_props_list */
					{
#line 217 "syntax.act"

    (ZIq) = NULL_list ( PROPERTY ) ;
#line 574 "syntax.c"
					}
					/* END OF ACTION: empty_props_list */
				}
				break;
			}
		}
		/* END OF INLINE: 71 */
		/* BEGINNING OF ACTION: cons_props_list */
		{
#line 221 "syntax.act"

    CONS_name ( (ZIb), (ZIq), (ZIp) ) ;
#line 587 "syntax.c"
		}
		/* END OF ACTION: cons_props_list */
	}
	goto ZL0;
ZL1:;
	SAVE_LEXER (26);
	return;
ZL0:;
	*ZOp = ZIp;
}

static void
ZRmap_Hlist(LIST_PARAM ZIs, LIST_MAP *ZOp)
{
	LIST_MAP ZIp;

	switch (CURRENT_TERMINAL) {
	case 5:
		{
			string ZIa;
			KEY ZIk;
			LIST_MESSAGE ZIm1;
			LIST_MESSAGE ZIm2;
			MAP ZIn;
			LIST_MAP ZIq;

			ADVANCE_LEXER;
			switch (CURRENT_TERMINAL) {
			case 20:
				break;
			default:
				goto ZL1;
			}
			ADVANCE_LEXER;
			switch (CURRENT_TERMINAL) {
			case 0:
				/* BEGINNING OF EXTRACT: identifier */
				{
#line 161 "syntax.act"

    ZIa = xstrdup ( token_buff ) ;
#line 629 "syntax.c"
				}
				/* END OF EXTRACT: identifier */
				break;
			default:
				goto ZL1;
			}
			ADVANCE_LEXER;
			switch (CURRENT_TERMINAL) {
			case 21:
				break;
			default:
				goto ZL1;
			}
			ADVANCE_LEXER;
			/* BEGINNING OF ACTION: find_key */
			{
#line 272 "syntax.act"

    NAME n = find_name ( all_keys, (ZIa) ) ;
    if ( IS_NULL_name ( n ) ) {
	error ( ERR_SERIOUS, "Key '%s' not defined", (ZIa) ) ;
    }
    (ZIk) = n ;
#line 653 "syntax.c"
			}
			/* END OF ACTION: find_key */
			ZRmessage_Hlist (ZIs, &ZIm1);
			/* BEGINNING OF INLINE: 63 */
			{
				switch (CURRENT_TERMINAL) {
				case 19:
					{
						ADVANCE_LEXER;
						ZRmessage_Hlist (ZIs, &ZIm2);
						if ((CURRENT_TERMINAL) == 26) {
							RESTORE_LEXER;
							goto ZL1;
						}
					}
					break;
				default:
					{
						ZIm2 = ZIm1;
					}
					break;
				case 26:
					RESTORE_LEXER;
					goto ZL1;
				}
			}
			/* END OF INLINE: 63 */
			/* BEGINNING OF ACTION: make_map */
			{
#line 353 "syntax.act"

    MAKE_map_basic  ( (ZIk), (ZIm1), (ZIm2), (ZIn) ) ;
#line 686 "syntax.c"
			}
			/* END OF ACTION: make_map */
			ZRmap_Hlist (ZIs, &ZIq);
			if ((CURRENT_TERMINAL) == 26) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: cons_map_list */
			{
#line 213 "syntax.act"

    CONS_map ( (ZIn), (ZIq), (ZIp) ) ;
#line 699 "syntax.c"
			}
			/* END OF ACTION: cons_map_list */
		}
		break;
	default:
		{
			/* BEGINNING OF ACTION: empty_map_list */
			{
#line 209 "syntax.act"

    (ZIp) = NULL_list ( MAP ) ;
#line 711 "syntax.c"
			}
			/* END OF ACTION: empty_map_list */
		}
		break;
	case 26:
		return;
	}
	goto ZL0;
ZL1:;
	SAVE_LEXER (26);
	return;
ZL0:;
	*ZOp = ZIp;
}

static void
ZRentry(ENTRY *ZOe)
{
	ENTRY ZIe;

	if ((CURRENT_TERMINAL) == 26) {
		return;
	}
	{
		string ZIa;
		LIST_PARAM ZIs;
		string ZIb;
		USAGE ZIu;
		USAGE ZIv;
		LIST_PROPERTY ZIp;
		LIST_MAP ZIm;

		switch (CURRENT_TERMINAL) {
		case 0:
			/* BEGINNING OF EXTRACT: identifier */
			{
#line 161 "syntax.act"

    ZIa = xstrdup ( token_buff ) ;
#line 751 "syntax.c"
			}
			/* END OF EXTRACT: identifier */
			break;
		default:
			goto ZL1;
		}
		ADVANCE_LEXER;
		switch (CURRENT_TERMINAL) {
		case 20:
			break;
		default:
			goto ZL1;
		}
		ADVANCE_LEXER;
		ZRsignature (&ZIs);
		switch (CURRENT_TERMINAL) {
		case 21:
			break;
		case 26:
			RESTORE_LEXER;
			goto ZL1;
		default:
			goto ZL1;
		}
		ADVANCE_LEXER;
		switch (CURRENT_TERMINAL) {
		case 22:
			break;
		default:
			goto ZL1;
		}
		ADVANCE_LEXER;
		/* BEGINNING OF INLINE: 87 */
		{
			switch (CURRENT_TERMINAL) {
			case 2:
				{
					ADVANCE_LEXER;
					switch (CURRENT_TERMINAL) {
					case 16:
						break;
					default:
						goto ZL1;
					}
					ADVANCE_LEXER;
					switch (CURRENT_TERMINAL) {
					case 0:
						/* BEGINNING OF EXTRACT: identifier */
						{
#line 161 "syntax.act"

    ZIb = xstrdup ( token_buff ) ;
#line 804 "syntax.c"
						}
						/* END OF EXTRACT: identifier */
						break;
					default:
						goto ZL1;
					}
					ADVANCE_LEXER;
				}
				break;
			default:
				{
					/* BEGINNING OF ACTION: null_identifier */
					{
#line 187 "syntax.act"

    (ZIb) = NULL ;
#line 821 "syntax.c"
					}
					/* END OF ACTION: null_identifier */
				}
				break;
			}
		}
		/* END OF INLINE: 87 */
		/* BEGINNING OF INLINE: 88 */
		{
			switch (CURRENT_TERMINAL) {
			case 11:
				{
					string ZIc;
					USAGE ZIw;

					ADVANCE_LEXER;
					switch (CURRENT_TERMINAL) {
					case 16:
						break;
					default:
						goto ZL1;
					}
					ADVANCE_LEXER;
					switch (CURRENT_TERMINAL) {
					case 0:
						/* BEGINNING OF EXTRACT: identifier */
						{
#line 161 "syntax.act"

    ZIc = xstrdup ( token_buff ) ;
#line 852 "syntax.c"
						}
						/* END OF EXTRACT: identifier */
						break;
					default:
						goto ZL1;
					}
					ADVANCE_LEXER;
					/* BEGINNING OF ACTION: find_usage */
					{
#line 298 "syntax.act"

    NAME n = find_name ( all_usages, (ZIc) ) ;
    if ( IS_NULL_name ( n ) ) {
	error ( ERR_SERIOUS, "Usage '%s' not defined", (ZIc) ) ;
	MAKE_name_basic ( (ZIc), 0, n ) ;
    }
    (ZIu) = n ;
#line 870 "syntax.c"
					}
					/* END OF ACTION: find_usage */
					/* BEGINNING OF INLINE: 90 */
					{
						switch (CURRENT_TERMINAL) {
						case 19:
							{
								string ZId;

								ADVANCE_LEXER;
								switch (CURRENT_TERMINAL) {
								case 0:
									/* BEGINNING OF EXTRACT: identifier */
									{
#line 161 "syntax.act"

    ZId = xstrdup ( token_buff ) ;
#line 888 "syntax.c"
									}
									/* END OF EXTRACT: identifier */
									break;
								default:
									goto ZL1;
								}
								ADVANCE_LEXER;
								/* BEGINNING OF ACTION: find_usage */
								{
#line 298 "syntax.act"

    NAME n = find_name ( all_usages, (ZId) ) ;
    if ( IS_NULL_name ( n ) ) {
	error ( ERR_SERIOUS, "Usage '%s' not defined", (ZId) ) ;
	MAKE_name_basic ( (ZId), 0, n ) ;
    }
    (ZIw) = n ;
#line 906 "syntax.c"
								}
								/* END OF ACTION: find_usage */
							}
							break;
						default:
							{
								ZIw = ZIu;
							}
							break;
						}
					}
					/* END OF INLINE: 90 */
					ZIv = ZIw;
				}
				break;
			default:
				{
					/* BEGINNING OF ACTION: null_usage */
					{
#line 323 "syntax.act"

    (ZIu) = NULL_name ;
#line 929 "syntax.c"
					}
					/* END OF ACTION: null_usage */
					/* BEGINNING OF ACTION: null_usage */
					{
#line 323 "syntax.act"

    (ZIv) = NULL_name ;
#line 937 "syntax.c"
					}
					/* END OF ACTION: null_usage */
				}
				break;
			}
		}
		/* END OF INLINE: 88 */
		/* BEGINNING OF INLINE: 96 */
		{
			switch (CURRENT_TERMINAL) {
			case 8:
				{
					LIST_PROPERTY ZIq;

					ADVANCE_LEXER;
					switch (CURRENT_TERMINAL) {
					case 16:
						break;
					default:
						goto ZL1;
					}
					ADVANCE_LEXER;
					/* BEGINNING OF INLINE: 97 */
					{
						switch (CURRENT_TERMINAL) {
						case 0:
							{
								ZRprops_Hlist (&ZIq);
								if ((CURRENT_TERMINAL) == 26) {
									RESTORE_LEXER;
									goto ZL1;
								}
							}
							break;
						default:
							{
								/* BEGINNING OF ACTION: empty_props_list */
								{
#line 217 "syntax.act"

    (ZIq) = NULL_list ( PROPERTY ) ;
#line 979 "syntax.c"
								}
								/* END OF ACTION: empty_props_list */
							}
							break;
						}
					}
					/* END OF INLINE: 97 */
					ZIp = ZIq;
				}
				break;
			default:
				{
					/* BEGINNING OF ACTION: empty_props_list */
					{
#line 217 "syntax.act"

    (ZIp) = NULL_list ( PROPERTY ) ;
#line 997 "syntax.c"
					}
					/* END OF ACTION: empty_props_list */
				}
				break;
			}
		}
		/* END OF INLINE: 96 */
		ZRmap_Hlist (ZIs, &ZIm);
		switch (CURRENT_TERMINAL) {
		case 23:
			break;
		case 26:
			RESTORE_LEXER;
			goto ZL1;
		default:
			goto ZL1;
		}
		ADVANCE_LEXER;
		/* BEGINNING OF ACTION: make_entry */
		{
#line 358 "syntax.act"

    MAKE_entry_basic ( (ZIa), (ZIb), (ZIs), (ZIu), (ZIv), (ZIp), (ZIm), (ZIe) ) ;
    counter = 0 ;
#line 1022 "syntax.c"
		}
		/* END OF ACTION: make_entry */
	}
	goto ZL0;
ZL1:;
	SAVE_LEXER (26);
	return;
ZL0:;
	*ZOe = ZIe;
}

void
read_errors(void)
{
	if ((CURRENT_TERMINAL) == 26) {
		return;
	}
	{
		LIST_NAME ZIt1;
		LIST_NAME ZIt2;
		LIST_NAME ZIt3;
		LIST_NAME ZIp1;
		LIST_NAME ZIp2;
		LIST_NAME ZIp3;
		LIST_NAME ZIk1;
		LIST_NAME ZIk2;
		LIST_NAME ZIk3;
		LIST_NAME ZIu1;
		LIST_NAME ZIu2;
		LIST_NAME ZIu3;
		LIST_ENTRY ZIe;

		/* BEGINNING OF INLINE: 131 */
		{
			switch (CURRENT_TERMINAL) {
			case 3:
				{
					string ZId;
					string ZI158;

					ADVANCE_LEXER;
					switch (CURRENT_TERMINAL) {
					case 16:
						break;
					default:
						goto ZL1;
					}
					ADVANCE_LEXER;
					switch (CURRENT_TERMINAL) {
					case 0:
						/* BEGINNING OF EXTRACT: identifier */
						{
#line 161 "syntax.act"

    ZId = xstrdup ( token_buff ) ;
#line 1078 "syntax.c"
						}
						/* END OF EXTRACT: identifier */
						break;
					default:
						goto ZL1;
					}
					ADVANCE_LEXER;
					/* BEGINNING OF INLINE: 132 */
					{
						switch (CURRENT_TERMINAL) {
						case 19:
							{
								ADVANCE_LEXER;
								switch (CURRENT_TERMINAL) {
								case 0:
									/* BEGINNING OF EXTRACT: identifier */
									{
#line 161 "syntax.act"

    ZI158 = xstrdup ( token_buff ) ;
#line 1099 "syntax.c"
									}
									/* END OF EXTRACT: identifier */
									break;
								default:
									goto ZL1;
								}
								ADVANCE_LEXER;
							}
							break;
						default:
							{
								ZI158 = ZId;
							}
							break;
						}
					}
					/* END OF INLINE: 132 */
					/* BEGINNING OF ACTION: set_db */
					{
#line 371 "syntax.act"

    db_name = (ZId) ;
    db_name_alt = (ZI158) ;
#line 1123 "syntax.c"
					}
					/* END OF ACTION: set_db */
				}
				break;
			default:
				break;
			}
		}
		/* END OF INLINE: 131 */
		/* BEGINNING OF INLINE: 134 */
		{
			switch (CURRENT_TERMINAL) {
			case 9:
				{
					string ZIr;

					ADVANCE_LEXER;
					switch (CURRENT_TERMINAL) {
					case 16:
						break;
					default:
						goto ZL1;
					}
					ADVANCE_LEXER;
					switch (CURRENT_TERMINAL) {
					case 0:
						/* BEGINNING OF EXTRACT: identifier */
						{
#line 161 "syntax.act"

    ZIr = xstrdup ( token_buff ) ;
#line 1155 "syntax.c"
						}
						/* END OF EXTRACT: identifier */
						break;
					default:
						goto ZL1;
					}
					ADVANCE_LEXER;
					/* BEGINNING OF ACTION: set_rig */
					{
#line 376 "syntax.act"

    rig_name = (ZIr) ;
#line 1168 "syntax.c"
					}
					/* END OF ACTION: set_rig */
				}
				break;
			default:
				break;
			}
		}
		/* END OF INLINE: 134 */
		/* BEGINNING OF INLINE: 135 */
		{
			switch (CURRENT_TERMINAL) {
			case 7:
				{
					string ZIq1;
					string ZIq2;
					string ZIq3;

					ADVANCE_LEXER;
					switch (CURRENT_TERMINAL) {
					case 16:
						break;
					default:
						goto ZL1;
					}
					ADVANCE_LEXER;
					/* BEGINNING OF INLINE: 136 */
					{
						switch (CURRENT_TERMINAL) {
						case 12:
							{
								ADVANCE_LEXER;
								switch (CURRENT_TERMINAL) {
								case 15:
									break;
								default:
									goto ZL1;
								}
								ADVANCE_LEXER;
								switch (CURRENT_TERMINAL) {
								case 0:
									/* BEGINNING OF EXTRACT: identifier */
									{
#line 161 "syntax.act"

    ZIq1 = xstrdup ( token_buff ) ;
#line 1215 "syntax.c"
									}
									/* END OF EXTRACT: identifier */
									break;
								default:
									goto ZL1;
								}
								ADVANCE_LEXER;
							}
							break;
						default:
							{
								/* BEGINNING OF ACTION: null_identifier */
								{
#line 187 "syntax.act"

    (ZIq1) = NULL ;
#line 1232 "syntax.c"
								}
								/* END OF ACTION: null_identifier */
							}
							break;
						}
					}
					/* END OF INLINE: 136 */
					/* BEGINNING OF INLINE: 137 */
					{
						switch (CURRENT_TERMINAL) {
						case 13:
							{
								ADVANCE_LEXER;
								switch (CURRENT_TERMINAL) {
								case 15:
									break;
								default:
									goto ZL1;
								}
								ADVANCE_LEXER;
								switch (CURRENT_TERMINAL) {
								case 0:
									/* BEGINNING OF EXTRACT: identifier */
									{
#line 161 "syntax.act"

    ZIq2 = xstrdup ( token_buff ) ;
#line 1260 "syntax.c"
									}
									/* END OF EXTRACT: identifier */
									break;
								default:
									goto ZL1;
								}
								ADVANCE_LEXER;
							}
							break;
						default:
							{
								/* BEGINNING OF ACTION: null_identifier */
								{
#line 187 "syntax.act"

    (ZIq2) = NULL ;
#line 1277 "syntax.c"
								}
								/* END OF ACTION: null_identifier */
							}
							break;
						}
					}
					/* END OF INLINE: 137 */
					/* BEGINNING OF INLINE: 139 */
					{
						switch (CURRENT_TERMINAL) {
						case 14:
							{
								ADVANCE_LEXER;
								switch (CURRENT_TERMINAL) {
								case 15:
									break;
								default:
									goto ZL1;
								}
								ADVANCE_LEXER;
								switch (CURRENT_TERMINAL) {
								case 0:
									/* BEGINNING OF EXTRACT: identifier */
									{
#line 161 "syntax.act"

    ZIq3 = xstrdup ( token_buff ) ;
#line 1305 "syntax.c"
									}
									/* END OF EXTRACT: identifier */
									break;
								default:
									goto ZL1;
								}
								ADVANCE_LEXER;
							}
							break;
						default:
							{
								/* BEGINNING OF ACTION: null_identifier */
								{
#line 187 "syntax.act"

    (ZIq3) = NULL ;
#line 1322 "syntax.c"
								}
								/* END OF ACTION: null_identifier */
							}
							break;
						}
					}
					/* END OF INLINE: 139 */
					/* BEGINNING OF ACTION: set_prefix */
					{
#line 380 "syntax.act"

    if ( (ZIq1) ) rig_comp_output = (ZIq1) ;
    if ( (ZIq2) ) rig_from_comp = (ZIq2) ;
    if ( (ZIq3) ) rig_from_db = (ZIq3) ;
#line 1337 "syntax.c"
					}
					/* END OF ACTION: set_prefix */
				}
				break;
			default:
				{
					string ZIq1;
					string ZIq2;
					string ZIq3;

					/* BEGINNING OF ACTION: null_identifier */
					{
#line 187 "syntax.act"

    (ZIq1) = NULL ;
#line 1353 "syntax.c"
					}
					/* END OF ACTION: null_identifier */
					/* BEGINNING OF ACTION: null_identifier */
					{
#line 187 "syntax.act"

    (ZIq2) = NULL ;
#line 1361 "syntax.c"
					}
					/* END OF ACTION: null_identifier */
					/* BEGINNING OF ACTION: null_identifier */
					{
#line 187 "syntax.act"

    (ZIq3) = NULL ;
#line 1369 "syntax.c"
					}
					/* END OF ACTION: null_identifier */
					/* BEGINNING OF ACTION: set_prefix */
					{
#line 380 "syntax.act"

    if ( (ZIq1) ) rig_comp_output = (ZIq1) ;
    if ( (ZIq2) ) rig_from_comp = (ZIq2) ;
    if ( (ZIq3) ) rig_from_db = (ZIq3) ;
#line 1379 "syntax.c"
					}
					/* END OF ACTION: set_prefix */
				}
				break;
			}
		}
		/* END OF INLINE: 135 */
		/* BEGINNING OF INLINE: 141 */
		{
			switch (CURRENT_TERMINAL) {
			case 10:
				{
					ADVANCE_LEXER;
					switch (CURRENT_TERMINAL) {
					case 16:
						break;
					default:
						goto ZL1;
					}
					ADVANCE_LEXER;
					ZRname_Hlist (&ZIt1, &ZIt2, &ZIt3);
					if ((CURRENT_TERMINAL) == 26) {
						RESTORE_LEXER;
						goto ZL1;
					}
				}
				break;
			default:
				{
					/* BEGINNING OF ACTION: empty_name_list */
					{
#line 246 "syntax.act"

    (ZIt1) = NULL_list ( NAME ) ;
#line 1414 "syntax.c"
					}
					/* END OF ACTION: empty_name_list */
					/* BEGINNING OF ACTION: empty_name_list */
					{
#line 246 "syntax.act"

    (ZIt2) = NULL_list ( NAME ) ;
#line 1422 "syntax.c"
					}
					/* END OF ACTION: empty_name_list */
					/* BEGINNING OF ACTION: empty_name_list */
					{
#line 246 "syntax.act"

    (ZIt3) = NULL_list ( NAME ) ;
#line 1430 "syntax.c"
					}
					/* END OF ACTION: empty_name_list */
				}
				break;
			}
		}
		/* END OF INLINE: 141 */
		/* BEGINNING OF ACTION: set_types */
		{
#line 386 "syntax.act"

    all_types = (ZIt1) ;
    all_types_aux = (ZIt2) ;
    all_types_alt = (ZIt3) ;
    counter = 0 ;
#line 1446 "syntax.c"
		}
		/* END OF ACTION: set_types */
		/* BEGINNING OF INLINE: 145 */
		{
			switch (CURRENT_TERMINAL) {
			case 8:
				{
					ADVANCE_LEXER;
					switch (CURRENT_TERMINAL) {
					case 16:
						break;
					default:
						goto ZL1;
					}
					ADVANCE_LEXER;
					ZRname_Hlist (&ZIp1, &ZIp2, &ZIp3);
					if ((CURRENT_TERMINAL) == 26) {
						RESTORE_LEXER;
						goto ZL1;
					}
				}
				break;
			default:
				{
					/* BEGINNING OF ACTION: empty_name_list */
					{
#line 246 "syntax.act"

    (ZIp1) = NULL_list ( NAME ) ;
#line 1476 "syntax.c"
					}
					/* END OF ACTION: empty_name_list */
					/* BEGINNING OF ACTION: empty_name_list */
					{
#line 246 "syntax.act"

    (ZIp2) = NULL_list ( NAME ) ;
#line 1484 "syntax.c"
					}
					/* END OF ACTION: empty_name_list */
					/* BEGINNING OF ACTION: empty_name_list */
					{
#line 246 "syntax.act"

    (ZIp3) = NULL_list ( NAME ) ;
#line 1492 "syntax.c"
					}
					/* END OF ACTION: empty_name_list */
				}
				break;
			}
		}
		/* END OF INLINE: 145 */
		/* BEGINNING OF ACTION: set_props */
		{
#line 393 "syntax.act"

    all_props = (ZIp1) ;
    all_props_aux = (ZIp2) ;
    all_props_alt = (ZIp3) ;
    counter = 0 ;
#line 1508 "syntax.c"
		}
		/* END OF ACTION: set_props */
		/* BEGINNING OF INLINE: 148 */
		{
			switch (CURRENT_TERMINAL) {
			case 6:
				{
					ADVANCE_LEXER;
					switch (CURRENT_TERMINAL) {
					case 16:
						break;
					default:
						goto ZL1;
					}
					ADVANCE_LEXER;
					ZRname_Hlist (&ZIk1, &ZIk2, &ZIk3);
					if ((CURRENT_TERMINAL) == 26) {
						RESTORE_LEXER;
						goto ZL1;
					}
				}
				break;
			default:
				{
					/* BEGINNING OF ACTION: empty_name_list */
					{
#line 246 "syntax.act"

    (ZIk1) = NULL_list ( NAME ) ;
#line 1538 "syntax.c"
					}
					/* END OF ACTION: empty_name_list */
					/* BEGINNING OF ACTION: empty_name_list */
					{
#line 246 "syntax.act"

    (ZIk2) = NULL_list ( NAME ) ;
#line 1546 "syntax.c"
					}
					/* END OF ACTION: empty_name_list */
					/* BEGINNING OF ACTION: empty_name_list */
					{
#line 246 "syntax.act"

    (ZIk3) = NULL_list ( NAME ) ;
#line 1554 "syntax.c"
					}
					/* END OF ACTION: empty_name_list */
				}
				break;
			}
		}
		/* END OF INLINE: 148 */
		/* BEGINNING OF ACTION: set_keys */
		{
#line 400 "syntax.act"

    all_keys = (ZIk1) ;
    all_keys_aux = (ZIk2) ;
    all_keys_alt = (ZIk3) ;
    counter = 0 ;
#line 1570 "syntax.c"
		}
		/* END OF ACTION: set_keys */
		/* BEGINNING OF INLINE: 151 */
		{
			switch (CURRENT_TERMINAL) {
			case 11:
				{
					ADVANCE_LEXER;
					switch (CURRENT_TERMINAL) {
					case 16:
						break;
					default:
						goto ZL1;
					}
					ADVANCE_LEXER;
					ZRname_Hlist (&ZIu1, &ZIu2, &ZIu3);
					if ((CURRENT_TERMINAL) == 26) {
						RESTORE_LEXER;
						goto ZL1;
					}
				}
				break;
			default:
				{
					/* BEGINNING OF ACTION: empty_name_list */
					{
#line 246 "syntax.act"

    (ZIu1) = NULL_list ( NAME ) ;
#line 1600 "syntax.c"
					}
					/* END OF ACTION: empty_name_list */
					/* BEGINNING OF ACTION: empty_name_list */
					{
#line 246 "syntax.act"

    (ZIu2) = NULL_list ( NAME ) ;
#line 1608 "syntax.c"
					}
					/* END OF ACTION: empty_name_list */
					/* BEGINNING OF ACTION: empty_name_list */
					{
#line 246 "syntax.act"

    (ZIu3) = NULL_list ( NAME ) ;
#line 1616 "syntax.c"
					}
					/* END OF ACTION: empty_name_list */
				}
				break;
			}
		}
		/* END OF INLINE: 151 */
		/* BEGINNING OF ACTION: set_usages */
		{
#line 407 "syntax.act"

    all_usages = (ZIu1) ;
    all_usages_aux = (ZIu2) ;
    all_usages_alt = (ZIu3) ;
    counter = 0 ;
#line 1632 "syntax.c"
		}
		/* END OF ACTION: set_usages */
		/* BEGINNING OF INLINE: 155 */
		{
			switch (CURRENT_TERMINAL) {
			case 4:
				{
					ADVANCE_LEXER;
					switch (CURRENT_TERMINAL) {
					case 16:
						break;
					default:
						goto ZL1;
					}
					ADVANCE_LEXER;
					ZRentries_Hlist (&ZIe);
					if ((CURRENT_TERMINAL) == 26) {
						RESTORE_LEXER;
						goto ZL1;
					}
				}
				break;
			default:
				{
					/* BEGINNING OF ACTION: empty_entry_list */
					{
#line 238 "syntax.act"

    (ZIe) = NULL_list ( ENTRY ) ;
#line 1662 "syntax.c"
					}
					/* END OF ACTION: empty_entry_list */
				}
				break;
			}
		}
		/* END OF INLINE: 155 */
		/* BEGINNING OF ACTION: set_entries */
		{
#line 414 "syntax.act"

    all_entries = (ZIe) ;
    counter = 0 ;
#line 1676 "syntax.c"
		}
		/* END OF ACTION: set_entries */
		switch (CURRENT_TERMINAL) {
		case 24:
			break;
		default:
			goto ZL1;
		}
		ADVANCE_LEXER;
	}
	return;
ZL1:;
	{
		/* BEGINNING OF ACTION: syntax_error */
		{
#line 426 "syntax.act"

    error ( ERR_SERIOUS, "Syntax error" ) ;
#line 1695 "syntax.c"
		}
		/* END OF ACTION: syntax_error */
	}
}

static void
ZRmessage_Hlist(LIST_PARAM ZIs, LIST_MESSAGE *ZOp)
{
	LIST_MESSAGE ZIp;

	switch (CURRENT_TERMINAL) {
	case 0:
		{
			string ZIa;
			PARAM ZIn;
			MESSAGE ZIm;
			LIST_MESSAGE ZIq;

			/* BEGINNING OF EXTRACT: identifier */
			{
#line 161 "syntax.act"

    ZIa = xstrdup ( token_buff ) ;
#line 1719 "syntax.c"
			}
			/* END OF EXTRACT: identifier */
			ADVANCE_LEXER;
			/* BEGINNING OF ACTION: find_param */
			{
#line 307 "syntax.act"

    PARAM a = find_param ( (ZIs), (ZIa) ) ;
    if ( IS_NULL_param ( a ) ) {
	error ( ERR_SERIOUS, "Parameter '%s' not defined", (ZIa) ) ;
    }
    (ZIn) = a ;
#line 1732 "syntax.c"
			}
			/* END OF ACTION: find_param */
			/* BEGINNING OF ACTION: message_param */
			{
#line 341 "syntax.act"

    if ( !IS_NULL_param ( (ZIn) ) ) {
	MAKE_msg_param ( (ZIn), (ZIm) ) ;
    } else {
	MAKE_msg_text ( "<error>", (ZIm) ) ;
    }
#line 1744 "syntax.c"
			}
			/* END OF ACTION: message_param */
			ZRmessage_Hlist (ZIs, &ZIq);
			if ((CURRENT_TERMINAL) == 26) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: cons_message_list */
			{
#line 205 "syntax.act"

    CONS_msg ( (ZIm), (ZIq), (ZIp) ) ;
#line 1757 "syntax.c"
			}
			/* END OF ACTION: cons_message_list */
		}
		break;
	case 1:
		{
			string ZIa;
			MESSAGE ZIm;
			LIST_MESSAGE ZIq;

			/* BEGINNING OF EXTRACT: string */
			{
#line 173 "syntax.act"

    ZIa = xstrdup ( token_buff ) ;
#line 1773 "syntax.c"
			}
			/* END OF EXTRACT: string */
			ADVANCE_LEXER;
			/* BEGINNING OF ACTION: message_string */
			{
#line 349 "syntax.act"

    MAKE_msg_text ( (ZIa), (ZIm) ) ;
#line 1782 "syntax.c"
			}
			/* END OF ACTION: message_string */
			ZRmessage_Hlist (ZIs, &ZIq);
			if ((CURRENT_TERMINAL) == 26) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: cons_message_list */
			{
#line 205 "syntax.act"

    CONS_msg ( (ZIm), (ZIq), (ZIp) ) ;
#line 1795 "syntax.c"
			}
			/* END OF ACTION: cons_message_list */
		}
		break;
	default:
		{
			/* BEGINNING OF ACTION: empty_message_list */
			{
#line 201 "syntax.act"

    (ZIp) = NULL_list ( MESSAGE ) ;
#line 1807 "syntax.c"
			}
			/* END OF ACTION: empty_message_list */
		}
		break;
	case 26:
		return;
	}
	goto ZL0;
ZL1:;
	SAVE_LEXER (26);
	return;
ZL0:;
	*ZOp = ZIp;
}

static void
ZRsignature(LIST_PARAM *ZOp)
{
	LIST_PARAM ZIp;

	switch (CURRENT_TERMINAL) {
	case 0:
		{
			ZRparam_Hlist (&ZIp);
			if ((CURRENT_TERMINAL) == 26) {
				RESTORE_LEXER;
				goto ZL1;
			}
		}
		break;
	default:
		{
			/* BEGINNING OF ACTION: empty_param_list */
			{
#line 225 "syntax.act"

    (ZIp) = NULL_list ( PARAM ) ;
#line 1845 "syntax.c"
			}
			/* END OF ACTION: empty_param_list */
		}
		break;
	case 26:
		return;
	}
	goto ZL0;
ZL1:;
	SAVE_LEXER (26);
	return;
ZL0:;
	*ZOp = ZIp;
}

static void
ZRentries_Hlist(LIST_ENTRY *ZOp)
{
	LIST_ENTRY ZIp;

	switch (CURRENT_TERMINAL) {
	case 0:
		{
			ENTRY ZIa;
			LIST_ENTRY ZIq;

			ZRentry (&ZIa);
			ZRentries_Hlist (&ZIq);
			if ((CURRENT_TERMINAL) == 26) {
				RESTORE_LEXER;
				goto ZL1;
			}
			/* BEGINNING OF ACTION: cons_entry_list */
			{
#line 242 "syntax.act"

    CONS_entry ( (ZIa), (ZIq), (ZIp) ) ;
#line 1883 "syntax.c"
			}
			/* END OF ACTION: cons_entry_list */
		}
		break;
	default:
		{
			/* BEGINNING OF ACTION: empty_entry_list */
			{
#line 238 "syntax.act"

    (ZIp) = NULL_list ( ENTRY ) ;
#line 1895 "syntax.c"
			}
			/* END OF ACTION: empty_entry_list */
		}
		break;
	case 26:
		return;
	}
	goto ZL0;
ZL1:;
	SAVE_LEXER (26);
	return;
ZL0:;
	*ZOp = ZIp;
}

/* BEGINNING OF TRAILER */

#line 431 "syntax.act"

#line 1915 "syntax.c"

/* END OF FILE */
