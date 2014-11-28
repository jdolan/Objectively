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

#include <assert.h>

#include <Objectively/URLSessionDownloadTask.h>

#define __Class __URLSessionDownloadTask

#pragma mark - ObjectInterface

/**
 * @see ObjectInterface::dealloc(Object *)
 */
static void dealloc(Object *self) {

	URLSessionDownloadTask *this = (URLSessionDownloadTask *) self;

	if (this->file) {
		fclose(this->file);
	}

	super(Object, self, dealloc);
}

#pragma mark - URLSessionTaskInterface

/**
 * @see URLSessionTaskInterface::initWithRequestInSession(URLSessionTask *, struct URLRequest *, struct URLSession *)
 */
static URLSessionTask *initWithRequestInSession(URLSessionTask *self,
		struct URLRequest *request,
		struct URLSession *session) {

	self = super(URLSessionTask, self, initWithRequestInSession, request, session);
	if (self) {

		URLSessionDownloadTask *this = (URLSessionDownloadTask *) self;

		// TODO
	}

	return self;
}

#pragma mark - Class lifecycle

/**
 * see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	((ObjectInterface *) clazz->interface)->dealloc = dealloc;

	((URLSessionTaskInterface *) clazz->interface)->initWithRequestInSession =
			initWithRequestInSession;
}

Class __URLSessionDownloadTask = {
	.name = "URLSessionDownloadTask",
	.superclass = &__URLSessionTask,
	.instanceSize = sizeof(URLSessionDownloadTask),
	.interfaceOffset = offsetof(URLSessionDownloadTask, interface),
	.interfaceSize = sizeof(URLSessionDownloadTaskInterface),
	.initialize = initialize, };

#undef __Class
