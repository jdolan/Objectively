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

#include <unistd.h>
#include <stdlib.h>

#include <check.h>

#include <Objectively.h>

START_TEST(mutableData)
	{
		MutableData *data = $(alloc(MutableData), init);
		$(data, appendBytes, (byte *) "123", 3);

		ck_assert_int_eq(3, data->data.length);
		ck_assert(strncmp("123", (char *) data->data.bytes, 3) == 0);

		$(data, setLength, 128);
		ck_assert_int_eq(128, data->data.length);
		ck_assert_int_eq(0, data->data.bytes[data->data.length - 1]);

		id mem = malloc(8192 * sizeof(byte));
		ck_assert(mem);

		memset(mem, 1, 8192 * sizeof(byte));

		Data *append = $(alloc(Data), initWithMemory, mem, 8192);
		ck_assert(append);

		$(data, appendData, append);
		release(append);

		ck_assert_int_eq(8192 + 128, data->data.length);
		ck_assert_int_eq(1, data->data.bytes[data->data.length - 1]);

		release(data);

	}END_TEST

int main(int argc, char **argv) {

	TCase *tcase = tcase_create("mutableData");
	tcase_add_test(tcase, mutableData);

	Suite *suite = suite_create("mutableData");
	suite_add_tcase(suite, tcase);

	SRunner *runner = srunner_create(suite);

	srunner_run_all(runner, CK_VERBOSE);
	int failed = srunner_ntests_failed(runner);

	srunner_free(runner);

	return failed;
}
