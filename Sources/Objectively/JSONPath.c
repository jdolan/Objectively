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

#include "Array.h"
#include "Boole.h"
#include "Dictionary.h"
#include "JSONPath.h"
#include "Number.h"
#include "Regexp.h"
#include "String.h"

#define _Class _JSONPath

#pragma mark - JSONPath

static Regexp *_re;

/**
 * @fn ident JSONPATH::objectForKeyPath(const ident root, const char *path)
 * @memberof JSONPath
 */
static ident objectForKeyPath(const ident root, const char *path) {

	assert(root);
	assert(path);

	assert(*path == '$');
	const char *c = path;

	ident obj = root;
	while (obj) {

		Range *matches;
		if ($(_re, matchesCharacters, c, 0, &matches) == false) {
			break;
		}

		const uint8_t *bytes = (uint8_t *) c + matches[1].location;
		const size_t length = matches[1].length;

		String *segment = $$(String, stringWithBytes, bytes, length, STRING_ENCODING_UTF8);
		if (*segment->chars == '.') {

			const Dictionary *dictionary = cast(Dictionary, obj);
			const Range range = { .location = 1, .length = segment->length - 1 };

			String *key = $(segment, substring, range);

			obj = $(dictionary, objectForKey, key);

			release(key);

		} else if (*segment->chars == '[') {

			const Array *array = cast(Array, obj);
			const unsigned long index = strtoul(segment->chars + 1, NULL, 10);
			if (index < array->count) {
				obj = $(array, objectAtIndex, index);
			} else {
				obj = NULL;
			}
		}
		release(segment);

		c += length;
	}

	return obj;
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	((JSONPathInterface *) clazz->interface)->objectForKeyPath = objectForKeyPath;

	_re = re("(.[^.\\[]+|\\[[0-9]+\\])", 0);
}

/**
 * @see Class::destroy(Class *)
 */
static void destroy(Class *clazz) {

	release(_re);
}

/**
 * @fn Class *JSONPath::_JSONPath(void)
 * @memberof JSONPath
 */
Class *_JSONPath(void) {
	static Class *clazz;
	static Once once;

	do_once(&once, {
		clazz = _initialize(&(const ClassDef) {
			.name = "JSONPath",
			.superclass = _Object(),
			.instanceSize = sizeof(JSONPath),
			.interfaceOffset = offsetof(JSONPath, interface),
			.interfaceSize = sizeof(JSONPathInterface),
			.initialize = initialize,
			.destroy = destroy,
		});
	});

	return clazz;
}

#undef _Class
