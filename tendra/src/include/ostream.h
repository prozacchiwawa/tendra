/*
 * Copyright (c) 2002, The Tendra Project <http://www.ten15.org/>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice unmodified, this list of conditions, and the following
 *    disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *
 *    		 Crown Copyright (c) 1997
 *
 *    This TenDRA(r) Computer Program is subject to Copyright
 *    owned by the United Kingdom Secretary of State for Defence
 *    acting through the Defence Evaluation and Research Agency
 *    (DERA).  It is made available to Recipients with a
 *    royalty-free licence for its use, reproduction, transfer
 *    to other parties and amendment for any purpose not excluding
 *    product development provided that any such use et cetera
 *    shall be deemed to be acceptance of the following conditions:-
 *
 *        (1) Its Recipients shall ensure that this Notice is
 *        reproduced upon any copies or amended versions of it;
 *
 *        (2) Any amended version of it shall be clearly marked to
 *        show both the nature of and the organisation responsible
 *        for the relevant amendment or amendments;
 *
 *        (3) Its onward transfer from a recipient to another
 *        party shall be deemed to be that party's acceptance of
 *        these conditions;
 *
 *        (4) DERA gives no warranty or assurance as to its
 *        quality or suitability for any purpose and DERA accepts
 *        no liability whatsoever in relation to any use to which
 *        it may be put.
 *
 * $TenDRA$
 */


/**** ostream.h --- Output stream handling.
 *
 ** Original author: Steve Folkes <smf@hermes.mod.uk>
 *
 **** Commentary:
 *
 ***=== INTRODUCTION =========================================================
 *
 * This file specifies the interface to the output stream facility.
 *
 ***=== TYPES ================================================================
 *
 ** Type:	OStreamT
 ** Type:	OStreamP
 ** Repr:	<private>
 *
 * This is the output stream type.
 *
 ***=== CONSTANTS ============================================================
 *
 ** Constant:	ostream_output
 *
 * This is the standard output stream.  Its initial buffering state is
 * undefined.  The ``ostream_setup'' function must be called before this
 * constant is used.
 *
 ** Constant:	ostream_error
 *
 * This is the standard error stream.  Its initial buffering state is
 * undefined.  The ``ostream_setup'' function must be called before this
 * constant is used.
 *
 ***=== FUNCTIONS ============================================================
 *
 ** Function:	void ostream_setup(void)
 *
 * This function initialises the output stream facility.  It should be called
 * before any other ostream manipulation function.
 *
 ** Function:	void ostream_init(OStreamP ostream)
 *
 * This function initialises the specified ostream not to write to any file.
 *
 ** Function:	BoolT ostream_open(OStreamP ostream, const char* name)
 *
 * This function initialises the specified ostream to write to the file with
 * the specified name.  If the file cannot be opened, the function returns
 * false.  If the file is opened successfully, the function returns true.  The
 * name should not be modified or deallocated until the ostream has been
 * closed.  The initial buffering state of the ostream is fully buffered.
 *
 ** Function:	BoolT ostream_is_open(OStreamP ostream)
 *
 * This function returns true if the specified ostream is writing to a file,
 * and false otherwise.
 *
 ** Function:	void ostream_buffer(OStreamP ostream)
 *
 * This function sets the buffering state of the specified ostream to fully
 * buffered.
 *
 ** Function:	void ostream_unbuffer(OStreamP ostream)
 *
 * This function sets the buffering state of the specified ostream to
 * unbuffered.
 *
 ** Function:	void ostream_close(OStreamP ostream)
 *
 * This function closes the specified ostream.
 *
 ** Function:	void ostream_flush(OStreamP ostream)
 *
 * This function flushes the ostream's output buffer.
 *
 ** Function:	const char* ostream_name(OStreamP ostream)
 *
 * This function returns the name of the file that the specified ostream is
 * writing to.  The return value should not be modified or deallocated.
 *
 ** Function:	unsigned ostream_line(OStreamP ostream)
 *
 * This function returns one more than the number of newlines that have been
 * written to the specified ostream.  The result is undefined if the stream
 * is not open.
 *
 ** Function:	void write_newline(OStreamP ostream)
 *
 * This function writes a newline to the specified ostream.
 *
 ** Function:	void write_tab(OStreamP ostream)
 *
 * This function writes a tab to the specified ostream.
 *
 ** Function:	void write_byte(OStreamP ostream, ByteT c)
 *
 * This function writes the specified byte to the specified ostream.
 *
 ** Function:	void write_char(OStreamP ostream, char c)
 *
 * This function writes the specified character to the specified ostream.
 *
 ** Function:	void write_escaped_char(OStreamP ostream, char c)
 *
 * This function writes the specified character to the specified ostream.
 * This differs from the ``write_char'' function, in that it will
 * `pretty-print' non-printing characters.
 *
 ** Function:	void write_int(OStreamP ostream, int i)
 *
 * This function writes the specified integer to the specified ostream.
 *
 ** Function:	void write_unsigned(OStreamP ostream, unsigned i)
 *
 * This function writes the specified unsigned integer to the specified
 * ostream.
 *
 ** Function:	void write_cstring(OStreamP ostream, const char* cstring)
 *
 * This function writes the specified C string to the specified ostream.
 *
 ** Function:	void write_bytes(OStreamP ostream, ByteP bytes, size_t length)
 *
 * This function writes the specified sequence of bytes (of the specified
 * length) to the specified ostream.
 *
 ** Function:	void write_chars(OStreamP ostream, const char *chars,
 *			size_t length)
 *
 * This function writes the specified sequence of characters (of the specified
 * length) to the specified ostream.
 *
 ** Function:	void write_escaped_chars(OStreamP ostream, const char *chars,
 *			size_t length)
 *
 * This function writes the specified sequence of characters (of the specified
 * length) to the specified ostream.  This differs from the ``write_chars''
 * function, in that it will `pretty-print' non-printing characters.
 *
 ** Function:	void write_system_error(OStreamP ostream)
 *
 * This function writes a string containing a description of the current
 * system error (as defined by the ``errno'' global variable) to the specified
 * ostream.
 *
 ** Function:	void write_pointer(OStreamP ostream, const void *pointer)
 *
 * This function writes a string containing the address of the object pointed
 * to by the pointer to the specified ostream.
 *
 */

