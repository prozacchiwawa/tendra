/*
 * Copyright 2002-2011, The TenDRA Project.
 * Copyright 1997-1998, United Kingdom Secretary of State for Defence.
 *
 * See doc/copyright/ for the full copyright terms.
 */

#include <stdio.h>
#include <assert.h>

#include <shared/check.h>
#include <shared/error.h>
#include <shared/xalloc.h>

#include <tdf/bitstream.h>

#include <utility/config.h>
#include <utility/version.h>
#include "c_types.h"
#include <utility/option.h>

#include <parse/preproc.h>

#include <utility/error.h>
#include <utility/catalog.h>
#include <utility/option.h>
#include <utility/buffer.h>
#include <utility/print.h>
#include <utility/ustring.h>

#include <syntax/syntax.h>

#include <parse/constant.h>
#include <parse/file.h>
#include <parse/hash.h>
#include <parse/literal.h>
#include <parse/parse.h>
#include <parse/predict.h>

#include <construct/access.h>
#include <construct/assign.h>
#include <construct/basetype.h>
#include <construct/cast.h>
#include <construct/check.h>
#include <construct/chktype.h>
#include <construct/class.h>
#include <construct/construct.h>
#include <construct/convert.h>
#include <construct/declare.h>
#include <construct/derive.h>
#include <construct/destroy.h>
#include <construct/exception.h>
#include <construct/expression.h>
#include <construct/field_iter.h>
#include <construct/function.h>
#include <construct/identifier.h>
#include <construct/initialise.h>
#include <construct/instance.h>
#include <construct/inttype.h>
#include <construct/in_order_inits.h>
#include <construct/namespace.h>
#include <construct/overload.h>
#include <construct/statement.h>
#include <construct/template.h>
#include <construct/tokdef.h>
#include <construct/token.h>

#include <output/tdf.h>
#include <output/compile.h>
#include <output/dump.h>
#include <output/tok.h>

/*
	 C99, 6.7.8, constraint 19 specifies that designated initialisers are
	 processed in order, and that each one overrides any previous initialisation
	 provided for an earlier mention of any specific subobject, so the following:

	 struct Foo { int x; const char *y; };

	 struct Foo bar[3] = { { .y = "hi there" }, { .y = "test" }, [0].x = 3 };

	 Causes bar[0] to be initialised to { .x = 3 } instead of { .y = "hi there" }

	 This struct handles one of two possibilities per tree node:

	 1) A single EXP, most likely a quoted string or aggregate initialiser is
	    specified for this node.

	 2) Somewhere inside this node, the elements in iio_refs apply to this
	    node's initialisation.
*/
#define RESIZE_MUL 3
#define RESIZE_ADD 3

void
iio_realloc_copy_in(InitialisersInOrder_t *designated_inits, EXP e) {
	int new_limit = designated_inits->iio_cur + 1;
	if (designated_inits->iio_len < new_limit) {
		designated_inits->iio_len = new_limit;
	}

	while (designated_inits->iio_cap <= designated_inits->iio_len) {
		EXP *new_refs;
		designated_inits->iio_cap =
			RESIZE_MUL * designated_inits->iio_cap + RESIZE_ADD;
		new_refs =
			xrealloc_nof(designated_inits->iio_refs, EXP, designated_inits->iio_cap);
		designated_inits->iio_refs = new_refs;
	}

	iio_replace(designated_inits, designated_inits->iio_cur++, e);
}

/* XXX - Figure out memory mgmt */
void
iio_replace(InitialisersInOrder_t *designated_inits, int target, EXP e) {
	assert(e);
	designated_inits->iio_refs[target] = e;
}

void
iio_reduce(InitialisersInOrder_t *designated_inits, int target, LIST(EXP) *r) {
	int i;
	*r = NULL_list(EXP);

	if (target >= designated_inits->iio_len) {
		return;
	}

	for (i = designated_inits->iio_len - 1; i >= target; i--) {
		CONS_list(NULL_exp, *r, *r);
		COPY_exp(HEAD_list(*r), designated_inits->iio_refs[i]);
	}

	assert(target <= designated_inits->iio_len);
	designated_inits->iio_cur = designated_inits->iio_len = target;
}

EXP
iio_pop(InitialisersInOrder_t *designated_inits) {
	EXP exp;

	assert(designated_inits->iio_len > 0);
	exp = designated_inits->iio_refs[designated_inits->iio_len - 1];
	designated_inits->iio_len = designated_inits->iio_cur = designated_inits->iio_len - 1;
	return exp;
}

void
iio_seek(InitialisersInOrder_t *designated_inits, int target) {
	assert(target <= designated_inits->iio_len);
	designated_inits->iio_cur = target;
}

void
iio_free(InitialisersInOrder_t *designated_inits) {
	xfree(designated_inits->iio_refs);
}
