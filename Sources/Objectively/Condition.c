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
#include <string.h>

#include <pthread.h>

#include <objectively/Condition.h>

#define __Class __Condition

#pragma mark - Object instance methods

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

	Condition *this = (Condition *) self;

	pthread_cond_destroy(this->condition);
	free(this->condition);

	super(Object, self, dealloc);
}

/**
 * @see Object::init(id, id, va_list *)
 */
static Object *init(id obj, id interface, va_list *args) {

	Condition *self = (Condition *) super(Object, obj, init, interface, args);
	if (self) {
		self->interface = (ConditionInterface *) interface;

		self->condition = calloc(1, sizeof(pthread_cond_t));
		assert(self->condition);

		int err = pthread_cond_init(self->condition, NULL);
		assert(err == 0);
	}

	return (Object *) self;
}

#pragma mark - Condition instance methods

/**
 * @see Condition::broadcast(Condition *)
 */
static void broadcast(Condition *self) {

	int err = pthread_cond_broadcast(self->condition);
	assert(err == 0);
}

/**
 * @see Condition::signal(Condition *)
 */
static void _signal(Condition *self) {

	int err = pthread_cond_signal(self->condition);
	assert(err == 0);
}

/**
 * @see Condition::wait(Condition *)
 */
static void _wait(Condition *self) {

	int err = pthread_cond_wait(self->condition, self->lock.lock);
	assert(err == 0);
}

#pragma mark - Condition Class methods

static void initialize(Class *self) {

	ObjectInterface *object = (ObjectInterface *) self->interface;

	object->dealloc = dealloc;
	object->init = init;

	ConditionInterface *condition = (ConditionInterface *) self->interface;

	condition->broadcast = broadcast;
	condition->signal = _signal;
	condition->wait = _wait;
}

Class __Condition = {
	.name = "Condition",
	.superclass = &__Lock,
	.instanceSize = sizeof(Condition),
	.interfaceSize = sizeof(ConditionInterface),
	.initialize = initialize,
};
