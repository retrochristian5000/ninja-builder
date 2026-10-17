# Ninja

Ninja is a small build system with a focus on speed.
https://ninja-build.org/

See [the manual](https://ninja-build.org/manual.html) or
`doc/manual.asciidoc` included in the distribution for background
and more details.

Binaries for Linux, Mac and Windows are available on
  [GitHub](https://github.com/ninja-build/ninja/releases).
Run `./ninja -h` for Ninja help.

Installation is not necessary because the only required file is the
resulting ninja binary. However, to enable features like Bash
completion and Emacs and Vim editing modes, some files in misc/ must be
copied to appropriate locations.

If you're interested in making changes to Ninja, read
[CONTRIBUTING.md](CONTRIBUTING.md) first.

## Building Ninja itself

You can either build Ninja via the custom generator script written in Python or
via CMake. For more details see
[the wiki](https://github.com/ninja-build/ninja/wiki).

### Python

```
./configure.py --bootstrap
```

This will generate the `ninja` binary and a `build.ninja` file you can now use
to build Ninja with itself.

If you have a GoogleTest source directory, you can build the tests
by passing its path with `--gtest-source-dir=PATH` option, or the
`GTEST_SOURCE_DIR` environment variable, e.g.:

```sh
./configure.py --bootstrap --gtest-source-dir=/path/to/googletest
./ninja all     # build ninja_test and other auxiliary binaries
./ninja_test    # run the unit-test suite.
```

Use the CMake build below if you want to use a preinstalled binary
version of the library.

### CMake

To build the ninja binary without building the unit tests, disable test building
by setting `BUILD_TESTING` to `OFF`:

```
cmake -Bbuild-cmake -DBUILD_TESTING=OFF
cmake --build build-cmake
```

The `ninja` binary will now be inside the `build-cmake` directory (you can
choose any other name you like).

To run the unit tests, omit the `-DBUILD_TESTING=OFF` option, and after
building, run:

```
build-cmake/ninja_test
```

## WHP incremental-build scheduling

The WHP fork supports an opt-in scheduler policy for repeated builds:

```sh
NINJA_CRITICAL_PATH_TIMES=1 ninja -C build
```

When an existing `.ninja_log` contains durations, Ninja weights the
critical-path queue by the previous elapsed time of each command instead of
treating all non-phony commands as equal. Missing or zero durations use a
one-unit fallback; each recorded duration is capped at one hour to avoid
unreasonable priority from bad history. Phony edges remain zero-cost.
This changes *scheduling only*: dependencies, command arguments, job counts,
jobserver tokens, and output formats are unchanged.

The default (environment variable unset or `0`) retains the ordinary
edge-count critical-path heuristic. This policy has no effect on a clean build
without historical timings, and it is not a guaranteed wall-clock improvement.

## Generating documentation

### Ninja Manual

You must have `asciidoc` and `xsltproc` in your PATH, then do:

```
./configure.py
ninja manual doc/manual.html
```

Which will generate `doc/manual.html`.

To generate the PDF version of the manual, you must have `dblatext` in your PATH
then do:

```sh
./configure.py    # only if you didn't do it previously.
ninja doc/manual.pdf
```

Which will generate `doc/manual.pdf`.

### Doxygen documentation

If you have `doxygen` installed, you can build documentation extracted from C++
declarations and comments to help you navigate the code. Note that Ninja is a
standalone executable, not a library, so there is no public API, all details
exposed here are internal.

```sh
./configure.py   # if needed
ninja doxygen
```

Then open `doc/doxygen/html/index.html` in a browser to look at it.
