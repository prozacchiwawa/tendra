# Copyright 2011, The TenDRA Project.
#
# See doc/copyright/ for the full copyright terms.


.if !defined(_TENDRA_SID_MK_)
_TENDRA_SID_MK_=1

.include <tendra.base.mk>
.include <tendra.functions.mk>

.if !defined(SYNTAX)
.BEGIN:
	@${ECHO} '$${SYNTAX} must be set'
	@${EXIT} 1;
.endif


ACTIONS?=	${SYNTAX:S/.sid$/.act/}

.if defined(SIDOPTS)
. if !empty(SIDOPTS:M*split*:N*nosplit*)
SIDSPLIT?=	@
. endif
.endif

SID=${BASE_DIR}/../sid/obj/src/sid

# TODO: really sid ought to delete its own output on error.
.for src in ${SYNTAX}
${src:R}${SIDSPLIT}.c ${src:R}.h: ${src} ${SYNTAX} ${ACTIONS:M${src:R}.act}
	@${ECHO} "==> Translating ${WRKDIR}/${src}"
	${SID} ${SIDOPTS} ${src} ${ACTIONS:M${src:R}.act} ${src:R}${SIDSPLIT}.c ${src:R}.h \
		|| ( ${RMFILE} ${src:R}${SIDSPLIT:S/@/*/}.c ${src:R}.h; ${EXIT} 1 )
.endfor



#
# User-facing targets
#

.for src in ${SYNTAX}
regen:: ${src:R}${SIDSPLIT}.c ${src:R}.h
.endfor


regen-clean::
.for src in ${SYNTAX}
	${RMFILE} ${src:R}${SIDSPLIT:S/@/*/}.c ${src:R}.h
.endfor


install::



.endif
