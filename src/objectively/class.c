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

/*
 * @brief Initializes the class by setting up its magic and archetype.
 */
static void initialize(Class *class) {

	assert(class);

	if (!class->magic) {
		class->magic = CLASS_MAGIC;

		assert(class->name);
		assert(class->instanceSize);
		assert(class->interfaceSize);

		if (class == (Class *) &__Object) {
			assert(class->superclass == NULL);
		} else {
			assert(class->superclass != NULL);

			initialize(class->superclass);

			assert(class->superclass->instanceSize <= class->instanceSize);
			assert(class->superclass->interfaceSize <= class->interfaceSize);

			class->interface = calloc(1, class->interfaceSize);
			assert(class->interface);

			memcpy(class->interface, class->superclass->interface, class->superclass->interfaceSize);
		}

		class->initialize(class);
	} else {
		assert(class->magic == CLASS_MAGIC);
	}
}

id __new(Class *class, ...) {

	initialize(class);

	id obj = calloc(1, class->instanceSize);
	if (obj) {

		((Object *) obj)->class = class;

		va_list args;
		va_start(args, class);

		obj = class->interface->init(obj, &args);

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

	if (obj) {
		$((Object * ) obj, dealloc);
	}
}
