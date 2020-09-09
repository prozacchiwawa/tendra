/*
 * Copyright 2011, The TenDRA Project.
 * Copyright 1997, United Kingdom Secretary of State for Defence.
 *
 * See doc/copyright/ for the full copyright terms.
 */

/*
 * The main procedure defined here is evaluated which outputs assembly
 * for data. The parameters are an evaluated exp and an index
 * into the table of externals (or 0 meaning anonymous).
 */

#include <assert.h>
#include <stdio.h>
#include <ctype.h>

#include <shared/bool.h>
#include <shared/check.h>
#include <shared/error.h>

#include <local/al.h>		/* for MAX_BF_SIZE */
#include <local/ash.h>
#include <local/fbase.h>
#include <local/out.h>

#include <tdf/shape.h>
#include <tdf/tag.h>

#include <reader/exp.h>
#include <reader/basicread.h>

#include <utility/max.h>

#include <main/driver.h>
#include <main/flags.h>
#include <main/print.h>

#include <construct/installtypes.h>
#include <construct/dec.h>
#include <construct/ash.h>
#include <construct/exp.h>

#include <flpt/flpt.h>

#include "procrec.h"
#include "addr.h"
#include "translate.h"
#include "inst_fmt.h"
#include "frames.h"
#include "eval.h"


#define proc_tag 118
#define is_zero(e) is_comm(e)

/* various pieces of info for outputting data depending on shape */
static mm  scmm = { 127,       -128,        "\t.BYTE\t%ld\n" };
static mm uscmm = { 0xff,       0,          "\t.BYTE\t%ld\n" };
static mm  shmm = { 0x7fff,     0xffff8000, "\t.HALF\t%ld\n" };
static mm ushmm = { 0xffff,     0,          "\t.HALF\t%ld\n" };
static mm  swmm = { 0x7fffffff, 0x80000000, "\t.WORD\t%ld\n" };
static mm uswmm = { 0xffffffff, 0,          "\t.WORD\t%ld\n" };

/*
 * finds the data size from the range of an integer shape
 */
mm
maxmin(shape s)
{
	switch (s->tag) {
	case scharhd: return  scmm;
	case ucharhd: return uscmm;
	case swordhd: return  shmm;
	case uwordhd: return ushmm;
	case slonghd: return  swmm;
	case ulonghd: return uswmm;
	default:      return uswmm;
	}
}

int
next_data_lab(void)
{
	static int n = 100;
	return ++n;
}

int
next_PIC_pcrel_lab(void)
{
	static int n = 100;
	return ++n;
}

/*
 * Output a unary representation of the number val. val should be
 * less than or equal to 31 as it represent the number of bits
 * in a bitfield which does not occupy a whole machine word.
 */
static long
unary(int val)
{
	int loop;
	long result = 0;

	assert(val <= 31);

	for (loop = 0; loop < val; ++loop) {
		result <<= 1;
		result |= 1;
	}

	return result;
}

/*
 * Output assembler representation of floating number
 */
static void
outfloat(flpt f)
{
	UNUSED(f);

	error(ERR_SERIOUS, "Illegal floating point constant");
}

/*
 * This routine converts the real constant e into an array of longs giving the
 * bitpattern corresponding to this constant. Although care has been taken,
 * this may not work properly on all machines (although it should for all
 * IEEE machines).
 * It returns NULL if it cannot convert the number sufficiently accurately.
 */
