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

#include <Objectively/Operation.h>
#include <Objectively/OperationQueue.h>

#define _Class _Operation

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

	Operation *this = (Operation *) self;

	release(this->locals.condition);
	release(this->locals.dependencies);

	super(Object, self, dealloc);
}

#pragma mark - Operation

/**
 * @fn void Operation::addDependency(Operation *self, Operation *dependency)
 * @memberof Operation
 */
static void addDependency(Operation *self, Operation *dependency) {

	assert(dependency);
	assert(dependency != self);

	assert($((Array *) self->locals.dependencies, indexOfObject, dependency) == -1);

	$(self->locals.dependencies, addObject, dependency);
}

/**
 * @fn void Operation::cancel(Operation *self)
 * @memberof Operation
 */
static void cancel(Operation *self) {

	if (self->isCancelled == false) {
		if (self->isFinished == false) {
			if (self->isExecuting == false) {
				self->isCancelled = true;
			}
		}
	}
}

/**
 * @fn Array *Operation::dependencies(const Operation *self)
 * @memberof Operation
 */
static Array *dependencies(const Operation *self) {

	ident dependencies = $((Object *) self->locals.dependencies, copy);

	return (Array *) dependencies;
}

/**
 * @fn Operation *Operation::init(Operation *self)
 * @memberof Operation
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
 * @fn Operation *Operation::initWithFunction(Operation *self, OperationFunction function, ident data)
 * @memberof Operation
 */
static Operation *initWithFunction(Operation *self, OperationFunction function, ident data) {

	self = $(self, init);
	if (self) {
		self->function = function;
		self->data = data;
	}

	return self;
}

/**
 * @fn _Bool Operation::isReady(const Operation *self)
 * @memberof Operation
 */
static _Bool isReady(const Operation *self) {

	if (self->isExecuting || self->isFinished) {
		return false;
	}

	if (self->isCancelled) {
		return true;
	}

	const Array *dependencies = (Array *) self->locals.dependencies;
	for (size_t i = 0; i < dependencies->count; i++) {

		Operation *dependency = $(dependencies, objectAtIndex, i);
		if (dependency->isFinished == false) {
			return false;
		}
	}

	return true;
}

/**
 * @fn void Operation::removeDependency(Operation *self, Operation *dependency)
 * @memberof Operation
 */
static void removeDepdenency(Operation *self, Operation *dependency) {

	assert(dependency);

	$(self->locals.dependencies, removeObject, dependency);
}

/**
 * @fn void Operation::start(Operation *self)
 * @memberof Operation
 */
static void start(Operation *self) {

	if (self->isFinished || self->isExecuting) {
		return;
	}

	if (self->isCancelled == false) {

		self->isExecuting = true;

		self->function(self);

		self->isExecuting = false;
	}

	self->isFinished = true;

	WithLock(self->locals.condition, {
		$(self->locals.condition, broadcast);
	});

	OperationQueue *currentQueue = $$(OperationQueue, currentQueue);
	if (currentQueue) {
		$(currentQueue, removeOperation, self);
	}
}

/**
 * @fn void Operation::waitUntilFinished(const Operation *self)
 * @memberof Operation
 */
static void waitUntilFinished(const Operation *self) {

	WithLock(self->locals.condition, {
		while (self->isFinished == false) {
			$(self->locals.condition, wait);
		}
	});
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	ObjectInterface *object = (ObjectInterface *) clazz->def->interface;

	object->copy = copy;
	object->dealloc = dealloc;

	OperationInterface *operation = (OperationInterface *) clazz->def->interface;

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
