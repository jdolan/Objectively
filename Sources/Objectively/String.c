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

#include "Config.h"

#include <assert.h>
#include <iconv.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>

#include "Hash.h"
#include "String.h"

#if defined(__MINGW32__)
#define towlower_l _towlower_l
#define towupper_l _towupper_l
#endif

#define _Class _String

#pragma mark - Object

/**
 * @see Object::copy(const Object *)
 */
static Object *copy(const Object *self) {

  const String *this = (const String *) self;
  String *that = $(alloc(String), initWithString, this);

  return (Object *) that;
}

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

  String *this = (String *) self;

  free(this->chars);

  super(Object, self, dealloc);
}

/**
 * @see Object::description(const Object *)
 */
static String *description(const Object *self) {

  return (String *) $((Object *) self, copy);
}

/**
 * @see Object::hash(const Object *)
 */
static int hash(const Object *self) {

  String *this = (String *) self;

  return HashForCString(HASH_SEED, this->chars);
}

/**
 * @see Object::isEqual(const Object *, const Object *)
 */
static bool isEqual(const Object *self, const Object *other) {

  if (super(Object, self, isEqual, other)) {
    return true;
  }

  if (other && $(other, isKindOfClass, _String())) {

    const String *this = (String *) self;
    const String *that = (String *) other;

    if (this->length == that->length) {

      const Range range = { 0, this->length };
      return $(this, compareTo, that, range) == OrderSame;
    }
  }

  return false;
}

#pragma mark - String

/**
 * @brief Character transcoding context for `iconv`.
 * @see iconv(3)
 */
typedef struct {
  StringEncoding to;
  StringEncoding from;
  char *in;
  size_t length;
  char *out;
  size_t size;
} Transcode;

/**
 * @brief Transcodes input from one character encoding to another via `iconv`.
 * @param trans A Transcode struct.
 * @return The number of bytes written to `trans->out`.
 */
static size_t transcode(Transcode *trans) {

  assert(trans);
  assert(trans->to);
  assert(trans->from);
  assert(trans->out);
  assert(trans->size);

  iconv_t cd = iconv_open(NameForStringEncoding(trans->to), NameForStringEncoding(trans->from));
  assert(cd != (iconv_t) -1);

  char *in = trans->in;
  char *out = trans->out;

  size_t inBytesRemaining = trans->length;
  size_t outBytesRemaining = trans->size;

  const size_t ret = iconv(cd, &in, &inBytesRemaining, &out, &outBytesRemaining);
  assert(ret != (size_t) -1);

  int err = iconv_close(cd);
  assert(err == 0);

  return trans->size - outBytesRemaining;
}

/**
 * @fn Order String::compareTo(const String *self, const String *other, const Range range)
 * @memberof String
 */
static Order compareTo(const String *self, const String *other, const Range range) {

  assert(range.location + range.length <= self->length);

  if (other) {
    const int i = strncmp(self->chars + range.location, other->chars, range.length);
    if (i == 0) {
      return OrderSame;
    }
    if (i > 0) {
      return OrderDescending;
    }
  }

  return OrderAscending;
}

/**
 * @fn Array *String::componentsSeparatedByCharacters(const String *self, const char *chars)
 * @memberof String
 */
static Array *componentsSeparatedByCharacters(const String *self, const char *chars) {

  assert(chars);

  Array *components = $(alloc(Array), init);

  Range search = { 0, self->length };
  Range result = $(self, rangeOfCharacters, chars, search);

  while (result.length) {
    search.length = result.location - search.location;

    String *component = $(self, substring, search);
    $(components, addObject, component);
    release(component);

    search.location = result.location + result.length;
    search.length = self->length - search.location;

    result = $(self, rangeOfCharacters, chars, search);
  }

  String *component = $(self, substring, search);
  $(components, addObject, component);
  release(component);

  return components;
}

/**
 * @fn Array *String::componentsSeparatedByString(const String *self, const String *string)
 * @memberof String
 */
static Array *componentsSeparatedByString(const String *self, const String *string) {

  assert(string);

  return $(self, componentsSeparatedByCharacters, string->chars);
}