static long *
realrep(exp e)
{
	int i, ex;
	char bits[128];
	static long longs[4];
	int exp_bits;
	long sz = shape_size(sh(e));

	/* Find size of exponent and mantissa */
	switch (sz) {
	case 32: exp_bits =  8; break;
	case 64: exp_bits = 11; break;
	default: exp_bits = 15; break;
	}

	if (e->tag == real_tag) {
		int j, k = -1;
		flt *f = flptnos + no(e);

		/* Deal with 0 */
		if (f->sign == 0) {
			for (i = 0; i < sz / 32; i++) {
				longs[i] = 0;
			}

			return longs;
		}

		/* Fill in sign */
		bits[0] = f->sign < 0 ? 1 : 0;

		/* Work out exponent */
		ex = FBITS * f->exp + FBITS - 1;

		/* Fill in mantissa */
		for (i = 0; i < MANT_SIZE; i++) {
			for (j = FBITS - 1; j >= 0; j--) {
				if ((f->mant[i]) & (1 << j)) {
					if (k >= 0) {
						if (k < sz) {
							bits[k] = 1;
						}
						k++;
					} else {
						/* Ignore first 1 */
						k = exp_bits + 1;
					}
				} else {
					if (k >= 0) {
						if (k < sz) {
							bits[k] = 0;
						}
						k++;
					} else {
						/* Step over initial zeros */
						ex--;
					}
				}
			}
		}
	} else {
		error(ERR_SERIOUS,  "Illegal floating-point constant" );
		return NULL;
	}

	/* Fill in exponent */
	ex += (1 << (exp_bits - 1)) - 1;
	if (ex <= 0 || ex >= (1 << exp_bits) - 1) {
		error(ERR_SERIOUS,  "Floating point constant out of range");
	}
	for (i = 0; i < exp_bits; i++) {
		int j = exp_bits - i;
		bits[j] = (ex & (1 << i)) ? 1 : 0;
	}

	/* Convert bits to longs */
	for (i = 0; i < sz / 32; i++) {
		long b0 = 0, b1 = 0, b2 = 0, b3 = 0;
		int j;

		for (j = 0; j < 8; j++) {
			b0 = 2 * b0 + bits[32 * i + j];
		}
		for (j = 8; j < 16; j++) {
			b1 = 2 * b1 + bits[32 * i + j];
		}
		for (j = 16; j < 24; j++) {
			b2 = 2 * b2 + bits[32 * i + j];
		}
		for (j = 24; j < 32; j++) {
			b3 = 2 * b3 + bits[32 * i + j];
		}

		switch (endian) {
		case ENDIAN_LITTLE: longs[i] = (b0 <<  0) + (b1 << 8) + (b2 << 16) + (b3 << 24); break;
		case ENDIAN_BIG:    longs[i] = (b0 << 24) + (b1 << 16) + (b2 << 8) + (b3 <<  0); break;
		}
	}

	return longs;
}

long
evalexp(exp e)
{
	switch (e->tag) {
	case top_tag:
		return 0;

	case val_tag:
	case null_tag:
		if (sh(e)->tag == offsethd && al2(sh(e)) >= 8) {
			return no(e) >> 3;
		} else {
			return no(e);
		}

	case bitf_to_int_tag:
		return evalexp(child(e));

	case int_to_bitf_tag: {
		ash a;
		unsigned long w = evalexp(child(e));

		a = ashof(sh(e));
		if (a.ashalign != 1 && !(sh(e)->tag == cpdhd && a.ashalign == 32)) {
			error(ERR_SERIOUS, "should be align 1");
		}
		if (a.ashsize != 32) {
			w &= ((1 << a.ashsize) - 1);
		}

		return w;
	}

	case not_tag: return evalexp(child(e));
	case and_tag: return evalexp(child(e)) & evalexp(next(child(e)));
	case or_tag:  return evalexp(child(e)) | evalexp(next(child(e)));
	case xor_tag: return evalexp(child(e)) ^ evalexp(next(child(e)));

	case shr_tag: {
		bool sgned = is_signed(sh(e));

		asm_comment("evalexp() shr_tag: sgned=%d", sgned);
		if (sgned) {
			return ((long) evalexp(child(e))) >> evalexp(next(child(e)));
		} else {
			return ((unsigned long) evalexp(child(e))) >> evalexp(next(child(e)));
		}
	}

	case shl_tag:
		return evalexp(child(e)) << evalexp(next(child(e)));

	case concatnof_tag: {
		unsigned long w_lhs = evalexp(child(e));
		unsigned long w_rhs = evalexp(next(child(e)));
		ash ash_lhs, ash_rhs;
		ash_lhs = ashof(sh(child(e)));
		ash_rhs = ashof(sh(next(child(e))));

		assert(ash_lhs.ashalign == 1 && ash_lhs.ashsize <= 32);
		assert(ash_rhs.ashalign == 1 && ash_rhs.ashsize <= 32);
		assert(ash_lhs.ashsize + ash_rhs.ashsize <= 32);

		asm_comment("evalexp() concatnof_tag: lhs,rhs=%#lx,%#lx ash(rhs)=%ld,%ld",
		            w_lhs, w_rhs, ash_rhs.ashalign, ash_rhs.ashsize);

		if (ash_rhs.ashsize == 32) {
			/* avoid illegal shift by 32 */
			assert(w_lhs == 0);
			return w_rhs;
		}
		return (w_lhs << ash_rhs.ashsize) | w_rhs;
	}

	case env_offset_tag:
	case general_env_offset_tag:
		return frame_offset(child(e));

	case env_size_tag: {
		exp tag = child(child(e));
		procrec * pr = &procrecs[no(child(tag))];
		return (pr->frame_sz + 0) >> 3;
	}

	case offset_add_tag:        return evalexp(child(e)) + evalexp(next(child(e)));
	case offset_max_tag:        return MAX(evalexp(child(e)), evalexp(next(child(e))));
	case offset_pad_tag:        return rounder(evalexp(child(e)), shape_align(sh(e)));
	case offset_mult_tag:       return evalexp(child(e)) * evalexp(next(child(e)));
	case offset_div_tag:
	case offset_div_by_int_tag: return evalexp(child(e)) / evalexp(next(child(e)));
	case offset_subtract_tag:   return evalexp(child(e)) - evalexp(next(child(e)));
	case offset_negate_tag:     return -evalexp(child(e));

	case clear_tag: {
		ash a;

		a = ashof(sh(e));

		asm_comment("evalexp() clear_tag: ash=%ld,%ld", a.ashalign, a.ashsize);

		return 0;
	}

	default:
		error(ERR_SERIOUS, "tag not in evalexp");
		return 0;
	}

	UNREACHED;
}

