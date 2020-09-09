/*
 * Copyright 2011, The TenDRA Project.
 * Copyright 1997, United Kingdom Secretary of State for Defence.
 *
 * See doc/copyright/ for the full copyright terms.
 */

/*
 * inlines some of the simpler standard library functions
 */

#include <string.h>

#include <shared/bool.h>
#include <shared/check.h>
#include <shared/error.h>
#include <shared/string.h>

#include <local/ash.h>

#include <tdf/shape.h>
#include <tdf/tag.h>

#include <reader/exp.h>
#include <reader/basicread.h>

#include <construct/installtypes.h>
#include <construct/dec.h>
#include <construct/ash.h>

#include "addr.h"
#include "move.h"
#include "pseudo.h"
#include "getregs.h"
#include "guard.h"
#include "code_here.h"
#include "inst_fmt.h"
#include "mips_ins.h"
#include "labels.h"
#include "procrec.h"
#include "bits.h"
#include "special.h"

/* these are the procs I could do something about */
static int
specno(char *n)
{
	return 0;
}

/* these are the needs for their expansion */
needs
specialneeds(int i)
{
	switch (i) {
	default:
		error(ERR_INTERNAL, "not special fn");
	}

	UNREACHED;
}

/* could I treat this function load specially ? */
int
specialfn(exp fn)
{
	if (fn->tag == name_tag && child(fn)->tag == ident_tag &&
	    isglob (child (fn)) && child (child (fn)) == NULL) {
		char *extname = nextg(child (fn)) -> name;
		return specno (extname);
	}

	return 0;
}

int
specialmake(int n, exp par, space sp, where dest, int exitlab)
{
	switch (n) {
	default:
		error(ERR_INTERNAL, "not special");
	}

	return exitlab;
}

