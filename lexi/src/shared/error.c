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
#include "error.h"
#include "../release/release.h"


/*
    ALLOW BOTH STDARG AND VARARGS

    The flag FS_STDARG decides which of stdarg.h and varargs.h is to be
    used for dealing with functions with a variable number of arguments.
*/

#if FS_STDARG
#include <stdarg.h>
#else
#include <varargs.h>
#endif


/*
    RELEASE VERSION

    This macro gives the release version, if known.
*/

#ifndef RELEASE
#define RELEASE		"unknown"
#endif


/*
    ERROR FLAGS AND VARIABLES

    These variables are used or set in the error routines.
*/

CONST char *progname = NULL;
CONST char *progvers = NULL;
int exit_status = EXIT_SUCCESS;
int maximum_errors = 20;
static int number_errors = 0;

int crt_line_no = 1;
CONST char *crt_file_name = NULL;


/*
    SET PROGRAM NAME

    This routine sets the program name to nm and the program version to
    vers.
*/

void
set_progname(CONST char *nm, CONST char *vers)
{
    char *r = strrchr(nm, '/');
    progname = (r ? r + 1 : nm);
    progvers = vers;
    return;
}


/*
    PRINT VERSION NUMBER

    This routine prints the program name and version number.
*/

void
report_version(void)
{
    CONST char *r = RELEASE;
    CONST char *nm = progname;
    CONST char *vers = progvers;
    if (nm == NULL) nm = "unknown";
    if (vers == NULL) vers = "1.0";
    fprintf_v(stderr, "%s: Version %s (Release %s)\n", nm, vers, r);
    return;
}


/*
    PRINT AN ERROR MESSAGE

    This routine prints an error message s with arguments args and severity
    e.  fn and ln give the error position.
*/

static void
error_msg(int e, CONST char *fn, int ln, CONST char *s, va_list args)
{
    if (e != ERROR_NONE) {
	if (progname) fprintf_v(stderr, "%s: ", progname);
	switch (e) {
	    case ERROR_WARNING: {
		fprintf_v(stderr, "Warning: ");
		break;
	    }
	    case ERROR_FATAL: {
		fprintf_v(stderr, "Fatal: ");
		exit_status = EXIT_FAILURE;
		number_errors++;
		break;
	    }
	    default : {
		fprintf_v(stderr, "Error: ");
		exit_status = EXIT_FAILURE;
		number_errors++;
		break;
	    }
	}
	if (fn) {
	    fprintf_v(stderr, "%s: ", fn);
	    if (ln != -1) fprintf_v(stderr, "line %d: ", ln);
	}
	vfprintf_v(stderr, s, args);
	fprintf_v(stderr, ".\n");
	if (e == ERROR_FATAL) exit(EXIT_FAILURE);
	if (number_errors >= maximum_errors && maximum_errors) {
	    error(ERROR_FATAL, "Too many errors (%d) - aborting",
		    number_errors);
	}
    }
    return;
}


/*
    PRINT AN ERROR AT CURRENT POSITION

    This routine prints the error message s of severity e at the current
    file position.  s is a printf format string whose arguments are passed
    as the optional procedure parameters.
*/

void
error(int e, CONST char *s, ...)
    /*VARARGS*/
{
    va_list args;
#if FS_STDARG
    va_start(args, s);
#else
    int e;
    CONST char *s;
    va_start(args);
    e = va_arg(args, int);
    s = va_arg(args, CONST char *);
#endif
    error_msg(e, crt_file_name, crt_line_no, s, args);
    va_end(args);
    return;
}


/*
    PRINT AN ERROR AT A GIVEN POSITION

    This routine prints the error message s of severity e at the file
    position given by fn and ln.  s is as above.
*/

void
error_posn(int e, CONST char *fn, int ln, CONST char *s, ...)
    /*VARARGS*/
{
    va_list args;
#if FS_STDARG
    va_start(args, s);
#else
    int e;
    CONST char *fn;
    int ln;
    CONST char *s;
    va_start(args);
    e = va_arg(args, int);
    fn = va_arg(args, CONST char *);
    ln = va_arg(args, int);
    s = va_arg(args, CONST char *);
#endif
    error_msg(e, fn, ln, s, args);
    va_end(args);
    return;
}


#ifdef DEBUG

/*
    PRINT AN ASSERTION

    This routine prints the assertion s which occurred at the location
    given by file and line.
*/

void
assertion(CONST char *s, CONST char *file, int line)
{
    if (progname) fprintf_v(stderr, "%s: ", progname);
    fprintf_v(stderr, "Assertion: %s: line %d: '%s'.\n", file, line, s);
    abort();
}

#endif