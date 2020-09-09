/*
 * Copyright 2011, The TenDRA Project.
 * Copyright 1997, United Kingdom Secretary of State for Defence.
 *
 * See doc/copyright/ for the full copyright terms.
 */

/*
 * The main procedure defined here is reg_alloc which allocates
 * registers and stack space for a procedure expression.  After the
 * application of weights to the body reg_alloc re-codes the number
 * field of each ident within it.  At the end of reg_alloc :
 *
 * 1) If the props field of an ident contains inreg_bits or
 *    infreg_bits and number = 0 then the value will be in a
 *    t-register to be chosen in make_code,
 *
 * 2) If the props contains the register bits then number of
 *    the ident is a fixed point s-register number or a floating
 *    point s-register number (divided by 2).
 *
 * 3) Otherwise the value is on the stack and :
 *    number of ident = (word displacement in locals) * 64 + R_FP.
 */

#include <assert.h>

#include <shared/bool.h>
#include <shared/check.h>
#include <shared/error.h>

#include <local/ash.h>

#include <tdf/tag.h>

#include <construct/installtypes.h>
#include <construct/dec.h>
#include <construct/ash.h>

#include <utility/max.h>

#include <main/flags.h>

#include "procrec.h"
#include "bits.h"
#include "regable.h"
#include "reg.h"
#include "proc.h"
#include "translate.h"
#include "regalloc.h"

/*
 * Flag: Avoid register L7 (used in PIC)
 */
bool avoid_L7 = 0;

/*
 * Map fixreg number to real register number
 *
 * The number n in the range 1...14 is mapped onto the corresponding
 * register out of R_L0,...,R_L7,R_I5,...,R_I0.
 */

/*
 * We will see the parameters first, so won't call this until
 * the remaining Ix's are available.
 */
static int
SREG_TO_REALREG(int n)
{
	int m = avoid_L7 ? 7 : 8;
	int par_reg_max;

	if (n <= m) {
		return R_L0 + (n - 1);
	}

	if ((gencompat && May_have_callees) || (!gencompat && in_general_proc)) {
		if(Has_vcallees) {
			par_reg_max = R_I3;
		} else {
			par_reg_max = R_I4;
		}
	} else {
		par_reg_max = R_I5;
	}

	return par_reg_max - (n - m - 1);
	/* return Has_vcallees ? R_I4 - (n - m - 1) : R_I5 - (n - m - 1); */
}

/*
 * Round b up to a multiple of a
 */
#define ALIGNNEXT(b, a) (((b) + (a) - 1) & ~((a) - 1))

static spacereq zerospace;

/*
 * Find the maximum of two spacereqs.
 * The register masks are just or'ed, the stack size is the larger of the two.
 */
static spacereq
maxspace(spacereq a, spacereq b)
{
	a.fixdump |= b.fixdump;
	a.fltdump |= b.fltdump;
	a.stack = MAX(a.stack, b.stack);

	return a;
}

/*
 * Calculate space requirements for a procedure
 *
 * The expression e gives the procedure body, freefixed and freefloat
 * are respectively the numbers to fixed and floating s-registers.
 * The initial stack size is given by stack.
 */
