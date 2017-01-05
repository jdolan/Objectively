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

#include <Objectively/Object.h>
#include <Objectively/String.h>

#define _Class _Object

#pragma mark - Object

/**
 * @fn Object *Object::copy(const Object *self)
 * @memberof Object
 */
static Object *copy(const Object *self) {

	ident obj = calloc(1, self->clazz->instanceSize);
	assert(obj);

	Object *object = memcpy(obj, self, self->clazz->instanceSize);
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

	return $(alloc(String), initWithFormat, "%s@%p", self->clazz->name, self);
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
 * @fn _Bool Object::isEqual(const Object *self, const Object *other)
 * @memberof Object
 */
static _Bool isEqual(const Object *self, const Object *other) {

	return self == other;
}

/**
 * @fn _Bool Object::isKindOfClass(const Object *self, const Class *clazz)
 * @memberof Object
 */
static _Bool isKindOfClass(const Object *self, const Class *clazz) {

	const Class *c = self->clazz;
	while (c) {
		if (c == clazz) {
			return true;
		}
		c = c->superclass;
	}

	return false;
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	ObjectInterface *object = (ObjectInterface *) clazz->def->interface;

	object->copy = copy;
	object->dealloc = dealloc;
	object->description = description;
	object->hash = hash;
	object->init = init;
	object->isEqual = isEqual;
	object->isKindOfClass = isKindOfClass;
}

Class *_Object(void) {
	static Class clazz;
	
	do_once({
		clazz.name = "Object";
		clazz.instanceSize = sizeof(Object);
		clazz.interfaceOffset = offsetof(Object, interface);
		clazz.interfaceSize = sizeof(ObjectInterface);
		clazz.initialize = initialize;
	});

	return &clazz;
}

#undef _Class
