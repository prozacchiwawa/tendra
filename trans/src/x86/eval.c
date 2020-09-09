/*
 * Copyright 2002-2011, The TenDRA Project.
 * Copyright 1997, United Kingdom Secretary of State for Defence.
 *
 * See doc/copyright/ for the full copyright terms.
 */

#include <shared/bool.h>
#include <shared/check.h>
#include <shared/error.h>

#include <local/out.h>
#include <local/code.h>
#include <local/exp.h>

#include <tdf/shape.h>
#include <tdf/tag.h>

#include <reader/basicread.h>
#include <reader/externs.h>
#include <reader/table_fns.h>

#include <construct/exp.h>
#include <construct/install_fns.h>
#include <construct/machine.h>

#include <flpt/flpt.h>

#include <main/flags.h>
#include <main/print.h>

#ifdef TDF_DIAG3
#include <diag3/diag_fns.h>
#include <diag3/diag_reform.h>
#endif
#ifdef TDF_DIAG4
#include <diag4/diag_fns.h>
#include <diag4/diag_reform.h>
#endif

#include "localtypes.h"
#include "instr.h"
#include "ops.h"
#include "assembler.h"
#include "eval.h"

static
void outsize(int n)
{
	switch ((n + 7) / 8) {
	case 1:  outbyte();  break;
	case 2:  outshort(); break;
	default: outlong();  break;
	}
}

static long
evalexp(exp e)
{
	switch (e->tag) {
	case val_tag:
	case null_tag:
	case top_tag:
		if (sh(e)->tag == offsethd && al2(sh(e)) >= 8) {
			return no(e) >> 3;
		}

		return no(e);

	case bitf_to_int_tag:
		return evalexp(child(e));

	case int_to_bitf_tag: {
		long  w = evalexp(child(e));

		if (shape_align(sh(e)) != 1) {
			error(ERR_INTERNAL, "should be align 1");
		}

		if (shape_size(sh(e)) != 32) {
			w &= ((1 << shape_size(sh(e))) - 1);
		}

		return w;
	}

	case not_tag: return ~evalexp(child(e));
	case and_tag: return evalexp(child(e))  & evalexp(next(child(e)));
	case or_tag:  return evalexp(child(e))  | evalexp(next(child(e)));
	case xor_tag: return evalexp(child(e))  ^ evalexp(next(child(e)));
	case shr_tag: return evalexp(child(e)) >> evalexp(next(child(e)));
	case shl_tag: return evalexp(child(e)) << evalexp(next(child(e)));

	case concatnof_tag: {
		long  wd = evalexp(child(e));
		return wd | (evalexp(next(child(e))) << shape_size(sh(child(e))));
	}

	case clear_tag:
		if (shape_size(sh(e)) <= 32) {
			return 0;
		}
		break;

	case env_offset_tag:
		if (child(e)->tag == 0) {
			return no(child(e)) / 8;
		}
		break;

	case env_size_tag: {
		dec *et = nextg(child(child(e)));
		if (et -> processed) {
			return et -> index;
		}
		break;
	}

	case offset_max_tag: {
		long a = evalexp(child(e));
		long b = evalexp(next(child(e)));
		return a > b ? a : b;
	}

	case offset_pad_tag:        return rounder(evalexp(child(e)), shape_align(sh(e)) / 8);
	case offset_add_tag:        return evalexp(child(e)) + evalexp(next(child(e)));
	case offset_mult_tag:       return evalexp(child(e)) * evalexp(next(child(e)));
	case offset_div_tag:
	case offset_div_by_int_tag: return evalexp(child(e)) / evalexp(next(child(e)));
	case offset_subtract_tag:   return evalexp(child(e)) - evalexp(next(child(e)));
	case offset_negate_tag:     return -evalexp(child(e));

	case seq_tag:
		if (child(child(e))->tag == prof_tag && child(child(e))->last) {
			return evalexp(next(child(e)));
		}
		break;

	case cont_tag:
		if (PIC_code && child(e)->tag == name_tag && isglob(child(child(e)))
		    && child(child(child(e))) != NULL
		    && !(nextg(child(child(e))) -> var))
		{
			return evalexp(child(child(child(e))));
		}
		break;
	}

	error(ERR_INTERNAL, "illegal val const");
	return 0;
}

