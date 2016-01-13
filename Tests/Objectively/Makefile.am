
AM_TESTS_ENVIRONMENT = \
	AM_TESTS=1; export AM_TESTS;

TESTS = \
	Array \
	Boolean \
	Date \
	Dictionary \
	Data \
	JSON \
	Locale \
	Log \
	MutableArray \
	MutableData \
	MutableDictionary \
	MutableSet \
	MutableString \
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
	-I$(top_builddir)/Sources \
	@CHECK_CFLAGS@

LDADD = \
	$(top_builddir)/Sources/Objectively/libObjectively.la \
	@CHECK_LIBS@

check_PROGRAMS = \
	$(TESTS)
