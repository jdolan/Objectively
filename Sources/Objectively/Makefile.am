AUTOMAKE_OPTIONS = nostdinc
AM_CPPFLAGS = -I$(top_srcdir)/Sources -iquote$(top_builddir)/Sources/Objectively

noinst_HEADERS = \
	Config.h

pkgincludedir = $(includedir)/Objectively

pkginclude_HEADERS = \
	Array.h \
	Boole.h \
	Class.h \
	Condition.h \
	Data.h \
	Date.h \
	DateFormatter.h \
	Dictionary.h \
	Enum.h \
	Error.h \
	Hash.h \
	HashTable.h \
	IndexPath.h \
	IndexSet.h \
	JSONContext.h \
	JSONPath.h \
	JSONSerializers.h \
	List.h \
	Lock.h \
	Log.h \
	Null.h \
	Number.h \
	NumberFormatter.h \
	Object.h \
	Operation.h \
	OperationQueue.h \
	Once.h \
	Pointer.h \
	PointerArray.h \
	Regexp.h \
	RESTClient.h \
	Resource.h \
	Set.h \
	String.h \
	StringReader.h \
	Thread.h \
	Types.h \
	URL.h \
	URLCache.h \
 	URLCachedResponse.h \
	URLRequest.h \
	URLResponse.h \
	URLSession.h \
	URLSessionConfiguration.h \
	URLSessionDataTask.h \
	URLSessionDownloadTask.h \
	URLSessionTask.h \
	URLSessionUploadTask.h \
	Vector.h

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
	HashTable.c \
	IndexPath.c \
	IndexSet.c \
	JSONContext.c \
	JSONPath.c \
	JSONSerializers.c \
	List.c \
	Lock.c \
	Log.c \
	Null.c \
	Number.c \
	NumberFormatter.c \
	Object.c \
	Operation.c \
	OperationQueue.c \
	Pointer.c \
	PointerArray.c \
	Regexp.c \
	RESTClient.c \
	Resource.c \
	Set.c \
	String.c \
	StringReader.c \
	Thread.c \
	URL.c \
	URLCache.c \
 	URLCachedResponse.c \
	URLRequest.c \
	URLResponse.c \
	URLSession.c \
	URLSessionConfiguration.c \
	URLSessionDataTask.c \
	URLSessionDownloadTask.c \
	URLSessionTask.c \
	URLSessionUploadTask.c \
	Vector.c

libObjectively_la_CFLAGS = \
	-I $(top_srcdir) \
	-I $(top_srcdir)/Sources \
	@HOST_CFLAGS@ \
	@CURL_CFLAGS@

libObjectively_la_LDFLAGS = \
	@HOST_LDFLAGS@ \
	-version-info @LT_VERSION@

libObjectively_la_LIBADD = \
	-lpthread \
	@HOST_LIBS@ \
	@CURL_LIBS@
