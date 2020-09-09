/*
 * Copyright 2011, The TenDRA Project.
 * Copyright 1997, United Kingdom Secretary of State for Defence.
 *
 * See doc/copyright/ for the full copyright terms.
 */

#include <stddef.h>

#include <shared/bool.h>

#include <tdf/shape.h>
#include <tdf/tag.h>

#include <construct/exp.h>
#include <construct/is_worth.h>

bool
is_worth(exp c)
{
	unsigned char cnam = c->tag;
	bool isflt = is_floating(sh(c)->tag);

	if (sh(c)->tag == ptrhd && al1(sh(c)) == 1 ) {
		return 0;    /* ptr bits */
	}

	if (cnam == real_tag) {
		return true;
	}

	if (cnam == cont_tag && isflt && (child(c)->tag != name_tag || isglob(child(child(c))) )) {
		return true;
	}

	if (cnam == cont_tag && child(c)->tag == name_tag && isglob (child (child (c)))) {
		return true;
	}

	if (cnam == val_tag) {
		/* it is sometimes worthwhile extracting big constants from loops ... */
		long  n = no(c);
		exp dad;

		if (n == 0 || next(c) == NULL) {
			return false;
		}

		dad = father(c);
		if (dad == NULL) {
			return false;
		}

		switch (dad->tag) {
		case and_tag: {
			exp grandad = father (dad);
			if ( grandad != NULL &&
			     grandad->tag == test_tag && (n & (n - 1)) == 0 &&
			     (grandad->props == 5 || grandad->props == 6) &&
			     ((next (child (grandad))->tag == val_tag && no (next (child (grandad))) == 0)
			      || (child (grandad)->tag == val_tag && no (child (grandad)) == 0))
			   ) {
				/*  a & 2^n == 0 is transformed later to
						   shift and test negative */
				return 0;
			}
			/* else next case */
		}

		case or_tag:
		case xor_tag:
			return n < 0 || n >= 0xffff;/* short literal operands */

		case test_tag:
			return 1;

		case mult_tag:
		case offset_mult_tag:
			if (n <= 0x7fff && n > -0x8000) {
				return 0;    /* short literal operands */
			}

			/* a*2^n and a*2^(n+-1) are transformed later to shifts and adds */
			return (n & (n - 1)) != 0 && (n & (n + 1)) != 0 && ((n - 1) & (n - 2)) != 0;

		case div1_tag:
		case div2_tag:
		case rem2_tag:
			if (n <= 0x7fff && n > -0x8000) {
				return 0 /* short literal operands */ ;
			}

			/* a/2^n transformed later to shift */
			return (n & (n - 1)) != 0;

		default:
			return n > 0x7fff || n < -0x8000 /* short literal operands */ ;
		}
	}

	return (!is_o (cnam) && cnam != clear_tag) ||
	        /* ignore simple things unless ... */
	        (cnam == cont_tag && child(c)->tag == cont_tag &&
	         child(child(c))->tag == name_tag);
}

