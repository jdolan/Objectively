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

install-xcode-templates:
	install -d ~/Library/Developer/Xcode/Templates/File\ Templates/Source
	ln -fs Templates/Xcode/Objectively\ C\ Type.xctemplate \
		~/Library/Developer/Xcode/Templates/File\ Templates/Source
