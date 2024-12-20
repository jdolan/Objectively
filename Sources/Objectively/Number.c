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

#include "Hash.h"
#include "Number.h"
#include "String.h"

#define _Class _Number

#pragma mark - Object

/**
 * @see Object::description(const Object *)
 */
static String *description(const Object *self) {

	Number *this = (Number *) self;

	return $(alloc(String), initWithFormat, "%.2f", this->value);
}

/**
 * @see Object::hash(const Object *)
 */
static int hash(const Object *self) {

	Number *this = (Number *) self;

	return HashForDecimal(HASH_SEED, this->value);
}

/**
 * @see Object::isEqual(const Object *, const Object *)
 */
static bool isEqual(const Object *self, const Object *other) {

	if (super(Object, self, isEqual, other)) {
		return true;
	}

	if (other && self->clazz == other->clazz) {

		const Number *this = (Number *) self;
		const Number *that = (Number *) other;

		return $(this, compareTo, that) == OrderSame;
	}

	return false;
}

#pragma mark - Number

/**
 * @fn bool Number::boolValue(const Number *self)
 * @memberof Number
 */
static bool boolValue(const Number *self) {
	return self->value ? true : false;
}

/**
 * @fn char Number::charValue(const Number *self)
 * @memberof Number
 */
static char charValue(const Number *self) {
	return (char) self->value;
}

/**
 * @fn Order Number::compareTo(const Number *self, const Number *other)
 * @memberof Number
 */
static Order compareTo(const Number *self, const Number *other) {

	if (other) {
		if (self->value == other->value) {
			return OrderSame;
		}

		return self->value < other->value ? OrderAscending : OrderDescending;
	}

	return OrderAscending;
}

/**
 * @fn double Number::doubleValue(const Number *self)
 * @memberof Number
 */
static double doubleValue(const Number *self) {
	return self->value;
}

/**
 * @fn float Number::floatValue(const Number *self)
 * @memberof Number
 */
static float floatValue(const Number *self) {
	return (float) self->value;
}

/**
 * @fn long Number::longValue(const Number *self)
 * @memberof Number
 */
static long longValue(const Number *self) {
	return (long) self->value;
}

/**
 * @fn Number *Number::initWithValue(Number *self, double value)
 * @memberof Number
 */
static Number *initWithValue(Number *self, const double value) {

	self = (Number *) super(Object, self, init);
	if (self) {
		self->value = value;
	}

	return self;
}

/**
 * @fn int Number::intValue(const Number *self)
 * @memberof Number
 */
static int intValue(const Number *self) {
	return (int) self->value;
}

/**
 * @fn Number *Number::numberWithValue(double value)
 * @memberof Number
 */
static Number *numberWithValue(double value) {
	return $(alloc(Number), initWithValue, value);
}

/**
 * @fn short Number::shortValue(const Number *self)
 * @memberof Number
 */
static short shortValue(const Number *self) {
	return (short) self->value;
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	((ObjectInterface *) clazz->interface)->description = description;
	((ObjectInterface *) clazz->interface)->hash = hash;
	((ObjectInterface *) clazz->interface)->isEqual = isEqual;

	((NumberInterface *) clazz->interface)->boolValue = boolValue;
	((NumberInterface *) clazz->interface)->charValue = charValue;
	((NumberInterface *) clazz->interface)->compareTo = compareTo;
	((NumberInterface *) clazz->interface)->doubleValue = doubleValue;
	((NumberInterface *) clazz->interface)->floatValue = floatValue;
	((NumberInterface *) clazz->interface)->longValue = longValue;
	((NumberInterface *) clazz->interface)->initWithValue = initWithValue;
	((NumberInterface *) clazz->interface)->intValue = intValue;
	((NumberInterface *) clazz->interface)->numberWithValue = numberWithValue;
	((NumberInterface *) clazz->interface)->shortValue = shortValue;
}

/**
 * @fn Class *Number::_Number(void)
 * @memberof Number
 */
Class *_Number(void) {
	static Class *clazz;
	static Once once;

	do_once(&once, {
		clazz = _initialize(&(const ClassDef) {
			.name = "Number",
			.superclass = _Object(),
			.instanceSize = sizeof(Number),
			.interfaceOffset = offsetof(Number, interface),
			.interfaceSize = sizeof(NumberInterface),
			.initialize = initialize,
		});
	});

	return clazz;
}

#undef _Class