static void
oneval(int val, int al, int rep)
{
	assert ( rep == 1 );
	outs( (al < 9 ? "\t.BYTE\t" : ( al < 17 ? "\t.HALF\t" : "\t.WORD\t")) );
	outn( val);
	outnl();
}

/*
 * Output as ascii for the human reader (48 bytes to the line).
 */
static void
outascii(char * str, int strsize)
{
	while (strsize > 0) {
		int i;

		outs("\t.STRING\t\"");

		for (i = 0 ; strsize > 0 && i < 48 ; i++) {
			unsigned char c = (unsigned char) *str;

			switch (c) {
			case '"':  outs("\\\"" ); break;
			case '\\': outs("\\\\" ); break;
			case   7:  outs("\\x07"); break;
			case '\b': outs("\\x08"); break;
			case '\f': outs("\\x0c"); break;
			case '\n': outs("\\x0a"); break;
			case '\r': outs("\\x0d"); break;
			case '\t': outs("\\x09"); break;
			case 11:   outs("\\x0b"); break;

			default:
				if (isprint(c)) {
					outc(c);
				} else {
					if (c < 16) {
						asm_printf("\\x0%x", c);
					} else {
						asm_printf("\\x%x", c);
					}
				}
				break;
			}
			str++;
			strsize--;
		}

		outs("\"\n");
	}
}

struct concbittypet {
	int bitposn;
	int value_size;
	unsigned long value;
};

typedef struct concbittypet concbittype;

static concbittype
emptyconcbit(int bitposn)
{
	concbittype start;

	start.bitposn    = bitposn;
	start.value_size = 0;
	start.value      = 0;

	return start;
}

static void
outconcbit(concbittype c)
{
	unsigned long w = c.value;
	int bytes = (c.value_size + 7) / 8;
	int i;

	insection(data_section);

	asm_comment("outconcbit: bits=%d w=%#lx", c.value_size, w);

	if (c.value_size == 0) {
		return;    /* avoid .BYTE with no data */
	}

	assert(c.value_size <= 32);

	/* to left end of word */
	if (c.value_size != 32) {
		w = w << (32 - c.value_size);
	}

	/* HPPA assembler only permits .WORD for 32-bit aligned values */

	/* output enough bytes */
	outs("\t.BYTE\t");
	for (i = 0; i < bytes; i++) {
		if (i != 0) {
			outc(',');
		}
		asm_printf("%#lx", (w >> 24) & 255);
		w = w << 8;
	}
	outnl();
	assert(w == 0);
}

