
TESTS = \
	Array \
	Date \
	Dictionary \
	Log \
	Object \
	String \
	Thread

CFLAGS += \
	-I$(includedir) \
	@CHECK_CFLAGS@

LDADD = \
	-L$(libdir) -lObjectively \
	@CHECK_LIBS@

noinst_PROGRAMS = \
	Hello

check_PROGRAMS = $(TESTS)
