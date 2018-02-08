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

#include <stdio.h>

#include "Hello.h"

#define _Class _Hello

#pragma mark - Hello

/**
 * @fn Hello::helloWithGreeting(const char *)
 */
static Hello *helloWithGreeting(const char *greeting) {
	return $(alloc(Hello), initWithGreeting, greeting);
}

/**
 * @fn Hello::initWithGreeting(Hello *, const char *)
 */
static Hello *initWithGreeting(Hello *self, const char *greeting) {

	self = (Hello *) super(Object, self, init);
	if (self) {
		self->greeting = greeting ? : "Hello World!";
	}
	return self;
}

/**
 * @fn Hello::sayHello(const Hello *)
 */
static void sayHello(const Hello *self) {
	printf("%s\n", self->greeting);
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	HelloInterface *hello = (HelloInterface *) clazz->interface;

	hello->helloWithGreeting = helloWithGreeting;
	hello->initWithGreeting = initWithGreeting;
	hello->sayHello = sayHello;
}

/**
 * @fn Class *Hello::_Hello(void)
 * @memberof Hello
 */
Class *_Hello(void) {
	static Class *clazz;
	static Once once;

	do_once(&once, {
		clazz = _initialize(&(const ClassDef) {
			.name = "Hello",
			.superclass = _Object(),
			.instanceSize = sizeof(Hello),
			.interfaceOffset = offsetof(Hello, interface),
			.interfaceSize = sizeof(HelloInterface),
			.initialize = initialize,
		});
	});

	return clazz;
}

#undef _Class

#pragma mark - main

int main(int argc, char **argv) {

	Hello *hello = $$(Hello, helloWithGreeting, NULL);

	$(hello, sayHello);

	release(hello);
}
