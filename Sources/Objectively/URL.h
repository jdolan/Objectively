/*
 * Objectively: Ultra-lightweight object oriented framework for c99.
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

#ifndef _Objectively_URL_h_
#define _Objectively_URL_h_

#include <Objectively/Array.h>
#include <Objectively/Object.h>
#include <Objectively/String.h>

/**
 * @file
 *
 * @brief Uniform Resource Locators (RFC 3986).
 *
 * @see http://www.ietf.org/rfc/rfc3986.txt
 */

typedef struct URL URL;
typedef struct URLInterface URLInterface;

/**
 * @brief The URL type.
 *
 * @ingroup URLSession
 */
struct URL {

	/**
	 * @brief The parent.
	 */
	Object object;

	/**
	 * @brief The typed interface.
	 */
	URLInterface *interface;

	/**
	 * @brief The fragment.
	 */
	String *fragment;

	/**
	 * @brief The host.
	 */
	String *host;

	/**
	 * @brief The path.
	 */
	String *path;

	/**
	 * @brief The port.
	 */
	unsigned short port;

	/**
	 * @brief The query.
	 */
	String *query;

	/**
	 * @brief The scheme, or protocol.
	 */
	String *scheme;

	/**
	 * @brief The URL String.
	 */
	String *urlString;
};

/**
 * @brief The URL interface.
 */
struct URLInterface {

	/**
	 * @brief The parent.
	 */
	ObjectInterface objectInterface;

	/**
	 * @return The base URL (scheme, host, and port) of this URL.
	 *
	 * @relates URL
	 */
	URL *(*baseURL)(const URL *self);

	/**
	 * @brief Initializes this URL with the specified characters.
	 *
	 * @param chars The URL characters.
	 *
	 * @return The initialized URL, or `NULL` on error.
	 *
	 * @relates URL
	 */
	URL *(*initWithCharacters)(URL *self, const char *chars);

	/**
	 * @brief Initializes this URL with the specified String.
	 *
	 * @param string The URL String.
	 *
	 * @return The initialized URL, or `NULL` on error.
	 *
	 * @relates URL
	 */
	URL *(*initWithString)(URL *self, const String *string);

	/**
	 * @return The `path` components of this URL.
	 *
	 * @relates URL
	 */
	Array *(*pathComponents)(const URL *self);
};

/**
 * @brief The URL Class.
 */
extern Class __URL;

#endif
