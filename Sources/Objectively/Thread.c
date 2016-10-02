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
#include <signal.h>
#include <stdlib.h>

#include <pthread.h>

#include <Objectively/Thread.h>

#define _Class _Thread

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

	Thread *this = (Thread *) self;

	assert(this->isExecuting == false);

	free(this->thread);

	super(Object, self, dealloc);
}

#pragma mark - Thread

/**
 * @fn void Thread::cancel(Thread *self)
 *
 * @memberof Thread
 */
static void cancel(Thread *self) {

	assert(self->isCancelled == false);

	// int err = pthread_cancel(*((pthread_t *) self->thread));
	// assert(err == 0);

	self->isCancelled = true;
}

__thread Thread *_currentThread;

/**
 * @fn Thread *Thread::currentThread(void)
 *
 * @memberof Thread
 */
static Thread *currentThread(void) {

	return _currentThread;
}

/**
 * @fn void Thread::detach(Thread *self)
 *
 * @memberof Thread
 */
static void detach(Thread *self) {

	assert(self->isDetached == false);

	int err = pthread_detach(*((pthread_t *) self->thread));
	assert(err == 0);

	self->isDetached = true;
}

/**
 * @fn Thread *Thread::init(Thread *self)
 *
 * @memberof Thread
 */
static Thread *init(Thread *self) {

	return $(self, initWithFunction, NULL, NULL);
}

/**
 * @fn Thread *Thread::initWithFunction(Thread *self, ThreadFunction function, ident data)
 *
 * @memberof Thread
 */
static Thread *initWithFunction(Thread *self, ThreadFunction function, ident data) {

	self = (Thread *) super(Object, self, init);
	if (self) {
		self->function = function;
		self->data = data;

		self->thread = calloc(1, sizeof(pthread_t));
		assert(self->thread);
	}

	return self;
}

/**
 * @fn void Thread::join(Thread *self, ident *status)
 *
 * @memberof Thread
 */
static void join(Thread *self, ident *status) {

	int err = pthread_join(*((pthread_t *) self->thread), status);
	assert(err == 0);
}

/**
 * @fn void Thread::kill(Thread *self, int signal)
 *
 * @memberof Thread
 */
static void _kill(Thread *self, int signal) {

	int err = pthread_kill(*((pthread_t *) self->thread), signal);
	assert(err == 0);
}

/**
 * @brief Wraps the user-specified ThreadFunction, providing cleanup.
 */
static ident run(ident obj) {

	Thread *self = _currentThread = (Thread *) obj;

	self->isExecuting = true;

	ident ret = self->function(self);

	self->isFinished = true;

	self->isExecuting = false;

	return ret;
}

/**
 * @fn void Thread::start(Thread *self)
 *
 * @memberof Thread
 */
static void start(Thread *self) {

	assert(self->function);

	assert(self->isCancelled == false);
	assert(self->isDetached == false);
	assert(self->isExecuting == false);
	assert(self->isFinished == false);

	int err = pthread_create(self->thread, NULL, run, self);
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

	ThreadInterface *thread = (ThreadInterface *) clazz->def->interface;

	thread->cancel = cancel;
	thread->currentThread = currentThread;
	thread->detach = detach;
	thread->init = init;
	thread->initWithFunction = initWithFunction;
	thread->join = join;
	thread->kill = _kill;
	thread->start = start;
}

Class _Thread = {
	.name = "Thread",
	.superclass = &_Object,
	.instanceSize = sizeof(Thread),
	.interfaceOffset = offsetof(Thread, interface),
	.interfaceSize = sizeof(ThreadInterface),
	.initialize = initialize,
};

#undef _Class
