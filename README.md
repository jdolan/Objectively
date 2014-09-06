Objectively
===
Ultra-lightweight object oriented framework for the c programming language. Zlib license.

Adding Objectively to your project
---
1. Do the Autotools dance (`./configure; make && make install`).
2. Include the main header file in your source (`#include <objectively.h>`).
3. Compile and link with Objectively (`pkg-config --cflags --libs objectively`).

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

Archetypes
---
For each declared type, an _archtype_ exists. The archetype is a statically allocated instance of the type that serves to hold the default method implementations. The archetype is defined and intialized by the `Implementation` and `Initialize` macros, respectively. The archetype for _FooBar_ is the symbol `__FooBar`.

Overriding a method
---
To override a method, simply overwrite the function pointer in your constructor or use the `Override` macro (*preferred*).

```c
	self->dealloc = (void (*)(Object *)) Foo_dealloc;

	/* or */

	Override(Object, self, dealloc, Foo_dealloc);
```

Calling super
---
To invoke a supertype's method implementation, either directly invoke the desired archetype implementation or use the `Super` macro (*strongly preferred*).

```c
	__Object.dealloc((Object *) self);
	
	/* or */

	Super(Object, self, dealloc);
```

FAQ
---
1. *Why?* Because c++ makes me puke, and Objective-c is not widely supported. 
1. *This looks like poor-man's Objective-c?* Yup.
