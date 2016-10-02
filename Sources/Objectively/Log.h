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

#pragma once

#include <stdio.h>

#include <Objectively/Object.h>

/**
 * @file
 * @brief A Log4J-inspired log appender.
 */

/**
 * @brief Every Log has a threshold for generating messages.
 * Messages beneath the set threshold level are not output. The default level
 * for newly initialized Logs is `LogLevelInfo`.
 */
typedef enum {
	LogLevelTrace,
	LogLevelDebug,
	LogLevelInfo,
	LogLevelWarn,
	LogLevelError,
	LogLevelFatal
} LogLevel;

/**
 * @brief The default Log format.
 */
#define LOG_FORMAT_DEFAULT "%c %%n [%%l]: %%m"

typedef struct Log Log;
typedef struct LogInterface LogInterface;

/**
 * @brief A Log4J-inspired log appender.
 * @extends Object
 */
struct Log {

	/**
	 * @brief The parent.
	 */
	Object object;

	/**
	 * @brief The typed interface.
	 * @protected
	 */
	LogInterface *interface;

	/**
	 * @brief The format string, defaults to `LOG_FORMAT_DEFAULT`.
	 * This string is post-processed after date substitution is performed by
	 * `strftime`. The following additional tokens are supported:
	 * * %%n - The Log name.
	 * * %%l - The message level.
	 * * %%m - The message.
	 */
	const char *format;

	/**
	 * @brief The file descriptor (defaults to `stdout`).
	 * @remarks Non-`tty` files are closed when the Log is deallocated.
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
 * @brief The Log interface.
 */
struct LogInterface {

	/**
	 * @brief The parent interface.
	 */
	ObjectInterface objectInterface;

	/**
	 * @fn void Log::debug(const Log *self, const char *fmt, ...)
	 * @brief Log a debug message.
	 * @param self The Log.
	 * @param fmt The format string.
	 * @memberof Log
	 */
	void (*debug)(const Log *self, const char *fmt, ...) __attribute__ ((format (printf, 2, 3)));

	/**
	 * @fn void Log::error(const Log *self, const char *fmt, ...)
	 * @brief Log an error message.
	 * @param self The Log.
	 * @param fmt The format string.
	 * @memberof Log
	 */
	void (*error)(const Log *self, const char *fmt, ...) __attribute__ ((format (printf, 2, 3)));

	/**
	 * @fn void Log::fatal(const Log *self, const char *fmt, ...)
	 * @brief Log a fatal message.
	 * @param self The Log.
	 * @param fmt The format string.
	 * @memberof Log
	 */
	void (*fatal)(const Log *self, const char *fmt, ...) __attribute__ ((format (printf, 2, 3)));

	/**
	 * @fn void Log::flush(const Log *self)
	 * @brief Flushes and pending output to this Log's file.
	 * @param self The Log.
	 * @memberof Log
	 */
	void (*flush)(const Log *self);

	/**
	 * @fn void Log::info(const Log *self, const char *fmt, ...)
	 * @brief Log an info message.
	 * @param self The Log.
	 * @param fmt The format string.
	 * @memberof Log
	 */
	void (*info)(const Log *self, const char *fmt, ...) __attribute__ ((format (printf, 2, 3)));

	/**
	 * @fn Log *Log::init(Log *self)
	 * @brief Initializes this Log.
	 * @param self The Log.
	 * @return The initialized Log, or `NULL` on error.
	 * @memberof Log
	 */
	Log *(*init)(Log *self);

	/**
	 * @fn Log *Log::initWithName(Log *self, const char *name)
	 * @brief Initializes this Log with the specified name.
	 * @param self The Log.
	 * @param name The Log name.
	 * @return The initialized Log, or `NULL` on error.
	 * @memberof Log
	 */
	Log *(*initWithName)(Log *self, const char *name);

	/**
	 * @fn void Log::log(const Log *self, LogLevel level, const char *fmt, va_list args)
	 * @brief Write a message to the Log.
	 * @param self The Log.
	 * @param level The severity of the message, which must be greater than or equal to the
	 * configured level of this Log.
	 * @param fmt The format string.
	 * @param args The format arguments.
	 * @memberof Log
	 */
	void (*log)(const Log *self, LogLevel level, const char *fmt, va_list args);

	/**
	 * @static
	 * @fn Log *Log::sharedInstance(void)
	 * @return The shared Log instance.
	 * @memberof Log
	 */
	Log *(*sharedInstance)(void);
	
	/**
	 * @fn void Log::trace(const Log *self, const char *fmt, ...)
	 * @brief Log a trace message.
	 * @memberof Log
	 */
	void (*trace)(const Log *self, const char *fmt, ...) __attribute__ ((format (printf, 2, 3)));

	/**
	 * @fn void Log::warn(const Log *self, const char *fmt, ...)
	 * @brief Log a warn message.
	 * @memberof Log
	 */
	void (*warn)(const Log *self, const char *fmt, ...) __attribute__ ((format (printf, 2, 3)));
};

/**
 * @brief The Log Class.
 */
extern Class _Log;
