Objectively
===
Ultra-lightweight object oriented framework for the C programming language. Cocoa-inspired API. Zlib license.

Adding Objectively to your project
---

1) Do the Autotools dance.

    ./configure; make; sudo make install

2) Include the main header file in your source.

    #include <Objectively.h>

3) Compile and link with Objectively.

    gcc `pkg-config --cflags --libs Objectively` -o myprogram *.c

Declaring a type
---

Types in Objectively are comprised of 3 components:

1) The instance `struct`, beginning with the parent type, the type interface and then any additional instance variables.

    /**
     * The Hello type.
     */
    struct Hello {
    
        /**
         * The parent.
         */
        Object object;
        
        /**
         * The typed interface.
         */
        HelloInterface *interface;
        
        /**
         * The greeting.
         */
        const char *greeting;
    };

2) The interface `struct`, beginning with the parent interface, followed by any Class or instance methods.

	/**
     * The Hello interface.
     */
    struct HelloInterface {
    
        /**
         * The parent.
         */
        ObjectInterface objectInterface;
        
        /**
         * A factory method for instantiating Hello.
         *
         * @return A new Hello with the given `greeting`.
         */
        Hello *(*helloWithGreeting)(const char *greeting);
        
        /**
         * Initializes this Hello with the given `greeting`.
         *
         * @return The initialized Hello, or `NULL` on error.
         */
        Hello *(*initWithGreeting)(Hello *self, const char *greeting);
        
        /**
         * Prints this Hello's greeting to the console.
         */
        void (*sayHello)(const Hello *self);
    };

3) The class descriptor, serving to tie 1) and 2) together.

    /**
     * The Hello Class.
     */
    extern Class __Hello;

Implementing a type
---

To implement a type, implement its instance and Class methods and Class initializer:

    #include <stdio.h>
    #include <Objectively.h>
    
    #define __class __Hello
    
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
            self->greeting = greeting ?: "Hello World!";
        }
        return self;
    }
    
    /**
     * @see HelloInterface::sayHello(const Hello *)
     */
    static void sayHello(const Hello *self) {
        printf("%s\n", self->bar);
    }
    
    /**
     * @see Class::initialize(Class *)
     */
    static void initialize(Class *clazz) {
    
        HelloInterface *hello = (HelloInterface *) clazz->interface;
        
        hello->helloWithGreeting = helloWithGreeting;
        hello->initWithGreeting = initWithGreeting;
        hello->sayHello = sayHello;
    }

    Class __Hello = {
        .name = "Hello",
        .superclass = &__Object,
        .instanceSize = sizeof(Hello),
        .interfaceOffset = offsetof(Hello, interface),
        .interfaceSize = sizeof(HelloInterface),
        .initialize = initialize
    };
    
    #undef __class
    
Using a type
---

    Hello *hello = $$(Hello, helloWithGreeting, NULL);
    
    $(hello, sayHello);
    
    release(hello);

See [Hello.c](Tests/Objectively/Hello.c) for the full source to this example.

Initialization
---
There is no explicit setup or teardown with Objectively. To instantiate a type, simply call `alloc` from anywhere in your program. The first time a type is instantiated, an optional Class initializer, `initialize`, is called. Use `initialize` to setup your interface, override methods, or initialize a library your class wraps. When your application terminates, an optional Class destructor, `destroy`, is also called.

Invoking an instance method
---
To invoke an instance method, use the `$` macro.

    $(condition, waitUntilDate, date);

Invoking a Class method
---
To invoke a Class method, use the `$$` macro.

    Dictionary *dict = $$(JSONSerialization, objectWithData, data);

Overriding a method
---
To override a method, overwrite the function pointer from within your Class' `initialize` method.

    ((ObjectInterface *) clazz->interface)->dealloc = dealloc;
    ((ObjectInterface *) clazz->interface)->isEqual = isEqual;

Calling super
---
To invoke a supertype's method implementation, use the `super` macro.

    super(Object, self, dealloc);
    
Managing memory
---
Objectively uses reference counting to govern object retention. Newly instantiated Objects have a reference count of 1. To retain a strong reference to an Object, call `retain(obj)`. To relinquish it, call `release(obj)`. Once an Object's reference count reaches 0, it is deallocated. Remember to balance every `retain` with a `release`.

Shared instances
---
A shared instance or _singleton pattern_ can be achieved through Class methods and _release-on-destroy_.

    static URLSession *__sharedInstance;
    
    /**
     * @see URLSessionInterface::sharedInstance(void)
     */
    static *URLSession sharedInstance(void) {
        static Once once;
        
        DispatchOnce(once, {
            __sharedInstance = $(alloc(URLSession), init);
        });
        
        return __sharedInstance;
    }
    
    /**
     * @see Class::destroy(Class *)
     */
    static void destroy(Class *clazz) {
        release(__sharedInstance);
    }
    
    // ...
    
    URLSession *session = $$(URLSession, sharedInstance);

See [Once.h](Sources/Objectively/Once.h) for details on `DispatchOnce`.
    
API documentation
---
The API documentation can be [browsed online](http://jaydolan.com/projects/objectively) or generated with [Doxygen](http://www.doxygen.org) by running `make html`.
