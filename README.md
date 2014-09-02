Objectively
===
Ultra-lightweight object oriented framework for the c programming language. Zlib license.

Adding Objectively to your project
---
Copy `objectively.c` and `objectively.h` to your project and include them in your build target.

Declaring a type
---
```c
/* animal.h */

#include "objectively.h"

Interface(Animal, Object)
	char *genus;
	char *species;

	char *(*scientificName)(Animal *self);
End

Constructor(Animal, const char *genus, const char *species);
```

Implementing a type:
---
```c
/* animal.c */

#include "animal.h"

static void Animal_dealloc(Animal *self) {

	free(self->genus);
	free(self->species);

	Super(Object, self, dealloc)
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

		Override(Object, self, dealloc, Animal_dealloc);
		self->scientificName = Animal_scientificName;
	}

	return self;
End
```

Using a type:
---
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
For each declared type, an _archtype_ exists. The archetype is a statically allocated instance of the type that serves to hold the default method implementations. The archetype is defined and intialized by the `Implementation` and `Initialize` macros, respectively. The archetype for _FooBar_ is the symbol `__FooBar`.

Overriding a method
---
To override a method, simply overwrite the function pointer in your constructor or use the `Override` macro (*preferred*).

```c
	self->dealloc = (void (*)(Object *)) Foo_dealloc;

	/* or */

	Override(Object, self, dealloc, Foo_dealloc);
```

Calling super
---
To invoke a supertype's method implementation, either directly invoke the desired archetype implementation or use the `Super` macro (*strongly preferred*).

```c
	__Object.dealloc((Object *) self);
	
	/* or */

	Super(Object, self, dealloc);
```

FAQ
---
1. *Why?* Because c++ makes me puke, and Objective-c is not widely supported. 
1. *This looks like poor-man's Objective-c?* Yup.
