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

#include "Hash.h"
#include "Value.h"

#define _Class _Value

#pragma mark - Object

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

	Value *this = (Value *) self;

	if (this->destructor) {
		this->destructor(this->value);
	}

	super(Object, self, dealloc);
}

/**
 * @see Object::hash(const Object *)
 */
static int hash(const Object *self) {

	const Value *this = (Value *) self;

	uintptr_t addr = (uintptr_t) this->value;

	return (int) ((13 * addr) ^ (addr >> 15));
}

/**
 * @see Object::isEqual(const Object *, const Object *)
 */
static _Bool isEqual(const Object *self, const Object *other) {

	if (super(Object, self, isEqual, other)) {
		return true;
	}

	if (other && $(other, isKindOfClass, _Value())) {

		const Value *this = (Value *) self;
		const Value *that = (Value *) other;

		return this->value == that->value;
	}

	return false;
}

#pragma mark - Value

/**
 * @fn Value *Value::initWithBytes(Value *self, const uint8_t *bytes, size_t length)
 * @memberof Value
 */
static Value *initWithBytes(Value *self, const uint8_t *bytes, size_t length) {

	self = (Value *) super(Object, self, init);
	if (self) {
		if (bytes) {
			self->value = calloc(1, length);
			assert(self->value);

			memcpy(self->value, bytes, length);
			self->destructor = free;
		}
	}

	return self;
}

/**
 * @fn Value *Value::initWithValue(Value *self, ident value)
 * @memberof Value
 */
static Value *initWithValue(Value *self, ident value) {

	self = (Value *) super(Object, self, init);
	if (self) {
		self->value = value;
	}

	return self;
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	((ObjectInterface *) clazz->interface)->dealloc = dealloc;
	((ObjectInterface *) clazz->interface)->hash = hash;
	((ObjectInterface *) clazz->interface)->isEqual = isEqual;

	((ValueInterface *) clazz->interface)->initWithBytes = initWithBytes;
	((ValueInterface *) clazz->interface)->initWithValue = initWithValue;
}

/**
 * @fn Class *Value::_Value(void)
 * @memberof Value
 */
Class *_Value(void) {
	static Class *clazz;
	static Once once;

	do_once(&once, {
		clazz = _initialize(&(const ClassDef) {
			.name = "Value",
			.superclass = _Object(),
			.instanceSize = sizeof(Value),
			.interfaceOffset = offsetof(Value, interface),
			.interfaceSize = sizeof(ValueInterface),
			.initialize = initialize,
		});
	});

	return clazz;
}

#undef _Class

