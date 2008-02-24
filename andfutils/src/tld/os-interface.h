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


/*
 * os-interface.h - Primitive definitions.
 *
 * This file specifies an interface to the host system's C library, compiler
 * and operating system.  It provides definitions of basic types, constants
 * and macros, and declarations of functions, that can be used by other
 * components of the program.
 *
 * This file (and others in this directory) makes use of several macros to
 * turn on certain features.  These macros are listed below.  In each case
 * there is a macro that begins with ``FS_NO_'' instead of ``FS_'' that turns
 * the feature off; if both are defined, the ``FS_NO_'' macro takes
 * precedence, and the feature is turned off.  In addition, the ``FS_NO_STDC''
 * macro may be used to turn off the ``__STDC__'' macro (in the cases where
 * the system isn't really ANSI compliant), and the ``FS_NO_FILE_INFO'' macro
 * should be defined if the ``__FILE__'' or ``__LINE__'' macros are
 * unsupported.
 *
 *	FS_FAST
 *
 * This should be defined if you want the program to use macros in place of
 * some function calls.  This will generally speed up the program (although
 * this is not guaranteed).  It is incompatible with ``FS_SHORTNAMES''.  It
 * also turns off the assertion checking (unless ``FS_ASSERT'' is also
 * defined).
 *
 *	FS_SHORTNAMES
 *
 * This is a hack for systems that cannot cope with long external identifier
 * names.  It includes the file "shortnames.h" which should macro define all
 * external identifiers to be a suitable short name.  It is incompatible with
 * ``FS_FAST''.
 *
 *	FS_ASSERT
 *
 * This should be defined if you want assertion checking enabled.  This is on
 * by default, unless ``FS_FAST'' is defined, so it is only really useful to
 * define this if you have also defined ``FS_FAST'', but still want assertions
 * to be checked.  It is possible that less assertions will be checked, as
 * some of the "inlined" functions may have lost their assertions in the
 * macro version.
 *
 *	FS_STDC_HASH
 *
 * This should be defined if the system supports ANSI C style macro hash
 * syntax (``#'' and ``##'').  It is automatically defined if the ``__STDC__''
 * macro is defined.
 *
 *	FS_PROTOTYPES
 *
 * This should be defined if the system supports ANSI C style function
 * prototypes.  It is automatically defined if the ``__STDC__'' macro is
 * defined.
 *
 *	FS_VOLATILE
 *
 * This should be defined if this system supports the ANSI C ``volatile''
 * keyword.  It is automatically defined if the ``__STDC__'' macro is defined.
 *
 *	FS_CONST
 *
 * This should be defined if this system supports the ANSI C ``const''
 * keyword.  It is automatically defined if the ``__STDC__'' macro is defined.
 *
 *	FS_ANSI_ENVIRON
 *
 * This should be defined if the system has an ANSI C compliant standard
 * library.  If this is not the case, then an attempt is made to approximate
 * that library.  It is automatically defined if the ``__STDC__'' macro is
 * defined to be ``1''.
 *
 *	FS_BINARY_STDIO
 *
 * This should be defined if the system supports the "b" modifier to the
 * standard library ``fopen'' function.  It is automatically defined if
 * ``FS_ANSI_ENVIRON'' is defined.
 *
 *	FS_STRERROR
 *
 * This should be defined if this system supports the ANSI ``strerror''
 * function for getting a textual description of a system error message.  It
 * is automatically defined if ``FS_ANSI_ENVIRON'' is defined.
 *
 *	FS_SYS_ERRLIST
 *
 * This should be defined if this system supports ``sys_nerr'' and
 * ``sys_errlist'' for getting a textual description of the current system
 * error.  If the ``FS_STRERROR'' macro is defined, then the ANSI ``strerror''
 * function will be used instead (overiding this macro).  If neither macro is
 * defined, then the error number will be used as the message.
 *
 *	FS_MKDIR
 *
 * This should be defined if this system supports the POSIX ``mkdir''
 * function.
 *
 * As well as the feature switch macros, there are some portability option
 * macros as well.  These provide values for types and constants when the type
 * cannot be otherwise deduced.  These macros are ignored when the correct
 * value can be obtained automatically (this generally means on an ANSI
 * compliant system).
 *
 *	PO_CHAR_BIT
 *
 * This should be defined to be the number of bits in a ``char'' object, used
 * to define ``CHAR_BIT''.  In an ANSI C compliant standard library, this
 * macro is defined in <limits.h>. If the standard library is not ANSI
 * compliant and no value is provided, then ``8'' will be used.
 *
 *	PO_EXIT_SUCCESS
 *	PO_EXIT_FAILURE
 *
 * These two macros should be defined to be the values for ``EXIT_SUCCESS''
 * and ``EXIT_FAILUE''.  In an ANSI C compliant standard library, they are
 * defined in <stdlib.h>.  If the standard library is not ANSI compliant and
 * no value is provided, then ``0'' and ``1'' will be used.
 *
 *	PO_UINT_MAX
 *
 * This should be defined to be the maximum unsinged integer, and is used to
 * define ``UINT_MAX''.  In an ANSI C compliant standard library, this macro
 * is defined in <limits.h>. If the standard library is not ANSI compliant and
 * no value is provided, then ``(~(unsigned) 0)'' will be used.
 *
 *	PO_SEEK_SET
 *	PO_SEEK_CUR
 *	PO_SEEK_END
 *
 * These should be defined to be the values for ``SEEK_SET'', ``SEEK_CUR'' and
 * ``SEEK_END''.  In an ANSI C compliant standard library, they are defined in
 * <stdio.h>.  If the standard library is not ANSI compliant and no value is
 * provided, then ``0'', ``1'' and ``2'' will be used.
 *
 * This file also requires that one external function be provided:
 *
 *	void E_assertion_failed(char *, char *, unsigned)
 *
 * This function will be called if an assertion fails.  It will be called with
 * the text of the assertion, the name of the file in which the assertion was
 * written, and line on which it occured.
 *
 ***=== FROM ANSI ============================================================
 *
 * The following ANSI C functions are declared:
 *
 *	NoReturnT			abort
 *			(void)
 *	void *			calloc
 *			(SizeT length, SizeT size)
 *	void *			malloc
 *			(SizeT size)
 *	NoReturnT			exit
 *			(int exit_code)
 *	void				free
 *			(void * pointer)
 *	char *			getenv
 *			(char * name)
 *	int				memcmp
 *			(void * ptr1, void * ptr2, SizeT length)
 *	void *			memcpy
 *			(void * to, void * from, SizeT length)
 *	void *			memset
 *			(void * ptr, int val, SizeT length)
 *	void *			memchr
 *			(void * ptr, int val, SizeT length)
 *	SizeT				strlen
 *			(char * string)
 *	int				strcmp
 *			(char * string1, char * string2)
 *	char *			strcpy
 *			(char * to, char * from)
 *	char *			strchr
 *			(char * string, int c)
 *	char *			strrchr
 *			(char * string, int c)
 *
 * Also everything in <setjmp.h> and <stdio.h> is included (hopefully this is
 * sufficiently common to be used even on non-ANSI platforms).  Where features
 * are absent on non-ANSI systems, support for them will need to be added.
 *
 ***=== FUNCTIONS ============================================================
 *
 ** Function:	int			mkdir
 *			(char * path, int mode)
 ** Exceptions:
 *
 * This function is only declared if the ``FS_MKDIR'' macro is defined.  This
 * macro should also be tested before the function is used anywhere.  It is
 * the POSIX directory creation function.
 *
 ** Function:	char *		strerror
 *			(int error)
 ** Exceptions:
 *
 * This function is only declared if the ``FS_STRERROR'' macro is defined.
 * This macro should also be tested before the function is used anywhere.  It
 * is the ANSI function for obtaining a textual description of a system error
 * message.
 *
 ***=== VARIABLES ============================================================
 *
 ** Variable:	int			sys_nerr
 *
 * This variable is only declared if the ``FS_SYS_ERRLIST'' macro is defined.
 * It contains the number of error messages stored in the ``sys_errlist''
 * vector.
 *
 ** Variable:	char *		sys_errlist []
 *
 * This variable is only declared if the ``FS_SYS_ERRLIST'' macro is defined.
 * It contains the text of the system error messages.
 *
 ** Variable:	int			errno
 *
 * This variable contains the number of the current system error.
 *
 ***=== TYPES ================================================================
 *
 ** Type:	NoReturnT
 ** Repr:	void
 *
 * This is the return type of a function that doesn't return (e.g.
 * ``longjmp'').  Normally this is just ``void'', however the GNU C compiler
 * can produce better code if this is declared as ``__volatile__ void''.  The
 * TenDRA compiler can be told that a function doesn't return as well.
 *
 ** Type:	SizeT
 ** Type:	SizeT *
 ** Repr:	size_t
 *
 * This is the type of the return value of the "sizeof" operator.
 *
 ** Type:	ByteT
 ** Type:	ByteT *
 ** Repr:	unsigned char
 *
 * This is the byte type.  It is possible that this could be larger than a
 * byte in some implementations.
 *
 ** Type:	BoolT
 ** Type:	BoolT *
 ** Repr:	int
 *
 * This is the boolean type.
 *
 ***=== CONSTANTS ============================================================
 *
 ** Constant:	TRUE
 ** Constant:	FALSE
 *
 * These expand to values suitable for the boolean constants true and false.
 *
 ** Constant:	EXIT_SUCCESS
 ** Constant:	EXIT_FAILURE
 *
 * These values should be used as arguments to the ``exit'' function to
 * indicate success or failure of the program.
 *
 ** Constant:	UINT_MAX
 *
 * This is the maximum unsigned integer value.
 *
 ** Constant:	CHAR_BIT
 *
 * This is the number of bits in a ``char'' object.
 *
 ***=== MACROS ===============================================================
 *
 ** Macro:	FS_NO_FILE_INFO
 ** Exceptions:
 *
 * This macro is defined if ``__FILE__'' or ``__LINE__'' are unsupported.
 * They will be defined as macros with some default value in this case, but
 * the programmer may wish to do something different.
 *
 ** Macro:	FS_STDC_HASH
 ** Exceptions:
 *
 * This macro is defined if the ANSI C hash operators (``#'' and ``##'') are
 * available.
 *
 ** Macro:	FS_BINARY_STDIO
 ** Exceptions:
 *
 * This macro is defined if ANSI binary input and output is supported
 * (i.e. the "b" modifier is allowed in the second argument to ``fopen'').
 *
 ** Macro:	FS_STRERROR
 ** Exceptions:
 *
 * This macro is defined if the ANSI function ``strerror'' is defined.
 *
 ** Macro:	FS_SYS_ERRLIST
 ** Exceptions:
 *
 * This macro is defined if the BSD ``sys_nerr'' and ``sys_errlist'' variables
 * are defined.  If ``FS_STRERROR'' is defined, then the ``strerror'' function
 * should be used in preference to ``sys_nerr'' and ``sys_errlist''.
 *
 ** Macro:	FS_MKDIR
 ** Exceptions:
 *
 * This macro is defined if the POSIX function ``mkdir'' is defined.
 *
 ** Macro:	NIL(type)
 ** Exceptions:
 *
 * This is the null pointer macro.  The argument is the type for which a null
 * pointer is required.
 *
 ** Macro:	INLINE
 ** Exceptions:
 *
 * This macro may be used before a function to indicate that the function
 * should be inlined.  Normally it expands to nothing, however under the GNU C
 * compiler it expands to ``__inline__'' which causes the compiler to inline
 * the function.
 *
 ** Macro:	ASSERT (assertion)
 ** Exceptions:
 *
 * This macro causes the program to abort if the assertion provided does not
 * hold.  Assertion checking is disabled if the ``FS_FAST'' macro is defined
 * or if the ``FS_NO_ASSERT'' macro is defined.  The assertion "ASSERT
 * (FALSE);" is used to indicate that the program should never reach the
 * current line.
 *
 ** Macro:	UNUSED (variable)
 ** Exceptions:
 *
 * This macro documents the fact that the specified variable will no longer be
 * used.  One use is to indicate function parameters that are not used.  On
 * most compilers it will do nothing, but on compilers that support it it will
 * tell the compiler that the variable is not expected to be used.
 *
 ** Macro:	EXHAUSTIVE
 ** Exceptions:
 *
 * This macro documents the fact that the switch statement in which it appears
 * is meant to be exhaustive.  It is used as follows:
 *
 *	switch (expression) EXHAUSTIVE { ... }
 *
 ** Macro:	UNREACHED
 ** Exceptions:
 *
 * This macro documents the fact that the location that it occurs in should be
 * unreachable.
 *
 ** Macro:	FALL_THROUGH
 ** Exceptions:
 *
 * This macro documents the fact that the current case of a switch statement
 * should follow through into the next (immediately following) case.  It is
 * used as follows:
 *
 *	case 1:
 *	  ...
 *	  FALL_THROUGH;
 *	case 2:
 *	  ...
 *
 */

