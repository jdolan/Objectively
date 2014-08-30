Objectively
===
Ultra-lightweight object oriented framework for the c programming language.

Overview
---
Objectively brings syntactic sugar and basic OO concepts to c. It requires c99 and the GNU c compiler (gcc).

Adding Objectively to your project
---
Copy `objectively.c` and `objectively.h` to your project, and include them in your build target.

Using Objectively
---
Declaring a type:
```c
/* animal.h */

#include <objectively.h>

Interface(Animal, Object)
	char *genus;
	char *species;

	char *(*scientificName)(Animal *self);
End

Constructor(Animal, const char *genus, const char *species);
```

Implementing a type:
```c
/* animal.c */

#include "animal.h"

static void Animal_dealloc(Object *self) {

	free(((Animal *) self)->genus);
	free(((Animal *) self)->species);

	__Object.dealloc(self); // calling "super"
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

		self->super.dealloc = Animal_dealloc; // overriding a method
		self->scientificName = Animal_scientificName;
	}

	return self;
End
```

Using a type:
```c
	Animal *lion = New(Animal, "Panthera", "leo");

	printf("%s\n", lion->genus);
	printf("%s\n", lion->species);
	
	char *name = lion->scientificName(lion);
	printf("%s\n", name);
	free(name);

	Destroy(lion);
```

Archetypes
---
For each declared type, an _archtype_ exists. The archetype is a statically allocated instance of the type that serves to hold the default method implementations.

The archetype is defined and intialized by the `Implementation` and `Initialize` macros, respectively. The archetype for _FooBar_ is the symbol `__FooBar`.

Overriding a method
---
To override a method, simply overwrite the function pointer in `self->super`. See the overriding of `Object_dealloc` above.

Calling super
---
To invoke a supertype's method implementation, use the supertype's archetype. See the invocation of `Object_dealloc` above.

FAQ
---
1. *Why?* Because c++ makes me puke, and Objective-c is not widely supported. 
1. *This looks like poor-man's Objective-c?* Yup.
