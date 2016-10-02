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
#include <Objectively/Operation.h>
#include <Objectively/Thread.h>

/**
 * @file
 * @brief OperationQueues provide a thread of execution for Operations.
 */

typedef struct OperationQueue OperationQueue;
typedef struct OperationQueueInterface OperationQueueInterface;

/**
 * @brief OperationQueues provide a thread of execution for Operations.
 * @extends Object
 * @ingroup Concurrency
 */
struct OperationQueue {

	/**
	 * @brief The parent.
	 * @private
	 */
	Object object;

	/**
	 * @brief The typed interface.
	 * @private
	 */
	OperationQueueInterface *interface;

	/**
	 * @private
	 */
	struct {

		/**
		 * @brief A condition signaled on `addOperation` and `removeOperation`.
		 */
		Condition *condition;

		/**
		 * @brief The Operations.
		 */
		MutableArray *operations;

		/**
		 * @brief The backing Thread.
		 */
		Thread *thread;

	} locals;

	/**
	 * @brief When `true`, the queue will not `start` any new Operations.
	 */
	_Bool isSuspended;
};

/**
 * @brief The OperationQueue interface.
 */
struct OperationQueueInterface {

	/**
	 * @brief The parent interface.
	 */
	ObjectInterface objectInterface;

	/**
	 * @fn void OperationQueue::addOperation(OperationQueue *self, Operation *operation)
	 * @brief Adds an Operation to this queue.
	 * @param self The OperationQueue.
	 * @param operation The Operation to add.
	 * @memberof OperationQueue
	 */
	void (*addOperation)(OperationQueue *self, Operation *operation);

	/**
	 * @fn void OperationQueue::cancelAllOperations(OperationQueue *self)
	 * @brief Cancels all pending Operations residing within this Queue.
	 * @param self The OperationQueue.
	 * @memberof OperationQueue
	 */
	void (*cancelAllOperations)(OperationQueue *self);

	/**
	 * @static
	 * @fn OperationQueue* OperationQueue::currentQueue(void)
	 * @return The current OperationQueue, or `NULL` if none can be determined.
	 * @remarks This method should only be called from a synchronous Operation that was dispatched 
	 * via an OperationQueue. This method uses thread-local storage.
	 * @memberof OperationQueue
	 */
	OperationQueue *(*currentQueue)(void);

	/**
	 * @fn OperationQueue *OperationQueue::init(OperationQueue *self)
	 * @brief Initializes this OperationQueue.
	 * @param self The OperationQueue.
	 * @return The initialized OperationQueue, or `NULL` on error.
	 * @memberof OperationQueue
	 */
	OperationQueue *(*init)(OperationQueue *self);

	/**
	 * @fn int OperationQueue::operationCount(const OperationQueue *self)
	 * @param self The OperationQueue.
	 * @return The instantaneous `count` of this OperationQueue's Operations.
	 * @memberof OperationQueue
	 */
	int (*operationCount)(const OperationQueue *self);

	/**
	 * @fn Array *OperationQueue::operations(const OperationQueue *self)
	 * @param self The OperationQueue.
	 * @return An instantaneous copy of this OperationQueue's Operations.
	 * @memberof OperationQueue
	 */
	Array *(*operations)(const OperationQueue *self);

	/**
	 * @fn void OperationQueue::removeOperation(OperationQueue *self, Operation *operation)
	 * @brief Removes the Operation from this queue.
	 * @param self The OperationQueue.
	 * @param operation The Operation to remove.
	 * @memberof OperationQueue
	 */
	void (*removeOperation)(OperationQueue *self, Operation *operation);

	/**
	 * @fn void OperationQueue::waitUntilAllOperationsAreFinished(OperationQueue *self)
	 * @brief Waits until all Operations submitted to this queue have finished.
	 * @param self The OperationQueue.
	 * @memberof OperationQueue
	 */
	void (*waitUntilAllOperationsAreFinished)(OperationQueue *self);
};

/**
 * @brief The OperationQueue Class.
 */
extern Class _OperationQueue;
