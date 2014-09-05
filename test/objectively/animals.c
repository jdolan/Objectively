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
#include <stdlib.h>
#include <check.h>

#include <objectively.h>

struct Animal {
	struct Object _;

	char *genus;
	char *species;

	char *(*scientificName)(const id self);
};

extern Class *Animal;

static void dealloc(id self) {

	struct Animal *this = cast(Animal, self);

	free(this->genus);
	free(this->species);

	super(Object, self, dealloc);
}

static char *scientificName(const id self) {

	char *name = NULL;

	const struct Animal *this = cast(Animal, self);

	if (this->genus && this->species) {
		name = malloc(strlen(this->genus) + strlen(this->species) + 1);
		sprintf(name, "%s %s", this->genus, this->species);
	}

	return name;
}

static id init(id self, va_list *args) {

	if (super(Object, self, init, args)) {

		override(Object, self, class, Animal);
		override(Object, self, dealloc, dealloc);

		const char *genus = va_arg(*args, const char *);
		const char *species = va_arg(*args, const char *);

		struct Animal *this = cast(Animal, self);

		if (genus) {
			this->genus = strdup(genus);
		}

		if (species) {
			this->species = strdup(species);
		}

		this->scientificName = scientificName;
	}

	return self;
}

static struct Animal archetype;

static Class class = {
	.name = "Animal",
	.size = sizeof(struct Animal),
	.archetype = &archetype,
	.init = init,
};

Class *Animal = &class;

int main(int argc, char **argv)
//START_TEST(animals)
	{
		struct Animal *lion = new(Animal, "Panthera", "leo");
		struct Animal *tiger = new(Animal, "Panthera", "tigris");
		struct Animal *bear = new(Animal, "Ursus", "arctos");

		ck_assert_str_eq("Panthera", lion->genus);
		ck_assert_str_eq("tigris", tiger->species);

		char *name = bear->scientificName(bear);
		ck_assert_str_eq("Ursus arctos", name);
		free(name);

		// No animals were actually harmed in the writing of this test

		delete(lion);
		delete(tiger);
		delete(bear);

	}//END_TEST

/*int main(int argc, char **argv) {

	TCase *tcase = tcase_create("animals");
	tcase_add_test(tcase, animals);

	Suite *suite = suite_create("animals");
	suite_add_tcase(suite, tcase);

	SRunner *runner = srunner_create(suite);

	srunner_run_all(runner, CK_VERBOSE);
	int failed = srunner_ntests_failed(runner);

	srunner_free(runner);

	return failed;
}*/
