AC_PREREQ(2.69)

AC_INIT([objectively], [0.1.0], [jay@jaydolan.com])

AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR([src/objectively.h])
AC_CONFIG_HEADERS([config.h])

AC_CANONICAL_SYSTEM

AC_PROG_CC
AC_PROG_LIBTOOL

AM_INIT_AUTOMAKE([silent-rules foreign])
AM_SILENT_RULES([yes])

dnl ---------------------------------
dnl Check whether to build unit tests
dnl ---------------------------------

BUILD_TESTS=no

AC_ARG_WITH(tests,
	AS_HELP_STRING([--with-tests],
		[build unit tests]
	)
)

AC_MSG_CHECKING(whether to build unit tests)

if test "x${with_tests}" = xyes; then
	BUILD_TESTS=yes
	PKG_CHECK_MODULES([CHECK], [check >= 0.9.4])
fi
AC_MSG_RESULT($BUILD_TESTS)

AM_CONDITIONAL(BUILD_TESTS, test "x$BUILD_TESTS" = xyes)

AC_CONFIG_FILES([
	Makefile
	src/Makefile
	tests/Makefile
])

AC_OUTPUT
