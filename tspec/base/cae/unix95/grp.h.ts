# $Id$

# Copyright 2002-2011, The TenDRA Project.
# Copyright 1997, United Kingdom Secretary of State for Defence.
#
# See doc/copyright/ for the full copyright terms.


+IMPLEMENT "cae/xsh4-base", "grp.h.ts" ;

+FUNC void endgrent ( void ) ;
+FUNC struct group *getgrent ( void ) ;
+FUNC void setgrent ( void ) ;
