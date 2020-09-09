/*
 * Copyright 2011, The TenDRA Project.
 * Copyright 1997, United Kingdom Secretary of State for Defence.
 *
 * See doc/copyright/ for the full copyright terms.
 */

/*
 * operand outputs a 80386 operand, given a "where" and the number of
 * bits the operand occupies.
 */

#include <shared/bool.h>
#include <shared/error.h>

#include <local/out.h>
#include <local/code.h>

#include <tdf/shape.h>
#include <tdf/tag.h>

#include <reader/exp.h>
#include <reader/basicread.h>
#include <reader/externs.h>
#include <reader/table_fns.h>

#include <construct/exp.h>
#include <construct/install_fns.h>

#include <flpt/flpt.h>

#include <main/flags.h>
#include <main/print.h>

#ifdef TDF_DIAG3
#include <diag3/diag_fns.h>
#endif
#ifdef TDF_DIAG4
#include <diag4/diag_fns.h>
#endif

#include "localtypes.h"
#include "instr.h"
#include "ops.h"
#include "assembler.h"
#include "operand.h"

/* All variables initialised */

int  crt_proc_id;	/* init by cproc */
int  stack_dec;		/* init by cproc */
/* current stack decrement */
exp const_list;		/* init by init_all */
/* list of constants belonging to current
	   procedure */

/*
 * Turns an exp and an offset (in bits) into a where
 */
where
mw(exp e, int off)
{
	where w;

	w.where_exp = e;
	w.where_off = off;

	return w;
}

/*
 * Compares wheres for equality of operand.
 *
 * This is also used by equiv_reg to detect invalidity of register copy,
 * in which case we need to detect overlaps, this case determined by 'overlap'
 */
int
eq_where_exp(exp a, exp b, int first, int overlap)
{
	unsigned char na;
	unsigned char nb;

rept:

	na = a->tag;
	nb = b->tag;

	if (a == b) {
		return 1;
	}

	if (na == nb) {		/* same kind of operation "equal names" */
		if (na == val_tag && !isbigval(a) && !isbigval(b)) {
			return no(a) == no(b);
		}

		if (na == ident_tag) {
			int good = child(a) != NULL && child(b) != NULL &&
			           next(child(a)) != NULL && next(child(b)) != NULL;

			if (good) {
				exp bsa = next(child(a));
				exp bsb = next(child(b));

				if (bsa->tag == name_tag && child(bsa) == a &&
				    bsb->tag == name_tag && child(bsb) == b) {
					a = child(a);
					b = child(b);
					first = 0;
					goto rept;
				}

				if (bsa->tag == reff_tag &&
				    bsb->tag == reff_tag &&
				    (overlap ? (no(bsa) & -32) == (no(bsb) & -32) : no(bsa) == no(bsb)) &&
				    child(bsa)->tag == name_tag &&
				    child(child(bsa)) == a &&
				    child(bsb)->tag == name_tag &&
				    child(child(bsb)) == b) {
					a = child(a);
					b = child(b);
					first = 0;
					goto rept;
				}
			}

			if (isglob(a) || isglob(b)) {
				return 0;
			}

			return pt(a) == pt(b) &&
			       (overlap ? (no(a) & -32) == (no(b) & -32) : no(a) == no(b));
		}

		if (na == name_tag) {
			if ((overlap ? (no(a) & -32) != (no(b) & -32) : no(a) != no(b)) ||
			    (isvar(child(a)) != isvar(child(b)))) {
				return 0;
			}

			a = child(a);
			b = child(b);
			first = 0;

			goto rept;
		}

		if (na == cont_tag || na == ass_tag) {
			a = child(a);
			b = child(b);
			first = 0;

			goto rept;
		}

		if (na == field_tag || na == reff_tag) {
			if (overlap ? (no(a) & -32) != (no(b) & -32) : no(a) != no(b)) {
				return 0;
			}

			a = child(a);
			b = child(b);
			first = 0;

			goto rept;
		}

		if (na == real_tag && sh(a)->tag == sh(b)->tag) {
			flt fa, fb;
			int  i;
			int is_zero = 1;

			fa = flptnos[no(a)];
			fb = flptnos[no(b)];

			for (i = 0; i < MANT_SIZE && fa.mant[i] == fb.mant[i]; i++) {
				if (fa.mant[i] != 0) {
					is_zero = 0;
				}
			}

			return i == MANT_SIZE &&
			       (is_zero || (fa.exp == fb.exp &&
			                    fa.sign == fb.sign));

		}

		return 0;
	} /* end equal names */

	if (na == name_tag && nb == ident_tag && first) {
		if (overlap ? (no(a) & -32) != 0 : no(a) != 0) {
			return 0;
		}

		a = child(a);
		first = 0;

		goto rept;
	}

	if (na == ident_tag && nb == name_tag && first) {
		if (overlap ? (no(b) & -32) != 0 : no(b) != 0) {
			return 0;
		}

		b = child(b);
		first = 0;

		goto rept;
	}

	if (na == cont_tag && child(a)->tag == name_tag &&
	    isvar(child(child(a))) && nb == ident_tag && first) {
		if (overlap ? (no(child(a)) & -32) != 0 : no(child(a)) != 0) {
			return 0;
		}

		a = child(child(a));
		first = 0;

		goto rept;
	}

	if (na == ident_tag && nb == cont_tag && child(b)->tag == name_tag
	    && isvar(child(child(b))) && first) {
		if (overlap ? (no(child(b)) & -32) != 0 : no(child(b)) != 0) {
			return 0;
		}

		b = child(b);
		first = 0;

		goto rept;
	}

	if ((na == cont_tag || na == ass_tag) &&
	    child(a)->tag == name_tag &&
	    isvar(child(child(a))) && nb == name_tag && !isvar(child(b))) {
		if (overlap ? (no(child(a)) & -32) != (no(b) & -32) : no(child(a)) != no(b)) {
			return 0;
		}

		a = child(child(a));
		b = child(b);
		first = 0;

		goto rept;
	}

	if ((nb == cont_tag || nb == ass_tag) &&
	    child(b)->tag == name_tag &&
	    isvar(child(child(b))) && na == name_tag && !isvar(child(a))) {
		if (overlap ? (no(child(b)) & -32) != (no(a) & -32) : no(child(b)) != no(a)) {
			return 0;
		}

		a = child(a);
		b = child(child(b));
		first = 0;

		goto rept;
	}

	if ((na == ass_tag && nb == cont_tag) ||
	    (nb == ass_tag && na == cont_tag)) {
		a = child(a);
		b = child(b);
		first = 0;

		goto rept;
	}

	return 0;
}

