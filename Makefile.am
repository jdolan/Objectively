ACLOCAL_AMFLAGS = -I m4

SUBDIRS = \
	Sources \
	Tests \
	Examples

html:
	doxygen

html-clean:
	rm -rf Documentation/html

html-release:
	rsync -rz --delete Documentation/html/ jaydolan.com:/var/www/projects/Objectively
