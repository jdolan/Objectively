
AM_TESTS_ENVIRONMENT = \
	AM_TESTS=1; export AM_TESTS;

DEFAULT_INCLUDES = \
	$(top_srcdir)/Sources

TESTS = \
	Array \
	Boole \
	Data \
	Date \
	Dictionary \
	IndexPath \
	IndexSet \
	JSON \
	Log \
	MutableArray \
	MutableData \
	MutableDictionary \
	MutableIndexSet \
	MutableSet \
	MutableString \
	Null \
	Number \
	Object \
	Operation \
	Regexp \
	Resource \
	Set \
	String \
	StringReader \
	Thread \
	URL \
	URLSession \
	Vector

CFLAGS += \
	-I$(top_srcdir)/Sources \
	@HOST_CFLAGS@ \
	@CHECK_CFLAGS@

LDADD = \
	$(top_srcdir)/Sources/Objectively/libObjectively.la \
	@HOST_LIBS@ \
	@CHECK_LIBS@

check_PROGRAMS = \
	$(TESTS)
