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

#include <Objectively/Data.h>
#include <Objectively/Date.h>
#include <Objectively/Object.h>
#include <Objectively/URLResponse.h>

/**
 * @file
 * @brief Cached HTTP responses.
 */

typedef struct URLCachedResponse URLCachedResponse;
typedef struct URLCachedResponseInterface URLCachedResponseInterface;

/**
 * @brief A cached HTTP response.
 * @extends Object
 * @ingroup URLSession
 */
struct URLCachedResponse {

  /**
   * @brief The superclass.
   */
  Object object;

  /**
   * @brief The interface.
   * @protected
   */
  URLCachedResponseInterface *interface;

  /**
   * @brief The HTTP response.
   */
  URLResponse *response;

  /**
   * @brief The response body.
   */
  Data *data;

  /**
   * @brief The time this response was cached.
   */
  Date *timestamp;

  /**
   * @brief The cached body size.
   */
  size_t size;
};

/**
 * @brief The URLCachedResponse interface.
 */
struct URLCachedResponseInterface {

  /**
   * @brief The superclass interface.
   */
  ObjectInterface objectInterface;

  /**
   * @fn URLCachedResponse *URLCachedResponse::initWithResponseData(URLCachedResponse *self, const URLResponse *response, const Data *data)
   * @brief Initializes this URLCachedResponse with the given response and data.
   * @param self The URLCachedResponse.
   * @param response The HTTP response.
   * @param data The response body.
   * @return The initialized URLCachedResponse, or `NULL` on error.
   * @memberof URLCachedResponse
   */
  URLCachedResponse *(*initWithResponseData)(URLCachedResponse *self, const URLResponse *response,
      const Data *data);
};

/**
 * @fn Class *URLCachedResponse::_URLCachedResponse(void)
 * @brief The URLCachedResponse archetype.
 * @return The URLCachedResponse Class.
 * @memberof URLCachedResponse
 */
OBJECTIVELY_EXPORT Class *_URLCachedResponse(void);
