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

#pragma once

#include <Objectively/Array.h>
#include <Objectively/Condition.h>
#include <Objectively/Object.h>
#include <Objectively/Thread.h>

/**
 * @file
 * @brief A fixed set of Threads that run ThreadFunctions on behalf of the caller.
 */

typedef struct ThreadPool ThreadPool;
typedef struct ThreadPoolInterface ThreadPoolInterface;

/**
 * @brief An opaque handle to a ThreadFunction submitted with ThreadPool::execute.
 * @remarks A ThreadPoolWork MUST be passed to ThreadPool::wait exactly once, before the ThreadPool
 * is released. ThreadPool::wait frees it.
 */
typedef struct ThreadPoolWork ThreadPoolWork;

/**
 * @brief A fixed set of Threads that run ThreadFunctions on behalf of the caller.
 * @details Submitted functions wait in a first-in, first-out queue until a Thread is free. Each
 * ThreadFunction receives a Thread whose `data` is the user data that was submitted with the
 * function.
 *
 * Deallocating a ThreadPool sets `isCancelled` on its Threads, which then run the functions still
 * in the queue before they exit. A long-running function MAY check `isCancelled` to return early.
 * A function MUST NOT cancel the Thread that it is given.
 *
 * A function runs on the calling thread, with a Thread that is never started and never cancelled,
 * in two cases: ThreadPool::wait is called while the function is still in the queue, or the size
 * of the ThreadPool is `0`. A function MUST NOT `join`, `kill` or `detach` that Thread, and MUST
 * NOT rely on `isCancelled` alone to return, because it is never set.
 * @remarks A ThreadPool MUST NOT be deallocated by one of its own Threads.
 * @extends Object
 * @ingroup Concurrency
 */
struct ThreadPool {

  /**
   * @brief The superclass.
   */
  Object object;

  /**
   * @brief The interface type.
   * @protected
   */
  ThreadPoolInterface *interface[0];

  /**
   * @brief The condition guarding the queue and signaling the state of each ThreadPoolWork.
   * @private
   */
  Condition *condition;

  /**
   * @brief The first ThreadPoolWork in the queue, or `NULL`.
   * @private
   */
  ThreadPoolWork *head;

  /**
   * @brief The number of Threads.
   * @remarks Read-only.
   */
  size_t size;

  /**
   * @brief The last ThreadPoolWork in the queue, or `NULL`.
   * @private
   */
  ThreadPoolWork *tail;

  /**
   * @brief The Threads, `size` in all.
   * @private
   */
  Array *threads;
};

/**
 * @brief The ThreadPool interface.
 */
struct ThreadPoolInterface {

  /**
   * @brief The superclass interface.
   */
  ObjectInterface objectInterface;

  /**
   * @fn void ThreadPool::detach(ThreadPool *self, ThreadFunction function, ident data)
   * @brief Queues `function` without a handle to wait on.
   * @param self The ThreadPool.
   * @param function The ThreadFunction.
   * @param data The user data, available to `function` as `thread->data`.
   * @remarks The return value of `function` is discarded.
   * @memberof ThreadPool
   */
  void (*detach)(ThreadPool *self, ThreadFunction function, ident data);

  /**
   * @fn ThreadPoolWork *ThreadPool::execute(ThreadPool *self, ThreadFunction function, ident data)
   * @brief Queues `function`, returning a handle to wait on.
   * @param self The ThreadPool.
   * @param function The ThreadFunction.
   * @param data The user data, available to `function` as `thread->data`.
   * @return The ThreadPoolWork, which MUST be passed to ThreadPool::wait.
   * @memberof ThreadPool
   */
  ThreadPoolWork *(*execute)(ThreadPool *self, ThreadFunction function, ident data);

  /**
   * @fn ThreadPool *ThreadPool::init(ThreadPool *self)
   * @brief Initializes this ThreadPool with one Thread per online processor.
   * @param self The ThreadPool.
   * @return The initialized ThreadPool, or `NULL` on error.
   * @memberof ThreadPool
   */
  ThreadPool *(*init)(ThreadPool *self);

  /**
   * @fn ThreadPool *ThreadPool::initWithSize(ThreadPool *self, size_t size)
   * @brief Initializes this ThreadPool with the specified number of Threads.
   * @param self The ThreadPool.
   * @param size The number of Threads. With `0`, every function runs on the calling thread.
   * @return The initialized ThreadPool, or `NULL` on error.
   * @memberof ThreadPool
   */
  ThreadPool *(*initWithSize)(ThreadPool *self, size_t size);

  /**
   * @fn ident ThreadPool::wait(ThreadPool *self, ThreadPoolWork *work)
   * @brief Waits for the specified ThreadPoolWork to finish, and frees it.
   * @param self The ThreadPool.
   * @param work The ThreadPoolWork returned by ThreadPool::execute, or `NULL`.
   * @return The return value of the ThreadFunction, or `NULL` if `work` is `NULL`.
   * @remarks If `work` is still in the queue, it runs on the calling thread. So a ThreadFunction
   * MAY wait on other work of its own ThreadPool without a deadlock.
   * @memberof ThreadPool
   */
  ident (*wait)(ThreadPool *self, ThreadPoolWork *work);
};

/**
 * @fn Class *ThreadPool::_ThreadPool(void)
 * @brief The ThreadPool archetype.
 * @return The ThreadPool Class.
 * @memberof ThreadPool
 */
OBJECTIVELY_EXPORT Class *_ThreadPool(void);
