/*
 * Copyright 2011, The TenDRA Project.
 * Copyright 1997, United Kingdom Secretary of State for Defence.
 * Copyright 1993, Open Software Foundation, Inc.
 *
 * See doc/copyright/ for the full copyright terms.
 */

/*
 * The main procedure defined here is reg_alloc which allocates registers
 * and stack space for a proc exp. After the application of weights to the
 * body reg_alloc re-codes the number field of each ident within it.
 * Paralloc in paralloc.c does the corresponding work for the parameters.
 *
 * At the end of reg_alloc:
 *
 *  1) props of ident contains inreg_bits or infreg_bits and number = 0
 *     then the value will be in a t reg to be chosen in make_code
 *  2) if props contains the reg bits then number of ident is fixpt s reg
 *     or floatpnt s reg (divided by 2)
 *  3) value is on the stack and:
 *
 * Number of ident = (word displacement in locals)*64 + R_SP
 */

#include <assert.h>

#include <shared/error.h>

#include <local/ash.h>

#include <tdf/tag.h>

#include <construct/installtypes.h>
#include <construct/dec.h>
#include <construct/ash.h>

#include <utility/max.h>

#include <main/print.h>

#include "reg.h"
#include "bits.h"
#include "procrec.h"
#include "regalloc.h"
#include "regable.h"
#include "stack.h"

static spacereq zerospace;

/*
 * Procedure to find the total spacereq of two spacereqs. The bit
 * representations of the s regs used are simply 'or'ed so that the
 * resulting dump fields contain all the regs of the parameters.
 * The largest of the two stack sizes is returned as the stack of the result.
 */
static spacereq
maxspace(spacereq a, spacereq b)
{
	a.fixdump |= b.fixdump;
	a.fltdump |= b.fltdump;
	a.stack = MAX(a.stack, b.stack);
	a.obtain = NULL;

	return a;
}

/*
 * maxspace2 is used by seq tags and ident_tags since the result of
 * these tags could be the result of one of the nexts.
 */
static spacereq
maxspace2(spacereq a, spacereq b)
{
	a.fixdump |= b.fixdump;
	a.fltdump |= b.fltdump;
	a.stack = MAX(a.stack, b.stack);
	a.obtain = b.obtain;

	return a;
}

/*
 * Delivers a spacereq which gives the local stack bit requirement in the
 * stack field and the s regs used as bit positions in the fixdump and
 * fltdump fields for fixed and float regs.
 *
 * e is a proc body.
 * freefixed and freefloat are the number of fixed and floating s regs
 * available. These are initialised at the outer level but may be reduced
 * by usage in paralloc.
 */
