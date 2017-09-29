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

#define timeval __real_timeval
#include <Windows.h>
#undef timeval

#include <Objectively/Config.h>
#include "Windowly.h"

#pragma region gettimeofday

const int64_t DELTA_EPOCH_IN_MICROSECS = 11644473600000000;

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
		tv->tv_sec = (int32_t)(tmpres * 0.000001);
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

int vasprintf(char ** __restrict ret, const char * __restrict format, va_list ap)
{
	int len;

	/* Get Length */
	len = _vscprintf(format, ap);

	if (len < 0)
		return -1;

	/* +1 for \0 terminator. */
	*ret = malloc(len + 1);

	/* Check malloc fail*/
	if (!*ret)
		return -1;

	/* Write String */
	_vsnprintf(*ret, len + 1, format, ap);

	/* Terminate explicitly */
	(*ret)[len] = '\0';

	return len;
}

char *asprintf(char ** __restrict ret, char * __restrict format, ...)
{
	va_list ap;
	int len;

	if (!format)
		return NULL;

	va_start(ap, format);

	/* Get Length */
	len = _vscprintf(format, ap);

	if (len < 0)
		return NULL;

	/* +1 for \0 terminator. */
	*ret = malloc(len + 1);

	/* Check malloc fail*/
	if (!*ret)
		return NULL;

	/* Write String */
	_vsnprintf(*ret, len + 1, format, ap);

	/* Terminate explicitly */
	(*ret)[len] = '\0';

	va_end(ap);
	return *ret;
}

#pragma endregion asprintf