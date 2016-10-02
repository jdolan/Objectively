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

#include <Objectively/Null.h>
#include <Objectively/Once.h>

#define _Class _Null

#pragma mark - Object

/**
 * @see Object::copy(const Object *)
 */
static Object *copy(const Object *self) {

	return (Object *) self;
}

#pragma mark - Null

static Null *_null;

/**
 * @fn Null *Null::null(void)
 *
 * @memberof Null
 */
static Null *null(void) {
	static Once once;

	do_once(&once, {
		_null = (Null *) $((Object *) alloc(Null), init);
	});

	return _null;
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	((ObjectInterface *) clazz->def->interface)->copy = copy;

	((NullInterface *) clazz->def->interface)->null = null;
}

/**
 * @see Class::destroy(Class *)
 */
static void destroy(Class *clazz) {

	release(_null);
}

Class _Null = {
	.name = "Null",
	.superclass = &_Object,
	.instanceSize = sizeof(Null),
	.interfaceOffset = offsetof(Null, interface),
	.interfaceSize = sizeof(NullInterface),
	.initialize = initialize,
	.destroy = destroy,
};

#undef _Class
