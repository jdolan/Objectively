---
name: new-type
description: Create a new Objectively class (header, implementation, archetype, Doxygen stubs) and register it in autotools, Xcode, Visual Studio, the umbrella header and the tests. Use add-method to extend an existing type.
---

# Create a new Objectively type

Use this skill to create a new class. To add methods to an existing type, use the `add-method`
skill. Read `AGENTS.md`, "Rules that fail silently", first: most of those rules apply to a new type.

## 1. Header: `Sources/Objectively/Foo.h`

```c
#pragma once

#include <Objectively/Bar.h>

typedef struct Foo Foo;
typedef struct FooInterface FooInterface;

/**
 * @brief One-line description.
 * @extends Bar
 */
struct Foo {

  /**
   * @brief The superclass.
   */
  Bar bar;

  /**
   * @brief The interface type.
   * @protected
   */
  FooInterface *interface[0];

  ...
};

/**
 * @brief The Foo interface.
 */
struct FooInterface {

  /**
   * @brief The superclass interface.
   */
  BarInterface barInterface;

  /**
   * @fn Foo *Foo::init(Foo *self)
   * @brief Initializes this Foo.
   * @param self The Foo.
   * @return The initialized Foo, or `NULL` on error.
   * @memberof Foo
   */
  Foo *(*init)(Foo *self);
};

/**
 * @fn Class *Foo::_Foo(void)
 * @brief The Foo archetype.
 * @return The Foo Class.
 * @memberof Foo
 */
OBJECTIVELY_EXPORT Class *_Foo(void);
```

The parent struct comes first, and `interface[0]` MUST follow it directly. Methods in the interface
are in alphabetical order.

## 2. Implementation: `Sources/Objectively/Foo.c`

```c
#include <assert.h>

#include "Foo.h"

#define _Class _Foo

#pragma mark - Object

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

  Foo *this = (Foo *) self;

  release(this->...);

  super(Object, self, dealloc);
}

#pragma mark - Foo

/**
 * @fn Foo *Foo::init(Foo *self)
 * @memberof Foo
 */
static Foo *init(Foo *self) {

  self = (Foo *) super(Bar, self, init);
  if (self) {
    ...
  }

  return self;
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

  ((ObjectInterface *) clazz->interface)->dealloc = dealloc;

  ((FooInterface *) clazz->interface)->init = init;
}

/**
 * @fn Class *Foo::_Foo(void)
 * @memberof Foo
 */
Class *_Foo(void) {
  static Class *clazz;
  static Once once;

  do_once(&once, {
    clazz = _initialize(&(const ClassDef) {
      .name = "Foo",
      .superclass = _Bar(),
      .instanceSize = sizeof(Foo),
      .interfaceSize = sizeof(FooInterface),
      .initialize = initialize,
    });
  });

  return clazz;
}

#undef _Class
```

- `_Class` MUST name this type. A `_Class` copied from another file makes `super` call that file's
  class.
- `.instanceSize` and `.interfaceSize` MUST be this type's own `sizeof`.
- If the type owns pointers or retained objects, override `Object::copy` too.

## 3. Build systems

A new source file goes in four places. Keep each list in alphabetical order.

1. `Sources/Objectively/Makefile.am`: `Foo.h` in `pkginclude_HEADERS`, and `Foo.c` in
   `libObjectively_la_SOURCES`.
2. `Sources/Objectively.h`: `#include <Objectively/Foo.h>`.
3. `Objectively.xcodeproj/project.pbxproj`. Object IDs are 24 uppercase hexadecimal characters.
   Use the entries for an existing type, such as `PointerArray`, as the model:

   | Section | Add |
   |---|---|
   | `PBXBuildFile` | `Foo.h in Headers`, with `settings = {ATTRIBUTES = (Public, ); }`, and `Foo.c in Sources` |
   | `PBXFileReference` | `Foo.h` and `Foo.c` |
   | `PBXGroup` (Sources/Objectively) | both file references |
   | `PBXHeadersBuildPhase` (the library target) | the build file for `Foo.h` |
   | `PBXSourcesBuildPhase` (the library target) | the build file for `Foo.c` |

   A header without the `Public` attribute is missing from the framework. Then run
   `plutil -lint Objectively.xcodeproj/project.pbxproj`.
4. `Objectively.vs15/Objectively.vcxproj` (`ClInclude` and `ClCompile`) and
   `Objectively.vcxproj.filters` (both, with `<Filter>Sources\Objectively</Filter>`).

Nothing in CI builds the Xcode project before a release, so check it locally:
`xcodebuild -workspace Objectively.xcworkspace -scheme Objectively build`.

## 4. Test

1. Create `Tests/Objectively/Foo.c`, with its own `main()`, using libcheck. Use an existing test,
   such as `Tests/Objectively/Array.c`, as the model.
2. Add `Foo` to `TESTS` in `Tests/Objectively/Makefile.am`.
3. Add `Foo` to `Tests/Objectively/.gitignore`.
4. In `project.pbxproj`, create the native target `Objectively-Foo` (use `Objectively-URLCache` as
   the model), make it a dependency of `Objectively-Tests`, and add `./Objectively-Foo &&` to that
   target's shell script phase, in alphabetical order.

## 5. Verify

```sh
autoreconf -i && ./configure
make -j$(sysctl -n hw.logicalcpu) && make check
```
