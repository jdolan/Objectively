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

#pragma once

#include <locale.h>

#include <Objectively/Config.h>

#if HAVE_XLOCALE_H
#include <xlocale.h>
#endif

#if defined(_WIN32)
typedef _locale_t locale_t;
#endif

#include <Objectively/Object.h>

/**
 * @file
 * @brief Locale abstraction for language and data formatting.
 */

/**
 * @defgroup Localization Localization
 * @brief Locale and localization facilities.
 */

typedef struct Locale Locale;
typedef struct LocaleInterface LocaleInterface;

/**
 * @brief Locale abstraction for language and data formatting.
 * @extends Object
 * @ingroup Localization
 */
struct Locale {

	/**
	 * @brief The superclass.
	 */
	Object object;

	/**
	 * @brief The interface.
	 * @protected
	 */
	LocaleInterface *interface;

	/**
	 * @brief The locale identifier, e.g. `en_US.UTF-8`.
	 * @see xlocale.h, locale.h
	 */
	char *identifier;

	/**
	 * @brief The internal C representation.
	 * @private
	 */
	ident locale;
};

/**
 * @brief The Locale interface.
 */
struct LocaleInterface {

	/**
	 * @brief The superclass interface.
	 */
	ObjectInterface objectInterface;

	/**
	 * @static
	 * @fn Locale *Locale::currentLocale(void)
	 * @return A copy of the current Locale.
	 * @memberof Locale
	 */
	Locale *(*currentLocale)(void);

	/**
	 * @fn Locale *Locale::initWithIdentifier(Locale *self, const char *identifier)
	 * @brief Initializes this Locale with the given identifier.
	 * @param self The Locale.
	 * @param identifier The locale identifier, e.g. `en-US`.
	 * @return The initialized Locale, or `NULL` on error.
	 * @memberof Locale
	 */
	Locale *(*initWithIdentifier)(Locale *self, const char *identifier);

	/**
	 * @fn Locale *Locale::initWithLocale(Locale *self, locale_t locale)
	 * @brief Initializes this Locale with the given locale.
	 * @param self The Locale.
	 * @param locale The backing locale.
	 * @return The initialized Locale, or `NULL` on error.
	 * @memberof Locale
	 */
	Locale *(*initWithLocale)(Locale *self, locale_t locale);

	/**
	 * @static
	 * @fn Locale *Locale::UTF8(void)
	 * @return The shared UTF-8 Locale.
	 * @memberof Locale
	 */
	Locale *(*UTF8)(void);
};

/**
 * @fn Class *Locale::_Locale(void)
 * @brief The Locale archetype.
 * @return The Locale Class.
 * @memberof Locale
 */
OBJECTIVELY_EXPORT Class *_Locale(void);
