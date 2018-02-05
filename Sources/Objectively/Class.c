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

#include <Objectively/Config.h>

#include <assert.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#if HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <Objectively/Class.h>
#include <Objectively/Object.h>

size_t _pageSize;

static ClassDef *_classes;
static ident _handle;

/**
 * @brief Called `atexit` to teardown Objectively.
 */
static void teardown(void) {
	ClassDef *c;

	c = _classes;
	while (c) {
		if (c->descriptor.destroy) {
			c->descriptor.destroy(&c->descriptor);
		}

		c = c->next;
	}

	c = _classes;
	while (c) {

		ClassDef *next = c->next;

		free(c->interface);
		free(c);

		c = next;
	}

	if (_handle) {
		dlclose(_handle);
	}
}

/**
 * @brief Called when initializing `Object` to setup Objectively.
 */
static void setup(void) {

	_classes = NULL;

#if !defined(_SC_PAGESIZE)
	_pageSize = 4096;
#else
	_pageSize = sysconf(_SC_PAGESIZE);
#endif

	atexit(teardown);
}

Class *_initialize(Class *clazz) {

	assert(clazz);

	if (__sync_val_compare_and_swap(&clazz->magic, 0, -1) == 0) {

		assert(clazz->name);
		assert(clazz->instanceSize);
		assert(clazz->interfaceSize);
		assert(clazz->interfaceOffset);

		ClassDef *def = clazz->def = calloc(1, sizeof(ClassDef));
		assert(def);

		def->descriptor = *clazz;

		def->interface = calloc(1, clazz->interfaceSize);
		assert(def->interface);

		if (clazz == _Object()) {
			setup();
		} else {
			Class *super = clazz->superclass;
			assert(super);

			assert(super->instanceSize <= clazz->instanceSize);
			assert(super->interfaceSize <= clazz->interfaceSize);

			_initialize(super);

			memcpy(def->interface, super->def->interface, super->interfaceSize);
		}

		if (clazz->initialize) {
			clazz->initialize(clazz);
		}

		def->descriptor.magic = CLASS_MAGIC;
		def->next = __sync_lock_test_and_set(&_classes, def);

		clazz->magic = CLASS_MAGIC;

	} else {
		while (clazz->magic != CLASS_MAGIC) {
			;
		}
	}

	return &clazz->def->descriptor;
}

ident _alloc(Class *clazz) {

	_initialize(clazz);

	ident obj = calloc(1, clazz->instanceSize);
	assert(obj);

	Object *object = (Object *) obj;

	object->clazz = clazz;
	object->referenceCount = 1;

	ident interface = clazz->def->interface;
	do {
		*(ident *) (obj + clazz->interfaceOffset) = interface;
	} while ((clazz = clazz->superclass));

	return obj;
}

ident _cast(Class *clazz, const ident obj) {

	if (obj) {
		const Class *c = ((Object *) obj)->clazz;
		while (c) {

			// as a special case, we optimize for _Object
			if (c == clazz || clazz == _Object()) {
				break;
			}

			c = c->superclass;
		}
		assert(c);
	}

	return (ident) obj;
}

Class *classForName(const char *name) {

	if (name) {
		ClassDef *c = _classes;
		while (c) {
			if (strcmp(name, c->descriptor.name) == 0) {
				return &c->descriptor;
			}
			c = c->next;
		}

		char *s;
		if (asprintf(&s, "_%s", name) > 0) {
			static Once once;

			do_once(&once, _handle = dlopen(NULL, 0));

			Class *clazz = NULL;
			Class *(*archetype)(void) = dlsym(_handle, s);
			if (archetype) {
				clazz = _initialize(archetype());
			}

			free(s);
			return clazz;
		}
	}

	return NULL;
}

ident release(ident obj) {

	classForName("HueColorPicker");

	if (obj) {
		Object *object = cast(Object, obj);

		assert(object);

		if (__sync_add_and_fetch(&object->referenceCount, -1) == 0) {
			$(object, dealloc);
		}
	}

	return NULL;
}

ident retain(ident obj) {

	Object *object = cast(Object, obj);

	assert(object);

	__sync_add_and_fetch(&object->referenceCount, 1);

	return obj;
}