spacereq
regalloc(exp e, int freefixed, int freefloat, long stack)
{
	int n = e->tag;
	exp s = child(e);
	spacereq def;

	switch(n) {
	case ident_tag: {
		int ffix = freefixed;
		int ffloat = freefloat;
		long st = stack;
		bool caller_in_postlude = (s->tag == caller_name_tag);
		spacereq body;

		asm_comment("regalloc ident_tag:	vis=%d	freefixed,freefloat,stack = %d %d %ld", isvis(e) != 0, freefixed, freefloat, stack);

		assert(freefixed >= 0);
		assert(freefloat >= 0);

		if (e->props & defer_bit) {
			/* the tag declared is transparent to code production */
			def = zerospace;
		} else if (
		    !isvar(e) && !isparam(e)
		    && s->tag == name_tag
		    && !isvar(child(s))
		    && !isvis(child(s))
		    && !isparam(child(s))
		    && (child(s)->props & inreg_bits)
		) {
			/*
			 * dont take space for this constant dec,
			 * initialiser is another simple constant ident
			 * (eg from double nested loop optimisation)
			 */
			e->props |= defer_bit;
			def = zerospace;
		} else {
			ash a;
			a = ashof(sh(s));

			if (s->tag == compound_tag ||
			    s->tag == nof_tag ||
			    s->tag == concatnof_tag ) {
				/*
				 * elements of tuples are done separately so evaluate above dec
				 * using stack space
				 * stack - bit address for current allocation
				 * st - bit address for next allocation
				 */

				assert((stack & 31) == 0);	/* we expect stack to be word aligned */

				st = ALIGNNEXT(stack, a.ashalign);
				st = ALIGNNEXT(st + a.ashsize, 32);	/* maintain word alignment */

				assert(st - stack >= a.ashsize);
				assert((st & 31) == 0);

				def = regalloc (s, freefixed, freefloat, st);
			} else {
				def = regalloc(s, freefixed, freefloat, stack);
			}

			asm_comment("regalloc ident_tag:	props=%#x fixregable=%d no(e)=%ld ffix=%d", e->props, fixregable(e), no(e), ffix);

			if ((e->props & inreg_bits) == 0 &&
			    fixregable(e) && no(e) < ffix
			    && !caller_in_postlude) {
				/* suitable for s reg, no(e) has been set up by weights */
				e->props |= inreg_bits;
				no(e) = SREG_TO_REALREG(ffix);	/* will be in s reg */
				def.fixdump |= RMASK(no(e));
				ffix--;
				asm_comment("regalloc suitable for s reg:	no(e)=%ld", no(e));
				assert(ffix >= 0);
				assert(IS_SREG(no(e)));
				assert(a.ashsize <= 32);
			} else if ((e->props & infreg_bits) == 0 &&
			           floatregable(e) && no(e) < ffloat
			           && !caller_in_postlude) {
				/* suitable for float s reg , no(e) has been set up by weights */
				e->props |= infreg_bits;
				no(e) = SFREG_TO_REALFREG(ffloat);	/* will be in s reg */
				def.fltdump |= RMASK(no(e));
				ffloat--;
				asm_comment("regalloc suitable for s freg:	no(e)=%ld", no(e));
				assert(ffloat >= 0);
				assert(IS_FLT_SREG(no(e)));
				assert(a.ashsize <= 64);
			} else if ((e->props & inanyreg) == 0) {
				/*
				 * not suitable for reg allocation
				 */
				if (child(e)->tag == val_tag && !isvar(e) && !isenvoff(e)) {
					/*
					 * must have been forced by const optimisation
					 * - replace uses by the value
					 */
					exp t = pt(e);

					for (; t != NULL;) {
						exp p = pt(t);

						t->tag = val_tag;
						child(t) = NULL;
						no(t) = no(child(e));
						t->props = 0;
						pt(t) = NULL;
						t = p;
					}
					pt(e) = NULL;

					asm_comment("regalloc heavily used const: no spare regs - replace use by value");
					e->props |= defer_bit;
					def = zerospace;
				} else if (child(e)->tag == name_tag && !isvar(e) && !isenvoff(e)) {
					/* must have been forced  - defer it */
					asm_comment("regalloc heavily used address: no spare regs - replace use by value");
					e->props |= defer_bit;
					def = zerospace;
				} else if (isparam(e) || caller_in_postlude) {
					/*
					 * Caller parameters and callee parameters are
					 * calculated in make_ident_tag_code
					 * Caller parameters identified in postludes are
					 * also done in make_ident_tag_code
					 *
					 * It is essential that caller parameters identified
					 * in postludes are not allocated into s-regs
					 */
					no(e) = 0;
				} else {
					/*
					 * allocate on stack
					 * stack - bit address for current allocation
					 * st - bit address for next allocation
					 */

					assert((stack & 31) == 0);	/* we expect stack to be word aligned */

					stack = ALIGNNEXT(stack, a.ashalign);
					st = ALIGNNEXT(stack + a.ashsize, 32);	/* maintain word alignment */

					assert(st - stack >= a.ashsize);
					assert((stack & 31) == 0);

					def.stack = MAX(def.stack, st);
					no(e) = (stack << 3) + R_FP;		/* no() decoded by boff() */
					assert((stack & 7) == 0);			/* must be byte aligned */
					asm_comment("regalloc allocate on stack:	stack,st=%ld,%ld	no(e)=%ld", stack, st, no(e));
				}
			} else {
				asm_comment("regalloc no(e)==%ld:/* allocation of stack like regs in make_code */", no(e));
			}
		}
		body = regalloc(next(s), ffix, ffloat, st);

		asm_comment("regalloc return:	ffix,ffloat,st = %d %d %ld", ffix, ffloat, st);
		return maxspace2(def, body);
	}

	case case_tag:
		/* We do not wish to recurse down the next(child(e)) */
		def = regalloc(s, freefixed, freefloat, stack);
		def.obtain = NULL;/* A case returns nothing */
		return def;

	case cont_tag:
		if (s->tag == name_tag &&
		    child(s)->tag == ident_tag &&
		    isvar(child(s)) &&
		    (
		        (((child(s)->props & inreg_bits) != 0) && IS_SREG(no(child(s))))  ||
		        (((child(s)->props & infreg_bits) != 0) && IS_FLT_SREG(no(child(s))))
		    )
		   ) {
			def = zerospace;
			def.stack = stack;
			def.obtain = child(s);
			return def;
		} else {
			goto label_default;
		}

	case name_tag:
		def = zerospace;
		def.stack = stack;

		if (s->tag == ident_tag &&
		    !isvar(s) &&
		    (
		        (((s->props & inreg_bits) != 0) && IS_SREG(no(s))) ||
		        (((s->props & infreg_bits) != 0) && IS_FLT_SREG(no(s)))
		    )
		  ) {
			/* This could be the last one */
			def.obtain = s;
		}

		return def;

	case env_offset_tag:
	case general_env_offset_tag:
	case caller_name_tag:
		/* We do not wish to recurse down these tags */
		def = zerospace;
		def.stack = stack;
		def.obtain = NULL;
		return def;

	case seq_tag:
		def = regalloc(s, freefixed, freefloat, stack);
		s = next(s);
		def = maxspace2(def, regalloc(s, freefixed, freefloat, stack));
		return def;

label_default:

	default:
		if (s == NULL) {
			def = zerospace;
			def.stack = stack;
			def.obtain = NULL;
			return def;
		} else {
			def = regalloc(s, freefixed, freefloat, stack);
			if (def.obtain == s) {
				if ((def.obtain->props & inreg_bits) != 0) {
					freefixed--;
				} else {
					freefloat--;
				}
			}

			while (!s->last) {
				s = next(s);
				def = maxspace(def, regalloc(s, freefixed, freefloat, stack));
				if (def.obtain == s) {
					if ((def.obtain->props & inreg_bits) != 0) {
						freefixed--;
					} else {
						freefloat--;
					}
				}
			}

			return def;
		}
	}
}
