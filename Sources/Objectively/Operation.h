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

#include <Objectively/Condition.h>
#include <Objectively/Object.h>
#include <Objectively/MutableArray.h>

/**
 * @file
 * @brief An abstraction for discrete units of work, or tasks.
 */

typedef struct Operation Operation;
typedef struct OperationInterface OperationInterface;

/**
 * @brief The function type for Operation execution.
 * @param operation The executing Operation.
 */
typedef void (*OperationFunction)(Operation *operation);

/**
 * @brief An abstraction for discrete units of work, or tasks.
 * @details Operations are typically executed via an OperationQueue.
 * @extends Object
 * @ingroup Concurrency
 */
struct Operation {

	/**
	 * @brief The parent.
	 */
	Object object;

	/**
	 * @brief The typed interface.
	 * @protected
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
	 * @brief If `true`, this Operation will be expected to coordinate its own
	 * concurrency and internal state management by overriding `start`.
	 */
	_Bool asynchronous;

	/**
	 * @brief The user data.
	 */
	ident data;

	/**
	 * @brief The Operation function.
	 */
	OperationFunction function;

	/**
	 * @brief `true` when this Operation has been cancelled, `false` otherwise.
	 */
	_Bool isCancelled;

	/**
	 * @brief `true` when this Operation is executing, `false` otherwise.
	 */
	_Bool isExecuting;

	/**
	 * @brief `true` when this Operation is finished, `false` otherwise.
	 */
	_Bool isFinished;
};

/**
 * @brief The Operation interface.
 */
struct OperationInterface {

	/**
	 * @brief The parent interface.
	 */
	ObjectInterface objectInterface;

	/**
	 * @fn void Operation::addDependency(Operation *self, Operation *dependency)
	 * @brief Makes this Operation dependent on the completion of `dependency`.
	 * @param self The Operation.
	 * @param dependency The Operation to await.
	 * @memberof Operation
	 */
	void (*addDependency)(Operation *self, Operation *dependency);

	/**
	 * @fn void Operation::cancel(Operation *self)
	 * @brief Cancels this Operation, allowing it to complete immediately.
	 * @param self The Operation.
	 * @memberof Operation
	 */
	void (*cancel)(Operation *self);

	/**
	 * @fn Array *Operation::dependencies(const Operation *self)
	 * @param self The Operation.
	 * @return An instantaneous copy of this Operations' dependencies.
	 * @memberof Operation
	 */
	Array *(*dependencies)(const Operation *self);

	/**
	 * @fn Operation *Operation::init(Operation *self)
	 * @brief Initializes this Operation.
	 * @param self The Operation.
	 * @return The initialized Operation, or `NULL` on error.
	 * @remarks Asynchronous subclasses should invoke this initializer.
	 * @memberof Operation
	 */
	Operation *(*init)(Operation *self);

	/**
	 * @fn Operation *Operation::initWithFunction(Operation *self, OperationFunction function, ident data)
	 * @brief Initializes a *synchronous* Operation with the given `function`.
	 * @param self The Operation.
	 * @param function The OperationFunction to perform
	 * @param data The user data.
	 * @return The initialized Operation, or `NULL` on error.
	 * @memberof Operation
	 */
	Operation *(*initWithFunction)(Operation *self, OperationFunction function, ident data);

	/**
	 * @fn _Bool Operation::isReady(const Operation *self)
	 * @param self The Operation.
	 * @return `true` when all criteria for this Operation to `start` are met.
	 * @memberof Operation
	 */
	_Bool (*isReady)(const Operation *self);

	/**
	 * @fn void Operation::removeDependency(Operation *self, Operation *dependency)
	 * @brief Removes the dependency on `dependency`.
	 * @param self The Operation.
	 * @param dependency The dependency.
	 * @memberof Operation
	 */
	void (*removeDependency)(Operation *self, Operation *dependency);

	/**
	 * @fn void Operation::start(Operation *self)
	 * @brief Starts this Operation.
	 * @param self The Operation.
	 * @remarks The default implementation of this method checks the state of the Operation and, if 
	 * all criteria are met, dispatches the configured `function` synchronously. When this method 
	 * returns, the Operation `isFinished` and has removed itself from any queues it belonged to.
	 * @remarks Asynchronous Operations should override this method and coordinate their own state
	 * transitions and queue removal. This method should _not_ be invoked by `super`.
	 * @memberof Operation
	 */
	void (*start)(Operation *self);

	/**
	 * @fn void Operation::waitUntilFinished(const Operation *self)
	 * @brief Blocks the current thread until this Operation `isFinished`.
	 * @param self The Operation.
	 * @memberof Operation
	 */
	void (*waitUntilFinished)(const Operation *self);
};

/**
 * @brief The Operation Class.
 */
extern Class _Operation;
