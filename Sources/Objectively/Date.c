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

#include <Objectively/Date.h>
#include <Objectively/Hash.h>

#define __Class __Date

#pragma mark - ObjectInterface

/**
 * @see ObjectInterface::hash(const Object *)
 */
static int hash(const Object *self) {

	Date *this = (Date *) self;

	int hash = HASH_SEED;

	hash = HashForInteger(hash, this->time.tv_sec);
	hash = HashForInteger(hash, this->time.tv_usec);

	return hash;
}

/**
 * @see ObjectInterface::isEqual(const Object *, const Object *)
 */
static BOOL isEqual(const Object *self, const Object *other) {

	if (super(Object, self, isEqual, other)) {
		return YES;
	}

	if (other && other->clazz == self->clazz) {

		const Date *this = (Date *) self;
		const Date *that = (Date *) other;

		return $(this, compareTo, that) == SAME;
	}

	return NO;
}

#pragma mark - DateInterface

/**
 * @see DateInterface::dateWithTimeSinceNow(const Time *)
 */
static Date *dateWithTimeSinceNow(const Time *interval) {

	Date *date = $(alloc(Date), init);
	if (date) {
		if (interval) {
			timeradd(&date->time, interval, &date->time);
		}
	}

	return date;
}

/**
 * @see DateInterface::compareTo(const Date *, const Date *)
 */
static ORDER compareTo(const Date *self, const Date *other) {

	if (other) {
		const long seconds = self->time.tv_sec - other->time.tv_sec;
		if (seconds == 0) {

			const long microseconds = self->time.tv_usec - other->time.tv_usec;
			if (microseconds == 0) {
				return SAME;
			}

			return microseconds < 0 ? ASCENDING : DESCENDING;
		}

		return seconds < 0 ? ASCENDING : DESCENDING;
	}

	return ASCENDING;
}

/**
 * @see DateInterface::init(Date *)
 */
static Date *init(Date *self) {
	return $(self, initWithTime, NULL);
}

/**
 * @see DateInterface::initWithTime(Date *, Time *)
 */
static Date *initWithTime(Date *self, const Time *time) {

	self = (Date *) super(Object, self, init);
	if (self) {
		if (time) {
			self->time = *time;
		} else {
			gettimeofday(&self->time, NULL);
		}
	}

	return self;
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	ObjectInterface *object = (ObjectInterface *) clazz->interface;

	object->hash = hash;
	object->isEqual = isEqual;

	DateInterface *date = (DateInterface *) clazz->interface;

	date->dateWithTimeSinceNow = dateWithTimeSinceNow;
	date->compareTo = compareTo;
	date->init = init;
	date->initWithTime = initWithTime;
}

Class __Date = {
	.name = "Date",
	.superclass = &__Object,
	.instanceSize = sizeof(Date),
	.interfaceOffset = offsetof(Date, interface),
	.interfaceSize = sizeof(DateInterface),
	.initialize = initialize,
};

#undef __Class

