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

#include <Objectively/OperationQueue.h>

#define _Class _OperationQueue

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

	OperationQueue *this = (OperationQueue *) self;

	$(this->locals.thread, cancel);
	$(this->locals.thread, join, NULL);

	release(this->locals.thread);
	release(this->locals.condition);
	release(this->locals.operations);

	super(Object, self, dealloc);
}

#pragma mark - OperationQueue

/**
 * @fn void OperationQueue::addOperation(OperationQueue *self, Operation *operation)
 * @memberof OperationQueue
 */
static void addOperation(OperationQueue *self, Operation *operation) {

	assert(operation);
	assert(operation->isCancelled == false);
	assert(operation->isExecuting == false);
	assert(operation->isFinished == false);

	synchronized(self->locals.condition, {
		$(self->locals.operations, addObject, operation);
		$(self->locals.condition, broadcast);
	});
}

/**
 * @fn void OperationQueue::cancelAllOperations(OperationQueue *self)
 * @memberof OperationQueue
 */
static void cancelAllOperations(OperationQueue *self) {

	Array *operations = $(self, operations);

	for (size_t i = 0; i < operations->count; i++) {
		$((Operation *) $(operations, objectAtIndex, i), cancel);
	}

	release(operations);
}

__thread OperationQueue *_currentQueue;

/**
 * @fn OperationQueue* OperationQueue::currentQueue(void)
 * @memberof OperationQueue
 */
static OperationQueue *currentQueue(void) {

	return _currentQueue;
}

/**
 * @brief ThreadFunction for the OperationQueue Thread.
 */
static ident run(Thread *thread) {

	OperationQueue *self = _currentQueue = thread->data;

	while (thread->isCancelled == false) {

		if (self->isSuspended == false) {
			Array *operations = NULL;

			retry:

			release(operations);
			operations = $(self, operations);

			for (size_t i = 0; i < operations->count; i++) {

				Operation *operation = $(operations, objectAtIndex, i);
				if ($(operation, isReady)) {
					$(operation, start);
					goto retry;
				}
			}

			release(operations);
		}

		const Time interval = { .tv_usec = 10 };
		Date *date = $$(Date, dateWithTimeSinceNow, &interval);

		synchronized(self->locals.condition, {
			$(self->locals.condition, waitUntilDate, date);
		});

		release(date);
	}

	return NULL;
}

/**
 * @fn OperationQueue *OperationQueue::init(OperationQueue *self)
 * @memberof OperationQueue
 */
static OperationQueue *init(OperationQueue *self) {

	self = (OperationQueue *) super(Object, self, init);
	if (self) {

		self->locals.condition = $(alloc(Condition), init);
		assert(self->locals.condition);

		self->locals.operations = $(alloc(MutableArray), init);
		assert(self->locals.operations);

		self->locals.thread = $(alloc(Thread), initWithFunction, run, self);
		assert(self->locals.thread);

		$(self->locals.thread, start);
	}

	return self;
}

/**
 * @fn size_t OperationQueue::operationCount(const OperationQueue *self)
 * @memberof OperationQueue
 */
static size_t operationCount(const OperationQueue *self) {

	size_t count;

	synchronized(self->locals.condition, {
		count = ((Array *) self->locals.operations)->count;
	});

	return count;
}

/**
 * @fn Array *OperationQueue::operations(const OperationQueue *self)
 * @memberof OperationQueue
 */
static Array *operations(const OperationQueue *self) {

	ident operations;

	synchronized(self->locals.condition, {
		operations = $((Object * ) self->locals.operations, copy);
	});

	return (Array *) operations;
}

/**
 * @fn void OperationQueue::removeOperation(OperationQueue *self, Operation *operation)
 * @memberof OperationQueue
 */
static void removeOperation(OperationQueue *self, Operation *operation) {

	assert(operation);
	assert(operation->isExecuting == false);

	synchronized(self->locals.condition, {
		$(self->locals.operations, removeObject, operation);
		$(self->locals.condition, broadcast);
	});
}

/**
 * @fn void OperationQueue::waitUntilAllOperationsAreFinished(OperationQueue *self)
 * @memberof OperationQueue
 */
static void waitUntilAllOperationsAreFinished(OperationQueue *self) {

	Array *operations = (Array *) self->locals.operations;
	while (operations->count > 0) {

		synchronized(self->locals.condition, {
			$(self->locals.condition, wait);
		});
	}
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	((ObjectInterface *) clazz->def->interface)->copy = copy;
	((ObjectInterface *) clazz->def->interface)->dealloc = dealloc;

	((OperationQueueInterface *) clazz->def->interface)->addOperation = addOperation;
	((OperationQueueInterface *) clazz->def->interface)->cancelAllOperations = cancelAllOperations;
	((OperationQueueInterface *) clazz->def->interface)->currentQueue = currentQueue;
	((OperationQueueInterface *) clazz->def->interface)->init = init;
	((OperationQueueInterface *) clazz->def->interface)->operationCount = operationCount;
	((OperationQueueInterface *) clazz->def->interface)->operations = operations;
	((OperationQueueInterface *) clazz->def->interface)->removeOperation = removeOperation;
	((OperationQueueInterface *) clazz->def->interface)->waitUntilAllOperationsAreFinished = waitUntilAllOperationsAreFinished;
}

/**
 * @fn Class *OperationQueue::_OperationQueue(void)
 * @memberof OperationQueue
 */
Class *_OperationQueue(void) {
	static Class clazz;
	static Once once;

	do_once(&once, {
		clazz.name = "OperationQueue";
		clazz.superclass = _Object();
		clazz.instanceSize = sizeof(OperationQueue);
		clazz.interfaceOffset = offsetof(OperationQueue, interface);
		clazz.interfaceSize = sizeof(OperationQueueInterface);
		clazz.initialize = initialize;
	});

	return &clazz;
}
#undef _Class
