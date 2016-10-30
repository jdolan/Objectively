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

/**
 * @file
 * @brief Mutable arrays.
 */

typedef struct MutableArrayInterface MutableArrayInterface;

/**
 * @brief Mutable arrays.
 * @extends Array
 * @ingroup Collections
 */
struct MutableArray {

	/**
	 * @brief The parent.
	 */
	Array array;

	/**
	 * @brief The typed interface.
	 * @protected
	 */
	MutableArrayInterface *interface;

	/**
	 * @brief The Array capacity.
	 * @private
	 */
	size_t capacity;
};

/**
 * @brief The MutableArray interface.
 */
struct MutableArrayInterface {

	/**
	 * @brief The parent interface.
	 */
	ArrayInterface arrayInterface;

	/**
	 * @fn void MutableArray::addObject(MutableArray *self, const ident obj)
	 * @brief Adds the specified Object to this MutableArray.
	 * @param self The MutableArray.
	 * @param obj The Object to add.
	 * @memberof MutableArray
	 */
	void (*addObject)(MutableArray *self, const ident obj);

	/**
	 * @fn void MutableArray::addObjects(MutableArray *self, const ident obj, ...)
	 * @brief Adds the specified objects to this Array.
	 * @param self The MutableArray.
	 * @param obj The `NULL`-terminated list of objects.
	 * @memberof MutableArray
	 */
	void (*addObjects)(MutableArray *self, const ident obj, ...);

	/**
	 * @fn void MutableArray::addObjectsFromArray(MutableArray *self, const Array *array)
	 * @brief Adds the Objects contained in `array` to this MutableArray.
	 * @param self The MutableArray.
	 * @param array An Array.
	 * @memberof MutableArray
	 */
	void (*addObjectsFromArray)(MutableArray *self, const Array *array);
	
	/**
	 * @static
	 * @fn MutableArray *MutableArray::array(void)
	 * @brief Returns a new MutableArray.
	 * @return The new MutableArray, or `NULL` on error.
	 * @memberof MutableArray
	 */
	MutableArray *(*array)(void);
	
	/**
	 * @static
	 * @fn MutableArray *MutableArray::arrayWithCapacity(size_t capacity)
	 * @brief Returns a new MutableArray with the given `capacity`.
	 * @param capacity The desired initial capacity.
	 * @return The new MutableArray, or `NULL` on error.
	 * @memberof MutableArray
	 */
	MutableArray *(*arrayWithCapacity)(size_t capacity);

	/**
	 * @fn void MutableArray::filter(MutableArray *self, Predicate predicate, ident data)
	 * @brief Filters this MutableArray in place using `predicate`.
	 * @param self The MutableArray.
	 * @param predicate A Predicate.
	 * @param data User data.
	 * @memberof MutableArray
	 */
	void (*filter)(MutableArray *self, Predicate predicate, ident data);

	/**
	 * @fn MutableArray *MutableArray::init(MutableArray *self)
	 * @brief Initializes this MutableArray.
	 * @param self The MutableArray.
	 * @return The initialized MutableArray, or `NULL` on error.
	 * @memberof MutableArray
	 */
	MutableArray *(*init)(MutableArray *self);

	/**
	 * @fn MutableArray *MutableArray::initWithCapacity(MutableArray *self, size_t capacity)
	 * @brief Initializes this MutableArray with the specified capacity.
	 * @param self The MutableArray.
	 * @param capacity The desired initial capacity.
	 * @return The initialized Array, or `NULL` on error.
	 * @memberof MutableArray
	 */
	MutableArray *(*initWithCapacity)(MutableArray *self, size_t capacity);

	/**
	 * @fn void MutableArray::insertObjectAtIndex(MutableArray *self, ident obj, int index)
	 * @brief Inserts the Object at the specified index.
	 * @param self The MutableArray.
	 * @param obj The Object to insert.
	 * @param index The index at which to insert.
	 * @memberof MutableArray
	 */
	void (*insertObjectAtIndex)(MutableArray *self, ident obj, int index);

	/**
	 * @fn void MutableArray::removeAllObjects(MutableArray *self)
	 * @brief Removes all Objects from this MutableArray.
	 * @param self The MutableArray.
	 * @memberof MutableArray
	 */
	void (*removeAllObjects)(MutableArray *self);

	/**
	 * @fn void MutableArray::removeObject(MutableArray *self, const ident obj)
	 * @brief Removes the specified Object from this MutableArray.
	 * @param self The MutableArray.
	 * @param obj The Object to remove.
	 * @memberof MutableArray
	 */
	void (*removeObject)(MutableArray *self, const ident obj);

	/**
	 * @fn void MutableArray::removeObjectAtIndex(MutableArray *self, size_t index)
	 * @brief Removes the Object at the specified index.
	 * @param self The MutableArray.
	 * @param index The index of the Object to remove.
	 * @memberof MutableArray
	 */
	void (*removeObjectAtIndex)(MutableArray *self, size_t index);

	/**
	 * @fn void MutableArray::setObjectAtIndex(MutableArray *self, const ident obj, size_t index)
	 * @brief Replaces the Object at the specified index.
	 * @param self The MutableArray.
	 * @param obj The Object with which to replace.
	 * @param index The index of the Object to replace.
	 * @remarks The index must not exceed the size of the MutableArray.
	 * @memberof MutableArray
	 */
	void (*setObjectAtIndex)(MutableArray *self, const ident obj, size_t index);

	/**
	 * @fn void MutableArray::sort(MutableArray *self, Comparator comparator)
	 * @brief Sorts this MutableArray in place using `comparator`.
	 * @param self The MutableArray.
	 * @param comparator A Comparator.
	 * @memberof MutableArray
	 */
	void (*sort)(MutableArray *self, Comparator comparator);
};

/**
 * @brief The MutableArray Class.
 */
extern Class _MutableArray;
