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

#include <Objectively/JSON.h>

#define __Class __JSON

#pragma mark - ObjectInterface

/**
 * @see ObjectInterface::dealloc(Object *)
 */
static void dealloc(Object *self) {

	// TODO

	super(Object, self, dealloc);
}

#pragma mark - JSONInterface

/**
 * @see JSONInterface::dataFromObject(JSON *, const id)
 */
static Data *dataFromObject(JSON *self, const id obj) {

	Data *data = $(alloc(Data), init);

	// TODO

	return data;
}

/**
 * @see JSONInterface::initWithOptions(JSON *)
 */
static JSON *initWithOptions(JSON *self, int options) {

	self = (JSON *) super(Object, self, init);
	if (self) {

		//..
	}

	return self;
}

/**
 * @see JSONInterface::objectFromData(JSON *, const Data *)
 */
static id objectFromData(JSON *self, const Data *data) {

	// TODO

	return NULL;
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	((ObjectInterface *) clazz->interface)->dealloc = dealloc;

	JSONInterface *json = (JSONInterface *) clazz->interface;

	json->dataFromObject = dataFromObject;
	json->initWithOptions = initWithOptions;
	json->objectFromData = objectFromData;
}

Class __JSON = {
	.name = "JSON",
	.superclass = &__Object,
	.instanceSize = sizeof(JSON),
	.interfaceOffset = offsetof(JSON, interface),
	.interfaceSize = sizeof(JSONInterface),
	.initialize = initialize,
};

#undef __Class
