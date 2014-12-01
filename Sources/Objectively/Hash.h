/*
 * Objectively: Ultra-lightweight object oriented framework for c99.
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

#ifndef _Objectively_Hash_h_
#define _Objectively_Hash_h_

#include <Objectively/Object.h>

/**
 * @file
 *
 * @brief Utilities for calculating hash values.
 */

/**
 * @brief The hash seed value.
 */
#define HASH_SEED 13

/**
 * @brief Accumulates the hash value of `chars` into `hash`.
 *
 * @param hash The hash accumulator.
 * @param chars The characters to hash.
 * @param range The RANGE to hash.
 *
 * @return The accumulated hash value.
 */
extern int HashForCharacters(int hash, const char *chars, const RANGE range);

/**
 * @brief Accumulates the hash value of `decimal` into `hash`.
 *
 * @param hash The hash accumulator.
 * @param decimal The decimal to hash.
 *
 * @return The accumulated hash value.
 */
extern int HashForDecimal(int hash, const double decimal);

/**
 * @brief Accumulates the hash value of `integer` into `hash`.
 *
 * @param hash The hash accumulator.
 * @param integer The integer to hash.
 *
 * @return The accumulated hash value.
 */
extern int HashForInteger(int hash, const long integer);

/**
 * @brief Accumulates the hash value of `object` into `hash`.
 *
 * @param hash The hash accumulator.
 * @param obj The Object to hash.
 *
 * @return The accumulated hash value.
 */
extern int HashForObject(int hash, const id obj);

#endif
