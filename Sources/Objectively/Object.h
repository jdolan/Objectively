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

#ifndef _Objectively_Object_h_
#define _Objectively_Object_h_

#include <Objectively/Class.h>
#include <Objectively/Types.h>

/**
 * @file
 *
 * @brief Object is the _root Class_ of the Objectively Class hierarchy.
 *
 * Every Class descends from Object, and every instance can be cast to Object.
 */

typedef struct ObjectInterface ObjectInterface;
typedef struct Object Object;

/**
 * @brief Object is the _root Class_ of the Objectively Class hierarchy.
 *
 * Every Class descends from Object, and every instance can be cast to Object.
 */
struct Object {

	/**
	 * @brief Every instance of Object begins with a pointer to its Class.
	 */
	Class *clazz;

	/**
	 * @brief The typed interface.
	 */
	ObjectInterface *interface;

	/**
	 * @brief The reference count of this Object.
	 */
	unsigned referenceCount;
};

typedef struct String String;

/**
 * @brief The Object interface.
 */
struct ObjectInterface {

	/**
	 * @brief Creates a shallow copy of this Object.
	 *
	 * @param self The instance.
	 *
	 * @return The copy.
	 *
	 * @relates Object
	 */
	Object *(*copy)(const Object *self);

	/**
	 * @brief Frees all resources held by this Object.
	 *
	 * @relates Object
	 */
	void (*dealloc)(Object *self);

	/**
	 * @return A brief description of this Object.
	 *
	 * @relates Object
	 */
	String *(*description)(const Object *self);

	/**
	 * @return An integer hash for use in hash tables, etc.
	 *
	 * @relates Object
	 */
	int (*hash)(const Object *self);

	/**
	 * @brief Initializes this Object.
	 *
	 * @return The initialized Object, or the unmodified pointer on error.
	 *
	 * @relates Object
	 */
	Object *(*init)(Object *self);

	/**
	 * @brief Tests equality of the other Object.
	 *
	 * @return YES if other is deemed equal, NO otherwise.
	 *
	 * @relates Object
	 */
	BOOL (*isEqual)(const Object *self, const Object *other);

	/**
	 * @brief Tests for class hierarchy membership.
	 *
	 * @return YES if this instance belongs to class' hierarchy, NO otherwise.
	 *
	 * @relates Object
	 */
	BOOL (*isKindOfClass)(const Object *self, const Class *clazz);
};

/**
 * @brief The Object Class.
 */
extern Class __Object;

#endif
