noinst_PROGRAMS = \
	Hello \
	HelloCpp

CFLAGS += \
	-I$(top_srcdir)/Sources \
	@HOST_CFLAGS@

CXXFLAGS += \
	-I$(top_srcdir)/Sources \
	@HOST_CFLAGS@
	
LDADD = \
	$(top_builddir)/Sources/Objectively/libObjectively.la \
	@HOST_LIBS@

HelloCpp_SOURCES = \
	HelloCpp.cpp

HelloCpp_LDADD = \
	$(LDADD)

if APPLE

noinst_PROGRAMS += \
	HelloObjC

OBJCFLAGS += \
	-I$(top_srcdir)/Sources \
	@HOST_CFLAGS@

HelloObjC_SOURCES = \
	HelloObjC.m

HelloObjC_LDFLAGS = \
	-framework Foundation

endif
