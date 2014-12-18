AUTOMAKE_OPTIONS = nostdinc

pkgincludedir = $(includedir)/Objectively

pkginclude_HEADERS = \
	Array.h \
	Boolean.h \
	Class.h \
	Condition.h \
	Data.h \
	Date.h \
	DateFormatter.h \
	Dictionary.h \
	Error.h \
	Hash.h \
	JSONPath.h \
	JSONSerialization.h \
	Lock.h \
	Log.h \
	MutableArray.h \
	MutableData.h \
	MutableDictionary.h \
	MutableSet.h \
	MutableString.h \
	Null.h \
	Number.h \
	NumberFormatter.h \
	Object.h \
	Operation.h \
	OperationQueue.h \
	Once.h \
	Regex.h \
	Set.h \
	String.h \
	Thread.h \
	URL.h \
	URLRequest.h \
	URLSession.h \
	URLSessionConfiguration.h \
	URLSessionDataTask.h \
	URLSessionDownloadTask.h \
	URLSessionTask.h \
	URLSessionUploadTask.h \
	Types.h

lib_LTLIBRARIES = \
	libObjectively.la

libObjectively_la_SOURCES = \
	Array.c \
	Boolean.c \
	Class.c \
	Condition.c \
	Data.c \
	Date.c \
	DateFormatter.c \
	Dictionary.c \
	Error.c \
	Hash.c \
	JSONPath.c \
	JSONSerialization.c \
	Lock.c \
	Log.c \
	MutableArray.c \
	MutableData.c \
	MutableDictionary.c \
	MutableSet.c \
	MutableString.c \
	Null.c \
	Number.c \
	NumberFormatter.c \
	Object.c \
	Operation.c \
	OperationQueue.c \
	Regex.c \
	Set.c \
	String.c \
	Thread.c \
	URL.c \
	URLRequest.c \
	URLSession.c \
	URLSessionConfiguration.c \
	URLSessionDataTask.c \
	URLSessionDownloadTask.c \
	URLSessionTask.c \
	URLSessionUploadTask.c

libObjectively_la_CFLAGS = \
	-I .. -I ../.. \
	@CURL_CFLAGS@

libObjectively_la_LDFLAGS = \
	-shared

libObjectively_la_LIBADD = \
	@CURL_LIBS@
