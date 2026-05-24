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

#include "Boole.h"
#include "String.h"

#define _Class boole

#pragma mark - Object

/**
 * @see Object::copy(const Object *)
 */
static Object *copy(const Object *self) {

  return (Object *) self;
}

/**
 * @see Object::description(const Object *)
 */
static String *description(const Object *self) {

  const Boole *this = (Boole *) self;

  return $(alloc(String), initWithCharacters, this->value ? "true" : "false");
}

#pragma mark - Boole

static Boole *_False;

/**
 * @fn Boole *Boole::False(void)
 * @memberof Boole
 */
static Boole *False(void) {

  static Once once;

  do_once(&once, {
    _False = (Boole *) $((Object *) alloc(Boole), init);
    _False->value = false;
  });

  return _False;
}

static Boole *_True;

/**
 * @fn Boole *Boole::True(void)
 * @memberof Boole
 */
static Boole *True(void) {

  static Once once;

  do_once(&once, {
    _True = (Boole *) $((Object *) alloc(Boole), init);
    _True->value = true;
  });

  return _True;
}

/**
 * @fn Boole *Boole::valueof(bool value)
 * @memberof Boole
 */
static Boole *valueof(bool value) {
  return value ? $$(Boole, True) : $$(Boole, False);
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

  ((ObjectInterface *) clazz->interface)->copy = copy;
  ((ObjectInterface *) clazz->interface)->description = description;

  ((BooleInterface *) clazz->interface)->False = False;
  ((BooleInterface *) clazz->interface)->True = True;
  ((BooleInterface *) clazz->interface)->valueof = valueof;
}

/**
 * @see Class::destroy(Class *)
 */
static void destroy(Class *clazz) {

  release(_False);
  release(_True);
}

/**
 * @fn Class *Boole::boole(void)
 * @memberof Boole
 */
Class *_Boole(void) {
  static Class *clazz;
  static Once once;

  do_once(&once, {
    clazz = _initialize(&(const ClassDef) {
      .name = "Boole",
      .superclass = _Object(),
      .instanceSize = sizeof(Boole),
      .interfaceOffset = offsetof(Boole, interface),
      .interfaceSize = sizeof(BooleInterface),
      .initialize = initialize,
      .destroy = destroy,
    });
  });

  return clazz;
}

#undef _Class
