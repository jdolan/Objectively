AUTOMAKE_OPTIONS = nostdinc

TEST_CFLAGS = \
	-I$(includedir) \
	@CHECK_CFLAGS@
	
TEST_LDADD = \
	@CHECK_LIBS@ \
	-L$(libdir) -lobjectively

TESTS = \
	object \
	string

noinst_PROGRAMS = $(TESTS)

object_SOURCES = \
	object.c
object_CFLAGS = \
	$(TEST_CFLAGS)
object_LDADD = \
	$(TEST_LDADD)

string_SOURCES = \
	string.c
string_CFLAGS = \
	$(TEST_CFLAGS)
string_LDADD = \
	$(TEST_LDADD)
