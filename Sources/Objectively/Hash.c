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

#include <string.h>

#include <Objectively/Hash.h>

int HashForBytes(int hash, const byte *bytes, const RANGE range) {

	for (size_t i = range.location; i < range.length; i++) {

		int shift;
		if (i & 1) {
			shift = 16 + (i % 16);
		} else {
			shift = (i % 16);
		}

		hash += 31 * ((int) bytes[i]) << shift;
	}

	return hash;
}

int HashForCharacters(int hash, const char *chars, const RANGE range) {

	return HashForBytes(hash, (const byte *) chars, range);
}

int HashForCString(int hash, const char *string) {

	const RANGE range = { 0, strlen(string) };
	return HashForCharacters(hash, string, range);
}

int HashForDecimal(int hash, const double decimal) {

	return hash + 31 * (int) decimal;
}

int HashForInteger(int hash, const long integer) {

	return hash + 31 * (int) integer;
}

int HashForObject(int hash, const ident obj) {

	if (obj) {
		return hash + 31 * $(cast(Object, obj), hash);
	}

	return 0;
}
