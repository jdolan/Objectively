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

#include <Objectively/Date.h>
#include <Objectively/Lock.h>

/**
 * @file
 * @brief POSIX Threads conditional variables.
 */

typedef struct Condition Condition;
typedef struct ConditionInterface ConditionInterface;

/**
 * @brief POSIX Threads conditional variables.
 * @details Conditions combine a Lock with a signaling mechanism, so that Threads may inform one 
 * another when a condition is met.
 * @extends Lock
 * @ingroup Concurrency
 */
struct Condition {

	/**
	 * @brief The superclass.
	 */
	Lock lock;

	/**
	 * @brief The interface.
	 * @protected
	 */
	ConditionInterface *interface;

	/**
	 * @brief The backing condition.
	 * @private
	 */
	ident condition;
};

/**
 * @brief The Condition interface.
 */
struct ConditionInterface {

	/**
	 * @brief The superclass interface.
	 */
	LockInterface lockInterface;

	/**
	 * @fn void Condition::broadcast(Condition *self)
	 * @brief Signals all Threads waiting on this Condition.
	 * @param self The Condition.
	 * @remarks This method should only be called when the Condition is locked.
	 * @memberof Condition
	 */
	void (*broadcast)(Condition *self);

	/**
	 * @fn Condition *Condition::init(Condition *self)
	 * @brief Initializes this Condition.
	 * @param self The Condition.
	 * @return The initialized Condition, or `NULL` on error.
	 * @memberof Condition
	 */
	Condition *(*init)(Condition *self);

	/**
	 * @fn void Condition::signal(Condition *self)
	 * @brief Signals a single Thread waiting on this Condition.
	 * @param self The Condition.
	 * @remarks This method should only be called when the Condition is locked.
	 * @memberof Condition
	 */
	void (*signal)(Condition *self);

	/**
	 * @fn void Condition::wait(Condition *self)
	 * @brief Waits indefinitely for this Condition to be signaled.
	 * @param self The Condition.
	 * @remarks This method should only be called when the Condition is locked.
	 * @memberof Condition
	 */
	void (*wait)(Condition *self);

	/**
	 * @fn _Bool Condition::waitUntilDate(Condition *self, const Date *date)
	 * @brief Waits until the specified Date for this Condition to be signaled.
	 * @param self The Condition.
	 * @param date The Date until which to wait.
	 * @return `true` if this Condition was signaled before `date`, `false` otherwise.
	 * @remarks This method should only be called when the Condition is locked.
	 * @memberof Condition
	 */
	_Bool (*waitUntilDate)(Condition *self, const Date *date);
};

/**
 * @fn Class *Condition::_Condition(void)
 * @brief The Condition archetype.
 * @return The Condition Class.
 * @memberof Condition
 */
OBJECTIVELY_EXPORT Class *_Condition(void);
