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

#include <pthread.h>

#include <Objectively/Class.h>
#include <Objectively/Object.h>

static Class *__classes;

/**
 * @brief Called `atexit` to teardown Objectively.
 */
static void teardown(void) {
	Class *c;

	c = __classes;
	while (c) {
		if (c->destroy) {
			c->destroy(c);
		}
		c = c->locals.next;
	}

	c = __classes;
	while (c) {
		if (c->interface) {
			free(c->interface);
		}
		c = c->locals.next;
	}

	pthread_exit(NULL);
}

/**
 * @brief Called when initializing `Object` to setup Objectively.
 */
static void setup(void) {

	atexit(teardown);
}

/**
 * @brief Initializes the class by setting up its magic and archetype.
 */
static void initialize(Class *clazz) {

	assert(clazz);

	if (__sync_val_compare_and_swap(&clazz->locals.magic, 0, -1) == 0) {

		assert(clazz->name);
		assert(clazz->instanceSize);
		assert(clazz->interfaceSize);
		assert(clazz->interfaceOffset);

		clazz->interface = calloc(1, clazz->interfaceSize);
		assert(clazz->interface);

		Class *super = clazz->superclass;

		if (clazz == &__Object) {
			assert(super == NULL);
			setup();
		} else {
			assert(super != NULL);

			assert(super->instanceSize <= clazz->instanceSize);
			assert(super->interfaceSize <= clazz->interfaceSize);

			initialize(super);

			memcpy(clazz->interface, super->interface, super->interfaceSize);
		}

		clazz->initialize(clazz);

		clazz->locals.next = __sync_lock_test_and_set(&__classes, clazz);
		clazz->locals.magic = CLASS_MAGIC;

	} else {
		while (__sync_fetch_and_or(&clazz->locals.magic, 0) != CLASS_MAGIC) {
			;
		}
	}
}

id __alloc(Class *clazz) {

	initialize(clazz);

	id obj = calloc(1, clazz->instanceSize);
	assert(obj);

	Object *object = (Object *) obj;
	object->clazz = clazz;

	Class *c = object->clazz;
	while (c) {
		*(id *) &obj[c->interfaceOffset] = c->interface;
		c = c->superclass;
	}

	object->referenceCount = 1;

	return obj;
}

id __cast(Class *clazz, const id obj) {

	if (obj) {
		const Class *c = ((Object *) obj)->clazz;
		while (c) {

			assert(c->locals.magic == CLASS_MAGIC);

			// as a special case, we optimize for __Object

			if (c == clazz || clazz == &__Object) {
				break;
			}

			c = c->superclass;
		}
		assert(c);
	}

	return (id) obj;
}

void release(id obj) {

	Object *object = cast(Object, obj);

	assert(object);

	if (__sync_add_and_fetch(&object->referenceCount, -1) == 0) {
		$(object, dealloc);
	}
}

void retain(id obj) {

	Object *object = cast(Object, obj);

	assert(object);

	__sync_add_and_fetch(&object->referenceCount, 1);
}
