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

#include <Objectively/Date.h>
#include <Objectively/Hash.h>

#define _Class _Date

#pragma mark - Object

/**
 * @see Object::hash(const Object *)
 */
static int hash(const Object *self) {

	Date *this = (Date *) self;

	int hash = HASH_SEED;

	hash = HashForInteger(hash, this->time.tv_sec);
	hash = HashForInteger(hash, this->time.tv_usec);

	return hash;
}

/**
 * @see Object::isEqual(const Object *, const Object *)
 */
static _Bool isEqual(const Object *self, const Object *other) {

	if (super(Object, self, isEqual, other)) {
		return true;
	}

	if (other && $(other, isKindOfClass, _Date())) {

		const Date *this = (Date *) self;
		const Date *that = (Date *) other;

		return $(this, compareTo, that) == OrderSame;
	}

	return false;
}

#pragma mark - Date

/**
 * @fn Order Date::compareTo(const Date *self, const Date *other)
 * @memberof Date
 */
static Order compareTo(const Date *self, const Date *other) {

	if (other) {
		const long seconds = self->time.tv_sec - other->time.tv_sec;
		if (seconds == 0) {

			const long microseconds = self->time.tv_usec - other->time.tv_usec;
			if (microseconds == 0) {
				return OrderSame;
			}

			return microseconds < 0 ? OrderAscending : OrderDescending;
		}

		return seconds < 0 ? OrderAscending : OrderDescending;
	}

	return OrderAscending;
}

/**
 * @fn Date *Date::date(void)
 * @memberof Date
 */
static Date *date(void) {
	
	return $$(Date, dateWithTimeSinceNow, NULL);
}

/**
 * @fn Date *Date::dateWithTimeSinceNow(const Time interval)
 * @memberof Date
 */
static Date *dateWithTimeSinceNow(const Time *interval) {
	
	Date *date = $(alloc(Date), init);
	if (date) {
		if (interval) {
			date->time.tv_sec += interval->tv_sec;
			date->time.tv_usec += interval->tv_usec;
			if (date->time.tv_usec >= MSEC_PER_SEC) {
				date->time.tv_sec++;
				date->time.tv_usec -= MSEC_PER_SEC;
			} else if (date->time.tv_usec < 0) {
				date->time.tv_sec--;
				date->time.tv_usec += MSEC_PER_SEC;
			}
		}
	}
	
	return date;
}

/**
 * @fn Date *Date::init(Date *self)
 * @memberof Date
 */
static Date *init(Date *self) {
	return $(self, initWithTime, NULL);
}

/**
 * @fn Date *Date::initWithTime(Date *self, const Time *time)
 * @memberof Date
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

/**
 * @fn Time Date::timeSinceDate(const Date *self, const Date *date)
 * @memberof Date
 */
static Time timeSinceDate(const Date *self, const Date *date) {

	assert(date);

	return $(self, timeSinceTime, &date->time);
}

/**
 * @fn Time Date::timeSinceNow(const Date *self)
 * @memberof Date
 */
static Time timeSinceNow(const Date *self) {

	Date *date = $$(Date, date);

	Time time = $(self, timeSinceDate, date);

	release(date);

	return time;
}

/**
 * @fn Time Date::timeSinceTime(const Date *self, const Time *time)
 * @memberof Date
 */
static Time timeSinceTime(const Date *self, const Time *time) {

	Time delta = {
		.tv_sec = self->time.tv_sec - time->tv_sec,
		.tv_usec = self->time.tv_usec - time->tv_usec
	};

	if (delta.tv_usec < 0) {
		delta.tv_sec--;
		delta.tv_usec += MSEC_PER_SEC;
	}

	return delta;
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	ObjectInterface *object = (ObjectInterface *) clazz->def->interface;

	object->hash = hash;
	object->isEqual = isEqual;

	DateInterface *interface = (DateInterface *) clazz->def->interface;

	interface->compareTo = compareTo;
	interface->date = date;
	interface->dateWithTimeSinceNow = dateWithTimeSinceNow;
	interface->init = init;
	interface->initWithTime = initWithTime;
	interface->timeSinceDate = timeSinceDate;
	interface->timeSinceNow = timeSinceNow;
	interface->timeSinceTime = timeSinceTime;
}

Class *_Date(void) {
	static Class clazz;
	
	if (!clazz.name) {
		clazz.name = "Date";
		clazz.superclass = _Object();
		clazz.instanceSize = sizeof(Date);
		clazz.interfaceOffset = offsetof(Date, interface);
		clazz.interfaceSize = sizeof(DateInterface);
		clazz.initialize = initialize;
	}

	return &clazz;
}

#undef _Class

