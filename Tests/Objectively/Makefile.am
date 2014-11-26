
AM_TESTS_ENVIRONMENT = \
	AM_TESTS=1; export AM_TESTS;

TESTS = \
	Array \
	Date \
	Dictionary \
	Log \
	Number \
	Object \
	Regex \
	String \
	Thread \
	URL

CFLAGS += \
	-I$(includedir) \
	@CHECK_CFLAGS@

LDADD = \
	-L$(libdir) -lObjectively \
	@CHECK_LIBS@

check_PROGRAMS = \
	$(TESTS) \
	Hello
