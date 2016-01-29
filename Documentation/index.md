Objectively Documentation {#index}
===
[Objectively](https://github.com/jdolan/Objectively) provides object oriented programming constructs for [GNU C](http://www.gnu.org/software/gnu-c-manual/), and implements a small but useful core library resembling Apple's [Foundation framework](https://developer.apple.com/library/mac/documentation/Cocoa/Reference/Foundation/ObjC_classic/index.html). It's primary goals are as follows:

 * Bring rich object oriented programming semantics to GNU C / C99
 * Provide convenient solutions to challenges commonly faced in modern application development
 * Embrace popular modern compilers (GCC and Clang), utilizing extensions thoughtfully

Key Features
---
 * Single-parent inheritance through _starts-with_ structure composition
 * Class and instance methods with strongly typed interfaces
 * Automatic class loading and lifecycle management
 * Automatic memory management with reference counting
 * Unicode and multibyte character set String support
 * Object primitives for Boole, Date, Null, Number, String
 * Mutable and immutable collections variants such as Array and MutableDictionary
 * JSON parsing, marshaling and introspection with JSONSerialization and JSONPath
 * Low-level concurrency constructs such as Lock, Condition, and Thread
 * High-level concurrency with Operation and OperationQueue
 * Resource loading via Internet protocols with URLSession and URLSessionTask

API
---
Browse the [Class hierarchy](hierarchy.html) to grok the API.

Examples
---
Examples of implementing types, invoking methods, etc.. are covered in the project's [README](https://github.com/jdolan/Objectively).

