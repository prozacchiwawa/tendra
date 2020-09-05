# $Id$

.include "Makefile.inc"


.if !defined(NOTESTS)
SUBDIR+=	tdf
.endif

.if !defined(NOTESTS)
SUBDIR+=	c89-hosted
SUBDIR+=	c89-common
SUBDIR+=	c89-library
SUBDIR+=  c99
.endif

.if !defined(NOTESTS)
SUBDIR+=	posix.1-2008
.endif

.include <tendra.subdir.mk>

