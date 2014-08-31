TESTS_CFLAGS = \
	-I ../src \
	@CHECK_CFLAGS@
	
TESTS_LIBS = \
	@CHECK_LIBS@ \
	../.libs/objectively.o

TESTS = \
	animals \
	shapes

noinst_PROGRAMS = $(TESTS)

animals_SOURCES = \
	animals.c
animals_CFLAGS = \
	$(TESTS_CFLAGS)
animals_LDADD = \
	$(TESTS_LIBS)

shapes_SOURCES = \
	shapes.c
shapes_CFLAGS = \
	$(TESTS_CFLAGS)
shapes_LDADD = \
	$(TESTS_LIBS)
