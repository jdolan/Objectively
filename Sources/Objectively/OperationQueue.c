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

#include "OperationQueue.h"

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

  const Array *threads = (Array *) this->locals.threads;

  for (size_t i = 0; i < threads->count; i++) {
    $((Thread *) $(threads, objectAtIndex, i), cancel);
  }

  synchronized(this->locals.condition, {
    $(this->locals.condition, broadcast);
  });

  for (size_t i = 0; i < threads->count; i++) {
    $((Thread *) $(threads, objectAtIndex, i), join, NULL);
  }

  release(this->locals.threads);
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
    operation->locals.queue = self;
    $(self->locals.operations, addObject, operation);
    $(self->locals.condition, signal);
  });
}

/**
 * @fn Operation *OperationQueue::addOperationWithFunction(OperationQueue *self, OperationFunction function, ident data)
 * @memberof OperationQueue
 */
static Operation *addOperationWithFunction(OperationQueue *self, OperationFunction function, ident data) {

  assert(function);

  Operation *operation = $(alloc(Operation), initWithFunction, function, data);

  $(self, addOperation, operation);

  return operation;
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

static __thread OperationQueue *_currentQueue;

/**
 * @fn OperationQueue* OperationQueue::currentQueue(void)
 * @memberof OperationQueue
 */
static OperationQueue *currentQueue(void) {
  return _currentQueue;
}

/**
 * @brief Predicate matching the next Operation eligible to start.
 */
static bool isOperationReady(const ident obj, ident data) {
  return $((Operation *) obj, isReady);
}

/**
 * @brief ThreadFunction for the OperationQueue Threads.
 */
static ident run(Thread *thread) {

  OperationQueue *self = _currentQueue = thread->data;

  bool isCancelled = false;

  while (!isCancelled) {

    Operation *operation = NULL;

    synchronized(self->locals.condition, {

      isCancelled = thread->isCancelled;

      if (!isCancelled && self->isSuspended == false) {

        operation = $(self->locals.operations, find, isOperationReady, NULL);
        if (operation) {
          operation->isDispatched = true;
          $(self->locals.condition, signal);
        }
      }

      if (operation == NULL && !isCancelled) {
        $(self->locals.condition, wait);
      }
    });

    if (operation == NULL) {
      continue;
    }

    $(operation, start);

    $(self, removeOperation, operation);
  }

  return NULL;
}

/**
 * @fn OperationQueue *OperationQueue::init(OperationQueue *self)
 * @memberof OperationQueue
 */
static OperationQueue *init(OperationQueue *self) {
  return $(self, initWithMaxConcurrentOperations, 1);
}

/**
 * @fn OperationQueue *OperationQueue::initWithMaxConcurrentOperations(OperationQueue *self, size_t maxConcurrentOperations)
 * @memberof OperationQueue
 */
static OperationQueue *initWithMaxConcurrentOperations(OperationQueue *self, size_t maxConcurrentOperations) {

  assert(maxConcurrentOperations);

  self = (OperationQueue *) super(Object, self, init);
  if (self) {

    self->locals.condition = $(alloc(Condition), init);
    assert(self->locals.condition);

    self->locals.operations = $(alloc(Array), init);
    assert(self->locals.operations);

    self->locals.threads = $(alloc(Array), init);
    assert(self->locals.threads);

    for (size_t i = 0; i < maxConcurrentOperations; i++) {

      Thread *thread = $(alloc(Thread), initWithFunction, run, self);
      assert(thread);

      $(self->locals.threads, addObject, thread);

      release(thread);
    }

    const Array *threads = (Array *) self->locals.threads;
    for (size_t i = 0; i < threads->count; i++) {
      $((Thread *) $(threads, objectAtIndex, i), start);
    }
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
    operation->locals.queue = NULL;
    $(self->locals.operations, removeObject, operation);
    $(self->locals.condition, signal);
  });
}

/**
 * @fn void OperationQueue::resume(OperationQueue *self)
 * @memberof OperationQueue
 */
static void resume(OperationQueue *self) {

  synchronized(self->locals.condition, {
    self->isSuspended = false;
    $(self->locals.condition, broadcast);
  });
}

/**
 * @fn void OperationQueue::suspend(OperationQueue *self)
 * @memberof OperationQueue
 */
static void suspend(OperationQueue *self) {

  synchronized(self->locals.condition, {
    self->isSuspended = true;
  });
}

/**
 * @fn void OperationQueue::waitUntilAllOperationsAreFinished(OperationQueue *self)
 * @memberof OperationQueue
 */
static void waitUntilAllOperationsAreFinished(OperationQueue *self) {

  while (true) {

    Operation *operation = NULL;

    synchronized(self->locals.condition, {
      const Array *operations = (Array *) self->locals.operations;
      if (operations->count) {
        operation = retain($(operations, firstObject));
      }
    });

    if (operation == NULL) {
      break;
    }

    $(operation, waitUntilFinished);

    release(operation);
  }
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

  ((ObjectInterface *) clazz->interface)->copy = copy;
  ((ObjectInterface *) clazz->interface)->dealloc = dealloc;

  ((OperationQueueInterface *) clazz->interface)->addOperation = addOperation;
  ((OperationQueueInterface *) clazz->interface)->addOperationWithFunction = addOperationWithFunction;
  ((OperationQueueInterface *) clazz->interface)->cancelAllOperations = cancelAllOperations;
  ((OperationQueueInterface *) clazz->interface)->currentQueue = currentQueue;
  ((OperationQueueInterface *) clazz->interface)->init = init;
  ((OperationQueueInterface *) clazz->interface)->initWithMaxConcurrentOperations = initWithMaxConcurrentOperations;
  ((OperationQueueInterface *) clazz->interface)->operationCount = operationCount;
  ((OperationQueueInterface *) clazz->interface)->operations = operations;
  ((OperationQueueInterface *) clazz->interface)->removeOperation = removeOperation;
  ((OperationQueueInterface *) clazz->interface)->resume = resume;
  ((OperationQueueInterface *) clazz->interface)->suspend = suspend;
  ((OperationQueueInterface *) clazz->interface)->waitUntilAllOperationsAreFinished = waitUntilAllOperationsAreFinished;
}

/**
 * @fn Class *OperationQueue::_OperationQueue(void)
 * @memberof OperationQueue
 */
Class *_OperationQueue(void) {
  static Class *clazz;
  static Once once;

  do_once(&once, {
    clazz = _initialize(&(const ClassDef) {
      .name = "OperationQueue",
      .superclass = _Object(),
      .instanceSize = sizeof(OperationQueue),
      .interfaceSize = sizeof(OperationQueueInterface),
      .initialize = initialize,
    });
  });

  return clazz;
}

#undef _Class
