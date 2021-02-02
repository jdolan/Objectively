/*
 * Objectively: Ultra-lightweight object oriented framework for GNU C.
 * Copyright (C) 2014 Jay Dolan <jay@jaydolan.com>
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 * claim that you wrote the original software. If you use this software
 * in a product, an acknowledgment in the product documentation would be
 * appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and must not be
 * misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source distribution.
 */

// Visual Studio Windows-related shims and adjustments

#include <WinSock2.h>

#include <Objectively/Config.h>
#include "Windowly.h"

#pragma region gettimeofday

static const int64_t DELTA_EPOCH_IN_MICROSECS = 11644473600000000;

long __sync_val_compare_and_swap_(long volatile *Destination, long Exchange, long Comparand) {
	return InterlockedCompareExchange(Destination, Exchange, Comparand);
}
long __sync_add_and_fetch_(long volatile *Append, long Value) {
	return InterlockedAdd(Append, Value);
}
void *__sync_lock_test_and_set_(void *volatile *Target, void *Value) {
	return InterlockedExchangePointer(Target, Value);
}

int gettimeofday(struct timeval *tv, struct timezone *tz)
{
	if (tv)
	{
		FILETIME ft;
		int64_t tmpres;

		ZeroMemory(&ft, sizeof(ft));

		GetSystemTimeAsFileTime(&ft);

		tmpres = ft.dwHighDateTime;
		tmpres <<= 32;
		tmpres |= ft.dwLowDateTime;

		/*converting file time to unix epoch*/
		tmpres /= 10;  /*convert into microseconds*/
		tmpres -= DELTA_EPOCH_IN_MICROSECS;
		tv->tv_sec = (int32_t)((double)tmpres * 0.000001);
		tv->tv_usec = (tmpres % 1000000);
	}

	//_tzset(),don't work properly, so we use GetTimeZoneInformation
	if (tz)
	{
		TIME_ZONE_INFORMATION tz_winapi;
		DWORD rez;

		ZeroMemory(&tz_winapi, sizeof(tz_winapi));

		rez = GetTimeZoneInformation(&tz_winapi);
		tz->tz_dsttime = (rez == 2) ? true : false;
		tz->tz_minuteswest = tz_winapi.Bias + ((rez == 2) ? tz_winapi.DaylightBias : 0);
	}

	return 0;
}

#pragma endregion gettimeofday

#pragma region asprintf

int vasprintf(char **str, const char *fmt, va_list args)
{
  int size = 0;
  va_list tmpa;

  // copy
  va_copy(tmpa, args);

  // apply variadic arguments to
  // sprintf with format to get size
  size = vsnprintf(NULL, 0, fmt, tmpa);

  // toss args
  va_end(tmpa);

  // return -1 to be compliant if
  // size is less than 0
  if (size < 0) { return -1; }

  // alloc with size plus 1 for `\0'
  *str = (char *) malloc(size + 1);

  // return -1 to be compliant
  // if pointer is `NULL'
  if (NULL == *str) { return -1; }

  // format string with original
  // variadic arguments and set new size
  size = vsprintf(*str, fmt, args);
  return size;
}

int asprintf(char **str, const char *fmt, ...)
{
  int size = 0;
  va_list args;

  // init variadic argumens
  va_start(args, fmt);

  // format and get size
  size = vasprintf(str, fmt, args);

  // toss args
  va_end(args);

  return size;
}

#pragma endregion asprintf