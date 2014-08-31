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

Interface(Animal, Object)

	char *genus;
	char *species;

	char *(*scientificName)(Animal *self);

End

Constructor(Animal, const char *genus, const char *species);

static void Animal_dealloc(Object *self) {

	free(((Animal *) self)->genus);
	free(((Animal *) self)->species);

	__Object.dealloc(self);
}

static char *Animal_scientificName(Animal *self) {

	char *name = NULL;

	if (self->genus && self->species) {
		name = malloc(strlen(self->genus) + strlen(self->species) + 1);
		sprintf(name, "%s %s", self->genus, self->species);
	}

	return name;
}

Implementation(Animal, const char *genus, const char *species)

	Initialize(Animal, NULL, NULL);

	if (Object_init((Object *) self)) {

		if (genus) {
			self->genus = strdup(genus);
		}

		if (species) {
			self->species = strdup(species);
		}

		Override(Object, dealloc, Animal_dealloc);
		self->scientificName = Animal_scientificName;
	}

	return self;

End

START_TEST(animals)
	{
		Animal *lion = New(Animal, "Panthera", "leo");
		Animal *tiger = New(Animal, "Panthera", "tigris");
		Animal *bear = New(Animal, "Ursus", "arctos");

		ck_assert_str_eq("Panthera", lion->genus);
		ck_assert_str_eq("tigris", tiger->species);

		char *name = bear->scientificName(bear);
		ck_assert_str_eq("Ursus arctos", name);
		free(name);

		// No animals were actually harmed in the writing of this test

		Destroy(lion);
		Destroy(tiger);
		Destroy(bear);

	}END_TEST

int main(int argc, char **argv) {

	TCase *tcase = tcase_create("animals");
	tcase_add_test(tcase, animals);

	Suite *suite = suite_create("animals");
	suite_add_tcase(suite, tcase);

	SRunner *runner = srunner_create(suite);

	srunner_run_all(runner, CK_VERBOSE);
	int failed = srunner_ntests_failed(runner);

	srunner_free(runner);

	return failed;
}
