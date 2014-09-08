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

/**
 * @file
 *
 * @brief Objectively: Ultra-lightweight object oriented framework for c99.
 *
 * Types in Objectively are comprised of 3 components:
 *
 *  1. The instance struct, containing the parent type and any additional fields.
 *  2. The interface struct, containing the parent interface and any additional methods.
 *  3. The class descriptor, serving to tie 1. and 2. together.
 *
 * @code
 *
 * // foo.h
 *
 * typedef struct Foo Foo;
 * typedef struct FooInterface FooInterface;
 *
 * struct Foo {
 * 	   Object object;
 *
 *     int bar;
 *     // ...
 *
 *     const FooInterface *interface;
 * };
 *
 * struct FooInterface {
 * 	   ObjectInterface objectInterface;
 *
 * 	   // ...
 *
 * 	   void (*baz)(const Foo *self);
 * };
 *
 * extern Class __Foo;
 *
 * // foo.c
 *
 * #pragma mark - Object instance methods
 *
 * static Object *init(id obj, id interface, va_list *args) {
 *
 *     Foo *self = (Foo *) super(Object, obj, init, interface, args);
 *     if (self) {
 *         self->interface = (FooInterface *) interface;
 *
 *         self->bar =
 *         // ...
 *     }
 *     return self;
 * }
 *
 * #pragma mark - Foo instance methods
 *
 * static void baz(const Foo *self) {
 *     printf("%d\n", self->bar);
 * }
 *
 * #pragma mark - Foo class methods
 *
 * static void initialize(Class *class) {
 *
 *     ObjectInterface *object = (ObjectInterface *) class->interface;
 *
 *     object->init = init;
 *
 *     FooInterface *foo = (FooInterface *) class->interface;
 *
 *     foo->baz = baz;
 * }
 *
 * Class __Foo = {
 *     .name = "Foo",
 *     .superclass = &__Object,
 *     .instanceSize = sizeof(Foo),
 *     .interfaceSize = sizeof(FooInterface),
 *     .initialize = initialize,
 * };
 *
 * @endcode
 */

#include <objectively/array.h>
#include <objectively/class.h>
#include <objectively/object.h>
#include <objectively/string.h>
#include <objectively/types.h>

#endif
