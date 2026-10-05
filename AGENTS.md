# Objectively

Objectively is an ultra-lightweight object-oriented framework for GNU C, inspired by Foundation:
classes and interfaces, `$(obj, method, ...)` dispatch, reference counting, collections, threads,
JSON and URL sessions. It requires `gcc` or `clang`, because it uses GNU extensions (statement
expressions and `typeof`).

This file is the shared instruction set for coding agents. Read this first.

It deliberately records only what a careful reading of the code does **not** reveal: rules that fail
silently, constraints that live outside this repository, and the build and test procedure. For the
object model itself, read [`Documentation/guide.md`](Documentation/guide.md), which covers the
anatomy of a type, dispatch, overriding, re-classing, memory management and shared instances. For
anything else, read the code.

## Downstream

[ObjectivelyGPU](https://github.com/jdolan/ObjectivelyGPU), [ObjectivelyMVC](https://github.com/jdolan/ObjectivelyMVC)
and [Quetoo](https://github.com/jdolan/quetoo) build on Objectively. They are normally checked out
beside this repository, and they consume the **installed** copy from `/usr/local` through
`pkg-config` (package name `Objectively`), not the sibling source.

- Editing a header here changes nothing downstream until it is rebuilt and installed with
  `sudo make install`. A stale installed header produces confusing errors downstream.
- The library is versioned with `-release MAJOR.MINOR` (`Sources/Objectively/Makefile.am`), so the
  soname is `libObjectively-2.2.dylib`. A change to any instance or interface layout MUST bump the
  minor version in `configure.ac`, and ObjectivelyGPU and ObjectivelyMVC MUST then be rebuilt and
  require `>=` that version.

## Rules that fail silently

- **`#define _Class _Foo` MUST name the type in its own file.** `super(...)` resolves the
  superclass through `_Class`. A `_Class` copied from another file makes `super` call the wrong
  class's methods. The only check is a `cast` `assert`, which `NDEBUG` removes.
- **`.instanceSize` and `.interfaceSize` MUST be the type's own `sizeof`.** `_initialize` asserts
  only that they are not smaller than the superclass's, and only in debug builds (`Class.c`). A
  size copied from the parent overruns memory with no error.
- **The zero-length `interface[0]` member MUST directly follow the parent struct.** It has pointer
  alignment, so a smaller field before it adds padding.
- **The superclass interface is `memcpy`'d before `initialize` runs.** Register only overrides and
  new methods. An inherited method is already in place.
- **`Object::copy` copies the instance with `memcpy`.** A type that owns pointers or retained
  objects MUST override `copy`, or the copy shares them and both release them.
- **`MakeJSONProperty` derives the JSON key from the field name.** Renaming the field silently stops
  it from deserializing. Use `MakeJSONPropertyWithKey` where the C name and the JSON key differ.
- **`_classesLock` MUST NOT be held across `dlsym`, `dlopen` or a class's `initialize`.**
  `removeClassImage` unregisters a class image's Classes and never frees them.

## Conventions the code shows, written down

- An `init` method calls `self = (Foo *) super(Object, self, init);` (or the nearest superclass's
  initializer), then fills the instance inside `if (self) { ... }`, and returns `self`.
- `dealloc` releases what the instance owns, then ends with `super(Object, self, dealloc)`.
- `release` always returns `NULL`, so clear a field with `self->foo = release(self->foo)`.
- A `.c` file groups its methods under `#pragma mark - <Superclass>`, `#pragma mark - <Type>` and
  `#pragma mark - Class lifecycle`. Methods in an interface struct and in its `.c` file are in
  alphabetical order. A private helper sits directly above its first caller.
- Every function in a `.c` file has a Doxygen stub directly above it, so Doxygen can join it to the
  header documentation: `@see Superclass::method(Params)` for an override, and
  `@fn ReturnType Type::method(Params)` with `@memberof Type` for a new method.

## Building and testing

```sh
autoreconf -i
./configure
make -j$(sysctl -n hw.logicalcpu)
make check
sudo make install
```

- `configure` requires [check](https://libcheck.github.io/check/) and libcurl.
- Each test is its own binary under `Tests/Objectively`. Run one test from that directory, for
  example `cd Tests/Objectively && ./String`. The `JSON` test reads `Fixtures/test.json` relative
  to that directory.
- `RESTClient` is listed in `XFAIL_TESTS`, because it calls httpbin.org over the network.
- A new type SHOULD get a test file `Tests/Objectively/Foo.c`, added to `TESTS` in
  `Tests/Objectively/Makefile.am`.

### A new source file goes in four places

1. `Sources/Objectively/Makefile.am`, in both `pkginclude_HEADERS` and `libObjectively_la_SOURCES`.
2. `Objectively.xcodeproj/project.pbxproj`. The release workflow builds the Apple xcframeworks from
   the Xcode project, so a file missing here is missing from every Apple release.
3. `Objectively.vs15/Objectively.vcxproj` and `Objectively.vcxproj.filters`.
4. The umbrella header `Sources/Objectively.h`.

`.github/copilot/skills/new-type.md` and `add-method.md` hold the full checklists, including the
Xcode test target.

### CI

- `build.yml` builds macOS and Linux with autotools and runs `make check`, and builds Windows with
  MSBuild. It runs on pushes and pull requests to `main`, and on tags. Windows CI is the only check
  on the Visual Studio project. **Nothing in CI builds the Xcode project** before a release.
- `release.yml` runs on `v*.*.*` tags and builds the xcframeworks through `Objectively.xcworkspace`.
- `docs.yml` publishes the Doxygen documentation to GitHub Pages.
