
check_PROGRAMS = \
	Hello

CFLAGS += \
	-I$(includedir)
	
LDADD = \
	-L$(libdir) -lObjectively

check_PROGRAMS += \
	HelloObjC

HelloObjC_SOURCES = \
	HelloObjC.m

HelloObjC_LDFLAGS = \
	-framework Foundation
