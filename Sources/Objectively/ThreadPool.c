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

#include "Config.h"

#include <assert.h>
#include <stdlib.h>

#if defined(_WIN32)
#include <Windows.h>
#endif

#if HAVE_UNISTD_H
#include <unistd.h>
#endif

#include "ThreadPool.h"

#define _Class _ThreadPool

/**
 * @brief The states of a ThreadPoolWork.
 */
typedef enum {
  ThreadPoolWorkPending,
  ThreadPoolWorkRunning,
  ThreadPoolWorkFinished,
} ThreadPoolWorkState;

struct ThreadPoolWork {
  /**
   * @brief The user data.
   */
  ident data;

  /**
   * @brief The ThreadFunction.
   */
  ThreadFunction function;

  /**
   * @brief `true` if the Thread that runs this ThreadPoolWork frees it.
   */
  bool isDetached;

  /**
   * @brief The next ThreadPoolWork in the queue, or `NULL`.
   */
  ThreadPoolWork *next;

  /**
   * @brief The ThreadPool owning this work (weak reference).
   */
  ThreadPool *pool;

  /**
   * @brief The return value of the ThreadFunction.
   */
  ident result;

  /**
   * @brief The state.
   */
  ThreadPoolWorkState state;
};

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

  ThreadPool *this = (ThreadPool *) self;

  const Array *threads = this->threads;

  synchronized(this->condition, {
    for (size_t i = 0; i < threads->count; i++) {
      $((Thread *) $(threads, objectAtIndex, i), cancel);
    }
    $(this->condition, broadcast);
  });

  for (size_t i = 0; i < threads->count; i++) {
    $((Thread *) $(threads, objectAtIndex, i), join, NULL);
  }

  assert(this->head == NULL);

  release(this->threads);

  release(this->condition);

  super(Object, self, dealloc);
}

#pragma mark - ThreadPool

/**
 * @brief Runs `work` on the calling thread, with a Thread that is never started.
 */
static void runOnCallingThread(ThreadPoolWork *work) {

  Thread *thread = $(alloc(Thread), initWithFunction, work->function, work->data);
  assert(thread);

  work->result = work->function(thread);
  work->state = ThreadPoolWorkFinished;

  release(thread);
}

/**
 * @brief Queues `function`, or runs it on the calling thread if this ThreadPool has no Threads.
 * @return The ThreadPoolWork, or `NULL` if `isDetached`.
 */
static ThreadPoolWork *submit(ThreadPool *self, ThreadFunction function, ident data, bool isDetached) {

  assert(function);

  ThreadPoolWork *work = calloc(1, sizeof(ThreadPoolWork));
  assert(work);

  work->pool = self;
  work->state = ThreadPoolWorkPending;
  work->isDetached = isDetached;
  work->function = function;
  work->data = data;

  if (self->size == 0) {
    runOnCallingThread(work);
    if (isDetached) {
      free(work);
      return NULL;
    }
    return work;
  }

  synchronized(self->condition, {
    if (self->tail) {
      self->tail->next = work;
    } else {
      self->head = work;
    }
    self->tail = work;
    $(self->condition, broadcast);
  });

  return isDetached ? NULL : work;
}

/**
 * @fn void ThreadPool::detach(ThreadPool *self, ThreadFunction function, ident data)
 * @memberof ThreadPool
 */
static void detach(ThreadPool *self, ThreadFunction function, ident data) {
  submit(self, function, data, true);
}

/**
 * @fn ThreadPoolWork *ThreadPool::execute(ThreadPool *self, ThreadFunction function, ident data)
 * @memberof ThreadPool
 */
static ThreadPoolWork *execute(ThreadPool *self, ThreadFunction function, ident data) {
  return submit(self, function, data, false);
}

/**
 * @fn ThreadPool *ThreadPool::init(ThreadPool *self)
 * @memberof ThreadPool
 */
static ThreadPool *init(ThreadPool *self) {

#if defined(_WIN32)
  const size_t size = GetActiveProcessorCount(ALL_PROCESSOR_GROUPS);
#elif defined(_SC_NPROCESSORS_ONLN)
  const long processors = sysconf(_SC_NPROCESSORS_ONLN);
  const size_t size = processors > 0 ? (size_t) processors : 1;
#else
  const size_t size = 1;
#endif

  return $(self, initWithSize, size);
}

