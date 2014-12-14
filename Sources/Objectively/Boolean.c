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

#include <Objectively/Boolean.h>
#include <Objectively/Once.h>
#include <Objectively/String.h>

#define _Class _Boolean

#pragma mark - ObjectInterface

/**
 * @see ObjectInterface::copy(const Object *)
 */
static Object *copy(const Object *self) {

	return (Object *) self;
}

/**
 * @see ObjectInterface::description(const Object *)
 */
static String *description(const Object *self) {

	const Boolean *this = (Boolean *) self;

	return $(alloc(String), initWithCharacters, this->bool ? "YES" : "NO");
}

#pragma mark - BooleanInterface

static Boolean *_no;

/**
 * @see BooleanInterface::no(void)
 */
static Boolean *no(void) {
	static Once once;

	DispatchOnce(once, {
		_no = (Boolean *) $((Object *) alloc(Boolean), init);
		_no->bool = NO;
	});

	return _no;
}

static Boolean *_yes;

/**
 * @see BooleanInterface::yes(void)
 */
static Boolean *yes(void) {
	static Once once;

	DispatchOnce(once, {
		_yes = (Boolean *) $((Object *) alloc(Boolean), init);
		_yes->bool = YES;
	});

	return _yes;
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	ObjectInterface *object = (ObjectInterface *) clazz->interface;

	object->copy = copy;
	object->description = description;

	BooleanInterface *boolean = (BooleanInterface *) clazz->interface;

	boolean->no = no;
	boolean->yes = yes;
}

/**
 * @see Class::destroy(Class *)
 */
static void destroy(Class *clazz) {

	release(_no);
	release(_yes);
}

Class _Boolean = {
	.name = "Boolean",
	.superclass = &_Object,
	.instanceSize = sizeof(Boolean),
	.interfaceOffset = offsetof(Boolean, interface),
	.interfaceSize = sizeof(BooleanInterface),
	.initialize = initialize,
	.destroy = destroy,
};

#undef _Class
