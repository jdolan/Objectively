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
#include "Pointer.h"

#define _Class _Pointer

#pragma mark - Object

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

  Pointer *this = (Pointer *) self;

  if (this->destroy) {
    this->destroy(this->pointer);
  }

  super(Object, self, dealloc);
}

/**
 * @see Object::hash(const Object *)
 */
static int hash(const Object *self) {

  const Pointer *this = (Pointer *) self;

  uintptr_t addr = (uintptr_t) this->pointer;

  return (int) ((13 * addr) ^ (addr >> 15));
}

/**
 * @see Object::isEqual(const Object *, const Object *)
 */
static bool isEqual(const Object *self, const Object *other) {

  if (super(Object, self, isEqual, other)) {
    return true;
  }

  if (other && $(other, isKindOfClass, _Pointer())) {

    const Pointer *this = (Pointer *) self;
    const Pointer *that = (Pointer *) other;

    return this->pointer == that->pointer;
  }

  return false;
}

#pragma mark - Pointer

/**
 * @fn Pointer *Pointer::initWithBytes(Pointer *self, const uint8_t *bytes, size_t length)
 * @memberof Pointer
 */
static Pointer *initWithBytes(Pointer *self, const uint8_t *bytes, size_t length) {

  self = (Pointer *) super(Object, self, init);
  if (self) {
    if (bytes) {
      self->pointer = calloc(1, length);
      assert(self->pointer);

      memcpy(self->pointer, bytes, length);
      self->destroy = free;
    }
  }

  return self;
}

/**
 * @fn Pointer *Pointer::initWithPointer(Pointer *self, ident pointer, Consumer destroy)
 * @memberof Pointer
 */
static Pointer *initWithPointer(Pointer *self, ident pointer, Consumer destroy) {

  self = (Pointer *) super(Object, self, init);
  if (self) {
    self->pointer = pointer;
    self->destroy = destroy;
  }

  return self;
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

  ((ObjectInterface *) clazz->interface)->dealloc = dealloc;
  ((ObjectInterface *) clazz->interface)->hash = hash;
  ((ObjectInterface *) clazz->interface)->isEqual = isEqual;

  ((PointerInterface *) clazz->interface)->initWithBytes = initWithBytes;
  ((PointerInterface *) clazz->interface)->initWithPointer = initWithPointer;
}

/**
 * @fn Class *Pointer::_Pointer(void)
 * @memberof Pointer
 */
Class *_Pointer(void) {
  static Class *clazz;
  static Once once;

  do_once(&once, {
    clazz = _initialize(&(const ClassDef) {
      .name = "Pointer",
      .superclass = _Object(),
      .instanceSize = sizeof(Pointer),
      .interfaceOffset = offsetof(Pointer, interface),
      .interfaceSize = sizeof(PointerInterface),
      .initialize = initialize,
    });
  });

  return clazz;
}

#undef _Class

Pointer *ptr(ident pointer, Consumer destroy) {
  return $(alloc(Pointer), initWithPointer, pointer, destroy);
}
