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

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "MutableArray.h"
#include "Resource.h"
#include "String.h"
#include "Value.h"

#define _Class _Resource

static MutableArray *_resourcePaths;
static MutableArray *_resourceProviders;

#pragma mark - Object

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

	Resource *this = (Resource *) self;

	release(this->data);

	free(this->name);

	super(Object, self, dealloc);
}

#pragma mark - Resource

/**
 * @fn void Resource::addResourcePath(const char *path)
 * @memberof Resource
 */
static void addResourcePath(const char *path) {

	assert(path);
	String *string = str(path);

	$(_resourcePaths, addObject, string);
	release(string);
}

/**
 * @fn void Resource::addResourceProvider(ResourceProvider provider)
 * @memberof Resource
 */
static void addResourceProvider(ResourceProvider provider) {

	assert(provider);
	Value *value = $(alloc(Value), initWithValue, provider);
	
	$(_resourceProviders, addObject, value);
	release(value);
}

/**
 * @fn Resource *Resource::initWithBytes(Resource *self, const uint8_t *bytes, size_t length, const char *name)
 * @memberof Resource
 */
static Resource *initWithBytes(Resource *self, const uint8_t *bytes, size_t length, const char *name) {

	Data *data = $$(Data, dataWithBytes, bytes, length);
	self = $(self, initWithData, data, name);

	release(data);
	return self;
}

/**
 * @fn Resource *Resource::initWithData(Resource *self, Data *data)
 * @memberof Resource
 */
static Resource *initWithData(Resource *self, Data *data, const char *name) {

	self = (Resource *) super(Object, self, init);
	if (self) {
		assert(data);
		self->data = retain(data);

		assert(name);
		self->name = strdup(name);
	}

	return self;
}

/**
 * @fn Resource *Resource::initWithName(Resource *self, const char *name)
 * @memberof Resource
 */
static Resource *initWithName(Resource *self, const char *name) {

	Data *data = NULL;

	const Array *resourceProviders = (Array *) _resourceProviders;
	for (size_t i = 0; i < resourceProviders->count && data == NULL; i++) {

		const Value *value = $(resourceProviders, objectAtIndex, i);
		data = ((ResourceProvider) (value->value))(name);
	}

	const Array *resourcePaths = (Array *) _resourcePaths;
	for (size_t i = 0; i < resourcePaths->count && data == NULL; i++) {

		const String *resourcePath = $(resourcePaths, objectAtIndex, i);
		String *path = str("%s%s%s", resourcePath->chars, PATH_SEPAR, name);

		struct stat s;
		if (stat(path->chars, &s) == 0 && S_ISREG(s.st_mode)) {
			data = $$(Data, dataWithContentsOfFile, path->chars);
		}

		release(path);
	}

	if (data) {
		self = $(self, initWithData, data, name);
	} else {
		self = release(self);
	}

	release(data);

	return self;
}

/**
 * @fn void Resource::removeResourcePath(const char *path)
 * @memberof Resource
 */
static void removeResourcePath(const char *path) {

	String *string = $$(String, stringWithCharacters, path);

	$(_resourcePaths, removeObject, string);

	release(string);
}

/**
 * @fn void Resource::removeResourceProvider(ResourceProvider provider)
 * @memberof Resource
 */
static void removeResourceProvider(ResourceProvider provider) {

	Value *value = $(alloc(Value), initWithValue, provider);

	$(_resourceProviders, removeObject, value);

	release(value);
}

/**
 * @fn Resource *Resource::resourceWithName(const char *name)
 * @memberof Resource
 */
static Resource *resourceWithName(const char *name) {
	return $(alloc(Resource), initWithName, name);
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	((ObjectInterface *) clazz->interface)->dealloc = dealloc;

	((ResourceInterface *) clazz->interface)->addResourcePath = addResourcePath;
	((ResourceInterface *) clazz->interface)->addResourceProvider = addResourceProvider;
	((ResourceInterface *) clazz->interface)->initWithBytes = initWithBytes;
	((ResourceInterface *) clazz->interface)->initWithData = initWithData;
	((ResourceInterface *) clazz->interface)->initWithName = initWithName;
	((ResourceInterface *) clazz->interface)->removeResourcePath = removeResourcePath;
	((ResourceInterface *) clazz->interface)->removeResourceProvider = removeResourceProvider;
	((ResourceInterface *) clazz->interface)->resourceWithName = resourceWithName;

	_resourcePaths = $(alloc(MutableArray), init);
	assert(_resourcePaths);

	const char *env = getenv("OBJECTIVELY_RESOURCE_PATH");
	if (env) {

		String *string = $$(String, stringWithCharacters, env);
		Array *paths = $(string, componentsSeparatedByCharacters, PATH_DELIM);

		for (size_t i = 0; i < paths->count; i++) {
			addResourcePath(((String *) $(paths, objectAtIndex, i))->chars);
		}

		release(paths);
		release(string);
	}

	addResourcePath(".");

	_resourceProviders = $(alloc(MutableArray), init);
	assert(_resourceProviders);
}

/**
 * @see Class::destroy(Class *)
 */
static void destroy(Class *clazz) {
	release(_resourcePaths);
	release(_resourceProviders);
}

/**
 * @fn Class *Resource::_Resource(void)
 * @memberof Resource
 */
Class *_Resource(void) {
	static Class *clazz;
	static Once once;

	do_once(&once, {
		clazz = _initialize(&(const ClassDef) {
			.name = "Resource",
			.superclass = _Object(),
			.instanceSize = sizeof(Resource),
			.interfaceOffset = offsetof(Resource, interface),
			.interfaceSize = sizeof(ResourceInterface),
			.initialize = initialize,
			.destroy = destroy,
		});
	});

	return clazz;
}

#undef _Class

