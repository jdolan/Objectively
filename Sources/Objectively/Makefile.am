AUTOMAKE_OPTIONS = nostdinc
AM_CPPFLAGS = -I$(top_srcdir)/Sources

pkgincludedir = $(includedir)/Objectively

pkginclude_HEADERS = \
	Array.h \
	Boole.h \
	Class.h \
	Condition.h \
	Config.h \
	Data.h \
	Date.h \
	DateFormatter.h \
	Dictionary.h \
	Enum.h \
	Error.h \
	Hash.h \
	IndexPath.h \
	IndexSet.h \
	JSONPath.h \
	JSONSerialization.h \
	Locale.h \
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
	Resource.h \
	Set.h \
	String.h \
	Thread.h \
	Types.h \
	URL.h \
	URLRequest.h \
	URLSession.h \
	URLSessionConfiguration.h \
	URLSessionDataTask.h \
	URLSessionDownloadTask.h \
	URLSessionTask.h \
	URLSessionUploadTask.h \
	Value.h

lib_LTLIBRARIES = \
	libObjectively.la

libObjectively_la_SOURCES = \
	Array.c \
	Boole.c \
	Class.c \
	Condition.c \
	Data.c \
	Date.c \
	DateFormatter.c \
	Dictionary.c \
	Enum.c \
	Error.c \
	Hash.c \
	IndexPath.c \
	IndexSet.c \
	JSONPath.c \
	JSONSerialization.c \
	Locale.c \
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
	Resource.c \
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
	URLSessionUploadTask.c \
	Value.c

libObjectively_la_CFLAGS = \
	-I $(top_srcdir) \
	-I $(top_srcdir)/Sources \
	@HOST_CFLAGS@ \
	@CURL_CFLAGS@

libObjectively_la_LDFLAGS = \
	@HOST_LDFLAGS@ \
	-shared

libObjectively_la_LIBADD = \
	-lpthread \
	@HOST_LIBS@ \
	@CURL_LIBS@
