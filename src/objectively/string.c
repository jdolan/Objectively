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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <objectively/macros.h>
#include <objectively/string.h>

static void dealloc(id self) {

	struct String *this = cast(String, self);

	free(this->str);

	super(Object, self, dealloc);
}

static id append(id self, const char *fmt, ...) {

	va_list args;
	va_start(args, fmt);

	char *str;
	vasprintf(&str, fmt, args);

	va_end(args);

	if (str) {

		struct String *this = cast(String, self);
		if (this->str) {

			const size_t len = strlen(this->str) + strlen(str);
			this->str = realloc(this->str, len);

			strlcat(this->str, str, len);
			free(str);
		} else {
			this->str = str;
		}
	}

	return self;
}

static id init(id self, va_list *args) {

	self = super(Object, self, init, args);
	if (self) {

		override(Object, self, init, init);
		override(Object, self, dealloc, dealloc);

		struct String *this = cast(String, self);

		const char *fmt = arg(args, const char *, NULL);
		if (fmt) {
			vasprintf(&this->str, fmt, *args);
		}

		this->append = append;
	}

	return self;
}

static struct String string;

static struct Class class = {
	.name = "String",
	.size = sizeof(struct String),
	.superclass = &Object,
	.archetype = &string,
	.init = init, };

const Class *String = &class;
