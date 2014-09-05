AC_PREREQ(2.69)

AC_INIT([objectively], [0.1.0], [jay@jaydolan.com])

AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR([src/objectively.h])
AC_CONFIG_HEADERS([config.h])

AC_CANONICAL_SYSTEM

AC_PROG_CC_STDC
AC_PROG_LIBTOOL

AM_INIT_AUTOMAKE([foreign])
AM_SILENT_RULES([yes])

PKG_CHECK_MODULES([CHECK], [check >= 0.9.4])

AC_CONFIG_FILES([
	Makefile
	src/Makefile
	src/objectively/Makefile
	src/objectively.pc
	test/Makefile
	test/objectively/Makefile
])

AC_OUTPUT
