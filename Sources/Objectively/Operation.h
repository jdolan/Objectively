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

#ifndef _Objectively_Operation_h
#define _Objectively_Operation_h

#include <Objectively/Array.h>
#include <Objectively/Condition.h>
#include <Objectively/Object.h>

typedef struct Operation Operation;
typedef struct OperationInterface OperationInterface;

/**
 * @brief The function type for Operation execution.
 *
 * @param operation The executing Operation.
 */
typedef void (*OperationFunction)(Operation *operation);

/**
 * @brief The Operation type.
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
		Array *dependencies;

	} locals;

	/**
	 * @brief If `YES`, this Operation will be expected to set its own TODO
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
 * @brief The Operation type.
 */
struct OperationInterface {

	/**
	 * @brief The parent.
	 */
	ObjectInterface objectInterface;

	void (*addDependency)(Operation *self, Operation *dependency);

	void (*cancel)(Operation *self);

	Array *(*dependencies)(const Operation *self);

	Operation *(*init)(Operation *self);

	Operation *(*initWithFunction)(Operation *self, OperationFunction function, id data);

	BOOL (*isReady)(const Operation *self);

	void (*removeDependency)(Operation *self, Operation *dependency);

	void (*start)(Operation *self);

	void (*waitUntilFinished)(const Operation *self);
};

/**
 * @brief The Operation Class.
 */
extern Class __Operation;

#endif