#ifndef TEN_OSTREAM_H
#define TEN_OSTREAM_H

#include "config.h"

/*
 * Output stream control structure
 */
typedef struct OStreamT {
	FILE	       *file;
	const char *	name;
	unsigned	line;
	unsigned	column;
	const char *	gen_name;
	int		no;
} OStreamT, *OStreamP;

/*
 * Standard output streams
 */
extern OStreamT *const ostream_output;
extern OStreamT *const ostream_error;

OStreamP ostream_new(void);
void	ostream_free(OStreamP);
void	ostream_setup(void);
void	ostream_init(OStreamP);
BoolT	ostream_open(OStreamP, const char *);
BoolT	ostream_is_open(OStreamP);
void	ostream_buffer(OStreamP);
void	ostream_unbuffer(OStreamP);
void	ostream_close(OStreamP);
void	ostream_flush(OStreamP);
const char *ostream_name(OStreamP);
const char *ostream_gen_name(OStreamP);
unsigned ostream_line(OStreamP);
unsigned ostream_column(OStreamP);
void	ostream_unput(OStreamP, unsigned);

void	write_byte(OStreamP, ByteT);
void	write_bytes(OStreamP, ByteP, size_t);
void	write_char(OStreamP, char);
void	write_chars(OStreamP, const char *, size_t);
void	write_cstring(OStreamP, const char*);
void	write_escaped_char(OStreamP, char);
void	write_escaped_chars(OStreamP, const char*, size_t);
void	write_fmt(OStreamP, const char *, ...);
void	write_int(OStreamP, int);
void	write_newline(OStreamP);
void	write_pointer(OStreamP, const void*);
void	write_system_error(OStreamP);
void	write_tab(OStreamP);
void	write_unsigned(OStreamP, unsigned);

#ifdef OSTREAM_VFMT
void	write_vfmt(OStreamP, const char *, va_list);
#endif

#endif /* !defined (TEN_OSTREAM_H) */
