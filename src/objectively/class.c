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
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include <objectively/class.h>
#include <objectively/object.h>

/**
 * @brief Initializes the class by setting up its magic and archetype.
 */
static void initialize(Class *class) {

	assert(class);

	if (__sync_val_compare_and_swap(&class->magic, 0, -1) == 0) {

		assert(class->name);
		assert(class->instanceSize);
		assert(class->interfaceSize);

		class->interface = calloc(1, class->interfaceSize);
		assert(class->interface);

		if (class == (Class *) &__Object) {
			assert(class->superclass == NULL);
		} else {
			assert(class->superclass != NULL);

			Class *super = class->superclass;
			initialize(super);

			assert(super->instanceSize <= class->instanceSize);
			assert(super->interfaceSize <= class->interfaceSize);

			memcpy(class->interface, super->interface, super->interfaceSize);
		}

		class->initialize(class);

		__sync_val_compare_and_swap(&class->magic, -1, CLASS_MAGIC);
	} else {
		while (__sync_fetch_and_or(&class->magic, 0) != CLASS_MAGIC) {
			;
		}
	}
}

id __new(Class *class, ...) {

	initialize(class);

	id obj = calloc(1, class->instanceSize);
	if (obj) {

		((Object *) obj)->class = class;
		((Object *) obj)->referenceCount = 1;

		id interface = class->interface;

		va_list args;
		va_start(args, class);

		obj = ((ObjectInterface *) interface)->init(obj, interface, &args);

		va_end(args);
	}

	return obj;
}

id __cast(Class *class, const id obj) {

	initialize(class);

	if (obj) {

		Object *object = (Object *) obj;
		if (object->class) {

			const Class *c = object->class;
			while (c) {

				assert(c->magic == CLASS_MAGIC);

				// as a special case, we optimize for __Object

				if (c == class || class == (Class *) &__Object) {
					break;
				}

				c = c->superclass;
			}
			assert(c);
		}
	}

	return (id) obj;
}

void delete(id obj) {

	Object *object = cast(Object, obj);

	assert(object);

	$(object, dealloc);
}

void release(id obj) {

	Object *object = cast(Object, obj);

	assert(object);

	if (__sync_add_and_fetch(&object->referenceCount, -1) == 0) {
		delete(object);
	}
}

void retain(id obj) {

	Object *object = cast(Object, obj);

	assert(object);

	__sync_add_and_fetch(&object->referenceCount, 1);
}
