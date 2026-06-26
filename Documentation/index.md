Objectively {#index}
============

[Objectively](https://github.com/jdolan/Objectively) is an ultra-lightweight object oriented framework for [GNU C](https://www.gnu.org/software/gnu-c-manual/). It brings OOP semantics — inheritance, polymorphism, and automatic reference counting — to C without requiring C++ or Objective-C.

## Features

- **Single-parent inheritance** via _starts-with_ struct composition
- **Class and instance methods** with strongly typed interfaces
- **Automatic reference counting** memory management
- **Unicode strings** with multibyte character set support
- **Collections**: Array, Dictionary, Set, Vector, and more
- **JSON** parsing, marshaling, and JSONPath queries
- **Concurrency**: Lock, Condition, Thread, Operation, OperationQueue
- **Networking**: URLSession and URLSessionTask for HTTP/HTTPS
- **Resource loading** with pluggable ResourceProvider
- **Windows, macOS, Linux** — cross-platform

## Building

```sh
autoreconf -i
./configure
make && sudo make install
```

Include the main header in your source and link with pkg-config:

```c
#include <Objectively.h>
```

```sh
gcc `pkg-config --cflags --libs Objectively` -o myprogram *.c
```

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

```c
Hello *hello = $(alloc(Hello), initWithGreeting, "Hello, World!");

$(hello, sayHello);

release(hello);
```

See [Hello.h](https://github.com/jdolan/Objectively/blob/main/Examples/Hello.h) and [Hello.c](https://github.com/jdolan/Objectively/blob/main/Examples/Hello.c) for the full source.

## Dispatch Macros

Macro | Purpose
----- | -------
`$(obj, method, args)` | Invoke an instance method
`$$(Type, method, args)` | Invoke a class (static) method
`super(Type, obj, method, args)` | Call the superclass method implementation
`alloc(Type)` | Allocate and zero an instance (refcount 1)
`retain(obj)` | Increment the reference count
`release(obj)` | Decrement the reference count; deallocates at 0, returns `NULL`

## Memory Management

Objectively uses reference counting. Newly allocated objects have a reference count of 1. Balance every `retain` with a `release`. Because `release` always returns `NULL`, assign it back when nulling a field:

```c
self->child = release(self->child);
```

## API Reference

Browse the [class hierarchy](hierarchy.html) to navigate the full API.
