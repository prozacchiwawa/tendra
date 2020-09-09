/*
 * Copyright 2002-2012, The TenDRA Project.
 * Copyright 1997, United Kingdom Secretary of State for Defence.
 *
 * See doc/copyright/ for the full copyright terms.
 */

#include <assert.h>
#include <stddef.h>
#include <string.h>

#include <shared/bool.h>
#include <shared/error.h>
#include <shared/string.h>

#include <tdf/shape.h>
#include <tdf/tag.h>

#include <reader/externs.h>
#include <reader/table_fns.h>
#include <reader/basicread.h>

#include <construct/installglob.h>
#include <construct/exp.h>

#include <special/special_call.h>

#include <main/flags.h>

#include <refactor/const.h>
#include <refactor/optimise.h>
#include <refactor/refactor_ext.h>

/*
 * This function replaces calls to the strcpy function
 * with a direct mem -> mem copy.
 */
static bool
special_strcpy(dec *dp)
{
	exp i = dp->exp;
	exp t;

	assert(!(strcmp)(dp->name, "strcpy"));

	if (writable_strings) {
		return false;
	}

	for (t = pt(i); t != NULL; t = pt(t)) {
		exp src, dst;
		dec *src_dec;

		exp src_def;
		shape sha;

		if (t->last || next(t)->last || !next(next(t))->last ||
			next(next(next(t)))->tag != apply_tag ||
			child(next(next(next(t)))) != t)
		{
			continue;
		}

		dst = next(t);
		src = next(dst);

		if (src->tag != name_tag ||
			!isglob(child(src)) || !isvar(child(src)) || no(child(src)) != 1)
		{
			continue;
		}

		src_dec = nextg(child(src));

		if (src_dec->extnamed || child(src_dec->exp) == NULL) {
			continue;
		}

		src_def = child(child(src));
		sha = sh(src_def);

		if (src_def->tag == string_tag && src_def->props == 8)
		{
			char *s = nostr(src_def);
			size_t l = shape_size(sha);
			size_t j;

			for (j = 0; j < l && s[j] != 0; j++)
				;

			if (j < l) {
				exp q;
				exp to_change = next(src);
				exp idsc = getexp(sh(next(src)), NULL, 0, dst, NULL, 0, 2, ident_tag);
				exp n1 = getexp(sh(dst), NULL, 0, idsc, NULL, 0, 0, name_tag);
				exp n2 = getexp(sh(dst), NULL, 0, idsc, n1, 0, 0, name_tag);
				exp_list el;

				pt(idsc) = n2;

				q = f_assign(n1, f_contents(sha, src));
				el.start = q;
				el.end = q;
				el.number = 1;

				q = f_sequence(el, n2);
				dst->last = false;
				next(dst) = q;
				setfather(idsc, q);
				kill_exp(t, t);
				replace(to_change, idsc, idsc);

				t = i;
			}
		}
	}

	return true;
}

static bool
special_strlen(dec *dp)
{
	exp i = dp->exp;
	exp t;

	assert(!(strcmp)(dp->name, "strlen"));

	if (writable_strings) {
		return false;
	}

	for (t = pt(i); t != NULL; t = pt(t)) {
		exp st;
		dec *src_dec;

		exp st_def;
		shape sha;

		if (t->last || !next(t)->last ||
			next(next(t))->tag != apply_tag ||
			child(next(next(t))) != t)
		{
			continue;
		}

		st = next(t);

		if (st->tag != name_tag || !isglob(child(st)) ||
			!isvar(child(st)) || no(child(st)) != 1)
		{
			continue;
		}

		src_dec = nextg(child(st));

		if (src_dec->extnamed || child(src_dec->exp) == NULL)	{
			continue;
		}

		st_def = child(child(st));
		sha = sh(st_def);

		if (st_def->tag == string_tag && st_def->props == 8)
		{
			char *s = nostr(st_def);
			size_t l = shape_size(sha) / 8;
			size_t j;

			for (j = 0; j < l && s[j] != 0; j++)
				;

			if (j < l) {
				exp to_change = next(st);
				exp res = getexp(sh(to_change), NULL, 0, NULL, NULL, 0, j, val_tag);
				kill_exp(t, t);
				replace(to_change, res, NULL);

				t = i;
			}
		}
	}

	return true;
}

const struct special_call special_calls[] = {
	{ "strcpy", BUILTIN_API, special_strcpy },
	{ "strlen", BUILTIN_API, special_strlen }
};

size_t special_calls_count = sizeof special_calls / sizeof *special_calls;

