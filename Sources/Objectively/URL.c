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

#include <Objectively/MutableString.h>
#include <Objectively/Regex.h>
#include <Objectively/URL.h>

#define _Class _URL

static Regex *_regex;

#pragma mark - Object

/**
 * @see Object::copy(const Object *)
 */
static Object *copy(const Object *self) {

	URL *this = (URL *) self;

	return (Object *) $alloc(URL, initWithString, this->urlString);
}

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

	URL *this = (URL *) self;

	release(this->fragment);
	release(this->host);
	release(this->path);
	release(this->query);
	release(this->scheme);
	release(this->urlString);

	super(Object, self, dealloc);
}

/**
 * @see Object::description(const Object *)
 */
static String *description(const Object *self) {

	const URL *this = (URL *) self;

	return $((Object *) this->urlString, description);
}

/**
 * @see Object::hash(const Object *self)
 */
static int hash(const Object *self) {

	const URL *this = (URL *) self;

	return $((Object *) this->urlString, hash);
}

/**
 * @see Object::isEqual(const Object *, const Object *)
 */
static _Bool isEqual(const Object *self, const Object *other) {

	if (super(Object, self, isEqual, other)) {
		return true;
	}

	if (other && self->clazz == other->clazz) {

		const URL *this = (URL *) self;
		const URL *that = (URL *) other;

		return $((Object *) this->urlString, isEqual, (Object *) that->urlString);
	}

	return false;
}

#pragma mark - URL

/**
 * @fn URL *URL::baseURL(const URL *self)
 *
 * @memberof URL
 */
static URL *baseURL(const URL *self) {

	MutableString *string = $alloc(MutableString, init);

	$(string, appendString, self->scheme);
	$(string, appendFormat, "://");

	if (self->host) {
		$(string, appendString, self->host);
	}

	if (self->port) {
		$(string, appendFormat, ":%u", self->port);
	}

	URL *baseURL = $alloc(URL, initWithString, (String *) string);

	release(string);

	return baseURL;
}

/**
 * @fn URL *URL::initWithCharacters(URL *self, const char *chars)
 *
 * @memberof URL
 */
static URL *initWithCharacters(URL *self, const char *chars) {

	assert(chars);

	self = (URL *) super(Object, self, init);
	if (self) {

		Range *matches;
		if ($(_regex, matchesCharacters, chars, 0, &matches)) {

			self->urlString = $alloc(String, initWithCharacters, chars);

			Range *scheme = &matches[1];
			Range *host = &matches[2];
			Range *port = &matches[3];
			Range *path = &matches[4];
			Range *query = &matches[5];
			Range *fragment = &matches[6];

			self->scheme = $(self->urlString, substring, *scheme);

			if (host->location > -1) {
				self->host = $(self->urlString, substring, *host);

				if (port->location > -1) {
					const char *s = self->urlString->chars + port->location + 1;
					self->port = strtoul(s, NULL, 10);
				}
			}

			if (path->location > -1) {
				self->path = $(self->urlString, substring, *path);
			}

			if (query->location > -1) {

				query->location++;
				query->length--;

				self->query = $(self->urlString, substring, *query);
			}

			if (fragment->location > -1) {

				fragment->location++;
				fragment->length--;

				self->fragment = $(self->urlString, substring, *fragment);
			}

			free(matches);
		} else {
			release(self);
			self = NULL;
		}
	}

	return self;
}

/**
 * @fn URL *URL::initWithString(URL *self, const String *string)
 *
 * @memberof URL
 */
static URL *initWithString(URL *self, const String *string) {

	assert(string);

	return $(self, initWithCharacters, string->chars);
}

/**
 * @fn Array *URL::pathComponents(const URL *self)
 *
 * @memberof URL
 */
static Array *pathComponents(const URL *self) {

	return $(self->path, componentsSeparatedByCharacters, "/");
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	ObjectInterface *object = (ObjectInterface *) clazz->interface;

	object->copy = copy;
	object->dealloc = dealloc;
	object->description = description;
	object->hash = hash;
	object->isEqual = isEqual;

	URLInterface *url = (URLInterface *) clazz->interface;

	url->baseURL = baseURL;
	url->initWithCharacters = initWithCharacters;
	url->initWithString = initWithString;
	url->pathComponents = pathComponents;

	_regex = $alloc(Regex, initWithPattern, "([a-z]+)://([^:/\?]+)?(:[0-9]+)?(/[^\?#]+)?([^#]+)?(#.*)?", 0);
}

/**
 * @see Class::destroy(Class *)
 */
static void destroy(Class *clazz) {

	release(_regex);
}

Class _URL = {
	.name = "URL",
	.superclass = &_Object,
	.instanceSize = sizeof(URL),
	.interfaceOffset = offsetof(URL, interface),
	.interfaceSize = sizeof(URLInterface),
	.initialize = initialize,
	.destroy = destroy,
};

#undef _Class