#ifndef H_OS_INTERFACE
#define H_OS_INTERFACE

# ifdef FS_NO_SHORTNAMES
#  undef FS_SHORTNAMES
# endif /* defined (FS_NO_SHORTNAMES) */
# ifdef FS_NO_FAST
#  undef FS_FAST
# endif /* defined (FS_NO_FAST) */
# ifdef FS_SHORTNAMES
#  include "shortnames.h"
#  ifdef FS_FAST
#   include "fast-short.h"
#   undef FS_FAST
#  endif /* defined (FS_FAST) */
# endif /* defined (FS_SHORTNAMES) */
# ifndef FS_FAST
#  define FS_ASSERT
# endif /* !defined (FS_FAST) */
# ifdef FS_NO_ASSERT
#  undef FS_ASSERT
# endif /* defined (FS_NO_ASSERT) */

# ifndef FS_NO_STDC
#  ifdef __STDC__
#   define FS_STDC_HASH
#   define FS_PROTOTYPES
#   define FS_VOLATILE
#   define FS_CONST
#   if __STDC__ == 1
#    define FS_ANSI_ENVIRON
#   endif /* __STDC__ == 1 */
#  endif /* defined (__STDC__) */
# endif /* !defined (FS_NO_STDC) */

# ifdef FS_NO_FILE_INFO
#  define __FILE__ "unknown"
#  define __LINE__ 0
# endif /* defined (FS_NO_FILE_INFO) */

