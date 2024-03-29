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

#include <stdarg.h>

#include <Objectively/Object.h>

/**
 * @file
 * @brief Immutable arrays.
 */

/**
 * @defgroup Collections Collections
 * @brief Abstract data types for aggregating Objects.
 */

typedef struct Array Array;
typedef struct ArrayInterface ArrayInterface;

/**
 * @brief A function pointer for Array enumeration (iteration).
 * @param array The Array.
 * @param obj The Object for the current iteration.
 * @param data User data.
 */
typedef void (*ArrayEnumerator)(const Array *array, ident obj, ident data);

/**
 * @brief Immutable arrays.
 * @extends Object
 * @ingroup Collections
 */
struct Array {

	/**
	 * @brief The superclass.
	 */
	Object object;

	/**
	 * @brief The interface.
	 * @protected
	 */
	ArrayInterface *interface;

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

typedef struct MutableArray MutableArray;

/**
 * @brief The Array interface.
 */
struct ArrayInterface {

	/**
	 * @brief The superclass interface.
	 */
	ObjectInterface objectInterface;

	/**
	 * @static
	 * @fn Array *Array::arrayWithArray(const Array *array)
	 * @brief Returns a new Array containing the contents of `array`.
	 * @param array An Array.
	 * @return The new Array, or `NULL` on error.
	 * @memberof Array
	 */
	Array *(*arrayWithArray)(const Array *array);

	/**
	 * @static
	 * @fn Array *Array::arrayWithObjects(ident obj, ...)
	 * @brief Returns a new Array containing the given Objects.
	 * @param obj The first in a `NULL`-terminated list of Objects.
	 * @return The new Array, or `NULL` on error.
	 * @memberof Array
	 */
	Array *(*arrayWithObjects)(ident obj, ...);

	/**
	 * @static
	 * @fn Array *Array::arrayWithVaList(va_list args)
	 * @brief Returns a new Array containing the Objects in the given `va_list`.
	 * @param args The `NULL`-terminated va_list of Objects.
	 * @return The new Array, or `NULL` on error.
	 * @memberof Array
	 */
	Array *(*arrayWithVaList)(va_list args);

	/**
	 * @fn String *Array::componentsJoinedByCharacters(const Array *self, const char *chars)
	 * @brief Returns the components of this Array joined by `chars`.
	 * @param self The Array.
	 * @param chars The joining characters.
	 * @return A String comprised of the components of this Array, joined by `chars`.
	 * @memberof Array
	 */
	String *(*componentsJoinedByCharacters)(const Array *self, const char *chars);

	/**
	 * @fn String *Array::componentsJoinedByString(const Array *self, const String *string)
	 * @brief Returns the components of this Array joined by the specified String.
	 * @param self The Array.
	 * @param string The joining String.
	 * @return A String comprised of the components of this Array, joined by `string`.
	 * @memberof Array
	 */
	String *(*componentsJoinedByString)(const Array *self, const String *string);

	/**
	 * @fn bool Array::containsObject(const Array *self, const ident obj)
	 * @param self The Array.
	 * @param obj An Object.
	 * @return `true` if this Array contains the given Object, `false` otherwise.
	 * @memberof Array
	 */
	bool (*containsObject)(const Array *self, const ident obj);

	/**
	 * @fn void Array::enumerateObjects(const Array *self, ArrayEnumerator enumerator, ident data)
	 * @brief Enumerate the elements of this Array with the given function.
	 * @param self The Array.
	 * @param enumerator The enumerator function.
	 * @param data User data.
	 * @memberof Array
	 */
	void (*enumerateObjects)(const Array *self, ArrayEnumerator enumerator, ident data);

	/**
	 * @fn Array *Array::filteredArray(const Array *self, Predicate predicate, ident data)
	 * @brief Creates a new Array with elements that pass `predicate`.
	 * @param self The Array.
	 * @param predicate The predicate function.
	 * @param data User data.
	 * @return The new, filtered Array.
	 * @memberof Array
	 */
	Array *(*filteredArray)(const Array *self, Predicate predicate, ident data);