/*
 * Add a value to a bit pattern
 */
static concbittype
addconcbitaux(unsigned long w, int sz, concbittype before)
{
	int wordpos; /* bit position in word */

	if (before.value_size == 32 || (before.value_size != 0 && (before.bitposn & 31) == 0)) {
		assert((before.bitposn & 31) == 0);
		wordpos = 32;
	} else {
		wordpos = (before.bitposn & 31);
	}

	assert(sz > 0);
	assert(sz <= 32);
	assert(before.value_size <= 32);
	assert(wordpos == 0 || before.value_size <= wordpos);

	if ((sz == 0 && (wordpos != 0 || before.value_size != 0)) ||
	     ((wordpos + sz) > 32))
	{
		/* int pad_bits = 32 - wordpos; gcc complains */
		assert(wordpos == 32); /* should be aligned automatically */
		outconcbit(before);

		/* clear before, as it has been output */
		before.value_size = 0;
		before.value      = 0;

		/* should be at word boundary */
		assert((before.bitposn & 31) == 0);
	}

	if (sz == 0) {
		return before;
	}

	/* add to before */
	if (sz == 32) {
		before.value = w;
	} else {
		switch (endian) {
		case ENDIAN_LITTLE: before.value = before.value | (w << before.value_size); break;
		case ENDIAN_BIG:    before.value = (before.value << sz) | (w & unary(sz)); break;
		}
	}

	before.bitposn += sz;
	before.value_size += sz;
	assert(before.value_size <= 32);

	return before;
}

static concbittype
evalconcbitaux(exp e, concbittype before)
{
	switch (e->tag) {
	case concatnof_tag: {
		concbittype lhs, rhs;
		lhs = evalconcbitaux(child(e), before);
		rhs = evalconcbitaux(next(child(e)), lhs);
		return rhs;
	}

	default:
		assert(shape_align(sh(e)) == 1);

		return addconcbitaux(evalexp(e), shape_size(sh(e)), before);
	}
}

static void
evalconcbit(exp e, int bitposn)
{
	concbittype start;
	start = emptyconcbit(bitposn);
	outconcbit(evalconcbitaux(e, start));
}

/*
 * Determine whether an exp is definitely zero valued. Zero-valued initialisers
 * can be put in the bss section. Does not exhaust all possibilities;
 * some zero valued expressions may have "is_zero(e) == 0".
 */
#if 0
bool
is_zero(exp e)
{
	if (e == NULL) {
		return 1;
	}

	/* +++ real values always explicitly initialised, which is not necessary */

	switch (e->tag) {
	case null_tag:
		return 1;

	case val_tag:
		return no(e) == 0 ? 1 : 0;

	case ncopies_tag:
	case int_to_bitf_tag:
		return is_zero(child(e));

	case compound_tag: {
		/* (compound_tag <offset> <initialiser> ...) */
		e = next(child(e));
		for (;;) {
			if (is_zero(e) == 0) {
				return 0;    /* found non-zero */
			}

			if (e->last) {
				return 1;    /* all done, all zero */
			}

			e = next(next(e));
		}

		UNREACHED;
	}

	case real_tag: {
		/* correct because bit representation of real zero is all zero bits */
		flt f;

		f = flptnos[no(e)];

		if (f.exp == 0) {
			int i;

			for (i = 0; i < MANT_SIZE; i++) {
				if (f.mant[i] != 0) {
					return 0; /* non-zero */
				}
			}

			return 1; /* all zero */
		}

		return 0;
	}

	default:
		return 0;
	}
}
#endif

void
set_align(int al)
{
	assert(al >= 8 && al <= 64);

	if (al > 8) {
		outs("\t.ALIGN\t");
		outn(al / 8);
		outnl();
	}
}

/*
 * This procedure outputs all expressions.
 */
