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

#include <objectively/class.h>
#include <objectively/object.h>

static void initialize(Class *class) {

	assert(class);

	if (class->initialized == NO) {

		assert(class->name);
		assert(class->size);
		assert(class->archetype);
		assert(class->init);

		if (class->initialize) {
			class->initialize();
		}

		class->init(class->archetype, NULL);
		class->initialized = YES;
	}
}

id new(Class *class, ...) {

	initialize(class);

	id obj = calloc(1, class->size);
	if (obj) {

		va_list args;
		va_start(args, class);

		obj = class->init(obj, &args);

		va_end(args);
	}

	return obj;
}

id cast(Class *class, const id obj) {

	initialize(class);

	if (obj) {
		struct Object *object = (struct Object *) obj;
		if (object->class) {
			assert(object->isKindOfClass(object, class));
		}
	}

	return (id) obj;
}

id archetype(Class *class) {

	initialize(class);

	return class->archetype;
}

id $(id obj, id selector, ...) {

	id ret = NULL;

	if (obj) {
		Class *c = (Object *) obj;
	}

	return ret;
}

void delete(id obj) {

	if (obj) {
		struct Object *object = cast(Object, obj);
		object->dealloc(object);
	}
}
