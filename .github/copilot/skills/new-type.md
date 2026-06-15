# Skill: New Objectively Type (new-type)

Use this skill when creating a brand-new class/type in Objectively.  
For adding methods to an *existing* type, use the `add-method` skill instead.

---

## Checklist

### 1. Header — `Sources/Objectively/Foo.h`

```c
#pragma once

typedef struct Foo Foo;
typedef struct FooInterface FooInterface;

#include <Objectively/Bar.h>   // parent type and any dependencies

struct Foo {
  Bar bar;                     // parent first (starts-with inheritance)
  FooInterface *interface;
  // instance variables...
};

struct FooInterface {
  BarInterface barInterface;   // parent interface first
  // method pointers in alphabetical order...
  Foo *(*init)(Foo *self);
};

OBJECTIVELY_EXPORT Class *_Foo(void);
```

### 2. Implementation — `Sources/Objectively/Foo.c`

```c
#include "Foo.h"
// other includes...

#define _Class _Foo

#pragma mark - Object   // (or whatever superclass section applies)

/**
 * @see Object::dealloc(Object *)   // for overrides
 */
static void dealloc(Object *self) {
  // release owned pointers
  super(Object, self, dealloc);
}

#pragma mark - Foo

/**
 * @fn Foo *Foo::init(Foo *)        // for interface methods
 * @memberof Foo
 */
static Foo *init(Foo *self) {
  self = (Foo *) super(Object, self, init);
  if (self) {
    // ...
  }
  return self;
}

#pragma mark - Class lifecycle

static void initialize(Class *clazz) {
  ((ObjectInterface *) clazz->interface)->dealloc = dealloc;
  FooInterface *foo = (FooInterface *) clazz->interface;
  foo->init = init;
  // all methods in alphabetical order
}

Class *_Foo(void) {
  static Class *clazz;
  static Once once;

  do_once(&once, {
    clazz = _initialize(&(const ClassDef) {
      .name = "Foo",
      .superclass = _Bar(),
      .instanceSize = sizeof(Foo),
      .interfaceOffset = offsetof(Foo, interface),
      .interfaceSize = sizeof(FooInterface),
      .initialize = initialize,
    });
  });

  return clazz;
}

#undef _Class
```

**Doxygen stub rules** (applied to every function body):
- Superclass override → `@see SuperClass::method(Params)`
- New interface method → `@fn ReturnType Class::method(Params)` + `@memberof Class`

### 3. `Sources/Objectively/Makefile.am`

Add `Foo.h` to `pkginclude_HEADERS` and `Foo.c` to `libObjectively_la_SOURCES`, in alphabetical order.

### 4. `Sources/Objectively.h` — umbrella header

Add `#include <Objectively/Foo.h>` in alphabetical order.

### 5. Xcode — `Objectively.xcodeproj/project.pbxproj`

UUIDs must be exactly **24 uppercase hex characters** (e.g. `A1B2C3D4E5F6A7B8C9D0E1F2`).  
Needed insertions (use nearby entries as positional reference):

| Section | What to add |
|---|---|
| `PBXBuildFile` | `Foo.h in Headers`, `Foo.c in Sources` |
| `PBXFileReference` | `Foo.h`, `Foo.c` |
| `PBXGroup` (Sources/Objectively) | file refs for both |
| `PBXHeadersBuildPhase` (library) | build file for `Foo.h` |
| `PBXSourcesBuildPhase` (library) | build file for `Foo.c` |

Always validate after editing:

```sh
plutil -lint Objectively.xcodeproj/project.pbxproj
```

### 6. MSVS — `Objectively.vs15/Objectively.vcxproj`

Add in alphabetical order:
```xml
<ClInclude Include="..\Sources\Objectively\Foo.h" />
<ClCompile Include="..\Sources\Objectively\Foo.c" />
```

### 7. MSVS — `Objectively.vs15/Objectively.vcxproj.filters`

Add both entries with `<Filter>Sources\Objectively</Filter>`.

---

## If adding a test for the new type

- Create `Tests/Objectively/Foo.c` (with its own `main()`; uses libcheck).
- Add `Foo` to `TESTS` and `check_PROGRAMS` in `Tests/Objectively/Makefile.am`.
- Add `Foo` to `Tests/Objectively/.gitignore`.
- Add `./Objectively-Foo &&` to the `ShellScript` build phase in `project.pbxproj` (in alphabetical order).
- Add `Objectively-Foo` as a target dependency of `Objectively-Tests` in `project.pbxproj`.
- Create the `Objectively-Foo` native target in `project.pbxproj` (model after an existing test target like `Objectively-URLCache`).
