
AM_TESTS_ENVIRONMENT = \
	AM_TESTS=1; export AM_TESTS;

TESTS = \
	Array \
	Boolean \
	Date \
	Dictionary \
	Data \
	JSON \
	Log \
	Null \
	Number \
	Object \
	Operation \
	Regex \
	Set \
	String \
	Thread \
	URL \
	URLSession

CFLAGS += \
	-I$(includedir) \
	@CHECK_CFLAGS@

LDADD = \
	-L$(libdir) -lObjectively \
	@CHECK_LIBS@

check_PROGRAMS = \
	$(TESTS)