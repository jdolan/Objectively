Objectively
============
Object oriented framework for C.

[View on GitHub](https://github.com/jdolan/Objectively) — Zlib license.

## About

[Objectively](https://github.com/jdolan/Objectively) is a cross-platform framework that provides rich object-oriented semantics to enable OOP in C.

## Features

- **Cross-platform** support for Android, iOS, macOS, Linux and Windows
- **Single-parent inheritance** with _starts-with_ struct composition
- **Class and instance methods** with strongly typed interfaces
- **Automatic class loading** and lifecycle management
- **Automatic memory management** with reference counting
- **Unicode strings** with multibyte character set support
- **Collections** for Objects and C types: Array, Dictionary, List, Set and more
- **JSON** parsing, marshaling, and JSONPath queries
- **Concurrency**: Lock, Condition, Thread, Operation, OperationQueue
- **Networking**: URLSession, JSONContext & RESTClient

## tl;dr

Objectively brings classes, single-parent inheritance, strongly typed methods and reference counting to plain C — no C++, and no code generation.

Declare a type once, then instantiate and use it like any other:

```c
Hello *hello = $(alloc(Hello), initWithGreeting, "Hello, World!");

$(hello, sayHello);

hello = release(hello);
```

`alloc` and `release` manage the lifecycle, `$` dispatches through the type's interface, and `super`, method overrides and shared instances all behave exactly as you'd expect from a "real" object oriented language.

## Getting Started

Consult the @subpage install for dependencies, building, and linking.

## User Guide

Consult the @subpage guide to declare, implement, and use your own types.

## Class Hierarchy

Browse the [Class Hierarchy](hierarchy.html) to navigate the full API.
