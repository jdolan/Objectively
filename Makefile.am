ACLOCAL_AMFLAGS = -I m4

SUBDIRS = \
	Sources \
	Tests

html:
	doxygen

html-release:
	rsync -rz Documentation/html/ jaydolan.com:/var/www/projects/objectively
