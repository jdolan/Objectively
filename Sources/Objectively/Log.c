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

#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "Config.h"

#if HAVE_UNISTD_H
#include <unistd.h>
#endif

#include "Log.h"

#define _Class _Log

#pragma mark - Object

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

	Log *this = (Log *) self;

	if (!isatty(fileno(this->file))) {
		const int err = fclose(this->file);
		assert(err == 0);
	}

	free(this->name);

	super(Object, self, dealloc);
}

#pragma mark - Log

/**
 * @fn void Log::debug(const Log *self, const char *fmt, ...)
 * @memberof Log
 */
static void debug(const Log *self, const char *fmt, ...) {

	va_list args;
	va_start(args, fmt);

	$(self, log, LogLevelDebug, fmt, args);

	va_end(args);
}

/**
 * @fn void Log::error(const Log *self, const char *fmt, ...)
 * @memberof Log
 */
static void error(const Log *self, const char *fmt, ...) {

	va_list args;
	va_start(args, fmt);

	$(self, log, LogLevelError, fmt, args);

	va_end(args);
}

/**
 * @fn void Log::fatal(const Log *self, const char *fmt, ...)
 * @memberof Log
 */
static void fatal(const Log *self, const char *fmt, ...) {

	va_list args;
	va_start(args, fmt);

	$(self, log, LogLevelFatal, fmt, args);

	va_end(args);
}

/**
 * @fn void Log::flush(const Log *self)
 * @memberof Log
 */
static void flush(const Log *self) {

	assert(self->file);
	fflush(self->file);
}

/**
 * @fn void Log::info(const Log *self, const char *fmt, ...)
 * @memberof Log
 */
static void info(const Log *self, const char *fmt, ...) {

	va_list args;
	va_start(args, fmt);

	$(self, log, LogLevelInfo, fmt, args);

	va_end(args);
}

/**
 * @fn Log *Log::init(Log *self)
 * @memberof Log
 */
static Log *init(Log *self) {

	return $(self, initWithName, NULL);
}

/**
 * @fn Log *Log::initWithName(Log *self, const char *name)
 * @memberof Log
 */
static Log *initWithName(Log *self, const char *name) {

	self = (Log *) super(Object, self, init);
	if (self) {
		self->name = strdup(name ?: "default");
		self->level = LogLevelInfo;
		self->format = LOG_FORMAT_DEFAULT;
		self->file = stdout;
	}

	return self;
}

/**
 * @fn void Log::log(const Log *self, LogLevel level, const char *fmt, va_list args)
 * @memberof Log
 */
static void _log(const Log *self, LogLevel level, const char *fmt, va_list args) {

	const char *levels[] = { "TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL" };
	assert(level < lengthof(levels));

	if (level < self->level) {
		return;
	}

	assert(self->file);

	const time_t date = time(NULL);
	const struct tm *localDate = localtime(&date);

	char buffer[128];
	strftime(buffer, sizeof(buffer), self->format, localDate);

	char *c = buffer;
	while (*c) {

		if (*c == '%') {
			bool token = true;

			if (*(c + 1) == 'n') {
				fputs(self->name, self->file);
			} else if (*(c + 1) == 'l') {
				fputs(levels[level], self->file);
			} else if (*(c + 1) == 'm') {
				vfprintf(self->file, fmt, args);
			} else {
				token = false;
			}

			if (token) {
				c++;
				c++;
				continue;
			}
		}

		fputc(*c, self->file);
		c++;
	}

	fputc('\n', self->file);
	fflush(self->file);
}

static Log *_sharedInstance;

/**
 * @fn Log *Log::sharedInstance(void)
 * @memberof Log
 */
static Log *sharedInstance(void) {

	static Once once;

	do_once(&once, {
		_sharedInstance = $(alloc(Log), init);
	});

	return _sharedInstance;
}

/**
 * @fn void Log::trace(const Log *self, const char *fmt, ...)
 * @memberof Log
 */
static void trace(const Log *self, const char *fmt, ...) {

	va_list args;
	va_start(args, fmt);

	$(self, log, LogLevelTrace, fmt, args);

	va_end(args);
}

/**
 * @fn void Log::warn(const Log *self, const char *fmt, ...)
 * @memberof Log
 */
static void warn(const Log *self, const char *fmt, ...) {

	va_list args;
	va_start(args, fmt);

	$(self, log, LogLevelWarn, fmt, args);

	va_end(args);
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	((ObjectInterface *) clazz->interface)->dealloc = dealloc;

	((LogInterface *) clazz->interface)->debug = debug;
	((LogInterface *) clazz->interface)->error = error;
	((LogInterface *) clazz->interface)->fatal = fatal;
	((LogInterface *) clazz->interface)->flush = flush;
	((LogInterface *) clazz->interface)->info = info;
	((LogInterface *) clazz->interface)->init = init;
	((LogInterface *) clazz->interface)->initWithName = initWithName;
	((LogInterface *) clazz->interface)->log = _log;
	((LogInterface *) clazz->interface)->trace = trace;
	((LogInterface *) clazz->interface)->sharedInstance = sharedInstance;
	((LogInterface *) clazz->interface)->warn = warn;
}

/**
 * @see Class::destroy(Class *)
 */
static void destroy(Class *clazz) {

	_sharedInstance = release(_sharedInstance);
}

/**
 * @fn Class *Log::_Log(void)
 * @memberof Log
 */
Class *_Log(void) {
	static Class *clazz;
	static Once once;

	do_once(&once, {
		clazz = _initialize(&(const ClassDef) {
			.name = "Log",
			.superclass = _Object(),
			.instanceSize = sizeof(Log),
			.interfaceOffset = offsetof(Log, interface),
			.interfaceSize = sizeof(LogInterface),
			.initialize = initialize,
			.destroy = destroy,
		});
	});

	return clazz;
}

#undef _Class
