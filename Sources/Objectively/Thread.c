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

#include <Objectively/Thread.h>

#define __Class __Thread

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

	Thread *this = (Thread *) self;

	assert(this->isExecuting == NO);

	free(this->thread);

	super(Object, self, dealloc);
}

/**
 * @see Object::init(id, id, va_list *)
 */
static Object *init(id obj, id interface, va_list *args) {

	Thread *self = (Thread *) super(Object, obj, init, interface, args);
	if (self) {
		self->interface = (ThreadInterface *) interface;

		self->function = arg(args, ThreadFunction, NULL);
		assert(self->function);

		self->data = arg(args, id, NULL);

		self->thread = calloc(1, sizeof(pthread_t));
		assert(self->thread);
	}

	return (Object *) self;
}

#pragma mark - Thread instance methods

/**
 * @see Thread::cancel(Thread *)
 */
static void cancel(Thread *self) {

	assert(self->isCancelled == NO);

	int err = pthread_cancel(*((pthread_t *) self->thread));
	assert(err == 0);

	self->isCancelled = YES;
}

/**
 * @see Thread::detach(Thread *)
 */
static void detach(Thread *self) {

	assert(self->isDetached == NO);

	int err = pthread_detach(*((pthread_t *) self->thread));
	assert(err == 0);

	self->isDetached = YES;
}

/**
 * @see Thread::join(Thread *, id *)
 */
static void join(Thread *self, id *status) {

	int err = pthread_join(*((pthread_t *) self->thread), status);
	assert(err == 0);
}

/**
 * @brief Cleans up after execution.
 */
static void cleanup(id obj) {

	Thread *self = (Thread *) obj;

	self->isExecuting = NO;
}

/**
 * @brief Wraps the user-specified ThreadFunction, providing cleanup.
 */
static id main(id obj) {

	Thread *self = (Thread *) obj;

	self->isExecuting = YES;

	id ret = NULL;

	pthread_cleanup_push(cleanup, self);

	ret = self->function(self);

	pthread_cleanup_pop(cleanup);

	self->isFinished = YES;

	return ret;
}

/**
 * @see Thread::start(Thread *)
 */
static void start(Thread *self) {

	assert(self->isCancelled == NO);
	assert(self->isExecuting == NO);
	assert(self->isFinished == NO);

	int err = pthread_create(self->thread, NULL, main, self);
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

	ThreadInterface *thread = (ThreadInterface *) self->interface;

	thread->cancel = cancel;
	thread->detach = detach;
	thread->join = join;
	thread->start = start;
}

Class __Thread = {
	.name = "Thread",
	.superclass = &__Object,
	.instanceSize = sizeof(Thread),
	.interfaceSize = sizeof(ThreadInterface),
	.initialize = initialize, };
