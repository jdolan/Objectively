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
 * @brief Immutable sets.
 */

/**
 * @defgroup Collections Collections
 * @brief Abstract data types for aggregating Objects.
 */

typedef struct Set Set;
typedef struct SetInterface SetInterface;

/**
 * @brief A function pointer for Set enumeration (iteration).
 * @param set The Set.
 * @param obj The Object for the current iteration.
 * @param data User data.
 */
typedef void (*SetEnumerator)(const Set *set, ident obj, ident data);

/**
 * @brief Immutable sets.
 * @extends Object
 * @ingroup Collections
 */
struct Set {

	/**
	 * @brief The superclass.
	 */
	Object object;

	/**
	 * @brief The interface.
	 * @protected
	 */
	SetInterface *interface;

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

typedef struct MutableSet MutableSet;

/**
 * @brief The Set interface.
 */
struct SetInterface {

	/**
	 * @brief The superclass interface.
	 */
	ObjectInterface objectInterface;

	/**
	 * @fn Array *Set::allObjects(const Set *self)
	 * @param self The Set.
	 * @return An Array containing all Objects in this Set.
	 * @memberof Set
	 */
	Array *(*allObjects)(const Set *self);

	/**
	 * @fn bool Set::containsObject(const Set *self, const ident obj)
	 * @param self The Set.
	 * @param obj The Object to check.
	 * @return `true` if this Set contains the given Object, `false` otherwise.
	 * @memberof Set
	 */
	bool (*containsObject)(const Set *self, const ident obj);

	/**
	 * @fn bool Set::containsObjectMatching(const Set *self, Predicate predicate, ident data)
	 * @param self The Set.
	 * @param predicate The predicate function.
	 * @param data User data.
	 * @return `true` if this Set contains an Object matching `predicate`, `false` otherwise.
	 * @memberof Set
	 */
	bool (*containsObjectMatching)(const Set *self, Predicate predicate, ident data);

	/**
	 * @fn void Set::enumerateObjects(const Set *self, SetEnumerator enumerator, ident data)
	 * @brief Enumerate the elements of this Set with the given function.
	 * @param self The Set.
	 * @param enumerator The enumerator function.
	 * @param data User data.
	 * @remarks The enumerator should return `true` to break the iteration.
	 * @memberof Set
	 */
	void (*enumerateObjects)(const Set *self, SetEnumerator enumerator, ident data);

	/**
	 * @fn Set *Set::filteredSet(const Set *self, Predicate predicate, ident data)
	 * @brief Creates a new Set with elements that pass `predicate`.
	 * @param self The Set.
	 * @param predicate The predicate function.
	 * @param data User data.
	 * @return The new, filtered Set.
	 * @memberof Set
	 */
	Set *(*filteredSet)(const Set *self, Predicate predicate, ident data);

	/**
	 * @fn Set *Set::initWithArray(Set *self, const Array *array)
	 * @brief Initializes this Set to contain the Objects in `array`.
	 * @param self The Set.
	 * @param array An Array.
	 * @return The initialized Set, or `NULL` on error.
	 * @memberof Set
	 */
	Set *(*initWithArray)(Set *self, const Array *array);

	/**
	 * @fn Set *Set::initWithObjects(Set *self, ...)
	 * @brief Initializes this Set with the specified objects.
	 * @param self The Set.
	 * @return The initialized Set, or `NULL` on error.
	 * @memberof Set
	 */
	Set *(*initWithObjects)(Set *self, ...);

	/**
	 * @fn Set *Set::initWithSet(Set *self, const Set *set)
	 * @brief Initializes this Set to contain the Objects in `set`.
	 * @param self The Set.
	 * @param set A Set.
	 * @return The initialized Set, or `NULL` on error.
	 * @memberof Set
	 */
	Set *(*initWithSet)(Set *self, const Set *set);

	/**
	 * @fn Set *Set::mappedSet(const Set *self, Functor functor, ident data)
	 * @brief Transforms the elements in this Set by `functor`.
	 * @param self The Set.
	 * @param functor The Functor.
	 * @param data User data.
	 * @return A Set containing the transformed elements of this Set.
	 * @memberof Set
	 */
	Set *(*mappedSet)(const Set *self, Functor functor, ident data);

	/**
	 * @fn MutableSet *Set::mutableCopy(const Set *self)
	 * @param self The Set.
	 * @return A MutableSet with the contents of this Set.
	 * @memberof Set
	 */
	MutableSet *(*mutableCopy)(const Set *self);

	/**
	 * @fn ident Set::reduce(const Set *self, Reducer reducer, ident accumulator, ident data)
	 * @param self The Set.
	 * @param reducer The Reducer.
	 * @param accumulator The initial accumulator value.
	 * @param data User data.
	 * @return The reduction result.
	 * @memberof Set
	 */
	ident (*reduce)(const Set *self, Reducer reducer, ident accumulator, ident data);

	/**
	 * @static
	 * @fn Set *Set::setWithArray(const Array *array)
	 * @brief Returns a new Set with the contents of `array`.
	 * @param array An Array.
	 * @return The new Set, or `NULL` on error.
	 * @memberof Set
	 */
	Set *(*setWithArray)(const Array *array);

	/**
	 * @static
	 * @fn Set *Set::setWithObjects(ident obj, ...)
	 * @brief Returns a new Set containing the specified Objects.
	 * @param obj A `NULL`-terminated list of Objects.
	 * @return The new Set, or `NULL` on error.
	 * @memberof Set
	 */
	Set *(*setWithObjects)(ident obj, ...);

	/**
	 * @static
	 * @fn Set *Set::setWithSet(const Set *set)
	 * @brief Returns a new Set with the contents of `set`.
	 * @param set A set.
	 * @return The new Set, or `NULL` on error.
	 * @memberof Set
	 */
	Set *(*setWithSet)(const Set *set);
};

/**
 * @fn Class *Set::_Set(void)
 * @brief The Set archetype.
 * @return The Set Class.
 * @memberof Set
 */
OBJECTIVELY_EXPORT Class *_Set(void);
