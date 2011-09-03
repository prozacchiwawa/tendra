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


#include "config.h"
#include "types.h"
#include "alignment.h"
#include "check.h"
#include "node.h"
#include "shape.h"
#include "table.h"
#include "tdf.h"
#include "utility.h"


/*
    BASIC ALIGNMENTS

    These are the basic alignments from the TDF specification.
*/

node *al_code = NULL;
node *al_frame =  NULL;
node *al_alloca = NULL;
node *al_var_param = NULL;
node *al_top =  NULL;
static node *al_offset = NULL;
static node *al_pointer = NULL;
static node *al_proc = NULL;


/*
    FIND THE ALIGNMENT OF A SHAPE

    For the node p, representing a shape or an alignment, this returns
    the alignment of p.
*/

node *
al_shape(node *p)
{
	node *q;
	sortname s;

	if (p == NULL)
		return NULL;

	s = p->cons->sortnum;

	if (s == SORT_alignment) {
		switch (p->cons->encoding) {
		case ENC_alignment:
			return al_shape(p->son);
		case ENC_alignment_apply_token:
			return al_shape(expand_tok(p));
		}
		return copy_node(p);
	}

	if (s == SORT_shape) {
		switch (p->cons->encoding) {
		case ENC_bottom:
			is_fatal = 0;
			input_error("Can't have alignment of bottom in %s",
			    checking);
			return NULL;
		case ENC_offset:
			return copy_node(al_offset);
		case ENC_pointer:
			return copy_node(al_pointer);
		case ENC_proc:
			return copy_node(al_proc);
		case ENC_top:
			return copy_node(al_top);
		case ENC_nof:
			return al_shape(p->son->bro);
		case ENC_shape_apply_token:
			return al_shape(expand_tok(p));
		}
	}

	q = new_node();
	q->cons = cons_no(SORT_alignment, ENC_alignment);
	q->son = copy_node(p);

	return NULL;
}


/*
    FIND WHAT A POINTER POINTS TO

    For the node p of the form ( pointer a ) this routine returns a.
*/

node *
ptr_to(node *p)
{
	p = expand_tok(p);

	if (p && p->cons == cons_no(SORT_shape, ENC_pointer))
		return p->son;

	return NULL;
}


/*
    FIND THE FIRST COMPONENT OF AN OFFSET

    For the node p of the form ( offset a b ) this routine returns a.
*/

node *
offset_from(node *p)
{
	p = expand_tok(p);

	if (p && p->cons == cons_no(SORT_shape, ENC_offset))
		return p->son;

	return NULL;
}


/*
    FIND THE SECOND COMPONENT OF AN OFFSET

    For the node p of the form ( offset a b ) this routine returns b.
*/

node *
offset_to(node *p)
{
	p = expand_tok(p);

	if (p && p->cons == cons_no(SORT_shape, ENC_offset))
		return p->son->bro;

	return NULL;
}


/*
    CHECK THAT TWO ALIGNMENTS ARE EQUAL

    This routine is not yet implemented.
*/

void
al_equals(node *p, node *q)
{
	(void) p;
	(void) q;
}


/*
    CHECK THAT ONE ALIGNMENT CONTAINS ANOTHER

    This routine is not yet implemented.
*/

void
al_includes(node *p, node *q)
{
	(void) p;
	(void) q;
}


/*
    FIND THE UNIONS OF TWO ALIGNMENTS

    The value of ( unite_alignments p q ) is returned.
*/

node *
al_union(node *p, node *q)
{
	if (p == NULL || p->cons->sortnum != SORT_alignment)
		return NULL;

	if (q == NULL || q->cons->sortnum != SORT_alignment)
		return NULL;

	if (p->cons->encoding == ENC_alignment &&
	    p->son->cons == cons_no(SORT_shape, ENC_top))
		return q;

	if (q->cons->encoding == ENC_alignment &&
	    q->son->cons == cons_no(SORT_shape, ENC_top))
		return p;

	return NULL;
}


/*
    INITIALIZE BASIC ALIGNMENTS

    The basic alignments, al_top, al_offset, al_pointer and al_proc, are
    initialized.
*/

void
init_alignments(void)
{
	/* Set up al_top */
	al_top =  new_node();
	al_top->cons = cons_no(SORT_alignment, ENC_alignment);
	al_top->son = copy_node(sh_top);

	/* Set up al_offset */
	al_offset =  new_node();
	al_offset->cons = cons_no(SORT_alignment, ENC_alignment);
	al_offset->son = sh_offset(al_top, al_top);

	/* Set up al_pointer */
	al_pointer =  new_node();
	al_pointer->cons = cons_no(SORT_alignment, ENC_alignment);
	al_pointer->son = sh_pointer(al_top);

	/* Set up al_proc */
	al_proc =  new_node();
	al_proc->cons = cons_no(SORT_alignment, ENC_alignment);
	al_proc->son = copy_node(sh_proc);
}