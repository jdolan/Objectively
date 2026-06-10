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
#include <ctype.h>
#include <stdlib.h>

#include "String.h"
#include "URLCache.h"

#define _Class _URLCache

#define URLCACHE_DEFAULT_MAX_SIZE (1024 * 1024)

#pragma mark - Helpers

/**
 * @brief Tests two C strings for case-insensitive equality.
 */
static bool stringEqualsIgnoreCase(const char *a, const char *b) {

  if (a == NULL || b == NULL) {
    return false;
  }

  while (*a && *b) {
    if (tolower((unsigned char) *a) != tolower((unsigned char) *b)) {
      return false;
    }

    a++;
    b++;
  }

  return *a == 0 && *b == 0;
}

/**
 * @brief Tests the first `length` characters of two C strings for case-insensitive equality.
 */
static bool stringEqualsIgnoreCaseN(const char *a, const char *b, size_t length) {

  if (a == NULL || b == NULL) {
    return false;
  }

  for (size_t i = 0; i < length; i++) {
    if (tolower((unsigned char) a[i]) != tolower((unsigned char) b[i])) {
      return false;
    }
  }

  return true;
}

/**
 * @brief Returns the value for the specified HTTP header field, case-insensitively.
 */
static const String *headerValueForField(const Dictionary *headers, const char *field) {

  if (headers && field) {

    Array *keys = $(headers, allKeys);
    for (size_t i = 0; i < keys->count; i++) {

      const String *key = $(keys, objectAtIndex, i);
      if (stringEqualsIgnoreCase(key->chars, field)) {

        const String *value = $(headers, objectForKey, (ident) key);
        release(keys);
        return value;
      }
    }

    release(keys);
  }

  return NULL;
}

/**
 * @brief Returns true if the provided Cache-Control value disables caching.
 */
static bool cacheControlDisablesCaching(const char *value) {

  if (value == NULL) {
    return false;
  }

  const char *cursor = value;
  while (*cursor) {

    while (*cursor == ' ' || *cursor == '\t' || *cursor == ',') {
      cursor++;
    }

    const char *end = cursor;
    while (*end && *end != ',') {
      end++;
    }

    const char *start = cursor;
    while (start < end && isspace((unsigned char) *start)) {
      start++;
    }

    while (end > start && isspace((unsigned char) *(end - 1))) {
      end--;
    }

    const size_t length = (size_t) (end - start);
    if (length > 0) {

      if (length == 8 && stringEqualsIgnoreCaseN(start, "no-cache", 8)) {
        return true;
      }

      if (length == 8 && stringEqualsIgnoreCaseN(start, "no-store", 8)) {
        return true;
      }

      if (length >= 8 && stringEqualsIgnoreCaseN(start, "max-age=", 8)) {
        if (strtol(start + 8, NULL, 10) <= 0) {
          return true;
        }
      }
    }

    cursor = end;
    if (*cursor == ',') {
      cursor++;
    }
  }

  return false;
}

/**
 * @brief Returns true if this request is cacheable.
 */
static bool requestIsCacheable(const URLRequest *request) {

  if (request == NULL) {
    return false;
  }

  switch (request->httpMethod) {
    case HTTP_NONE:
    case HTTP_GET:
      break;
    default:
      return false;
  }

  const String *cacheControl = headerValueForField(request->httpHeaders, "Cache-Control");
  if (cacheControl && cacheControlDisablesCaching(cacheControl->chars)) {
    return false;
  }

  const String *pragma = headerValueForField(request->httpHeaders, "Pragma");
  if (pragma && stringEqualsIgnoreCase(pragma->chars, "no-cache")) {
    return false;
  }

  return true;
}

/**
 * @brief Returns true if this response is cacheable.
 */
static bool responseIsCacheable(const URLResponse *response) {

  if (response == NULL || response->httpStatusCode != 200) {
    return false;
  }

  const String *cacheControl = headerValueForField(response->httpHeaders, "Cache-Control");
  if (cacheControl && cacheControlDisablesCaching(cacheControl->chars)) {
    return false;
  }

  const String *pragma = headerValueForField(response->httpHeaders, "Pragma");
  if (pragma && stringEqualsIgnoreCase(pragma->chars, "no-cache")) {
    return false;
  }

  return true;
}

/**
 * @brief Removes the oldest cached response while this cache exceeds its maximum size.
 */
static void evictIfNeeded(URLCache *self) {

  const Dictionary *dictionary = (Dictionary *) self->responses;

  while (self->currentSize > self->maxSize && self->responses->dictionary.count > 0) {

    Array *keys = $(dictionary, allKeys);

    const URLRequest *oldestKey = NULL;
    CachedURLResponse *oldestResponse = NULL;

    for (size_t i = 0; i < keys->count; i++) {

      const URLRequest *key = $(keys, objectAtIndex, i);
      CachedURLResponse *candidate = $(dictionary, objectForKey, (ident) key);

      if (candidate && (oldestResponse == NULL
          || $(candidate->timestamp, compareTo, oldestResponse->timestamp) == OrderAscending)) {
        oldestKey = key;
        oldestResponse = candidate;
      }
    }

    release(keys);

    if (oldestKey == NULL || oldestResponse == NULL) {
      break;
    }

    self->currentSize -= oldestResponse->size;
    $(self->responses, removeObjectForKey, (ident) oldestKey);
  }
}

