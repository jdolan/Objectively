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

#include <Objectively/Object.h>

/**
 * @file
 *
 * @brief Immutable arrays.
 */

/**
 * @defgroup Collections Collections
 *
 * @brief Abstract data types for aggregating Objects.
 */

typedef struct Array Array;
typedef struct ArrayInterface ArrayInterface;

/**
 * @brief A function pointer for Array enumeration (iteration).
 *
 * @param array The Array.
 * @param obj The Object for the current iteration.
 * @param data User data.
 *
 * @return See the documentation for the enumeration methods.
 */
typedef _Bool (*ArrayEnumerator)(const Array *array, ident obj, ident data);

/**
 * @brief Immutable arrays.
 *
 * @extends Object
 *
 * @ingroup Collections
 */
struct Array {

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
	ArrayInterface *interface;

	/**
	 * @brief The count of elements.
	 */
	size_t count;

	/**
	 * @brief The elements.
	 *
	 * @private
	 */
	ident *elements;
};

typedef struct MutableArray MutableArray;

/**
 * @brief The Array interface.
 */
struct ArrayInterface {

	/**
	 * @brief The parent interface.
	 */
	ObjectInterface objectInterface;

	/**
	 * @static
	 *
	 * @fn Array *Array::arrayWithArray(const Array *array)
	 *
	 * @brief Returns a new Array containing the contents of `array`.
	 *
	 * @param array An Array.
	 *
	 * @return The new Array, or `NULL` on error.
	 *
	 * @memberof Array
	 */
	Array *(*arrayWithArray)(const Array *array);

	/**
	 * @static
	 *
	 * @fn Array *Array::arrayWithObjects(ident obj, ...)
	 *
	 * @brief Returns a new Array containing the given Objects.
	 *
	 * @param obj The first in a `NULL`-terminated sequence of Objects.
	 *
	 * @return The new Array, or `NULL` on error.
	 *
	 * @memberof Array
	 */
	Array *(*arrayWithObjects)(ident obj, ...);

	/**
	 * @fn String *Array::componentsJoinedByCharacters(const Array *self, const char *chars)
	 *
	 * @brief Returns the components of this Array joined by `chars`.
	 *
	 * @param chars The joining characters.
	 *
	 * @return A String comprised of the components of this Array, joined by `chars`.
	 *
	 * @memberof Array
	 */
	String *(*componentsJoinedByCharacters)(const Array *self, const char *chars);

	/**
	 * @fn String *Array::componentsJoinedByString(const Array *self, const String *string)
	 *
	 * @brief Returns the components of this Array joined by the specified String.
	 *
	 * @param string The joining String.
	 *
	 * @return A String comprised of the components of this Array, joined by `string`.
	 *
	 * @memberof Array
	 */
	String *(*componentsJoinedByString)(const Array *self, const String *string);

	/**
	 * @fn _Bool Array::containsObject(const Array *self, const ident obj)
	 *
	 * @param self The Array.
	 * @param obj An Object.
	 *
	 * @return `true` if this Array contains the given Object, `false` otherwise.
	 *
	 * @memberof Array
	 */
	_Bool (*containsObject)(const Array *self, const ident obj);

	/**
	 * @fn void Array::enumerateObjects(const Array *self, ArrayEnumerator enumerator, ident data)
	 *
	 * @brief Enumerate the elements of this Array with the given function.
	 *
	 * @param self The Array.
	 * @param enumerator The enumerator function.
	 * @param data User data.
	 *
	 * @remarks The enumerator should return `true` to break the iteration.
	 *
	 * @memberof Array
	 */
	void (*enumerateObjects)(const Array *self, ArrayEnumerator enumerator, ident data);

	/**
	 * @fn void Array::filteredArray(const Array *self, Predicate predicate, ident data)
	 *
	 * @brief Creates a new Array with elements that pass `predicate`.
	 *
	 * @param self The Array.
	 * @param predicate The predicate function.
	 * @param data User data.
	 *
	 * @return The new, filtered Array.
	 *
	 * @memberof Array
	 */
	Array *(*filteredArray)(const Array *self, Predicate predicate, ident data);
	
	/**
	 * @fn ident Array::findObject(const Array *self, Predicate predicate, ident data)
	 *
	 * @param predicate The predicate function.
	 * @param data User data.
	 *
	 * @return The first element of this Array to pass the predicate function.
	 *
	 * @memberof Array
	 */
	ident (*findObject)(const Array *self, Predicate predicate, ident data);
	
	/**
	 * @fn ident Array::firstObject(const Array *self)
	 *
	 * @return The first Object in this Array, or `NULL` if empty.
	 *
	 * @memberof Array
	 */
	ident (*firstObject)(const Array *self);
	
	/**
	 * @fn int Array::indexOfObject(const Array *self, const ident obj)
	 *
	 * @param self The Array.
	 * @param obj An Object.
	 *
	 * @return The index of the given Object, or `-1` if not found.
	 *
	 * @memberof Array
	 */
	int (*indexOfObject)(const Array *self, const ident obj);

	/**
	 * @fn Array *Array::initWithArray(Array *self, const Array *array)
	 *
	 * @brief Initializes this Array to contain the Objects in `array`.
	 *
	 * @param self The Array.
	 * @param array An Array.
	 *
	 * @return The initialized Array, or `NULL` on error.
	 *
	 * @memberof Array
	 */
	Array *(*initWithArray)(Array *self, const Array *array);

	/**
	 * @fn Array *Array::initWithObjects(Array *self, ...)
	 *
	 * @brief Initializes this Array with the specified objects.
	 *
	 * @param self The uninitialized Array.
	 *
	 * @return The initialized Array, or `NULL` on error.
	 *
	 * @memberof Array
	 */
	Array *(*initWithObjects)(Array *self, ...);

	/**
	 * @fn ident Array::lastObject(const Array *self)
	 *
	 * @return The last Object in this Array, or `NULL` if empty.
	 *
	 * @memberof Array
	 */
	ident (*lastObject)(const Array *self);
	
	/**
	 * @fn MutableArray *Array::mutableCopy(const Array *self)
	 *
	 * @param self The Array.
	 *
	 * @return A MutableArray with the contents of this Array.
	 *
	 * @memberof Array
	 */
	MutableArray *(*mutableCopy)(const Array *self);

	/**
	 * @fn ident Array::objectAtIndex(const Array *self, int index)
	 *
	 * @param self The Array.
	 * @param index The index of the desired Object.
	 *
	 * @return The Object at the specified index.
	 *
	 * @memberof Array
	 */
	ident (*objectAtIndex)(const Array *self, int index);

	/**
	 * @fn Array *Array::sortedArray(const Array *self, Comparator comparator)
	 *
	 * @param comparator The Comparator
	 *
	 * @return A copy of this Array, sorted by the given Comparator.
	 *
	 * @memberof Array
	 */
	Array *(*sortedArray)(const Array *self, Comparator comparator);
};

/**
 * @brief The Array Class.
 */
extern Class _Array;
