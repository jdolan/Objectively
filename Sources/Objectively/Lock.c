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
#include <stdlib.h>

#include <pthread.h>

#include <Objectively/Lock.h>

#define __Class __Lock

#pragma mark - Object instance methods

/**
 * @see Object::copy(const Object *)
 */
static Object *copy(const Object *self) {
	return NULL;
}

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

	Lock *this = (Lock *) self;

	pthread_mutex_destroy(this->lock);
	free(this->lock);

	super(Object, self, dealloc);
}

/**
 * @see Object::init(id, id, va_list *)
 */
static Object *init(id obj, id interface, va_list *args) {

	Lock *self = (Lock *) super(Object, obj, init, interface, args);
	if (self) {
		self->interface = (LockInterface *) interface;

		self->lock = calloc(1, sizeof(pthread_mutex_t));
		assert(self->lock);

		int err = pthread_mutex_init(self->lock, NULL);
		assert(err == 0);
	}

	return (Object *) self;
}

#pragma mark - Lock instance methods

/**
 * @see Lock::lock(Lock *)
 */
static void lock(Lock *self) {

	int err = pthread_mutex_lock(self->lock);
	assert(err == 0);
}

/**
 * @see Lock::tryLock(Lock *)
 */
static BOOL tryLock(Lock *self) {

	int err = pthread_mutex_trylock(self->lock);
	assert(err == 0 || err == EBUSY);

	return err == 0;
}

/**
 * @see Lock::unlock(Lock *)
 */
static void unlock(Lock *self) {

	int err = pthread_mutex_unlock(self->lock);
	assert(err == 0);
}

#pragma mark - Object class methods

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *self) {

	ObjectInterface *object = (ObjectInterface *) self->interface;

	object->copy = copy;
	object->dealloc = dealloc;
	object->init = init;

	LockInterface *interface = (LockInterface *) self->interface;

	interface->lock = lock;
	interface->tryLock = tryLock;
	interface->unlock = unlock;
}

Class __Lock = {
	.name = "Lock",
	.superclass = &__Object,
	.instanceSize = sizeof(Lock),
	.interfaceSize = sizeof(LockInterface),
	.initialize = initialize, };

#undef __Class
