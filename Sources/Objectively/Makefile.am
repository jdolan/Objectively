AUTOMAKE_OPTIONS = nostdinc

pkgincludedir = $(includedir)/Objectively

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
	libObjectively.la

libObjectively_la_SOURCES = \
	Array.c \
	Class.c \
	Dictionary.c \
	Lock.c \
	Object.c \
	String.c \
	Thread.c

libObjectively_la_CFLAGS = \
	-I ..

libObjectively_la_LDFLAGS = \
	-pthread \
	-shared