/**
 * @brief Removes `work` from the queue.
 * @remarks The caller MUST hold the condition.
 */
static void dequeue(ThreadPool *self, ThreadPoolWork *work) {

  ThreadPoolWork *previous = NULL;
  for (ThreadPoolWork *w = self->head; w; previous = w, w = w->next) {
    if (w == work) {
      if (previous) {
        previous->next = work->next;
      } else {
        self->head = work->next;
      }
      if (self->tail == work) {
        self->tail = previous;
      }
      work->next = NULL;
      return;
    }
  }

  assert(false);
}

/**
 * @brief ThreadFunction for the ThreadPool Threads.
 * @remarks `thread->data` is the user data of the current ThreadPoolWork, so the ThreadPool is
 * kept in a local.
 */
static ident run(Thread *thread) {

  ThreadPool *self = thread->data;
  Condition *condition = self->condition;

  $((Lock *) condition, lock);

  while (true) {

    while (self->head == NULL && thread->isCancelled == false) {
      $(condition, wait);
    }

    ThreadPoolWork *work = self->head;
    if (work == NULL) {
      break;
    }

    dequeue(self, work);
    work->state = ThreadPoolWorkRunning;

    $((Lock *) condition, unlock);

    thread->data = work->data;
    const ident result = work->function(thread);

    $((Lock *) condition, lock);

    if (work->isDetached) {
      free(work);
    } else {
      work->result = result;
      work->state = ThreadPoolWorkFinished;
      $(condition, broadcast);
    }
  }

  $((Lock *) condition, unlock);

  return NULL;
}

/**
 * @fn ThreadPool *ThreadPool::initWithSize(ThreadPool *self, size_t size)
 * @memberof ThreadPool
 */
static ThreadPool *initWithSize(ThreadPool *self, size_t size) {

  self = (ThreadPool *) super(Object, self, init);
  if (self) {

    self->condition = $(alloc(Condition), init);
    assert(self->condition);

    self->size = size;

    self->threads = $(alloc(Array), initWithCapacity, size);
    assert(self->threads);

    for (size_t i = 0; i < self->size; i++) {
      Thread *thread = $(alloc(Thread), initWithFunction, run, self);
      assert(thread);

      $(self->threads, addObject, thread);
      $(thread, start);

      release(thread);
    }
  }

  return self;
}

/**
 * @fn ident ThreadPool::wait(ThreadPool *self, ThreadPoolWork *work)
 * @memberof ThreadPool
 */
static ident _wait(ThreadPool *self, ThreadPoolWork *work) {

  if (work == NULL) {
    return NULL;
  }

  assert(work->pool == self);
  assert(work->isDetached == false);

  bool isPending = false;

  synchronized(self->condition, {
    if (work->state == ThreadPoolWorkPending) {
      dequeue(self, work);
      work->state = ThreadPoolWorkRunning;
      isPending = true;
    } else {
      while (work->state != ThreadPoolWorkFinished) {
        $(self->condition, wait);
      }
    }
  });

  if (isPending) {
    runOnCallingThread(work);
  }

  const ident result = work->result;
  free(work);

  return result;
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

  ((ObjectInterface *) clazz->interface)->copy = copy;
  ((ObjectInterface *) clazz->interface)->dealloc = dealloc;

  ((ThreadPoolInterface *) clazz->interface)->detach = detach;
  ((ThreadPoolInterface *) clazz->interface)->execute = execute;
  ((ThreadPoolInterface *) clazz->interface)->init = init;
  ((ThreadPoolInterface *) clazz->interface)->initWithSize = initWithSize;
  ((ThreadPoolInterface *) clazz->interface)->wait = _wait;
}

/**
 * @fn Class *ThreadPool::_ThreadPool(void)
 * @memberof ThreadPool
 */
Class *_ThreadPool(void) {
  static Class *clazz;
  static Once once;

  do_once(&once, {
    clazz = _initialize(&(const ClassDef) {
      .name = "ThreadPool",
      .superclass = _Object(),
      .instanceSize = sizeof(ThreadPool),
      .interfaceSize = sizeof(ThreadPoolInterface),
      .initialize = initialize,
    });
  });

  return clazz;
}

#undef _Class
