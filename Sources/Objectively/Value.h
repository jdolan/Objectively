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

#include <Objectively/Object.h>

/**
 * @file
 * @brief Values provide Object encapsulation for C types.
 */

typedef struct Value Value;
typedef struct ValueInterface ValueInterface;

/**
 * @brief Values may optionally reference a destructor to be called on `dealloc`.
 */
typedef void (*ValueDestructor)(ident value);

/**
 * @brief Values provide Object encapsulation for C types.
 * @extends Object
 */
struct Value {
	
	/**
	 * @brief The parent.
	 */
	Object object;
	
	/**
	 * @brief The typed interface.
	 * @protected
	 */
	ValueInterface *interface;

	/**
	 * @brief The backing value.
	 */
	ident value;

	/**
	 * @brief An optional destructor that, if iset, is called on `dealloc`.
	 */
	ValueDestructor destroy;
};

/**
 * @brief The Value interface.
 */
struct ValueInterface {
	
	/**
	 * @brief The parent interface.
	 */
	ObjectInterface objectInterface;
	
	/**
	 * @fn Value *Value::initWithValue(Value *self, ident value)
	 * @brief Initializes this Value.
	 * @param self The Value.
	 * @param value The backing value.
	 * @return The initialized Value, or `NULL` on error.
	 * @memberof Value
	 */
	Value *(*initWithValue)(Value *self, ident value);
};

/**
 * @brief The Value Class.
 */
extern Class _Value;

