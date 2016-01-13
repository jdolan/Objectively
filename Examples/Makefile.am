
check_PROGRAMS = \
	Hello

CFLAGS += \
	-I$(includedir) \
	@HOST_CFLAGS@
	
LDADD = \
	-L$(libdir) -lObjectively \
	@HOST_LIBS@

if APPLE

check_PROGRAMS += \
	HelloObjC

HelloObjC_SOURCES = \
	HelloObjC.m

HelloObjC_LDFLAGS = \
	-framework Foundation

endif