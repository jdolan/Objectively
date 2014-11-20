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

#pragma mark - ObjectInterface

/**
 * @see ObjectInterface::dealloc(Object *)
 */
static void dealloc(Object *self) {

	Condition *this = (Condition *) self;

	pthread_cond_destroy(this->condition);
	free(this->condition);

	super(Object, self, dealloc);
}

#pragma mark - ConditionInterface

/**
 * @see ConditionInterface::broadcast(Condition *)
 */
static void broadcast(Condition *self) {

	int err = pthread_cond_broadcast(self->condition);
	assert(err == 0);
}

/**
 * @see ConditionInterface::init(Condition *)
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
 * @see ConditionInterface::signal(Condition *)
 */
static void _signal(Condition *self) {

	int err = pthread_cond_signal(self->condition);
	assert(err == 0);
}

/**
 * @see ConditionInterface::wait(Condition *)
 */
static void _wait(Condition *self) {

	int err = pthread_cond_wait(self->condition, self->lock.lock);
	assert(err == 0);
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	ObjectInterface *object = (ObjectInterface *) clazz->interface;

	object->dealloc = dealloc;

	ConditionInterface *condition = (ConditionInterface *) clazz->interface;

	condition->broadcast = broadcast;
	condition->init = init;
	condition->signal = _signal;
	condition->wait = _wait;
}

Class __Condition = {
	.name = "Condition",
	.superclass = &__Lock,
	.instanceSize = sizeof(Condition),
	.interfaceOffset = offsetof(Condition, interface),
	.interfaceSize = sizeof(ConditionInterface),
	.initialize = initialize,
};

#undef __Class

