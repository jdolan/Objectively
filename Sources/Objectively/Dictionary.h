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
 * @brief A mutable dictionary implementation.
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
 * @brief The Dictionary type.
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
	 * @brief The capacity, or number of bins, in this Dictionary.
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

	/**
	 * @brief The initial capacity, or number of bins, in this Dictionary.
	 */
	size_t initialCapacity;
};

/**
 * @brief The Dictionary interface.
 */
struct DictionaryInterface {

	/**
	 * @brief The parent.
	 */
	ObjectInterface objectInterface;

	/**
	 * @return An Array containing all keys in this Dictionary.
	 */
	Array *(*allKeys)(const Dictionary *self);

	/**
	 * @return An Array containing all Objects in this Dictionary.
	 */
	Array *(*allObjects)(const Dictionary *self);

	/**
	 * @brief Enumerate the pairs of this Dictionary with the given function.
	 *
	 * @param enumerator The enumerator function.
	 * @param data User data.
	 *
	 * @remark The enumerator should return `YES` to break the iteration.
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
	 */
	Dictionary *(*filterObjectsAndKeys)(const Dictionary *self, DictionaryEnumerator enumerator,
			id data);

	/**
	 * Initializes this Dictionary.
	 *
	 * @return The initialized Dictionary, or `NULL` on error.
	 */
	Dictionary *(*init)(Dictionary *self);

	/**
	 * Initializes this Dictionary with the specified capacity.
	 *
	 * @param capacity The initial capacity.
	 *
	 * @return The initialized Dictionary, or `NULL` on error.
	 */
	Dictionary *(*initWithCapacity)(Dictionary *self, size_t capacity);

	/**
	 * @return The Object stored at the specified key in this Dictionary.
	 */
	id (*objectForKey)(const Dictionary *self, const id key);

	/**
	 * @brief Removes all Objects from this Dictionary.
	 */
	void (*removeAllObjects)(Dictionary *self);

	/**
	 * @brief Removes the specified Object from this Dictionary.
	 */
	void (*removeObjectForKey)(Dictionary *self, const id key);

	/**
	 * @brief Sets a pair in this Dictionary.
	 */
	void (*setObjectForKey)(Dictionary *self, const id obj, const id key);

	/**
	 * @brief Sets pairs in this Dictionary from the NULL-terminated list.
	 */
	void (*setObjectsForKeys)(Dictionary *self, ...);
};

/**
 * @brief The Dictionary Class.
 */
extern Class __Dictionary;

#endif
