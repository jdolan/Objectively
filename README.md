[![Build](https://github.com/jdolan/Objectively/actions/workflows/build.yml/badge.svg)](https://github.com/jdolan/Objectively/actions/workflows/build.yml)
[![Zlib License](https://img.shields.io/badge/license-Zlib-brightgreen.svg)](https://opensource.org/licenses/Zlib)
![Stable](https://img.shields.io/badge/maturity-stable-green.svg)

# Objectively
Object oriented framework for C. 

Zlib [license](./COPYING).

## About
Objectively is a cross-platform framework that provides rich object-oriented semantics to enable OOP in C.

## Features
 * **Cross-platform** support for Android, iOS, macOS, Linux and Windows
 * **Single-parent inheritance** with _starts-with_ struct composition
 * **Class and instance methods** with strongly typed interfaces
 * **Automatic class loading** and lifecycle management
 * **Automatic memory management** with reference counting
 * **Unicode strings** with mutlibyte character support
 * **Collections** for Objects and C types: Array, Dictionary, List, Set and more
 * **Concurrency**: Lock, Condition, Thread, Operation, OperationQueue
 * **Networking**: URLSession, JSONContext & RESTClient

## User Guide

Consult the [User Guide](./GUIDE.md) to install Objectively and write your first Class.

## API Documentation

Browse the [API Documentation](https://jdolan.github.io/Objectively/) to explore the library.

## Examples & projects using Objectively

1. [Objectively test suite](Tests/Objectively) provides examples for using every Objectively C type.
1. [ObjectivelyGPU](https://github.com/jdolan/ObjectivelyGPU) is a framework for modern graphics built on SDL3 and Objectively.
1. [ObjectivelyMVC](https://github.com/jdolan/ObjectivelyMVC) is a framework for modern game interfaces built on SDL3, Objectively and ObjectivelyGPU.
1. [Quetoo](https://github.com/jdolan/quetoo) is a free first-person shooter that uses Objectively, ObjectivelyGPU and ObjectivelyMVC extensively.

<p align="center">
  <img src="https://raw.githubusercontent.com/jdolan/ObjectivelyGPU/main/Documentation/Hello.gif" width="48%" alt="ObjectivelyGPU Hello — a spinning, multisampled 3D cube">
  <img src="https://raw.githubusercontent.com/jdolan/ObjectivelyGPU/main/Documentation/HelloCompute.gif" width="48%" alt="ObjectivelyGPU HelloCompute — particles animated by a compute shader">
  <img src="https://github.com/jdolan/ObjectivelyMVC/raw/jdolan-sdl-gpu-renderer/Documentation/demo.gif" width="96%" alt="ObjectivelyMVC Hello - a simple in-game menu">
</p>
