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
#include <string.h>

#include "Hash.h"
#include "Regexp.h"

#define _Class _Regexp

#pragma mark - Object

/**
 * @see Object::copy(const Object *)
 */
static Object *copy(const Object *self) {

  const Regexp *this = (Regexp *) self;

  return (Object *) re(this->pattern, this->options);
}

/**
 * @see Object::description(const Object *)
 */
static String *description(const Object *self) {

  const Regexp *this = (Regexp *) self;

  return $$(String, stringWithCharacters, this->pattern);
}

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

  Regexp *this = (Regexp *) self;

  regfree(this->regex);
  free(this->regex);

  super(Object, self, dealloc);
}

/**
 * @see Object::hash(const Object *)
 */
static int hash(const Object *self) {

  Regexp *this = (Regexp *) self;

  int hash = HASH_SEED;
  hash = HashForInteger(hash, this->options);

  const Range range = { 0, strlen(this->pattern) };
  hash = HashForBytes(hash, (uint8_t *) this->pattern, range);

  return hash;
}

/**
 * @see Object::isEqual(const Object *, const Object *)
 */
static bool isEqual(const Object *self, const Object *other) {

  if (super(Object, self, isEqual, other)) {
    return true;
  }

  if (other && $(other, isKindOfClass, _Regexp())) {

    const Regexp *this = (Regexp *) self;
    const Regexp *that = (Regexp *) other;

    if (this->options == that->options) {
      return strcmp(this->pattern, that->pattern) == 0;
    }
  }

  return false;
}

#pragma mark - Regexp

/**
 * @fn Regexp *Regexp::initWithPattern(Regexp *self, const char *pattern, int options)
 * @memberof Regexp
 */
static Regexp *initWithPattern(Regexp *self, const char *pattern, int options) {

  self = (Regexp *) super(Object, self, init);
  if (self) {
    self->regex = calloc(1, sizeof(regex_t));
    assert(self->regex);

    const int err = regcomp(self->regex, pattern, REG_EXTENDED | options);
    assert(err == 0);

    self->pattern = pattern;
    self->options = options;
    self->numberOfSubExpressions = ((regex_t *) self->regex)->re_nsub;
  }

  return self;
}

/**
 * @fn bool Regexp::matchesCharacters(const Regexp *self, const char *chars, int options, Range **matches)
 * @memberof Regexp
 */
static bool matchesCharacters(const Regexp *self, const char *chars, int options, Range **ranges) {

  if (ranges) {
    const size_t numberOfMatches = self->numberOfSubExpressions + 1;
    regmatch_t matches[numberOfMatches];

    const int err = regexec(self->regex, chars, numberOfMatches, matches, options);
    assert(err == 0 || err == REG_NOMATCH);

    *ranges = calloc(numberOfMatches, sizeof(Range));
    assert(*ranges);

    Range *range = *ranges;
    const regmatch_t *match = matches;
    for (size_t i = 0; i < numberOfMatches; i++, range++, match++) {
      range->location = match->rm_so;
      if (range->location > -1) {
        range->length = match->rm_eo - match->rm_so;
      } else {
        range->length = 0;
      }
    }

    return err == 0;
  }

  const int err = regexec(self->regex, chars, 0, NULL, options);
  assert(err == 0 || err == REG_NOMATCH);

  return err == 0;
}

/**
 * @fn bool Regexp::matchesString(const Regexp *self, const String *string, int options, Range **matches)
 * @memberof Regexp
 */
static bool matchesString(const Regexp *self, const String *string, int options, Range **ranges) {

  assert(string);

  return $(self, matchesCharacters, string->chars, options, ranges);
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

  ((ObjectInterface *) clazz->interface)->copy = copy;
  ((ObjectInterface *) clazz->interface)->dealloc = dealloc;
  ((ObjectInterface *) clazz->interface)->description = description;
  ((ObjectInterface *) clazz->interface)->hash = hash;
  ((ObjectInterface *) clazz->interface)->isEqual = isEqual;

  ((RegexpInterface *) clazz->interface)->initWithPattern = initWithPattern;
  ((RegexpInterface *) clazz->interface)->matchesCharacters = matchesCharacters;
  ((RegexpInterface *) clazz->interface)->matchesString = matchesString;
}

/**
 * @fn Class *Regexp::_Regexp(void)
 * @memberof Regexp
 */
Class *_Regexp(void) {
  static Class *clazz;
  static Once once;

  do_once(&once, {
    clazz = _initialize(&(const ClassDef) {
      .name = "Regexp",
      .superclass = _Object(),
      .instanceSize = sizeof(Regexp),
      .interfaceOffset = offsetof(Regexp, interface),
      .interfaceSize = sizeof(RegexpInterface),
      .initialize = initialize,
    });
  });

  return clazz;
}

#undef _Class

Regexp *re(const char *pattern, int options) {
  return $(alloc(Regexp), initWithPattern, pattern, options);
}
