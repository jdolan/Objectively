/*
 * URLSessionConfiguration.c
 *  Created on: Nov 30, 2014
 *      Author: jdolan
 */


#include <assert.h>

#include <Objectively/URLSessionConfiguration.h>

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

	((ObjectInterface *) clazz->def->interface)->dealloc = dealloc;

	((URLSessionConfigurationInterface *) clazz->def->interface)->init = init;
}

Class *_URLSessionConfiguration(void) {
	static Class clazz;
	static Once once;
	
	do_once(&once, {
		clazz.name = "URLSessionConfiguration";
		clazz.superclass = _Object();
		clazz.instanceSize = sizeof(URLSessionConfiguration);
		clazz.interfaceOffset = offsetof(URLSessionConfiguration, interface);
		clazz.interfaceSize = sizeof(URLSessionConfigurationInterface);
		clazz.initialize = initialize;
	});

	return &clazz;
}

#undef _Class