/**
 * @fn Data *String::getData(const String *self, StringEncoding encoding)
 * @memberof String
 */
static Data *getData(const String *self, StringEncoding encoding) {

  Transcode trans = {
    .to = encoding,
    .from = STRING_ENCODING_UTF8,
    .in = self->chars,
    .length = self->length,
    .out = calloc(self->length, sizeof(Unicode) / sizeof(char)),
    .size = self->length * sizeof(Unicode)
  };

  assert(trans.out);

  const size_t size = transcode(&trans);
  assert(size <= trans.size);

  return $$(Data, dataWithMemory, trans.out, size);
}

/**
 * @fn bool String::hasPrefix(const String *self, const String *prefix)
 * @memberof String
 */
static bool hasPrefix(const String *self, const String *prefix) {

  if (prefix->length > self->length) {
    return false;
  }

  Range range = { 0, prefix->length };
  return $(self, compareTo, prefix, range) == OrderSame;
}

/**
 * @fn bool String::hasSuffix(const String *self, const String *suffix)
 * @memberof String
 */
static bool hasSuffix(const String *self, const String *suffix) {

  if (suffix->length > self->length) {
    return false;
  }

  Range range = { self->length - suffix->length, suffix->length };
  return $(self, compareTo, suffix, range) == OrderSame;
}

/**
 * @fn String *String::initWithBytes(String *self, const uint8_t *bytes, size_t length, StringEncoding encoding)
 * @memberof String
 */
/**
 * @fn String *String::lowercaseString(const String *self)
 * @memberof String
 */
static String *lowercaseString(const String *self) {

  Data *data = $(self, getData, STRING_ENCODING_WCHAR);
  assert(data);

  const size_t codepoints = data->length / sizeof(Unicode);
  Unicode *unicode = (Unicode *) data->bytes;

  for (size_t i = 0; i < codepoints; i++, unicode++) {
    *unicode = towlower(*unicode);
  }

  String *lowercase = $$(String, stringWithData, data, STRING_ENCODING_WCHAR);

  release(data);
  return lowercase;
}

/**
 * @fn Range String::rangeOfCharacters(const String *self, const char *chars, const Range range)
 * @memberof String
 */
static Range rangeOfCharacters(const String *self, const char *chars, const Range range) {

  assert(chars);
  assert(range.location > -1);
  assert(range.length > 0);
  assert(range.location + range.length <= self->length);

  Range match = { -1, 0 };
  const size_t len = strlen(chars);

  const char *str = self->chars + range.location;
  for (size_t i = 0; i < range.length; i++, str++) {
    if (strncmp(str, chars, len) == 0) {
      match.location = range.location + i;
      match.length = len;
      break;
    }
  }

  return match;
}

/**
 * @fn Range String::rangeOfString(const String *self, const String *string, const Range range)
 * @memberof String
 */
static Range rangeOfString(const String *self, const String *string, const Range range) {

  assert(string);

  return $(self, rangeOfCharacters, string->chars, range);
}

/**
 * @fn String *String::stringWithBytes(const uint8_t *bytes, size_t length, StringEncoding encoding)
 * @memberof String
 */
static String *stringWithBytes(const uint8_t *bytes, size_t length, StringEncoding encoding) {

  return $(alloc(String), initWithBytes, bytes, length, encoding);
}

/**
 * @fn String *String::stringWithCharacters(const char *chars)
 * @memberof String
 */
static String *stringWithCharacters(const char *chars) {

  return $(alloc(String), initWithCharacters, chars);
}

/**
 * @fn String *String::stringWithContentsOfFile(const char *path, StringEncoding encoding)
 * @memberof String
 */
static String *stringWithContentsOfFile(const char *path, StringEncoding encoding) {

  return $(alloc(String), initWithContentsOfFile, path, encoding);
}

/**
 * @fn String *String::stringWithData(const Data *data, StringEncoding encoding)
 * @memberof String
 */
static String *stringWithData(const Data *data, StringEncoding encoding) {

  return $(alloc(String), initWithData, data, encoding);
}

/**
 * @fn String *String::stringWithFormat(const char *fmt)
 * @memberof String
 */
