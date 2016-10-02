[![Build Status](http://ci.quetoo.org/buildStatus/icon?job=Objectively-Linux-x86_64)](http://ci.quetoo.org/job/Objectively-Linux-x86_64/)
[![Zlib License](https://img.shields.io/badge/license-Zlib-brightgreen.svg)](https://opensource.org/licenses/Zlib)
![This software is ALPHA](https://img.shields.io/badge/development_stage-ALPHA-yellow.svg)

Objectively
===
Ultra-lightweight object oriented framework for [GNU C](http://www.gnu.org/software/gnu-c-manual/).

[Foundation](https://developer.apple.com/reference/foundation)-inspired [core library](http://jaydolan.com/projects/Objectively).

Zlib [license](./COPYING).

Adding Objectively to your project
---

1) Do the Autotools dance.

```shell
./configure; make; sudo make install
```

2) Include the main header file in your source.

```c
#include <Objectively.h>
```

3) Compile and link with Objectively.

```shell
gcc `pkg-config --cflags --libs Objectively` -o myprogram *.c
```

Declaring a type
---

Types in Objectively are comprised of 3 components:

1) The instance `struct`, beginning with the parent type, the type interface and then any additional instance variables.

```c
/**
 * @brief The Hello type.
 */
struct Hello {

	/**
	 * @brief The parent.
	 */
	Object object;

	/**
	 * @brief The typed interface.
	 */
	HelloInterface *interface;

	/**
	 * @brief The greeting.
	 */
	const char *greeting;
};
```

2) The interface `struct`, beginning with the parent interface, followed by any Class or instance methods.

```c
/**
 * @brief The Hello interface.
 */
struct HelloInterface {

	/**
	 * @brief The parent.
	 */
	ObjectInterface objectInterface;

	/**
	 * @static
	 * @fn Hello *Hello::helloWithGreeting(const char *greeting)
	 * @brief A factory method for instantiating Hello.
	 * @param greeting The greeting.
	 * @return A new Hello with the given `greeting`.
	 * @memberof Hello
	 */
	Hello *(*helloWithGreeting)(const char *greeting);

	/**
	 * @fn Hello *Hello::initWithGreeting(Hello *self, const char *greeting)
	 * @brief Initializes this Hello with the given `greeting`.
	 * @param self The Hello.
	 * @param greeting The greeting.
	 * @return The initialized Hello, or `NULL` on error.
	 * @memberof Hello
	 */
	Hello *(*initWithGreeting)(Hello *self, const char *greeting);

	/**
	 * @fn void Hello::sayHello(const Hello *self)
	 * @brief Prints this Hello's greeting to the console.
	 * @param self The Hello.
	 * @memberof Hello
	 */
	void (*sayHello)(const Hello *self);
};
```

3) The class descriptor, serving to tie 1) and 2) together.

```c
/**
 * @brief The Hello Class.
 */
extern Class _Hello;
```

Implementing a type
---

To implement a type, implement its instance and Class methods and Class initializer:

```c
#include <stdio.h>
#include <Objectively.h>

#define _Class _Hello

/**
 * @fn Hello *HelloInterface::helloWithGreeting(const char *greeting)
 * @memberof Hello
 */
static Hello *helloWithGreeting(const char *greeting) {
	return $(alloc(Hello), initWithGreeting, greeting);
}

/**
 * @fn Hello *HelloInterface::initWithGreeting(Hello *self, const char *greeting)
 * @memberof Hello
 */
static Hello *initWithGreeting(Hello *self, const char *greeting) {

	self = (Hello *) super(Object, self, init);
	if (self) {
		self->greeting = greeting ? : "Hello World!";
	}
	return self;
}

/**
 * @fn void HelloInterface::sayHello(const Hello *self)
 * @memberof Hello
 */
static void sayHello(const Hello *self) {
	printf("%s\n", self->greeting);
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {
 
	((HelloInterface *) clazz->def->interface)->helloWithGreeting = helloWithGreeting;
	((HelloInterface *) clazz->def->interface)->initWithGreeting = initWithGreeting;
	((HelloInterface *) clazz->def->interface)->sayHello = sayHello;
}

Class _Hello = {
	.name = "Hello",
	.superclass = &_Object,
	.instanceSize = sizeof(Hello),
	.interfaceOffset = offsetof(Hello, interface),
	.interfaceSize = sizeof(HelloInterface),
	.initialize = initialize,
};
    
#undef _Class
```

Using a type
---
```c
Hello *hello = $$(Hello, helloWithGreeting, NULL);

$(hello, sayHello);

release(hello);
```
See [Hello.c](Examples/Hello.c) for the full source to this example.

Initialization
---
There is no explicit setup or teardown with Objectively. To instantiate a type, simply call `alloc` from anywhere in your program. The first time a type is instantiated, its Class initializer, `initialize`, is called. Use `initialize` to setup your interface, override methods, or initialize a library your class wraps. When your application terminates, an optional Class destructor, `destroy`, is also called.

Invoking an instance method
---
To invoke an instance method, use the `$` macro.

```c
    $(condition, waitUntilDate, date);
```

Invoking a Class method
---
To invoke a Class method, use the `$$` macro.

```c
    Dictionary *dict = $$(JSONSerialization, objectWithData, data);
```

Overriding a method
---
To override a method, overwrite the function pointer from within your Class' `initialize` method.

```c
    ((ObjectInterface *) clazz->def->interface)->dealloc = dealloc;
    ((ObjectInterface *) clazz->def->interface)->isEqual = isEqual;
```

Calling super
---
To invoke a supertype's method implementation, use the `super` macro.

```c
    super(Object, self, dealloc);
```

Managing memory
---
Objectively uses reference counting to govern object retention. Newly instantiated Objects have a reference count of 1. To retain a strong reference to an Object, call `retain(obj)`. To relinquish it, call `release(obj)`. Once an Object's reference count reaches 0, it is deallocated. Remember to balance every `retain` with a `release`.

Shared instances
---
A shared instance or _singleton pattern_ can be achieved through Class methods and _release-on-destroy_.

```c
static URLSession *_sharedInstance;

/**
 * @fn URLSession *URLSessionInterface::sharedInstance(void)
 * @memberof URLSession
 */
static *URLSession sharedInstance(void) {
    static Once once;

	do_once(&once, {
		_sharedInstance = $(alloc(URLSession), init);
	});

	return _sharedInstance;
}

/**
 * @see Class::destroy(Class *)
 */
static void destroy(Class *clazz) {
	release(_sharedInstance);
}

// ...

URLSession *session = $$(URLSession, sharedInstance);
```

See [Once.h](Sources/Objectively/Once.h) for details on `do_once`.
    
Code Templates
---
Objectively provides code templates for Xcode and Eclipse CDT that greatly cut down on the boilerplate required to declare and implement a type. These are _highly recommended_, as they will save you time and reduce errors in type declaration.

#### Xcode 7.x
Install the Xcode 7.x code templates by simply building the Xcode project.

![Objectively code templates in Xcode 7](Documentation/xcode-template-dialog.png)

#### Eclipse CDT
Install the Eclipse CDT templates through the Eclipse CDT preferences:

![Objectively code templates in Xcode 7](Documentation/eclipse-template-dialog.png)

API documentation
---
The API documentation can be [browsed online](http://jaydolan.com/projects/Objectively) or generated with [Doxygen](http://www.doxygen.org) by running `make html`.
