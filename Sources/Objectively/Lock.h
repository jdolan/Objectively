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

#include <Objectively/Object.h>

/**
 * @file
 * @brief POSIX Threads locks.
 */

typedef struct Lock Lock;
typedef struct LockInterface LockInterface;

/**
 * @brief POSIX Threads locks.
 * @details Mediate access to a critical section by enforcing mutual exclusion.
 * @extends Object
 * @ingroup Concurrency
 */
struct Lock {

  /**
   * @brief The superclass.
   */
  Object object;

  /**
   * @brief The interface.
   * @protected
   */
  LockInterface *interface;

  /**
   * @brief The backing lock.
   * @private
   */
  ident lock;
};

/**
 * @brief The Lock interface.
 */
struct LockInterface {

  /**
   * @brief The superclass interface.
   */
  ObjectInterface objectInterface;

  /**
   * @fn Lock *Lock::init(Lock *self)
   * @brief Initializes this Lock.
   * @param self The Lock.
   * @return The initialized Lock, or `NULL` on error.
   * @memberof Lock
   */
  Lock *(*init)(Lock *self);

  /**
   * @fn void Lock::lock(Lock *self)
   * @brief Acquire this lock, waiting indefinitely.
   * @param self The Lock.
   * @memberof Lock
   */
  void (*lock)(Lock *self);

  /**
   * @fn bool Lock::tryLock(Lock *self)
   * @brief Attempt to acquire this lock immediately.
   * @param self The Lock.
   * @return `true` if the Lock was acquired, `false` otherwise.
   * @memberof Lock
   */
  bool (*tryLock)(Lock *self);

  /**
   * @fn void Lock::unlock(Lock *self)
   * @brief Release this Lock.
   * @param self The Lock.
   * @memberof Lock
   */
  void (*unlock)(Lock *self);
};

/**
 * @fn Class *Lock::_Lock(void)
 * @brief The Lock archetype.
 * @return The Lock Class.
 * @memberof Lock
 */
OBJECTIVELY_EXPORT Class *_Lock(void);

/**
 * @brief Wraps `statements` with calls to `lock` and `unlock`.
 * @param _lock The Lock instance.
 * @param statements The statements to perform while the Lock is held.
 */
#define synchronized(_lock, statements) { \
  $((Lock *) _lock, lock); \
    statements; \
  $((Lock *) _lock, unlock); \
}
