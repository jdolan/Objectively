AUTOMAKE_OPTIONS = nostdinc
AM_CPPFLAGS = -I$(top_srcdir)/Sources

noinst_HEADERS = \
	Hello.h

noinst_PROGRAMS = \
	Hello \
	HelloCpp

CFLAGS += \
	@HOST_CFLAGS@

CXXFLAGS += \
	@HOST_CFLAGS@

LDADD = \
	$(top_builddir)/Sources/Objectively/libObjectively.la \
	@HOST_LIBS@

HelloCpp_SOURCES = \
	HelloCpp.cpp

HelloCpp_LDADD = \
	$(LDADD)
