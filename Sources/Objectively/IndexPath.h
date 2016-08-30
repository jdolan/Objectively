/*
 * ObjectivelyMVC: MVC framework for OpenGL and SDL2 in c.
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
 * @brief The IndexPath type.
 */

typedef struct IndexPath IndexPath;
typedef struct IndexPathInterface IndexPathInterface;

/**
 * @brief Index paths represent the path to an element or node within a tree or graph structure.
 *
 * @extends Object
 *
 * @ingroup
 */
struct IndexPath {
	
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
	IndexPathInterface *interface;

	/**
	 * @brief The indexes.
	 */
	int *indexes;

	/**
	 * @brief The length of `indexes`.
	 */
	size_t length;
};

/**
 * @brief The IndexPath interface.
 */
struct IndexPathInterface {
	
	/**
	 * @brief The parent interface.
	 */
	ObjectInterface objectInterface;

	/**
	 * @fn int IndexPath::indexAtPosition(const IndexPath *self, int position)
	 *
	 * @param position The index position.
	 *
	 * @return The index at the given position.
	 *
	 * @memberof IndexPath
	 */
	int (*indexAtPosition)(const IndexPath *self, int position);

	/**
	 * @fn IndexPath *IndexPath::initWithIndex(IndexPath *self, int index)
	 *
	 * @brief Initializes this IndexPath with the specified index.
	 *
	 * @param indexes The index.
	 *
	 * @return The intialized IndexPath, or `NULL` on error.
	 *
	 * @memberof IndexPath
	 */
	IndexPath *(*initWithIndex)(IndexPath *self, int index);

	/**
	 * @fn IndexPath *IndexPath::initWithIndexes(IndexPath *self, int *indexes, size_t length)
	 *
	 * @brief Initializes this IndexPath with the specified indexes and length.
	 *
	 * @param indexes The indexes.
	 * @param length The length of `indexes`.
	 *
	 * @return The intialized IndexPath, or `NULL` on error.
	 *
	 * @memberof IndexPath
	 */
	IndexPath *(*initWithIndexes)(IndexPath *self, int *indexes, size_t length);
};

/**
 * @brief The IndexPath Class.
 */
extern Class _IndexPath;