	/**
	 * @fn ident Array::findObject(const Array *self, Predicate predicate, ident data)
	 * @param self The Array.
	 * @param predicate The predicate function.
	 * @param data User data.
	 * @return The first element of this Array to pass the predicate function.
	 * @memberof Array
	 */
	ident (*findObject)(const Array *self, Predicate predicate, ident data);

	/**
	 * @fn ident Array::firstObject(const Array *self)
	 * @param self The Array.
	 * @return The first Object in this Array, or `NULL` if empty.
	 * @memberof Array
	 */
	ident (*firstObject)(const Array *self);

	/**
	 * @fn ssize_t Array::indexOfObject(const Array *self, const ident obj)
	 * @param self The Array.
	 * @param obj An Object.
	 * @return The index of the given Object, or `-1` if not found.
	 * @memberof Array
	 */
	ssize_t (*indexOfObject)(const Array *self, const ident obj);

	/**
	 * @fn Array *Array::initWithArray(Array *self, const Array *array)
	 * @brief Initializes this Array to contain the Objects in `array`.
	 * @param self The Array.
	 * @param array An Array.
	 * @return The initialized Array, or `NULL` on error.
	 * @memberof Array
	 */
	Array *(*initWithArray)(Array *self, const Array *array);

	/**
	 * @fn Array *Array::initWithObjects(Array *self, ...)
	 * @brief Initializes this Array to contain the Objects in the `NULL`-terminated arguments list.
	 * @param self The Array.
	 * @return The initialized Array, or `NULL` on error.
	 * @memberof Array
	 */
	Array *(*initWithObjects)(Array *self, ...);

	/**
	 * @fn Array *Array::initWithVaList(Array *self, va_list args)
	 * @brief Initializes this Array to contain the Objects in the `NULL`-terminated va_list.
	 * @param self The Array.
	 * @param args The `NULL`-terminated va_list of Objects.
	 * @return The initialized Array, or `NULL` on error.
	 * @memberof Array
	 */
	Array *(*initWithVaList)(Array *self, va_list args);

	/**
	 * @fn ident Array::lastObject(const Array *self)
	 * @param self The Array.
	 * @return The last Object in this Array, or `NULL` if empty.
	 * @memberof Array
	 */
	ident (*lastObject)(const Array *self);

	/**
	 * @fn Array *Array::mappedArray(const Array *self, Functor functor, ident data)
	 * @brief Transforms the elements in this Array by `functor`.
	 * @param self The Array.
	 * @param functor The Functor.
	 * @param data User data.
	 * @return An Array containing the transformed elements of this Array.
	 * @memberof Array
	 */
	Array *(*mappedArray)(const Array *self, Functor functor, ident data);

	/**
	 * @fn MutableArray *Array::mutableCopy(const Array *self)
	 * @param self The Array.
	 * @return A MutableArray with the contents of this Array.
	 * @memberof Array
	 */
	MutableArray *(*mutableCopy)(const Array *self);

	/**
	 * @fn ident Array::objectAtIndex(const Array *self, int index)
	 * @param self The Array.
	 * @param index The index of the desired Object.
	 * @return The Object at the specified index.
	 * @memberof Array
	 */
	ident (*objectAtIndex)(const Array *self, size_t index);

	/**
	 * @fn ident Array::reduce(const Array *self, Reducer reducer, ident accumulator, ident data)
	 * @param self The Array.
	 * @param reducer The Reducer.
	 * @param accumulator The initial accumulator value.
	 * @param data User data.
	 * @return The reduction result.
	 * @memberof Array
	 */
	ident (*reduce)(const Array *self, Reducer reducer, ident accumulator, ident data);

	/**
	 * @fn Array *Array::sortedArray(const Array *self, Comparator comparator)
	 * @param self The Array.
	 * @param comparator The Comparator
	 * @return A copy of this Array, sorted by the given Comparator.
	 * @memberof Array
	 */
	Array *(*sortedArray)(const Array *self, Comparator comparator);
};

/**
 * @fn Class *Array::_Array(void)
 * @brief The Array archetype.
 * @return The Array Class.
 * @memberof Array
 */
OBJECTIVELY_EXPORT Class *_Array(void);
