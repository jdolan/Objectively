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

#ifndef _Objectively_Dictionary_h_
#define _Objectively_Dictionary_h_

#include <Objectively/Array.h>
#include <Objectively/Object.h>

/**
 * @file
 *
 * @brief Immutable key-value stores.
 */

typedef struct Dictionary Dictionary;
typedef struct DictionaryInterface DictionaryInterface;

/**
 * A function pointer for Dictionary enumeration (iteration).
 *
 * @param dictionary The Dictionary.
 * @param obj The Object for the current iteration.
 * @param key The key for the current iteration.
 * @param data User data.
 *
 * @return See the documentation for the enumeration methods.
 */
typedef BOOL (*DictionaryEnumerator)(const Dictionary *dictionary, id obj, id key, id data);

/**
 * @brief Immutable key-value stores.
 *
 * @extends Object
 *
 * @ingroup Collections
 */
struct Dictionary {

	/**
	 * @brief The parent.
	 */
	Object object;

	/**
	 * @brief The typed interface.
	 */
	DictionaryInterface *interface;

	/**
	 * The internal size (number of bins).
	 *
	 * @private
	 */
	size_t capacity;

	/**
	 * @brief The count of elements.
	 */
	size_t count;

	/**
	 * @brief The Dictionary elements.
	 *
	 * @private
	 */
	id *elements;
};

/**
 * @brief The Dictionary interface.
 *
 * @ingroup Collections
 */
struct DictionaryInterface {

	/**
	 * @brief The parent.
	 */
	ObjectInterface objectInterface;

	/**
	 * @return An Array containing all keys in this Dictionary.
	 *
	 * @relates Dictionary
	 */
	Array *(*allKeys)(const Dictionary *self);

	/**
	 * @return An Array containing all Objects in this Dictionary.
	 *
	 * @relates Dictionary
	 */
	Array *(*allObjects)(const Dictionary *self);

	/**
	 * @brief Enumerate the pairs of this Dictionary with the given function.
	 *
	 * @param enumerator The enumerator function.
	 * @param data User data.
	 *
	 * @remark The enumerator should return `YES` to break the iteration.
	 *
	 * @relates Dictionary
	 */
	void (*enumerateObjectsAndKeys)(const Dictionary *self, DictionaryEnumerator enumerator,
			id data);

	/**
	 * @brief Creates a new Dictionary with pairs that pass the filter function.
	 *
	 * @param enumerator The enumerator function.
	 * @param data User data.
	 *
	 * @return The new, filtered Dictionary.
	 *
	 * @relates Dictionary
	 */
	Dictionary *(*filterObjectsAndKeys)(const Dictionary *self, DictionaryEnumerator enumerator,
			id data);

	/**
	 * @brief Initializes this Dictionary to contain elements of `dictionary`.
	 *
	 * @param dictionary A Dictionary.
	 *
	 * @return The initialized Dictionary, or `NULL` on error.
	 *
	 * @relates Dictionary
	 */
	Dictionary *(*initWithDictionary)(Dictionary *self, const Dictionary *dictionary);

	/**
	 * @brief Initializes this Dictionary with the `NULL`-terminated list of
	 * Objects and keys.
	 *
	 * @return The initialized Dictionary, or `NULL` on error.
	 *
	 * @relates Dictionary
	 */
	Dictionary *(*initWithObjectsAndKeys)(Dictionary *self, ...);

	/**
	 * @return The Object stored at the specified key in this Dictionary.
	 *
	 * @relates Dictionary
	 */
	id (*objectForKey)(const Dictionary *self, const id key);
};

/**
 * @brief The Dictionary Class.
 */
extern Class __Dictionary;

#endif
