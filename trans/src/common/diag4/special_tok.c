/*
 * Copyright 2002-2011, The TenDRA Project.
 * Copyright 1997, United Kingdom Secretary of State for Defence.
 *
 * See doc/copyright/ for the full copyright terms.
 */

#include <shared/bool.h>
#include <shared/xalloc.h>

#include <tdf/tag.h>

#include <reader/readglob.h>
#include <reader/table_fns.h>
#include <reader/basicread.h>
#include <reader/main_reads.h>
#include <reader/externs.h>

#include <refactor/refactor.h>

#include "dg_aux.h"
#include "mark_scope.h"
#include "dg_fns.h"
#include "diag_fns.h"
#include "special_tok.h"

exp
read_exp_to_source(exp body)
{
	dg_sourcepos s1, s2;
	bool was_within_diags = within_diags;

	within_diags = true;

	s1 = d_sourcemark();
	s2 = d_sourcemark();

	within_diags = was_within_diags;

	if (s1.file == s2.file) {
		s2.sp_key = SP_SPAN;
		s2.to_file = s2.file;
		s2.to_line = s2.from_line;
		s2.to_column = s2.from_column;
		s2.from_line = s1.from_line;
		s2.from_column = s1.from_column;
	}

	if (body->tag == proc_tag || body->tag == general_proc_tag) {
		return body;
	}

	body = f_dg_exp(body, f_singlestep_dg(s1));
	body = f_dg_exp(body, f_sourcepos_dg(s2));

	return body;
}

exp
read_diag_id_scope(exp body)
{
	dg_idname name;
	exp acc;
	dg_type type;
	dg_name dgn;
	bool was_within_diags = within_diags;

	within_diags = true;

	name = f_dg_sourcestring_idname(d_tdfstring());
	acc  = hold_refactor(d_exp());
	type = d_diag_type();

	within_diags = was_within_diags;

	dgn = f_dg_object_name(name, f_dg_null_sourcepos, type,
	                       yes_exp_option(diag_locate(acc)),
	                       no_dg_accessibility_option);

	if (isparam(child(acc)) ||
	    (child(child(acc)) != NULL && child(child(acc))->tag == chvar_tag &&
	     shape_size(sh(child(child(acc)))) < 32 &&
	     child(child(child(acc)))->tag == cont_tag &&
	     child(child(child(child(acc))))->tag == name_tag &&
	     isparam(child(child(child(child(child(acc)))))))) {
		dg_info x = dgf(body);
		if (x && x->key == DGA_PARAMS) {
			dgn->next = x->data.i_param.args;
			x->data.i_param.args = dgn;
		} else {
			body = f_dg_exp(body, f_params_dg(dgn, no_exp_option));
		}
	} else {
		body = f_dg_exp(body, f_name_decl_dg(dgn));
		mark_scope(body);
	}

	return body;
}

exp
read_diag_type_scope(exp body)
{
	dg_idname name;
	dg_type type;
	dg_name dgn;
	bool was_within_diags = within_diags;

	within_diags = true;

	name = f_dg_sourcestring_idname(d_tdfstring());
	type = d_diag_type();

	within_diags = was_within_diags;

	dgn = f_dg_type_name(name, f_dg_null_sourcepos,
	                     no_dg_accessibility_option, type, f_false,
	                     no_bool_option, no_dg_constraint_list_option);

	body = f_dg_exp(body, f_name_decl_dg(dgn));
	mark_scope2(body);

	return body;
}

exp
read_dg_exp(exp body)
{
	dg diag;
	bool was_within_diags = within_diags;

	within_diags = true;

	diag = d_dg();

	within_diags = was_within_diags;

	return f_dg_exp(body, diag);
}

