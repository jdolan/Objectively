/*
 * URLSessionConfiguration.c
 *  Created on: Nov 30, 2014
 *      Author: jdolan
 */


#include <assert.h>

#include "URLSessionConfiguration.h"

#define _Class _URLSessionConfiguration

#pragma mark - Object

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

	URLSessionConfiguration *this = (URLSessionConfiguration *) self;

	release(this->credentials.username);
	release(this->credentials.password);
	release(this->httpHeaders);

	super(Object, self, dealloc);
}

#pragma mark - URLSessionConfiguration

/**
 * @fn URLSessionConfiguration *URLSessionConfiguration::init(URLSessionConfiguration *self)
 * @memberof URLSessionConfiguration
 */
static URLSessionConfiguration *init(URLSessionConfiguration *self) {

	self = (URLSessionConfiguration *) super(Object, self, init);
	if (self) {
		// wut
	}

	return self;
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	((ObjectInterface *) clazz->interface)->dealloc = dealloc;

	((URLSessionConfigurationInterface *) clazz->interface)->init = init;
}

/**
 * @fn Class *URLSessionConfiguration::_URLSessionConfiguration(void)
 * @memberof URLSessionConfiguration
 */
Class *_URLSessionConfiguration(void) {
	static Class *clazz;
	static Once once;

	do_once(&once, {
		clazz = _initialize(&(const ClassDef) {
			.name = "URLSessionConfiguration",
			.superclass = _Object(),
			.instanceSize = sizeof(URLSessionConfiguration),
			.interfaceOffset = offsetof(URLSessionConfiguration, interface),
			.interfaceSize = sizeof(URLSessionConfigurationInterface),
			.initialize = initialize,
		});
	});

	return clazz;
}

#undef _Class
