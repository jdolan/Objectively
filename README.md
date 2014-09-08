Objectively
===
Ultra-lightweight object oriented framework for the c programming language. Zlib license.

Adding Objectively to your project
---

1) Do the Autotools dance.

    ./configure; make; sudo make install

2) Include the main header file in your source.

    #include <objectively.h>

3) Compile and link with Objectively.

    gcc `pkg-config --cflags --libs objectively` -o foo *.c

Declaring a type
---

    // foo.h

    #include <objectively.h>

    typedef struct Foo Foo;
    typedef struct FooInterface FooInterface;

    struct Foo {
        Object object;

        int bar;
        // ...

        const FooInterface *interface;
    };

    struct FooInterface {
        ObjectInterface objectInterface;

        // ...

        void (*baz)(const Foo *self);
    };

    extern Class __Foo;

Implementing a type
---

    // foo.c
    
    #include <stdio.h>
    #include "foo.h"

    #pragma mark - Object instance methods

    static Object *init(id obj, id interface, va_list *args) {

        Foo *self = (Foo *) super(Object, obj, init, interface, args);
        if (self) {
            self->interface = (FooInterface *) interface;

            self->bar = arg(args, int, 0x69);
            // ...
        }
        return self;
    }

    #pragma mark - Foo instance methods

    static void baz(const Foo *self) {
        printf("%d\n", self->bar);
    }

    #pragma mark - Foo class methods

    static void initialize(Class *class) {

        ((ObjectInterface *) interface)->init = init;
	((FooInterface *) interface)->baz = baz;
    }

    Class __Foo = {
        .name = "Foo",
        .superclass = &__Object,
        .instanceSize = sizeof(Foo),
        .interfaceSize = sizeof(FooInterface),
        .initialize = initialize, };

Using a type
---

    Foo *foo = new(Foo, "hello world!");
    $(foo, bar);
    delete(foo);

Initialization
---
Classes are initialized at compile time in Objectively, which means you rarely have to monkey with them at runtime. To instantiate a type, simply call `new` from anywhere in your program. The first time a type is instantiated, an optional Class initializer, `initialize`, is called. Use `initialize` for things like singletons and other shared resources.

Invoking a method
---
To invoke a method, use the `$` macro.

    $(self, isEqual, other);

Overriding a method
---
To override a method, overwrite the function pointer from within your Class' `initialize` method.

    ((ObjectInterface *) interface)->init = init;
    ((ObjectInterface *) interface)->isEqual = isEqual;

Calling super
---
To invoke a supertype's method implementation, use the `super` macro.

    super(Object, self, dealloc);
