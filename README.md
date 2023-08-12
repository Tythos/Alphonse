# ArtemisAlphonse

"Alphonse" evolution of the Artemis engine library. See the following dev.to
article for more details on context, architecture, etc.:

  https://dev.to/tythos/engines-evolution-10gk

"Alphonse" is a super-basic architecture pattern in which all states and
behaviors exist within the global scope as program-level variables and
functions. This includes the main loop, rendering context, GL configuration,
program/shader variables, vertex buffers... It's all global.

.. image:: doc/engineAlphonse.jpg

## Cloning

Upon cloning this repository, you will want to also initialize submodules:

```
> git submodule update --init --recursive
```

This will instantiate the `glew` and `SDL` dependencies.

## Building

The standard `cmake` sequence of commands build configure and build the project:

```
> cmake -S . -B build
> cmake --build build
```

## Running

Once built, you will need to copy several runtime dependencies into the executable folder. We assume here that you are on Windows building with MSVC. Specifically, the `.GLSL` shader source and the `.DLL` library are needed:

```
> copy *.glsl build\Debug
> copy build\SDL\Debug\SDL3.dll build\Debug
```

You should then be able run the program directly:

```
> build\Debug\ArtemisAlphonse.exe
```
