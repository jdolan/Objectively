AUTOMAKE_OPTIONS = nostdinc

TEST_CFLAGS = \
	-I$(includedir) \
	@CHECK_CFLAGS@
	
TEST_LDADD = \
	@CHECK_LIBS@ \
	-L$(libdir) -lObjectively

TESTS = \
	Array \
	Dictionary \
	Object \
	String

check_PROGRAMS = $(TESTS)

Array_SOURCES = \
	Array.c
Array_CFLAGS = \
	$(TEST_CFLAGS)
Array_LDADD = \
	$(TEST_LDADD)

Dictionary_SOURCES = \
	Dictionary.c
Dictionary_CFLAGS = \
	$(TEST_CFLAGS)
Dictionary_LDADD = \
	$(TEST_LDADD)

Object_SOURCES = \
	Object.c
Object_CFLAGS = \
	$(TEST_CFLAGS)
Object_LDADD = \
	$(TEST_LDADD)

String_SOURCES = \
	String.c
String_CFLAGS = \
	$(TEST_CFLAGS)
String_LDADD = \
	$(TEST_LDADD)
