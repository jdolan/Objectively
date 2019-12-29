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
 * @brief Mutable contiguous storage for C types.
 */

typedef struct Vector Vector;
typedef struct VectorInterface VectorInterface;

/**
 * @brief Vectors.
 * @extends Object
 * @ingroup Collections
 */
struct Vector {

	/**
	 * @brief The superclass.
	 */
	Object object;

	/**
	 * @brief The interface.
	 * @protected
	 */
	VectorInterface *interface;

	/**
	 * @brief The capacity.
	 */
	size_t capacity;

	/**
	 * @brief The count of elements.
	 */
	size_t count;

	/**
	 * @brief The size of each element.
	 */
	size_t size;

	/**
	 * @brief The elements.
	 */
	ident elements;
};

typedef struct MutableVector MutableVector;

/**
 * @brief Access the typed element of vector at the specified index.
 */
#define VectorElement(vector, type, index) \
	(((type *) vector->elements) + (index))

/**
 * @brief The Vector interface.
 */
struct VectorInterface {

	/**
	 * @brief The superclass interface.
	 */
	ObjectInterface objectInterface;

	/**
	 * @fn void Vector::addElement(Vector *self, const ident element)
	 * @brief Adds the specified element to this Vector.
	 * @param self The Vector.
	 * @param element The element to add.
	 * @memberof Vector
	 */
	void (*addElement)(Vector *self, const ident element);

	/**
	 * @fn ssize_t Vector::indexOfElement(const Vector *self, const ident element)
	 * @param self The Vector.
	 * @param element The element.
	 * @return The index of the given element, or `-1` if not found.
	 * @memberof Vector
	 */
	ssize_t (*indexOfElement)(const Vector *self, const ident element);

	/**
	 * @fn Vector *Vector::initWithElements(Vector *self, size_t size, size_t count, ident elements)
	 * @brief Initializes this Vector with the specified elements.
	 * @param self The Vector.
	 * @param size The element size.
	 * @param count The count of elements.
	 * @param elements The elements, which will be freed when this Vector is released.
	 * @return The initialized Vector, or `NULL` on error.
	 * @memberof Vector
	 */
	Vector *(*initWithElements)(Vector *self, size_t size, size_t count, ident elements);

	/**
	 * @fn Vector *Vector::initWithSize(Vector *self, size_t size)
	 * @brief Initializes this Vector with the specified element size.
	 * @param self The Vector.
	 * @param size The element size.
	 * @return The initialized Vector, or `NULL` on error.
	 * @memberof Vector
	 */
	Vector *(*initWithSize)(Vector *self, size_t size);

	/**
	 * @fn void Vector::insertElementAtIndex(Vector *self, const ident element, size_t index)
	 * @brief Inserts the element at the specified index.
	 * @param self The Vector.
	 * @param element The element to insert.
	 * @param index The index at which to insert.
	 * @memberof Vector
	 */
	void (*insertElementAtIndex)(Vector *self, const ident element, size_t index);

	/**
	 * @fn void Vector::removeElementAtIndex(Vector *self, size_t index)
	 * @brief Removes the element at the specified index.
	 * @param self The Vector.
	 * @param index The index of the element to remove.
	 * @memberof Vector
	 */
	void (*removeElementAtIndex)(Vector *self, size_t index);
};

/**
 * @fn Class *Vector::_Vector(void)
 * @brief The Vector archetype.
 * @return The Vector Class.
 * @memberof Vector
 */
OBJECTIVELY_EXPORT Class *_Vector(void);
