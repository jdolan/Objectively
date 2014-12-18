// Hello.h

#include <stdio.h>

#include <Objectively.h>

typedef struct Hello Hello;
typedef struct HelloInterface HelloInterface;

/**
 * @brief The Hello type.
 */
struct Hello {

	/**
	 * @brief The parent.
	 */
	Object object;

	/**
	 * @brief The typed interface.
	 */
	HelloInterface *interface;

	/**
	 * @brief The greeting.
	 */
	const char *greeting;
};

/**
 * @brief The Hello interface.
 */
struct HelloInterface {

	/**
	 * @brief The parent interface.
	 */
	ObjectInterface objectInterface;

	/**
	 * @brief A factory method for instantiating Hello.
	 *
	 * @return A new Hello with the given `greeting`.
	 *
	 * @relates Hello
	 */
	Hello *(*helloWithGreeting)(const char *greeting);

	/**
	 * @brief Initializes this Hello with the given `greeting`.
	 *
	 * @return The initialized Hello, or `NULL` on error.
	 *
	 * @relates Hello
	 */
	Hello *(*initWithGreeting)(Hello *self, const char *greeting);

	/**
	 * @brief Prints this Hello's greeting to the console.
	 *
	 * @relates Hello
	 */
	void (*sayHello)(const Hello *self);
};

/**
 * @brief The Hello Class.
 */
extern Class _Hello;

// Hello.c

#define _Class _Hello

#pragma mark - HelloInterface

/**
 * @see HelloInterface::helloWithGreeting(const char *)
 */
static Hello *helloWithGreeting(const char *greeting) {
	return $(alloc(Hello), initWithGreeting, greeting);
}

/**
 * @see HelloInterface::initWithGreeting(Hello *, const char *)
 */
static Hello *initWithGreeting(Hello *self, const char *greeting) {

	self = (Hello *) super(Object, self, init);
	if (self) {
		self->greeting = greeting ? : "Hello World!";
	}
	return self;
}

/**
 * @see HelloInterface::sayHello(const Hello *)
 */
static void sayHello(const Hello *self) {
	printf("%s\n", self->greeting);
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	HelloInterface *hello = (HelloInterface *) clazz->interface;

	hello->helloWithGreeting = helloWithGreeting;
	hello->initWithGreeting = initWithGreeting;
	hello->sayHello = sayHello;
}

Class _Hello = {
	.name = "Hello",
	.superclass = &_Object,
	.instanceSize = sizeof(Hello),
	.interfaceOffset = offsetof(Hello, interface),
	.interfaceSize = sizeof(HelloInterface),
	.initialize = initialize,
};

#undef _Class

#pragma mark - main

int main(int argc, char **argv) {

	Hello *hello = $$(Hello, helloWithGreeting, NULL);

	$(hello, sayHello);

	release(hello);
}
