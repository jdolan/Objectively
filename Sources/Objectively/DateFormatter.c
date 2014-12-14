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

#define _Class _DateFormatter

#pragma mark - DateFormatterInterface

/**
 * @see DateFormatterInterface::dateFromCharacters(const DateFormatter *, const char *)
 */
static Date *dateFromCharacters(const DateFormatter *self, const char *chars) {
	struct tm time;

	const char *res = strptime(chars, self->fmt, &time);
	if (res) {
		const Time t = {
			.tv_sec = mktime(&time)
		};

		return $(alloc(Date), initWithTime, &t);
	}

	return NULL;
}

/**
 * @see DateFormatterInterface::dateFromString(const DateFormatter *, const String *)
 */
static Date *dateFromString(const DateFormatter *self, const String *string) {

	assert(string);

	return $(self, dateFromCharacters, string->chars);
}

/**
 * @see DateFormatterInterface::initWithFormat(DateFormatter *, const char *fmt)
 */
static DateFormatter *initWithFormat(DateFormatter *self, const char *fmt) {

	self = (DateFormatter *) super(Object, self, init);
	if (self) {
		self->fmt = fmt ?: DATEFORMAT_ISO8601;
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

	char *str = calloc(128, 1);
	assert(str);

	const size_t len = strftime(str, 128, self->fmt, &time);

	str = realloc(str, len);
	assert(str);

	return $(alloc(String), initWithMemory, str);
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	DateFormatterInterface *dateFormatter = (DateFormatterInterface *) clazz->interface;

	dateFormatter->dateFromCharacters = dateFromCharacters;
	dateFormatter->dateFromString = dateFromString;
	dateFormatter->initWithFormat = initWithFormat;
	dateFormatter->stringFromDate = stringFromDate;

	tzset();
}

Class _DateFormatter = {
	.name = "DateFormatter",
	.superclass = &_Object,
	.instanceSize = sizeof(DateFormatter),
	.interfaceOffset = offsetof(DateFormatter, interface),
	.interfaceSize = sizeof(DateFormatterInterface),
	.initialize = initialize,
};

#undef _Class