static String *stringWithFormat(const char *fmt, ...) {

  va_list args;
  va_start(args, fmt);

  String *string = $(alloc(String), initWithVaList, fmt, args);

  va_end(args);

  return string;
}

/**
 * @fn String *String::stringWithMemory(const ident mem, size_t length)
 * @memberof String
 */
static String *stringWithMemory(const ident mem, size_t length) {

  return $(alloc(String), initWithMemory, mem, length);
}

/**
 * @fn String *String::substring(const String *string, const Range range)
 * @memberof String
 */
static String *substring(const String *self, const Range range) {

  assert(range.location + range.length <= self->length);

  ident mem = calloc(range.length + 1, sizeof(char));
  assert(mem);

  strncpy(mem, self->chars + range.location, range.length);

  return $(alloc(String), initWithMemory, mem, range.length);
}

/**
 * @fn String *String::trimmedString(const String *self)
 * @memberof String
 */
static String *trimmedString(const String *self) {

  Range range = { .location = 0, .length = self->length };

  while (isspace(self->chars[range.location])) {
    range.location++;
    range.length--;
  }

  while (range.length > 0 && isspace(self->chars[range.location + range.length - 1])) {
    range.length--;
  }

  return $(self, substring, range);
}

/**
 * @fn String *String::uppercaseString(const String *self)
 * @memberof String
 */
static String *uppercaseString(const String *self) {

  Data *data = $(self, getData, STRING_ENCODING_WCHAR);
  assert(data);

  const size_t codepoints = data->length / sizeof(Unicode);
  Unicode *unicode = (Unicode *) data->bytes;

  for (size_t i = 0; i < codepoints; i++, unicode++) {
    *unicode = towupper(*unicode);
  }

  String *uppercase = $$(String, stringWithData, data, STRING_ENCODING_WCHAR);

  release(data);
  return uppercase;
}

/**
 * @fn bool String::writeToFile(const String *self, const char *path, StringEncoding encoding)
 * @memberof String
 */
static bool writeToFile(const String *self, const char *path, StringEncoding encoding) {

  Data *data = $(self, getData, encoding);
  assert(data);

  const bool success = $(data, writeToFile, path);

  release(data);
  return success;
}

#pragma mark - String mutation

/**
 * @fn void String::appendBytes(String *self, const uint8_t *bytes, size_t length, StringEncoding encoding)
 * @memberof String
 */
static void appendBytes(String *self, const uint8_t *bytes, size_t length, StringEncoding encoding) {

  if (bytes) {

    Transcode trans = {
      .to = STRING_ENCODING_UTF8,
      .from = encoding,
      .in = (char *) bytes,
      .length = length,
      .out = calloc(length * sizeof(Unicode) + 1, sizeof(char)),
      .size = length * sizeof(Unicode) + 1
    };

    assert(trans.out);

    const size_t size = transcode(&trans);
    assert(size < trans.size);

    trans.out[size] = '\0';

    $(self, appendCharacters, trans.out);

    free(trans.out);
  }
}

/**
 * @fn void String::appendCharacters(String *self, const char *chars)
 * @memberof String
 */
static void appendCharacters(String *self, const char *chars) {

  if (chars) {

    const size_t len = strlen(chars);
    if (len) {

      const size_t newSize = self->length + strlen(chars) + 1;
      const size_t newCapacity = (newSize / _pageSize + 1) * _pageSize;

      if (newCapacity > self->capacity) {

        if (self->length) {
          self->chars = realloc(self->chars, newCapacity);
        } else {
          self->chars = malloc(newCapacity);
        }

        assert(self->chars);
        self->capacity = newCapacity;
      }

      ident ptr = self->chars + self->length;
      memmove(ptr, chars, len);

      self->chars[newSize - 1] = '\0';
      self->length += len;
    }
  }
}

/**
 * @fn void String::appendFormat(String *self, const char *fmt, ...)
 * @memberof String
 */
static void appendFormat(String *self, const char *fmt, ...) {

  va_list args;
  va_start(args, fmt);

  $(self, appendVaList, fmt, args);

  va_end(args);
}

/**
 * @fn void String::appendString(String *self, const String *string)
 * @memberof String
 */
