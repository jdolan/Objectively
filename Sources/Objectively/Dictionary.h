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

#include <Objectively/Array.h>
#include <Objectively/Object.h>

/**
 * @file
 * @brief Immutable key-value stores.
 */

typedef struct Dictionary Dictionary;
typedef struct DictionaryInterface DictionaryInterface;

/**
 * A function pointer for Dictionary enumeration (iteration).
 * @param dictionary The Dictionary.
 * @param obj The Object for the current iteration.
 * @param key The key for the current iteration.
 * @param data User data.
 * @return See the documentation for the enumeration methods.
 */
typedef _Bool (*DictionaryEnumerator)(const Dictionary *dictionary, ident obj, ident key, ident data);

/**
 * @brief Immutable key-value stores.
 * @extends Object
 * @ingroup Collections
 */
struct Dictionary {

	/**
	 * @brief The parent.
	 * @private
	 */
	Object object;

	/**
	 * @brief The typed interface.
	 * @private
	 */
	DictionaryInterface *interface;

	/**
	 * @brief The internal size (number of bins).
	 * @private
	 */
	size_t capacity;

	/**
	 * @brief The count of elements.
	 */
	size_t count;

	/**
	 * @brief The elements.
	 * @private
	 */
	ident *elements;
};

typedef struct MutableDictionary MutableDictionary;

/**
 * @brief The Dictionary interface.
 */
struct DictionaryInterface {

	/**
	 * @brief The parent interface.
	 */
	ObjectInterface objectInterface;
	
	/**
	 * @fn Array *Dictionary::allKeys(const Dictionary *self)
	 * @param self The Dictionary.
	 * @return An Array containing all keys in this Dictionary.
	 * @memberof Dictionary
	 */
	Array *(*allKeys)(const Dictionary *self);

	/**
	 * @fn Array *Dictionary::allObjects(const Dictionary *self)
	 * @param self The Dictionary.
	 * @return An Array containing all Objects in this Dictionary.
	 * @memberof Dictionary
	 */
	Array *(*allObjects)(const Dictionary *self);
	
	/**
	 * @static
	 * @fn Dictionary *Dictionary::dictionaryWithDictionary(const Dictionary *dictionary)
	 * @brief Returns a new Dictionary containing all pairs from `dictionary`.
	 * @param dictionary A Dictionary.
	 * @return The new Dictionary, or `NULL` on error.
	 * @memberof Dictionary
	 */
	Dictionary *(*dictionaryWithDictionary)(const Dictionary *dictionary);
	
	/**
	 * @static
	 * @fn Dictionary *Dictionary::dictionaryWithObjectsAndKeys(ident obj, ...)
	 * @brief Returns a new Dictionary containing pairs from the given arguments.
	 * @param obj The first in a `NULL`-terminated list of Objects and keys.
	 * @return The new Dictionary, or `NULL` on error.
	 * @memberof Dictionary
	 */
	Dictionary *(*dictionaryWithObjectsAndKeys)(ident obj, ...);

	/**
	 * @fn void Dictionary::enumerateObjectsAndKeys(const Dictionary *self, DictionaryEnumerator enumerator, ident data)
	 * @brief Enumerate the pairs of this Dictionary with the given function.
	 * @param self The Dictionary.
	 * @param enumerator The enumerator function.
	 * @param data User data.
	 * @remarks The enumerator should return `true` to break the iteration.
	 * @memberof Dictionary
	 */
	void (*enumerateObjectsAndKeys)(const Dictionary *self, DictionaryEnumerator enumerator, ident data);

	/**
	 * @fn void Dictionary::filterObjectsAndKeys(const Dictionary *self, DictionaryEnumerator enumerator, ident data)
	 * @brief Creates a new Dictionary with pairs that pass the filter function.
	 * @param self The Dictionary.
	 * @param enumerator The enumerator function.
	 * @param data User data.
	 * @return The new, filtered Dictionary.
	 * @memberof Dictionary
	 */
	Dictionary *(*filterObjectsAndKeys)(const Dictionary *self, DictionaryEnumerator enumerator, ident data);

	/**
	 * @fn Dictionary *Dictionary::initWithDictionary(Dictionary *self, const Dictionary *dictionary)
	 * @brief Initializes this Dictionary to contain elements of `dictionary`.
	 * @param self The Dictionary.
	 * @param dictionary A Dictionary.
	 * @return The initialized Dictionary, or `NULL` on error.
	 * @memberof Dictionary
	 */
	Dictionary *(*initWithDictionary)(Dictionary *self, const Dictionary *dictionary);

	/**
	 * @fn Dictionary *Dictionary::initWithObjectsAndKeys(Dictionary *self, ...)
	 * @brief Initializes this Dictionary with the `NULL`-terminated list of Objects and keys.
	 * @param self The Dictionary.
	 * @return The initialized Dictionary, or `NULL` on error.
	 * @memberof Dictionary
	 */
	Dictionary *(*initWithObjectsAndKeys)(Dictionary *self, ...);

	/**
	 * @fn MutableDictionary *Dictionary::mutableCopy(const Dictionary *self)
	 * @param self The Dictionary.
	 * @return A MutableDictionary with the contents of this Dictionary.
	 * @memberof Dictionary
	 */
	MutableDictionary *(*mutableCopy)(const Dictionary *self);

	/**
	 * @fn ident Dictionary::objectForKey(const Dictionary *self, const ident key)
	 * @param self The Dictionary.
	 * @param key The key.
	 * @return The Object stored at the specified key in this Dictionary.
	 * @memberof Dictionary
	 */
	ident (*objectForKey)(const Dictionary *self, const ident key);

	/**
	 * @fn ident Dictionary::objectForKeyPath(const Dictionary *self, const char *path)
	 * @param self The Dictionary.
	 * @param path The key path.
	 * @return The Object stored at the specified key path in this Dictionary.
	 * @memberof Dictionary
	 */
	ident (*objectForKeyPath)(const Dictionary *self, const char *path);
};

/**
 * @brief The Dictionary Class.
 */
extern Class _Dictionary;
