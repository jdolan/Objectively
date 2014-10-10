Objectively
===
Ultra-lightweight object oriented framework for the c programming language. Zlib license.

Adding Objectively to your project
---

1) Do the Autotools dance.

    ./configure; make; sudo make install

2) Include the main header file in your source.

    #include <Objectively.h>

3) Compile and link with Objectively.

    gcc `pkg-config --cflags --libs objectively` -o foo *.c

Declaring a type
---

Types in Objectively are comprised of 3 components:

1) The instance struct, containing the parent type and any additional fields.

    struct Foo {
        Object object;
        const char *bar;
        
        const FooInterface *interface;
    };
    
2) The interface struct, containing the parent interface and any additional methods.

    struct FooInterface {
        ObjectInterface objectInterface;

        void (*baz)(const Foo *self);
    };

3) The class descriptor, serving to tie 1) and 2) together.

    extern Class __Foo;

Implementing a type
---

    static void baz(const Foo *self) {
        printf("%s\n", self->bar);
    }
    
    static Object *init(id obj, id interface, va_list *args) {

        Foo *self = (Foo *) super(Object, obj, init, interface, args);
        if (self) {
            self->interface = (FooInterface *) interface;
            self->bar = arg(args, const char *, NULL);
        }
        return self;
    }

    static void initialize(Class *class) {
        ((ObjectInterface *) class->interface)->init = init;
        ((FooInterface *) class->interface)->baz = baz;
    }

    Class __Foo = {
        .name = "Foo",
        .superclass = &__Object,
        .instanceSize = sizeof(Foo),
        .interfaceSize = sizeof(FooInterface),
        .initialize = initialize
    };

Using a type
---

    Foo *foo = new(Foo, "hello world!");
    $(foo, bar);
    release(foo);


See [Foo.c](test/Objectively/Foo.c) for the full source to this example.

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

 * [Object](Sources/Objectively/Object.h) - Hurr.
 * [String](Sources/Objectively/String.h) - Mutable strings.

Collections:

 * [Array](Sources/Objectively/Array.h) - Mutable arrays.
 * [Dictionary](Sources/Objectively/Dictionary.h) - Mutable dictionaries.

Concurrency:

 * [Condition](Sources/Objectively/Condition.h) - Conditions (pthreads).
 * [Lock](Sources/Objectively/Lock.h) - Locks (pthreads).
 * [Thread](Sources/Objectively/Thread.h) - Threads (pthreads).

Miscellaneous:
 * [Log](Sources/Objectively/Log.h) - Configurable logging.
