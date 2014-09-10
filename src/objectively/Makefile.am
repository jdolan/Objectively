AUTOMAKE_OPTIONS = nostdinc

pkginclude_HEADERS = \
	array.h \
	class.h \
	dictionary.h \
	lock.h \
	object.h \
	string.h \
	thread.h \
	types.h

lib_LTLIBRARIES = \
	libobjectively.la

libobjectively_la_SOURCES = \
	array.c \
	class.c \
	dictionary.c \
	lock.c \
	object.c \
	string.c \
	thread.c

libobjectively_la_CFLAGS = \
	-I ..

libobjectively_la_LDFLAGS = \
	-pthread \
	-shared

