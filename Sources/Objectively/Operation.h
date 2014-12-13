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

#ifndef _Objectively_Operation_h_
#define _Objectively_Operation_h_

#include <Objectively/Condition.h>
#include <Objectively/MutableArray.h>
#include <Objectively/Object.h>

/**
 * @file
 *
 * @brief An abstraction for discrete units of work, or tasks.
 *
 * Operations are typically executed via an OperationQueue.
 */

typedef struct Operation Operation;
typedef struct OperationInterface OperationInterface;

/**
 * @brief The function type for Operation execution.
 *
 * @param operation The executing Operation.
 */
typedef void (*OperationFunction)(Operation *operation);

/**
 * @brief An abstraction for discrete units of work, or tasks.
 *
 * Operations are typically executed via an OperationQueue.
 *
 * @ingroup Threads
 */
struct Operation {

	/**
	 * @brief The parent.
	 */
	Object object;

	/**
	 * @brief The typed interface.
	 */
	OperationInterface *interface;

	/**
	 * @private
	 */
	struct {

		/**
		 * @brief The Condition enabling `waitUntilFinished`.
		 */
		Condition *condition;

		/**
		 * @brief Contains Operations which must finish before this one can start.
		 */
		MutableArray *dependencies;

	} locals;

	/**
	 * @brief If `YES`, this Operation will be expected to coordinate its own
	 * concurrency and internal state management by overriding `start`.
	 */
	BOOL asynchronous;

	/**
	 * @brief The user data.
	 */
	id data;

	/**
	 * @brief The Operation function.
	 */
	OperationFunction function;

	/**
	 * @brief `YES` when this Thread has been cancelled, `NO` otherwise.
	 */
	BOOL isCancelled;

	/**
	 * @brief `YES` when this Thread is live, `NO` otherwise.
	 */
	BOOL isExecuting;

	/**
	 * @brief `YES` when this Operation is finished, `NO` otherwise.
	 */
	BOOL isFinished;
};

/**
 * @brief The Operation interface.
 *
 * @ingroup Threads
 */
struct OperationInterface {

	/**
	 * @brief The parent.
	 */
	ObjectInterface objectInterface;

	/**
	 * @brief Makes this Operation dependent on the completion of `dependency`.
	 *
	 * @param dependency The Operation to await.
	 *
	 * @relates Operation
	 */
	void (*addDependency)(Operation *self, Operation *dependency);

	/**
	 * @brief Cancels this Operation, allowing it to complete immediately.
	 *
	 * @relates Operation
	 */
	void (*cancel)(Operation *self);

	/**
	 * @return An instantaneous copy of this Operations' dependencies.
	 *
	 * @relates Operation
	 */
	Array *(*dependencies)(const Operation *self);

	/**
	 * @brief Initializes this Operation.
	 *
	 * @return The initialized Operation, or `NULL` on error.
	 *
	 * @remark Asynchronous subclasses should invoke this initializer.
	 *
	 * @relates Operation
	 */
	Operation *(*init)(Operation *self);

	/**
	 * @brief Initializes a *synchronous* Operation with the given `function`.
	 *
	 * @param function The OperationFunction to perform
	 * @param data The user data.
	 *
	 * @return The initialized Operation, or `NULL` on error.
	 *
	 * @relates Operation
	 */
	Operation *(*initWithFunction)(Operation *self, OperationFunction function, id data);

	/**
	 * @return `YES` when all criteria for this Operation to `start` are met.
	 *
	 * @relates Operation
	 */
	BOOL (*isReady)(const Operation *self);

	/**
	 * @brief Removes the dependency on `dependency`.
	 *
	 * @param dependency The dependency.
	 *
	 * @relates Operation
	 */
	void (*removeDependency)(Operation *self, Operation *dependency);

	/**
	 * @brief Starts this Operation.
	 *
	 * @remark The default implementation of this method checks the state of
	 * the Operation and, if all criteria are met, dispatches the configured
	 * `function` synchronously. When this method returns, the Operation
	 * `isFinished` and has removed itself from any queues it belonged to.
	 *
	 * @remark Asynchronous Operations should override this method and
	 * coordinate their own state transitions and queue removal. This method
	 * must not be invoked by `super`.
	 *
	 * @relates Operation
	 */
	void (*start)(Operation *self);

	/**
	 * @brief Blocks the current thread until this Operation `isFinished`.
	 *
	 * @relates Operation
	 */
	void (*waitUntilFinished)(const Operation *self);
};

/**
 * @brief The Operation Class.
 */
extern Class __Operation;

#endif
