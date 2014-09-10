AUTOMAKE_OPTIONS = nostdinc

pkginclude_HEADERS = \
	Array.h \
	Class.h \
	Dictionary.h \
	Lock.h \
	Object.h \
	String.h \
	Thread.h \
	Types.h

lib_LTLIBRARIES = \
	libobjectively.la

libobjectively_la_SOURCES = \
	Array.c \
	Class.c \
	Dictionary.c \
	Lock.c \
	Object.c \
	String.c \
	Thread.c

libobjectively_la_CFLAGS = \
	-I ..

libobjectively_la_LDFLAGS = \
	-pthread \
	-shared

