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
#include <stdio.h>
#include <stdlib.h>

#include "NumberFormatter.h"

#define _Class _NumberFormatter

#pragma mark - NumberFormatter

/**
 * @fn NumberFormatter *NumberFormatter::initWithFormat(NumberFormatter *self, const char *fmt)
 * @memberof NumberFormatter
 */
static NumberFormatter *initWithFormat(NumberFormatter *self, const char *fmt) {

	self = (NumberFormatter *) super(Object, self, init);
	if (self) {
		self->fmt = fmt ?: NUMBERFORMAT_DECIMAL;
	}

	return self;
}

/**
 * @fn Number *NumberFormatter::numberFromString(const NumberFormatter *self, const String *string)
 * @memberof NumberFormatter
 */
static Number *numberFromString(const NumberFormatter *self, const String *string) {

	if (string) {
		double value;

		const int res = sscanf(string->chars, self->fmt, &value);
		if (res == 1) {
			return $(alloc(Number), initWithValue, value);
		}
	}

	return NULL;
}

/**
 * @fn String *NumberFormatter::stringFromNumber(const NumberFormatter *self, const Number *number)
 * @memberof NumberFormatter
 */
static String *stringFromNumber(const NumberFormatter *self, const Number *number) {

	return $(alloc(String), initWithFormat, self->fmt, number->value);
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	((NumberFormatterInterface *) clazz->interface)->numberFromString = numberFromString;
	((NumberFormatterInterface *) clazz->interface)->initWithFormat = initWithFormat;
	((NumberFormatterInterface *) clazz->interface)->stringFromNumber = stringFromNumber;
}

/**
 * @fn Class *NumberFormatter::_NumberFormatter(void)
 * @memberof NumberFormatter
 */
Class *_NumberFormatter(void) {
	static Class *clazz;
	static Once once;

	do_once(&once, {
		clazz = _initialize(&(const ClassDef) {
			.name = "NumberFormatter",
			.superclass = _Object(),
			.instanceSize = sizeof(NumberFormatter),
			.interfaceOffset = offsetof(NumberFormatter, interface),
			.interfaceSize = sizeof(NumberFormatterInterface),
			.initialize = initialize,
		});
	});

	return clazz;
}

#undef _Class
