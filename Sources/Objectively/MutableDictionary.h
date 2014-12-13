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

#ifndef _Objectively_MutableDictionary_h_
#define _Objectively_MutableDictionary_h_

#include <Objectively/Dictionary.h>

/**
 * @file
 *
 * @brief Mutable key-value stores.
 */

typedef struct MutableDictionary MutableDictionary;
typedef struct MutableDictionaryInterface MutableDictionaryInterface;

/**
 * @brief Mutable key-value stores.
 *
 * @extends Dictionary
 *
 * @ingroup Collections
 */
struct MutableDictionary {

	/**
	 * @brief The parent.
	 */
	Dictionary dictionary;

	/**
	 * @brief The typed interface.
	 */
	MutableDictionaryInterface *interface;
};

/**
 * @brief The MutableDictionary interface.
 *
 * @extends DictionaryInterface
 *
 * @ingroup Collections
 */
struct MutableDictionaryInterface {

	/**
	 * @brief The parent.
	 */
	DictionaryInterface dictionaryInterface;

	/**
	 * @brief Adds the key-value entries from `dictionary` to this Dictionary.
	 *
	 * @param dictionary A Dictionary.
	 */
	void (*addEntriesFromDictionary)(MutableDictionary *self, const Dictionary *dictionary);

	/**
	 * @brief Initializes this Dictionary.
	 *
	 * @return The initialized Dictionary, or `NULL` on error.
	 *
	 * @relates MutableDictionary
	 */
	MutableDictionary *(*init)(MutableDictionary *self);

	/**
	 * @brief Initializes this Dictionary with the specified capacity.
	 *
	 * @param capacity The initial capacity.
	 *
	 * @return The initialized Dictionary, or `NULL` on error.
	 *
	 * @relates MutableDictionary
	 */
	MutableDictionary *(*initWithCapacity)(MutableDictionary *self, size_t capacity);

	/**
	 * @brief Removes all Objects from this Dictionary.
	 *
	 * @relates MutableDictionary
	 */
	void (*removeAllObjects)(MutableDictionary *self);

	/**
	 * @brief Removes the specified Object from this Dictionary.
	 *
	 * @relates MutableDictionary
	 */
	void (*removeObjectForKey)(MutableDictionary *self, const id key);

	/**
	 * @brief Resizes this Dictionary, if necessary, based on its load factor.
	 *
	 * @private
	 */
	void (*resize)(MutableDictionary *self);

	/**
	 * @brief Sets a pair in this Dictionary.
	 *
	 * @relates MutableDictionary
	 */
	void (*setObjectForKey)(MutableDictionary *self, const id obj, const id key);

	/**
	 * @brief Sets pairs in this Dictionary from the NULL-terminated list.
	 *
	 * @relates MutableDictionary
	 */
	void (*setObjectsForKeys)(MutableDictionary *self, ...);
};

/**
 * @brief The MutableDictionary Class.
 */
extern Class __MutableDictionary;

#endif
