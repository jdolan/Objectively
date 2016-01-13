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
#include <string.h>

#include <Objectively/Hash.h>
#include <Objectively/Locale.h>
#include <Objectively/Once.h>
#include <Objectively/String.h>

#define _Class _Locale

#pragma mark - ObjectInterface

/**
 * @see ObjectInterface::copy(Object *)
 */
static Object *copy(const Object *self) {

	Locale *this = (Locale *) self;
	Locale *that = $(alloc(Locale), initWithIdentifier, this->identifier);

	return (Object *) that;
}

/**
 * @see ObjectInterface::dealloc(Object *)
 */
static void dealloc(Object *self) {

	Locale *this = (Locale *) self;

#if __MINGW32__
	_free_locale(this->locale);
#else
	freelocale(this->locale);
#endif

	super(Object, self, dealloc);
}

#pragma mark - LocaleInterface

/**
 * @see LocaleInterface::initWithIdentifier(Locale *, const char *)
 */
static Locale *initWithIdentifier(Locale *self, const char *identifier) {

	self = (Locale *) super(Object, self, init);
	if (self) {
		self->identifier = identifier;
		assert(self->identifier);

#if __MINGW32__
		self->locale = _create_locale(LC_ALL, self->name);
#else
		self->locale = newlocale(LC_ALL_MASK, self->identifier, NULL);
#endif
		assert(self->locale);
	}

	return self;
}

/**
 * @see LocaleInterface::initWithLocale(Locale *, LOCALE)
 */
static Locale *initWithLocale(Locale *self, LOCALE locale) {

	self = (Locale *) super(Object, self, init);
	if (self) {
		self->locale = locale;
		assert(self->locale);

		self->identifier = "";
	}

	return self;
}

/**
 * @see LocaleInterface::systemLocale(void)
 */
static Locale *systemLocale(void) {
#if __MINGW32__
	return $(alloc(Locale), initWithLocale, _get_current_locale(LC_ALL));
#else
	return $(alloc(Locale), initWithLocale, duplocale(LC_GLOBAL_LOCALE));
#endif
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	((ObjectInterface *) clazz->interface)->copy = copy;
	((ObjectInterface *) clazz->interface)->dealloc = dealloc;

	((LocaleInterface *) clazz->interface)->initWithIdentifier = initWithIdentifier;
	((LocaleInterface *) clazz->interface)->initWithLocale = initWithLocale;
	((LocaleInterface *) clazz->interface)->systemLocale = systemLocale;
}

Class _Locale = {
	.name = "Locale",
	.superclass = &_Object,
	.instanceSize = sizeof(Locale),
	.interfaceOffset = offsetof(Locale, interface),
	.interfaceSize = sizeof(LocaleInterface),
	.initialize = initialize,
};

#undef _Class
