TEST_CFLAGS = \
	-I ../../src \
	@CHECK_CFLAGS@
	
TEST_LDADD = \
	@CHECK_LIBS@ \
	../../src/objectively/.libs/libobjectively.la

TESTS = \
	animals \
	shapes

noinst_PROGRAMS = $(TESTS)

animals_SOURCES = \
	animals.c
animals_CFLAGS = \
	$(TEST_CFLAGS)
animals_LDADD = \
	$(TEST_LDADD)

shapes_SOURCES = \
	shapes.c
shapes_CFLAGS = \
	$(TEST_CFLAGS)
shapes_LDADD = \
	$(TEST_LDADD)
