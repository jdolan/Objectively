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

#include <Objectively/Data.h>

#if defined(_WIN32)
 #define PATH_DELIM ";"
 #define PATH_SEPAR "\\"
#else
 #define PATH_DELIM ":"
 #define PATH_SEPAR "/"
#endif

/**
 * @file
 * @brief Resources provide an abstraction for file and stream resources.
 */

typedef struct Resource Resource;
typedef struct ResourceInterface ResourceInterface;

/**
 * @brief Applications may specify a provider function for loading via file system abstractions.
 */
typedef Data *(*ResourceProvider)(const char *name);

/**
 * @brief Resources provide an abstraction for file and stream resources.
 * @extends Object
 */
struct Resource {

	/**
	 * @brief The superclass.
	 */
	Object object;

	/**
	 * @brief The interface.
	 * @protected
	 */
	ResourceInterface *interface;

	/**
	 * @brief The resource data.
	 */
	Data *data;

	/**
	 * @brief The resource name.
	 */
	char *name;
};

/**
 * @brief The Resource interface.
 */
struct ResourceInterface {

	/**
	 * @brief The superclass interface.
	 */
	ObjectInterface objectInterface;

	/**
	 * @static
	 * @fn void Resource::addResourcePath(const char *path)
	 * @brief Adds the specified Resource path.
	 * @param path The resource path to add.
	 * @remarks Resource paths may be relative to the working directory, or absolute.
	 * @memberof Resource
	 */
	void (*addResourcePath)(const char *path);

	/**
	 * @static
	 * @fn void Resource::addResourceProvider(ResourceProvider provider)
	 * @brief Adds the specified ResourceProvider.
	 * @param provider The ResoureProvider to add.
	 * @memberof Resource
	 */
	void (*addResourceProvider)(ResourceProvider provider);

	/**
	 * @fn Resource *Resource::initWithBytes(Resource *self, const uint8_t *bytes, size_t length, const char *name)
	* @brief Initializes this Resource with the specified bytes.
	 * @param self The Resource.
	 * @param bytes The bytes.
	 * @param length The length of bytes.
	 * @param name The resource name.
	 * @return The initialized Resource, or `NULL` on error.
	 * @memberof Resource
	 */
	Resource *(*initWithBytes)(Resource *self, const uint8_t *bytes, size_t length, const char *name);

	/**
	 * @fn Resource *Resource::initWithData(Resource *self, Data *data, const char *name)
	 * @brief Initializes this Resource with the specified Data.
	 * @param self The Resource.
	 * @param data The Data.
	 * @param name The resource name.
	 * @return The initialized Resource, or `NULL` on error.
	 * @memberof Resource
	 */
	Resource *(*initWithData)(Resource *self, Data *data, const char *name);

	/**
	 * @fn Resource *Resource::initWithName(Resource *self, const char *name)
	 * @brief Initializes this Resource with the specified `name`.
	 * @param self The Resource.
	 * @param name The resource name.
	 * @return The initialized Resource, or `NULL` on error.
	 * @details The configured resource paths are searched, in order, for a file by the given name.
	 * @memberof Resource
	 */
	Resource *(*initWithName)(Resource *self, const char *name);

	/**
	 * @static
	 * @fn void Resource::removeResourcePath(const char *path)
	 * @brief Removes the specified Resource path.
	 * @param path The resource path to remove.
	 * @memberof Resource
	 */
	void (*removeResourcePath)(const char *path);

	/**
	 * @static
	 * @fn void Resource::removeResourceProvider(ResourceProvider provider)
	 * @brief Removes the specified ResourceProvider.
	 * @param provider The ResourceProvider to remove.
	 * @memberof Resource
	 */
	void (*removeResourceProvider)(ResourceProvider provider);

	/**
	 * @static
	 * @fn Resource *Resource::resourceWithName(const char *name)
	 * @brief Returns a new Resource with the specified `name`.
	 * @param name The resource name.
	 * @return The new Resource, or `NULL` on error.
	 * @memberof Resource
	 */
	Resource *(*resourceWithName)(const char *name);
};

/**
 * @fn Class *Resource::_Resource(void)
 * @brief The Resource archetype.
 * @return The Resource Class.
 * @memberof Resource
 */
OBJECTIVELY_EXPORT Class *_Resource(void);
