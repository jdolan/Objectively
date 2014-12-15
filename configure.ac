AC_PREREQ(2.69)

AC_INIT([Objectively], [0.1.0], [jay@jaydolan.com])

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR([Sources/Objectively.h])

AC_CANONICAL_SYSTEM

AC_GNU_SOURCE

AC_PROG_CC_STDC
AC_PROG_LIBTOOL

AM_INIT_AUTOMAKE([foreign])
AM_SILENT_RULES([yes])

AC_CHECK_HEADERS([pthread.h])

PKG_CHECK_MODULES([CHECK], [check >= 0.9.4])
PKG_CHECK_MODULES([CURL], [libcurl >= 7.16.0])

AC_CONFIG_FILES([
	Makefile
	Sources/Makefile
	Sources/Objectively/Makefile
	Sources/Objectively.pc
	Tests/Makefile
	Tests/Objectively/Makefile
])

AC_OUTPUT
