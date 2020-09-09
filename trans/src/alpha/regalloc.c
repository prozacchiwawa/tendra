/*
 * Copyright 2011, The TenDRA Project.
 * Copyright 1997, United Kingdom Secretary of State for Defence.
 *
 * See doc/copyright/ for the full copyright terms.
 */

/*
 * functions to allocate register and stack space for a proc.
 * The following changes are made to the ident tags:
 *
 * ident->props contains inreg_bits or infreg_bits, no(ident)==0
 * => value is a t reg(chosen in make_code()).
 * ident->props contains reg bits =>  no(ident) is sreg.
 * ident->props is instore => no = displacement*64+SP.
 */

#include <stddef.h>

#include <shared/bool.h>

#include <local/ash.h>

#include <tdf/tag.h>

#include <flpt/flpt.h>

#include <construct/installtypes.h>
#include <construct/dec.h>
#include <construct/ash.h>

#include <utility/max.h>

#include "procrec.h"
#include "bits.h"
#include "regable.h"
#include "reg.h"
#include "frames.h"
#include "reg_defs.h"
#include "make_code.h"
#include "regalloc.h"

static spacereq zerospace;

static spacereq
maxspace(spacereq a, spacereq b)
{
	a.fixdump |= b.fixdump;
	a.fltdump |= b.fltdump;
	a.stack = MAX(a.stack, b.stack);

	return a;
}

static int spareparregs = 0;

/*
 * Delivers a spacereq which gives the local stack bit
 * requirement in the stack field and the s regs used as bit
 * positions in the fixdump, sdump and ddump fields for fixed
 * point, single and double floats respectively.
 *
 * e is a proc body . freefixed and freefloat are the number of
 * fixed and floating s regs available. These are initialised at
 * the outer level but may be reduced by usage in paralloc
 */
spacereq
regalloc(exp e, int freefixed, int freefloat, int stack)
{
	int n = e->tag;
	exp s = child(e);
	spacereq def;

	if (n == ident_tag) {
		int ffix   = freefixed;
		int ffloat = freefloat;
		int st = stack;
		int old_spareparregs = spareparregs;
		spacereq body;
		ash a;

		if (e->props & defer_bit) {
			/* the tag declared is transparent to code production */
			def = zerospace;
		} else {
			a = ashof(sh(s));

			if (s->tag != compound_tag && s->tag != nof_tag
			    && s->tag != concatnof_tag ) {
				def = regalloc(s, freefixed, freefloat, stack);
			} else  {
				/*elements of tuples are done separately so evaluate above dec*/
				if (a.ashalign <= 64 || (stack & 0x40) == 0) {
					st = stack + ((a.ashsize + 63) & ~63);
				} else {
					st = stack + 64 + ((a.ashsize + 63) & ~63);
				}
				def = regalloc(s, freefixed, freefloat, st);
			}

			if ((e->props & inreg_bits) == 0 && fixregable(e) && no(e) < ffix) {
				/* suitable for s reg , no(e) has been set
				   up by weights */
				e->props |= (inreg_bits | sreguse);
				no(e) = ffix + 8; /* will be in s reg , note s0 = $9 */
				def.fixdump |= (1 << (no(e) - FIRST_S_REG)/*ffix*/);
				ffix -= 1;
				/* def.fixdump |= (1 << no(e)); */
			} else if ((e->props & infreg_bits) == 0
			           && floatregable(e) && no(e) < ffloat) {
				/* suitable for float s reg , no(e) has been set up by weights */
				e->props |= (infreg_bits | sreguse);
				no(e) = ffloat + 1;	/* will be in s reg,note start from $f9*/
				def.fltdump |= (1 << ffloat);
				ffloat -= 1;
				/* have another look at this */
			} else if ((e->props & inanyreg) == 0) {
				if (fixregable(e) && PossParReg(e) && spareparregs > 0) {
					e->props |= inreg_bits;
					no(e) = NO_REG;
					spareparregs--;
				}
				/* not suitable for reg allocation */
				else if (child(e)->tag == val_tag && !isvar(e) && !isvis(e)) {
					exp t = pt(e);
					for ( ; t != NULL;) {
						exp p = pt(t);
						t->tag = val_tag;
						child(t) = NULL;
						no(t) = no(child(e));
						t->props = 0;
						pt(t) = NULL;
						t = p;
					}
					pt(e) = NULL;
					e->props |= defer_bit;
					def = zerospace;
				} else if (child(e)->tag == name_tag && !isvar(e) && !isvis(e)) {
					/* must have been forced  - defer it */
					e->props |= defer_bit;
					def = zerospace;
				} else if (isparam(e)) {
					if (child(e)->props != 0) {
						spareparregs++;
					}
					no(e) = 0;
					/* don't know framesize yet;
					 displacement in no(child(e)) */
				} else {		/* allocate on stack */
					int basereg = Has_vcallees ? local_reg : (Has_fp ? FP : SP);
					if (a.ashalign <= 64 || (stack & 0x40) == 0) {
						st = stack + ((a.ashsize + 63) & ~63);
					} else {
						stack += 64;
						st = stack + ((a.ashsize + 63) & ~63);
					}
					def.stack = MAX(def.stack, st);
					no(e) = stack * 2 + basereg;
				}
			} else if (no(e) == 101) {
				/*
				 * Use the result register (the result register has the
				 * same number for both floating and fixed point results.
				 */
				no(e) = RESULT_REG;
			}
			/* else  allocation of stack like regs in make_code */
		}

		body = regalloc(next(s), ffix, ffloat, st);
		spareparregs = old_spareparregs;
		return maxspace (body, def);
	} else if (n == case_tag) {
		/* recurse on all expressions in tree */
		return regalloc (s, freefixed, freefloat, stack);
	} else if (n != name_tag && n != env_offset_tag && n != general_env_offset_tag && s != NULL) {
		def = regalloc(s, freefixed, freefloat, stack);
		while (!s->last) {
			s = next(s);
			def = maxspace(def, regalloc(s, freefixed, freefloat, stack));
		}
		return def;
	} else {
		def = zerospace;
		def.stack = stack;
		return def;
	}
}

