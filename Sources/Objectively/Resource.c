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

#include <Objectively/MutableArray.h>
#include <Objectively/Resource.h>
#include <Objectively/String.h>

#define _Class _Resource

static MutableArray *_resourcePaths;

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
	String *temp = str(path);
	$(_resourcePaths, addObject, temp);
	release(temp);
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

	const Array *resourcePaths = (Array *) _resourcePaths;
	for (size_t i = 0; i < resourcePaths->count && data == NULL; i++) {

		const String *resourcePath = $(resourcePaths, objectAtIndex, i);
		String *path = str("%s/%s", resourcePath->chars, name);

		struct stat s;
		if (stat(path->chars, &s) == 0 && S_ISREG(s.st_mode)) {
			data = $$(Data, dataWithContentsOfFile, path->chars);
		}

		release(path);
	}

	return $(self, initWithData, data, name);
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
	
	((ObjectInterface *) clazz->def->interface)->dealloc = dealloc;

	((ResourceInterface *) clazz->def->interface)->addResourcePath = addResourcePath;
	((ResourceInterface *) clazz->def->interface)->initWithData = initWithData;
	((ResourceInterface *) clazz->def->interface)->initWithName = initWithName;
	((ResourceInterface *) clazz->def->interface)->removeResourcePath = removeResourcePath;
	((ResourceInterface *) clazz->def->interface)->resourceWithName = resourceWithName;

	_resourcePaths = $(alloc(MutableArray), init);
	assert(_resourcePaths);

	String *temp = str(".");
	$(_resourcePaths, addObject, temp);
	release(temp);
}

/**
 * @see Class::destroy(Class *)
 */
static void destroy(Class *clazz) {
	release(_resourcePaths);
}

Class *_Resource(void) {
	static Class clazz;
	static Once once;
	
	do_once(&once, {
		clazz.name = "Resource";
		clazz.superclass = _Object();
		clazz.instanceSize = sizeof(Resource);
		clazz.interfaceOffset = offsetof(Resource, interface);
		clazz.interfaceSize = sizeof(ResourceInterface);
		clazz.initialize = initialize;
		clazz.destroy = destroy;
	});

	return &clazz;
}

#undef _Class

