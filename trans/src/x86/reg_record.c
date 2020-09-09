/*
 * Copyright 2002-2011, The TenDRA Project.
 * Copyright 1997, United Kingdom Secretary of State for Defence.
 *
 * See doc/copyright/ for the full copyright terms.
 */

#include <stddef.h>

#include <shared/bool.h>

#include <tdf/shape.h>
#include <tdf/tag.h>

#include <reader/exp.h>

#include <local/code.h>

#include <construct/exp.h>

#include <main/flags.h>

#include "localtypes.h"
#include "operand.h"
#include "ops.h"
#include "reg_record.h"

#ifdef DWARF2
#include "dw2_extra.h"
#endif

/*
 * This collection of routines maintains a record of what is known to
 * to be in each register during code production
 */

/* init by cproc */
reg_record crt_reg_record;

/* clear out all the register records */
void
clear_reg_record(regcell *s)
{
	int i;

	for (i = 0; i < no_fixed_regs; ++i) {
		s[i].regcell_key = 4;

#ifdef DWARF2
		if (diag == DIAG_DWARF2) {
			dw_close_regassn(i, 0);
			dw_close_regassn(i, 1);
		}
#endif
	}
}

/*
 * clear out the registers not preserved over procedure calls.
 */
void
clear_low_reg_record(regcell *s)
{
	int i;

	for (i = 0; i < no_fixed_regs; ++i) {
		s[i].regcell_key = 4;

#ifdef DWARF2
		if (diag == DIAG_DWARF2) {
			dw_close_regassn(i, 0);
			dw_close_regassn(i, 1);
		}
#endif
	}
}

/* true if changing d invalidates r */
static int
inval(exp d, exp r)
{
	if ((d == NULL || d->tag == cont_tag) &&
	    (r->tag == cont_tag || (r->tag == name_tag && isglob(child(r))))) {
		return 1;
	}

	if ((r->tag == name_tag && !isvar(child(r))) || r->tag == cont_tag) {
		return eq_where(mw(d, 0), mw(r, 0));
	}

	if (r->tag == reff_tag) {
		return inval(d, child(r));
	}

	if (r->tag == addptr_tag) {
		if (next(child(r))->tag == offset_mult_tag) {
			return inval(d, child(r)) || inval(d, child(next(child(r))));
		}

		return inval(d, child(r)) || inval(d, next(child(r)));
	}

	if (r->tag == ident_tag) {
		return inval(d, child(r)) || inval(d, next(child(r)));
	}

	return 0;
}

/* true if changing d invalidates r */
int
invalidates(exp d, exp r)
{
	if (r->tag == cont_tag || r->tag == ass_tag) {
		return inval(d, child(r));
	}

	return 0;
}

/* convert register mask to register number */
static int
get_regno(int mask)
{
	int m = 1;
	int res;

	for (res = 0; res < no_fixed_regs && (mask & m) == 0; res++) {
		m = m + m;
	}

	return res;
}

/*
 * If there is a register holding the same value as is in w,
 * return this register (as a where), otherwise the where_exp
 * field of the result will be NULL.
 */
where
equiv_reg(where w, int sz)
{
	int i;
	where res;

	res.where_exp = NULL;

	if (w.where_off != 0) {
		return res;
	}

	for (i = 0; i < no_fixed_regs; i++) {
		regcell *p;

		p = &crt_reg_record[i];
		if ((p->regcell_key & 1)
			&& (sz == 0 || sz == p->first_size)
			&& eq_where_exp(p->first_dest, w.where_exp, 1, (sz == 0)))
		{
#ifdef DWARF2
			if (diag == DIAG_DWARF2) {
				dw_used_regassn(i, 0);
			}
#endif
			return reg_wheres[i];
		}

		if ((p->regcell_key & 2)
			&& (sz == 0 || sz == p->second_size)
			&& eq_where_exp(p->second_dest, w.where_exp, 1, (sz == 0)))
		{
#ifdef DWARF2
			if (diag == DIAG_DWARF2) {
				dw_used_regassn(i, 1);
			}
#endif

			return reg_wheres[i];
		}
	}

	res.where_exp = NULL;

	return res;
}

