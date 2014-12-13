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

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include <Objectively/Object.h>
#include <Objectively/String.h>

#define __class __Object

#pragma mark - ObjectInterface

/**
 * @see ObjectInterface::copy(const Object *)
 */
static Object *copy(const Object *self) {

	id obj = calloc(1, self->clazz->instanceSize);
	assert(obj);

	Object *object = memcpy(obj, self, self->clazz->instanceSize);
	object->referenceCount = 1;

	return object;
}

/**
 * @see ObjectInterface::dealloc(Object *)
 */
static void dealloc(Object *self) {

	free(self);
}

/**
 * @see ObjectInterface::description(const Object *)
 */
static String *description(const Object *self) {

	return $(alloc(String), initWithFormat, "%s@%p", self->clazz->name, self);
}

/**
 * @see ObjectInterface::hash(const Object *)
 */
static int hash(const Object *self) {

	uintptr_t addr = (uintptr_t) self;

	return (int) ((13 * addr) ^ (addr >> 15));
}

/**
 * @see ObjectInterface::init(Object *)
 */
static Object *init(Object *self) {

	return self;
}

/**
 * @see ObjectInterface::isEqual(const Object *, const Object *)
 */
static BOOL isEqual(const Object *self, const Object *other) {

	return self == other;
}

/**
 * @see ObjectInterface::isKindOfClass(const Object *, const Class *)
 */
static BOOL isKindOfClass(const Object *self, const Class *clazz) {

	const Class *c = self->clazz;
	while (c) {
		if (c == clazz) {
			return YES;
		}
		c = c->superclass;
	}

	return NO;
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	ObjectInterface *object = (ObjectInterface *) clazz->interface;

	object->copy = copy;
	object->dealloc = dealloc;
	object->description = description;
	object->hash = hash;
	object->init = init;
	object->isEqual = isEqual;
	object->isKindOfClass = isKindOfClass;
}

Class __Object = {
	.name = "Object",
	.instanceSize = sizeof(Object),
	.interfaceOffset = offsetof(Object, interface),
	.interfaceSize = sizeof(ObjectInterface),
	.initialize = initialize,
};

#undef __class
