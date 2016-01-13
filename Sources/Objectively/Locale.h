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

#ifndef _Objectively_Locale_h_
#define _Objectively_Locale_h_

#if __MINGW32__
#include <locale.h>
typedef _locale_t LOCALE;
#else
#include <locale.h>
#include <xlocale.h>
typedef locale_t LOCALE;
#endif

#include <Objectively/Object.h>

/**
 * @file
 * 
 * @brief Locale.
 */

typedef struct Locale Locale;
typedef struct LocaleInterface LocaleInterface;

/**
 * @brief Locale.
 */
struct Locale {

	/**
	 * @brief The parent.
	 *
	 * @private
	 */
	Object object;

	/**
	 * @brief The typed interface.
	 *
	 * @private
	 */
	LocaleInterface *interface;

	/**
	 * @brief The locale identifier, e.g. `en_US`.
	 *
	 * @see xlocale.h, locale.h
	 */
	const char *identifier;

	/**
	 * @brief The internal C representation.
	 */
	LOCALE locale;
};

/**
 * @brief The Locale interface.
 */
struct LocaleInterface {

	/**
	 * @brief The parent interface.
	 */
	ObjectInterface objectInterface;

	/**
	 * @brief Initializes this Locale with the given identifier.
	 *
	 * @param identifier The locale identifier, e.g. `en-US`.
	 *
	 * @return The initialized Locale, or `NULL` on error.
	 *
	 * @relates Locale
	 */
	Locale *(*initWithIdentifier)(Locale *self, const char *identifier);

	/**
	 * @brief Initializes this Locale with the given locale.
	 *
	 * @param locale The locale.
	 *
	 * @return The initialized Locale, or `NULL` on error.
	 *
	 * @relates Locale
	 */
	Locale *(*initWithLocale)(Locale *self, LOCALE locale);

	/**
	 * @return The system Locale.
	 *
	 * @relates Locale
	 */
	Locale *(*systemLocale)(void);
};

/**
 * @brief The Locale Class.
 */
extern Class _Locale;

#endif