static int
is_aliased(exp dest)
{
	if (dest == NULL) {
		return 0;
	}

	if (dest->tag != cont_tag &&
	    dest->tag != ass_tag) {
		return 0;
	}

	if (child(dest)->tag == name_tag &&
	    isvar(child(child(dest))) &&
	    iscaonly(child(child(dest)))) {
		return 0;
	}

	return 1;
}

static int
shape_overlap(exp e1, exp e2)
{
	shape s1 = sh(e1);
	shape s2 = sh(e2);

	if (s1->tag <= doublehd && s1->tag > tophd && s2->tag == ptrhd) {
		return 0;
	}

	if (s2->tag <= doublehd && s2->tag > tophd && s1->tag == ptrhd) {
		return 0;
	}

	return 1;
}

/*
 * make changes to the register record needed
 * when dest receives an unknown value
 */
void
invalidate_dest(where dest)
{
	exp d = dest.where_exp;
	int regmask = (d == NULL) ? 0 : (in_reg(d) & 0x7fffffff);
	int regno;
	where weq;
	int i;

	/*
	 * This repeats the condition state check at start of move,
	 * in case contop has reset it
	 */

	if ((cond1_set && (eq_where(dest, cond1) ||
	                   invalidates(dest.where_exp, cond1.where_exp))) ||
	    (cond2_set &&
	     (eq_where(dest, cond2a) || eq_where(dest, cond2b) ||
	      invalidates(dest.where_exp, cond2a.where_exp) ||
	      invalidates(dest.where_exp, cond2b.where_exp))))
	{
		cond1_set = false;
		cond2_set = false;
	}

	if (is_aliased(dest.where_exp)) {
		for (i = 0; i < no_fixed_regs; i++) {
			regcell *pr;

			pr = &crt_reg_record[i];
			switch (pr->regcell_key) {
			case 1:
				if (is_aliased(pr->first_dest) &&
				    shape_overlap(dest.where_exp, pr->first_dest))
				{
					pr->regcell_key = 4;
#ifdef DWARF2
					if (diag == DIAG_DWARF2) {
						dw_close_regassn(i, 0);
					}
#endif
				}
				break;

			case 2:
				if (is_aliased(pr->second_dest) &&
				    shape_overlap(dest.where_exp, pr->second_dest))
				{
					pr->regcell_key = 4;
#ifdef DWARF2
					if (diag == DIAG_DWARF2) {
						dw_close_regassn(i, 1);
					}
#endif
				}
				break;

			case 3:
				if (is_aliased(pr->first_dest) &&
				    shape_overlap(dest.where_exp, pr->first_dest))
				{
					pr->regcell_key &= 2;
#ifdef DWARF2
					if (diag == DIAG_DWARF2) {
						dw_close_regassn(i, 0);
					}
#endif
				}

				if (is_aliased(pr->second_dest) &&
				    shape_overlap(dest.where_exp, pr->second_dest))
				{
					pr->regcell_key &= 1;
#ifdef DWARF2
					if (diag == DIAG_DWARF2) {
						dw_close_regassn(i, 1);
					}
#endif
				}

				if (pr->regcell_key == 0) {
					pr->regcell_key = 4;
				}

			default:
				;
			}
		}
	}

	if (regmask) {
		regno = get_regno(regmask);
		if (regno < no_fixed_regs) {
			crt_reg_record[regno].regcell_key = 4;
		}
	}

	if (regmask || d == NULL) {
		for (i = 0; i < no_fixed_regs; ++i) {
			regcell *pr;

			pr = &crt_reg_record[i];
			switch (pr->regcell_key) {
			case 1:
				if (invalidates(d, pr->first_dest)) {
					pr->regcell_key = 4;
#ifdef DWARF2
					if (diag == DIAG_DWARF2) {
						dw_close_regassn(i, 0);
					}
#endif
				}
				break;

			case 2:
				if (invalidates(d, pr->second_dest)) {
					pr->regcell_key = 4;
#ifdef DWARF2
					if (diag == DIAG_DWARF2) {
						dw_close_regassn(i, 1);
					}
#endif
				}
				break;

			case 3:
				if (invalidates(d, pr->first_dest)) {
					pr->regcell_key &= 2;
#ifdef DWARF2
					if (diag == DIAG_DWARF2) {
						dw_close_regassn(i, 0);
					}
#endif
				}

				if (invalidates(d, pr->second_dest)) {
					pr->regcell_key &= 1;
#ifdef DWARF2
					if (diag == DIAG_DWARF2) {
						dw_close_regassn(i, 1);
					}
#endif
				}

				if (pr->regcell_key == 0) {
					pr->regcell_key = 4;
				}

			default:
				;
			}
		}
		return;
	}

#ifdef DWARF2
	dw_ignore_used_regassn = 1;
#endif
	for (;;) {
		weq = equiv_reg(dest, 0);
		if (weq.where_exp == NULL) {
			break;
		}

		regmask = (in_reg(weq.where_exp) & 0x7fffffff);
		regno = get_regno(regmask);
		if (regno < no_fixed_regs) {
			crt_reg_record[regno].regcell_key = 4;
		}
	}
#ifdef DWARF2
	dw_ignore_used_regassn = 0;
#endif
}

