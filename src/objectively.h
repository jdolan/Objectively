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

#ifndef _objectively_h_
#define _objectively_h_

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

/*
 * @brief Define the interface (methods and members) of your type.
 */
#define Interface(Type, SuperType) \
	typedef struct _##Type Type; \
	struct _##Type { \
		SuperType super; \
		Type *archetype;

/*
 * @brief Terminate an interface or implementation block.
 */
#define End };

/*
 * @brief Declare the constructor prototype for your type.
 */
#define Constructor(Type, ...) \
	extern Type __##Type; \
	extern Type *Type##_init(Type *self, ## __VA_ARGS__)

/*
 * @brief Implement the constructor for your type.
 */
#define Implementation(Type, ...) \
	Type __##Type; \
	Type *Type##_init(Type *self, ## __VA_ARGS__) { \
		self->archetype = &__##Type;

/*
 * @brief Initialize the archetype of your type from within your constructor.
 */
#define Initialize(Type, ...) \
	static _Bool initialized; \
	\
	if (initialized == false) { \
		initialized = true; \
		\
		memset(&__##Type, 0, sizeof(Type)); \
		Type##_init(&__##Type, ## __VA_ARGS__); \
	}

/*
 * @brief Override a supertype method in your type.
 */
#define Override(Type, instance, method, implementation) \
	((Type *) instance)->method = (typeof(__##Type.method)) implementation;

/*
 * @brief Invoke a supertype method on an instance of your type.
 */
#define Super(Type, instance, method, ...) \
	((Type *) (instance)->super.archetype)->method((Type *) instance, ## __VA_ARGS__);

/*
 * @brief Instantiate your type.
 */
#define New(Type, ...) \
	Type##_init(calloc(1, sizeof(Type)), ## __VA_ARGS__)

/*
 * @brief Destroy an instance of your type.
 */
#define Destroy(obj) \
	if (obj) { \
		((Object *) obj)->dealloc((Object *) obj); \
		obj = NULL; \
	}

/*
 * @brief The Object base type.
 */
Interface(Object, void *)

	void (*dealloc)(Object *self);

End

/*
 * @brief The Object constructor prototype.
 */
Constructor(Object);

#endif
