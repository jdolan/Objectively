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

#include <stdio.h>
#include <string.h>
#include <check.h>

#include "../objectively.h"

Interface(Shape, Object)
	void (*hello)(Shape *self);
End

Constructor(Shape);

static void Shape_hello(Shape *self) {
	printf("shape\n");
}

Implementation(Shape)

	Initialize(Shape);

	if (Object_init((Object *) self)) {
		self->hello = Shape_hello;
	}

	return self;

End

Interface(Rectangle, Shape)
End

static void Rectangle_hello(Rectangle *self) {
	printf(">rectangle\n");
	Super(Shape, self, hello);
}

Implementation(Rectangle)

	Initialize(Rectangle);

	if (Shape_init((Shape *) self)) {
		Override(Shape, hello, Rectangle_hello);
	}

	return self;

End

Interface(Square, Rectangle)
End

static void Square_hello(Square *self) {
	printf(">>square\n");
	Super(Shape, self, hello);
}

Implementation(Square)

	Initialize(Square);

	if (Rectangle_init((Rectangle *) self)) {
		Override(Shape, hello, Square_hello);
	}

	return self;

End

START_TEST(shapes)
	{
		Square *square = New(Square);
		((Shape *) square)->hello((Shape *) square);
		Destroy(square);

	}END_TEST

int main(int argc, char **argv) {

	TCase *tcase = tcase_create("shapes");
	tcase_add_test(tcase, shapes);

	Suite *suite = suite_create("shapes");
	suite_add_tcase(suite, tcase);

	SRunner *runner = srunner_create(suite);

	srunner_run_all(runner, CK_VERBOSE);
	int failed = srunner_ntests_failed(runner);

	srunner_free(runner);

	return failed;
}
