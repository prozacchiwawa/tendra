/*
 * Copyright (c) 2002-2006 The TenDRA Project <http://www.tendra.org/>.
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


#ifndef DIAG_INCLUDED
#define DIAG_INCLUDED


/*
    TDF DIAGNOSTICS ENCODING ROUTINES

    The routines in this module are concerned with the encoding of TDF
    diagnostic information.
*/

#if TDF_OUTPUT
extern void enc_diag_id(IDENTIFIER, int);
extern void enc_diag_init(const char *, ulong, TYPE);
extern void enc_diag_token(IDENTIFIER, TYPE);
extern BITSTREAM *enc_diag_name(BITSTREAM *, IDENTIFIER, int);
extern BITSTREAM *enc_diag_local(BITSTREAM *, IDENTIFIER, BITSTREAM *);
extern BITSTREAM *enc_diag_params(BITSTREAM *, LIST(IDENTIFIER), BITSTREAM *,
				  EXP);
extern BITSTREAM *enc_diag_start(BITSTREAM *);
extern BITSTREAM *enc_diag_begin(BITSTREAM **);
extern BITSTREAM *enc_diag_stmt(BITSTREAM *, EXP, int);
extern BITSTREAM *enc_diag_end(BITSTREAM *, BITSTREAM *, EXP, int);
extern BITSTREAM *enc_diag_block(BITSTREAM *, EXP);
extern BITSTREAM *enc_diag_ctype(BITSTREAM *, CLASS_TYPE);
extern BITSTREAM *enc_diag_type(BITSTREAM *, TYPE, int);
extern int enc_diag_pending(void);
extern int is_diag_stmt(EXP);
#endif


#endif