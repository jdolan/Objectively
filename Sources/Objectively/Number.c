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

#include <Objectively/Hash.h>
#include <Objectively/Number.h>
#include <Objectively/String.h>

#define _Class _Number

#pragma mark - Object

/**
 * @see Object::description(const Object *)
 */
static String *description(const Object *self) {

	Number *this = (Number *) self;

	return $(alloc(String), initWithFormat, "%.5f", this->value);
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
static _Bool isEqual(const Object *self, const Object *other) {

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
 * @fn _Bool Number::boolValue(const Number *self)
 * @memberof Number
 */
static _Bool boolValue(const Number *self) {
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

	ObjectInterface *object = (ObjectInterface *) clazz->def->interface;

	object->description = description;
	object->hash = hash;
	object->isEqual = isEqual;

	NumberInterface *number = (NumberInterface *) clazz->def->interface;

	number->boolValue = boolValue;
	number->charValue = charValue;
	number->compareTo = compareTo;
	number->doubleValue = doubleValue;
	number->floatValue = floatValue;
	number->longValue = longValue;
	number->initWithValue = initWithValue;
	number->intValue = intValue;
	number->numberWithValue = numberWithValue;
	number->shortValue = shortValue;
}

Class *_Number(void) {
	static Class _class;
	
	if (!_class.name) {
		_class.name = "Number";
		_class.superclass = _Object();
		_class.instanceSize = sizeof(Number);
		_class.interfaceOffset = offsetof(Number, interface);
		_class.interfaceSize = sizeof(NumberInterface);
		_class.initialize = initialize;
	}

	return &_class;
}

#undef _Class
