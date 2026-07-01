Installing Objectively {#install}
======================

Dependencies, building, and linking against Objectively.

[TOC]

## Releases

Tagged releases are published on the [GitHub releases page](https://github.com/jdolan/Objectively/releases). To build the latest from source, follow the steps below.

## Dependencies

* [libcurl](https://curl.se/libcurl/) >= 7.16.0 — for `URLSession`, `JSONContext` and `RESTClient`
* [check](https://libcheck.github.io/check/) >= 0.9.4 — to build and run the test suite (optional)

## Building

Do the Autotools dance:

```sh
autoreconf -i
./configure
make && sudo make install
```

## Linking

Compile and link against Objectively with `pkg-config`:

```sh
gcc `pkg-config --cflags --libs Objectively` -o myprogram *.c
```
