SUBDIRS = \
	. \
	tests

include_HEADERS = \
	objectively.h

lib_LTLIBRARIES = \
	libobjectively.la

libobjectively_la_SOURCES = \
	objectively.c

libobjectively_la_LDFLAGS = \
	-shared

pkgconfigdir = $(libdir)/pkgconfig
nodist_pkgconfig_DATA = libobjectively.pc
