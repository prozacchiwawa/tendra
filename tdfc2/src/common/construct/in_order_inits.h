/*
 * Copyright 2002-2011, The TenDRA Project.
 * Copyright 1997, United Kingdom Secretary of State for Defence.
 *
 * See doc/copyright/ for the full copyright terms.
 */


#ifndef IN_ORDER_INITS_INCLUDED
#define IN_ORDER_INITS_INCLUDED

/*
    Represents a stack based data structure that can be used as a work area
		for processing hierarchial information.

		Contains push, reduce and replace operations.

		Allows random access in a nicer fasion than LIST(EXP).
*/
typedef struct _InitialisersInOrder_t {
	int iio_cap, iio_cur, iio_len;
	EXP *iio_refs;
} InitialisersInOrder_t;

/*
	  Copy one expression into the vector.
*/
void
iio_realloc_copy_in(InitialisersInOrder_t *designated_inits, EXP e);

/*
	  Set the cursor to a new location.
*/
void
iio_seek(InitialisersInOrder_t *designated_inits, int target);

/*
	  Replace an expression in the vector.
*/
void
iio_replace(InitialisersInOrder_t *designated_inits, int target, EXP e);

/*
	  Given iio_len > target, construct a list of those expressions into r,
		ending with iio_len = target.
*/
void
iio_reduce(InitialisersInOrder_t *designated_inits, int target, LIST(EXP) *r);

/*
	  Ensure that resources used are freed.
*/
void
iio_free(InitialisersInOrder_t *designated_inits);


#endif /* IN_ORDER_INITS */
