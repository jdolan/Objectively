AUTOMAKE_OPTIONS = nostdinc

pkgincludedir = $(includedir)/Objectively

pkginclude_HEADERS = \
	Array.h \
	Class.h \
	Condition.h \
	Data.h \
	Date.h \
	DateFormatter.h \
	Dictionary.h \
	Error.h \
	Hash.h \
	Lock.h \
	Log.h \
	Number.h \
	NumberFormatter.h \
	Object.h \
	Once.h \
	Regex.h \
	String.h \
	Thread.h \
	URL.h \
	URLRequest.h \
	URLSession.h \
	URLSessionDataTask.h \
	URLSessionDownloadTask.h \
	URLSessionTask.h \
	URLSessionUploadTask.h \
	Types.h

lib_LTLIBRARIES = \
	libObjectively.la

libObjectively_la_SOURCES = \
	Array.c \
	Class.c \
	Condition.c \
	Data.c \
	Date.c \
	DateFormatter.c \
	Dictionary.c \
	Error.c \
	Hash.c \
	Lock.c \
	Log.c \
	Number.c \
	NumberFormatter.c \
	Object.c \
	Regex.c \
	String.c \
	Thread.c \
	URL.c \
	URLRequest.c \
	URLSession.c \
	URLSessionDataTask.c \
	URLSessionDownloadTask.c \
	URLSessionTask.c \
	URLSessionUploadTask.c

libObjectively_la_CFLAGS = \
	-I .. \
	@CURL_CFLAGS@

libObjectively_la_LDFLAGS = \
	-shared

libObjectively_la_LIBADD = \
	@CURL_LIBS@
