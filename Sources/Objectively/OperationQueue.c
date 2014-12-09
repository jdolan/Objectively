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

#include <Objectively/Once.h>
#include <Objectively/OperationQueue.h>

#define __Class __OperationQueue

#pragma mark - ObjectInterface

/**
 * @see ObjectInterface::copy(const Object *)
 */
static Object *copy(const Object *self) {

	return NULL;
}

/**
 * @see ObjectInterface::dealloc(Object *)
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

#pragma mark - OperationQueueInterface

__thread OperationQueue *__currentQueue;

/**
 * @see OperationQueueInterface::currentQueue(void)
 */
static OperationQueue *currentQueue(void) {

	return __currentQueue;
}

/**
 * @see OperationQueueInterface::addOperation(OperationQueue *, Operation *)
 */
static void addOperation(OperationQueue *self, Operation *operation) {

	assert(operation);
	assert(operation->isCancelled == NO);
	assert(operation->isExecuting == NO);
	assert(operation->isFinished == NO);

	WithLock(self->locals.condition, {
		$(self->locals.operations, addObject, operation);
		$(self->locals.condition, broadcast);
	});
}

/**
 * @see OperationQueueInterface::cancelAllOperations(OperationQueue *)
 */
static void cancelAllOperations(OperationQueue *self) {

	Array *operations = $(self, operations);

	for (size_t i = 0; i < operations->count; i++) {
		$((Operation *) $(operations, objectAtIndex, i), cancel);
	}

	release(operations);
}

/**
 * @brief ThreadFunction for the OperationQueue Thread.
 */
static id run(Thread *thread) {

	OperationQueue *self = __currentQueue = thread->data;

	while (YES) {

		if (self->isSuspended == NO) {
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

		WithLock(self->locals.condition, {
			$(self->locals.condition, waitUntilDate, date);
		});

		release(date);
	}

	return NULL;
}

/**
 * @see OperationQueueInterface::init(OperationQueue *)
 */
static OperationQueue *init(OperationQueue *self) {

	self = (OperationQueue *) super(Object, self, init);
	if (self) {

		self->locals.condition = $(alloc(Condition), init);
		assert(self->locals.condition);

		self->locals.operations = $(alloc(Array), init);
		assert(self->locals.operations);

		self->locals.thread = $(alloc(Thread), initWithFunction, run, self);
		assert(self->locals.thread);

		$(self->locals.thread, start);
	}

	return self;
}

/**
 * @see OperationQueueInterface::operationCount(const OperationQueue *)
 */
static int operationCount(const OperationQueue *self) {

	int count;

	WithLock(self->locals.condition, {
		count = self->locals.operations->count;
	});

	return count;
}

/**
 * @see OperationQueueInterface::operations(OperationQueue *)
 */
static Array *operations(const OperationQueue *self) {

	id operations;

	WithLock(self->locals.condition, {
		operations = $((Object * ) self->locals.operations, copy);
	});

	return (Array *) operations;
}

/**
 * @see OperationQueueInterface::removeOperation(OperationQueue *, Operation *)
 */
static void removeOperation(OperationQueue *self, Operation *operation) {

	assert(operation);
	assert(operation->isExecuting == NO);

	WithLock(self->locals.condition, {
		$(self->locals.operations, removeObject, operation);
		$(self->locals.condition, broadcast);
	});
}

/**
 * @see OperationQueueInterface::waitUntilAllOperationsAreFinished(OperationQueue *)
 */
static void waitUntilAllOperationsAreFinished(OperationQueue *self) {

	while (self->locals.operations->count > 0) {

		WithLock(self->locals.condition, {
			$(self->locals.condition, wait);
		});
	}
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	ObjectInterface *object = (ObjectInterface *) clazz->interface;

	object->copy = copy;
	object->dealloc = dealloc;

	OperationQueueInterface *queue = (OperationQueueInterface *) clazz->interface;

	queue->currentQueue = currentQueue;
	queue->addOperation = addOperation;
	queue->cancelAllOperations = cancelAllOperations;
	queue->init = init;
	queue->operationCount = operationCount;
	queue->operations = operations;
	queue->removeOperation = removeOperation;
	queue->waitUntilAllOperationsAreFinished = waitUntilAllOperationsAreFinished;
}

Class __OperationQueue = {
	.name = "OperationQueue",
	.superclass = &__Object,
	.instanceSize = sizeof(OperationQueue),
	.interfaceOffset = offsetof(OperationQueue, interface),
	.interfaceSize = sizeof(OperationQueueInterface),
	.initialize = initialize, };

#undef __Class
