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

#include <Objectively/Null.h>
#include <Objectively/Once.h>

#define __Class __Null

#pragma mark - ObjectInterface

/**
 * @see ObjectInterface::copy(const Object *)
 */
static Object *copy(const Object *self) {

	return (Object *) self;
}

#pragma mark - NullInterface

static Null *__null__;

/**
 * @see NullInterface::null(void)
 */
static Null *null(void) {
	static Once once;

	DispatchOnce(once, {
		__null__ = (Null *) $((Object *) alloc(Null), init);
	});

	return __null__;
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	((ObjectInterface *) clazz->interface)->copy = copy;

	((NullInterface *) clazz->interface)->null = null;
}

/**
 * @see Class::destroy(Class *)
 */
static void destroy(Class *clazz) {

	release(__null__);
}

Class __Null = {
	.name = "Null",
	.superclass = &__Object,
	.instanceSize = sizeof(Null),
	.interfaceOffset = offsetof(Null, interface),
	.interfaceSize = sizeof(NullInterface),
	.initialize = initialize,
	.destroy = destroy,
};

#undef __Class