/*
 * Compares wheres for equality of operand
 */
int
eq_where(where wa, where wb)
{
	exp a = wa.where_exp;
	exp b = wb.where_exp;

	if (a == NULL || b == NULL) {
		return 0;
	}

	if (wa.where_off != wb.where_off) {
		return 0;
	}

	return eq_where_exp(a, b, 1, 0);
}

/*
 * Find the first register in the register bit pattern r
 */
frr
first_reg(int r)
{
	frr t;

	t.regno = 1;
	t.fr_no = 0;

	if (r == 0) {
		error(ERR_INTERNAL, "illegal register");
		return t;
	}

	while (!(t.regno & r)) {
		t.regno = t.regno << 1;
		++t.fr_no;
	}

	return t;
}

/*
 * Output operand, wh is a where
 *
 * Note that the pt field of a declaration now hold a code for the position of
 * the value (eg. reg_pl for in a register, local_pl for relative to sp etc.).
 *
 * The no field hold the location, bit pattern for register, offset (in bits)
 * for local etc. stack_dec hold the amount the stack is decremented from its
 * position at the start of the procedure (ie the place where no is measured
 * from). This is to allow for push operations. b is passed to extn to control
 * whether a bracket is output (index instructions). addr is true if we need
 * a literal address.
 */
void
operand(int le, where wh, int b, int addr)
{
	exp w = wh.where_exp;
	int  off = wh.where_off;
	unsigned char  n = w->tag;

	if (n == val_tag && !isbigval(w)) {		/* integer constant */
		int  k = no(w) + off;
		if (sh(w)->tag == offsethd && al2(sh(w)) != 1) {
			k = k / 8;
		}

		int_operand(k, le);
		return;
	}

	if (n == ident_tag || n == labst_tag) {/* can only be dest */
		switch (ptno(w)) {
		case local_pl:
			rel_sp((no(w) + off - stack_dec) / 8, b);
			return;

		case reg_pl:
			regn(no(w), off, w, le);
			return;

		default:
			error(ERR_INTERNAL, "illegal operand");
			return;
		}
	}

	if (n == name_tag) {
		exp ident = child(w);
		int  noff = no(w) + off;
		int  ni = no(ident);

		if (isglob(ident)) {
			if (sh(w)->tag == prokhd) {	/* special treatment for procedures */
				const_extn(ident, noff);
				return;
			}

			if (isvar(ident)) {
				const_extn(ident, noff);
			} else {
				extn(ident, noff, b);
			}

			return;
		}

		switch (ptno(ident)) {
		case local_pl: /* local so relative to stack pointer or fp */
			rel_sp((ni + noff - stack_dec) / 8, b);
			return;

		case callstack_pl: /* caller arg so relative to stack pointer */
			rel_cp((ni + noff - stack_dec) / 8, b);
			return;

		case par_pl: /* parameter so relative to fp */
			rel_ap((ni + noff + 32) / 8, b);
			return;

		case reg_pl: /* in a register */
			regn(ni, noff, w, le);
			return;

		case ferr_pl: /* relative to fp, depending on push space */
			rel_ap1((ni + noff) / 8, b);
			return;

		default: /* doesnt happen */
			error(ERR_INTERNAL, "illegal operand");
			return;
		}
	}

	if (n == cont_tag || n == ass_tag) {
		exp ref = child(w);
		unsigned char  s = ref->tag;

		if (addr) {
			operand(le, mw(child(w), 0), b, 0);
			return;
		}

		if (s == name_tag) {	/* content of id */
			if (!isvar(child(ref))) {
				exp ident = child(ref);

				if (ptno(ident) != reg_pl && off != 0) {
					error(ERR_INTERNAL, "illegal operand");
				}

				if (isglob(ident)) {
					if (sh(w)->tag != prokhd) {
						error(ERR_INTERNAL, "illegal operand");
					} else {
						if (PIC_code) {
							proc_extn(ident, no(ref));
						} else {
							extn(ident, no(ref), b);
						}
					}
					return;
				}

				switch (ptno(ident)) {
				case reg_pl: /* indirect from register */
					ind_reg(no(ident), no(ref), off, ref, b);
					return;

				default:
					error(ERR_INTERNAL, "illegal operand");
					return;
				}
			} else { /* variable */
				exp ident = child(ref);
				int  noff = no(ref) + off;
				int  ni = no(ident);

				if (isglob(ident)) {
					extn(ident, noff, b);
					return;
				}

				switch (ptno(ident)) {
				case local_pl:
					/* local so relative to stack pointer or fp */
					rel_sp((ni + noff - stack_dec) / 8, b);
					return;

				case callstack_pl:
					/* caller arg so relative to stack pointer */
					rel_cp((ni + noff - stack_dec) / 8, b);
					return;

				case par_pl: /* parameter so relative to fp */
					rel_ap((ni + noff + 32) / 8, b);
					return;

				case reg_pl: /* in a register */
					regn(ni, noff, ref, le);
					return;

				default: /* doesnt happen */
					error(ERR_INTERNAL, "illegal operand");
					return;
				}
			}
		} /* end of cont(name) */

		if (s == cont_tag && child(ref)->tag == name_tag &&
		    isvar(child(child(ref)))) {
			exp ident = child(child(ref));

			if (ptno(ident) != reg_pl && off != 0) {
				error(ERR_INTERNAL, "illegal operand");
			}

			if (isglob(ident)) {
				if (sh(w)->tag != prokhd) {
					error(ERR_INTERNAL, "illegal operand");
				} else {
					extn(ident, no(child(ref)), b);
				}
				return;
			}

			switch (ptno(ident)) {
			case reg_pl: /* indirect from register */
				ind_reg(no(ident), no(child(ref)), off, ref, b);
				return;

			default:
				error(ERR_INTERNAL, "illegal operand");
				return;
			}
		} /* end of cont(cont(var)) */

		if (s == reff_tag) {
			exp et = child(ref);
			unsigned char t = et->tag;

			if (t == name_tag) {
				if (isglob(child(et))) {
					extn(child(et), no(ref), b);
					return;
				}

				switch (ptno(child(et))) {
				case reg_pl:
					ind_reg(no(child(et)), no(et), (no(ref) + off), et, b);
					return;

				default:
					error(ERR_INTERNAL, "illegal operand");
					return;
				}
			}			/* end of cont(reff(name)) */

			if (t == cont_tag) {
				switch (ptno(child(child(et)))) {
				case reg_pl:
					ind_reg(no(child(child(et))), no(child(et)),
					        (no(ref) + off), child(et), b);
					return;

				default:
					error(ERR_INTERNAL, "illegal operand");
					return;
				}
			} /* end of cont(ref(cont())) */

			if (t == addptr_tag) {
				where new_w;
				new_w.where_exp = et;
				new_w.where_off = off + no(ref);
				operand(le, new_w, b, 0);
				return;
			} /* end of cont(reff(addptr())) */

			error(ERR_INTERNAL, "illegal operand");
		} /* end of cont(reff()) */

		if (s == addptr_tag) {
			exp u = next(child(ref));
			exp c = getexp(f_bottom, NULL, 0, child(ref), NULL, 0, 0, cont_tag);
			where wc, wu;

			wc.where_exp = c;
			wc.where_off = off;
			wu.where_exp = u;
			wu.where_off = 0;

			if (u->tag == name_tag || u->tag == cont_tag) {
				index_opnd(wc, wu, 1);
				return;
			} /* end of cont(addptr(-, name)) */

			if (u->tag == offset_mult_tag) {
				int  k = no(next(child (u))) / 8; /* cannot be bitfield */
				wu.where_exp = child(u);
				index_opnd(wc, wu, k);
				return;
			} /* end of cont(addptr(-, mult)) */
		} /* end of cont(addptr()) */
	} /* end of cont */


	if (n == reff_tag) {
		exp se = child(w);
		unsigned char  s = se->tag;
		if (s == name_tag) {
			if (isglob(child(se))) {
				extn(child(se), no(w), b);
				return;
			}

			switch (ptno(child(se))) {
			case reg_pl:
				ind_reg(no(child(se)), no(child(se)), no(w), se, b);
				return;

			default:
				error(ERR_INTERNAL, "illegal operand");
				return;
			}
		} /* end of reff(name)  */

		if (s == cont_tag) {
			if (isglob(child(child(se)))) {
				extn(child(child(se)), no(w), b);
				return;
			}

			switch (ptno(child(child(se)))) {
			case reg_pl:
				ind_reg(no(child(child(se))), no(child(se)), no(w), child(se), b);
				return;

			default:
				error(ERR_INTERNAL, "illegal operand");
				return;
			}
		} /* end of reff(cont()) */

		if (s == addptr_tag) {
			where ww;

			ww.where_exp = se;
			ww.where_off = off + no(w);

			operand(le, ww, b, 0);
			return;
		} /* end of reff(addptr()) */
	} /* end of reff() */

	if (n == addptr_tag) {
		exp u = next(child(w));
		exp c = getexp(f_bottom, NULL, 0, child(w), NULL, 0, 0, cont_tag);
		where wc, wu;

		wc.where_exp = c;
		wc.where_off = off;
		wu.where_exp = u;
		wu.where_off = 0;

		if (u->tag == name_tag || u->tag == cont_tag) {
			index_opnd(wc, wu, 1);
			return;
		} /* end of addptr(-, name)  */

		if (u->tag == offset_mult_tag) {
			int  k = no(next(child (u))) / 8;	/* cannot be bitfield */
			wu.where_exp = child(u);
			index_opnd(wc, wu, k);
			return;
		} /* end of addptr(-, mult) */
	} /* end of addptr() */

	if (n == real_tag || n == val_tag || n == string_tag ||
	    n == proc_tag || n == general_proc_tag) {
		int  ln;
		if (off == 0 || addr) {
			ln = next_lab();
			const_list = getexp(f_bottom, const_list, 0, w, NULL, 0, ln, 0);
			const_intnl((addr || n == proc_tag || n == general_proc_tag), ln, 0);
			return;
		}
		/* assumes this is only used just after using the first part of the constant */
		const_intnl(0, no(const_list), off);
		return;
	}

	if (n == res_tag) {
		const_intnl(0, no(w), off);
		return;
	}

	if (n == null_tag) {
		int_operand(no(w), le);
		return;
	}

	if (n == field_tag) {
		operand(le, mw(child(w), off + no(w)), b, addr);
		return;
	}

	if (n == make_lv_tag) {
		label_operand(w);
		return;
	}

	if (n == current_env_tag) {
		outbp();
		return;
	}

	if (n == env_offset_tag) {
		if (child(w)->tag == 0) {	/* must be caller arg with var_callees */
			int_operand(no(child(w)) / 8, le);
			return;
		}

		asm_printf("$");
		envoff_operand(child(w), no(w));
		return;
	}

	if (n == env_size_tag) {
		asm_printf("$");
		envsize_operand(child(child(w)));
		return;
	}

	if (n == local_free_all_tag) {
		ldisp();
		return;
	}

	if (n == clear_tag) {
		/* any legal operand will do! */
		if (sh(w)->tag >= shrealhd && sh(w)->tag <= doublehd) {
			asm_printf("%s", "%st");
			return;
		}

		switch (shape_size(sh(w))) {
		case  8: asm_printf("%s", "%al");  return;
		case 16: asm_printf("%s", "%ax");  return;
		default: asm_printf("%s", "%eax"); return;
		}
	}

	error(ERR_INTERNAL, "illegal operand");
}

