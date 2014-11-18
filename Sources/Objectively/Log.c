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

#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <Objectively/Log.h>

#define __Class __Log

#pragma mark - Object instance methods

/**
 * @see ObjectInterface::dealloc(Object *)
 */
static void dealloc(Object *self) {

	Log *this = (Log *) self;

	if (!isatty(fileno(this->file))) {
		int err = fclose(this->file);
		assert(err == 0);
	}

	free(this->name);

	super(Object, self, dealloc);
}

#pragma mark - Log instance methods

/**
 * @see LogInterface::debug(const Log *, const char *, ...)
 */
static void debug(const Log *self, const char *fmt, ...) {

	va_list args;
	va_start(args, fmt);

	$(Log, self, log, DEBUG, fmt, args);

	va_end(args);
}

/**
 * @see LogInterface::error(const Log *, const char *, ...)
 */
static void error(const Log *self, const char *fmt, ...) {

	va_list args;
	va_start(args, fmt);

	$(Log, self, log, ERROR, fmt, args);

	va_end(args);
}

/**
 * @see LogInterface::fatal(const Log *, const char *, ...)
 */
static void fatal(const Log *self, const char *fmt, ...) {

	va_list args;
	va_start(args, fmt);

	$(Log, self, log, FATAL, fmt, args);

	va_end(args);
}

/**
 * @see LogInterface::flush
 */
static void flush(const Log *self) {

	assert(self->file);
	fflush(self->file);
}

/**
 * @see LogInterface::info(const Log *, const char *, ...)
 */
static void info(const Log *self, const char *fmt, ...) {

	va_list args;
	va_start(args, fmt);

	$(Log, self, log, INFO, fmt, args);

	va_end(args);
}

/**
 * @see LogInterface::init(Log *)
 */
static Log *init(Log *self) {

	return $(Log, self, initWithName, NULL);
}

/**
 * @see LogInterface::initWithName(Log *, const char *)
 */
static Log *initWithName(Log *self, const char *name) {

	self = (Log *) super(Object, self, init);
	if (self) {
		self->name = strdup(name ?: "default");
		self->level = INFO;
		self->format = LOG_FORMAT_DEFAULT;
		self->file = stdout;
	}

	return self;
}

/**
 * @brief Writes a formatted message to this Log's stream.
 *
 * @param level The message level.
 * @param fmt The user's format string.
 * @param args The format string arguments.
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
			BOOL token = YES;

			if (*(c + 1) == 'n') {
				fputs(self->name, self->file);
			} else if (*(c + 1) == 'l') {
				fputs(levels[level], self->file);
			} else if (*(c + 1) == 'm') {
				vfprintf(self->file, fmt, args);
			} else {
				token = NO;
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
}

/**
 * @see LogInterface::trace(const Log *, const char *, ...)
 */
static void trace(const Log *self, const char *fmt, ...) {

	va_list args;
	va_start(args, fmt);

	$(Log, self, log, TRACE, fmt, args);

	va_end(args);
}

/**
 * @see LogInterface::warn(const Log *, const char *, ...)
 */
static void warn(const Log *self, const char *fmt, ...) {

	va_list args;
	va_start(args, fmt);

	$(Log, self, log, WARN, fmt, args);

	va_end(args);
}

#pragma mark - Log Class methods

/**
 * see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	ObjectInterface *object = (ObjectInterface *) clazz->interface;

	object->dealloc = dealloc;

	LogInterface *log = (LogInterface *) clazz->interface;

	log->debug = debug;
	log->error = error;
	log->fatal = fatal;
	log->flush = flush;
	log->info = info;
	log->init = init;
	log->initWithName = initWithName;
	log->log = _log;
	log->trace = trace;
	log->warn = warn;
}

Class __Log = {
	.name = "Log",
	.superclass = &__Object,
	.instanceSize = sizeof(Log),
	.interfaceSize = sizeof(LogInterface),
	.initialize = initialize,
};

#undef __Class
