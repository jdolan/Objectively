/*
 * Objectively: Ultra-lightweight object oriented framework for GNU C.
 * Copyright (C) 2014 Jay Dolan <jay@jaydolan.com>
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 * 1. The origin of this software must not be misrepresented; you must not
 * claim that you wrote the original software. If you use this software
 * in a product, an acknowledgment in the product documentation would be
 * appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 * misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */

#pragma once

// WinSock2 is included since it includes Windows.h
// and includes def for timeval.
#include <WinSock2.h>

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

// MATH STUFF
#include <float.h>

#ifndef __DBL_EPSILON__
#define __DBL_EPSILON__ DBL_EPSILON
#endif

// TIME STUFF
struct timezone
{
	int32_t		tz_minuteswest; /* minutes W of Greenwich */
	_Bool		tz_dsttime;     /* type of dst correction */
};

extern int gettimeofday(struct timeval *tv, struct timezone *tz);

// STRING STUFF
extern int vasprintf(char ** __restrict ret, const char * __restrict format, va_list ap);
extern char *asprintf(char ** __restrict ret, char * __restrict format, ...);

#define towupper_l _towupper_l
#define towlower_l _towlower_l
#define strcasecmp _stricmp

// INTERLOCK STUFF
#define __sync_val_compare_and_swap(c0, c1, c2) InterlockedCompareExchange((volatile long*)c0, c1, c2)
#define __sync_add_and_fetch(c0, c1) InterlockedAdd((volatile long*)c0, c1)
#define __sync_lock_test_and_set(c0, c1) InterlockedExchangePointer((PVOID volatile *)c0, (PVOID)c1)

// POSIX STUFF
#define strdup _strdup

#include <io.h>
#define isatty _isatty

#if !defined(__clang__)
// THREAD STUFF
#define __thread __declspec(thread)

// OTHER CRAP
#define __attribute__(...)
#else
#define fileno _fileno
#define tzset _tzset
#define alloca _alloca
#endif

#define iconv(c0, c1, c2, c3, c4) libiconv(c0, (const char **)c1, c2, c3, c4)

#undef interface