/*
 * Objectively: Ultra-lightweight object oriented framework for GNU C.
 * Copyright (C) 2014 Jay Dolan <jay@jaydolan.com>
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 * claim that you wrote the original software. If you use this software
 * in a product, an acknowledgment in the product documentation would be
 * appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and must not be
 * misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source distribution.
 */

#include <Objectively.h>

typedef struct Hello Hello;
typedef struct HelloInterface HelloInterface;

/**
 * @brief The Hello type.
 */
struct Hello {

	/**
	 * @brief The superclass.
	 */
	Object object;

	/**
	 * @brief The interface.
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
	 * @brief The superclass interface.
	 */
	ObjectInterface objectInterface;

	/**
	 * @static
	 * @brief A factory method for instantiating Hello.
	 * @return A new Hello with the given `greeting`.
	 * @param greeting The greeting.
	 * @memberof Hello
	 */
	Hello *(*helloWithGreeting)(const char *greeting);

	/**
	 * @brief Initializes this Hello with the given `greeting`.
	 * @return The initialized Hello, or `NULL` on error.
	 * @param self The Hello.
	 * @param greeting The greeting.
	 * @memberof Hello
	 */
	Hello *(*initWithGreeting)(Hello *self, const char *greeting);

	/**
	 * @brief Prints this Hello's greeting to the console.
	 * @param self The Hello.
	 * @memberof Hello
	 */
	void (*sayHello)(const Hello *self);
};

/**
 * @fn Class *Hello::_Hello(void)
 * @brief The Hello archetype.
 * @return The Hello Class.
 * @memberof Hello
 */
OBJECTIVELY_EXPORT Class *_Hello(void);