# ifdef FS_NO_STDC_HASH
#  undef FS_STDC_HASH
# endif /* defined (FS_NO_STDC_HASH) */

# ifdef FS_NO_VOLATILE
#  undef FS_VOLATILE
# endif /* defined (FS_NO_VOLATILE) */
# ifndef FS_VOLATILE
#  define volatile
# endif /* !defined (FS_VOLATILE) */

# ifdef FS_NO_CONST
#  undef FS_CONST
# endif /* defined (FS_NO_CONST) */
# ifndef FS_CONST
#  define const
# endif /* !defined (FS_CONST) */

# ifdef FS_NO_ANSI_ENVIRON
#  undef FS_ANSI_ENVIRON
# endif /* defined (FS_NO_ANSI_ENVIRON) */

# ifdef FS_ANSI_ENVIRON
#  define FS_BINARY_STDIO
#  define FS_STRERROR
# endif /* defined (FS_ANSI_ENVIRON) */
# ifdef FS_NO_BINARY_STDIO
#  undef FS_BINARY_STDIO
# endif /* defined (FS_NO_BINARY_STDIO) */
# ifdef FS_NO_STRERROR
#  undef FS_STRERROR
# endif /* defined (FS_NO_STRERROR) */
# ifdef FS_NO_SYS_ERRLIST
#  undef FS_SYS_ERRLIST
# endif /* defined (FS_NO_SYS_ERRLIST) */
# ifdef FS_NO_MKDIR
#  undef FS_MKDIR
# endif /* defined (FS_NO_MKDIR) */

