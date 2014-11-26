AUTOMAKE_OPTIONS = nostdinc

pkgincludedir = $(includedir)/Objectively

pkginclude_HEADERS = \
	Array.h \
	Class.h \
	Condition.h \
	Date.h \
	DateFormatter.h \
	Dictionary.h \
	Hash.h \
	Lock.h \
	Log.h \
	Number.h \
	NumberFormatter.h \
	Object.h \
	Regex.h \
	String.h \
	Thread.h \
	URL.h \
	Types.h

lib_LTLIBRARIES = \
	libObjectively.la

libObjectively_la_SOURCES = \
	Array.c \
	Class.c \
	Condition.c \
	Date.c \
	DateFormatter.c \
	Dictionary.c \
	Hash.c \
	Lock.c \
	Log.c \
	Number.c \
	NumberFormatter.c \
	Object.c \
	Regex.c \
	String.c \
	Thread.c \
	URL.c

libObjectively_la_CFLAGS = \
	-I ..

libObjectively_la_LDFLAGS = \
	-shared

