/*
 * $TenDRA$
 */

#ifndef __TDF_SYS_INCLUDED
#define __TDF_SYS_INCLUDED

#include <Xsystem>

#pragma TenDRA begin
#pragma TenDRA set size_t : unsigned long
#pragma TenDRA set wchar_t : unsigned int
#pragma TenDRA set ptrdiff_t : long
#pragma TenDRA character signed
#pragma TenDRA directive assert allow
#pragma TenDRA directive unassert allow
#pragma TenDRA longlong type allow

#ifndef __NO_BUILTIN_ASSERTIONS
#assert machine(alpha)
#assert cpu(alpha)
#assert system(unix)
#assert system(FreeBSD)
#unassert lint
#endif

#define unix		1
#define alpha		1
#define __unix		1
#define __alpha		1
#define __unix__	1
#define __alpha__	1
#define __FreeBSD__	2

#ifndef __inline
#pragma TenDRA keyword __inline for inline
#endif

#ifndef __signed
#define __signed	signed
#endif

#endif
