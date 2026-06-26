Objectively {#index}
============

[Objectively](https://github.com/jdolan/Objectively) is an ultra-lightweight object oriented framework for [GNU C](https://www.gnu.org/software/gnu-c-manual/). It brings OOP semantics — inheritance, polymorphism, and automatic reference counting — to C without requiring C++ or Objective-C.

## Features

- **Single-parent inheritance** via _starts-with_ struct composition
- **Class and instance methods** with strongly typed interfaces
- **Automatic reference counting** memory management
- **Unicode strings** with multibyte character set support
- **Collections**: Array, Dictionary, Set, Vector, and more
- **JSON** parsing, marshaling, and JSONPath queries
- **Concurrency**: Lock, Condition, Thread, Operation, OperationQueue
- **Networking**: URLSession and URLSessionTask for HTTP/HTTPS
- **Resource loading** with pluggable ResourceProvider
- **Windows, macOS, Linux** — cross-platform

## API

Browse the [class hierarchy](hierarchy.html) to navigate the API.

## Building

```sh
autoreconf -i
./configure
make && sudo make install
```
