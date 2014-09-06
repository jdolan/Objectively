Objectively
===
Ultra-lightweight object oriented framework for the c programming language. Zlib license.

Adding Objectively to your project
---
1. Do the Autotools dance: `./configure; make; sudo make install`
2. Include the main header file in your source: `#include <objectively.h>`
3. Compile and link with Objectively: `pkg-config --cflags --libs objectively`

Declaring a type
---
```c
/* foo.h */

#include <objectively.h>

struct Foo {
    Object object;
    int bar;
    void (*baz)(const id self);
    // ...
}

extern const Class *Foo;
```

Implementing a type:
---
```c

/* foo.c */

#include "foo.h"

static void baz(const id self) {
    const struct Foo *this = cast(Foo, self);
    printf("%d\n", this->bar);
}

static id init(id self, va_list *args) {

    self = super(Object, self, init, args);
    if (self) {
        override(Object, self, init, init);
        
    	struct Foo *this = cast(Foo, self);
    	
    	this->bar = 0x69;
        this->baz = baz;
    }
    return self;
}

static struct Foo foo;

static struct Class class {
    .name = "Foo",
    .size = sizeof(struct Foo),
    .superclass = &Object,
    .archetype = &foo,
    .init = init,
};

const Class *Foo = &class;

```

Using a type:
---
```c
	struct Foo *foo = new(Foo);
	$(foo, baz);
	delete(foo);
```

Initialization
---
Types are initialized at compile time in Objectively, which means you rarely have to monkey with them at runtime. To instantiate a type, simply call `new` from anywhere in your program. The first time a type is instantiated, an optional `initialize` method is invoked. Use `initialize` for things like singletons and other shared resources.

Archetypes
---
For each class, an _archtype_ exists. The archetype is a statically allocated instance of the type that serves to hold the default method implementations. This serves as the basis for Objectively's inheritance graph. The archetype is initialized the first time the type is referenced in the application.

Invoking a method
---
To invoke a method, call the function pointer or use the `$` macro.

```c
	this->isEqual(this, that);
	
	/* or */
	
	$(this, isEqual, that);
```

Overriding a method
---
To override a method, simply overwrite the function pointer in your constructor or use the `override` macro.

```c
	((struct Object *) self)->dealloc = Foo_dealloc;

	/* or */

	override(Object, self, dealloc, Foo_dealloc);
```

Calling super
---
To invoke a supertype's method implementation, either directly invoke the desired archetype implementation or use the `super` macro.

```c
	((struct Object *) archetype(Object))->dealloc(self);
	
	/* or */

	super(Object, self, dealloc);
```
