/*
 * Copyright 2002-2011, The TenDRA Project.
 * Copyright 1997, United Kingdom Secretary of State for Defence.
 *
 * See doc/copyright/ for the full copyright terms.
 */

#include <stdio.h>
#include <string.h>

#include <shared/bool.h>
#include <shared/error.h>
#include <shared/string.h>

#include <main/driver.h>
#include <main/print.h>

#include <local/out.h>

#include <tdf/tag.h>

#include <reader/exp.h>
#include <reader/readglob.h>
#include <reader/table_fns.h>
#include <reader/basicread.h>
#include <reader/main_reads.h>

#ifdef TDF_DIAG3
#include <diag3/diag_fns.h>
#include <diag3/mark_scope.h>
#include <diag3/diagtypes.h>
#include <diag3/diaginfo1.h>
#endif
#ifdef TDF_DIAG4
#include <diag4/diag_fns.h>
#include <diag4/mark_scope.h>
#endif

#include <dwarf1/dw1_consts.h>
#include <dwarf1/dw1_types.h>
#include <dwarf1/dw1_type.h>
#include <dwarf1/dw1_out.h>
#include <dwarf1/dw1_loc.h>
#include <dwarf1/dw1_queue.h>

#ifdef TDF_DIAG4
#include <tdf/tag.h>
#endif

int continue_decs;

static long lex_blk_stk_ptr = -1;

static dwarf_label lex_blk_stk[100];

#define PUSH_LEX_BLK	(&lex_blk_stk[++lex_blk_stk_ptr])
#define POP_LEX_BLK	(&lex_blk_stk[lex_blk_stk_ptr--])
#define TOS_LEX_BLK	(&lex_blk_stk[lex_blk_stk_ptr])
#define CHK_LEX_STK	if (lex_blk_stk_ptr < -1) {		\
				error(ERR_INTERNAL, "lex stk underflow");	\
			}

static void
out_dwarf_start_scope(dwarf_label *l)
{
	char expr_buf[100];

	if (lex_blk_stk_ptr == -1) {
		return;
	}

	sprintf(expr_buf, "%s - %s", LAB2CHAR(l->beg),
		LAB2CHAR(TOS_LEX_BLK->beg));
	OUT_DWARF_ATTR(AT_start_scope);
	dwarf4(expr_buf);
}

#ifdef TDF_DIAG3

void
dw1_output_diag(diag_info *d, int proc_no, exp e)
{
	if (d->key == DIAG_INFO_SOURCE) {
		out_dwarf_sourcemark(&(d->data.source.beg));
		return;
	}
	{
		dwarf_label tlab;

		mark_scope(e);

		if (e->props & 0x80) {
			next_dwarf_lab(PUSH_LEX_BLK);
			OUT_DWARF_BEG(TOS_LEX_BLK);
			cont_sib_chain(TAG_lexical_block);
			OUT_DWARF_ATTR(AT_low_pc);
			dwarf4(LAB2CHAR(TOS_LEX_BLK->beg));
			OUT_DWARF_ATTR(AT_high_pc);
			dwarf4(LAB2CHAR(TOS_LEX_BLK->end));
			leave_dwarf_blk();
			make_next_new_chain();
		}

		next_dwarf_lab(&tlab);
		OUT_DWARF_BEG(&tlab);	/* always needed for start_scope */
		switch (d->key)
		{
		case DIAG_INFO_ID: {
			exp x = d->data.id_scope.access;

			if (x->tag != name_tag) {
				error(ERR_INTERNAL, "diagnosing non-identifier");
				return;
			}

			if ((base_type(d->data.id_scope.type))->key ==
			    DIAG_TYPE_INITED) {
				error(ERR_WARN, "%s %s has no diagtype... omitting",
					isparam(child(x)) ? "Formal parameter" :
					"Local variable",
					TDFSTRING2CHAR(d->data.id_scope.name));
				break;
			}
			if (isglob(child(x))) {
				if (nextg(child(x)) ->extnamed) {
					break;
				} else {
					/* static; goes out as local */
					cont_sib_chain(TAG_local_variable);
					/* only for local vars */
					out_dwarf_start_scope(&tlab);
				}
			} else if (isparam(child(x))) {
				cont_sib_chain(TAG_formal_parameter);
			} else {
				cont_sib_chain(TAG_local_variable);
				/* only for local vars */
				out_dwarf_start_scope(&tlab);
			}

			out_dwarf_name_attr(TDFSTRING2CHAR(d->data.id_scope.name));
			out_dwarf_type_attr(d->data.id_scope.type);
			if (!out_dwarf_loc_attr(x, proc_no)) {
				error(ERR_SERIOUS, "Unable to generate location info for variable '%s'",
					TDFSTRING2CHAR(d->data.id_scope.name));
			}
			leave_dwarf_blk();
			dump_type_q();
		}
			break;
		case DIAG_INFO_TYPE:
			cont_sib_chain(TAG_typedef);
			out_dwarf_start_scope(&tlab);
			out_dwarf_name_attr(TDFSTRING2CHAR(d->data.type_scope.name));
			out_dwarf_type_attr(d->data.type_scope.type);
			leave_dwarf_blk();
			break;
		case DIAG_INFO_TAG:
			fprintf(stderr, "diag_info_tag named %s\n",
				TDFSTRING2CHAR(d->data.tag_scope.name));
			if (streq(TDFSTRING2CHAR(d->data.tag_scope.name),
				    TDFSTRING2CHAR(d->data.tag_scope.type->data.t_struct.name)))
			{
				fprintf(stderr, "diag type gives name as %s\n",
					TDFSTRING2CHAR(d->data.tag_scope.type->data.t_struct.name));
				error(ERR_INTERNAL, "different names in output_diag");
			}
			out_dwarf_user_type(d->data.tag_scope.type);
			break;
		default:
			error(ERR_INTERNAL, "Illegal key in output_diag. key was %d", d->key);
		}
	}
}


