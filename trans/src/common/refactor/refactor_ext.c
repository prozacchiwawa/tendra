/*
 * Copyright 2002-2012, The TenDRA Project.
 * Copyright 1997, United Kingdom Secretary of State for Defence.
 *
 * See doc/copyright/ for the full copyright terms.
 */

#include <stddef.h>

#include <shared/bool.h>
#include <shared/error.h>

#include <tdf/shape.h>
#include <tdf/tag.h>

#include <construct/installglob.h>
#include <construct/exp.h>

#include <main/flags.h>

#include <refactor/refactor.h>
#include <refactor/refactor_id.h>
#include <refactor/const.h>
#include <refactor/optimise.h>
#include <refactor/refactor_ext.h>

/*
 * refactor_ext examines the external definition e to see if it can be improved.
 * It returns 1 if it makes a change, 0 otherwise.
 */
void
refactor_ext(exp e)
{
	exp def = child(e);

	/* if it is not used or there is no definition here, do nothing */
	if (no(e) == 0 || def == NULL) {
		return;
	}

	if ((!PIC_code || nextg(e)->var == 0) && !isvar(e) &&
	    (def->tag == val_tag || def->tag == real_tag ||
	     def->tag == null_tag))
	{
		while (pt(e) != NULL) {
			/* substitute constants in */
			exp q = pt(e);
			if (next(q) != NULL) {
				/* can be NULL for diags */
				exp d = copy(def);
				replace(q, d, NULL);
				kill_exp(q, q);
			} else {
				pt(e) = pt(q);
			}
		}

		return;
	}
}

