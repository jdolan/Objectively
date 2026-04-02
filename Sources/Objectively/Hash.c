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

#include "Hash.h"

int HashForBytes(int hash, const uint8_t *bytes, const Range range) {

  unsigned int uhash = (unsigned int) hash;

  for (size_t i = range.location; i < range.location + range.length; i++) {

    int shift;
    if (i & 1) {
      shift = 16 + (i % 16);
    } else {
      shift = (i % 16);
    }

    uhash += 31U * ((unsigned int) bytes[i]) << shift;
  }

  return (int) uhash;
}

int HashForCharacters(int hash, const char *chars, const Range range) {
  return HashForBytes(hash, (const uint8_t *) chars, range);
}

int HashForCString(int hash, const char *string) {

  if (string) {
    return HashForCharacters(hash, string, (Range) { 0, strlen(string) });
  }

  return 0;
}

int HashForDecimal(int hash, const double decimal) {
  unsigned int uhash = (unsigned int) hash;
  uhash += 31U * (unsigned int) decimal;
  return (int) uhash;
}

int HashForInteger(int hash, const long integer) {
  unsigned int uhash = (unsigned int) hash;
  uhash += 31U * (unsigned int) integer;
  return (int) uhash;
}

int HashForObject(int hash, const ident obj) {

  if (obj) {
    unsigned int uhash = (unsigned int) hash;
    uhash += 31U * (unsigned int) $(cast(Object, obj), hash);
    return (int) uhash;
  }

  return 0;
}
