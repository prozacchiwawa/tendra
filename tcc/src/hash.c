/*
 * Copyright (c) 2002-2005 The TenDRA Project <http://www.tendra.org/>.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of The TenDRA Project nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific, prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS
 * IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * $Id$
 */
/*
		 Crown Copyright (c) 1997

    This TenDRA(r) Computer Program is subject to Copyright
    owned by the United Kingdom Secretary of State for Defence
    acting through the Defence Evaluation and Research Agency
    (DERA).  It is made available to Recipients with a
    royalty-free licence for its use, reproduction, transfer
    to other parties and amendment for any purpose not excluding
    product development provided that any such use et cetera
    shall be deemed to be acceptance of the following conditions:-

        (1) Its Recipients shall ensure that this Notice is
        reproduced upon any copies or amended versions of it;

        (2) Any amended version of it shall be clearly marked to
        show both the nature of and the organisation responsible
        for the relevant amendment or amendments;

        (3) Its onward transfer from a recipient to another
        party shall be deemed to be that party's acceptance of
        these conditions;

        (4) DERA gives no warranty or assurance as to its
        quality or suitability for any purpose and DERA accepts
        no liability whatsoever in relation to any use to which
        it may be put.
*/


#include <sys/stat.h>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>

#include "error.h"
#include "xalloc.h"

#include "utility.h"
#include "environ.h"
#include "flags.h"
#include "hash.h"
#include "options.h"


struct hash {
	struct hash *next;
	const char *value;
	const char *name;
	unsigned int flag;
	const char *file;
	int  line_num;
	enum hash_precedence precedence;
};


/*
 * Reconcile the table of user-defined env options. At present this function
 * just makes sure that non-tccenv(5) variables declared by the user were used
 * in the env files. If not, it's likely a subtle bug or typo, and a warning
 * issues if the version -v switch is used.
 */
void
reconcile_envopts(const struct hash *h)
{
	const struct hash *n;

	/*
	 * If no -Y args were given whatsoever, give a warning even without
	 * verbose being set.
	 */
	if (environ_count == 0)
		error(ERROR_USAGE, "not invoked with any -Y env arguments");

	/*
	 * If the global env table is NULL, no -Y args succeeded, or none were
	 * given.
	 */
	/* TODO: this needs reworking, now; it cannot happen
	if (environ_hashtable == NULL)
		error(ERROR_FATAL, "failed to load any environment files");
	*/

	if (!verbose) {
		return;
	}

	for (n = h; n != NULL; n = n->next) {
		if ((h->flag & HASH_USR) && !(h->flag & HASH_READ)) {
			error(ERROR_WARNING,
			    "%s, line %d: environment option %s declared"
			    " but never used", h->file, h->line_num, h->name);
		}

		/*
		 * Additional error checking for those platforms supporting stat() for
		 * variables which represent paths on the filesystem.
		 */
		if (0 == strncmp(n->name, "PREFIX_", 7) && strlen(n->value) > 0) {
			struct stat sb;

			if (stat(n->value, &sb) == -1) {
				error(ERROR_SERIOUS, "%s: %s", n->value, strerror(errno));
				return;
			}

			if (!S_ISDIR(sb.st_mode)) {
				error(ERROR_SERIOUS, "%s expected to be a directory");
				return;
			}
		}
	}
}

/*
 * PRINT OUT AN ENVIRONMENT
 */
void
dump_env(const struct hash *h)
{
	const struct hash *n;

	assert(h != NULL);

	IGNORE printf("Environment dump:\n");

	for (n = h; n != NULL; n = n->next) {
		IGNORE printf("\t%c%-18.*s = %s\n",
			(~n->flag & HASH_USR)  ? '$' : ' ',
			(int) strcspn(n->name, "$"), n->name, n->value);
	}
}

/*
 * SET A VARIABLE
 */
void
envvar_set(struct hash **h, const char *name, const char *value,
	enum hash_order order, enum hash_precedence precedence)
{
	struct hash *n;

	assert(h != NULL);
	assert(name != NULL);
	assert(value != NULL);

	for (n = *h; n != NULL; n = n->next) {
		if (0 == strcmp(name, n->name)) {
			break;
		}
	}

	/* Case 1.  Node was not found; push */
	if (n == NULL) {
		n = xmalloc(sizeof *n);

		n->flag       = option_istccopt(name) ? 0 : HASH_USR;
		n->name       = string_copy(name);
		n->value      = string_copy(value);
		n->file       = NULL;	/* TODO */
		n->line_num   = -1;	/* TODO */
		n->precedence = precedence;

		n->next = *h;
		*h = n;

		return;
	}

	assert(n->value != NULL);

	/* Decline to update if we're lower precedence than the existing value */
	if (precedence < n->precedence) {
		return;
	}

	n->precedence = precedence;

	/* Case 2.  Update with a value */
	switch (order) {
	case HASH_ASSIGN:
		n->value = value;
		break;

	case HASH_APPEND:
		n->value = string_append(n->value, value, ' ');
		break;

	case HASH_PREPEND:
		n->value = string_append(value, n->value, ' ');
		break;

	default:
		error(ERROR_FATAL, "Attempt to update hash with invalid order %d\n",
			(int) order);
	}
}

/*
 * GET THE FLAGS FOR A VARIABLE
 */
unsigned int
envvar_flags(const struct hash *h, const char *name)
{
	const struct hash *n;

	assert(name != NULL);

	for (n = h; n != NULL; n = n->next) {
		if (0 == strcmp(name, n->name)) {
			return n->flag;
		}
	}

	return 0;
}

/*
 * GET A VARIABLE
 *
 * Lookup value for tccenv(5) variables. This function takes in an escape
 * sequence from an env file, and attempts to find a definition. For example,
 * <PREFIX> may be passed in, and be mapped to the value supplied previously
 * by -y arguments.
 *
 * This function performs all error handling; it will return a valid char *,
 * or fail.
 */
const char *
envvar_dereference(struct hash *h, const char *name, char *end,
	const char *nm, int line_num)
{
	struct hash *n;
	char tmp;

	/* temporarily replace '>' with '\0' to facilitate lookup */
	/* TODO: modify the key_match function instead */
	/* TODO: or do this in the caller */
	tmp = *end;
	*end = '\0';

	for (n = h; n != NULL; n = n->next) {
		if (0 == strcmp(name, n->name)) {
			n->flag |= HASH_READ;
			break;
		}
	}

	*end = tmp;

	if (n == NULL) {
		/* TODO: %.*s based on *end */
		error(ERROR_FATAL, "Undefined variable <%s> in %s line %d",
			  name, nm, line_num);
	}

	assert(n->value != NULL);

	return n->value;
}
