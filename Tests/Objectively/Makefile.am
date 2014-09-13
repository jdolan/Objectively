AUTOMAKE_OPTIONS = nostdinc

TEST_CFLAGS = \
	-I$(includedir) \
	@CHECK_CFLAGS@
	
TEST_LDADD = \
	@CHECK_LIBS@ \
	../../Sources/Objectively/.libs/*.o

TESTS = \
	Array \
	Dictionary \
	Log \
	Object \
	String \
	Thread

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

Log_SOURCES = \
	Log.c
Log_CFLAGS = \
	$(TEST_CFLAGS)
Log_LDADD = \
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

Thread_SOURCES = \
	Thread.c
Thread_CFLAGS = \
	$(TEST_CFLAGS)
Thread_LDADD = \
	$(TEST_LDADD)

