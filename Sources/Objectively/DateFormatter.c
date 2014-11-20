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

#include <Objectively/DateFormatter.h>

#define __Class __DateFormatter

#pragma mark - DateFormatterInterface

/**
 * @see DateFormatterInterface::dateFromString(const DateFormatter *, const char *)
 */
static Date *dateFromString(const DateFormatter *self, const char *str) {
	struct tm time;

	const char *res = strptime(str, self->fmt, &time);
	if (res) {
		const Time t = {
			.tv_sec = mktime(&time)
		};

		return $(alloc(Date), initWithTime, &t);
	}

	return NULL;
}

/**
 * @see DateFormatterInterface::initWithFormat(DateFormatter *, const char *fmt)
 */
static DateFormatter *initWithFormat(DateFormatter *self, const char *fmt) {

	self = (DateFormatter *) super(Object, self, init);
	if (self) {
		self->fmt = fmt;
		assert(self->fmt);
	}

	return self;
}

/**
 * @see DateFormatterInterface::stringFromDate(const DateFormatter *, const Date *)
 */
static String *stringFromDate(const DateFormatter *self, const Date *date) {

	const time_t seconds = date->time.tv_sec;
	struct tm time;

	id res = localtime_r(&seconds, &time);
	assert(res == &time);

	const size_t len = strftime(NULL, 0, self->fmt, &time);

	char *str = calloc(len, 1);
	assert(str);

	strftime(str, len, self->fmt, &time);

	return $(alloc(String), initWithMemory, str);
}

#pragma mark - Class lifecycle

/**
 * see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	DateFormatterInterface *dateFormatter = (DateFormatterInterface *) clazz->interface;

	dateFormatter->dateFromString = dateFromString;
	dateFormatter->initWithFormat = initWithFormat;
	dateFormatter->stringFromDate = stringFromDate;

	tzset();
}

Class __DateFormatter = {
	.name = "DateFormatter",
	.superclass = &__Object,
	.instanceSize = sizeof(DateFormatter),
	.interfaceOffset = offsetof(DateFormatter, interface),
	.interfaceSize = sizeof(DateFormatterInterface),
	.initialize = initialize,
};

#undef __Class
