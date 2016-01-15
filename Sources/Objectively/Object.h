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

/**
 * @defgroup Core Core
 *
 * @brief Objectively core.
 */
typedef struct Object Object;
typedef struct ObjectInterface ObjectInterface;

/**
 * @brief Object is the _root Class_ of the Objectively Class hierarchy.
 *
 * Every Class descends from Object, and every instance can be cast to Object.
 *
 * @ingroup Core
 */
struct Object {

	/**
	 * @brief Every instance of Object begins with a pointer to its Class.
	 */
	Class *clazz;

	/**
	 * @brief The typed interface.
	 *
	 * @private
	 */
	ObjectInterface *interface;

	/**
	 * @brief The reference count of this Object.
	 *
	 * @private
	 */
	unsigned referenceCount;
};

typedef struct String String;

/**
 * @brief The Object interface.
 */
struct ObjectInterface {

	/**
	 * @fn Object *Object::copy(const Object *self)
	 *
	 * @brief Creates a shallow copy of this Object.
	 *
	 * @param self The object.
	 *
	 * @return The copy.
	 *
	 * @memberof Object
	 */
	Object *(*copy)(const Object *self);

	/**
	 * @fn void Object::dealloc(Object *self)
	 *
	 * @brief Frees all resources held by this Object.
	 *
	 * @param self The object.
	 *
	 * @memberof Object
	 */
	void (*dealloc)(Object *self);

	/**
	 * @fn String *Object::description(const Object *self)
	 *
	 * @param self The object.
	 *
	 * @return A brief description of this Object.
	 *
	 * @memberof Object
	 */
	String *(*description)(const Object *self);

	/**
	 * @fn int Object::hash(const Object *self)
	 *
	 * @param self The object.
	 *
	 * @return An integer hash for use in hash tables, etc.
	 *
	 * @memberof Object
	 */
	int (*hash)(const Object *self);

	/**
	 * @fn Object *Object::init(Object *self)
	 *
	 * @brief Initializes this Object.
	 *
	 * @param self The uninitialized object.
	 *
	 * @return The initialized Object, or the unmodified pointer on error.
	 *
	 * @memberof Object
	 */
	Object *(*init)(Object *self);

	/**
	 * @fn _Bool Object::isEqual(const Object *self, const Object *other)
	 *
	 * @brief Tests equality of the other Object.
	 *
	 * @param self The object.
	 * @param other The object to test for equality.
	 *
	 * @return true if other is deemed equal, false otherwise.
	 *
	 * @memberof Object
	 */
	_Bool (*isEqual)(const Object *self, const Object *other);

	/**
	 * @fn _Bool Object::isKindOfClass(const Object *self, const Class *clazz)
	 *
	 * @brief Tests for class hierarchy membership.
	 *
	 * @param self The object.
	 * @param clazz The Class to test for membership.
	 *
	 * @return true if this instance belongs to class' hierarchy, false otherwise.
	 *
	 * @memberof Object
	 */
	_Bool (*isKindOfClass)(const Object *self, const Class *clazz);
};

/**
 * @brief The Object Class.
 */
extern Class _Object;

#endif