/* outputs a value */
static void
evalval(exp e)
{
	int e_size = shape_size(sh(e));
	unsigned char  n = e->tag;
	int ov;

	if (n == val_tag) {
		int k = (sh(e)->tag == offsethd && al2(sh(e)) != 1)
		        ? no(e) / 8 : no(e);
		flt64 x;
		if (isbigval(e)) {
			x = flt_to_f64(k, is_signed(sh(e)), &ov);
			k = x.small;
		}

		switch (e_size) {
		case  8: asm_printf("%d", k & 0xff);   break;
		case 16: asm_printf("%d", k & 0xffff); break;
		case 32: asm_printf("%d", k);          break;

		case 64:
			asm_printf("%d, ", k);
			if (isbigval(e)) {
				SET(x);
				asm_printf("%d", x.big);
			} else if (is_signed(sh(e)) && k < 0) {
				asm_printf("%d", -1);
			} else {
				asm_printf("%d", 0);
			}
			break;

		default:
			asm_printf("%d", k);
			break;
		}

		return;
	}

	if (n == real_tag) {
		outreal(e);
		return;
	}

	if (n == reff_tag && child(e)->tag == name_tag && isglob(child(child(e)))) {
		outopenbr();
		asm_printf("%s + %ld", nextg(child(child(e))) -> name, (no(e) + no(child(e))) / 8);
		outclosebr();
		return;
	}

	if (n == name_tag) {
		if (no(e) != 0) {
			outopenbr();
			asm_printf("%s + %ld", nextg(child(e)) -> name, no(e) / 8);
			outclosebr();
		} else {
			asm_printf("%s", nextg(child(e)) -> name);
		}
		return;
	}

	{
		int k = evalexp(e);

		switch (e_size) {
		case  8: asm_printf("%d", k & 0xff);   break;
		case 16: asm_printf("%d", k & 0xffff); break;
		case 32: asm_printf("%d", k);          break;
		default: asm_printf("%d", k);          break;
		}
	}
}

/* auxiliary for evalaux */
static void
clear_out(int n, int isconst, int al)
{
	if (n == 0) {
		return;
	}

	if (!isconst) {
		asm_printf(".set .,.+%d\n", n);
		return;
	}

	while (al >= 32 && n >= 4) {
		outlong();
		asm_printf("0\n");
		n -= 4;
	}

	while (n > 0) {
		outbyte();
		asm_printf("0\n");
		--n;
	}
}