void
evalone(exp e, int bitposn)
{
	ash a;
	/*  long al = ( long ) shape_align ( sh (e) ) ; gcc complains */
	long sz = ( long ) shape_size ( sh (e) );

	insection(data_section);

	a = ashof(sh(e));

	asm_comment("evalone: e->tag=%d, bitposn=%d, ash=%ld,%ld", e->tag, bitposn, a.ashsize, a.ashalign);

	set_align(a.ashalign);

	/* align bitposn */
	if (a.ashalign != 0) {
		bitposn = (bitposn / a.ashalign) * a.ashalign;
	}

	/* generate data initialiser for e */
	switch (e->tag) {
	case string_tag: {
		long char_size = e->props;
		long strsize = shape_size(sh(e)) / char_size;
		char *st = nostr(e);
		int i, j;

		if (char_size == 8) {
			outascii(st, strsize);
			return;
		}

		if (strsize > 0) {
			set_align(char_size);
		}

		for (j = 0; j < strsize;) {
			outs(char_size == 8
				? "\t.BYTE\t"
				: char_size == 16
					? "\t.HALF\t"
					: "\t.WORD\t");
			/* output chars in batches */
			for (i = j; i < strsize && i - j < 8; i++) {
				if (i != j) {
					outc(',');
				}

				/* switch (ptno(e)) */
				switch (char_size) {
				case  8: asm_printf("0x%x", st[i]);             break;
				case 16: asm_printf("0x%x", ((short *) st)[i]); break;
				case 32: asm_printf("0x%x", ((int   *) st)[i]); break;
				}
			}
			outnl();
			j = i;
		}
		return;
	}

	case real_tag :
		if ((has & HAS_LONG_DOUBLE)) {
			/* Floating point constant */
			flt *f = flptnos + no (e);
			r2l v;

			if ( sz == 32 ) {
				v = real2longs_IEEE(f, 0);

				outs("\t.WORD\t");
				outn(v.i1);
			} else if ( sz == 64 ) {
				v = real2longs_IEEE(f, 1);

				outs("\t.WORD\t");
				outn(v.i2);
				outc(',');
				outn(v.i1);
			} else {
				v = real2longs_IEEE(f, 2);
				outs("\t.WORD\t");
				outn(v.i4);
				outc(',');
				outn(v.i3);
				outc(',');
				outn(v.i2);
				outc(',');
				outn(v.i1);
			}
			outnl();
		} else {
			long sz = a.ashsize;
			long *p = realrep (e);
			if (p) {
				outs("\t.WORD\t");
				outn(p[0]);
				if (sz > 32) {
					outc(',');
					outn(p[1]);
				}
				outnl();
			} else {
				if (sz == 32) {
					outs( sz == 32 ? "\t.FLOAT\t0r" : "\t.DOUBLE\t0r");
				}
				outfloat(no(e));
				outnl();
			}
		}
		return;

	case null_tag:
	case top_tag:
		no(e) = 0;
		FALL_THROUGH;
	case val_tag: {
		if (shape_size(sh(e)) > 32) {
			flt64 t;
			int ov;

			if (isbigval(e)) {
				t = flt_to_f64(no(e), 0, &ov);
			} else {
				t.big = (is_signed(sh(e)) && no(e) < 0) ? -1 : 0;
				t.small = no(e);
			}

			oneval(t.big, 32, 1);
			oneval(t.small, 32, 1);
			return;
		}

		if (a.ashalign == 1) {
			evalconcbit(e, bitposn);
		} else {
			oneval(evalexp(e), a.ashalign, 1);
		}
		return;
	}

	case name_tag: {
		dec *globdec = nextg(child(e)) ;	/* must be global name */
		char *name = globdec->name;

		assert(isglob(child(e)));

		if (child(globdec->exp) != NULL &&
		     (child(globdec->exp)->tag == proc_tag ||
		      child(globdec->exp)->tag == general_proc_tag)) {
			/* It's a plabel */
			outs("\t.WORD\tP%");
		} else {
			outs("\t.WORD\t");
		}

		outs(name);
		if (no(e)) {
			outc('+');
			outn(no(e) / 8);
		}
		outnl();
		return;
	}

	case compound_tag: {
		/* Compound values */
		exp off = child(e);
		exp tup = next(off);
		ash tupa;
		concbittype left;
		long last_offset = 0;
		long last_align = 0;
		tupa = ashof(sh(tup));
		left = emptyconcbit(bitposn);

		/* output elements of aggregate recursively */
		for (;;) {
			int gap = no(off) - left.bitposn;

			/* check that component's alignment matches offset in struct */
			/* XXX: what declares ta?
				 assert((no(off)/ta)*ta <= no(off));
			*/
			/* and is no greater than struct's alignment */
			/* XXX: what declares maxalign?
			 assert(tupa.ashalign <= maxalign);
			*/

			if (shape_size(sh(tup)) == 0) {
				if (tup->last) {
					return;
				} else {
					off = next(next(off));
					assert(!off->last);
					tup = next(off);
					tupa = ashof(sh(tup));
					continue;
				}
			}

			if (no(off) < last_offset) {
				error(ERR_SERIOUS,  "Compound components badly ordered" );
			}
			if (last_align <= 1 || tupa.ashalign <= 1 || gap >= tupa.ashalign) {
				/* get gap down */
				while (gap > 0) {
					left = addconcbitaux(0, 1, left);
					gap--;
				}
			} else {
				/* alignment will handle gap */
				left.bitposn = (int) rounder(left.bitposn, tupa.ashalign);
			}

			last_offset = no(off);
			last_align = tupa.ashalign;
			assert(left.bitposn - bitposn == no(off));
			if (tupa.ashalign == 1) {
				/* collect bitfields */
				left = evalconcbitaux(tup, left);
			} else {
				/* output final bits from any previous field */
				outconcbit(left);
				left = emptyconcbit(left.bitposn);
				evalone(tup, left.bitposn);
				left.bitposn += tupa.ashsize;
			}

			if (tup->last) {
				/* output final bits from any previous field */
				long databits = no(off) + tupa.ashsize;
				long trailing_bytes = (a.ashsize - databits) / 8;
				outconcbit(left);
				assert(a.ashsize >= databits);

				/* pad out trailing unitialised space, eg union */
				if (a.ashsize > databits && trailing_bytes > 0) {
					outs( "\t.BLOCKZ\t" );
					outn(trailing_bytes);
					outnl();
				}
				return;
			}

			off = next(next(off));
			assert(!off->last);
			tup = next(off);
			tupa = ashof(sh(tup));
		}

		UNREACHED;
	}

	case nof_tag: {
		exp s = child(e);
		set_align(a.ashalign);

		for (;;) {
			evalone(s, bitposn);
			if (s->last) {
				return;
			}

			s = next(s);
		}
	}

	case ncopies_tag: {
		int n = no(e);
		int i;

		while (child(e)->tag == ncopies_tag) {
			e = child(e);
			n *= no(e);
		}

		e = child(e);

		for (i = 0; i < n; i++) {
			evalone(e, bitposn);
		}
		return;
	}

	case concatnof_tag: {
		asm_comment("concatnof_tag: ashalign=%ld, ashsize=%ld", a.ashalign, a.ashsize);

		/* allow for bitfields */
		if (a.ashalign == 1) {
			evalconcbit(e, bitposn);
		} else {
			ash a;

			a = ashof(sh(child(e)));
			evalone(child(e), bitposn);
			bitposn += a.ashsize;

			a = ashof(sh(next(child(e))));
			if (a.ashalign != 0) {
				bitposn = (bitposn / a.ashalign) * a.ashalign;
			}
			evalone(next(child(e)), bitposn);
		}
		return;
	}

	case clear_tag:
		if (a.ashalign == 1) {
			/* allow for bitfields */
			evalconcbit(e, bitposn);
			return;
		}
		outs("\t.BLOCKZ\t");
		outn((a.ashsize + 7) >> 3);
		outnl();
		return;

	case not_tag:
	case and_tag:
	case or_tag:
	case shl_tag:
	case shr_tag:
	case bitf_to_int_tag:
	case int_to_bitf_tag:
	case env_offset_tag:
	case general_env_offset_tag:
		outs("\t.WORD\t");
		outn(evalexp(e));
		outnl();
		return;

	case env_size_tag: {
		exp tag = child(child(e));
		procrec *pr = &procrecs[no(child(tag))];
		outs("\t.WORD\t");
		outn((pr->frame_sz + 0) >> 3);
		outnl();
		return;
	}

	case offset_add_tag:
		outs("\t.WORD\t");
		outn(evalexp(child(e)) + evalexp(next(child(e))));
		outnl();
		return;

	case offset_max_tag:
		outs("\t.WORD\t");
		outn(MAX(evalexp(child(e)), evalexp(next(child(e)))));
		outnl();
		return;

	case offset_pad_tag:
		outs("\t.WORD\t");
		outn(rounder(evalexp(child(e)), shape_align(sh(e))));
		outnl();
		return;

	case offset_mult_tag:
		outs("\t.WORD\t");
		outn(evalexp(child(e)) * evalexp(next(child(e))));
		outnl();
		return;

	case offset_div_tag:
	case offset_div_by_int_tag:
		outs("\t.WORD\t");
		outn(evalexp(child(e)) / evalexp(next(child(e))));
		outnl();
		return;

	case offset_subtract_tag:
		outs("\t.WORD\t");
		outn(evalexp(child(e)) - evalexp(next(child(e))));
		outnl();
		return;

	case offset_negate_tag:
		outs("\t.WORD\t");
		outn(-evalexp(child(e)));
		outnl();
		return;

	case chvar_tag:
		if ( shape_size ( sh (e) ) == shape_size ( sh ( child (e) ) ) ) {
			sh ( child (e) ) = sh (e);
			evalone ( child (e), bitposn );
		} else {
			error(ERR_SERIOUS,  "Illegal chvar constant" );
		}
		return;

	default:
		error(ERR_SERIOUS, "tag not in evaluated");
	}
}

