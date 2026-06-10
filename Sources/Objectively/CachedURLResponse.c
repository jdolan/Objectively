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

#include "CachedURLResponse.h"
#include "Hash.h"

#define _Class _CachedURLResponse

#pragma mark - Object

/**
 * @brief Tests equality of two Objects, accounting for NULL.
 */
static bool objectEquals(const Object *self, const Object *other) {

  if (self) {
    return other && $(self, isEqual, other);
  }

  return other == NULL;
}

/**
 * @see Object::copy(const Object *)
 */
static Object *copy(const Object *self) {

  const CachedURLResponse *this = (CachedURLResponse *) self;

  CachedURLResponse *that = $(alloc(CachedURLResponse), initWithResponseData, this->response, this->data);
  if (that) {
    release(that->timestamp);
    that->timestamp = (Date *) $((Object *) this->timestamp, copy);
    that->size = this->size;
  }

  return (Object *) that;
}

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

  CachedURLResponse *this = (CachedURLResponse *) self;

  release(this->response);
  release(this->data);
  release(this->timestamp);

  super(Object, self, dealloc);
}

/**
 * @see Object::hash(const Object *)
 */
static int hash(const Object *self) {

  const CachedURLResponse *this = (CachedURLResponse *) self;

  int hash = HASH_SEED;
  hash = HashForInteger(hash, this->size);
  hash = HashForInteger(hash, this->response ? this->response->httpStatusCode : 0);
  hash = HashForObject(hash, this->response ? this->response->httpHeaders : NULL);
  hash = HashForObject(hash, this->data);
  hash = HashForObject(hash, this->timestamp);

  return hash;
}

/**
 * @see Object::isEqual(const Object *, const Object *)
 */
static bool isEqual(const Object *self, const Object *other) {

  if (super(Object, self, isEqual, other)) {
    return true;
  }

  if (other && $(other, isKindOfClass, _CachedURLResponse())) {

    const CachedURLResponse *this = (CachedURLResponse *) self;
    const CachedURLResponse *that = (CachedURLResponse *) other;

    return this->size == that->size
      && this->response->httpStatusCode == that->response->httpStatusCode
      && objectEquals((Object *) this->response->httpHeaders, (Object *) that->response->httpHeaders)
      && objectEquals((Object *) this->data, (Object *) that->data)
      && objectEquals((Object *) this->timestamp, (Object *) that->timestamp);
  }

  return false;
}

#pragma mark - CachedURLResponse

/**
 * @fn CachedURLResponse *CachedURLResponse::initWithResponseData(CachedURLResponse *self, const URLResponse *response, const Data *data)
 * @memberof CachedURLResponse
 */
static CachedURLResponse *initWithResponseData(CachedURLResponse *self, const URLResponse *response,
    const Data *data) {

  assert(response);

  self = (CachedURLResponse *) super(Object, self, init);
  if (self) {
    self->response = (URLResponse *) $((Object *) response, copy);

    if (data) {
      self->data = $(alloc(Data), initWithBytes, data->bytes, data->length);
    } else {
      self->data = $(alloc(Data), initWithBytes, (const uint8_t *) "", 0);
    }

    self->timestamp = $$(Date, date);
    self->size = self->data ? self->data->length : 0;
  }

  return self;
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

  ((ObjectInterface *) clazz->interface)->copy = copy;
  ((ObjectInterface *) clazz->interface)->dealloc = dealloc;
  ((ObjectInterface *) clazz->interface)->hash = hash;
  ((ObjectInterface *) clazz->interface)->isEqual = isEqual;

  ((CachedURLResponseInterface *) clazz->interface)->initWithResponseData = initWithResponseData;
}

/**
 * @fn Class *CachedURLResponse::_CachedURLResponse(void)
 * @memberof CachedURLResponse
 */
Class *_CachedURLResponse(void) {
  static Class *clazz;
  static Once once;

  do_once(&once, {
    clazz = _initialize(&(const ClassDef) {
      .name = "CachedURLResponse",
      .superclass = _Object(),
      .instanceSize = sizeof(CachedURLResponse),
      .interfaceOffset = offsetof(CachedURLResponse, interface),
      .interfaceSize = sizeof(CachedURLResponseInterface),
      .initialize = initialize,
    });
  });

  return clazz;
}

#undef _Class
