// Hello.h

#include <stdio.h>

#include <Objectively.h>

#define __Class __Hello

typedef struct Hello Hello;
typedef struct HelloInterface HelloInterface;

struct Hello {
	Object object;

	const HelloInterface *interface;

	const char *greeting;
};

struct HelloInterface {
	ObjectInterface objectInterface;

	void (*hello)(const Hello *self);
};

extern Class __Hello;

// Hello.c

#pragma mark - Object instance methods

static Object *init(id obj, id interface, va_list *args) {

	Hello *self = (Hello *) super(Object, obj, init, interface, args);
	if (self) {
		self->interface = (HelloInterface *) interface;
		self->greeting = $arg(args, const char *, NULL);
	}
	return (Object *) self;
}

#pragma mark - Hello instance methods

static void hello(const Hello *self) {
	printf("%s\n", self->greeting);
}

#pragma mark - Hello class methods

static void initialize(Class *class) {

	((ObjectInterface *) class->interface)->init = init;
	((HelloInterface *) class->interface)->hello = hello;
}

Class __Hello = {
	.name = "Hello",
	.superclass = &__Object,
	.instanceSize = sizeof(Hello),
	.interfaceSize = sizeof(HelloInterface),
	.initialize = initialize,
};

#undef __Class

#pragma mark - main

int main(int argc, char **argv) {

	Hello *hello = new(Hello, "Hello World!");
	$(hello, hello);
	release(hello);
}
