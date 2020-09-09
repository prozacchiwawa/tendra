/*
 * Copyright 2002-2011, The TenDRA Project.
 * Copyright 1997, United Kingdom Secretary of State for Defence.
 *
 * See doc/copyright/ for the full copyright terms.
 */

/*
 * This is the principal code producing module
 */

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include <shared/bool.h>
#include <shared/check.h>
#include <shared/error.h>

#include <local/ash.h>
#include <local/exp.h>
#include <local/tag.h>

#include <tdf/shape.h>
#include <tdf/tag.h>

#include <reader/exp.h>
#include <reader/externs.h>
#include <reader/basicread.h>

#include <construct/installtypes.h>
#include <construct/dec.h>
#include <construct/ash.h>
#include <construct/exp.h>

#include <flpt/flpt.h>

#include <utility/max.h>

#include <main/driver.h>
#include <main/flags.h>
#include <main/print.h>

#include <symtab/syms.h>

#include <diag3/diag_fns.h>
#include <diag3/diag_reform.h>

#include "addr.h"
#include "loc_signal.h"
#include "procrec.h"
#include "eval.h"
#include "move.h"
#include "ops.h"
#include "pseudo.h"
#include "getregs.h"
#include "guard.h"
#include "locate.h"
#include "code_here.h"
#include "inst_fmt.h"
#include "mips_ins.h"
#include "handle_sregs.h"
#include "bits.h"
#include "labels.h"
#include "regexps.h"
#include "special.h"
#include "out_ba.h"
#include "ibinasm.h"
#include "main.h"
#include "dump_distr.h"
#include "stabs_diag3.h"
#include "frames.h"
#include "regable.h"
#include "make_code.h"

extern int current_symno;
extern  procrec * procrecs;

long  fscopefile;

static ans procans;
static int result_label = 0;
static long max_args;

where nowhere;

static exp crt_proc;

static bool NONEGSHIFTS = 1;

static long aritherr_lab  = 0;
static long stackerr_lab  = 0;
static long allocaerr_lab = 0;

extern exp find_named_tag(char *, shape);
extern shape f_pointer(alignment);
extern alignment f_alignment(shape);
extern shape f_proc;

typedef struct {
	int dble;
	r2l r;
	instore ad;
} rcache;

static rcache rca[16];
static int nca = 0;

static void
do_exception(int e)
{
	baseoff b;
	b.base = 0;
	b.offset = e;
	ls_ins(i_li, 4,  b);
	b = boff(find_named_tag("__TDFhandler", f_pointer(f_alignment(f_proc))));
	ls_ins(i_lw, 25, b);
	br_ins(i_j, 25);
}

static long
trap_label(exp e)
{
	if ((errhandle(e) & 3) == 3) {
		if (aritherr_lab == 0) {
			aritherr_lab = new_label();
		}
		return aritherr_lab;
	} else {
		return no(child(pt(e)));
	}
}

static void
dump_gp(void)
{
	baseoff b;
	b.base = 29;
	b.offset = locals_offset >> 3;
	ls_ins(i_sw, 28, b);
}

static void
reset_gp(void)
{
	baseoff b;

	if (Has_vcallees) {
		b.base = local_reg;
		b.offset = (locals_offset - frame_size) >> 3;
	} else if (Has_fp) {
		b.base = 30;
		b.offset = (locals_offset - frame_size - callee_size) >> 3;
	} else {
		b.base = 29;
		b.offset = locals_offset >> 3;
	}

	ls_ins(i_lw, 28, b);
}

static void
checknan(exp e, space sp)
{
	long trap = trap_label(e);
	int r1 = getreg(sp.fixed);
	int r2 = getreg(guardreg(r1, sp).fixed);
	cop_ins(i_cfc1, r1, 31);
	rri_ins(i_and, r2, r1, 0x70); /* not including underflo - my choice */
	rrr_ins(i_xor, r1, r1, r2);
	cop_ins(i_ctc1, r1, 31);
	condri_ins(i_bne, r2, 0, trap);
}

/*
char *
usbranches(int i)
{
  switch (i) {
    case 1: return i_bleu;
    case 2: return i_bltu;
    case 3: return i_bgeu;
    case 4: return i_bgtu;
    case 5: return i_bne;
    case 6: return i_beq;
  }
}
*/

static void
testsigned(int r, long lower, long upper, long lab)
{
	condri_ins(i_blt, r, lower, lab);
	condri_ins(i_bgt, r, upper, lab);
}

static void
testusigned(int r, long maxval, long lab)
{
	condri_ins(i_bgtu, r, maxval, lab);
}

/*
char *
sbranches(int i)
{
	switch (i) {
	case 1: return i_ble;
	case 2: return i_blt;
	case 3: return i_bge;
	case 4: return i_bgt;
	case 5: return i_bne;
	case 6: return i_beq;
	}
}
*/

static char *
branches(shape s, int i)
{
	int n = s->tag;

	if (n == scharhd || n == swordhd || n == slonghd || n == offsethd) {
		switch (i) {
		case 1: return i_ble;
		case 2: return i_blt;
		case 3: return i_bge;
		case 4: return i_bgt;
		case 5: return i_bne;
		case 6: return i_beq;
		}
	} else {
		switch (i) {
		case 1: return i_bleu;
		case 2: return i_bltu;
		case 3: return i_bgeu;
		case 4: return i_bgtu;
		case 5: return i_bne;
		case 6: return i_beq;
		}
	}

	return i_beq;
}

/*
char *
ussets(int i)
{
	switch (i) {
	case 1: return i_sgtu;
	case 2: return i_sgeu;
	case 3: return i_sltu;
	case 4: return i_sleu;
	case 5: return i_seq;
	case 6: return i_sne;
	}
}

char *
ssets(int i)
{
	switch (i) {
	case 1: return i_sgt;
	case 2: return i_sge;
	case 3: return i_slt;
	case 4: return i_sle;
	case 5: return i_seq;
	case 6: return i_sne;
	}
}
*/

static char *
sets(shape s, int i)
{
	int n = s->tag;

	if (n == scharhd || n == swordhd || n == slonghd
	    || n == offsethd) {
		switch (i) {
		case 1: return i_sgt;
		case 2: return i_sge;
		case 3: return i_slt;
		case 4: return i_sle;
		case 5: return i_seq;
		case 6: return i_sne;
		}
	} else {
		switch (i) {
		case 1: return i_sgtu;
		case 2: return i_sgeu;
		case 3: return i_sltu;
		case 4: return i_sleu;
		case 5: return i_seq;
		case 6: return i_sne;
		}
	}

	return i_seq;
}

static char *
fbranches(int i)
{
	switch (i) {
	case 1: return i_c_le_s;
	case 2: return i_c_lt_s;
	case 3: return i_c_lt_s;
	case 4: return i_c_le_s;
	case 5: return i_c_eq_s;
	case 6: return i_c_eq_s;
	}

	return i_c_eq_s;
}

static char *
fdbranches(int i)
{
	switch (i) {
	case 1: return i_c_le_d;
	case 2: return i_c_lt_d;
	case 3: return i_c_lt_d;
	case 4: return i_c_le_d;
	case 5: return i_c_eq_d;
	case 6: return i_c_eq_d;
	}

	return i_c_eq_d;
}

/* used to invert TDF tests */
long notbranch[6] = {
	4, 3, 2, 1, 6, 5
};

static void
move_dlts(int dr, int sr, int szr, int mr, int bytemove)
{
	/* move szr bytes to dr from sr (use mr)- either nooverlap or dr<=sr */
	baseoff b;
	int lin = new_label();

	b.offset = 0;
	set_label(lin);
	b.base = sr;
	ls_ins((bytemove) ? i_lb : i_lw, mr, b);
	rri_ins(i_addu, sr, sr, (bytemove) ? 1 : 4);
	b.base = dr;
	ls_ins((bytemove) ? i_sb : i_sw, mr, b);
	rri_ins(i_addu, dr, dr, (bytemove) ? 1 : 4);
	rri_ins(i_subu, szr, szr, (bytemove) ? 1 : 4);
	condrr_ins(i_bne, szr, 0, lin);
}

static void
move_dgts(int dr, int sr, int szr, int mr, int bytemove)
{
	/* move szr bytes to dr from sr (use mr) with overlap and dr>sr */
	baseoff b;
	int lin = new_label();

	b.offset = (bytemove) ? -1 : -4;

	rrr_ins(i_addu, dr, dr, szr);
	rrr_ins(i_addu, sr, sr, szr);
	set_label(lin);
	b.base = sr;
	ls_ins((bytemove) ? i_lb : i_lw, mr, b);
	rri_ins(i_subu, sr, sr, (bytemove) ? 1 : 4);
	b.base = dr;
	ls_ins((bytemove) ? i_sb : i_sw, mr, b);
	rri_ins(i_subu, dr, dr, (bytemove) ? 1 : 4);
	rri_ins(i_subu, szr, szr, (bytemove) ? 1 : 4);
	condrr_ins(i_bne, szr, 0, lin);
}

static void
reset_tos(void)
{
	if (Has_tos) {
		baseoff b;

		b.base   = 30;
		b.offset = -8;

		ls_ins(i_sw, 29, b);
	}
}

/*
 * finds the last test in sequence e which is a branch to second,
 * if any, otherwise nil
 */
static exp
testlast(exp e, exp second)
{
	exp list;

	if (e->tag == test_tag && pt(e) == second) {
		return e;
	}

	if (e->tag != seq_tag) {
		return 0;
	}

	if (next(child(e))->tag == test_tag && pt(next(child(e))) == second) {
		return next(child(e));
	}

	if (next(child(e))->tag != top_tag) {
		return 0;
	}

	for (list = child(child(e)); !list->last; list = next(list))
		;

	if (list->tag == test_tag && pt(list) == second) {
		return list;
	} else {
		return 0;
	}
}

bool
last_param(exp e)
{
	if (!isparam(e)) {
		return 0;
	}

	e = next(child(e));

aa:

	if (e->tag == ident_tag && isparam(e)
		&& child(e)->tag != formal_callee_tag)
	{
		return 0;
	}

	if (e->tag == diagnose_tag) {
		e = child(e);
		goto aa;
	}

	return 1;
}

static int
regfrmdest(where *dest, space sp)
{
	switch (dest->answhere.discrim) {
	case inreg: {
		int r = regalt(dest->answhere);
		if (r != 0) {
			return r;
		}
	}

	default:
		return getreg(sp.fixed);
	}
}

static space
do_callers(exp list, space sp)
{
	int   disp = 0;
	int   spar = 4;
	int   fpar = 6;
	bool hadfixed = 0;

	/* evaluate parameters in turn */
	for (;;) {
		int   hd = sh(list)->tag;
		instore is;
		where w;
		ash ap;
		int paral;
		int parsize;
		exp par;
		par = (list->tag == caller_tag) ? child(list) : list;
		ap = ashof(sh(list));
		paral = (ap.ashalign < 32) ? 32 : ap.ashalign;
		parsize = ap.ashsize;
		w.ashwhere = ap;
		disp = rounder(disp, paral);
		spar = 4 + (disp >> 5);
		is.b.base = 29;
		is.b.offset = disp >> 3;
		is.adval = 1;

		if (disp > 96) {
			spar = 8;
			fpar = 8;
		}

		if (is_floating(hd) && disp + parsize <= 128) {
			freg frg;
			ans ansfr;
			frg.fr = fpar++;
			frg.dble = (hd != shrealhd) ? 1 : 0;
			setfregalt(ansfr, frg);
			w.answhere = ansfr;
			code_here(par, sp, w);
			/* eval parameter into floating parameter register */
			sp = guardfreg(frg.fr, sp);

			if (hadfixed) {
				setregalt(w.answhere, spar);
				move(ansfr, w, sp, 1);
				sp = guardreg(spar, sp);
				if (hd != shrealhd) {
					sp = guardreg(spar + 1, sp);
				}
			}
		} else if (valregable(sh(list)) && disp + parsize <= 128) {
			ans ansr;
			setregalt(ansr, spar);
			w.answhere = ansr;
			hadfixed = 1;
			code_here(par, sp, w);
			/* eval parameter into fixed parameter register */
			sp = guardreg(spar, sp);
		} else {
			setinsalt(w.answhere, is);
			code_here(par, sp, w);
			hadfixed = 1;

			/* eval parameter into argument space on stack */
			while (spar <= 7 && ap.ashsize > 0) {
				/* copy (parts of) any struct results into parregs */
				ls_ins(i_lw, spar, is.b);
				sp = guardreg(spar++, sp);
				is.b.offset += 4;
				ap.ashsize -= 32;
			}
		}

		if (list->tag == caller_tag) {
			no(list) = disp;
		}

		disp += parsize;
		disp = rounder(disp, 32);

		if (list->last) {
			return sp;
		}

		list = next(list);
	}
}

static void
load_reg(exp e, int r, space sp)
{
	where w;

	w.ashwhere = ashof(sh(e));

	setregalt(w.answhere, r);
	code_here(e, sp, w);
}

static int diagPIClab;

typedef struct postl_ {
	exp pl;
	struct postl_ * outer;
} postl_chain;
static postl_chain * old_pls;

static void
update_plc(postl_chain *ch, int ma)
{
	while (ch != NULL) {
		exp pl;

		for (pl = ch->pl; pl->tag == ident_tag && child(pl)->tag == caller_name_tag; pl = next(child(pl))) {
			no(pl) += (ma << 1);
		}

		ch = ch->outer;
	}
}

static void
do_callee_list(exp e, space sp)
{
	long x = ((no(e) >> 3) + 23) & ~7;
	exp list = child(e);
	instore is;
	where w;
	baseoff b;
	long disp;
	ash ap;
	disp = 0;
	rri_ins(i_subu, 29, 29, x);
	b.base = 29;
	b.offset = x - 4;
	ls_ins(i_sw, 30, b);

	update_plc(old_pls, x << 3);

	if (no(e) != 0) {
		for (;;) {
			ap = ashof(sh(list));
			disp = rounder(disp, ap.ashalign);
			is.b.offset = disp >> 3;
			is.b.base = 29;
			is.adval = 1;
			w.ashwhere = ap;
			setinsalt(w.answhere, is);
			code_here(list, sp, w);
			disp = rounder(disp + ap.ashsize, 32);

			if (list->last) {
				break;
			}

			list = next(list);
		}
	}

	update_plc(old_pls, - (x << 3));
}

static exp
find_ote(exp e, int n)
{
	exp d = father(e);

	while (d->tag != apply_general_tag) {
		d = father(d);
	}

	d = child(next(child(d))); /* list otagexps */
	while (n != 0) {
		d = next(d);
		n--;
	}

	assert(d->tag == caller_tag);
	return d;
}

/*
 * produce code for expression e, putting its result in dest using
 * t-regs given by sp. If non-zero, exitlab is the label of where the
 * code is to continue
 */
