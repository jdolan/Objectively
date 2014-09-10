// foo.h

#include <stdio.h>
#include <Objectively.h>

typedef struct Foo Foo;
typedef struct FooInterface FooInterface;

struct Foo {
	Object object;

	const char *bar;
	const FooInterface *interface;
};

struct FooInterface {
	ObjectInterface objectInterface;

	void (*baz)(const Foo *self);
};

extern Class __Foo;

// foo.c

#pragma mark - Object instance methods

static Object *init(id obj, id interface, va_list *args) {

	Foo *self = (Foo *) super(Object, obj, init, interface, args);
	if (self) {
		self->interface = (FooInterface *) interface;
		self->bar = arg(args, const char *, NULL);
	}
	return (Object *) self;
}

#pragma mark - Foo instance methods

static void baz(const Foo *self) {
	printf("%s\n", self->bar);
}

#pragma mark - Foo class methods

static void initialize(Class *class) {

	((ObjectInterface *) class->interface)->init = init;

	((FooInterface *) class->interface)->baz = baz;
}

Class __Foo = {
	.name = "Foo",
	.superclass = &__Object,
	.instanceSize = sizeof(Foo),
	.interfaceSize = sizeof(FooInterface),
	.initialize = initialize,
};

#pragma mark - main

int main(int argc, char **argv) {

	Foo *foo = new(Foo, "hello world!");
	$(foo, baz);
	release(foo);
}
