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

#include <pthread.h>

#include <Objectively/Lock.h>

#define _Class _Lock

#pragma mark - Object

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

#pragma mark - Lock

/**
 * @fn Lock *Lock::init(Lock *self)
 * @memberof Lock
 */
static Lock *init(Lock *self) {

	self = (Lock *) super(Object, self, init);
	if (self) {

		self->lock = calloc(1, sizeof(pthread_mutex_t));
		assert(self->lock);

		const int err = pthread_mutex_init(self->lock, NULL);
		assert(err == 0);
	}

	return self;
}

/**
 * @fn void Lock::lock(Lock *self)
 * @memberof Lock
 */
static void lock(Lock *self) {

	int err = pthread_mutex_lock(self->lock);
	assert(err == 0);
}

/**
 * @fn _Bool Lock::tryLock(Lock *self)
 * @memberof Lock
 */
static _Bool tryLock(Lock *self) {

	int err = pthread_mutex_trylock(self->lock);
	assert(err == 0 || err == EBUSY);

	return err == 0;
}

/**
 * @fn void Lock::unlock(Lock *self)
 * @memberof Lock
 */
static void unlock(Lock *self) {

	int err = pthread_mutex_unlock(self->lock);
	assert(err == 0);
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	ObjectInterface *object = (ObjectInterface *) clazz->def->interface;

	object->copy = copy;
	object->dealloc = dealloc;

	LockInterface *interface = (LockInterface *) clazz->def->interface;

	interface->init = init;
	interface->lock = lock;
	interface->tryLock = tryLock;
	interface->unlock = unlock;
}

Class *_Lock(void) {
	static Class clazz;
	
	if (!clazz.name) {
		clazz.name = "Lock";
		clazz.superclass = _Object();
		clazz.instanceSize = sizeof(Lock);
		clazz.interfaceOffset = offsetof(Lock, interface);
		clazz.interfaceSize = sizeof(LockInterface);
		clazz.initialize = initialize;
	};

	return &clazz;
}

#undef _Class

