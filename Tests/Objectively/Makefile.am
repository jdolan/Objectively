
TESTS = \
	Array \
	Date \
	Dictionary \
	Log \
	Object \
	Regex \
	String \
	Thread

CFLAGS += \
	-I$(includedir) \
	@CHECK_CFLAGS@

LDADD = \
	-L$(libdir) -lObjectively \
	@CHECK_LIBS@

check_PROGRAMS = \
	$(TESTS) \
	Hello
