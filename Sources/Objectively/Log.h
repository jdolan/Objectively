/*
 * Objectively: Ultra-lightweight object oriented framework for c99.
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

#ifndef _Objectively_Log_h
#define _Objectively_Log_h

#include <stdio.h>

#include <objectively/Object.h>

typedef enum {
	TRACE, DEBUG, INFO, WARN, ERROR, FATAL
} LogLevel;

#define LOG_FORMAT_DEFAULT "%c %%n [%%l]: %%m"

typedef struct Log Log;
typedef struct LogInterface LogInterface;

/**
 * @brief The Log type.
 */
struct Log {

	/**
	 * @brief The parent.
	 */
	Object object;

	/**
	 * @brief The typed interface.
	 */
	LogInterface *interface;

	/**
	 * @brief The format string, defaults to `LOG_FORMAT_DEFAULT`.
	 *
	 * This string is post-processed after date substitution is performed by
	 * `strftime`. The following tokens are supported:
	 *
	 * * %%n - The Log name.
	 * * %%l - The message level.
	 * * %%m - The message.
	 */
	const char *format;

	/**
	 * @brief The file descriptor (defaults to `stdout`).
	 *
	 * @remark Non-`tty` files are closed when the Log is deallocated.
	 */
	FILE *file;

	/**
	 * @brief The LogLevel of this Log.
	 */
	LogLevel level;

	/**
	 * @brief The name of this Log.
	 */
	char *name;
};

/**
 * @brief The Log type.
 */
struct LogInterface {

	/**
	 * @brief The parent.
	 */
	ObjectInterface objectInterface;

	/**
	 * @brief Log a debug message.
	 */
	void (*debug)(const Log *self, const char *fmt, ...) __attribute__ ((format (printf, 2, 3)));

	/**
	 * @brief Log an error message.
	 */
	void (*error)(const Log *self, const char *fmt, ...) __attribute__ ((format (printf, 2, 3)));

	/**
	 * @brief Log a fatal message.
	 */
	void (*fatal)(const Log *self, const char *fmt, ...) __attribute__ ((format (printf, 2, 3)));

	/**
	 * @brief Flushes and pending output to this Log's file.
	 */
	void (*flush)(const Log *self);

	/**
	 * @brief Log an info message.
	 */
	void (*info)(const Log *self, const char *fmt, ...) __attribute__ ((format (printf, 2, 3)));

	/**
	 * @brief Initializes this Log.
	 *
	 * @return The newly initialized Log.
	 */
	Log *(*init)(Log *self);

	/**
	 * Initializes this Log with the specified name.
	 *
	 * @param name The Log name.
	 *
	 * @return The initialized Log, or NULL on error.
	 */
	Log *(*initWithName)(Log *self, const char *name);

	/**
	 * @brief Write a message to the Log.
	 *
	 * @param level The severity of the message, which must be greater than or
	 * equal to the configured level of this Log.
	 */
	void (*log)(const Log *self, LogLevel level, const char *fmt, va_list args);

	/**
	 * @brief Log a trace message.
	 */
	void (*trace)(const Log *self, const char *fmt, ...) __attribute__ ((format (printf, 2, 3)));

	/**
	 * @brief Log a warn message.
	 */
	void (*warn)(const Log *self, const char *fmt, ...) __attribute__ ((format (printf, 2, 3)));
};

/**
 * @brief The Log Class.
 */
extern Class __Log;

#endif
