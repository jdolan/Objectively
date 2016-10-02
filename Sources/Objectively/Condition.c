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

#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include <pthread.h>

#include <Objectively/Condition.h>

#define _Class _Condition

#pragma mark - Object

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

	Condition *this = (Condition *) self;

	pthread_cond_destroy(this->condition);
	free(this->condition);

	super(Object, self, dealloc);
}

#pragma mark - Condition

/**
 * @fn void Condition::broadcast(Condition *self)
 *
 * @memberof Condition
 */
static void broadcast(Condition *self) {

	int err = pthread_cond_broadcast(self->condition);
	assert(err == 0);
}

/**
 * @fn Condition *Condition::init(Condition *self)
 *
 * @memberof Condition
 */
static Condition *init(Condition *self) {

	self = (Condition *) super(Lock, self, init);
	if (self) {

		self->condition = calloc(1, sizeof(pthread_cond_t));
		assert(self->condition);

		const int err = pthread_cond_init(self->condition, NULL);
		assert(err == 0);
	}

	return self;
}

/**
 * @fn void Condition::signal(Condition *self)
 *
 * @memberof Condition
 */
static void _signal(Condition *self) {

	int err = pthread_cond_signal(self->condition);
	assert(err == 0);
}

/**
 * @fn void Condition::wait(Condition *self)
 *
 * @memberof Condition
 */
static void _wait(Condition *self) {

	int err = pthread_cond_wait(self->condition, self->lock.lock);
	assert(err == 0);
}

/**
 * @fn _Bool Condition::waitUntilDate(Condition *self, const Date *date)
 *
 * @memberof Condition
 */
static _Bool waitUntilDate(Condition *self, const Date *date) {

	Lock *lock = (Lock *) self;

	const struct timespec time = {
		.tv_sec = date->time.tv_sec,
		.tv_nsec = date->time.tv_usec * 1000
	};

	int err = pthread_cond_timedwait(self->condition, lock->lock, &time);
	assert(err == 0 || err == ETIMEDOUT);

	return err == 0;
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	ObjectInterface *object = (ObjectInterface *) clazz->def->interface;

	object->dealloc = dealloc;

	ConditionInterface *condition = (ConditionInterface *) clazz->def->interface;

	condition->broadcast = broadcast;
	condition->init = init;
	condition->signal = _signal;
	condition->wait = _wait;
	condition->waitUntilDate = waitUntilDate;
}

Class _Condition = {
	.name = "Condition",
	.superclass = &_Lock,
	.instanceSize = sizeof(Condition),
	.interfaceOffset = offsetof(Condition, interface),
	.interfaceSize = sizeof(ConditionInterface),
	.initialize = initialize,
};

#undef _Class

