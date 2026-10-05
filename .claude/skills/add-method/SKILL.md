---
name: add-method
description: Add one or more methods to an existing Objectively type (header, implementation, initialize, Doxygen stubs, test, version bump). Use new-type for a type that does not exist yet.
---

# Add a method to an Objectively type

Use this skill to add one or more methods to an **existing** type. For a new type, use the
`new-type` skill.

## 1. Header: `Sources/Objectively/Foo.h`

Add the function pointer to `FooInterface`, in alphabetical order:

```c
  /**
   * @fn ReturnType Foo::myMethod(const Foo *self, ArgType arg)
   * @brief One-line description.
   * @param self The Foo.
   * @param arg Description.
   * @return Description.
   * @memberof Foo
   */
  ReturnType (*myMethod)(const Foo *self, ArgType arg);
```

## 2. Implementation: `Sources/Objectively/Foo.c`

Add the `static` implementation under `#pragma mark - Foo`, in alphabetical order, with its Doxygen
stub directly above it. The stub repeats the full signature, parameter names included:

```c
/**
 * @fn ReturnType Foo::myMethod(const Foo *self, ArgType arg)
 * @memberof Foo
 */
static ReturnType myMethod(const Foo *self, ArgType arg) {
  ...
}
```

An override of a superclass method goes under that superclass's `#pragma mark`, and its stub is
`@see`:

```c
/**
 * @see Object::description(const Object *)
 */
static String *description(const Object *self) {
  ...
}
```

## 3. Register it in `initialize`

Assign it in `initialize`, in alphabetical order, with one cast on each line. Overrides come first,
grouped by superclass:

```c
  ((FooInterface *) clazz->interface)->myMethod = myMethod;
```

An inherited method that is not overridden needs no line: the superclass interface is copied before
`initialize` runs.

## 4. Test

Add a `START_TEST` case for the method to `Tests/Objectively/Foo.c`, and add it to that file's
suite. If the type has no test file, the `new-type` skill describes how to add one.

## 5. Version

A new method makes `FooInterface` larger, and every subclass interface after it moves. That is an
ABI change. Bump the minor version in `AC_INIT` in `configure.ac`, which also sets the soname, and
rebuild ObjectivelyGPU and ObjectivelyMVC against it. See `AGENTS.md`, "Downstream".

## 6. Verify

```sh
make -j$(sysctl -n hw.logicalcpu) && make check
```
