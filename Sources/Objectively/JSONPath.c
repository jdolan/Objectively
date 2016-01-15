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

#include <Objectively/Array.h>
#include <Objectively/Dictionary.h>
#include <Objectively/JSONPath.h>
#include <Objectively/Once.h>
#include <Objectively/Regex.h>

#define _Class _JSONPath

#pragma mark - JSONPathInterface

static Regex *_regex;

/**
 * @fn ident JSONPATH::objectWithPath(const ident root, const char *path)
 *
 * @memberof JSONPath
 */
static ident objectWithPath(const ident root, const char *path) {

	ident obj = root;
	const char *c = path;

	while (obj) {

		Range *matches;
		if ($(_regex, matchesCharacters, c, 0, &matches) == false) {
			break;
		}

		const byte *bytes = (byte *) c + matches[1].location;
		const size_t length = matches[1].length;

		String *segment = $$(String, stringWithBytes, bytes, length, STRING_ENCODING_UTF8);
		c += length;

		if (*segment->chars == '.') {

			Dictionary *dictionary = cast(Dictionary, obj);

			const Range range = { .location = 1, .length = segment->length - 1 };
			String *key = $(segment, substring, range);

			obj = $(dictionary, objectForKey, key);

			release(key);

		} else if (*segment->chars == '[') {

			Array *array = cast(Array, obj);

			const unsigned index = strtoul(segment->chars + 1, NULL, 10);
			if (index < array->count) {
				obj = $(array, objectAtIndex, index);
			} else {
				obj = NULL;
			}
		}

		release(segment);
	}

	return obj;
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	((JSONPathInterface *) clazz->interface)->objectWithPath = objectWithPath;

	static Once once;

	DispatchOnce(once, {
		_regex = $(alloc(Regex), initWithPattern, "(.[^.\[]+|\[[0-9]+\\])", 0);
	});
}

/**
 * @see Class::destroy(Class *)
 */
static void destroy(Class *clazz) {

	release(_regex);
}

Class _JSONPath = {
	.name = "JSONPath",
	.superclass = &_Object,
	.instanceSize = sizeof(JSONPath),
	.interfaceOffset = offsetof(JSONPath, interface),
	.interfaceSize = sizeof(JSONPathInterface),
	.initialize = initialize,
	.destroy = destroy,
};

#undef _Class
