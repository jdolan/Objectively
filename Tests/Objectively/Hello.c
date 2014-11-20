// Hello.h

#include <stdio.h>

#include <Objectively.h>

typedef struct Hello Hello;
typedef struct HelloInterface HelloInterface;

struct Hello {
	Object object;
	HelloInterface *interface;

	const char *greeting;
};

struct HelloInterface {
	ObjectInterface objectInterface;

	Hello *(*init)(Hello *self);
	Hello *(*initWithGreeting)(Hello *self, const char *greeting);

	void (*sayHello)(const Hello *self);
};

extern Class __Hello;

// Hello.c

#define __Class __Hello

#pragma mark - Hello initializers

static Hello *init(Hello *self) {
	return $(self, initWithGreeting, NULL);
}

static Hello *initWithGreeting(Hello *self, const char *greeting) {

	self = (Hello *) super(Object, self, init);
	if (self) {
		self->greeting = greeting ? : "hello";
	}
	return self;
}

#pragma mark - HelloInterface

static void sayHello(const Hello *self) {
	printf("%s\n", self->greeting);
}

#pragma mark - Hello class methods

static void initialize(Class *clazz) {

	HelloInterface *hello = (HelloInterface *) clazz->interface;

	hello->init = init;
	hello->initWithGreeting = initWithGreeting;
	hello->sayHello = sayHello;
}

Class __Hello = {
	.name = "Hello",
	.superclass = &__Object,
	.instanceSize = sizeof(Hello),
	.interfaceOffset = offsetof(Hello, interface),
	.interfaceSize = sizeof(HelloInterface),
	.initialize = initialize,
};

#undef __Class

#pragma mark - main

int main(int argc, char **argv) {

	Hello *hello = $(alloc(Hello), initWithGreeting, "Hello World!");
	$(hello, sayHello);
	release(hello);
}
