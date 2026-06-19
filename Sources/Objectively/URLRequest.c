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

#include "Hash.h"
#include "Dictionary.h"
#include "URLRequest.h"

#define _Class _URLRequest

#pragma mark - Object

/**
 * @see Object::copy(const Object *)
 */
static Object *copy(const Object *self) {

  URLRequest *this = (URLRequest *) self;

  URLRequest *that = $(alloc(URLRequest), initWithURL, this->url);

  if (this->httpBody) {
    that->httpBody = (Data *) $((Object *) this->httpBody, copy);
  }

  if (this->httpHeaders) {
    that->httpHeaders = (Dictionary *) $((Object *) this->httpHeaders, copy);
  }

  that->httpMethod = this->httpMethod;

  return (Object *) that;
}

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

  URLRequest *this = (URLRequest *) self;

  release(this->httpBody);
  release(this->httpHeaders);
  release(this->url);

  super(Object, self, dealloc);
}

/**
 * @see Object::hash(const Object *)
 */
static int hash(const Object *self) {

  const URLRequest *this = (URLRequest *) self;

  int hash = HASH_SEED;
  hash = HashForObject(hash, this->url);
  hash = HashForInteger(hash, this->httpMethod);
  hash = HashForObject(hash, this->httpBody);

  if (this->httpHeaders) {

    const Dictionary *headers = (Dictionary *) this->httpHeaders;
    Array *keys = $(headers, allKeys);

    int headersHash = 0;
    for (size_t i = 0; i < keys->count; i++) {

      const Object *key = $(keys, objectAtIndex, i);
      const Object *value = $(headers, objectForKey, (ident) key);

      int pairHash = HASH_SEED;
      pairHash = HashForObject(pairHash, (ident) key);
      pairHash = HashForObject(pairHash, (ident) value);

      headersHash += pairHash;
    }

    release(keys);

    hash = HashForInteger(hash, headersHash);
    hash = HashForInteger(hash, this->httpHeaders->count);
  }

  return hash;
}

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
 * @see Object::isEqual(const Object *, const Object *)
 */
static bool isEqual(const Object *self, const Object *other) {

  if (super(Object, self, isEqual, other)) {
    return true;
  }

  if (other && $(other, isKindOfClass, _URLRequest())) {

    const URLRequest *this = (URLRequest *) self;
    const URLRequest *that = (URLRequest *) other;

    return this->httpMethod == that->httpMethod
      && objectEquals((Object *) this->url, (Object *) that->url)
      && objectEquals((Object *) this->httpBody, (Object *) that->httpBody)
      && objectEquals((Object *) this->httpHeaders, (Object *) that->httpHeaders);
  }

  return false;
}

#pragma mark - URLRequest

/**
 * @fn URLRequest *URLRequest::initWithURL(URLRequest *self, URL *url)
 * @memberof URLRequest
 */
static URLRequest *initWithURL(URLRequest *self, URL *url) {

  assert(url);

  self = (URLRequest *) super(Object, self, init);
  if (self) {
    self->url = retain(url);
    assert(self->url);

    self->httpMethod = HTTP_GET;
  }

  return self;
}

/**
 * @fn void setValueForHTTPHeaderField(URLREquest *self, const char *value, const char *field)
 * @memberof URLRequest
 */
static void setValueForHTTPHeaderField(URLRequest *self, const char *value, const char *field) {

  if (self->httpHeaders == NULL) {
    self->httpHeaders = (Dictionary *) $(alloc(Dictionary), init);
  }

  String *object = str(value);
  String *key = str(field);

  $((Dictionary *) self->httpHeaders, setObjectForKey, object, key);

  release(object);
  release(key);
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

  ((ObjectInterface *) clazz->interface)->copy = copy;
  ((ObjectInterface *) clazz->interface)->hash = hash;
  ((ObjectInterface *) clazz->interface)->dealloc = dealloc;
  ((ObjectInterface *) clazz->interface)->isEqual = isEqual;

  ((URLRequestInterface *) clazz->interface)->initWithURL = initWithURL;
  ((URLRequestInterface *) clazz->interface)->setValueForHTTPHeaderField = setValueForHTTPHeaderField;
}

/**
 * @fn Class *URLRequest::_URLRequest(void)
 * @memberof URLRequest
 */
Class *_URLRequest(void) {
  static Class *clazz;
  static Once once;

  do_once(&once, {
    clazz = _initialize(&(const ClassDef) {
      .name = "URLRequest",
      .superclass = _Object(),
      .instanceSize = sizeof(URLRequest),
      .interfaceOffset = offsetof(URLRequest, interface),
      .interfaceSize = sizeof(URLRequestInterface),
      .initialize = initialize,
    });
  });

  return clazz;
}

#undef _Class
