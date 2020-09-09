/*
 * Copyright 2002-2011, The TenDRA Project.
 * Copyright 1997, United Kingdom Secretary of State for Defence.
 *
 * See doc/copyright/ for the full copyright terms.
 */

#include <stddef.h>
#include <stdio.h>

#include <shared/check.h>

#include "types.h"
#include "ascii.h"
#include "file.h"
#include "pretty.h"
#include "tree.h"


static void display(int);
static void expression(word *, int, int, int, int, int);


int helpflag = 1;
int dflag = 1;
int progress = 0;
int quickflag = 0;


/*
 * VARIABLES USED IN PRINTING ROUTINE
 *
 * The variable maxcol is used to specify the maximum number of columns
 * into which the output should be fitted.  maxtab gives the current
 * indentation.  dot_spacing is used to indicate the distance between
 * the vertical alignment lines when helpflag is true.
 */
int maxcol = 80;
static int maxtab;
static int dot_spacing;


/*
 * CHOICE TABLE
 *
 * This table is used to decide the maximum suitable indentation.  For
 * example the entry { a, b, c } means, try printing with an indentation
 * of a.  If this fits into the required number of columns, try with
 * the (larger) indentation c.  Otherwise try with the (smaller)
 * indentation b.
 */

#define M 100

static int dec[7][3] = {
    { 4, 2, 8 },
    { 2, 1, 3 },
    { 8, 6, M },
    { 1, 0, 1 },
    { 3, 2, 3 },
    { 6, 4, 6 },
    { M, 8, M }
};


/*
 * PRINT THE TDF TREE
 *
 * This routine has overall control of the printing of the tree.  It
 * determines the most suitable indentation by means of trial runs,
 * and then prints the tree with that indentation.
 */
void
pretty_tree(void)
{
    int i, j;
    int maximum0;

    initialize_tree();

    /* Do some trial runs to try to fit into right number of columns... */
    printflag = 0;

    /* With a maximum identation of 0... */
    maxtab = 0;
    display(0);
    maximum0 = maximum;

    /* Try some larger/smaller values */
    if (!quickflag && maximum < maxcol) {
	maxtab = 4;
	for (i = 0; i < 3; i++) {
	    display(1);
	    j = -1;
	    while (dec[++j][0]!= maxtab);
	    maxtab = dec[j][maximum < maxcol ? 2 : 1];
	}
    }

    /* Work out dot spacing */
    dot_spacing = 8;
    if (maxtab != 100) dot_spacing = 2 *(maxtab + 1);
    if (maxtab == 0) {
	if (maximum0 > maxcol)maxcol = maximum0;
	dot_spacing = 4;
    }
    init_spaces(dot_spacing);

    /* Actually do the printing */
    printflag = 1;
    display(0);
    if (progress) {
	IGNORE fprintf(stderr, "Printed in %d columns.\n", maximum);
    }
    return;
}


/*
 * TRY TO PRETTY-PRINT THE TDF TREE
 *
 * This routine actually outputs the tree.  If test is true, then this
 * is only a trial run, and may be aborted if the output exceeds the
 * required number of columns.
 */
static void
display(int test)
{
    word *ptr;
    maximum = 0;
    for (ptr = word1.next; ptr; ptr = ptr->next) {
	column = 0;
	expression(ptr, 0, 0, 0, 1, 0);
	if (test && (maximum > maxcol)) return;
    }
    return;
}


/*
 * OUTPUT A CHARACTER
 *
 * This routine outputs a single character into the output file.
 */
#define put_out(c)						\
    {								\
	if (c) {						\
	    if (printflag)IGNORE fputc((c), pp_file); \
	    column++; \
	    lastc = (c);					\
	}							\
    }


/*
 * OUTPUT A NEWLINE
 *
 * This routine outputs a newline character into the output file.
 * It also causes column to be compared against maximum.
 */
#define new_line()						\
    {								\
	if (column > maximum)maximum = column; \
	if (printflag)IGNORE fputc(NEWLINE, pp_file); \
	column = 0; \
	lastc = NEWLINE; \
    }


