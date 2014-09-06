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

    /* foo.h */

    #include <objectively.h>

    typedef struct Foo {
        Object object;
        void (*bar)(const Foo *self);
        const char *baz;
        // ...
    } Foo;

    extern const Class *__Foo;

Implementing a type
---

    /* foo.c */

    #include "foo.h"

    static void bar(const Foo *self) {
        printf("%s\n", self->bar);
    }

    static id init(id obj, va_list *args) {

        Foo *self = (Foo *) super(Object, obj, init, args);
        if (self) {
            override(Object, self, init, init);

            self->bar = bar;
            self->baz = arg(args, const char *, NULL);
        }
        return self;
    }

    /* class initialization boilerplate */

    static Foo foo;

    static struct Class class {
        .name = "Foo",
        .size = sizeof(Foo),
        .superclass = &__Object,
        .archetype = &foo,
        .init = init,
    };

    const Class *__Foo = &class;

Using a type
---

    Foo *foo = new(Foo, "hello world!");
	$(foo, bar);
	delete(foo);

Initialization
---
Classes are initialized at compile time in Objectively, which means you rarely have to monkey with them at runtime. To instantiate a type, simply call `new` from anywhere in your program. The first time a type is instantiated, an optional Class initializer, `initialize`, is called. Use `initialize` for things like singletons and other shared resources.

Archetypes
---
For each Class, an _archtype_ exists. The archetype is a statically allocated instance of the corresponding type that serves to hold the default method implementations. This serves as the basis for Objectively's inheritance graph. The archetype is initialized the first time the type is referenced in the application.

Invoking a method
---
To invoke a method, call the function pointer or use the `$` macro.

	self->isEqual(self, other);

	/* or */

	$(self, isEqual, other);

Overriding a method
---
To override a method, simply overwrite the function pointer or use the `override` macro. Overrides are typically installed in the initializer method.

	((Object *) self)->dealloc = dealloc;

	/* or */

	override(Object, self, dealloc, dealloc);

Calling super
---
To invoke a supertype's method implementation, either directly invoke the desired archetype implementation or use the `super` macro.

	((Object *) archetype(__Object))->dealloc(self);

	/* or */

	super(Object, self, dealloc);
