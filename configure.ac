AC_PREREQ(2.69)

AC_INIT([Objectively], [0.3.0], [jay@jaydolan.com])

AC_CONFIG_HEADERS([Sources/Objectively/Config.h])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR([Sources/Objectively.h])

AC_CANONICAL_HOST

AC_GNU_SOURCE

AC_PROG_CC_STDC
AC_PROG_CXX
AC_PROG_OBJC
AC_PROG_LIBTOOL

AM_INIT_AUTOMAKE([foreign])
AM_SILENT_RULES([yes])

AH_TOP([#pragma once])

HOST_NAME=""
HOST_CFLAGS=""
HOST_LDFLAGS=""
HOST_LIBS=""

AC_MSG_CHECKING([host type])
case "$host" in
	*-apple-*)
		HOST_NAME="APPLE"
		HOST_LDFLAGS="-headerpad_max_install_names"
		HOST_LIBS="-liconv"
		AC_MSG_RESULT(apple)
		;;
	*-*-mingw*)
		HOST_NAME="MINGW"
		HOST_LDFLAGS="-no-undefined"
		HOST_LIBS="-liconv -lmsvcr110 -lregex"
		AC_MSG_RESULT(mingw)
		;;
	*)
		HOST_NAME="LINUX"
		HOST_CFLAGS="-D_GNU_SOURCE"
		AC_MSG_RESULT(linux)
		;;
esac

AC_SUBST(HOST_NAME)
AC_SUBST(HOST_CFLAGS)
AC_SUBST(HOST_LDFLAGS)
AC_SUBST(HOST_LIBS)

AM_CONDITIONAL([APPLE], [test "x$HOST_NAME" = "xAPPLE"])
AM_CONDITIONAL([MINGW], [test "x$HOST_NAME" = "xMINGW"])
AM_CONDITIONAL([LINUX], [test "x$HOST_NAME" = "xLINUX"])

AC_CHECK_HEADERS([iconv.h])
AC_CHECK_HEADERS([locale.h])
AC_CHECK_HEADERS([xlocale.h])
AC_CHECK_HEADERS([pthread.h])
AC_CHECK_HEADERS([sys/time.h])

PKG_CHECK_MODULES([CHECK], [check >= 0.9.4])
PKG_CHECK_MODULES([CURL], [libcurl >= 7.16.0])

AC_CONFIG_FILES([
	Makefile
	Examples/Makefile
	Sources/Makefile
	Sources/Objectively/Makefile
	Sources/Objectively.pc
	Tests/Makefile
	Tests/Objectively/Makefile
])

AC_OUTPUT
