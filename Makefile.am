ACLOCAL_AMFLAGS = -I m4

SUBDIRS = \
	Sources \
	Tests \
	Examples

# Requires: git clone --depth 1 https://github.com/jothepro/doxygen-awesome-css.git doxygen-awesome-css
html:
	doxygen

html-clean:
	rm -rf Documentation/html