static void appendString(String *self, const String *string) {

  if (string) {
    $(self, appendCharacters, string->chars);
  }
}

/**
 * @fn void String::appendVaList(String *self, const char *fmt, va_list args)
 * @memberof String
 */
static void appendVaList(String *self, const char *fmt, va_list args) {
  char *chars;

  const int len = vasprintf(&chars, fmt, args);
  if (len > 0) {
    $(self, appendCharacters, chars);
  }

  free(chars);
}

/**
 * @fn void String::deleteCharactersInRange(String *self, const Range range)
 * @memberof String
 */
static void deleteCharactersInRange(String *self, const Range range) {

  assert(range.location >= 0);
  assert(range.length <= self->length);

  ident ptr = self->chars + range.location;
  const size_t length = self->length - range.location - range.length + 1;

  memmove(ptr, ptr + range.length, length);

  self->length -= range.length;
}

/**
 * @fn String *String::init(String *self)
 * @memberof String
 */
static String *init(String *self) {
  return $(self, initWithCapacity, 0);
}

/**
 * @fn String *String::initWithBytes(String *self, const uint8_t *bytes, size_t length, StringEncoding encoding)
 * @memberof String
 */
static String *initWithBytes(String *self, const uint8_t *bytes, size_t length, StringEncoding encoding) {

  self = $(self, init);
  if (self) {
    $(self, appendBytes, bytes, length, encoding);
  }

  return self;
}

/**
 * @fn String *String::initWithCapacity(String *self, size_t capacity)
 * @memberof String
 */
static String *initWithCapacity(String *self, size_t capacity) {

  self = $(self, initWithMemory, NULL, 0);
  if (self) {
    self->capacity = capacity;
    if (self->capacity) {
      self->chars = calloc(self->capacity, sizeof(char));
      assert(self->chars);
    }
  }

  return self;
}

/**
 * @fn String *String::initWithCharacters(String *self, const char *chars)
 * @memberof String
 */
static String *initWithCharacters(String *self, const char *chars) {

  self = $(self, init);
  if (self) {
    $(self, appendCharacters, chars);
  }

  return self;
}

/**
 * @fn String *String::initWithContentsOfFile(String *self, const char *path, StringEncoding encoding)
 * @memberof String
 */
static String *initWithContentsOfFile(String *self, const char *path, StringEncoding encoding) {

  Data *data = $$(Data, dataWithContentsOfFile, path);
  if (data) {
    self = $(self, initWithData, data, encoding);
  } else {
    self = $(self, init);
  }

  release(data);
  return self;
}

/**
 * @fn String *String::initWithData(String *self, const Data *data, StringEncoding encoding)
 * @memberof String
 */
static String *initWithData(String *self, const Data *data, StringEncoding encoding) {

  assert(data);

  return $(self, initWithBytes, data->bytes, data->length, encoding);
}

/**
 * @fn String *String::initWithFormat(String *self, const char *fmt, ...)
 * @memberof String
 */
static String *initWithFormat(String *self, const char *fmt, ...) {

  self = $(self, init);
  if (self) {

    va_list args;
    va_start(args, fmt);

    $(self, appendVaList, fmt, args);

    va_end(args);
  }

  return self;
}

/**
 * @fn String *String::initWithMemory(String *self, const ident mem, size_t length)
 * @memberof String
 */
static String *initWithMemory(String *self, const ident mem, size_t length) {

  self = (String *) super(Object, self, init);
  if (self) {
    self->chars = (char *) mem;
    self->length = length;
    self->capacity = self->chars ? length + 1 : 0;
  }

  return self;
}

/**
 * @fn String *String::initWithString(String *self, const String *string)
 * @memberof String
 */
static String *initWithString(String *self, const String *string) {

  self = $(self, init);
  if (self) {
    $(self, appendString, string);
  }

  return self;
}

/**
 * @fn String *String::initWithVaList(String *self, const char *fmt, va_list args)
 * @memberof String
 */
static String *initWithVaList(String *self, const char *fmt, va_list args) {

  self = $(self, init);
  if (self) {
    $(self, appendVaList, fmt, args);
  }

  return self;
}