makeans
make_code(exp e, space sp, where dest, int exitlab)
{
	long constval;
	makeans mka;

tailrecurse:

	mka.lab = exitlab;
	mka.regmove = NOREG;

	switch (e->tag) {
	case ident_tag: {
		where placew;
		int   r = NOREG;
		bool remember = 0;

		/*
		 * the tag of this declaration is transparently identified with its
		 * definition, without reserving more space
		 */
		if (e->props & defer_bit) {
			e = next(child(e));
			goto tailrecurse;

		}

		if (child (e) == NULL) {
			/* I think this is historical - unused tags are now removed cleanly */
			placew = nowhere;
		} else if (child(e)->tag == caller_name_tag) {
			exp ote = find_ote(e, no(child(e)));
			long disp = no(ote);
			no(e) = (disp << 1) + 29;
			placew = nowhere;
		} else {
			ash a;
			int   n = no(e);

			a = ashof(sh(child(e)));
			if ((e->props & inreg_bits) != 0) {
				/* tag in some fixed pt reg */
				if (n == 0) {
					/* if it hasn't been already allocated into a s-reg (or r2)
					 * allocate tag into fixed t-reg ... */
					int   s = sp.fixed;
					if (e->props & notparreg) { /* ... but not a parameter reg */
						s |= 0xf0;
					}

					n = getreg(s);
					no(e) = n;
				}

				setregalt(placew.answhere, n);
			} else if ((e->props & infreg_bits) != 0) {
				/* tag in some float reg */
				freg frg;

				if (n == 0) {
					/* if it hasn't been already allocated into a s-reg (or r0)
					 * allocate tag into float-reg ... */
					int   s = sp.flt;

					if (e->props & notparreg) {
						s |= 0xc0;
					}

					n = getfreg(s);
					no(e) = n;
				} else if (n == 16) {
					/* result reg */
					n = 0;
					no(e) = 0;
				}

				frg.fr = n;
				frg.dble = (a.ashsize == 64) ? 1 : 0;
				setfregalt(placew.answhere, frg);
			} else if (isparam(e)) {
				if (child(e)->tag != formal_callee_tag) {
					long n = (no(child(e)) + frame_size + callee_size) >> 3 ; /* byte disp of params */
					instore is;
					is.adval = 1;
					no(e) = ((no(child(e)) + frame_size + callee_size - locals_offset) << 1) + 29 + Has_fp;

					if ((!Has_no_vcallers ||
					     (isvis(e) && child(e)->props != 0)) && last_param(e)) {
						/* vararg in reg ? */
						int r = rounder(no(child(e)) + shape_size(sh(child(e))), 32);

						while (r <= 96) {
							is.b.offset = (r + ((Has_fp) ? 0
							                    : frame_size + callee_size)) >> 3;
							is.b.base = 29 + Has_fp;
							ls_ins(i_sw, 4 + (r >> 5), is.b);
							r += 32;
						}
					}

					if (shape_size(sh(child(e))) == 0) {
						/* vararg ... param */
						e = next(child(e));
						goto tailrecurse;
					}

					is.b.offset = (Has_fp) ? (no(child(e)) >> 3) : n;
					is.b.base = 29 + Has_fp;
					if (endian == ENDIAN_BIG && child(e)->props != 0 && shape_size(sh(child(e))) < 32) {
						is.b.offset += (shape_size(sh(child(e))) == 8) ? 3 : 2;
						/* short promotions */
					}

					setinsalt(placew.answhere, is);
					remember = 1;
				} else {
					no(e) = ((no(child(e)) + frame_size - locals_offset) << 1)
					        + ((Has_vcallees) ? local_reg : 30);
					placew = nowhere;
				}
			} else {
				/* allocate on stack */
				int   base = n & 0x3f;
				instore is;
				is.b.base = base;
				is.b.offset = (n - base) >> 4;
				is.adval = 1;

				if (base == 29) {
					is.b.offset += locals_offset >> 3;
				} else if ((base == 30 && Has_fp)) {
					is.b.offset += ((locals_offset - frame_size - callee_size) >> 3);
				} else if ((base == local_reg && Has_vcallees)) {
					is.b.offset += ((locals_offset - frame_size) >> 3);
				}

				setinsalt(placew.answhere, is);
				remember = 1;
			}

			placew.ashwhere = a;
		}

		if (isparam(e)
		    && child(e)->tag != formal_callee_tag) {
			exp se = child(e);
			exp d = e;

			/* parameter fiddles */
			if (se->props == 0 && (d->props & inanyreg) != 0) {
				/* not originally in required reg */
				ans a;
				instore is;
				is.b.base = 29 + Has_fp;
				is.b.offset = (no(se) + ((Has_fp) ? 0 : (frame_size + callee_size))) >> 3;
				is.adval = 0;

				if (endian == ENDIAN_BIG && shape_size(sh(child(e))) < 32) {
					is.b.offset += (shape_size(sh(child(e))) == 8) ? 3 : 2;
					/* short promotions */
				}

				setinsalt(a, is);
				IGNORE move(a, placew, sp, is_signed(sh(se)));
			} else if (se->props != 0 && (d->props & inanyreg) == 0) {
				/* originally in reg and required in store */
				ans a;

				if (is_floating(sh(se)->tag)) {
					freg fr;
					fr.fr = se->props;
					fr.dble = (sh(se)->tag != shrealhd);
					setfregalt(a, fr);
				} else {
					setregalt(a, se->props);
				}

				r = move(a, placew, sp, 0);
			} else if (se->props != 0 && se->props != no(d)) {
				/* in wrong register */
				int sr = no(d);
				int tr = se->props;

				if (is_floating(sh(se)->tag)) {
					if ((fltdone & (3 << (sr << 1))) != 0) {
						rrfp_ins((sh(se)->tag != shrealhd) ? i_mov_d : i_mov_s,
						         no(d) << 1, se->props << 1);
					} else {
						se->props = sr;
						no(d) = tr;
						sp = guardfreg(tr, sp);
						/* !? swopped and moved in  dump_tag !? */
					}
				} else {
					if ((fixdone & (1 << sr)) != 0) {
						mon_ins(i_move, no(d), se->props);
					} else {
						se->props = sr;
						no(d) = tr;
						sp = guardreg(tr, sp);
						/* !? swopped for dump_tag !? */
					}
				}
			}

			/* maybe more about promotions */
		} else if (isparam(e) && child(e)->tag == formal_callee_tag) {
			exp se = child(e);
			exp d = e;
			if ((d->props & inanyreg) != 0) {
				/* callee parameter assigned to reg */
				ans a;
				instore is;

				if (Has_vcallees) {
					is.b.base = local_reg;
					is.b.offset = (no(se)) >> 3;
				} else {
					is.b.base = 30;
					is.b.offset = (no(se) - callee_size) >> 3;
				}

				is.adval = 0;
				setinsalt(a, is);
				IGNORE move(a, placew, sp, is_signed(sh(se)));
			}
		} else {
			r = code_here(child(e), sp, placew);
		}

		/* evaluate the initialisation of tag, putting it into place allocated ... */

		if (remember && r != NOREG && pt(e) != NULL && no(pt(e)) == 0
		    && eq_sze(sh(child(e)), sh(pt(e)))) {
			/* ...if it was temporarily in a register, remember it */
			if (isvar(e)) {
				keepcont(pt(e), r);
			} else {
				keepreg(pt(e), r);
			}
		}

		sp =  guard(placew, sp);
		e = next(child(e));

		goto tailrecurse; /* and evaluate the body of the declaration */
	}

	case clear_tag:
		return mka;

	case seq_tag: {
		exp t = child(child(e));

		for (;;) {
			exp next = (t->last) ? (next(child(e))) : next(t);

			if (next->tag == goto_tag) {/* gotos end sequences */
				make_code(t, sp, nowhere, no(child(pt(next))));
			} else {
				code_here(t, sp, nowhere);
			}

			if (t->last) {
				e = next(child(e));
				goto tailrecurse;
			}

			t = next(t);
		}
	}

	case cond_tag: {
		exp first = child(e);
		exp second = next(child(e));
		exp test;

		if (dest.answhere.discrim == insomereg) {
			/* must make choice of register to contain answer to cond */
			int  *sr = someregalt(dest.answhere);
			if (*sr != -1) {
				error(ERR_INTERNAL, "Somereg *2");
			}

			*sr = getreg(sp.fixed);
			setregalt(dest.answhere, *sr);
		} else if (dest.answhere.discrim == insomefreg) {
			somefreg sfr;
			freg fr;
			sfr = somefregalt(dest.answhere);
			if (*sfr.fr != -1) {
				error(ERR_INTERNAL, "Somefreg *2");
			}

			*sfr.fr = getfreg(sp.flt);
			fr.fr = *sfr.fr;
			fr.dble = sfr.dble;
			setfregalt(dest.answhere, fr);
		}

		if (first->tag == goto_tag && pt(first) == second) {
			/* first is goto second */
			no(child(second)) = 0;
			return make_code(second, sp, dest, exitlab);
		} else if (second->tag == labst_tag &&
		           next(child(second))->tag == top_tag) {
			/* second is empty */
			int   endl = (exitlab == 0) ? new_label() : exitlab;
			no(child(second)) = endl;
			make_code(first, sp, dest, endl);
			mka.lab = endl;

			return mka;
		} else if (second->tag == labst_tag &&
		           next(child(second))->tag == goto_tag) {
			/* second is goto */
			exp g = next(child(second));
			no(child(second)) = no(child(pt(g)));

			return make_code(first, sp, dest, exitlab);
		}

		test = testlast (first, second);
		if (test) {
			/* effectively an empty then part */
			int   l = (exitlab != 0) ? exitlab : new_label();
			bool rev = IsRev(test);
			ptno(test) = -l;	/* make test jump to exitlab - see test_tag: */
			test->props = notbranch[(test->props & 127) - 1];
			if (rev) {
				SetRev(test);
			}

			/* ... with inverse test */
			no(child(second)) = new_label();
			make_code(first, sp, dest, l);
			make_code(second, sp, dest, l);
			mka.lab = l;

			return mka;
		} else {
			int   fl;
			no(child(second)) = new_label();
			fl = make_code(first, sp, dest, exitlab).lab;

			{
				int l = (fl != 0) ? fl : ((exitlab != 0) ? exitlab : new_label());

				/* Alteration 4 */
				if (sh(first)->tag != bothd) {
					uncond_ins(i_b, l);
				}

				make_code(second, sp, dest, l);
				clear_all();
				mka.lab = l;

				return mka;
			}
		}
	}

	case labst_tag:
		if (no(child(e)) != 0) {
			set_label(no(child(e)));
		}

		/* can be target of long_jump; reset sp */
		if (is_loaded_lv(e) && No_S) {
			baseoff b;
			b.base = 30;

			if (Has_vcallees) {
				b.offset = -12;
				ls_ins(i_lw, local_reg, b);
			}

			if (Has_tos) {
				b.offset = -8;
				ls_ins(i_lw, 29, b);
			} else {
				rri_ins(i_subu, 29, 30, (frame_size + callee_size) >> 3);
			}
		}

		e = next(child(e));

		goto tailrecurse;

	case rep_tag: {
		exp first = child(e);
		exp second = next(first);

		/* Alteration 1: adds this line :- */
		code_here(first, sp, nowhere);
		no(child(second)) = new_label();
		e = second;

		goto tailrecurse;
	}

	case goto_tag: {
		exp gotodest = pt(e);
		int   lab = no(child(gotodest));
		clear_all();

		if (!e->last || next(e)->tag != seq_tag || !next(e)->last ||
		    next(next(e))->last || next(next(next(e))) != gotodest) {
			uncond_ins(i_b, lab);
		} /* dest is next in sequence */

		return mka;
	}

	case make_lv_tag: {
		int r = regfrmdest(&dest, sp);
		ans aa;
		condr_ins(i_la, r, no(child(pt(e))) ); /*???? */
		setregalt(aa, r);
		move(aa, dest, guardreg(r, sp), 0);
		mka.regmove = r;

		return mka;
	}

	case long_jump_tag: {
		int fp = reg_operand(child(e), sp);
		int labval = reg_operand(next(child(e)), sp);
		mon_ins(i_move, 30, fp);
		br_ins(i_j, labval);

		return mka;
	}

	case max_tag:
	case min_tag:
	case offset_max_tag: {
		exp l = child(e);
		exp r = next(l);
		shape shl = sh(l);
		int a1, a2, d;
		ans aa;
		space nsp;
		char * setins = sets(shl, 3);
		int lab = new_label();

		a1 = reg_operand(l, nsp);
		nsp = guardreg(a1, sp);
		d = regfrmdest(&dest, nsp);
		if (d == a1) {
			d = getreg(nsp.fixed);
		}

		nsp = guardreg(d, nsp);
		if (r->tag == val_tag) {
			rri_ins(setins, d, a1, no(r));
		} else {
			a2 = reg_operand(r, nsp);
			if (d == a2) {
				d = getreg(guardreg(a2, nsp).fixed);
			}
			rrr_ins(setins, d, a1, a2);
		}

		setnoreorder();
		condri_ins((e->tag != min_tag) ? i_beq : i_bne, d, 0, lab);
		rrr_ins(i_addu, d, a1, 0);
		if (r->tag == val_tag) {
			baseoff b;
			b.base = 0;
			b.offset = no(r);
			ls_ins(i_li, d, b);
		} else {
			rrr_ins(i_addu, d, a2, 0);
		}

		set_label_no_clear(lab);
		setreorder();

		setregalt(aa, d);
		move(aa, dest, guardreg(d, sp), 0);
		mka.regmove = d;

		return mka;
	}

	case absbool_tag:
	case maxlike_tag:
	case minlike_tag:
	case abslike_tag: {
		exp l = child(child(e));
		exp r = next(l);
		shape shl = sh(l);
		char *setins;
		int   n = child(e)->props;
		int   d;
		int   a1;
		int   a2;
		bool xlike = (e->tag != absbool_tag);
		ans aa;

		/* put literal operand on right */
		if (!xlike && l->tag == val_tag) {
			exp temp = l;
			l = r;
			r = temp;

			if (n <= 2) {
				n += 2;
			} else if (n <= 4) {
				n -= 2;
			}
		}

		setins = sets(shl, n);
		/* chose set instruction from test and shape */

		d = regfrmdest(&dest, sp);

		/* reg d will contain result of set (eventually) */

		a1 = reg_operand(l, sp);
		if (xlike && a1 == d) {
			sp = guardreg(a1, sp);
			d = getreg(sp.fixed);
		}

		if (r->tag == val_tag) {
			rri_ins(setins, d, a1, no(r));
		} else {
			space nsp;
			nsp = guardreg(a1, sp);
			a2 = reg_operand(r, nsp);
			if (xlike && a2 == d) {
				nsp = guardreg(a2, nsp);
				d = getreg(nsp.fixed);
			}
			rrr_ins(setins, d, a1, a2);
		}

		if (e->tag == maxlike_tag || e->tag == minlike_tag) {
			int l = new_label();
			setnoreorder();
			condri_ins((e->tag == maxlike_tag) ? i_bne : i_beq, d, 0, l);
			rrr_ins(i_addu, d, a1, 0);

			if (r->tag == val_tag) {
				baseoff b;
				b.base = 0;
				b.offset = no(r);
				ls_ins(i_li, d, b);
			} else {
				rrr_ins(i_addu, d, a2, 0);
			}

			set_label_no_clear(l);
			setreorder();
		} else if (e->tag == abslike_tag) {
			int l = new_label();
			setnoreorder();
			condri_ins(i_bne, d, 0, l);
			rrr_ins(i_addu, d, a1, 0);
			rrr_ins(i_subu, d, 0, a1);
			set_label_no_clear(l);
			setreorder();
		}

		setregalt(aa, d);
		move(aa, dest, guardreg(d, sp), 0);
		mka.regmove = d;
		return mka;
	}

	case test_tag: {
		exp l = child(e);
		exp r = next(l);
		int   lab = (ptno(e) < 0) ? -ptno(e) : no(child(pt(e)));

		/* see frig in cond_tag */
		shape shl = sh(l);
		char *branch;
		int   n = (e->props) & 127; /* could have Rev bit in props*/

		if (is_floating(sh(l)->tag)) {
			bool dble = (shl->tag != shrealhd) ? 1 : 0;
			int   a1;
			char *branch = (n <= 2 || n == 6) ? i_bc1t : i_bc1f;
			char *compare = (dble) ? fdbranches(n) : fbranches(n);

			/* choose branch and compare instructions */
			int   a2;
			space nsp;
			if (IsRev(e)) {
				a2 = freg_operand(r, sp);
				nsp = guardfreg(a2, sp);
				a1 = freg_operand(l, nsp);
			} else {
				a1 = freg_operand(l, sp);
				nsp = guardfreg(a1, sp);
				a2 = freg_operand(r, nsp);
			}

			rrfpcond_ins(compare, a1 << 1, a2 << 1);
			br_ins(branch, lab);

			return mka;
		} else {
			int a1;
			int a2;

			/* put literal operand on right */
			if (l->tag == val_tag) {
				exp temp = l;
				l = r;
				r = temp;
				if (n <= 2) {
					n += 2;
				} else if (n <= 4) {
					n -= 2;
				}
			}

			branch = branches(shl, n);
			/* choose branch instruction */
			a1 = reg_operand(l, sp);
			if (r->tag == val_tag) {
				condri_ins(branch, a1, no(r), lab);
			} else {
				space nsp;
				nsp = guardreg(a1, sp);
				a2 = reg_operand(r, nsp);
				condrr_ins(branch, a1, a2, lab);
			}

			return mka;
		}
	}

	case ass_tag:
	case assvol_tag: {
		exp lhs = child(e);
		exp rhs = next(lhs);
		where assdest;
		space nsp;
		ash arhs;

		int contreg = NOREG;
		if (e->tag == assvol_tag) {
			clear_all();
			setvolatile();
		}

		arhs = ashof(sh(rhs));

		if (e->tag == ass_tag && rhs->tag == apply_tag &&
		    (is_floating(sh(rhs)->tag) || valregable(sh(rhs)))) {
			/* if source is simple proc call, evaluate it first and do
			   assignment */
			ans aa;

			code_here(rhs, sp, nowhere);
			if (is_floating(sh(rhs)->tag)) {
				freg frg;
				frg.fr = 0;
				frg.dble = (arhs.ashsize == 64) ? 1 : 0;
				setfregalt(aa, frg);
			} else {
				setregalt(aa, 2);
				sp = guardreg(2, sp);
			}

			assdest = locate(lhs, sp, sh(rhs), 0);
			move(aa, assdest, sp, 1);
			clear_dep_reg(lhs);

			return mka;
		}

		assdest = locate(lhs, sp, sh(rhs), 0);
		nsp = guard(assdest, sp);
		/* evaluate 'address' of destination */

		if (e->tag == ass_tag
		    && assdest.answhere.discrim == notinreg)
		{
			instore is;
			is = insalt(assdest.answhere);
			if (!is.adval) {
				/* this is an indirect assignment, so make
				 * it direct by loading pointer into reg (and remember it) */
				int r = getreg(sp.fixed);

				ls_ins(i_lw, r, is.b);
				nsp = guardreg(r, sp);
				is.adval = 1;
				is.b.base = r;
				is.b.offset = 0;
				setinsalt(assdest.answhere, is);
				keepexp(lhs, assdest.answhere);
			}
		}

		contreg = code_here(rhs, nsp, assdest);
		/* evaluate source into assignment destination .... */

		switch (assdest.answhere.discrim) {
		case inreg: {
			int a = regalt(assdest.answhere);

			keepreg(rhs, a); /* remember that source has been evaluated into a */
			clear_dep_reg(lhs); /* forget register dependencies on destination */
			break;
		}

		case infreg: {
			freg frg;
			int   r;

			frg = fregalt(assdest.answhere);
			r = frg.fr + 32;

			if (frg.dble) {
				r = -r;
			}

			keepreg(rhs, r); /* remember that source has been evaluated into a */
			clear_dep_reg(lhs); /* forget register dependencies on destination */
			break;
		}

		case notinreg: {
			if (contreg != NOREG && e->tag == ass_tag) {
				clear_dep_reg(lhs);
				/* forget register dependencies on destination */

				if (lhs->tag == name_tag) {
					exp dc = child(lhs);
					exp u = pt(dc);

					while (u != NULL) {
						/* loook through uses to find cont(name) */
						if (u->last && no(u) == no(lhs) && next(u) != NULL &&
						    next(u)->tag == cont_tag &&
						    shape_size(sh(next(u))) == shape_size(sh(rhs))) {
							keepreg(next(u), contreg);
							break;
						}

						u = pt(u);
					}

					/* remember cont of name as in contreg */
				} else if (!dependson(lhs, 0, lhs)) {
					/* remember that dest contains source, provided that it
					   is not dependent on it */
					keepcont(lhs, contreg);
				}

				return mka;
			}

			clear_dep_reg(lhs);
			/* forget register dependencies on destination */
			break;
		}

		case insomereg:
		case insomefreg:
			clear_dep_reg(lhs);
			/* forget register dependencies on destination */
		}

		if (e->tag == assvol_tag) {
			setnovolatile();
		}

		return mka;
	}

	case compound_tag: {
		exp t = child(e);
		space nsp;
		instore str;
		int r;

		nsp = sp;

		switch (dest.answhere.discrim) {
		case notinreg: {
			str = insalt (dest.answhere);/* it should be !! */
			if (!str.adval) {
				int   r = getreg(sp.fixed);
				nsp = guardreg(r, sp);
				ls_ins(i_lw, r, str.b);
				str.adval = 1;
				str.b.base = r;
				str.b.offset = 0;
			}

			for (;;) {
				where newdest;
				instore newis;
				newis = str;
				newis.b.offset += no(t);

				assert(t->tag == val_tag && al2(sh(t)) >= 8);

				setinsalt(newdest.answhere, newis);
				newdest.ashwhere = ashof(sh(next(t)));
				code_here(next(t), nsp, newdest);
				if (next(t)->last) {
					return mka;
				}

				t = next(next(t));
			}
		}

		case insomereg: {
			int *sr = someregalt(dest.answhere);
			if (*sr != -1) {
				error(ERR_INTERNAL, "Somereg *2");
			}

			*sr = getreg(sp.fixed);
			setregalt(dest.answhere, *sr);

			/* ,... */
		}

		case inreg:
			code_here(next(t), sp, dest);
			r = regalt(dest.answhere);

			assert(t->tag == val_tag);

			if (no(t) != 0) {
				rri_ins(i_sll, r, r, (al2(sh(t)) >= 8) ? (no(t) << 3) : no(t));
			}

			nsp = guardreg(r, sp);
			while (!next(t)->last) {
				int z;
				t = next(next(t));
				assert(t->tag == val_tag);
				z = reg_operand(next(t), nsp);
				if (no(t) != 0) {
					rri_ins(i_sll, z, z, (al2(sh(t)) >= 8) ? (no(t) << 3) : no(t));
				}

				rrr_ins(i_or, r, r, z);
			}

			return mka;

		case insomefreg: {
			somefreg sfr;
			freg fr;

			sfr = somefregalt(dest.answhere);
			if (*sfr.fr != -1) {
				error(ERR_INTERNAL, "Somefreg *2");
			}

			*sfr.fr = getfreg(sp.flt);
			fr.fr = *sfr.fr;
			fr.dble = sfr.dble;
			setfregalt(dest.answhere, fr);
		}

		case infreg:
			code_here(next(t), sp, dest);
			if (!next(t)->last || t->tag != val_tag || no(t) != 0) {
				error(ERR_INTERNAL, "No Tuples in freg");
			}

			return mka;
		}
	}

	case nof_tag:
	case concatnof_tag: {
		exp t = child(e);
		space nsp;
		instore str;
		int r, disp = 0;

		if (t == NULL) {
			return mka;
		}

		nsp = sp;
		switch (dest.answhere.discrim) {
		case notinreg:
			str = insalt (dest.answhere);/* it should be !! */
			if (!str.adval) {
				int   r = getreg(sp.fixed);
				nsp = guardreg(r, sp);
				ls_ins(i_lw, r, str.b);
				str.adval = 1;
				str.b.base = r;
				str.b.offset = 0;
			}

			for (;;) {
				where newdest;
				instore newis;
				newis = str;
				newis.b.offset += disp;
				setinsalt(newdest.answhere, newis);
				newdest.ashwhere = ashof(sh(t));
				code_here(t, nsp, newdest);

				if (t->last) {
					return mka;
				}

				disp += (rounder(shape_size(sh(t)), shape_align(sh(next(t)))) >> 3);
				t = next(t);
			}

		case insomereg: {
			int * sr = someregalt(dest.answhere);
			if (*sr != -1) {
				error(ERR_INTERNAL, "Somereg *2");
			}

			*sr = getreg(sp.fixed);
			setregalt(dest.answhere, *sr);

			/* ,... */
		}

		case inreg:
			code_here(t, sp, dest);
			r = regalt(dest.answhere);
			nsp = guardreg(r, sp);

			while (!t->last) {
				int z;

				disp += rounder(shape_size(sh(t)), shape_align(sh(next(t))));
				t = next(t);
				z = reg_operand(t, nsp);
				rri_ins(i_sll, z, z, disp);
				rrr_ins(i_or, r, r, z);
			}

			return mka;

		default:
			error(ERR_INTERNAL, "No Tuples in freg");
		}
	}

	case ncopies_tag: {
		exp t = child(e);
		space nsp;
		instore str;
		int i, r, disp = 0;

		nsp = sp;
		switch (dest.answhere.discrim) {
		case notinreg:
			str = insalt (dest.answhere);/* it should be !! */
			if (!str.adval) {
				int r = getreg(sp.fixed);
				nsp = guardreg(r, sp);
				ls_ins(i_lw, r, str.b);
				str.adval = 1;
				str.b.base = r;
				str.b.offset = 0;
			}

			for (i = 1; i <= no(e); i++) {
				where newdest;
				instore newis;

				newis = str;
				newis.b.offset += disp;
				setinsalt(newdest.answhere, newis);
				newdest.ashwhere = ashof(sh(t));
				code_here(t, nsp, newdest);
				disp += (rounder(shape_size(sh(t)), shape_align(sh(t))) >> 3);
			}

			return mka;

		case insomereg: {
			int * sr = someregalt(dest.answhere);
			if (*sr != -1) {
				error(ERR_INTERNAL, "Somereg *2");
			}

			*sr = getreg(sp.fixed);
			setregalt(dest.answhere, *sr);

			/* ,... */
		}

		case inreg:
			code_here(t, sp, dest);
			r = regalt(dest.answhere);
			nsp = guardreg(r, sp);

			for (i = 1; i <= no(e); i++) {
				int z;

				disp += rounder(shape_size(sh(t)), shape_align(sh(t)));
				z = reg_operand(t, nsp);
				rri_ins(i_sll, z, z, disp);
				rrr_ins(i_or, r, r, z);
			}

			return mka;

		default:
			error(ERR_INTERNAL, "No Tuples in freg");
		}
	}

	case caller_tag:
		e = child(e);
		goto tailrecurse;

	case apply_general_tag: {
		exp fn = child(e);
		exp cers = next(fn);
		exp cees = next(cers);
		exp pl = next(cees);
		space nsp;

		if (no(cers) != 0) {
			nsp = do_callers(child(cers), sp);
		} else {
			nsp = sp;
		}

		IGNORE make_code(cees, nsp, nowhere, 0);

		if (fn->tag == name_tag && child(fn)->tag == ident_tag
		    && (child(child(fn)) == NULL ||
		        child(child(fn))->tag == proc_tag ||
		        child(child(fn))->tag == general_proc_tag))
		{
			/* the procedure can be entered directly */
			extj_ins(i_jal, boff(child(fn)));
		} else if (PIC_code) {
			/* have to get address of proc into r25 */
			where w;

			setregalt(w.answhere, 25);
			w.ashwhere = ashof(sh(fn));
			code_here(fn, sp, w);
			br_ins(i_jal, 25);
		} else {
			/* the address of the proc is evaluated and entered indirectly */
			clear_reg(31); /* can't use 31 as temp reg for jal */
			br_ins(i_jal, reg_operand(fn, guardreg(31, sp)));
		}

		if (PIC_code) {
			reset_gp();
		}

		clear_all(); /* forget all register memories */

		{
			int hda = sh(e)->tag;
			ans aa;

			if (is_floating(hda)) {
				freg frg;
				frg.fr = 0;
				frg.dble = (hda != shrealhd);
				setfregalt(aa, frg);
				move(aa, dest, sp, 1);
				/* move floating point result of application to destination */
			} else {
				setregalt(aa, 2);
				mka.regmove = 2;
				move(aa, dest, sp, 1);
				/* move floating point result of application to destination */
			}

			/* else struct results are moved by body of proc */
		}

		if (call_is_untidy(cees)) {
			rri_ins(i_subu, 29, 29, max_args >> 3);
			reset_tos();
			assert(pl->tag == top_tag);
		} else if (postlude_has_call(e)) {
			exp x = child(cers);
			postl_chain p;

			for (; x != NULL ;) {
				if (x->tag == caller_tag) {
					no(x) += max_args;
				}

				if (x->last) {
					break;
				}

				x = next(x);
			}

			mka.regmove = NOREG;
			update_plc(old_pls, max_args);
			p.pl = pl;
			p.outer = old_pls;
			old_pls = &p;
			rri_ins(i_subu, 29, 29, max_args >> 3);
			IGNORE make_code(pl, sp, nowhere, 0);
			rri_ins(i_addu, 29, 29, max_args >> 3);
			old_pls = p.outer;
			update_plc(old_pls, -max_args);
		} else {
			IGNORE make_code(pl, sp, nowhere, 0);
		}

		return mka;
	}

	case caller_name_tag:
		return mka;

	case make_callee_list_tag: {
		long x = ((no(e) >> 3) + 23) & ~7;
		do_callee_list(e, sp);

		if (call_has_vcallees(e)) {
			rri_ins(i_addu, 30, 29, x);
		}

		/*
		 *          | 1st callee par        |    = sf on entry
		 *          | 2nd    ""             |
		 *                 .....
		 *   -16    | callers loc reg 23    | Has_vcallees    \
		 *   -12    | callees loc reg 23    | Has_vcallees    |  4 overhd wds
		 *   -8     | callees tos           | Has_tos         |
		 *   -4     | caller's fp = $30     |                 /
		 *           -----------------------
		 *          | caller pars           |       = top of callers env
		 *          |                       |         also $30 if var callees
		 *
		 */

		return mka;
	}

	case same_callees_tag: {
		baseoff b;
		bool vc = call_has_vcallees(e);
		space nsp;
		exp bdy = child(crt_proc);

		while (bdy->tag == dump_tag || bdy->tag == diagnose_tag) {
			bdy = child(bdy);
		}

		while (bdy->tag == ident_tag && isparam(bdy)) {
			/* make sure that current callees are in right place */
			exp sbdy = child(bdy);
			if (sbdy->tag == formal_callee_tag && (bdy->props &inanyreg) != 0
			    && isvar(bdy))
			{
				baseoff b;

				if (Has_fp) {
					b.base = 30;
					b.offset = (no(sbdy) - callee_size) >> 3;
				} else {
					b.base = 29;
					b.offset = (no(sbdy) + frame_size) >> 3;
				}

				if (is_floating(sh(sbdy)->tag)) {
					lsfp_ins((sh(sbdy)->tag != shrealhd) ? i_s_d : i_s_s,
					         no(bdy) << 1, b);
				} else {
					ls_ins(i_sw, no(bdy), b);
				}
			}

			bdy = next(sbdy);
		}

		if (Has_vcallees) {
			/*  move [fp+16..local_reg] -> top of stack */

			int rsize = getreg(sp.fixed);
			int rsrce;
			int rdest;
			int t30;
			int le = new_label();
			int lb = new_label();
			nsp = guardreg(rsize, sp);
			t30 = getreg(nsp.fixed);
			nsp = guardreg(t30, nsp);
			rsrce = getreg(nsp.fixed);
			nsp = guardreg(rsrce, nsp);
			rdest = getreg(nsp.fixed);
			nsp = guardreg(rdest, nsp);

			mon_ins(i_move, t30, 29);
			rrr_ins(i_subu, rsize, 30, local_reg);
			rrr_ins(i_subu, 29, 29, rsize);
			b.base = t30;
			b.offset = -4;
			ls_ins(i_sw, 30, b);
			rri_ins(i_subu, rsrce, 30, 16);
			rri_ins(i_subu, rdest, t30, 16);
			condrr_ins(i_beq, rdest, 29, le);
			set_label(lb);
			b.base = rsrce;
			b.offset = -4;
			ls_ins(i_lw, rsize, b);
			b.base = rdest;
			ls_ins(i_sw, rsize, b);
			rri_ins(i_subu, rsrce, rsrce, 4);
			rri_ins(i_subu, rdest, rdest, 4);
			condrr_ins(i_bne, rdest, 29, lb);
			set_label(le);
			if (vc) {
				mon_ins(i_move, 30, t30);
			}
		} else {
			int cs = (callee_size >> 3);
			int i;
			int tr = getreg(sp.fixed);
			rri_ins(i_subu, 29, 29, cs);
			b.base = 29;
			b.offset = cs - 4;
			ls_ins(i_sw, 30, b);

			for (i = cs - 16; i > 0; i -= 4) {
				b.base = 30;
				b.offset = i - cs - 4;
				ls_ins(i_lw, tr, b);
				b.base = 29;
				b.offset = i - 4;
				ls_ins(i_sw, tr, b);
			}

			if (vc) {
				rri_ins(i_addu, 30, 29, cs);
			}
		}

		return mka;
	}

	case make_dynamic_callee_tag: {
		bool vc = call_has_vcallees(e);
		int rptr;
		int rsize;
		int rdest;
		int tempreg;
		space nsp;
		baseoff b;
		int ls, le;

		rptr = getreg(sp.fixed);
		load_reg(child(e), rptr, sp);
		nsp = guardreg(rptr, sp);
		rsize = getreg(nsp.fixed);
		load_reg(next(child(e)), rsize, sp);
		nsp = guardreg(rsize, nsp);
		rdest = getreg(nsp.fixed);
		nsp = guardreg(rdest, nsp);
		tempreg = getreg(nsp.fixed);
		rri_ins(i_addu, rdest, rsize, 7 + 16);
		rri_ins(i_and, rdest, rdest, ~7);
		b.base = 29;
		b.offset = -4;
		ls_ins(i_sw, 30, b);

		if (vc) {
			mon_ins(i_move, 30, 29);
		}

		rrr_ins(i_subu, 29, 29, rdest);
		rri_ins(i_addu, rdest, 29, 0);
		ls = new_label();
		le = new_label();
		condrr_ins(i_ble, rsize, 0, le);
		b.offset = 0;
		set_label(ls);
		b.base = rptr;
		ls_ins(i_lw, tempreg, b);
		b.base = rdest;
		ls_ins(i_sw, tempreg, b);
		rri_ins(i_addu, rdest, rdest, 4);
		rri_ins(i_addu, rptr, rptr, 4);
		rri_ins(i_subu, rsize, rsize, 4);
		condrr_ins(i_bgt, rsize, 0, ls);
		set_label(le);

		return mka;
	}

	case tail_call_tag: {
		exp fn = child(e);
		exp cees = next(fn);
		bool glob = (fn->tag == name_tag && child(fn)->tag == ident_tag
		             && (child(child(fn)) == NULL ||
		                 child(child(fn))->tag == proc_tag ||
		                 child(child(fn))->tag ==
		                 general_proc_tag));
		exp bdy = child(crt_proc);
		int rptr;
		int rsz;
		space nsp;
		space xsp;
		int temp_fn_reg;

		nsp = sp;
		xsp = sp;
		if (cees->tag == make_callee_list_tag) {
			do_callee_list(cees, sp);
		}

		xsp.fixed = 0x800000f0;
		nsp.fixed |= 0x800000f0; /* don't use parregs or linkreg */
		if (cees->tag == make_dynamic_callee_tag) {
			rptr = getreg(nsp.fixed);
			load_reg(child(cees), rptr, nsp);
			nsp = guardreg(rptr, nsp);
			xsp = guardreg(rptr, xsp);
			rsz = getreg(nsp.fixed);
			load_reg(next(child(cees)), rsz, nsp);
			nsp = guardreg(rsz, nsp);
			xsp = guardreg(rsz, nsp);
		}

		if (PIC_code) {
			temp_fn_reg = 25;
			load_reg(fn, temp_fn_reg, nsp);
		} else if (!glob) {
			temp_fn_reg = getreg(nsp.fixed);
			load_reg(fn, temp_fn_reg, nsp);
			nsp = guardreg(temp_fn_reg, nsp);
			xsp = guardreg(temp_fn_reg, xsp);
		}

		while (bdy->tag == dump_tag || bdy->tag == diagnose_tag) {
			bdy = child(bdy);
		}

		while (bdy->tag == ident_tag && isparam(bdy)) {
			/* make sure that current callers and callees are in right place */
			exp sbdy = child(bdy);
			baseoff b;

			if (Has_fp) {
				b.base = 30;
				b.offset = no(sbdy) >> 3;
			} else {
				b.base = 29;
				b.offset = (no(sbdy) + frame_size + callee_size) >> 3;
			}

			if (sbdy->tag == formal_callee_tag
			    && cees->tag == same_callees_tag) {
				if ((bdy->props &inanyreg) != 0) {
					b.offset -= (callee_size >> 3);
					if (isvar(bdy)) {
						if (is_floating(sh(sbdy)->tag)) {
							lsfp_ins((sh(sbdy)->tag != shrealhd) ? i_s_d : i_s_s,
							         no(bdy) << 1, b);
						} else {
							ls_ins(i_sw, no(bdy), b);
						}
					}
				}
			} else if (sbdy->props == 0 && (bdy->props &inanyreg) != 0) {
				/* should be instore; is in reg */
				if (isvar(bdy)) {
					if (is_floating(sh(sbdy)->tag)) {
						lsfp_ins((sh(sbdy)->tag != shrealhd) ? i_s_d : i_s_s,
						         no(bdy) << 1, b);
					} else {
						ls_ins(i_sw, no(bdy), b);
					}
				}
			} else if (sbdy->props != 0 && (bdy->props & inanyreg) == 0) {
				/* should be in reg; is in store */
				if (!Has_no_vcallers && isvis(bdy) && last_param(bdy)) {
					int i = no(sbdy) >> 5;
					for (; i < 4; i++) {
						ls_ins(i_lw, i + 4, b);
						b.offset += 4;
					}
				} else if (is_floating(sh(sbdy)->tag)) {
					lsfp_ins((sh(sbdy)->tag != shrealhd) ? i_l_d : i_l_s,
					         sbdy->props << 1, b);
				} else {
					ls_ins(i_lw, sbdy->props, b);
				}

			} else if (sbdy->props != 0 && sbdy->props != no(bdy)) {
				/* in wrong register */
				if (is_floating(sh(sbdy)->tag)) {
					rrfp_ins((sh(sbdy)->tag != shrealhd) ? i_mov_d : i_mov_s,
					         sbdy->props << 1, no(bdy) << 1);
				} else {
					mon_ins(i_move, sbdy->props, no(bdy));
				}
			}

			bdy = next(sbdy);
		}

		restore_sregs(fixdone, fltdone);

		if (cees->tag == make_callee_list_tag) {
			/* copy from top of stack */
			int x = ((no(cees) >> 3) + 23) & ~7;
			int r = getreg(xsp.fixed);
			int r1 = getreg(guardreg(r, xsp).fixed);
			int i;
			baseoff b;
			int ncees = no(cees) >> 3;
			int rnc = (ncees + 7) & ~7;

			for (i = ncees; i > 0; i -= 4) {
				int x = r;
				b.base = 29;
				b.offset = i - 4;
				ls_ins(i_lw, r, b);
				b.base = 30;
				b.offset = i - rnc - 20;
				ls_ins(i_sw, r, b);
				r = r1;
				r1 = x;
			}

			/*
			 *  sp + 0: p1 ->   fp - 20: p1
			 *  sp + 4: p2      fp - 24: p2
			 *  sp + 8: p3      fp - 28: p3
			 *  	....
			 */

			rri_ins(i_subu, 29, 30, x);

		} else if (cees->tag == make_dynamic_callee_tag) {
			/*
			 *   rdest = fp-16;
			 *    rsize = (rsize+23)&~7
			 *    rsource = rptr + rsize
			 *    while rsize>0
			 * 	  [rdest-4] = [rsource-4];
			 * 	  rdest-=4; rsource-=4; rsize-=4;
			 *    sp = rdest;
			 */
			int rdest;
			int rsize = rsz;
			int rsource = rptr;
			int tempr;
			int le, ls;
			baseoff b;

			rdest = getreg(xsp.fixed);
			nsp = guardreg(rdest, nsp);
			tempr = getreg(xsp.fixed);
			rri_ins(i_subu, rdest, 30, 16);
			rri_ins(i_addu, rsize, rsize, 7);
			rri_ins(i_and, rsize, rsize, ~7);
			rrr_ins(i_addu, rsource, rsource, rsize);
			le = new_label();
			ls = new_label();
			condrr_ins(i_ble, rsize, 0, le);
			set_label(ls);
			b.base = rsource;
			b.offset = -4;
			ls_ins(i_lw, tempr, b);
			b.base = rdest;
			ls_ins(i_sw, tempr, b);
			rri_ins(i_subu, rdest, rdest, 4);
			rri_ins(i_subu, rsource, rsource, 4);
			rri_ins(i_subu, rsize, rsize, 4);
			condrr_ins(i_bgt, rsize, 0, ls);
			set_label(le);
			mon_ins(i_move, 29, rdest);
		} else {
			if (Has_vcallees) {
				mon_ins(i_move, 29, local_reg);
			} else if (Has_fp) {
				rri_ins(i_subu, 29, 30, callee_size >> 3);
			} else { /* this should only occur in initialisation procs */
				rri_ins(i_addu, 29, 29, (frame_size + callee_size) >> 3);
			}
		}

		if (Has_vcallees) {
			baseoff b;
			b.base = 30;
			b.offset = -16;
			ls_ins(i_lw, local_reg, b); /* old l-reg in -16(30) */
		}

		if (glob && !PIC_code) {
			extj_ins(i_j, boff(child(fn)));
		} else {
			br_ins(i_j, temp_fn_reg);

		}

		clear_all();
		return mka;
	}

	case apply_tag: {
		exp fn = child(e);
		exp par = next(fn);
		exp list = par;
		exp dad = father(e);
		bool tlrecurse = dad->tag == res_tag && dad->props;
		int hda = sh(e)->tag;
		int disp;

		if ((disp = specialfn (fn)) > 0) {
			mka.lab = specialmake(disp, list, sp, dest, exitlab);
			return mka;
		}

		if (!fn->last) {
			sp = do_callers(list, sp);
		}

		if (fn->tag == name_tag && child(fn)->tag == ident_tag
		    && (child(child(fn)) == NULL || child(child(fn))->tag == proc_tag)) {
			/* the procedure can be entered directly */
			if (! tlrecurse) {
				extj_ins(i_jal, boff(child(fn)));
				if (PIC_code) {
					reset_gp();
				}
			} else {
				if (Has_fp) {
					baseoff b;
					b.base = 30;
					b.offset = -4;
					restore_sregs(fixdone, fltdone);
					mon_ins(i_move, 29, 30);
					ls_ins(i_lw, 30, b);
				} else {
					restore_sregs(fixdone, fltdone);
					rri_ins(i_addu, 29, 29, (frame_size + callee_size) >> 3);
				}

				extj_ins(i_j, boff(child(fn)));
				if (as_file) {
					asm_printf(" # Tail recursion\n");
				}
			}
		} else if (PIC_code) {
			/* have to get address of proc into r25 */
			where w;
			setregalt(w.answhere, 25);
			w.ashwhere = ashof(sh(fn));
			code_here(fn, sp, w);
			br_ins(i_jal, 25);
			reset_gp();
		} else {
			/* the address of the proc is evaluated
						   and entered indirectly */
			clear_reg(31); /* can't use 31 as temp reg for jal */
			br_ins(i_jal, reg_operand(fn, guardreg(31, sp)));
		}

		clear_all(); /* forget all register memories */

		{
			ans aa;

			if (is_floating(hda)) {
				freg frg;
				frg.fr = 0;
				frg.dble = (hda != shrealhd);
				setfregalt(aa, frg);
				move(aa, dest, sp, 1);
				/* move floating point result of application to destination */
			} else {
				setregalt(aa, 2);
				mka.regmove = 2;
				move(aa, dest, guardreg(2, sp), 1);
				/* move fixed point result of application to destination */
			}

			/* else struct results are moved by body of proc */
		}

		return mka;
	}

	case return_to_label_tag: {
		int r = getreg(sp.fixed);
		where w;

		setregalt(w.answhere, r);
		w.ashwhere = ashof(sh(child(e)));
		code_here(child(e), sp, w);
		clear_all();

		if (Has_fp) {
			baseoff b;
			b.base = 30;
			restore_sregs(fixdone, fltdone);

			if (Has_vcallees) {
				b.offset = -16;
				ls_ins(i_lw, local_reg, b);
			}

			b.offset = -4;
			mon_ins(i_move, 29, 30);
			ls_ins(i_lw, 30, b);
		} else if (frame_size != 0) {
			restore_sregs(fixdone, fltdone);
			/* restore dumped value of s-regs on entry */
			rri_ins(i_addu, 29, 29, frame_size >> 3);
			/* reset stack ptr */
		}

		uncond_ins(i_j, r);

		return mka;
	}

	case res_tag:
	case untidy_return_tag: {
		where w;
		w.answhere = procans;
		w.ashwhere = ashof(sh(child(e)));
		code_here(child(e), sp, w);
		/* evaluate result value */

		clear_all(); /* clear all register memories */

		if (child(e)->tag == apply_tag && e->props) {
			return mka;
		}

		/* was a tail recursion */
		if (frame_size == 0
		    && !Has_fp) {
			uncond_ins(i_j, 31);
		} else if (result_label != 0 && e->tag == res_tag) {
			uncond_ins(i_b, result_label);
			if (as_file) {
				asm_printf( " # Return\n");
			}
		} else {
			if ((fixdone | fltdone) == 0) {
				result_label = new_label();
				set_label(result_label);
			}

			if (Has_fp) {
				baseoff b;
				b.base = 30;
				restore_sregs(fixdone, fltdone);
				if (Has_vcallees) {
					b.offset = -16;
					ls_ins(i_lw, local_reg, b);
				}

				b.offset = -4;
				if (e->tag == res_tag) {
					mon_ins(i_move, 29, 30);
				}

				ls_ins(i_lw, 30, b);
			} else {
				restore_sregs(fixdone, fltdone);
				/* restore dumped value of s-regs on entry */
				if (frame_size != 0 && e->tag == res_tag) {
					rri_ins(i_addu, 29, 29, frame_size >> 3);
				}

				/* reset stack ptr */
			}

			if (diagPIClab != 0) {
				uncond_ins(i_b, diagPIClab);
			} else {
				uncond_ins(i_j, 31);
			}
		}

		return mka;
	}

	case diagnose_tag:
		diag3_driver->output_diag(dno(e), 0, e);
		mka = make_code(child(e), sp, dest, exitlab);
		diag3_driver->output_end_scope(dno(e), e);

		return mka;

	case solve_tag: {
		exp m = next(child(e));
		int l = exitlab;

		if (dest.answhere.discrim == insomereg) {
			/* choose register for result */
			int  *sr = someregalt(dest.answhere);
			if (*sr != -1) {
				error(ERR_INTERNAL, "Somereg *2");
			}
			*sr = getreg(sp.fixed);
			setregalt(dest.answhere, *sr);
		} else if (dest.answhere.discrim == insomefreg) {
			somefreg sfr;
			freg fr;
			sfr = somefregalt(dest.answhere);
			if (*sfr.fr != -1) {
				error(ERR_INTERNAL, "Somefreg *2");
			}
			*sfr.fr = getfreg(sp.flt);
			fr.fr = *sfr.fr;
			fr.dble = sfr.dble;
			setfregalt(dest.answhere, fr);
		}

		for (;;) {
			/* set up all the labels in the component
						   labst_tags */
			no(child(m)) = new_label();
			if (m->last) {
				break;
			}
			m = next(m);
		}

		for (m = child(e); !m->last; m = next(m)) {
			/* evaluate all the component statements */
			int fl = make_code(m, sp, dest, l).lab;
			clear_all();

			if (fl != 0) {
				l = fl;
			}

			if (!m->last) {
				/* jump to end of solve */
				if (l == 0) {
					l = new_label();
				}

				if (sh(m)->tag != bothd) {
					uncond_ins(i_b, l);
				}
			}
		}

		mka.lab = l;
		return mka;
	}

	case case_tag: {
		int r = reg_operand(child(e), sp);
		/* evaluate controlling integer into reg r */
		mm lims;
		exp z = next(child(e));
		exp zt = z;
		long  n;
		long  l;
		long  u = 0x80000000;

		sp = guardreg(r, sp);

		l = no(zt);
		for (n = 1;; n++) {
			/* calculate crude criterion for using
						   jump vector or branches */
			if (u + 1 != no(zt) && child(zt) != NULL) {
				n++;
			}

			if (zt->last) {
				u = (child(zt) != NULL) ? no(child(zt)) : no(zt);
				break;
			}

			if (child(zt) != NULL) {
				u = no(child(zt));
			} else {
				if (u + 1 == no(zt)) {
					u += 1;
				}
			}

			zt = next(zt);
		}

		/* now l is lowest controlling value and u is highest */

		if (is_signed(sh(child(e)))) {
			u = u / 2 - l / 2;
		} else {
			u = ((unsigned)u) / 2 - ((unsigned)l) / 2;
		}

		if ( u <= n * n / 4 - 3 /* ware overflow! */) {
			/* space-time product criterion for jump vector instead of tests
			   and branches *//* use jump vector */
			int   endlab = new_label();
			int   veclab = next_dlab_sym();
			baseoff zeroveclab;
			baseoff zero3;
			int   mr = getreg(sp.fixed);
			int r3 = getreg(guardreg(mr, sp).fixed);
			zero3.base = r3;
			zero3.offset = 0;
			zeroveclab.offset = 0;
			zeroveclab.base = veclab;
			n = l;

			if (as_file) {
				asm_printop(".rdata\n$$%d:", veclab);
			}

			out_common(0, irdata);
			out_common(tempsnos[veclab - 32], ilabel);

			for (;;) {
				for (; no (z) > n; n++) {/* o/p jump vector */
					if (as_file)
						asm_printf(PIC_code ? "\t.gpword\t$%d\n" : "\t.word\t$%d\n", endlab);
					out_value(-endlab, PIC_code ? igpword : iword, 0, 1);
				}

				u = (child(z) == NULL) ? n : no(child(z));
				for (; n <= u; n++) {
					child(pt(z))->props = 1; /* as bug - see labst_tag */
					if (as_file)
						asm_printf(
						    PIC_code ? "\t.gpword\t$%ld\n" : "\t.word\t$%ld\n", no(child(pt(z))));
					out_value(-no(child(pt(z))), PIC_code ? igpword : iword, 0, 1);
				}

				if (z->last) {
					break;
				}

				z = next(z);
			}

			if (as_file) {
				asm_printop(".text");
			}

			out_common(0, itext);

			ls_ins(i_la, r3, zeroveclab);
			if (l != 0) {
				rri_ins(i_addu, mr, r, -l);
				condri_ins(i_bgeu, mr, u - l + 1, endlab);
				rri_ins(i_mul, mr, mr, 4);
			} else {
				condri_ins(i_bgeu, r, u + 1, endlab);
				rri_ins(i_mul, mr, r, 4);
			}

			rrr_ins(i_addu, r3, r3, mr);
			ls_ins(i_lw, r3, zero3);
			if (PIC_code) {
				rrr_ins(i_addu, r3, r3, 28);
			}

			uncond_ins(i_j, r3);
			set_label(endlab);
			return mka;
		} else if (is_signed(sh(child(e)))) {
			int over = 0;	/* use branches - tests are already ordered */

			lims = maxmin(sh(child(e)));
			for (;;) {
				int   lab = no(child(pt(z)));
				long l = no(z);

				if (child (z) == NULL) {/* only single test required */
					condri_ins(i_beq, r, l, lab);
					if (l == lims.maxi) {
						lims.maxi -= 1;
					} else if (l == lims.mini) {
						lims.mini += 1;
					}
				} else if (u = no(child(z)), l > lims.mini) {
					if (u >= lims.maxi) {
						/* have already tested lower */
						condri_ins(i_bge, r, l, lab);
						lims.maxi = l - 1;
					} else {
						if (over == 0) {
							over = new_label();
						}
						condri_ins(i_blt, r, l, over);
						condri_ins(i_ble, r, u, lab);
						lims.mini = u + 1;
					}
				} else {
					/* lower is <= lower limit of shape */
					if (u < lims.maxi) {
						condri_ins(i_ble, r, u, lab);
						lims.mini = u + 1;
					} else {		/* upper is >= upper limit of shape */
						uncond_ins(i_b, lab);
					}
				}

				if (z->last) {
					if (over != 0) {
						set_label(over);
					}
					return mka;
				}

				z = next(z);
			}
		} else {
			int over = 0;	/* use branches - tests are already ordered */
			unsigned long maxi;
			unsigned long mini;

			lims = maxmin(sh(child(e)));
			maxi = (unsigned)lims.maxi;
			mini = (unsigned)lims.mini;

			for (;;) {
				int lab = no(child(pt(z)));
				unsigned long l = no(z);

				if (child (z) == NULL) {/* only single test required */
					condri_ins(i_beq, r, l, lab);
					if (l == maxi) {
						maxi -= 1;
					} else if (l == mini) {
						mini += 1;
					}
				} else if (u = no(child(z)), l > mini) {
					if (u >= maxi) {
						/* have already tested lower */
						condri_ins(i_bgeu, r, l, lab);
						maxi = l - 1;
					} else {
						if (over == 0) {
							over = new_label();
						}
						condri_ins(i_bltu, r, l, over);
						condri_ins(i_bleu, r, u, lab);
						mini = u + 1;
					}
				} else {
					/* lower is <= lower limit of shape */
					if (u < maxi) {
						condri_ins(i_bleu, r, u, lab);
						mini = u + 1;
					} else {		/* upper is >= upper limit of shape */
						uncond_ins(i_b, lab);
					}
				}

				if (z->last) {
					if (over != 0) {
						set_label(over);
					}
					return mka;
				}

				z = next(z);
			}
		}
	}

	/* byte offset + bit offset - see needs scan */
	case offset_add_tag: {
		exp l = child(e);
		exp r = next(l);
		int r1 = reg_operand(l, sp);
		int tmp, d, r2;
		space nsp;
		ans aa;
		tmp = getreg(sp.fixed);
		rri_ins(i_sll, tmp, r1, 3);
		d = regfrmdest(&dest, sp);

		if (r->tag == val_tag) {
			rri_ins(i_addu, d, tmp, no(r));
		} else {
			nsp = guardreg(tmp, sp);
			r2 = reg_operand(r, nsp);
			rrr_ins(i_addu, d, tmp, r2);
		}

		setregalt(aa, d);
		mka.regmove = move(aa, dest, guardreg(d, sp), 0);

		return mka;
	}

	/* bit offset - byte offset - see needs scan */
	case offset_subtract_tag: {
		exp l = child(e);
		exp r = next(l);
		int r2 = reg_operand(r, sp);
		int tmp, d, r1;
		space nsp;
		ans aa;
		tmp = getreg(sp.fixed);

		rri_ins(i_sll, tmp, r2, 3);
		d = regfrmdest(&dest, sp);
		nsp = guardreg(tmp, sp);
		r1 = reg_operand(l, nsp);
		rrr_ins(i_subu, d, r1, tmp);

		setregalt(aa, d);
		mka.regmove = move(aa, dest, guardreg(d, sp), 0);

		return mka;
	}

	case plus_tag:
		if (optop(e)) {
			mka.regmove = comm_op(e, sp, dest, i_addu);
			return mka;
		}

		if ((errhandle(e) & 3) == 3 && is_signed(sh(e)) && shape_size(sh(e)) == 32) {
			mka.regmove = comm_op(e, sp, dest, i_add);
			return mka;
		} else {
			/* possible overflow - can optimised a bit fot lit. operand*/
			int r1 = reg_operand(child(e), sp);
			int r2, r3, r0;
			long over = new_label();
			long trap = trap_label(e);
			space nsp;
			ans aa;

			nsp = guardreg(r1, sp);
			r2 = reg_operand(next(child(e)), nsp);
			nsp = guardreg(r2, nsp);
			r0 = getreg(nsp.fixed);
			nsp = guardreg(r0, nsp);
			rrr_ins(i_addu, r0, r1, r2);

			switch (sh(e)->tag) {
			case slonghd:
				r3 = getreg(nsp.fixed);
				rrr_ins(i_xor, r3, r1, r2);

				condr_ins(i_bltz, r3, over);
				rrr_ins(i_xor, r3, r0, r1);
				condr_ins(i_bltz, r3, trap);

				set_label(over);
				break;

			case ulonghd:
				r3 = getreg(nsp.fixed);
				mon_ins(i_not, r3, r1);
				rrr_ins(i_sltu, r3, r3, r2);
				condrr_ins(i_bne, r3, 0, trap);
				break;

			case scharhd:
				testsigned(r0, -128, 127, trap);
				break;

			case ucharhd:
				testusigned(r0, 255, trap);
				break;

			case swordhd:
				testsigned(r0, -0x8000, 0x7fff, trap);
				break;

			case uwordhd:
				testusigned(r0, 0xffff, trap);
				break;

			default:
				error(ERR_INTERNAL, "NOT integer in plus with o/f");
			}

			setregalt(aa, r0);
			mka.regmove = move(aa, dest, nsp, 0);
			return mka;
		}

	case chvar_tag: {
		int a;
		int d;
		ans aa;
		int nsh = sh(e)->tag;

		if (!endian == ENDIAN_BIG && dest.answhere.discrim == inreg
		    && regalt(dest.answhere) != 0) {
			ash arga;
			arga = ashof(sh(child(e)));

			if (arga.ashsize <= dest.ashwhere.ashsize) {
				dest.ashwhere = arga;
			}

			a = regalt(dest.answhere);
			code_here(child(e), sp, dest);
			/* evaluate arguement into reg */
		} else {
			a = reg_operand(child(e), sp);
			/* evaluate arguement into a */
		}

		setregalt(aa, a);

		if (!optop(e)) {
			long trap = trap_label(e);
			bool sg = is_signed(sh(child(e)));

			switch (nsh) {
			case scharhd:
				if (sg) {
					testsigned(a, -128, 127, trap);
				} else {
					testusigned(a, 127, trap);
				}
				break;

			case ucharhd:
				testusigned(a, 255, trap);
				break;

			case swordhd:
				if (sg) {
					testsigned(a, -0x8000, 0x7fff, trap);
				} else {
					testusigned(a, 0x7fff, trap);
				}
				break;

			case uwordhd:
				testusigned(a, 0xffff, trap);
				break;

			case slonghd:
				if (!sg) {
					testusigned(a, 0x7fffffff, trap);
				}
				break;

			case ulonghd:
				if (sg) {
					testusigned(a, 0x7fffffff, trap);
				}
				break;
			}

			mka.regmove = move(aa, dest, sp, 1);
			return mka;
		}

		if (sh(child(e)) == sh(e) || nsh  >= slonghd) {
			/* no changes required, so just move */
			mka.regmove = move(aa, dest, sp, 1);
			return mka;
		}

		switch (dest.answhere.discrim) {
		case insomereg: {
			int  *dr = someregalt(dest.answhere);
			d = getreg(sp.fixed);
			*dr = d;
			goto out;
		}

		case inreg:
			d = regalt(dest.answhere);
			goto out;

		default:
			/* representation in store will be same so just move */
			move(aa, dest, sp, 1);
			return mka;
		}

out:

		/* d is destination register - do appropriate ands etc */
		if (d == 0) {
			return mka;
		}

		if (nsh == ucharhd) {
			rri_ins(i_and, d, a, 255);
		} else if (nsh == uwordhd) {
			rri_ins(i_and, d, a, (1 << 16) - 1);
		} else if (nsh == scharhd) {
			rri_ins(i_sll, d, a, 24);
			rri_ins(i_sra, d, d, 24);
		} else if (nsh == swordhd) {
			rri_ins(i_sll, d, a, 16);
			rri_ins(i_sra, d, d, 16);
		}

		mka.regmove = d;
		return mka;
	}

	case minus_tag: {
		if (optop(e)) {
			mka.regmove =
			    non_comm_op(e, sp, dest, i_subu);
			return mka;
		} else if ((errhandle(e) & 3) == 3 && is_signed(sh(e)) && shape_size(sh(e)) == 32) {
			mka.regmove =
			    non_comm_op(e, sp, dest, i_sub);
			return mka;
		} else {
			/* possible overflow - can optimised a bit for lit. operand*/
			int r1 = reg_operand(child(e), sp);
			int r2, r3, r0;
			long over = new_label();
			long trap = trap_label(e);
			space nsp;
			ans aa;

			nsp = guardreg(r1, sp);
			r2 = reg_operand(next(child(e)), nsp);
			nsp = guardreg(r2, nsp);
			r0 = getreg(nsp.fixed);
			nsp = guardreg(r0, nsp);
			rrr_ins(i_subu, r0, r1, r2);

			switch (sh(e)->tag) {
			case slonghd:
				r3 = getreg(nsp.fixed);
				rrr_ins(i_xor, r3, r1, r2);

				condr_ins(i_bgez, r3, over);
				rrr_ins(i_xor, r3, r0, r1);
				condr_ins(i_bltz, r3, trap);

				set_label(over);
				/* Alteration 2 also in plus_tag */
				break;

			case ulonghd:
				r3 = getreg(nsp.fixed);
				/* Alteration 3 */
				rrr_ins(i_sltu, r3, r1, r2);
				condrr_ins(i_bne, r3, 0, trap);
				break;

			case scharhd:
				testsigned(r0, -128, 127, trap);
				break;

			case ucharhd:
				testusigned(r0, 255, trap);
				break;

			case swordhd:
				testsigned(r0, -0x8000, 0x7fff, trap);
				break;

			case uwordhd:
				testusigned(r0, 0xffff, trap);
				break;

			default:
				error(ERR_INTERNAL, "NOT integer in minus with o/f");
			}

			setregalt(aa, r0);
			mka.regmove = move(aa, dest, nsp, 0);
			return mka;
		}
	}

	case mult_tag:
	case offset_mult_tag: {
		exp rop = next(child(e));

		if (!optop(e)) {	/* test for overflo */
			int r1 = reg_operand(child(e), sp);
			int r2, r3, r0;
			long trap = trap_label(e);
			space nsp;
			ans aa;

			nsp = guardreg(r1, sp);
			r2 = reg_operand(next(child(e)), nsp);
			nsp = guardreg(r2, nsp);
			r0 = getreg(nsp.fixed);
			nsp = guardreg(r0, nsp);
			r3 = getreg(nsp.fixed);

			switch (sh(e)->tag) {
			case slonghd: {
				int r4;
				mon_ins(i_mult, r1, r2);
				hilo_ins(i_mflo, r0);
				hilo_ins(i_mfhi, r3);
				r4 = getreg(guardreg(r3, nsp).fixed);
				rri_ins(i_sra, r4, r0, 31);
				condrr_ins(i_bne, r4, r3, trap);
				break;
			}

			case ulonghd:
				mon_ins(i_multu, r1, r2);
				hilo_ins(i_mflo, r0);
				hilo_ins(i_mfhi, r3);
				condrr_ins(i_bne, r3, 0, trap);
				break;

			case scharhd:
				rrr_ins(i_mul, r0, r1, r2);
				testsigned(r0, -128, 127, trap);
				break;

			case ucharhd:
				rrr_ins(i_mul, r0, r1, r2);
				testusigned(r0, 255, trap);
				break;

			case swordhd:
				rrr_ins(i_mul, r0, r1, r2);
				testsigned(r0, -0x8000, 0x7fff, trap);
				break;

			case uwordhd:
				rrr_ins(i_mul, r0, r1, r2);
				testusigned(r0, 0xffff, trap);
				break;

			default:
				error(ERR_INTERNAL, "NOT integer in mult with o/f");
			}

			setregalt(aa, r0);
			mka.regmove = move(aa, dest, nsp, 0);

			return mka;
		}

		if (rop->last && rop->tag == val_tag) {
			/* multiplication by constant m */
			int m = no(rop);
			int p2;

			if (m > 1 && (
			        ((p2 = m) & (m - 1)) == 0 ||
			        (m & (p2 = m + 1)) == 0 ||
			        ((p2 = m - 1) & (m - 2)) == 0))
			{
				/* m = 2^shleng   or  m = 2^(shleng +/- 1) */
				int r = reg_operand(child(e), sp);
				/* evaluate first arguement */
				int rr;
				space nsp;
				int shleng;
				ans aa;

				for (shleng = 0; p2 != 1; shleng++) {
					p2 >>= 1;
				}

				switch (dest.answhere.discrim) {
				case inreg:
					rr = regalt(dest.answhere);
					if (rr != r || (m & (m - 1)) == 0) {
						nsp = sp;
						break;
					}
					/* else continue to next case */
				default:
					if ((m & (m - 1)) == 0) {
						rr = getreg(sp.fixed);
						nsp = sp;
					} else {
						nsp = guardreg(r, sp);
						rr = getreg(nsp.fixed);
					}
				}

				rri_ins(i_sll, rr, r, shleng);

				if ((m & (m - 1)) != 0) {
					rrr_ins(((m & (m + 1)) == 0) ? i_subu : i_addu, rr, rr, r);
				}

				setregalt(aa, rr);
				mka.regmove = move(aa, dest, guardreg(rr, sp), 1);

				return mka;
			}
		} /* else do straightforward mult */

		mka.regmove = comm_op(e, sp, dest, i_mul);
		return mka;
	}

	case div0_tag:
	case div2_tag:
	case offset_div_by_int_tag:
	case offset_div_tag: {
		exp rop = next(child(e));
		exp lop = child(e);
		bool uns = !(is_signed(sh(e)));
		int trap;
		space nsp;
		int r0, r1, r2;
		ans aa;

		/* division by constant */
		if (rop->tag == val_tag) {
			int m;

			m = no(rop);
			if (m == 1) {
				e = lop;
				goto tailrecurse;
			}

			/* replace div by 2^shleng by sh right shleng */
			if ((e->tag == div0_tag || uns) && m > 1 && (m & (m - 1)) == 0) {
				int r = reg_operand(child(e), sp);
				int shleng;
				int rr;

				for (shleng = 0; m != 1; shleng++) {
					m >>= 1;
				}

				rr = regfrmdest(&dest, sp);
				rri_ins((uns) ? i_srl : i_sra, rr, r, shleng);
				setregalt(aa, rr);
				mka.regmove = move(aa, dest, guardreg(rr, sp), 1);
				return mka;
			}
		}

		r1 = reg_operand(lop, sp);
		nsp = guardreg(r1, sp);
		r2 = reg_operand(rop, nsp);

		/* test for (-inf)/-1 and /0 */
		if (!optop(e) || (errhandle(e) & 3) == 2) {
			long over = new_label();
			trap = ((errhandle(e) & 3) == 2) ? new_label() : trap_label(e);
			condri_ins(i_beq, r2, 0, trap);

			if (!uns) {
				condri_ins(i_bne, r2, -1, over);
				condri_ins(i_beq, r1, maxmin(sh(e)).mini, trap);
				set_label(over);
			}
		}

		r0 = regfrmdest(&dest, sp);
		rrr_ins((uns) ? i_divu : i_div, r0, r1, r2);
		if ((errhandle(e) & 3) == 2) {
			set_label(trap);
		}

		setregalt(aa, r0);
		mka.regmove = move(aa, dest, guardreg(r0, sp), 0);

		return mka;
	}

	/* only applies to signed operands */
	case div1_tag: {
		exp rop = next(child(e));
		exp lop = child(e);
		space nsp;
		int r0, r1, r2;
		int lab, treg, trap;
		ans aa;

		/* division by constant */
		if (rop->tag == val_tag ) {
			int m;

			m = no(rop);
			if (m > 1 && (m & (m - 1)) == 0) {
				int   r = reg_operand(child(e), sp);
				/* replace div by 2^shleng by arith sh right shleng */
				int   shleng;
				int   rr;
				for (shleng = 0; m != 1; shleng++) {
					m >>= 1;
				}

				rr = regfrmdest(&dest, sp);
				rri_ins(i_sra, rr, r, shleng);
				setregalt(aa, rr);
				mka.regmove = move(aa, dest, guardreg(rr, sp), 1);
				return mka;
			}
		}

		r1 = reg_operand(lop, sp);
		nsp = guardreg(r1, sp);
		r2 = reg_operand(rop, nsp);
		nsp = guardreg(r2, sp);

		if (!optop(e) || (errhandle(e) & 3) == 2) { /* test for (-inf)/-1 and /0 */
			long over = new_label();
			trap = ((errhandle(e) & 3) == 2) ? new_label() : trap_label(e);
			condri_ins(i_beq, r2, 0, trap);
			condri_ins(i_bne, r2, -1, over);
			condri_ins(i_beq, r1, maxmin(sh(e)).mini, trap);
			set_label(over);
		}

		r0 = regfrmdest(&dest, nsp);
		rrr_ins((is_signed(sh(e))) ? i_div : i_divu, r0, r1, r2);
		treg = getreg(guardreg(r0, nsp).fixed);
		lab = new_label();
		hilo_ins(i_mfhi, treg);
		condri_ins(i_beq, treg, 0, lab);
		rrr_ins(i_xor, treg, treg, r2);
		rri_ins(i_sra, treg, treg, 31);
		rrr_ins(i_addu, r0, r0, treg);
		set_label(lab);

		if ((errhandle(e) & 3) == 2) {
			set_label(trap);
		}

		setregalt(aa, r0);
		mka.regmove = move(aa, dest, guardreg(r0, sp), 0);

		return mka;
	}

	case abs_tag: {
		int r = reg_operand(child(e), sp);
		int d = regfrmdest(&dest, guardreg(r, sp));
		int l = new_label();
		ans aa;

		setnoreorder();
		condri_ins(i_bge, r, 0, l);
		rri_ins(i_addu, d, r, 0);
		rrr_ins(i_subu, d, 0, r);
		setreorder();

		if (!optop(e)) {
			condri_ins(i_ble, r, maxmin(sh(e)).mini, trap_label(e));
		}

		set_label_no_clear(l);
		setregalt(aa, d);
		mka.regmove = move(aa, dest, guardreg(d, sp), 0);

		return mka;
	}

	case neg_tag:
	case offset_negate_tag:
		if (optop(e)) {
			mka.regmove = monop(e, sp, dest, i_negu);
			return mka;
		}

		if ((errhandle(e) & 3) == 3 && shape_size(sh(e)) == 32) {
			mka.regmove = monop(e, sp, dest, i_neg);
			return mka;
		} else {
			int r1 = reg_operand(child(e), sp);
			long trap = trap_label(e);
			int r2;
			ans aa;

			condri_ins((is_signed(sh(e))) ? i_ble : i_bne, r1, maxmin(sh(e)).mini, trap);
			r2 = getreg(sp.fixed);
			mon_ins(i_neg, r2, r1);

			if (is_signed(sh(e))) {
				condri_ins(i_ble, r2, maxmin(sh(e)).mini, trap);
			}

			setregalt(aa, r2);
			mka.regmove = move(aa, dest, guardreg(r2, sp), 0);

			return mka;
		}

	case goto_lv_tag: {
		int r = reg_operand(child(e), sp);
		uncond_ins(i_j, r);
		clear_all();

		return mka;
	}

	case shl_tag:
	case shr_tag: {
		exp s = child(e);
		exp b = next(s);
		int a;
		int d;
		int sz = shape_size(sh(s));
		bool lded = ((s->tag == name_tag && regofval(s) >= 100)
		             || (s->tag == cont_tag &&
		                 (child(s)->tag != name_tag || regofval(child(s)) > 0)));
		bool signok = (sz == 32 || s->tag == chvar_tag || lded);
		ans aa;
		space nsp;
		bool sgned = is_signed(sh(e));
		char *shnat;
		char *shun;
		int norms = 0;

		/* can use short loads instead of shifts */
		if (lded && b->tag == val_tag && (no(b) == 16 || no(b) == 24)
		    && e->tag == shr_tag ) {
			where w;
			instore is;
			w = locate(s, sp, sh(s), 0);
			/* 'address' of  first operand with shape of result */

			switch (w.answhere.discrim) {
			/* if w is a register we still have to do shifts */
			case inreg:
				a = regalt(w.answhere);
				goto alreadythere;

			case notinreg:
				is = insalt(w.answhere);
				if (!is.adval) {
					break;
				}

				if (is.b.offset == 0 && (a = is.b.base) >= 0 && a <= 31) {
					goto alreadythere;
				}

			default: {
				/* this shoudn't happen - shift of address or perhaps float in reg */
				where temp;

				a = -1;
				setsomeregalt(temp.answhere, &a);
				temp.ashwhere = dest.ashwhere;
				move(w.answhere, temp, sp, 1);

				goto alreadythere;
			}
			}

			d = regfrmdest(&dest, sp); /* d is destination register */

			if (endian == ENDIAN_LITTLE) {
				if (no(b) == 16) {
					is.b.offset += 2;
				} else {
					is.b.offset += 3;
				}
			}

			ls_ins((no(b) == 16) ? ((sgned) ? i_lh : i_lhu) : ((sgned) ? i_lb : i_lbu), d, is.b);
			setregalt(aa, d);
			move(aa, dest, guardreg(d, sp), 0);
			mka.regmove = d;

			return mka;
		}

		a = reg_operand(s, sp);

alreadythere:

		/* choose which shift instruction */
		if (e->tag == shr_tag) {
			shnat = (sgned) ? i_sra : i_srl;
			shun = i_sll;

			if (!signok) {
				rri_ins(i_sll, a, a, norms = 32 - sz);
			}
		} else {
			shnat = i_sll;
			shun = (sgned) ? i_sra : i_srl;
		}

		nsp = guardreg(a, sp);
		d = regfrmdest(&dest, nsp);

		if (b->tag == val_tag) {
			/* if its a constant shift we dont have to choose shift dynamically ... */
			if (no(b) >= 0) {
				rri_ins(shnat, d, a, no(b) + norms);
			} else {
				rri_ins(shun, d, a, -no(b));
			}
		} else {
			int sr = getreg(nsp.fixed);
			int ar = reg_operand(b, nsp);

			if (norms != 0) {
				rri_ins(shnat, a, a, norms);
			}

			if (NONEGSHIFTS || !is_signed(sh(e))
			    || (b->tag == and_tag && next(child(b))->tag == val_tag
			        && no(next(child(b))) > 0 && no(next(child(b))) <= 31)
			   ) {			/* ... similarly in these cases */
				rrr_ins(shnat, d, a, ar);
			} else {
				/* choose shift dynamically - is this
						   necessary for C? */
				int   l = new_label();
				int   endl = new_label();
				condri_ins(i_bge, ar, 0, l);
				mon_ins(i_neg, sr, ar);
				rrr_ins(shun, d, a, sr);
				uncond_ins(i_b, endl);
				set_label(l);
				rrr_ins(shnat, d, a, ar);
				set_label(endl);
			}
		}

		setregalt(aa, d);
		move(aa, dest, nsp, 1);
		mka.regmove = d;

		return mka;
	}

	/* only applies to signed operands */
	case mod_tag: {
		exp rop = next(child(e));
		exp lop = child(e);
		space nsp;
		int r0, r1, r2;
		int lab, treg;
		ans aa;

		/* mod by constant */
		if (rop->tag == val_tag) {
			int m;

			m = no(rop);
			if (m > 1 && (m & (m - 1)) == 0) {
				/* mod by power of 2 replaced by and */
				int   r = reg_operand(child(e), sp);
				ans aa;
				int   rr = regfrmdest(&dest, sp);
				rri_ins(i_and, rr, r, no(rop) - 1);
				setregalt(aa, rr);
				mka.regmove = move(aa, dest, guardreg(rr, sp), 1);
				return mka;
			}
		}

		r1 = reg_operand(lop, sp);
		nsp = guardreg(r1, sp);
		r2 = reg_operand(rop, nsp);

		if (!optop(e)) { /* test for (-inf)/-1 and /0 */
			long over = new_label();
			long trap = trap_label(e);
			condri_ins(i_beq, r2, 0, trap);
			condri_ins(i_bne, r2, -1, over);
			condri_ins(i_beq, r1, maxmin(sh(e)).mini, trap);
			set_label(over);
		}

		r0 = regfrmdest(&dest, nsp);
		rrr_ins(i_rem, r0, r1, r2);
		treg = getreg(guardreg(r0, nsp).fixed);
		lab = new_label();
		condri_ins(i_beq, r0, 0, lab);
		rrr_ins(i_xor, treg, r0, r2);
		condri_ins(i_bge, treg, 0, lab);
		rrr_ins(i_addu, r0, r0, r2);
		set_label(lab);
		setregalt(aa, r0);
		mka.regmove = move(aa, dest, guardreg(r0, sp), 0);

		return mka;
	}

	case rem2_tag:
	case rem0_tag: {
		exp rop = next(child(e));
		exp lop = child(e);
		bool uns = !is_signed(sh(e));
		space nsp;
		int r0, r1, r2;
		ans aa;

		nsp = sp;

		if ((uns || e->tag == rem0_tag) && rop->tag == val_tag) {
			/* mod by constant */
			int m;

			m = no(rop);
			if (m > 1 && (m & (m - 1)) == 0) {
				/* mod by power of 2 replaced by and */
				int   r = reg_operand(child(e), sp);
				ans aa;
				int   rr = regfrmdest(&dest, sp);
				rri_ins(i_and, rr, r, no(rop) - 1);
				setregalt(aa, rr);
				mka.regmove = move(aa, dest, guardreg(rr, sp), 1);

				return mka;
			}

			if (m != 0 && (m != -1 || uns)) {
				r1 = reg_operand(lop, sp);
				r0 = regfrmdest(&dest, sp);
				rri_ins((uns) ? i_remu : i_rem, r0, r1, m);
				setregalt(aa, r0);
				mka.regmove = move(aa, dest, guardreg(r0, nsp), 0);

				return mka;
			}
		}

		r1 = reg_operand(lop, sp);
		nsp = guardreg(r1, sp);
		r2 = reg_operand(rop, nsp);

		/* test for (-inf)/-1 and /0 */
		if (!optop(e)) {
			long over = new_label();
			long trap = trap_label(e);
			condri_ins(i_beq, r2, 0, trap);

			if (!uns) {
				condri_ins(i_bne, r2, -1, over);
				condri_ins(i_beq, r1, maxmin(sh(e)).mini, trap);
				set_label(over);
			}
		}

		r0 = regfrmdest(&dest, sp);
		rrr_ins((uns) ? i_remu : i_rem, r0, r1, r2);
		setregalt(aa, r0);
		mka.regmove = move(aa, dest, guardreg(r0, nsp), 0);
		return mka;
	}

	case minptr_tag:
	case make_stack_limit_tag:
		mka.regmove = non_comm_op(e, sp, dest, i_subu);

		return mka;

	case fplus_tag:
		mka.regmove = fop(e, sp, dest, (sh(e)->tag != shrealhd) ? i_add_d : i_add_s);
		if (!optop(e)) {
			checknan(e, sp);
		}

		return mka;

	case fminus_tag:
		mka.regmove = fop(e, sp, dest, (sh(e)->tag != shrealhd) ? i_sub_d : i_sub_s);

		if (!optop(e)) {
			checknan(e, sp);
		}

		return mka;

	case fmult_tag:
		mka.regmove = fop(e, sp, dest, (sh(e)->tag != shrealhd) ? i_mul_d : i_mul_s);

		if (!optop(e)) {
			checknan(e, sp);
		}

		return mka;

	case fdiv_tag:
		mka.regmove = fop(e, sp, dest, (sh(e)->tag != shrealhd) ? i_div_d : i_div_s);

		if (!optop(e)) {
			checknan(e, sp);
		}

		return mka;

	case fneg_tag:
		mka.regmove = fmop(e, sp, dest, (sh(e)->tag != shrealhd) ? i_neg_d : i_neg_s);

		if (!optop(e)) {
			checknan(e, sp);
		}

		return mka;

	case fabs_tag:
		mka.regmove = fmop(e, sp, dest, (sh(e)->tag != shrealhd) ? i_abs_d : i_abs_s);

		if (!optop(e)) {
			checknan(e, sp);
		}

		return mka;

	case float_tag: {
		exp in = child(e);
		where w;
		int r;
		int f = (dest.answhere.discrim == infreg) ? regalt(dest.answhere) /* cheat */
				: getfreg(sp.flt);
		freg frg;
		ans aa;
		ash ain;
		ain = ashof(sh(in));
		frg.fr = f;
		frg.dble = 0;

		if (ain.ashsize == 32 && sh(in)->tag != ulonghd) {
			setfregalt(w.answhere, frg);
			w.ashwhere = ashof(sh(in));
			code_here(in, sp, w);
			/* evaluate fix pt arguement into float pt reg f */
		} else {
			/* bytes and halfs must go through fixpt
						   regs */
			r = reg_operand(in, sp);
			cop_ins(i_mtc1, r, f << 1);
		}

		rrfp_ins((sh(e)->tag != shrealhd) ? i_cvt_d_w : i_cvt_s_w,
		         f << 1, f << 1);
		if (sh(e)->tag != shrealhd) {
			frg.dble = 1;
		}

		if (sh(in)->tag == ulonghd) {
			int tmp = getreg(sp.fixed);
			int constf = getfreg(guardfreg(f, sp).flt);
			rri_ins(i_and, tmp, r, 0x80000000);
			cop_ins(i_mtc1, tmp, constf << 1);
			rrfp_ins((frg.dble) ? i_cvt_d_w : i_cvt_s_w,
			         constf << 1, constf << 1);
			rrrfp_ins((frg.dble) ? i_sub_d : i_sub_s, f << 1, f << 1, constf << 1);
			rrrfp_ins((frg.dble) ? i_sub_d : i_sub_s, f << 1, f << 1, constf << 1);
		}

		setfregalt(aa, frg);
		move(aa, dest, sp, 1);
		mka.regmove = (frg.dble) ? - (f + 32) : (f + 32);

		return mka;
	}

	case chfl_tag: {
		int to   = sh(e)->tag;
		int from = sh(child(e))->tag;
		bool dto   = (to   != shrealhd) ? 1 : 0;
		bool dfrom = (from != shrealhd) ? 1 : 0;

		if (dto == dfrom) {	/* no change in representation */
			return make_code(child(e), sp, dest, exitlab);
		} else {
			freg frg;
			ans aa;
			where w;

			if (dest.answhere.discrim == infreg) {
				frg = fregalt(dest.answhere);
			} else {
				frg.fr = getfreg(sp.flt);
			}

			frg.dble = dfrom;
			setfregalt(aa, frg);
			w.answhere = aa;
			w.ashwhere = ashof(sh(child(e)));
			code_here(child(e), sp, w);

			if (!optop(e) && !dto) {
				setnoreorder();
			}

			rrfp_ins((dfrom) ? i_cvt_s_d : i_cvt_d_s, frg.fr << 1, frg.fr << 1);
			frg.dble = dto;
			setfregalt(aa, frg);
			move(aa, dest, sp, 1);
			mka.regmove = (frg.dble) ? - (frg.fr + 32) : (frg.fr + 32);

			if (!optop(e) && !dto) {
				setreorder();
				checknan(e, sp);
			}

			return mka;
		}
	}

	case and_tag: {
		exp r = child(e);
		exp l = next(child(e));
		ans aa;
		space nsp;
		where d1;

		if (l->last && l->tag == val_tag && (no(l) == 255 || no(l) == 0xffff)
		    && ((r->tag == name_tag && regofval(r) == 100)
		        || (r->tag == cont_tag &&
		            (child(r)->tag != name_tag
		             || regofval(child(r)) > 0)))
		    && (aa = iskept(r), (aa.discrim == inreg && regalt(aa) == 0)))
		{
			/* can use load short instructions */
			where w;
			int   dsize = dest.ashwhere.ashsize;
			int   asize = (no(l) == 255) ? 8 : 16;

			w = locate(r, sp, sh(r), 0);
			if (w.answhere.discrim == notinreg
			    && dest.answhere.discrim == notinreg && no(l) == 0xffff) {
				instore isw;
				instore isd;
				isw = insalt(w.answhere);
				isd = insalt(dest.answhere);

				if (!isw.adval && isd.adval && isw.b.base == isd.b.base &&
				    isd.b.offset == isw.b.offset) {

					if (dsize > 16) {
						if (endian == ENDIAN_LITTLE) {
							isd.b.offset += 2;
						}
						/* just clear out top bits */
						ls_ins(i_sh, 0, isd.b);
					}

					return mka;
				}

				/* else drop through to load short case */
			}

			if (endian == ENDIAN_LITTLE) {
				nsp = guard(w, sp);
				setregalt(aa, getreg(nsp.fixed));
				d1.answhere = aa;
				d1.ashwhere.ashsize = d1.ashwhere.ashalign = asize;
				move(w.answhere, d1, nsp, 0);
				mka.regmove
				    = move (aa, dest, guard(d1, nsp), 0 /* unsigned */ );

				return mka;
			}
		}

		mka.regmove = comm_op(e, sp, dest, i_and);
		return mka;
	}

	case or_tag:
		mka.regmove = comm_op(e, sp, dest, i_or);
		return mka;

	case xor_tag:
		mka.regmove = comm_op(e, sp, dest, i_xor);
		return mka;

	case not_tag:
		if (child(e)->tag == or_tag) {
			mka.regmove = comm_op(child(e), sp, dest, i_nor);
		} else {
			mka.regmove = monop(e, sp, dest, i_not);
		}

		return mka;

	case offset_pad_tag: {
		int r, v;
		ans aa;

		if (al2(sh(child(e))) >= al2(sh(e))) {
			if (al2(sh(e)) != 1 || al2(sh(child(e))) == 1) {
				/* is already aligned correctly, whether as bit or byte-offset*/
				e = child(e);
				goto tailrecurse;
			}

			r = regfrmdest(&dest, sp);
			v = reg_operand(child(e), sp);
			rri_ins(i_sll, r, v, 3);
		} else {
			int al = (al2(sh(child(e))) == 1) ? al2(sh(e)) : (al2(sh(e)) / 8);
			r = regfrmdest(&dest, sp);
			v = reg_operand(child(e), sp);
			rri_ins(i_addu, r, v, al - 1);
			rri_ins(i_and, r, r, -al);

			if (al2(sh(child(e))) == 1) {
				/* operand is bit-offset, byte-offset required */
				rri_ins(i_sra, r, r, 3);
			}
		}

		setregalt(aa, r);
		mka.regmove = move(aa, dest, guardreg(r, sp), 0);

		return mka;
	}

	case locptr_tag: {
		int pr = reg_operand(child(e), sp);
		int ansr = regfrmdest(&dest, sp);

		baseoff b;
		ans aa;
		b.base = pr;
		b.offset = -12;
		ls_ins(i_lw, ansr, b);
		setregalt(aa, ansr);
		mka.regmove = move(aa, dest, guardreg(ansr, sp), 0);

		return mka;
	}

	case cont_tag:
	case name_tag:
	case field_tag:
	case reff_tag:
	case addptr_tag:
	case subptr_tag:
	case contvol_tag: {
		where w;
		bool sgned;

		int dr = (dest.answhere.discrim == inreg) ? dest.answhere.val.regans : 0;

		if (e->tag == contvol_tag) {
			clear_all();
			setvolatile();
		}

		w = locate(e, sp, sh(e), dr);
		/* 'address of arguement */
		sgned = ((w.ashwhere.ashsize >= 32) || is_signed(sh(e))) ? 1 : 0;
		mka.regmove = move(w.answhere, dest, (guard(w, sp)), sgned);
		if (e->tag == contvol_tag) {
			mka.regmove = NOREG;
			setnovolatile();
		}

		return mka;
	}

	case real_tag: {
		int dble = shape_size(sh(e)) > 32;
		r2l x;
		int i;
		ans aa;
		instore isa;
		int n = (nca < 16) ? nca : 16;

		x = real2longs_IEEE(&flptnos[no(e)], dble);
		for (i = 0; i < n; i++) {
			rcache *r = &rca[i];
			if (r->dble == dble && r->r.i1 == x.i1 && r-> r.i2 == x.i2) {
				isa = r->ad;
				goto found;
			}
		}

		isa = evaluated(e, 0, NULL);
		rca[nca & 15].dble = dble;
		rca[nca & 15].r = x;
		rca[nca & 15].ad = isa;
		nca++;
		settext();

found:

		setinsalt(aa, isa);
		mka.regmove = move(aa, dest, sp, 0);

		return mka;
	}

	case string_tag: {
		instore isa;
		ans aa;
		bool sgned = ((ashof(sh(e)).ashsize >= 32) || is_signed(sh(e))) ? 1 : 0;

		isa = evaluated(e, 0, NULL);
		/* place constant in appropriate data segment */
		settext();
		setinsalt(aa, isa);
		mka.regmove = move(aa, dest, sp, sgned);

		return mka;
	}

	case val_tag:
		if (shape_size(sh(e)) > 32) {
			flt64 temp;
			int ov;
			int r = getreg(sp.fixed);
			space nsp;
			baseoff bc;
			ans aa;
			if (dest.answhere.discrim != notinreg) {
				return mka;
			}
			if (isbigval(e)) {
				temp = flt_to_f64(no(e), 0, &ov);
			} else {
				temp.big = (is_signed(sh(e)) && no(e) < 0) ? -1 : 0;
				temp.small = no(e);
			}
			nsp = guardreg(r, sp);
			bc.base = 0;
			bc.offset = temp.small;
			ls_ins(i_li, r, bc);
			setregalt(aa, r);
			dest.ashwhere.ashsize = 32;
			dest.ashwhere.ashalign = 32;
			move(aa, dest, nsp, 1);
			bc.offset = temp.big;
			ls_ins(i_li, r, bc);
			dest.answhere.val.instoreans.b.offset += 4;
			move(aa, dest, nsp, 1);
			return mka;
		}

		if (no(e) == 0) {
			goto null_tag_case;
		} else {
			ash a;

			a = ashof(sh(e));
			if (a.ashsize == 32 || !is_signed(sh(e))) {
				constval = no(e);
			} else if (a.ashsize == 8) {
				constval = no(e) & 255;
				constval -= (constval & 128) << 1;
			} else {
				constval = no(e) & 65535;
				constval -= (constval & 32768) << 1;
			}

			goto moveconst;
		}

	case top_tag:
		return mka;

	case dump_tag: {
		long fxd = no(e);
		long fld = no(pt(e));
		long old_fixdone = fixdone;
		long old_fltdone = fltdone;
		long old_result_label = result_label;
		exp l;

		result_label = 0;
		dump_sregs(fxd, fld);
		if ((fxd & (1 << 31))) {
			sp.fixed &= ~(1 << 31); /*release link reg */
		}

		for (l = child(crt_proc); l->tag == ident_tag && isparam(l);) {
			/* move any pars still in registers which go into dump regs */
			int sr = child(l)->props;
			int tr = no(l);

			if ((l->props & inanyreg) != 0 && (tr != sr) && sr != 0) {
				if ((l->props & infreg_bits) != 0 &&
				    (fld & (3 << (sr << 1))) != 0) {
					if (sh(child(l))->tag != shrealhd) {
						rrfp_ins(i_mov_d, sr << 1, tr << 1);
					} else {
						rrfp_ins(i_mov_s, sr << 1, tr << 1);
					}
					sp.flt &= ~(1 << tr); /* release fpar reg */
					no(l) = sr;
					child(l)->props = tr;
				} else if ((fxd & (1 << sr)) != 0) {
					mon_ins(i_move, sr, tr);
					sp.fixed &= ~(1 << tr); /* release par reg */
					no(l) = sr;
					child(l)->props = tr;
				}
			}

			l = next(child(l));
			if (l->tag == dump_tag) {
				l = child(l);
			}
		}

		code_here(child(e), sp, dest);
		for (l = child(crt_proc); l->tag == ident_tag && isparam(l);) {
			/* restore structure of moved pars */
			int sr = child(l)->props;
			int tr = no(l);

			if ((l->props & inanyreg) != 0 && (tr != sr) && sr != 0) {
				if ((l->props & infreg_bits) != 0 &&
				    (fld & (3 << (tr << 1))) != 0) {
					no(l) = sr;
					child(l)->props = tr;
				} else if ((fxd & (1 << tr)) != 0) {
					no(l) = sr;
					child(l)->props = tr;
				}
			}

			l = next(child(l));
			if (l->tag == dump_tag) {
				l = child(l);
			}
		}

		if (sh(e)->tag != bothd) {
			restore_sregs(fxd, fld);
		}

		fixdone = old_fixdone;
		fltdone = old_fltdone;
		result_label = old_result_label;

		return mka;
	}

	case env_size_tag: {
		exp tag = child(child(e));
		procrec * pr = &procrecs[no(child(tag))];
		constval = (pr->frame_size + pr->callee_size) >> 3;
		goto moveconst;
	}

	case proc_tag:
	case general_proc_tag: {
		/*
		 * set up locals_offset, fixdump, floatdump, frame_size, dumpstart
		 * dec stack  ; output frame and mask
		 * code here;
		 */
		procrec * pr = & procrecs[no(e)];
		needs * ndpr = & pr->needsproc;
		long pprops = (ndpr->propneeds);
		bool leaf = (pprops & anyproccall) == 0;
		space tbd;
		space pars;
		long st;
		exp l;

		crt_proc = e;
		old_pls = NULL;

		frame_size = pr->frame_size;
		locals_offset = pr->locals_offset;
		max_args = pr->max_args;
		fixdump = pr->fixdump;
		floatdump = pr->floatdump;
		dumpstart = pr->dumpstart;
		fldumpstart = pr->fldumpstart;
		callee_size = pr->callee_size;

		setframe_flags(e, leaf);

		st = (frame_size + callee_size) >> 3;

		fixdone = fltdone = 0;	/* no s-regs have been dumped yet */

		tbd.fixed = fixdump;
		tbd.flt   = floatdump;

		pars.fixed = leaf ? 0 : (1 << 31);
		pars.flt   = 0;

		for (l = child(e);
		     l->tag == ident_tag && isparam(l)
		     && child(l)->tag	!= formal_callee_tag;
		     l = next(child(l)))
		{
			if ((l->props & infreg_bits) != 0) {
				int n = child(l)->props;
				if (n != no(l) && n != 0) {
					pars.flt |= (3 << (no(l) << 1));
				}
			} else if ((l->props & inreg_bits) != 0) {
				int n = child(l)->props;
				if (n != no(l) && n != 0) {
					pars.fixed |= (1 << no(l));
				}
			}
		}

		dump_opt(e, &tbd, &pars);

		if (PIC_code) {
			setnoreorder();
			out_cpload(current_symno, 25);
			if (as_file) {
				asm_printop(".cpload $25");
			}
			setreorder();
		}

		if (e->tag == general_proc_tag) {
			if (Has_vcallees) {
				baseoff b;
				b.base = 30;
				b.offset = -16;
				ls_ins(i_sw, local_reg, b); /* old l-reg in -16(30) */
				mon_ins(i_move, local_reg, 29);
				/* if(!leaf) */ {
					b.offset = -12; /* new l_reg in -12(30); */
					ls_ins(i_sw, local_reg, b);
				}
			} else if (Has_fp && e->tag == general_proc_tag) {
				rri_ins(i_addu, 30, 29, callee_size >> 3);
			}
			if (frame_size != 0) {
				rri_ins(i_subu, 29, 29, frame_size >> 3);
			}
		} else {
			if (st != 0) {
				rri_ins(i_subu, 29, 29, st);
			}
			if (Has_fp) {
				baseoff b;
				b.base = 29;
				b.offset = st - 4;
				ls_ins(i_sw, 30, b);
				rri_ins(i_addu, 30, 29, st);
			}
		}

		if (Has_tos) {
			baseoff b;
			b.base = 30;
			b.offset = -8;
			ls_ins(i_sw, 29, b);
		}

		diagPIClab = 0;
		if (PIC_code && !leaf) {
			dump_gp();
			if (diag != DIAG_NONE && frame_size != 0) {
				diagPIClab = new_label();
			}
		}

		allocaerr_lab = 0;
		if (proc_has_checkstack(e)) {
			baseoff b;
			exp stl = find_named_tag("__TDFstacklim",
			                         f_pointer(f_alignment(f_proc)));
			setvar(stl);
			b = boff(stl);
			stackerr_lab = new_label();
			ls_ins(i_lw, 2, b);
			condrr_ins(i_bgt, 2, 29, stackerr_lab);
		} else {
			stackerr_lab = 0;
		}

		setframe(st);
		/* I'm not sure that this is the right order for these -
		   diagnostics ? */

		if (fixdump != 0) {
			setmask(fixdump, dumpstart - st - 4);
		}
		if (floatdump != 0) {
			setfmask(floatdump, fldumpstart - st - 4);
		}

		if ((pprops & realresult_bit) != 0) {
			/* proc has real result */
			freg frg;
			frg.fr = 0;
			frg.dble = (pprops & longrealresult_bit) ? 1 : 0;
			setfregalt(procans, frg);
		} else if ((pprops & has_result_bit) != 0) {
			/* proc has fixed pt result */
			setregalt(procans, 2);
		} else {		/* proc has no result */
			setregalt(procans, 0);
		}

		result_label = 0;
		aritherr_lab = 0;

		code_here(child(e), guardreg(31, sp), nowhere);

		/* evaluate body of proc */
		if (stackerr_lab != 0 || allocaerr_lab != 0) {
			if (stackerr_lab != 0) {
				set_label(stackerr_lab);
				rri_ins(i_addu, 29, 29, frame_size >> 3);
			}

			if (allocaerr_lab != 0) {
				set_label(allocaerr_lab);
			}

			do_exception(MIPS_SIGUSR1);
		}

		if (aritherr_lab != 0) {
			set_label(aritherr_lab);
			do_exception(MIPS_SIGFPE);
		}

		if (diagPIClab != 0) {
			set_label(diagPIClab);
			uncond_ins(i_j, 31);
		}

		return mka;
	}

	case alloca_tag: {
		exp s = child(e);
		int r = regfrmdest(&dest, sp);
		ans aa;

		if (checkalloc(e)) {
			int tmp = getreg(guardreg(r, sp).fixed);
			exp stl = find_named_tag("__TDFstacklim",
			                         f_pointer(f_alignment(f_proc)));
			baseoff b;
			setvar(stl);

			if (s->tag == val_tag) {
				rri_ins(i_subu, r, 29, (no(s) + 7) & ~7);
			} else {
				int rr = reg_operand(s, sp);
				rri_ins(i_addu, tmp, rr, 7);
				rri_ins(i_and, tmp, tmp, ~7);
				rrr_ins(i_subu, r, 29, tmp);
			}

			b = boff(stl);
			if (allocaerr_lab == 0) {
				allocaerr_lab = new_label();
			}

			ls_ins(i_lw, tmp, b);
			condrr_ins(i_bgt, tmp, r, allocaerr_lab);
			rri_ins(i_addu, 29, r, 0);
		} else {
			if (s->tag == val_tag) {
				rri_ins(i_subu, 29, 29, (no(s) + 7) & ~7);
			} else {
				int tmp = getreg(sp.fixed);
				int rr = reg_operand(s, sp);
				rri_ins(i_addu, tmp, rr, 7);
				rri_ins(i_and, tmp, tmp, ~7);
				rrr_ins(i_subu, 29, 29, tmp);
			}
		}

		reset_tos();
		rri_ins(i_addu, r, 29, (max_args >> 3));
		setregalt(aa, r);
		mka.regmove = move(aa, dest, sp, 1);

		return mka;
	}

	case last_local_tag: {
		int r = regfrmdest(&dest, sp);
		ans aa;

		rri_ins(i_addu, r, 29, max_args >> 3);
		setregalt(aa, r);
		mka.regmove = move(aa, dest, sp, 1);

		return mka;
	}

	case local_free_tag: {
		exp p = child(e);
		int r = reg_operand(p, sp);
		exp off = next(p);

		if (off->tag == val_tag) {
			rri_ins(i_addu, r, r, (no(off) + 7) & ~7);
		} else {
			int tmp = reg_operand(off, guardreg(r, sp));
			rri_ins(i_addu, tmp, tmp, 7);
			rri_ins(i_and, tmp, tmp, ~7);
			rrr_ins(i_addu, r, r, tmp);
		}

		if (Has_fp) {
			rri_ins(i_subu, 29, r, max_args >> 3);
			reset_tos();
		}

		return mka;
	}

	case local_free_all_tag:
		if (Has_fp) {
			rri_ins(i_subu, 29, 30 , (frame_size + callee_size) >> 3);
			reset_tos();
		}

		return mka;

	case current_env_tag: {
		int r = regfrmdest(&dest, sp);
		ans aa;

		if (Has_fp) {
			mon_ins(i_move, r, 30);
		} else {
			rri_ins(i_addu, r, 29, (frame_size + callee_size) >> 3);
		}

		setregalt(aa, r);
		mka.regmove = move(aa, dest, sp, 1);

		return mka;
	}

	case env_offset_tag:
	case general_env_offset_tag:
		constval = frame_offset(child(e));
		goto moveconst;

	case null_tag: {
		ans aa;

null_tag_case:

		setregalt(aa, 0);
		mka.regmove = move(aa, dest, sp, 1);

		return mka;
	}

	/* case trunc_tag: */
	case round_tag: {
		int r = (dest.answhere.discrim == inreg) ? regalt(dest.answhere)
		          :     getreg(sp.fixed);
		int sfr = freg_operand(child(e), sp);
		int dfr = getfreg(guardfreg(sfr, sp).flt);
		ans aa;
		int s  = shape_size(sh(child(e)));
		int mr = (round_number(e) == f_to_nearest) ? 3 :
		         (round_number(e) == f_toward_zero) ? 2 :
		         (round_number(e) == f_toward_larger) ? 1 :
		         (round_number(e) == f_toward_smaller) ? 0 : 3;
		int r1;

		if (r == 0) {
			r = getreg(sp.fixed);
		}

		sp = guardreg(r, sp);
		r1 = getreg(sp.fixed);
		if (!optop(e)) {
			setnoreorder();
		}

		cop_ins(i_cfc1, r, 31);
		rrr_ins(i_or, 0, 0, 0); /* nop */
		rri_ins(i_or, r1, r , 3);
		rri_ins(i_xor, r1, r1, mr);
		cop_ins(i_ctc1, r1, 31);
		rrr_ins(i_or, 0, 0, 0); /* nop */
		rrfp_ins((s == 32) ? i_cvt_w_s : i_cvt_w_d, dfr << 1, sfr << 1);

		if (!optop(e)) {
			setreorder();
			checknan(e, guardreg(r, sp));
		}

		cop_ins(i_ctc1, r, 31);
		cop_ins(i_mfc1, r, dfr << 1);

		/*
		 *  cfc1    r,$31
		 *  cfc1    r,$31
		 *  ori     r1,dfr,0x3
		 *  xori    r1,r1, to_n = 3, to_z = 2, to+i = 1, to-i = 0
		 *  ctc1    r1,$31
		 *  srl     r0,r0,0
		 *  cvt.w.s $f6,$f0
		 *  check
		 *  ctc1    r,$31
		 *  expansion of i_round_w_s etc
		 */

		setregalt(aa, r);
		mka.regmove = move(aa, dest, sp, 1);

		return mka;
	}

	case int_to_bitf_tag: {
		int r;
		where w;
		ash a;
		ash ai;
		ans aa;
		space nsp;

		a = ashof(sh(e));
		ai = ashof(sh(child(e)));
		r = regfrmdest(&dest, sp);

		setregalt(w.answhere, r);
		w.ashwhere = a;
		code_here(child(e), sp, w);

		if (a.ashsize != ai.ashsize) {
			rri_ins(i_and, r, r, (1 << a.ashsize) - 1);
		}

		nsp = guardreg(r, sp);
		setregalt(aa, r);
		move(aa, dest, nsp, 0);

		return mka;
	}

	case bitf_to_int_tag: {
		ash a;
		int   r;
		where w;
		a = ashof(sh(child(e)));
		r = regfrmdest(&dest, sp);

		setregalt(w.answhere, r);
		w.ashwhere = a;
		code_here(child(e), sp, w);

		if (a.ashsize != 32) {
			if (is_signed(sh(e))) {
				rri_ins(i_sll, r, r, 32 - a.ashsize);
				rri_ins(i_sra, r, r, 32 - a.ashsize);
			} else {
				rri_ins(i_and, r, r, ((1 << a.ashsize) - 1));
			}
		}

		move(w.answhere, dest, guardreg(r, sp), 0);
		keepreg(e, r);

		return mka;
	}

	case movecont_tag: {
		exp szarg = next(next(child(e)));
		int dr, sr, szr, mr;
		int lout = new_label();
		space nsp;
		int bytemove;
		where w;

		sr = getreg(sp.fixed);
		setregalt(w.answhere, sr);
		w.ashwhere = ashof(sh(child(e)));
		IGNORE make_code(child(e), sp, w , 0);
		nsp = guardreg(sr, sp);
		dr = getreg(nsp.fixed);
		setregalt(w.answhere, dr);
		IGNORE make_code(next(child(e)), nsp, w, 0);
		nsp = guardreg(dr, nsp);
		w.ashwhere = ashof(sh(next(next(child(e)))));
		szr = getreg(nsp.fixed);
		setregalt(w.answhere, szr);
		IGNORE make_code(szarg, nsp, w, 0);
		nsp = guardreg(szr, nsp);
		mr = getreg(nsp.fixed);
		bytemove = (al2(sh(szarg)) <= 8);

		if (szarg->tag != val_tag || no(szarg) == 0) {
			condrr_ins(i_beq, szr, 0, lout);
		}

		if (isnooverlap(e)) {
			move_dlts(dr, sr, szr, mr, bytemove);
		} else {
			int gtlab = new_label();
			condrr_ins(i_bgt, dr, sr, gtlab);
			move_dlts(dr, sr, szr, mr, bytemove);
			uncond_ins(i_b, lout);
			set_label(gtlab);
			move_dgts(dr, sr, szr, mr, bytemove);
		}

		set_label(lout);

		return mka;
	}

	case set_stack_limit_tag: {
		baseoff b;
		int r = reg_operand(child(e), sp);
		exp stl = find_named_tag("__TDFstacklim",
		                         f_pointer(f_alignment(f_proc)));
		setvar(stl);
		b = boff(stl);
		ls_ins(i_sw, r, b);

		return mka;
	}

	case give_stack_limit_tag: {
		baseoff b;
		ans aa;
		int r = regfrmdest(&dest, sp);
		exp stl = find_named_tag("__TDFstacklim",
		                         f_pointer(f_alignment(f_proc)));
		setvar(stl);
		b = boff(stl);
		ls_ins(i_lw, r, b);
		setregalt(aa, r);
		move(aa, dest, guardreg(r, sp), 1);

		return mka;
	}

	case trap_tag: {
		if (no(e) == f_overflow) {
			do_exception(MIPS_SIGFPE);
		} else if (no(e) == f_nil_access) {
			do_exception(MIPS_SIGSEGV);
		} else {
			do_exception(MIPS_SIGUSR1);
		}

		return mka;
	}

	default:
		error(ERR_INTERNAL, "not done yet");
	}

moveconst:

	{
		int r = regfrmdest(&dest, sp);
		baseoff b;
		ans aa;

		if (r != 0) {
			/* somewhere! */
			b.base = 0;
			b.offset = constval;
			ls_ins(i_li, r, b);
			setregalt(aa, r);
			move(aa, dest, guardreg(r, sp), 1);
		}

		mka.regmove = r;

		return mka;
	}
}

