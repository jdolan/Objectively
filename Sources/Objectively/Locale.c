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
#include <Objectively/String.h>

#define _Class _Locale

#pragma mark - Object

/**
 * @see Object::copy(Object *)
 */
static Object *copy(const Object *self) {

	Locale *this = (Locale *) self;
	Locale *that = $(alloc(Locale), initWithIdentifier, this->identifier);

	return (Object *) that;
}

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

	Locale *this = (Locale *) self;

#if defined(_WIN32)
	_free_locale(this->locale);
#else
	freelocale(this->locale);
#endif

	super(Object, self, dealloc);
}

#pragma mark - Locale

/**
 * @fn Locale *Locale::initWithIdentifier(Locale *self, const char *identifier)
 * @memberof Locale
 */
static Locale *initWithIdentifier(Locale *self, const char *identifier) {

	self = (Locale *) super(Object, self, init);
	if (self) {
		self->identifier = identifier;
		assert(self->identifier);

#if defined(_WIN32)
		self->locale = _create_locale(LC_ALL, self->identifier);
#else
		self->locale = newlocale(LC_ALL_MASK, self->identifier, NULL);
#endif
		assert(self->locale);
	}

	return self;
}

/**
 * @fn Locale *Locale::initWithLocale(Locale *self, locale_t locale)
 * @memberof Locale
 */
static Locale *initWithLocale(Locale *self, locale_t locale) {

	self = (Locale *) super(Object, self, init);
	if (self) {
		self->locale = locale;
		assert(self->locale);

		self->identifier = "";
	}

	return self;
}

/**
 * @fn Locale *Locale::systemLocale(void)
 * @memberof Locale
 */
static Locale *systemLocale(void) {
#if defined(_WIN32)
	return $(alloc(Locale), initWithLocale, _get_current_locale());
#else
	return $(alloc(Locale), initWithLocale, duplocale(LC_GLOBAL_LOCALE));
#endif
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	((ObjectInterface *) clazz->def->interface)->copy = copy;
	((ObjectInterface *) clazz->def->interface)->dealloc = dealloc;

	((LocaleInterface *) clazz->def->interface)->initWithIdentifier = initWithIdentifier;
	((LocaleInterface *) clazz->def->interface)->initWithLocale = initWithLocale;
	((LocaleInterface *) clazz->def->interface)->systemLocale = systemLocale;
}

/**
 * @fn Class *Locale::_Locale(void)
 * @memberof Locale
 */
Class *_Locale(void) {
	static Class clazz;
	static Once once;
	
	do_once(&once, {
		clazz.name = "Locale";
		clazz.superclass = _Object();
		clazz.instanceSize = sizeof(Locale);
		clazz.interfaceOffset = offsetof(Locale, interface);
		clazz.interfaceSize = sizeof(LocaleInterface);
		clazz.initialize = initialize;
	});

	return &clazz;
}

#undef _Class
