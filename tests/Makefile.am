if BUILD_TESTS

TESTS_CFLAGS = \
	-I ../src \
	@CHECK_CFLAGS@
	
TESTS_LIBS = \
	@CHECK_LIBS@ \
	../src/libobjectively.la

TESTS = \
	animals

noinst_PROGRAMS = $(TESTS)

animals_SOURCES = \
	animals.c
animals_CFLAGS = \
	$(TESTS_CFLAGS)
animals_LDADD = \
	$(TESTS_LIBS)

endif
