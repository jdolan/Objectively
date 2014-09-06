ACLOCAL_AMFLAGS = -I m4

SUBDIRS = \
	src \
	test

docsdir = $(abs_builddir)/docs

docs:
	doxygen

docs-clean:
	rm -rf $(docsdir)/html
