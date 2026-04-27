# Objectively — Copilot Instructions

Ultra-lightweight object-oriented framework for GNU C. Foundation-inspired. Requires `gcc` or `clang` (uses GNU extensions).

## Build & Test

```sh
# First-time setup
autoreconf -i && ./configure

# Build
make -j$(nproc)

# Run all tests
make check

# Run a single test (each test is a standalone binary)
cd Tests/Objectively && ./MutableString
```

Tests use [libcheck](https://libcheck.github.io/check/). Each test file has its own `main()`.

## Architecture

Every Objectively type consists of exactly three pieces:

1. **Instance struct** (`Foo.h`) — starts with the parent type, then the interface pointer, then instance variables:
   ```c
   struct Foo {
     Bar bar;               // parent (starts-with composition = single inheritance)
     FooInterface *interface;
     int myField;
   };
   ```

2. **Interface struct** (`Foo.h`) — starts with the parent interface, then method function pointers:
   ```c
   struct FooInterface {
     BarInterface barInterface;  // parent interface first
     Foo *(*initWithThing)(Foo *self, Thing *thing);
     void (*doSomething)(Foo *self);
   };
   ```

3. **Class archetype** (`Foo.h`) — a single exported function:
   ```c
   OBJECTIVELY_EXPORT Class *_Foo(void);
   ```

The `.c` file contains `static` implementations, an `initialize` function that wires them into the interface, and the thread-safe `_Foo()` archetype using `do_once`.

## Key Conventions

### Dispatch macros
```c
$(obj, method, args)          // instance method dispatch (obj->interface->method)
$$(Type, method, args)        // class/static method dispatch
super(Type, obj, method, args) // call superclass implementation
alloc(Type)                   // allocate an instance
release(obj)                  // decrement refcount; deallocates at 0
retain(obj)                   // increment refcount
```

### Implementing a type
- `#define _Class _TypeName` at the top of each `.c` file; `#undef _Class` at the bottom.
- All method implementations are `static` functions.
- Every method must be registered in `initialize`:
  ```c
  static void initialize(Class *clazz) {
    ((FooInterface *) clazz->interface)->doSomething = doSomething;
  }
  ```
- The superclass interface is `memcpy`'d into the subclass interface before `initialize` is called, so only overrides and new methods need to be registered — not inherited ones.

### `initWith*` pattern
- Call `$(self, init)` (or `super(...)` for the root init) first; bail out if it returns `NULL`.
- `Mutable*` types follow the pattern: `init` then `append*` — they do not duplicate parent transcoding/loading logic; instead they use a temporary parent-type object.

### `dealloc`
Always end with `super(Object, self, dealloc)`.

### Class archetype function
```c
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
```

### Ordering
Method declarations in interface structs and their implementations in `.c` files are kept in **alphabetical order** by method name.

### Memory management
- `alloc` gives refcount 1. Balance every `retain` with a `release`.
- `release` always returns `NULL` — assign it back when nulling a field: `self->foo = release(self->foo)`.

### Mutable / immutable pairs
Mutable types (`MutableArray`, `MutableString`, etc.) extend their immutable counterparts and add mutating methods. The immutable type's `copy` override returns a `Mutable*` instance.

### `_pageSize`
Used in `Mutable*` types for capacity growth calculations (round up to page boundary).

### `ident`
`typedef void *ident` — the generic object pointer type, analogous to Objective-C `id`.
