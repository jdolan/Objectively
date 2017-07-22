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

#include <Objectively/Enum.h>

int valueof(const EnumName *names, const char *chars) {

	int value = 0;

	const char *c = chars;
	while (*c) {
		const size_t size = strcspn(c, " |");
		if (size) {
			for (const EnumName *en = names; en->name; en++) {
				if (strncmp(en->name, c, size) == 0) {
					value |= en->value;
				}
			}
		}
		c += size + 1;
	}

	return value;
}
