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

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "Object.h"
#include "String.h"

#define _Class _Object

#pragma mark - Object

/**
 * @fn Object *Object::copy(const Object *self)
 * @memberof Object
 */
static Object *copy(const Object *self) {

	ident obj = calloc(1, self->clazz->def.instanceSize);
	assert(obj);

	Object *object = memcpy(obj, self, self->clazz->def.instanceSize);
	object->referenceCount = 1;

	return object;
}

/**
 * @fn void Object::dealloc(Object *self)
 * @memberof Object
 */
static void dealloc(Object *self) {

	free(self);
}

/**
 * @fn String *Object::description(const Object *self)
 * @memberof Object
 */
static String *description(const Object *self) {

	return $(alloc(String), initWithFormat, "%s@%p", self->clazz->def.name, self);
}

/**
 * @fn int Object::hash(const Object *self)
 * @memberof Object
 */
static int hash(const Object *self) {

	uintptr_t addr = (uintptr_t) self;

	return (int) ((13 * addr) ^ (addr >> 15));
}

/**
 * @fn Object *Object::init(Object *self)
 * @memberof Object
 */
static Object *init(Object *self) {

	return self;
}

/**
 * @fn bool Object::isEqual(const Object *self, const Object *other)
 * @memberof Object
 */
static bool isEqual(const Object *self, const Object *other) {

	return self == other;
}

/**
 * @fn bool Object::isKindOfClass(const Object *self, const Class *clazz)
 * @memberof Object
 */
static bool isKindOfClass(const Object *self, const Class *clazz) {

	assert(clazz);
	
	const Class *c = self->clazz;
	while (c) {
		if (memcmp(c, clazz, sizeof(*c)) == 0) {
			return true;
		}
		c = c->def.superclass;
	}

	return false;
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	((ObjectInterface *) clazz->interface)->copy = copy;
	((ObjectInterface *) clazz->interface)->dealloc = dealloc;
	((ObjectInterface *) clazz->interface)->description = description;
	((ObjectInterface *) clazz->interface)->hash = hash;
	((ObjectInterface *) clazz->interface)->init = init;
	((ObjectInterface *) clazz->interface)->isEqual = isEqual;
	((ObjectInterface *) clazz->interface)->isKindOfClass = isKindOfClass;
}

/**
 * @fn Class *Object::_Object(void)
 * @memberof Object
 */
Class *_Object(void) {
	static Class *clazz;
	static Once once;

	do_once(&once, {
		clazz = _initialize(&(const ClassDef) {
			.name = "Object",
			.instanceSize = sizeof(Object),
			.interfaceOffset = offsetof(Object, interface),
			.interfaceSize = sizeof(ObjectInterface),
			.initialize = initialize,
		});
	});

	return clazz;
}

#undef _Class
