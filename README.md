# libdsp
Audio dsp library written in c++ with no external dependencies.


## Building

The library use CMake and can probably be built in multiple of ways, but here's how I build it:

```
> cmake -GNinja -Bbuild
> cmake --build ./build
```

## Including libdsp

When including the library in another project you might want to disable certain features with these CMAKE option flags:

`LIBDSP_LIB_ONLY`: Set to 'ON' to only build the static library. This will avoid building all the external dependencies used by the tools. 'OFF' by default.

`LIBDSP_BUILD_TESTS`: Set to 'OFF' to forgo building the /tests directory. 'ON' by default.


![ci status](https://github.com/Segfault1602/libdsp/actions/workflows/ci.yml/badge.svg)
