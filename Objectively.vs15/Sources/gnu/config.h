#pragma once

#define __USE_GNU
#define __builtin_expect(x, y) (x)

#define CODESET 0

#if defined(_UNICODE)
 static const char *nl_langinfo(int c)
 {
	 return "UTF-16";
 }
#elif defined(_MBCS)
 static const char *nl_langinfo(int c)
 {
	 return "ASCII";
 }
#else
 #error No charset?
#endif