AUTOMAKE_OPTIONS = nostdinc

pkginclude_HEADERS = \
	class.h \
	macros.h \
	object.h \
	string.h \
	types.h

lib_LTLIBRARIES = \
	libobjectively.la

libobjectively_la_SOURCES = \
	class.c \
	object.c \
	string.c

libobjectively_la_CFLAGS = \
	-I ..

libobjectively_la_LDFLAGS = \
	-shared

