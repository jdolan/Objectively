Getting Started {#getting-started}
===============

A guide to declaring, implementing, and using your own types with Objectively.

[TOC]

## Declaring a Type

Every Objectively type consists of three components:

**1. The instance struct** — starts with the parent type, then the interface pointer, then instance variables:

```c
struct Hello {
    Object object;           // parent (starts-with = single inheritance)
    HelloInterface *interface;
    const char *greeting;
};
```

**2. The interface struct** — starts with the parent interface, then method function pointers:

```c
struct HelloInterface {
    ObjectInterface objectInterface;

    Hello *(*helloWithGreeting)(const char *greeting);
    Hello *(*initWithGreeting)(Hello *self, const char *greeting);
    void   (*sayHello)(const Hello *self);
};
```

**3. The Class archetype** — a single exported function that returns the Class:

```c
OBJECTIVELY_EXPORT Class *_Hello(void);
```

## Implementing a Type

```c
#include <stdio.h>
#include <Objectively.h>

#define _Class _Hello

static Hello *helloWithGreeting(const char *greeting) {
    return $(alloc(Hello), initWithGreeting, greeting);
}

static Hello *initWithGreeting(Hello *self, const char *greeting) {
    self = (Hello *) super(Object, self, init);
    if (self) {
        self->greeting = greeting ?: "Hello World!";
    }
    return self;
}

static void sayHello(const Hello *self) {
    printf("%s\n", self->greeting);
}

static void initialize(Class *clazz) {
    ((HelloInterface *) clazz->interface)->helloWithGreeting = helloWithGreeting;
    ((HelloInterface *) clazz->interface)->initWithGreeting  = initWithGreeting;
    ((HelloInterface *) clazz->interface)->sayHello          = sayHello;
}

Class *_Hello(void) {
    static Class *clazz;
    static Once once;

    do_once(&once, {
        clazz = _initialize(&(const ClassDef) {
            .name            = "Hello",
            .superclass      = _Object(),
            .instanceSize    = sizeof(Hello),
            .interfaceOffset = offsetof(Hello, interface),
            .interfaceSize   = sizeof(HelloInterface),
            .initialize      = initialize,
        });
    });

    return clazz;
}

#undef _Class
```

## Using a Type

Using your custom type is trivial. Instantiate it like you would any Objectively type.

```c
Hello *hello = $(alloc(Hello), initWithGreeting, "Hello, World!");

$(hello, sayHello);

release(hello);
```

See [Hello.h](https://github.com/jdolan/Objectively/blob/main/Examples/Hello.h) and [Hello.c](https://github.com/jdolan/Objectively/blob/main/Examples/Hello.c) for the full source.

## Initialization

There is no explicit setup or teardown with Objectively. To instantiate a type, simply call `alloc` from anywhere in your program. The first time a type is instantiated, its Class initializer, `initialize`, is called. Use `initialize` to setup your interface, override methods, or initialize a library your class wraps. When your application terminates, an optional Class destructor, `destroy`, is also called.

## Invoking an instance method

To invoke an instance method, use the `$` macro.

```c
    $(condition, waitUntilDate, date);
```

## Invoking a Class method

To invoke a Class method, use the `$$` macro.

```c
    Dictionary *dict = $$(Dictionary, dictionary);
```

## Overriding a method

To override a method, overwrite the function pointer from within your Class' `initialize` method.

```c
    ((ObjectInterface *) clazz->interface)->dealloc = dealloc;
    ((ObjectInterface *) clazz->interface)->isEqual = isEqual;
```

## Calling super

To invoke a supertype's method implementation, use the `super` macro.

```c
    super(Object, self, dealloc);
```

## Managing memory

Objectively uses reference counting to govern object retention. Newly instantiated Objects have a reference count of 1. To retain a strong reference to an Object, call `retain(obj)`. To relinquish it, call `release(obj)`. Once an Object's reference count reaches 0, it is deallocated. Remember to balance every `retain` with a `release`.

## Shared instances

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
  _sharedInstance = release(_sharedInstance);
}

// ...

URLSession *session = $$(URLSession, sharedInstance);
```

Remember to wire up the desctructor in your Class' initialization block. See [Once.h](https://github.com/jdolan/Objectively/blob/main/Sources/Objectively/Once.h) for details on `do_once`.
