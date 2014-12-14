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

#include <Objectively/Operation.h>
#include <Objectively/OperationQueue.h>

#define _Class _Operation

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

	Operation *this = (Operation *) self;

	release(this->locals.condition);
	release(this->locals.dependencies);

	super(Object, self, dealloc);
}

#pragma mark - OperationInterface

/**
 * @see OperationInterface::addDependency(Operation *, Operation *)
 */
static void addDependency(Operation *self, Operation *dependency) {

	assert(dependency);

	const Array *dependencies = (Array *) self->locals.dependencies;

	assert($(dependencies, indexOfObject, self) == -1);
	assert($(dependencies, indexOfObject, dependency) == -1);

	$(self->locals.dependencies, addObject, dependency);
}

/**
 * @see OperationInterface::cancel(Operation *)
 */
static void cancel(Operation *self) {

	if (self->isCancelled == NO) {
		if (self->isFinished == NO) {
			if (self->isExecuting == NO) {
				self->isCancelled = YES;
			}
		}
	}
}

/**
 * @see OperationInterface::dependencies(const Operation *)
 */
static Array *dependencies(const Operation *self) {

	id dependencies = $((Object *) self->locals.dependencies, copy);

	return (Array *) dependencies;
}

/**
 * @see OperationInterface::init(Operation *)
 */
static Operation *init(Operation *self) {

	self = (Operation *) super(Object, self, init);
	if (self) {

		self->locals.condition = $(alloc(Condition), init);
		assert(self->locals.condition);

		self->locals.dependencies = $(alloc(MutableArray), init);
		assert(self->locals.dependencies);
	}

	return self;
}

/**
 * @see OperationInterface::initWithFunction(Operation *, OperationFunction, id)
 */
static Operation *initWithFunction(Operation *self, OperationFunction function, id data) {

	self = $(self, init);
	if (self) {
		self->function = function;
		self->data = data;
	}

	return self;
}

/**
 * @see OperationInterface::isReady(const Operation *)
 */
static BOOL isReady(const Operation *self) {

	if (self->isExecuting || self->isFinished) {
		return NO;
	}

	if (self->isCancelled) {
		return YES;
	}

	const Array *dependencies = (Array *) self->locals.dependencies;
	for (size_t i = 0; i < dependencies->count; i++) {

		Operation *dependency = $(dependencies, objectAtIndex, i);
		if (dependency->isFinished == NO) {
			return NO;
		}
	}

	return YES;
}

/**
 * @see OperationInterface::removeDependency(Operation *, Operation *)
 */
static void removeDepdenency(Operation *self, Operation *dependency) {

	assert(dependency);

	$(self->locals.dependencies, removeObject, dependency);
}

/**
 * @see OperationInterface::start(Operation *)
 */
static void start(Operation *self) {

	if (self->isFinished || self->isExecuting) {
		return;
	}

	if (self->isCancelled == NO) {

		self->isExecuting = YES;

		self->function(self);

		self->isExecuting = NO;
	}

	self->isFinished = YES;

	WithLock(self->locals.condition, {
		$(self->locals.condition, broadcast);
	});

	OperationQueue *currentQueue = $$(OperationQueue, currentQueue);
	if (currentQueue) {
		$(currentQueue, removeOperation, self);
	}
}

/**
 * @see ConditionInterface::waitUntilFinished(const Operation *)
 */
static void waitUntilFinished(const Operation *self) {

	WithLock(self->locals.condition, {
		while (self->isFinished == NO) {
			$(self->locals.condition, wait);
		}
	});
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	ObjectInterface *object = (ObjectInterface *) clazz->interface;

	object->copy = copy;
	object->dealloc = dealloc;

	OperationInterface *operation = (OperationInterface *) clazz->interface;

	operation->addDependency = addDependency;
	operation->cancel = cancel;
	operation->dependencies = dependencies;
	operation->init = init;
	operation->initWithFunction = initWithFunction;
	operation->isReady = isReady;
	operation->removeDependency = removeDepdenency;
	operation->start = start;
	operation->waitUntilFinished = waitUntilFinished;
}

Class _Operation = {
	.name = "Operation",
	.superclass = &_Object,
	.instanceSize = sizeof(Operation),
	.interfaceOffset = offsetof(Operation, interface),
	.interfaceSize = sizeof(OperationInterface),
	.initialize = initialize,
};

#undef _Class
