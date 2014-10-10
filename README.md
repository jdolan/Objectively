Objectively
===
Ultra-lightweight object oriented framework for the C programming language. Zlib license.

Adding Objectively to your project
---

1) Do the Autotools dance.

    ./configure; make; sudo make install

2) Include the main header file in your source.

    #include <Objectively.h>

3) Compile and link with Objectively.

    gcc `pkg-config --cflags --libs objectively` -o myprogram *.c

Declaring a type
---

Types in Objectively are comprised of 3 components:

1) The instance struct, containing the parent type, the interface pointer, and any additional fields.

    struct Hello {
        Object object;
        const HelloInterface *interface;
        
        const char *bar;
    };
    
2) The interface struct, containing the parent interface and any additional methods.

    struct HelloInterface {
        ObjectInterface objectInterface;

        void (*baz)(const Hello *self);
    };

3) The class descriptor, serving to tie 1) and 2) together.

    extern Class __Hello;

Implementing a type
---

To implement a type, define its initializer, instance methods and Class initializer:

    #include <stdio.h>
    #include <Objectively.h>
    
    #define __Class __Hello
        
    static Object *init(id obj, id interface, va_list *args) {

        Hello *self = (Hello *) super(Object, obj, init, interface, args);
        if (self) {
            self->interface = (HelloInterface *) interface;
            self->bar = $arg(args, const char *, NULL);
        }
        return self;
    }
            
    static void baz(const Hello *self) {
        printf("%s\n", self->bar);
    }
        
    static void initialize(Class *clazz) {
        ((ObjectInterface *) clazz->interface)->init = init;
        ((HelloInterface *) clazz->interface)->baz = baz;
    }

    Class __Hello = {
        .name = "Hello",
        .superclass = &__Object,
        .instanceSize = sizeof(Hello),
        .interfaceSize = sizeof(HelloInterface),
        .initialize = initialize
    };
    
    #undef __Class

Using a type
---

    Hello *hello = new(Hello, "hello world!");
    $(hello, bar);
    release(hello);


See [Hello.c](test/Objectively/Hello.c) for the full source to this example.

Initialization
---
There is no explicit setup or teardown with Objectively. To instantiate a type, simply call `new` from anywhere in your program. The first time a type is instantiated, an optional Class initializer, `initialize`, is called. Use `initialize` to setup your interface, override methods, or create a singleton. When your application terminates, an optional Class destructor, `destroy`, is also called.

Invoking a method
---
To invoke a method, use the `$` macro.

    $(self, isEqual, other);

Overriding a method
---
To override a method, overwrite the function pointer from within your Class' `initialize` method.

    ((ObjectInterface *) self->interface)->init = init;
    ((ObjectInterface *) self->interface)->isEqual = isEqual;

Calling super
---
To invoke a supertype's method implementation, use the `super` macro.

    super(Object, self, dealloc);
    
Managing memory
---
Objectively uses reference counting to govern object retention. Newly instantiated Objects have a reference count of 1. To maintain a strong reference to an Object, call `retain(obj)`. To dispose of it, call `release(obj)`. Once an Object's reference count reaches 0, it is deallocated. Remember to balance every `retain` with a `release`.

Core library
---
Objectively provides a small but useful core library:

 * Object - Hurr.
 * String - Mutable strings.

Collections:

 * Array - Mutable arrays.
 * Dictionary - Mutable dictionaries.

Concurrency:

 * Condition - Conditions (pthreads).
 * Lock - Locks (pthreads).
 * Thread - Threads (pthreads).

Miscellaneous:
 * Log - Configurable logging.
