# $Id$

.include "Makefile.inc"


LIBCOPTS+=	-Nsystem:${LIBCPREFIX}/usr/include

PROXY+= float

.for proxy in ${PROXY}
APIOBJSc89+= ${OBJ_SDIR}/${proxy}.j
.endfor

.include <tendra.proxy.mk>


APIS+=  c89
APIS+=  c89-fs
APIS+=	c95
#APIS+=	cpp
#APIS+=	system

APIS+=	posix
#APIS+=	posix1
#APIS+=	posix2
#APIS+=	xpg3
#APIS+=	xpg4
#APIS+=	unix95
#APIS+=	svid3

#APIS+=	gcc
#APIS+=	bsd_extn

#APIS+= sus/xsh2

# This OS makes no attempt to provide conformance for these APIs
#APIS+= acse-xap
#APIS+= dtp-xa
#APIS+= xns5-ar
#APIS+= xns5-socket

.include <tendra.api.mk>

