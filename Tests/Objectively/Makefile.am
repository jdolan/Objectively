AUTOMAKE_OPTIONS = nostdinc

CFLAGS = \
	-I../../Sources \
	@CHECK_CFLAGS@
	
LDADD = \
	../../Sources/Objectively/.libs/*.o \
	@CHECK_LIBS@

noinst_PROGRAMS = \
	Hello

TESTS = \
	Array \
	Date \
	Dictionary \
	Log \
	Object \
	String \
	Thread

check_PROGRAMS = $(TESTS)
