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

#ifndef _Objectively_Condition_h
#define _Objectively_Condition_h

#include <objectively/lock.h>

/**
 * @file
 *
 * POSIX Threads conditional variables. Conditions combine a Lock with a
 * signaling mechanism, so that Threads may inform one another when a condition
 * is met.
 */

typedef struct Condition Condition;
typedef struct ConditionInterface ConditionInterface;

/**
 * @brief The Condition type.
 *
 * @extends Lock
 */
struct Condition {

	/**
	 * @brief The parent.
	 */
	Lock lock;

	/**
	 * @brief The backing condition.
	 */
	id condition;
};

/**
 * @brief The Condition type.
 */
struct ConditionInterface {

	/**
	 * @brief The parent.
	 */
	LockInterface lockInterface;

	/**
	 * @brief Signals all Threads waiting on this Condition.
	 */
	void (*broadcast)(Condition *self);

	/**
	 * @brief Initializes this Condition.
	 *
	 * @return The initialized instance, or NULL on error.
	 */
	Condition *(*init)(Condition *self);

	/**
	 * @brief Signals a single Thread waiting on this Condition.
	 */
	void (*signal)(Condition *self);

	/**
	 * @brief Waits indefinitely for this Condition to be signaled.
	 */
	void (*wait)(Condition *self);
};

/**
 * @brief The Condition Class.
 */
extern Class __Condition;

#endif
