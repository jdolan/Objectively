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
#if !defined(_MSC_VER)
#error What are you doing
#endif

#include <stdlib.h>

extern "C"
{
	#include "regex.h"
}

#include <regex>

struct regex_buff
{
	std::regex								regex;
	const char * __restrict					string;
	std::cregex_iterator					match_iterator;
};

extern "C"
{
	int    regcomp(regex_t * __restrict preg, const char * __restrict pattern, int cflags)
	{
		preg->re_nsub = 0;
		preg->buffer = nullptr;

		std::regex_constants::syntax_option_type sot = (cflags & REG_EXTENDED) ? std::regex_constants::syntax_option_type::extended : std::regex_constants::syntax_option_type::basic;
	
		if (cflags & REG_ICASE)
			sot |= std::regex_constants::syntax_option_type::icase;
		if (cflags & REG_NOSUB)
			sot |= std::regex_constants::syntax_option_type::nosubs;
		if (cflags & REG_NEWLINE)
			return REG_BADFLG;
	
		try
		{
			regex_buff *buffer = new regex_buff { std::regex(pattern, sot), nullptr };
			preg->buffer = buffer;
			preg->re_nsub = buffer->regex.mark_count();
		}
		catch (std::regex_error)
		{
			return REG_BADPAT;
		}

		return REG_OK;
	}

	size_t regerror(int errcode, const regex_t * __restrict preg, char * __restrict errbuf, size_t errbuf_size)
	{
		return 0;
	}

	int    regexec(const regex_t * __restrict preg, const char * __restrict string, size_t nmatch, regmatch_t * __restrict pmatch, int eflags)
	{
		regex_buff *buffer = (regex_buff *)preg->buffer;
		std::cregex_iterator end = std::cregex_iterator();

		if (!buffer->string || buffer->string != string)
		{
			buffer->string = string;
			std::regex_constants::match_flag_type mft = std::regex_constants::match_flag_type::match_default;
		
			if (eflags & REG_NOTBOL)
				mft |= std::regex_constants::match_flag_type::match_not_bol;
			if (eflags & REG_NOTEOL)
				mft |= std::regex_constants::match_flag_type::match_not_eol;

			buffer->match_iterator = std::cregex_iterator(string, string + strlen(string), buffer->regex, mft);
		}

		if (buffer->match_iterator == end)
			return REG_NOMATCH;

		std::cmatch match = *buffer->match_iterator;

		if (match.ready() && !match.empty() && match.size())
		{
			size_t smallest = std::min(match.size(), nmatch);

			for (size_t i = 0; i < smallest; ++i)
			{
				pmatch[i].rm_so = match.position(i);
				pmatch[i].rm_eo = pmatch[i].rm_so + match.length(i);
			}

			++buffer->match_iterator;
			return REG_OK;
		}

		return REG_NOMATCH;
	}

	void   regfree(regex_t *preg)
	{
		if (preg->buffer)
		{
			delete (std::regex *)preg->buffer;
			preg->buffer = nullptr;
		}
	}
};