/*
 * Outputs data initialisers for the evaluated exp.
 * The result is the instore "address" of the constant.
 * A negative l implies that this is the initialisation of a global variable.
 */
instore
evaluated(exp e, long l)
{
	int lab = (l == 0) ? next_data_lab() : (l < 0) ? l : -l;
	int lab0 = lab;
	instore isa;
	exp z = e;
	ash a;
	bool extnamed = (l == 0) ? 0 : main_globals[-lab - 1]->extnamed;
	a = ashof(sh(e));

	asm_comment("evaluated: %d %ld", e->tag, l);

	isa.adval    = 0;
	isa.b.offset = 0;
	isa.b.base   = lab0;

	if (is_zero(e)) {
		int byte_size = (a.ashsize + 7) >> 3;
		int align = ((a.ashalign > 32 || a.ashsize > 32) ? 8 : 4);

		if (!extnamed) {
			/* uninitialised global */
			if (byte_size > 8) {
				insection(bss_section);
			} else {
				insection(shortbss_section);
			}

			outs("\t.ALIGN\t");
			outn(align);
			outnl();
			outs( ext_name(lab) );
			outs("\t.BLOCK\t");
			outn(byte_size);
			outnl();
		} else {
			/* align at least to word for speed of access */
			/* if size greater than 4 bytes, align on double boundry for speed */
			if (a.ashalign > 32 || a.ashsize > 32) {
				set_align(64);
			} else {
				set_align(32);
			}

			if (byte_size > 8) {
				insection(bss_section);
			} else {
				insection(shortbss_section);
			}

			outs( ext_name(lab) );
			outs("\t.COMM\t");
			outn(byte_size);
			outnl();
		}
	} else {
		insection(data_section);

		/* align at least to word for speed of access */
		/* if size greater than 4 bytes, align on double boundry for speed */
		if (a.ashalign > 32 || a.ashsize > 32) {
			set_align(64);
		} else {
			set_align(32);
		}

		outs( ext_name(lab) );
		outnl();
		evalone(z, 0);

		/* evalone does not output .BLOCKZ to finish off up to size, so protect next one */
		if (a.ashalign > 32) {
			set_align(64);
		}
	}

	return isa;
}

