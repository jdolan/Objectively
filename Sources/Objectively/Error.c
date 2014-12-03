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

#include <Objectively/Error.h>
#include <Objectively/Hash.h>

#define __Class __Error

#pragma mark - ObjectInterface

/**
 * @see ObjectInterface::copy(const Object *)
 */
static Object *copy(const Object *self) {

	Error *this = (Error *) self;

	const char *message = this->message ? this->message->chars : NULL;

	return (Object *) $(alloc(Error), initWithDomain, this->domain->chars, this->code, message);
}

/**
 * @see ObjectInterface::dealloc(Object *)
 */
static void dealloc(Object *self) {

	Error *this = (Error *) self;

	release(this->domain);
	release(this->message);

	super(Object, self, dealloc);
}

/**
 * @see ObjectInterface::description(const Object *)
 */
static String *description(const Object *self) {

	Error *this = (Error *) self;

	String *description = $(alloc(String), initWithFormat, "%s: %d", this->domain->chars,
			this->code);

	if (this->message) {
		$(description, appendFormat, ": %s", this->message->chars);
	}

	return description;
}

/**
 * @see ObjectInterface::hash(const Object *)
 */
static int hash(const Object *self) {

	Error *this = (Error *) self;

	int hash = HASH_SEED;

	hash = HashForInteger(hash, this->code);
	hash = HashForObject(hash, this->domain);
	hash = HashForObject(hash, this->message);

	return hash;
}

/**
 * @see ObjectInterface::isEqual(const Object *, const Object *)
 */
static BOOL isEqual(const Object *self, const Object *other) {

	if (super(Object, self, isEqual, other)) {
		return YES;
	}

	if (other && (self->clazz == other->clazz)) {

		const Error *this = (Error *) self;
		const Error *that = (Error *) other;

		if (this->code == that->code) {

			if ($((Object * ) this->domain, isEqual, (Object * ) that->domain)) {

				if (this->message) {
					return $((Object * ) this->message, isEqual, (Object * ) that->message);
				}

				return that->message == NULL;
			}
		}
	}

	return NO;
}

#pragma mark - ErrorInterface

/**
 * @see ErrorInterface::initWithDomain(Error *, const char *, int, cons char *)
 */
static Error *initWithDomain(Error *self, const char *domain, int code, const char *message) {

	assert(domain);

	self = (Error *) super(Object, self, init);
	if (self) {

		self->domain = $(alloc(String), initWithCharacters, domain);
		self->code = code;

		if (message) {
			self->message = $(alloc(String), initWithCharacters, message);
		}
	}

	return self;
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

	ErrorInterface *error = (ErrorInterface *) clazz->interface;

	error->initWithDomain = initWithDomain;
}

Class __Error = {
	.name = "Error",
	.superclass = &__Object,
	.instanceSize = sizeof(Error),
	.interfaceOffset = offsetof(Error, interface),
	.interfaceSize = sizeof(ErrorInterface),
	.initialize = initialize,
};

#undef __Class