void
dw1_output_end_scope(diag_info *d, exp e)
{
	/* TODO: asm_comment("END diag_info key %d", d->key); avoid x86 outnl side effect */
	if (d->key != DIAG_INFO_SOURCE && e->props & 0x80) {
		OUT_DWARF_END(POP_LEX_BLK);
		CHK_LEX_STK;
		end_sib_chain();
	}
}

#endif

#ifdef TDF_DIAG4

static void
comment_end_scope(diag_info *d)
{
	/* TODO: asm_fcomment(as_file, "END diag_info key %d", d->key); avoid x86 outnl which has side effect */
}


void
code_diag_info(diag_info *d, int proc_no, void(*mcode)(void *), void *args)
{
	if (d == NULL) {
		(*mcode)(args);
		return;
	}
	switch (d->key) {
	case DIAG_INFO_SCOPE:
		next_dwarf_lab(PUSH_LEX_BLK);
		OUT_DWARF_BEG(TOS_LEX_BLK);
		cont_sib_chain(TAG_lexical_block);
		OUT_DWARF_ATTR(AT_low_pc);
		dwarf4(LAB2CHAR(TOS_LEX_BLK->beg));
		OUT_DWARF_ATTR(AT_high_pc);
		dwarf4(LAB2CHAR(TOS_LEX_BLK->end));
		leave_dwarf_blk();
		make_next_new_chain();
		code_diag_info(d->more, proc_no, mcode, args);
		OUT_DWARF_END(POP_LEX_BLK);
		CHK_LEX_STK;
		end_sib_chain();
		return;
	case DIAG_INFO_SOURCE:
		out_dwarf_sourcemark(&(d->data.source.beg));
		code_diag_info(d->more, proc_no, mcode, args);
		comment_end_scope(d);
		return;
	case DIAG_INFO_ID: {
		exp x = d->data.id_scope.access;
		dwarf_label tlab;
		next_dwarf_lab(&tlab);
		OUT_DWARF_BEG(&tlab);	/* always needed for start_scope */
		for (;;) {
			if (x->tag != hold_tag) {
				error(ERR_INTERNAL, "access should be in hold");
				break;
			}
			x = child(x);
			if (x->tag == cont_tag && child(x)->tag == name_tag &&
			    isvar(child(child(x)))) {
				x = child(x);
			}
			if ((x->tag != name_tag || isdiscarded(x)) &&
			    x->tag != val_tag && x->tag != null_tag) {
				break;	/* should do better ! */
			}

			if ((base_type(d->data.id_scope.type))->key ==
			    DIAG_TYPE_INITED) {
				error(ERR_WARN, "%s has no diagtype... omitting",
					TDFSTRING2CHAR(d->data.id_scope.name));
				break;
			}
			if (x->tag == name_tag && isglob(child(x))) {
				if (nextg(child(x)) ->extnamed) {
					break;
				} else {
					/* static; goes out as local */
					cont_sib_chain(TAG_local_variable);
					/* only for local vars */
					out_dwarf_start_scope(&tlab);
				}
			} else if (x->tag == name_tag && isparam(child(x))) {
				cont_sib_chain(TAG_formal_parameter);
			} else {
				cont_sib_chain(TAG_local_variable);
				/* only for local vars */
				out_dwarf_start_scope(&tlab);
			}

			out_dwarf_name_attr(TDFSTRING2CHAR(d->data.id_scope.name));
			out_dwarf_type_attr(d->data.id_scope.type);
			if (!out_dwarf_loc_attr(x, proc_no)) {
				error(ERR_SERIOUS, "Unable to generate location info for variable '%s'",
					TDFSTRING2CHAR(d->data.id_scope.name));
			}
			leave_dwarf_blk();
			dump_type_q();
			break;
		}
		code_diag_info(d->more, proc_no, mcode, args);
		comment_end_scope(d);
		return;
	}
	case DIAG_INFO_TYPE: {
		dwarf_label tlab;
		next_dwarf_lab(&tlab);
		OUT_DWARF_BEG(&tlab);	/* always needed for start_scope */
		cont_sib_chain(TAG_typedef);
		out_dwarf_start_scope(&tlab);
		out_dwarf_name_attr(TDFSTRING2CHAR(d->data.type_scope.name));
		out_dwarf_type_attr(d->data.type_scope.type);
		leave_dwarf_blk();
		code_diag_info(d->more, proc_no, mcode, args);
		comment_end_scope(d);
		return;
	}
		error(ERR_INTERNAL, "Illegal key in output_diag. key was %d", d->key);
		code_diag_info(d->more, proc_no, mcode, args);
	}
}

#endif