/* does the work of outputting of constants recursively */
static void
evalaux(exp e, int isconst, int al)
{
	int e_size = shape_size(sh(e));
	unsigned char  n = e->tag;

	if (n == compound_tag) {
		/* output components in turn */
		int work = 0;
		exp offe;
		exp val;
		int bits_left = 0;
		int crt_off = 0;
		int off, offn, sz, nx, i;

		if (child(e) == NULL) {
			return;
		}

		offe = child(e);

		for (;;) {
			off = no(offe);
			val = next(offe);

			if (bits_left &&
			    off >= (crt_off + 8)) {
				outbyte();
				asm_printf("%d\n", work & 0xff);
				crt_off += 8;
				work = 0;
				bits_left = 0;
			}

			if (off < crt_off) {
				error(ERR_INTERNAL, "compound constants out of order");
			}
			if (off >= (crt_off + 8)) {
				clear_out((off - crt_off) / 8, isconst, al);
				crt_off = off & -8;
			}

			if (sh(val)->tag != bitfhd) {
				evalaux(val, isconst, (crt_off + al) & 56);
				crt_off += shape_size(sh(val));
			} else {
				offn = off - crt_off;
				sz = shape_size(sh(val));
				nx = (val->tag == int_to_bitf_tag) ? no(child(val)) : no(val);
				work += nx << offn;
				bits_left = offn + sz;
				if ((offn + sz) <= 32) {
					while ((offn + sz) >= 8) {
						outbyte();
						asm_printf("%d\n", work & 0xff);
						crt_off += 8;
						work >>= 8;
						offn -= 8;
						bits_left = offn + sz;
					}
					work &= ((1 << bits_left) - 1);
				} else {
					for (i = 0; i < 4; ++i) {
						outbyte();
						asm_printf("%d\n", work & 0xff);
						crt_off += 8;
						work >>= 8;
						offn -= 8;
						bits_left = offn + sz;
					}
					work = nx >> bits_left;
				}
			}

			if (val->last) {
				/* CLEAR OUT SHAPE size_shape(e) - crt_off */
				if (bits_left) {
					outbyte();
					asm_printf("%d\n", work & 0xff);
					crt_off += 8;
				}
				clear_out((shape_size(sh(e)) - crt_off) / 8, isconst, 8);
				return;
			}

			offe = next(val);
		}
	}

	/*
	 * String constants
	 */
	if (n == string_tag) {
		char *s = nostr(e);
		int goon; /* Go on */
		int i, j;
		int char_size = (int) string_char_size(e);

		/* Total number of chars */
		goon = shape_size(sh(e)) / char_size;

#if 0
		{
			char *c = s;

			asm_printf("\t# string constant: ");
			do {
				switch (*c) {
				case '\n': asm_printf("\\n");    break;
				case '\0': asm_printf("\\0");    break;
				default:   asm_printf("%c", *c); break;
				}
			} while (*c++ != '\0');
			asm_printf("\n");
		}
#endif

		/* 10 chars per line */
		for (i = 0; goon; i += 10) {
			switch (char_size) {
			case 8:  outbyte();  break;
			case 16: outshort(); break;
			case 32: outlong();  break;
			case 64: outlong();  break;
			}

			for (j = i; goon && j < i + 10; ++j) {
				switch (e->props) {
				case 8:  asm_printf("%d", s[j]); break;
				case 16: asm_printf("%d", ((short *) (void *) s)[j]); break; /* the pun to short * is correct: jmf */
				case 32: asm_printf("%d", ((int   *) (void *) s)[j]); break; /* the pun to int   * is correct: jmf */

				case 64: {
					flt64 x;
					int ov;
					x = flt_to_f64(
					        ((int*)(void*)s)[j],
					        0,
					        &ov);
					asm_printf("%d, %d", x.small, x.big);
				}
				}

				--goon;
				if (goon && j < i + 9) {
					asm_printf(", ");
				}
			}
			asm_printf("\n");
		}
		return;
	}

	if (n == res_tag) {
		int  nb;
		nb = shape_size(sh(child(e))) / 8;
		clear_out(nb, isconst, shape_align(sh(child(e))));
		return;
	}

	if (n == ncopies_tag) {
		int  m = no(e);
		int  sz, i;
		exp val = child(e);

		while (val->tag == ncopies_tag) {
			m *= no(val);
			val = child(val);
		}

		sz = shape_size(sh(val)) / 8;
		if ((val->tag == null_tag ||
		     val->tag == val_tag) && !isbigval(val) && no(val) == 0) {
			clear_out(m * sz, isconst, shape_align(sh(val)));
		} else {
			for (i = 0; i < m; i++) {
				evalaux(val, isconst, al);
			}
		}
		return;
	}

	if (n == nof_tag) {
		exp t = child(e);

		if (t == NULL) {
			return;
		}

		for (;;) {
			evalaux(t, isconst, al);
			if (t->last) {
				return;
			}

			t = next(t);
			dot_align((shape_align(sh(t)) <= 8) ? 1 : shape_align(sh(t)) / 8);
		}
	}

	if (n == concatnof_tag) {
		evalaux(child(e), isconst, al);
		evalaux(next(child(e)), isconst, (al + shape_size(child(e))) & 63);
		return;
	}

	if (n == clear_tag) {
		int sz = shape_size(sh(e)) / 8;
		clear_out(sz, isconst, al);
		return;
	}

	if (n == chvar_tag && shape_size(sh(e)) == shape_size(sh(child(e)))) {
		sh(child(e)) = sh(e);
		evalaux(child(e), isconst, al);
		return;
	}

	outsize(e_size);
	evalval(e);
	asm_printf("\n");
}

/*
 * Output a constant of given label number cname,
 * or identifier s cname==-1 means use s
 */
void
evaluate(exp c, int cname, char *s, int isconst, int global
#ifdef TDF_DIAG3
	, diag_descriptor *diag_props
#endif
#ifdef TDF_DIAG4
	, struct dg_name_t *diag_props
#endif
	)
{
	int al = shape_align(sh(c));

	if (global && cname == -1) {
		asm_printf(".globl %s\n", s);
	}

	if (sh(c)->tag == realhd ||
	    (sh(c)->tag == nofhd && ptno(sh(c)) == realhd) ||
	    shape_size(sh(c)) >= 512) {
		al = 64;
	}

	if (al <= 8) {
		dot_align(4);
	} else {
		dot_align(al / 8);
	}

	if (diag_props) {
		if (diag != DIAG_NONE) {
#ifdef TDF_DIAG4
			diag4_driver->out_diag_global(diag_props, global, cname, s);
#endif
#ifdef TDF_DIAG3
			diag3_driver->diag_val_begin(diag_props, global, cname, s);
#endif
		}
	}

	if (cname == -1) {
		asm_label("%s", s);
	} else {
		asm_label("%s%d", local_prefix, cname);
	}

	evalaux(c, isconst, al);

	if (global) {
		eval_postlude(s, c);
	}

	asm_printf("\n");

	if (diag_props) {
#ifdef TDF_DIAG3
		diag3_driver->diag_val_end(diag_props);
#endif
	}
}

