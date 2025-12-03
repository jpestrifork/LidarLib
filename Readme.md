# Project for Seyond LiDAR

This project uses the inno-lidar-sdk to allow easy access to the Falcon K2C.

It uses a modified repository in order to handle the library using 
[Conan](https://conan.io) which is used to handle all dependencies

The Idea behind this library is to allow easy usage in Cuda,
however so far no cuda code has been added to the library.

## Building
An attempt has been made to ease the building and usage of this library,
however there are still some steps:

### Prerequisites
The following tools are needed in order to build the project:

#### A C++20 capable compiler
The compiler used should be able to handle C++20 code, however the capabilities
of GCC version 13 is enough.

#### Ninja
The Ninja build system is used in order to speed up (re)compilation, if it is
not installed, make sure to change the `CMakePresets.json` accordingly

#### CMake
As both this project and the inno-lidar-sdk uses CMake, this is a prerequisite
The features used in this project requires the minimum version to be 3.28

#### Conan
Conan is used to manage dependencies in the project.
Conan version 2 is what has been used, but the later versions of version 1 
might still be usable.

Because of the way nanobind (that is used to create bindings to python) is done
it is unfortunately not possible to use cmake-conan.

In order to start the project, `conan install` must first be run from the base 
directory as follows:

```shell
conan install -s build_type=Debug -of cmake-build-debug/ -c="tools.cmake.cmaketoolchain:user_presets=" . 
```
of course, you should replace with Release if you are just going to use the 
python bindings.

```shell
conan install -s build_type=Release -of cmake-build-release/ -c="tools.cmake.cmaketoolchain:user_presets=" . 
```

This will set up the project so that cmake can be run.

Some of the prerequisites may be missing for the other packages as well,
in that case add `--build=missing` in order to rebuild the packages.

If the `inno_client_sdk` package is missing, do not run with `--build=missing`
but make sure you have installed the package using the correct settings.

#### CMake
As both this project and the inno-lidar-sdk uses CMake, this is a prerequisite
The features used in this project requires the minimum version to be 3.28

In order to configure the system for building, create the cmake configuration:

```shell
cmake --preset debug .
```

### Building the Application
The application available in `src/main.cpp` demonstrates how to use the library

Assuming cmake has already been set up, you can now use the prepared directory
and build it using

```shell
cmake --build cmake-build-debug -t cuLiDAR
```
### Building the python library
Likewise, assuming the cmake project has been created correctly you can build
the python library using
```shell
cmake --build cmake-build-debug -t inno
```

this will create the two files `inno.cpython-312-x86_64-linux-gnu.so` and
`libprocessor.so`, that will need to be put in the correct place for python to
find them.

## Usage

### Using with a Dev Container (Recommended)

For an easier setup with all dependencies containerized, you can use a dev container:

1. Build the Python wheel file from the project
2. Copy the generated `.whl` file to your dev container
3. Install the wheel inside the container:

```shell
pip install /path/to/inno-*.whl
```

This approach ensures all dependencies are properly isolated and avoids potential conflicts with your system's existing packages.