/**
 * @fn void String::insertCharactersAtIndex(String *self, const char *chars, size_t index)
 * @memberof String
 */
static void insertCharactersAtIndex(String *self, const char *chars, size_t index) {

  const Range range = { .location = index };

  $(self, replaceCharactersInRange, range, chars);
}

/**
 * @fn void String::insertStringAtIndex(String *self, const String *string, size_t index)
 * @memberof String
 */
static void insertStringAtIndex(String *self, const String *string, size_t index) {

  $(self, insertCharactersAtIndex, string->chars, index);
}

/**
 * @fn void String::replaceCharactersInRange(String *self, const Range range, const char *chars)
 * @memberof String
 */
static void replaceCharactersInRange(String *self, const Range range, const char *chars) {

  assert(range.location >= 0);
  assert(range.location + range.length <= self->length);

  if (self->capacity == 0) {
    $(self, appendCharacters, chars);
  } else {
    char *remainder = strdup(self->chars + range.location + range.length);

    self->length = range.location;
    self->chars[range.location + 1] = '\0';

    $(self, appendCharacters, chars);
    $(self, appendCharacters, remainder);

    free(remainder);
  }
}

/**
 * @fn void String::replaceOccurrencesOfCharacters(String *self, const char *chars, const char *replacement)
 * @memberof String
 */
static void replaceOccurrencesOfCharacters(String *self, const char *chars, const char *replacement) {
  $(self, replaceOccurrencesOfCharactersInRange, chars, (Range) { .length = self->length }, replacement);
}

/**
 * @fn void String::replaceOccurrencesOfCharactersInRange(String *self, const char *chars, const Range range, const char *replacement)
 * @memberof String
 */
static void replaceOccurrencesOfCharactersInRange(String *self, const char *chars, const Range range, const char *replacement) {

  assert(chars);
  assert(replacement);

  assert(range.location >= 0);
  assert(range.location + range.length <= self->length);

  Range search = range;
  while (true) {

    const Range result = $((String *) self, rangeOfCharacters, chars, search);
    if (result.location == -1) {
      break;
    }

    $(self, replaceCharactersInRange, result, replacement);

    search.length -= (result.location - search.location);
    search.length -= strlen(replacement);
    search.length += ((int) strlen(replacement) - (int) strlen(chars));

    search.location = result.location + strlen(replacement);
  }
}

/**
 * @fn void String::replaceOccurrencesOfString(String *self, const String *string, const String *replacement)
 * @memberof String
 */
static void replaceOccurrencesOfString(String *self, const String *string, const String *replacement) {
  $(self, replaceOccurrencesOfStringInRange, string, (Range) { .length = self->length }, replacement);
}

/**
 * @fn void String::replaceOccurrencesOfStringInRange(String *self, const String *string, const Range range, const String *replacement)
 * @memberof String
 */
static void replaceOccurrencesOfStringInRange(String *self, const String *string, const Range range, const String *replacement) {

  assert(string);
  assert(replacement);

  $(self, replaceOccurrencesOfCharactersInRange, string->chars, range, replacement->chars);
}

/**
 * @fn void String::replaceStringInRange(String *self, const Range range, const String *string)
 * @memberof String
 */
static void replaceStringInRange(String *self, const Range range, const String *string) {

  $(self, replaceCharactersInRange, range, string->chars);
}

/**
 * @fn void String::setCharacters(String *self, const char *chars)
 * @memberof String
 */
static void setCharacters(String *self, const char *chars) {

  $(self, setLength, 0);

  $(self, appendCharacters, chars);
}

/**
 * @fn void String::setFormat(String *self, const char *fmt, ...)
 * @memberof String
 */
static void setFormat(String *self, const char *fmt, ...) {

  $(self, setLength, 0);

  va_list args;
  va_start(args, fmt);

  $(self, appendVaList, fmt, args);

  va_end(args);
}

/**
 * @fn void String::setLength(String *self, size_t length)
 * @memberof String
 */
static void setLength(String *self, size_t length) {

  if (length < self->length) {
    self->length = length;
    self->chars[length] = '\0';
  }
}

/**
 * @fn void String::setString(String *self, const String *string)
 * @memberof String
 */
