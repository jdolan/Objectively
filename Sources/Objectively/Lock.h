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

#ifndef _Objectively_Lock_h_
#define _Objectively_Lock_h_

#include <Objectively/Object.h>

/**
 * @file
 *
 * Mediate access to a critical section by enforcing mutual exclusion.
 */

typedef struct Lock Lock;
typedef struct LockInterface LockInterface;

/**
 * @brief The Lock type.
 */
struct Lock {

	/**
	 * @brief The parent.
	 */
	Object object;

	/**
	 * @brief The backing lock.
	 */
	id lock;
};

/**
 * @brief The Lock interface.
 */
struct LockInterface {

	/**
	 * @brief The parent.
	 */
	ObjectInterface objectInterface;

	/**
	 * @brief Initializes this Lock.
	 *
	 * @return The initialized instance, or NULL on error.
	 */
	Lock *(*init)(Lock *self);

	/**
	 * @brief Acquire this lock, waiting indefinitely.
	 */
	void (*lock)(Lock *self);

	/**
	 * @brief Attempt to acquire this lock immediately.
	 *
	 * @return `YES` if the Lock was acquired, `NO` otherwise.
	 */
	BOOL (*tryLock)(Lock *self);

	/**
	 * @brief Release this Lock.
	 */
	void (*unlock)(Lock *self);
};

/**
 * @brief The Lock Class.
 */
extern Class __Lock;

#endif