/*
 * OUTPUT A TDF EXPRESSION
 *
 * This routine is called recursively to print the tree.  ptr gives
 * the current position within the tree.  col gives the column
 * where printing should start.  flag is true to indicate that a
 * newline should be output at the end.  pending gives the number
 * of trailing close brackets which are pending.  first gives a
 * character which needs to be output before the current
 * expression, and last gives one which should be output after it.
 */
static void
expression(word *ptr, int col, int first, int last, int flag, int pending)
{
    word *p;
    char bar = '|';
    char open = '(';
    char close = ')';
    char comma = ',';
    int more, pends;
    char sort, opener, sep;
    int len, visible, horiz;
    int mflag, new_col, temp_col, temp_max;

    /* print initial spaces */
    if (column == 0)spaces(col);

    /* output first character */
    put_out(first);

    /* copy out initial text */
    col = column;
    len = (int)ptr->length;
    sort = ptr->type;
    if (printflag)IGNORE fputs(ptr->text, pp_file);
    column += len;

    /* if we have parameters, we need to decode them */
    if (sort != SIMPLE) {

	/* are the brackets visible or not? */
	visible = ((sort == HORIZ_NONE || sort == VERT_NONE)? 0 : 1);
	if (!visible) {
	    open = close = bar = 0;
	    comma = ' ';
	}

	/* are we printing horizontally or vertically? */
	horiz = (sort == HORIZ_BRACKETS || sort == HORIZ_NONE);

	/* does the maximum tab come into effect? */
	mflag = (!horiz && (len > maxtab));
	new_col = col + (mflag ? maxtab : len) + 1;

	if (sort == VERT_BRACKETS) {
	    /* try to print things with only one parameter horizontally */
	    p = ptr->child;
	    if (p == NULL) {
		horiz = 1;
	    } else {
		if (p->child == NULL && p->next == NULL)horiz = 1;
	    }
	}

	/* have a test run, if printing horizontally */
	if (horiz && printflag) {
	    horiz = 1;
	    /* save old values */
	    temp_col = column;
	    temp_max = maximum;
	    printflag = 0;
	    maximum = column;
	    /* open bracket */
	    put_out(open);
	    sep = comma;
	    pends = 0;
	    /* print parameters */
	    for (p = ptr->child; p; p = p->next) {
		if (p->next == NULL) {
		    /* for last, take trailing brackets into account */
		    sep = close;
		    pends = pending + visible;
		}
		/* print this parameter */
		expression(p, column, 0, sep, 0, pends);
	    }
	    /* close bracket */
	    put_out(last);
	    /* if this doesn't fit in, try vertically */
	    if (maximum >= maxcol || column + pending >= maxcol) {
		horiz = 0;
		mflag = (len > maxtab);
		new_col = col + (mflag ? maxtab : len) + 1;
	    }
	    /* restore old values */
	    column = temp_col;
	    maximum = temp_max;
	    printflag = 1;
	}

	if (horiz && printflag) {
	    /* do horizontal printing of parameters */
	    /* open bracket */
	    put_out(open);
	    sep = comma;
	    pends = 0;
	    /* print parameters */
	    for (p = ptr->child; p; p = p->next) {
		/* for last, take trailing brackets into account */
		if (p->next == NULL) {
		    sep = close;
		    pends = pending + visible;
		}
		/* print this parameter */
		expression(p, column, 0, sep, 0, pends);
	    }
	} else {
	    /* do vertical printing of parameters */
	    put_out(open);
	    opener = bar;
	    if (mflag) {
		new_line();
		spaces(new_col - 1);
	    }
	    sep = comma;
	    more = 1;
	    pends = 0;
	    new_col = column;
	    if (!mflag) {
		new_col--;
		opener = 0;
	    }
	    /* print parameters */
	    for (p = ptr->child; p; p = p->next) {
		if (p->next == NULL) {
		    /* last requires special treatment */
		    sep = 0;
		    more = 0;
		    pends = pending + visible;
		}
		/* print this parameter */
		expression(p, new_col, opener, sep, more, pends);
		opener = bar;
	    }
	    if (visible)put_out(close);
	}
    }

    /* output last character */
    if (last != ',' || (lastc != '.' && lastc != ':'))put_out(last);

    /* and a newline if required */
    if (flag)new_line();
    return;
}
