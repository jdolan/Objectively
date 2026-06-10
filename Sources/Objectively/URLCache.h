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

#pragma once

#include <Objectively/CachedURLResponse.h>
#include <Objectively/Lock.h>
#include <Objectively/MutableDictionary.h>
#include <Objectively/URLRequest.h>

/**
 * @file
 * @brief A cache for HTTP responses.
 */

typedef struct URLCache URLCache;
typedef struct URLCacheInterface URLCacheInterface;

/**
 * @brief A cache for HTTP responses.
 * @extends Object
 * @ingroup URLSession
 */
struct URLCache {

  /**
   * @brief The superclass.
   */
  Object object;

  /**
   * @brief The interface.
   * @protected
   */
  URLCacheInterface *interface;

  /**
   * @brief The lock protecting this cache.
   */
  Lock *lock;

  /**
   * @brief The cached responses, keyed by URLRequest.
   */
  MutableDictionary *responses;

  /**
   * @brief The current cached body size.
   */
  size_t currentSize;

  /**
   * @brief The maximum cached body size.
   */
  size_t maxSize;
};

/**
 * @brief The URLCache interface.
 */
struct URLCacheInterface {

  /**
   * @brief The superclass interface.
   */
  ObjectInterface objectInterface;

  /**
   * @fn CachedURLResponse *URLCache::cachedResponseForRequest(URLCache *self, const URLRequest *request)
   * @brief Returns the cached response for the given request.
   * @param self The URLCache.
   * @param request The URLRequest.
   * @return The cached response, retained, or `NULL` if none is cached.
   * @memberof URLCache
   */
  CachedURLResponse *(*cachedResponseForRequest)(URLCache *self, const URLRequest *request);

  /**
   * @fn URLCache *URLCache::init(URLCache *self)
   * @brief Initializes this URLCache.
   * @param self The URLCache.
   * @return The initialized URLCache, or `NULL` on error.
   * @memberof URLCache
   */
  URLCache *(*init)(URLCache *self);

  /**
   * @fn void URLCache::removeAllCachedResponses(URLCache *self)
   * @brief Removes all cached responses.
   * @param self The URLCache.
   * @memberof URLCache
   */
  void (*removeAllCachedResponses)(URLCache *self);

  /**
   * @fn void URLCache::removeCachedResponseForRequest(URLCache *self, const URLRequest *request)
   * @brief Removes the cached response for the given request.
   * @param self The URLCache.
   * @param request The URLRequest.
   * @memberof URLCache
   */
  void (*removeCachedResponseForRequest)(URLCache *self, const URLRequest *request);

  /**
   * @fn void URLCache::setMaxSize(URLCache *self, size_t maxSize)
   * @brief Sets the maximum cached body size.
   * @param self The URLCache.
   * @param maxSize The maximum cache size.
   * @memberof URLCache
   */
  void (*setMaxSize)(URLCache *self, size_t maxSize);

  /**
   * @fn void URLCache::storeCachedResponseForRequest(URLCache *self, const URLRequest *request, const URLResponse *response, const Data *data)
   * @brief Stores a cached response for the given request.
   * @param self The URLCache.
   * @param request The URLRequest.
   * @param response The HTTP response.
   * @param data The response body.
   * @memberof URLCache
   */
  void (*storeCachedResponseForRequest)(URLCache *self, const URLRequest *request,
      const URLResponse *response, const Data *data);
};

/**
 * @fn Class *URLCache::_URLCache(void)
 * @brief The URLCache archetype.
 * @return The URLCache Class.
 * @memberof URLCache
 */
OBJECTIVELY_EXPORT Class *_URLCache(void);
