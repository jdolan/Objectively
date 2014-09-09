AUTOMAKE_OPTIONS = nostdinc

pkginclude_HEADERS = \
	array.h \
	class.h \
	dictionary.h \
	object.h \
	string.h \
	types.h

lib_LTLIBRARIES = \
	libobjectively.la

libobjectively_la_SOURCES = \
	array.c \
	class.c \
	dictionary.c \
	object.c \
	string.c

libobjectively_la_CFLAGS = \
	-I ..

libobjectively_la_LDFLAGS = \
	-shared

