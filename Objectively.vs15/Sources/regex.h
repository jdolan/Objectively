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

#if !defined(_MSC_VER)
#error What are you doing
#endif

#pragma once

typedef struct
{
	size_t		re_nsub;

	void		*buffer;
} regex_t;

typedef long regoff_t;

typedef struct
{
	regoff_t	rm_so, rm_eo;
} regmatch_t;

#define REG_EXTENDED		1
#define REG_ICASE			2
#define REG_NOSUB			4
#define REG_NEWLINE			8

#define REG_NOTBOL			1
#define REG_NOTEOL			2

#define REG_OK				-0
#define REG_NOMATCH			-1
#define REG_BADPAT			-2
#define REG_ECOLLATE		-3
#define REG_ECTYPE			-4
#define REG_EESCAPE			-5
#define REG_ESUBREG			-6
#define REG_EBRACK			-7
#define REG_EPAREN			-8
#define REG_EBRACE			-9
#define REG_BADBR			-10
#define REG_ERANGE			-11
#define REG_ESPACE			-12
#define REG_BADRPT			-13
#define REG_ENOSYS			-14
#define REG_BADFLG			-15

extern int    regcomp(regex_t * __restrict, const char * __restrict, int);
extern size_t regerror(int, const regex_t * __restrict, char * __restrict, size_t);
extern int    regexec(const regex_t * __restrict, const char * __restrict, size_t, regmatch_t * __restrict, int);
extern void   regfree(regex_t *);