static void setString(String *self, const String *string) {

  $(self, setLength, 0);

  $(self, appendString, string);
}

/**
 * @fn String *String::string(void)
 * @memberof String
 */
static String *string(void) {
  return $(alloc(String), init);
}

/**
 * @fn String *String::stringWithCapacity(size_t capacity)
 * @memberof String
 */
static String *stringWithCapacity(size_t capacity) {
  return $(alloc(String), initWithCapacity, capacity);
}

/**
 * @fn void String::trim(String *self)
 * @memberof String
 */
static void trim(String *self) {

  String *trimmed = $((String *) self, trimmedString);

  $(self, replaceStringInRange, (const Range) { .length = self->length }, trimmed);

  release(trimmed);
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

  ((StringInterface *) clazz->interface)->compareTo = compareTo;
  ((StringInterface *) clazz->interface)->componentsSeparatedByCharacters = componentsSeparatedByCharacters;
  ((StringInterface *) clazz->interface)->componentsSeparatedByString = componentsSeparatedByString;
  ((StringInterface *) clazz->interface)->getData = getData;
  ((StringInterface *) clazz->interface)->hasPrefix = hasPrefix;
  ((StringInterface *) clazz->interface)->hasSuffix = hasSuffix;
  ((StringInterface *) clazz->interface)->initWithBytes = initWithBytes;
  ((StringInterface *) clazz->interface)->initWithCharacters = initWithCharacters;
  ((StringInterface *) clazz->interface)->initWithContentsOfFile = initWithContentsOfFile;
  ((StringInterface *) clazz->interface)->initWithData = initWithData;
  ((StringInterface *) clazz->interface)->initWithFormat = initWithFormat;
  ((StringInterface *) clazz->interface)->initWithMemory = initWithMemory;
  ((StringInterface *) clazz->interface)->initWithVaList = initWithVaList;
  ((StringInterface *) clazz->interface)->lowercaseString = lowercaseString;
  ((StringInterface *) clazz->interface)->appendBytes = appendBytes;
  ((StringInterface *) clazz->interface)->appendCharacters = appendCharacters;
  ((StringInterface *) clazz->interface)->appendFormat = appendFormat;
  ((StringInterface *) clazz->interface)->appendString = appendString;
  ((StringInterface *) clazz->interface)->appendVaList = appendVaList;
  ((StringInterface *) clazz->interface)->deleteCharactersInRange = deleteCharactersInRange;
  ((StringInterface *) clazz->interface)->init = init;
  ((StringInterface *) clazz->interface)->initWithCapacity = initWithCapacity;
  ((StringInterface *) clazz->interface)->initWithString = initWithString;
  ((StringInterface *) clazz->interface)->insertCharactersAtIndex = insertCharactersAtIndex;
  ((StringInterface *) clazz->interface)->insertStringAtIndex = insertStringAtIndex;
  ((StringInterface *) clazz->interface)->replaceCharactersInRange = replaceCharactersInRange;
  ((StringInterface *) clazz->interface)->replaceOccurrencesOfCharacters = replaceOccurrencesOfCharacters;
  ((StringInterface *) clazz->interface)->replaceOccurrencesOfCharactersInRange = replaceOccurrencesOfCharactersInRange;
  ((StringInterface *) clazz->interface)->replaceOccurrencesOfString = replaceOccurrencesOfString;
  ((StringInterface *) clazz->interface)->replaceOccurrencesOfStringInRange = replaceOccurrencesOfStringInRange;
  ((StringInterface *) clazz->interface)->replaceStringInRange = replaceStringInRange;
  ((StringInterface *) clazz->interface)->setCharacters = setCharacters;
  ((StringInterface *) clazz->interface)->setFormat = setFormat;
  ((StringInterface *) clazz->interface)->setLength = setLength;
  ((StringInterface *) clazz->interface)->setString = setString;
  ((StringInterface *) clazz->interface)->string = string;
  ((StringInterface *) clazz->interface)->stringWithCapacity = stringWithCapacity;
  ((StringInterface *) clazz->interface)->trim = trim;
  ((StringInterface *) clazz->interface)->rangeOfCharacters = rangeOfCharacters;
  ((StringInterface *) clazz->interface)->rangeOfString = rangeOfString;
  ((StringInterface *) clazz->interface)->stringWithBytes = stringWithBytes;
  ((StringInterface *) clazz->interface)->stringWithCharacters = stringWithCharacters;
  ((StringInterface *) clazz->interface)->stringWithContentsOfFile = stringWithContentsOfFile;
  ((StringInterface *) clazz->interface)->stringWithData = stringWithData;
  ((StringInterface *) clazz->interface)->stringWithFormat = stringWithFormat;
  ((StringInterface *) clazz->interface)->stringWithMemory = stringWithMemory;
  ((StringInterface *) clazz->interface)->substring = substring;
  ((StringInterface *) clazz->interface)->trimmedString = trimmedString;
  ((StringInterface *) clazz->interface)->uppercaseString = uppercaseString;
  ((StringInterface *) clazz->interface)->writeToFile = writeToFile;
}

