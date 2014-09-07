AUTOMAKE_OPTIONS = nostdinc

pkginclude_HEADERS = \
	array.h \
	class.h \
	object.h \
	string.h \
	types.h

lib_LTLIBRARIES = \
	libobjectively.la

libobjectively_la_SOURCES = \
	array.c \
	class.c \
	object.c \
	string.c

libobjectively_la_CFLAGS = \
	-std=c11 \
	-I ..

libobjectively_la_LDFLAGS = \
	-shared

