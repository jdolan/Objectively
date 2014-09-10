AUTOMAKE_OPTIONS = nostdinc

TEST_CFLAGS = \
	-I$(includedir) \
	@CHECK_CFLAGS@
	
TEST_LDADD = \
	@CHECK_LIBS@ \
	-L$(libdir) -lobjectively

TESTS = \
	array \
	dictionary \
	object \
	string

check_PROGRAMS = $(TESTS)

array_SOURCES = \
	array.c
array_CFLAGS = \
	$(TEST_CFLAGS)
array_LDADD = \
	$(TEST_LDADD)

dictionary_SOURCES = \
	dictionary.c
dictionary_CFLAGS = \
	$(TEST_CFLAGS)
dictionary_LDADD = \
	$(TEST_LDADD)

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
