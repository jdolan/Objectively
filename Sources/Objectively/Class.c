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
#include <unistd.h>

#include <Objectively/Class.h>
#include <Objectively/Object.h>

size_t _pageSize;

static Class *_classes;

/**
 * @brief Called `atexit` to teardown Objectively.
 */
static void teardown(void) {
	Class *c;

	c = _classes;
	while (c) {
		if (c->destroy) {
			c->destroy(c);
		}
		c = c->locals.next;
	}

	c = _classes;
	while (c) {
		if (c->interface) {
			free(c->interface);
			c->interface = NULL;
		}
		c->locals.magic = 0;
		c = c->locals.next;
	}
}

/**
 * @brief Called when initializing `Object` to setup Objectively.
 */
static void setup(void) {

	_classes = NULL;

#if __MINGW32__
	_pageSize = 4096;
#else
	_pageSize = sysconf(_SC_PAGESIZE);
#endif

	atexit(teardown);
}

void _initialize(Class *clazz) {

	assert(clazz);

	if (__sync_val_compare_and_swap(&clazz->locals.magic, 0, -1) == 0) {

		assert(clazz->name);
		assert(clazz->instanceSize);
		assert(clazz->interfaceSize);
		assert(clazz->interfaceOffset);

		clazz->interface = calloc(1, clazz->interfaceSize);
		assert(clazz->interface);

		Class *super = clazz->superclass;

		if (clazz == &_Object) {
			setup();
		} else {
			assert(super);

			assert(super->instanceSize <= clazz->instanceSize);
			assert(super->interfaceSize <= clazz->interfaceSize);

			_initialize(super);

			memcpy(clazz->interface, super->interface, super->interfaceSize);
		}

		clazz->initialize(clazz);

		clazz->locals.next = __sync_lock_test_and_set(&_classes, clazz);
		clazz->locals.magic = CLASS_MAGIC;

	} else {
		while (clazz->locals.magic != CLASS_MAGIC) {
			;
		}
	}
}

ident _alloc(Class *clazz) {

	_initialize(clazz);

	ident obj = calloc(1, clazz->instanceSize);
	assert(obj);

	Object *object = (Object *) obj;

	object->clazz = clazz;
	object->referenceCount = 1;

	ident interface = clazz->interface;
	do {
		*(ident *) (obj + clazz->interfaceOffset) = interface;
	} while ((clazz = clazz->superclass));

	return obj;
}

ident _cast(Class *clazz, const ident obj) {

	if (obj) {
		const Class *c = ((Object *) obj)->clazz;
		while (c) {

			assert(c->locals.magic == CLASS_MAGIC);

			// as a special case, we optimize for _Object

			if (c == clazz || clazz == &_Object) {
				break;
			}

			c = c->superclass;
		}
		assert(c);
	}

	return (ident) obj;
}

void release(ident obj) {

	if (obj) {
		Object *object = cast(Object, obj);

		assert(object);

		if (__sync_add_and_fetch(&object->referenceCount, -1) == 0) {
			$(object, dealloc);
		}
	}
}

ident retain(ident obj) {

	Object *object = cast(Object, obj);

	assert(object);

	__sync_add_and_fetch(&object->referenceCount, 1);

	return obj;
}
