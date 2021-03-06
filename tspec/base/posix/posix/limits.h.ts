# Copyright 2002-2011, The TenDRA Project.
# Copyright 1997, United Kingdom Secretary of State for Defence.
#
# See doc/copyright/ for the full copyright terms.


+IMPLEMENT "c/c89", "limits.h.ts" ;

+DEFMIN _POSIX_ARG_MAX 4096 ;
+DEFMIN _POSIX_CHILD_MAX 6 ;
+DEFMIN _POSIX_LINK_MAX 8 ;
+DEFMIN _POSIX_MAX_CANON 255 ;
+DEFMIN _POSIX_MAX_INPUT 255 ;
+DEFMIN _POSIX_NAME_MAX 14 ;
+DEFMIN _POSIX_NGROUPS_MAX 0 ;
+DEFMIN _POSIX_OPEN_MAX 16 ;
+DEFMIN _POSIX_PATH_MAX 255 ;
+DEFMIN _POSIX_PIPE_BUF 512 ;

+CONST int NGROUPS_MAX ;

+IF 0
/* These values are optional */
+CONST int ARG_MAX, CHILD_MAX, LINK_MAX, MAX_CANON, MAX_INPUT ;
+CONST int NAME_MAX, OPEN_MAX, PATH_MAX, PIPE_BUF ;
+ENDIF