# ifdef __GNUC__
typedef void NoReturnT;
# else
#  ifdef __TenDRA__
#   pragma TenDRA type NoReturnT for bottom
#  else
#   define NoReturnT void
#  endif /* defined (__TenDRA__) */
# endif /* defined (__GNUC__) */

# ifdef __TenDRA__
#  pragma TenDRA keyword UNUSED for discard variable
# else
#  define UNUSED(v)
# endif /* defined (__TenDRA__) */

typedef unsigned char ByteT;
typedef int BoolT;

# define NIL(type) ((type) (0))

# define FALSE (0)
# define TRUE (1)

# ifdef __GNUC__
#  define INLINE __inline__
# else
#  define INLINE
# endif /* defined (__GNUC__) */

# ifdef FS_MKDIR
extern int			mkdir
	(char *, int);
# endif /* defined (FS_MKDIR) */

# ifdef FS_ASSERT
extern void			E_assertion_failed
	(char *, char *, unsigned);
#  ifdef FS_STDC_HASH
#   define ASSERT(a) \
if (!(a)) { \
    E_assertion_failed (#a, __FILE__, (unsigned) __LINE__); \
    abort (); \
}
#  else
#   define ASSERT(a) \
if (!(a)) { \
    E_assertion_failed ("<unavailable>", __FILE__, (unsigned) __LINE__); \
    abort (); \
}
#  endif /* defined (FS_STDC_HASH) */
# else
#  define ASSERT(a)
# endif /* defined (FS_ASSERT) */

# ifdef __TenDRA__
#  pragma TenDRA keyword EXHAUSTIVE for exhaustive
#  pragma TenDRA keyword FALL_THROUGH for fall into case
#  pragma TenDRA keyword UNREACHED for set unreachable
# else
#  define EXHAUSTIVE
#  define FALL_THROUGH
#  define UNREACHED ASSERT (FALSE)
# endif /* defined (__TenDRA__) */

# ifdef FS_ANSI_ENVIRON
#  include <errno.h>
#  include <limits.h>
#  include <setjmp.h>
#  include <stddef.h>
#  include <stdio.h>
#  include <stdlib.h>
#  include <string.h>
typedef size_t SizeT;
# else

extern NoReturnT abort(void);
extern void * calloc(SizeT, SizeT);
extern void *	malloc(SizeT);
extern NoReturnT exit(int);
extern void free(void *);
extern char *	getenv(char *);
#  ifndef __GNUC__
extern int memcmp(void *, void *, SizeT);
#  endif /* defined (__GNUC__) */
#  ifndef __GNUC__
extern void *	memcpy(void *, void *, SizeT);
#  endif /* defined (__GNUC__) */
extern void *	memset(void *, int, SizeT);
extern void * memchr(void *, int, SizeT);
#  ifndef __GNUC__
extern SizeT strlen(char *);
#  endif /* defined (__GNUC__) */
#  ifndef __GNUC__
extern int strcmp(char *, char *);
#  endif /* defined (__GNUC__) */
#  ifndef __GNUC__
extern char * strcpy(char *, char *);
#  endif /* defined (__GNUC__) */
extern char * strchr(char *, int);
extern char *	strrchr(char *, int);
extern int errno;
#  ifdef FS_STRERROR
extern char * strerror(int);
#  endif /* defined (FS_STRERROR) */

#  include <setjmp.h>
#  include <stdio.h>

# endif /* !defined (FS_ANSI_ENVIRON) */

#  ifndef CHAR_BIT
#   ifdef PO_CHAR_BIT
#    define CHAR_BIT PO_CHAR_BIT
#   else
#    define CHAR_BIT 8
#   endif /* defined (PO_CHAR_BIT) */
#  endif /* !defined (CHAR_BIT) */

#  ifndef EXIT_SUCCESS
#   ifdef PO_EXIT_SUCCESS
#    define EXIT_SUCCESS PO_EXIT_SUCCESS
#   else
#    define EXIT_SUCCESS (0)
#   endif /* defined (PO_EXIT_SUCCESS) */
#  endif /* !defined (EXIT_SUCCESS) */

#  ifndef EXIT_FAILURE
#   ifdef PO_EXIT_FAILURE
#    define EXIT_FAILURE PO_EXIT_FAILURE
#   else
#    define EXIT_FAILURE (1)
#   endif /* defined (PO_EXIT_FAILURE) */
#  endif /* !defined (EXIT_FAILURE) */

#  ifndef UINT_MAX
#   ifdef PO_UINT_MAX
#    define UINT_MAX PO_UINT_MAX
#   else
#    define UINT_MAX (~(unsigned) 0)
#   endif /* defined (PO_UINT_MAX) */
#  endif /* !defined (UINT_MAX) */

#  ifndef SEEK_SET
#   ifdef PO_SEEK_SET
#    define SEEK_SET PO_SEEK_SET
#   else
#    define SEEK_SET (0)
#   endif /* defined (PO_SEEK_SET) */
#  endif /* !defined (SEEK_SET) */

#  ifndef SEEK_CUR
#   ifdef PO_SEEK_CUR
#    define SEEK_CUR PO_SEEK_CUR
#   else
#    define SEEK_CUR (1)
#   endif /* defined (PO_SEEK_CUR) */
#  endif /* !defined (SEEK_CUR) */

#  ifndef SEEK_END
#   ifdef PO_SEEK_END
#    define SEEK_END PO_SEEK_END
#   else
#    define SEEK_END (2)
#   endif /* defined (PO_SEEK_END) */
#  endif /* !defined (SEEK_END) */

# ifdef FS_SYS_ERRLIST
extern int			sys_nerr;
extern char *			sys_errlist [];
# endif /* defined (FS_SYS_ERRLIST) */

#endif /* !defined (H_OS_INTERFACE) */
