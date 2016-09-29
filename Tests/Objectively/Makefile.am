
AM_TESTS_ENVIRONMENT = \
	AM_TESTS=1; export AM_TESTS;

DEFAULT_INCLUDES = \
	$(top_srcdir)/Sources

TESTS = \
	Array \
	Boole \
	Date \
	Dictionary \
	Data \
	IndexPath \
	IndexSet \
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
	-I$(top_srcdir)/Sources \
	@HOST_CFLAGS@ \
	@CHECK_CFLAGS@

LDADD = \
	$(top_srcdir)/Sources/Objectively/libObjectively.la \
	@HOST_LIBS@ \
	@CHECK_LIBS@

check_PROGRAMS = \
	$(TESTS)
