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
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include <Objectively/NumberFormatter.h>

#define __Class __NumberFormatter

#pragma mark - NumberFormatterInterface

/**
 * @see NumberFormatterInterface::numberFromCharacters(const NumberFormatter *, const char *)
 */
static Number *numberFromCharacters(const NumberFormatter *self, const char *chars) {

	double value;
	const int res = sscanf(chars, self->fmt, &value);
	if (res == 1) {
		return $(alloc(Number), initWithValue, value);
	}

	return NULL;
}

/**
 * @see NumberFormatterInterface::numberFromString(const NumberFormatter *, const String *)
 */
static Number *numberFromString(const NumberFormatter *self, const String *string) {

	assert(string);

	return $(self, numberFromCharacters, string->chars);
}

/**
 * @see NumberFormatterInterface::initWithFormat(NumberFormatter *, const char *fmt)
 */
static NumberFormatter *initWithFormat(NumberFormatter *self, const char *fmt) {

	self = (NumberFormatter *) super(Object, self, init);
	if (self) {
		self->fmt = fmt ?: NUMBERFORMAT_DECIMAL;
	}

	return self;
}

/**
 * @see NumberFormatterInterface::stringFromNumber(const NumberFormatter *, const Number *)
 */
static String *stringFromNumber(const NumberFormatter *self, const Number *number) {

	return $(alloc(String), initWithFormat, self->fmt, number->value);
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	NumberFormatterInterface *numberFormatter = (NumberFormatterInterface *) clazz->interface;

	numberFormatter->numberFromCharacters = numberFromCharacters;
	numberFormatter->numberFromString = numberFromString;
	numberFormatter->initWithFormat = initWithFormat;
	numberFormatter->stringFromNumber = stringFromNumber;
}

Class __NumberFormatter = {
	.name = "NumberFormatter",
	.superclass = &__Object,
	.instanceSize = sizeof(NumberFormatter),
	.interfaceOffset = offsetof(NumberFormatter, interface),
	.interfaceSize = sizeof(NumberFormatterInterface),
	.initialize = initialize,
};

#undef __Class