/*
 * From is being moved to to. Make changes
 * to the register records accordingly.
 */
void
move_reg(where from, where to, shape sha)
{
	int  regmask_to = in_reg(to.where_exp);
	int  regmask_from = in_reg(from.where_exp);
	int sz = shape_size(sha);

	if (sha->tag == shrealhd) {
		return;
	}

	if (regmask_from != 0 && regmask_to != 0) {
		return;
	}

	if ((regmask_from & (int) 0x80000000) || (regmask_to & (int) 0x80000000)) {
		return;
	}

	if (from.where_off != 0 || to.where_off != 0) {
		return;
	}

	if (regmask_to && invalidates(to.where_exp, from.where_exp)) {
		return;
	}

	if (regmask_to && regmask_to < 64) {
		int regno_to = get_regno(regmask_to);
		regcell *p;

		p = &crt_reg_record[regno_to];
		switch (p->regcell_key) {
		case 1:
			p->regcell_key = 3;
			p->second_dest = from.where_exp;
			p->second_size = sz;

#ifdef DWARF2
			if (diag == DIAG_DWARF2) {
				dw_init_regassn(regno_to, 1);
			}
#endif
			break;

		case 2:
			p->regcell_key = 3;
			p->first_dest = from.where_exp;
			p->first_size = sz;

#ifdef DWARF2
			if (diag == DIAG_DWARF2) {
				dw_init_regassn(regno_to, 0);
			}
#endif
			break;

		case 3:
			break;

		default:
			p->regcell_key = 1;
			p->first_dest = from.where_exp;
			p->first_size = sz;

#ifdef DWARF2
			if (diag == DIAG_DWARF2) {
				dw_init_regassn(regno_to, 0);
			}
#endif
			break;
		}
	}

	if (regmask_from && regmask_from < 64) {
		int regno_from = get_regno(regmask_from);
		regcell *p;

		p = &crt_reg_record[regno_from];

		switch (p->regcell_key) {
		case 1:
			p->regcell_key = 3;
			p->second_dest = to.where_exp;
			p->second_size = sz;

#ifdef DWARF2
			if (diag == DIAG_DWARF2) {
				dw_init_regassn(regno_from, 1);
			}
#endif
			break;

		case 2:
			p->regcell_key = 3;
			p->first_dest = to.where_exp;
			p->first_size = sz;

#ifdef DWARF2
			if (diag == DIAG_DWARF2) {
				dw_init_regassn(regno_from, 0);
			}
#endif
			break;

		case 3:
			break;

		default:
			p->regcell_key = 1;
			p->first_dest = to.where_exp;
			p->first_size = sz;

#ifdef DWARF2
			if (diag == DIAG_DWARF2) {
				dw_init_regassn(regno_from, 0);
			}
#endif
			break;
		}
	}
}