spacereq
regalloc(exp e, int freefixed, int freefloat, long stack)
{
	spacereq def;
	exp s = child(e);
	unsigned char n = e->tag;

	if (n == ident_tag) {
		int ffix = freefixed;
		int ffloat = freefloat;
		long st = stack;
		spacereq body;

		ash a;
		assert(freefixed >= 0);
		assert(freefloat >= 0);

		if (e->props & defer_bit) {
			/* the tag declared is transparent to code production */
			def = zerospace;
		} else {
			a = ashof(sh(s));
#if 1
			if (s->tag != compound_tag && s->tag != nof_tag &&
			   s->tag != concatnof_tag) {
				def = regalloc(s, freefixed, freefloat, stack);
			} else {
				if (a.ashalign <= 32 || (stack & 0x20) == 0) {
					st = stack + ((a.ashsize + 31) & ~31);
				} else {
					st = stack + 32 + ((a.ashsize + 31) & ~31);
				}
				def = regalloc(s, freefixed, freefloat, st);
			}
#else
			def = regalloc(s, freefixed, freefloat, stack);
#endif
			if (isparam(e)) {
				/* (some) SPARC params in s-regs, reserve them here */
				int n2 = (int) child(e)->props;
				int start = no(child(e)) >> 5;
				if (start <= 5) {
					/* Some input registers (%i0 .. %i5) are used */
					int nregs;
					/* "end" Word offset beyond end of param */
					long sz = shape_size(sh(child(e)));
					int end = (int) (start + (rounder(sz, 32) >> 5));
					if (end > 6) {
						end = 6;
					}
					nregs = end - start;
					/* assert((n2 - R_I0) == start); */
					assert(nregs <= 6);
					ffix -= nregs ; /* this also prevents SREG_TO_REALREG
			     from using these regs...*/
				}
				if ((e->props & inreg_bits) != 0) {
					assert(no(e) == n2);
					assert(IS_SREG(no(e)));
					assert(a.ashsize <= 32);
					assert(R_I0 <= n2 && n2 <= R_I5);
					def.fixdump |= RMASK(n2);
				} else {
					no(e) = 0;
				}
				assert(ffix >= 0);
			} else if ((e->props & inreg_bits) == 0 && fixregable(e) && no(e) < ffix) {
				/* suitable for s reg, no(e) has been set up by weights */
				pset(e, inreg_bits);
				no(e) = SREG_TO_REALREG(ffix); /* will be in s reg */
				ffix -= 1;
				def.fixdump |= RMASK(no(e)); /* was ffix */
				assert(ffix >= 0);
				assert(IS_SREG(no(e)));
				assert(a.ashsize <= 32);
			} else if ((e->props & infreg_bits) == 0 && floatregable(e) && no(e) < ffloat) {
				error(ERR_SERIOUS,  "regalloc : no float point s regs on SPARC" );
			} else if ((e->props & inanyreg) == 0) {
				if (child(e)->tag == val_tag && !isvar(e) && !isenvoff(e)) {
					/* must have been forced by const optimisation -
					   replace uses by the value */
					exp t = pt(e);
					for ( ; t != NULL ; ) {
						exp p = pt(t);
						t->tag = val_tag;
						child(t) = NULL;
						no(t) = no(child(e));
						t->props = 0;
						pt(t) = NULL;
						t = p;
					}
					pt(e) = NULL;
					pset(e, defer_bit);
					def = zerospace;
				} else if (child(e)->tag == name_tag && !isvar(e) & !isenvoff(e)) {
					/* must have been forced - defer it */
					pset(e, defer_bit);
					def = zerospace;
				} else {	/* all envoffset MUST go on the stack */
					/* allocate on stack */
					/*int basereg = (Has_vcallees)?local_reg:R_FP;*/
					int basereg = R_FP;

					assert((stack & 31) == 0);
					stack = ALIGNNEXT(stack, a.ashalign);
					st    = ALIGNNEXT(stack + a.ashsize, 32);
					assert(st - stack >= a.ashsize);
					assert((stack & 31) == 0);
					def.stack = MAX(def.stack, st);
					no(e) = (int) (stack * 2 + basereg);
				}
			} else if (no(e) == R_USE_RES_REG) {
				/* use result register */
				assert(!isenvoff(e));
				no(e) = ((e->props & inreg_bits) != 0) ?  R_O0 : R_DEFER_F0;
			} else {
				/* allocation of stack like regs in make_code */
				assert (!isenvoff(e));
			}
		}
		body = regalloc(next(s), ffix, ffloat, st);
		return maxspace(body, def);
	} else if (n == case_tag) {
		/* recurse on all expressions in tree */
		return regalloc(s, freefixed, freefloat, stack);
	} else if (n != name_tag && n != env_offset_tag && n != general_env_offset_tag && s != NULL) {
		/* recurse on all expressions in tree */
		def = regalloc(s, freefixed, freefloat, stack);
		while (!s->last) {
			spacereq sdef;
			s = next(s);
			sdef = regalloc(s, freefixed, freefloat, stack);
			def = maxspace(def, sdef);
		}
		return def;
	} else {
		def = zerospace;
		def.stack = stack;
		return def;
	}

	UNREACHED;
}