#pragma mark - Object

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

  URLCache *this = (URLCache *) self;

  release(this->lock);
  release(this->responses);

  super(Object, self, dealloc);
}

#pragma mark - URLCache

/**
 * @fn CachedURLResponse *URLCache::cachedResponseForRequest(URLCache *self, const URLRequest *request)
 * @memberof URLCache
 */
static CachedURLResponse *cachedResponseForRequest(URLCache *self, const URLRequest *request) {

  if (requestIsCacheable(request) == false) {
    return NULL;
  }

  CachedURLResponse *cachedResponse = NULL;
  const Dictionary *dictionary = (Dictionary *) self->responses;
  synchronized(self->lock, {
    cachedResponse = $(dictionary, objectForKey, (ident) request);
    if (cachedResponse) {
      retain(cachedResponse);
    }
  });

  return cachedResponse;
}

/**
 * @fn URLCache *URLCache::init(URLCache *self)
 * @memberof URLCache
 */
static URLCache *init(URLCache *self) {

  self = (URLCache *) super(Object, self, init);
  if (self) {
    self->lock = $(alloc(Lock), init);
    assert(self->lock);

    self->responses = $(alloc(MutableDictionary), init);
    assert(self->responses);

    self->maxSize = URLCACHE_DEFAULT_MAX_SIZE;
  }

  return self;
}

/**
 * @fn void URLCache::removeAllCachedResponses(URLCache *self)
 * @memberof URLCache
 */
static void removeAllCachedResponses(URLCache *self) {

  synchronized(self->lock, {
    $(self->responses, removeAllObjects);
    self->currentSize = 0;
  });
}

/**
 * @fn void URLCache::removeCachedResponseForRequest(URLCache *self, const URLRequest *request)
 * @memberof URLCache
 */
static void removeCachedResponseForRequest(URLCache *self, const URLRequest *request) {

  synchronized(self->lock, {
    const Dictionary *dictionary = (Dictionary *) self->responses;
    CachedURLResponse *cachedResponse = $(dictionary, objectForKey, (ident) request);
    if (cachedResponse) {
      self->currentSize -= cachedResponse->size;
      $(self->responses, removeObjectForKey, (ident) request);
    }
  });
}

/**
 * @fn void URLCache::setMaxSize(URLCache *self, size_t maxSize)
 * @memberof URLCache
 */
static void setMaxSize(URLCache *self, size_t maxSize) {

  synchronized(self->lock, {
    self->maxSize = maxSize;

    if (self->maxSize == 0) {
      $(self->responses, removeAllObjects);
      self->currentSize = 0;
    } else {
      evictIfNeeded(self);
    }
  });
}

/**
 * @fn void URLCache::storeCachedResponseForRequest(URLCache *self, const URLRequest *request, const URLResponse *response, const Data *data)
 * @memberof URLCache
 */
static void storeCachedResponseForRequest(URLCache *self, const URLRequest *request,
    const URLResponse *response, const Data *data) {

  if (requestIsCacheable(request) == false || responseIsCacheable(response) == false) {
    return;
  }

  CachedURLResponse *cachedResponse = $(alloc(CachedURLResponse), initWithResponseData, response, data);
  if (cachedResponse == NULL) {
    return;
  }

  synchronized(self->lock, {
    if (cachedResponse->size <= self->maxSize && self->maxSize > 0) {

      const Dictionary *dictionary = (Dictionary *) self->responses;
      CachedURLResponse *existing = $(dictionary, objectForKey, (ident) request);
      if (existing) {
        self->currentSize -= existing->size;
        $(self->responses, removeObjectForKey, (ident) request);
      }

      URLRequest *key = (URLRequest *) $((Object *) request, copy);
      $(self->responses, setObjectForKey, cachedResponse, key);
      release(key);

      self->currentSize += cachedResponse->size;
      evictIfNeeded(self);
    }
  });

  release(cachedResponse);
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

  ((ObjectInterface *) clazz->interface)->dealloc = dealloc;

  ((URLCacheInterface *) clazz->interface)->cachedResponseForRequest = cachedResponseForRequest;
  ((URLCacheInterface *) clazz->interface)->init = init;
  ((URLCacheInterface *) clazz->interface)->removeAllCachedResponses = removeAllCachedResponses;
  ((URLCacheInterface *) clazz->interface)->removeCachedResponseForRequest = removeCachedResponseForRequest;
  ((URLCacheInterface *) clazz->interface)->setMaxSize = setMaxSize;
  ((URLCacheInterface *) clazz->interface)->storeCachedResponseForRequest = storeCachedResponseForRequest;
}

/**
 * @fn Class *URLCache::_URLCache(void)
 * @memberof URLCache
 */
Class *_URLCache(void) {
  static Class *clazz;
  static Once once;

  do_once(&once, {
    clazz = _initialize(&(const ClassDef) {
      .name = "URLCache",
      .superclass = _Object(),
      .instanceSize = sizeof(URLCache),
      .interfaceOffset = offsetof(URLCache, interface),
      .interfaceSize = sizeof(URLCacheInterface),
      .initialize = initialize,
    });
  });

  return clazz;
}

#undef _Class
