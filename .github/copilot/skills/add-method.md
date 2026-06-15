# Skill: Add Method to Objectively Type (add-method)

Use this skill when adding one or more new methods to an *existing* Objectively type.  
For creating a brand-new type from scratch, use the `new-type` skill instead.

---

## Checklist

### 1. Header — `Sources/Objectively/Foo.h`

Add the method pointer to `FooInterface` in **alphabetical order**:

```c
/**
 * @fn ReturnType Foo::myMethod(Foo *self, ArgType arg)
 * @brief One-line description.
 * @param self The Foo.
 * @param arg Description.
 * @return Description.
 * @memberof Foo
 */
ReturnType (*myMethod)(Foo *self, ArgType arg);
```

### 2. Implementation — `Sources/Objectively/Foo.c`

Add the `static` implementation in **alphabetical order** among the other methods,
with the appropriate Doxygen stub immediately above:

```c
/**
 * @fn ReturnType Foo::myMethod(Foo *, ArgType)   // new interface method
 * @memberof Foo
 */
static ReturnType myMethod(Foo *self, ArgType arg) {
  // ...
}
```

For a **superclass override** the stub format is different:

```c
/**
 * @see SuperClass::overriddenMethod(SuperClass *)
 */
static ReturnType overriddenMethod(SuperClass *self) {
  // ...
  super(SuperClass, self, overriddenMethod);
}
```

### 3. Register in `initialize`

Wire the new function pointer in `initialize`, in **alphabetical order**:

```c
static void initialize(Class *clazz) {
  FooInterface *foo = (FooInterface *) clazz->interface;
  // ...
  foo->myMethod = myMethod;   // ← add this
  // ...
}
```

---

## Notes

- Method declarations in the interface struct and their `initialize` registrations must stay in **alphabetical order** — this is an enforced Objectively convention.
- `static` functions are the only implementation; there are no non-static method bodies.
- Always rebuild and run `make check` after changes.
