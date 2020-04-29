Non-normative Test FMUs for FMI 3.0
===================================

[![C/C++ CMake CI](https://github.com/PMSFIT/FMI30TestFMUs/workflows/C/C++%20CMake%20CI/badge.svg)](https://github.com/PMSFIT/FMI30TestFMUs/actions?query=workflow%3A%22C%2FC%2B%2B+CMake+CI%22)

This repository contains non-normative Test FMUs implementing draft
versions of the upcoming FMI 3.0 standard. The FMUs are manually
coded in order to excercise various parts of the specification and
implementations thereof in order to aid in finalizing the specification
and to aid in implementors implementing and testing their own
implementations against a wider variety of test FMUs.

It should be noted that these FMUs are non-normative, and that it is
not unlikely that they will at various points in time contain bugs and
errors vis-a-vis the current draft of the specification. They should
be viewed as a starting point for discussions on FMI 3.0, not as any
sort of validation suite.

The FMUs are made available under the MPL 2.0, see LICENSE.txt. Any
feedback or contributions under this license is highly welcome.

Build Instructions
------------------

```bash
$ git clone https://github.com/PMSFIT/FMI30TestFMUs.git
$ cd FMI30TestFMUs
$ git submodule update --init
$ mkdir -p build
$ cd build
$ cmake ..
$ make
```
