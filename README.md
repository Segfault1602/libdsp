# libdsp
Audio dsp library written in c++ with no external dependencies.


## Building

The library use CMake and can probably be built in multiple of ways, but here's how I build it:

```
> cmake -GNinja -Bbuild
> cmake --build ./build
```

I also included a toolchain file for cross compiling libDSP for the Daisy platform. This assumes that you
already have the [Daisy Toolchain installed](https://github.com/electro-smith/DaisyWiki/wiki/1.-Setting-Up-Your-Development-Environment).

```
> cmake -GNinja -Bdaisy -DCMAKE_TOOLCHAIN_FILE=cmake/daisy.cmake
> cmake --build ./daisy
```

## Including libdsp

When including the library in another project you might want to disable certain features with these CMAKE option flags:

`LIBDSP_LIB_ONLY`: Set to 'ON' to only build the static library. This will avoid building all the external dependencies used by the tools. 'OFF' by default.

`LIBDSP_BUILD_TESTS`: Set to 'OFF' to forgo building the /tests directory. 'ON' by default.


![ci status](https://github.com/Segfault1602/libdsp/actions/workflows/ci.yml/badge.svg)