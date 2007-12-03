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


/**** arg-data.h --- Command line argument data ADT.
 *
 ** Author: Steve Folkes <smf@hermes.mod.uk>
 *
 **** Commentary:
 *
 * See the file "arg-data.c" for more information.
 *
 **** Change Log:
 * $Log: arg-data.h,v $
 * Revision 1.1.1.1  1998/01/17  15:57:16  release
 * First version to be checked into rolling release.
 *
 * Revision 1.3  1995/07/07  15:31:55  smf
 * Updated to support TDF specification 4.0.
 *
 * Revision 1.2  1994/12/12  11:43:54  smf
 * Performing changes for 'CR94_178.sid+tld-update' - bringing in line with
 * OSSG C Coding Standards.
 *
 * Revision 1.1.1.1  1994/07/25  16:03:22  smf
 * Initial import of TDF linker 3.5 non shared files.
 *
**/

/****************************************************************************/

#ifndef H_ARG_DATA
#define H_ARG_DATA

#include "os-interface.h"
#include "cstring.h"
#include "cstring-list.h"
#include "dalloc.h"
#include "dstring.h"
#include "name-key.h"
#include "ostream.h"

/*--------------------------------------------------------------------------*/

typedef struct ShapeControlEntryT {
    struct ShapeControlEntryT  *next;
    NStringT			shape;
    BoolT			all;
    NameKeyListT		names;
} ShapeControlEntryT, *ShapeControlEntryP;

typedef struct ShapeControlT {
    ShapeControlEntryP		head;
} ShapeControlT, *ShapeControlP;

typedef struct RenameControlEntryT {
    struct RenameControlEntryT *next;
    NStringT			shape;
    NameKeyPairListT		names;
} RenameControlEntryT, *RenameControlEntryP;

typedef struct RenameControlT {
    RenameControlEntryP		head;
} RenameControlT, *RenameControlP;

typedef struct ArgDataT {
    BoolT			all_hide_defined;
    BoolT			suppress_mult;
    ShapeControlT		hides;
    ShapeControlT		keeps;
    ShapeControlT		suppresses;
    RenameControlT		renames;
    BoolT			extract_all;
    BoolT			extract_basename;
    BoolT			extract_match_base;
    BoolT			content_index;
    BoolT			content_size;
    BoolT			content_version;
    OStreamT			debug_file;
    CStringP			default_output_file;
    CStringP			output_file;
    unsigned			num_library_files;
    unsigned			num_library_paths;
    union {
	struct {
	    CStringListT	file;
	    CStringListT	path;
	} list;
	struct {
	    CStringP	       *file;
	    CStringP	       *path;
	} vector;
    } library;
    CStringP			unit_file;
    unsigned			num_files;
    CStringP		       *files;
} ArgDataT, *ArgDataP;

/*--------------------------------------------------------------------------*/

extern void			shape_control_iter
	(ShapeControlP, void(*)(NStringP, BoolT, NameKeyListP,
					   GenericP), GenericP);
extern void			rename_control_iter
	(RenameControlP, void(*)(NStringP, NameKeyPairListP,
					    GenericP), GenericP);

extern void			arg_data_init
(ArgDataP, CStringP);
extern void			arg_data_set_all_hide_defd
(ArgDataP, BoolT);
extern BoolT			arg_data_get_all_hide_defd
(ArgDataP);
extern void			arg_data_set_suppress_mult
(ArgDataP, BoolT);
extern BoolT			arg_data_get_suppress_mult
(ArgDataP);
extern void			arg_data_add_hide
(ArgDataP, CStringP, CStringP);
extern void			arg_data_add_hide_defined
(ArgDataP, CStringP);
extern ShapeControlP		arg_data_get_hides
(ArgDataP);
extern void			arg_data_add_keep
(ArgDataP, CStringP, CStringP);
extern void			arg_data_add_keep_all
(ArgDataP, CStringP);
extern ShapeControlP		arg_data_get_keeps
(ArgDataP);
extern void			arg_data_add_suppress
(ArgDataP, CStringP, CStringP);
extern void			arg_data_add_suppress_all
(ArgDataP, CStringP);
extern ShapeControlP		arg_data_get_suppresses
(ArgDataP);
extern void			arg_data_add_rename
(ArgDataP, NStringP, NameKeyP, NameKeyP);
extern void			arg_data_parse_rename
(ArgDataP, CStringP, CStringP, CStringP);
extern RenameControlP		arg_data_get_renames
(ArgDataP);
extern void			arg_data_set_extract_all
(ArgDataP, BoolT);
extern BoolT			arg_data_get_extract_all
(ArgDataP);
extern void			arg_data_set_extract_basename
(ArgDataP, BoolT);
extern BoolT			arg_data_get_extract_basename
(ArgDataP);
extern void			arg_data_set_extract_match_base
(ArgDataP, BoolT);
extern BoolT			arg_data_get_extract_match_base
(ArgDataP);
extern void			arg_data_set_content_index
(ArgDataP, BoolT);
extern BoolT			arg_data_get_content_index
(ArgDataP);
extern void			arg_data_set_content_size
(ArgDataP, BoolT);
extern BoolT			arg_data_get_content_size
(ArgDataP);
extern void			arg_data_set_content_version
(ArgDataP, BoolT);
extern BoolT			arg_data_get_content_version
(ArgDataP);
extern void			arg_data_set_debug_file
(ArgDataP, CStringP);
extern OStreamP			arg_data_get_debug_file
(ArgDataP);
extern void			arg_data_set_output_file
(ArgDataP, CStringP);
extern CStringP			arg_data_get_output_file
(ArgDataP);
extern void			arg_data_add_library_file
(ArgDataP, CStringP);
extern void			arg_data_add_library_path
(ArgDataP, CStringP);
extern void			arg_data_vector_libraries
(ArgDataP);
extern unsigned			arg_data_num_library_files
(ArgDataP);
extern unsigned			arg_data_num_library_paths
(ArgDataP);
extern CStringP		       *arg_data_library_files
(ArgDataP);
extern CStringP		       *arg_data_library_paths
(ArgDataP);
extern void			arg_data_set_unit_file
(ArgDataP, CStringP);
extern void			arg_data_set_files
(ArgDataP, int, CStringP *);
extern unsigned			arg_data_get_num_files
(ArgDataP);
extern CStringP		       *arg_data_get_files
(ArgDataP);

#endif /* !defined (H_ARG_DATA) */

/*
 * Local variables(smf):
 * eval: (include::add-path-entry "../os-interface" "../library" "../tdf")
 * eval: (include::add-path-entry "../generated")
 * End:
**/