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
#include <stdio.h>
#include <string.h>

#include "Enum.h"
#include "MutableString.h"

String *nameof(const EnumName *names, int value) {

	for (const EnumName *name = names; name->name; name++) {
		if (name->value == value) {
			return $$(String, stringWithCharacters, name->name);
		}
	}

	MutableString *string = NULL;

	for (const EnumName *name = names; name->name; name++) {
		if (name->value) {
			if ((name->value & value) == name->value) {
				if (string == NULL) {
					string = $$(MutableString, string);
				} else {
					$(string, appendCharacters, " | ");
				}
				$(string, appendCharacters, name->name);
			}
		}
	}

	return (String *) string;
}

int valueof(const EnumName *names, const char *chars) {

	int value = 0;

	const char *c = chars;
	while (*c) {
		const size_t size = strcspn(c, " |");
		if (size) {
			const EnumName *en;
			for (en = names; en->name; en++) {
				if (strncmp(en->name, c, size) == 0) {
					value |= en->value;
					break;
				}
				if (en->alias && strncmp(en->alias, c, size) == 0) {
					value |= en->value;
					break;
				}
			}
			if (en->name == NULL) {
				fprintf(stderr, "%s: Unknown character sequence: %s\n", __func__, c);
			}
		}
		c += size;
		c += strspn(c, " |");
	}

	return value;
}