/**
 * @fn Class *String::_String(void)
 * @memberof String
 */
Class *_String(void) {
  static Class *clazz;
  static Once once;

  do_once(&once, {
    clazz = _initialize(&(const ClassDef) {
      .name = "String",
      .superclass = _Object(),
      .instanceSize = sizeof(String),
      .interfaceOffset = offsetof(String, interface),
      .interfaceSize = sizeof(StringInterface),
      .initialize = initialize,
    });
  });

  return clazz;
}

#undef _Class

const char *NameForStringEncoding(StringEncoding encoding) {

  switch (encoding) {
    case STRING_ENCODING_ASCII:
      return "ASCII";
    case STRING_ENCODING_LATIN1:
      return "ISO-8859-1";
    case STRING_ENCODING_LATIN2:
      return "ISO-8859-2";
    case STRING_ENCODING_MACROMAN:
      return "MacRoman";
    case STRING_ENCODING_UTF16:
      return "UTF-16";
    case STRING_ENCODING_UTF32:
      return "UTF-32";
    case STRING_ENCODING_UTF8:
      return "UTF-8";
    case STRING_ENCODING_WCHAR:
      return "WCHAR_T";
  }

  return "ASCII";
}

StringEncoding StringEncodingForName(const char *name) {

  if (strcasecmp("ASCII", name) == 0) {
    return STRING_ENCODING_ASCII;
  } else if (strcasecmp("ISO-8859-1", name) == 0) {
    return STRING_ENCODING_LATIN1;
  } else if (strcasecmp("ISO-8859-2", name) == 0) {
    return STRING_ENCODING_LATIN2;
  } else if (strcasecmp("MacRoman", name) == 0) {
    return STRING_ENCODING_MACROMAN;
  } else if (strcasecmp("UTF-16", name) == 0) {
    return STRING_ENCODING_UTF16;
  } else if (strcasecmp("UTF-32", name) == 0) {
    return STRING_ENCODING_UTF32;
  } else if (strcasecmp("UTF-8", name) == 0) {
    return STRING_ENCODING_UTF8;
  } else if (strcasecmp("WCHAR", name) == 0) {
    return STRING_ENCODING_WCHAR;
  }

  return STRING_ENCODING_ASCII;
}

Order StringCompare(const ident a, const ident b) {

  if (a) {
    if (b) {
      const int i = strcmp(((String *) a)->chars, ((String *) b)->chars);
      if (i == 0) {
        return OrderSame;
      }
      if (i > 0) {
        return OrderDescending;
      }
    } else {
      return OrderDescending;
    }
  }
  return OrderAscending;
}

String *str(const char *fmt, ...) {

  va_list args;
  va_start(args, fmt);

  String *string = $(alloc(String), initWithVaList, fmt, args);
  assert(string);

  va_end(args);

  return string;
}

char *strtrim(const char *s) {

  assert(s);
  while (isspace(*s)) {
    s++;
  }

  char *trimmed = strdup(s);
  assert(trimmed);

  char *end = trimmed + strlen(trimmed);
  if (end > trimmed) {
    while (isspace(*(--end))) {
      *end = '\0';
    }
  }

  return trimmed;
}
