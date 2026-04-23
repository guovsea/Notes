---
tags:
  - tools
  - tools/cmake
mastery: 5
---
## Hello CMake

 ### Minimum CMake version

### Projects

`project(名称)`

### Creating an Executable

`add_executable(名称 源文件列表)`

可以用 `${PROJECT_NAME}` 以项目名称作为可执行文件名

### Binary Directory

The root or top level folder that you run the cmake command from is known as your `CMAKE_BINARY_DIR` and is the root folder for all your binary files. `CMake` supports building and generating your binary files both **in-place** and also **out-of-source**.


## Hello Header

### Directory Paths

| Variable                     | Info                                                                                      |
| ---------------------------- | ----------------------------------------------------------------------------------------- |
| **CMAKE_SOURCE_DIR**         | The root source directory                                                                 |
| **CMAKE_CURRENT_SOURCE_DIR** | The source directory being processed.                                                     |
| **PROJECT_SOURCE_DIR**       | The source directory of the current project(current sub-project).                         |
| *CMAKE_BINARY_DIR*           | The root binary / build directory. This is the directory where you ran the cmake command. |
| * *                          | The build directory being processed.                                                      |
| **PROJECT_BINARY_DIR**       | The build directory for the current project(current sub-project).                         |
|                              |                                                                                           |

[Are CMAKE\_SOURCE\_DIR and PROJECT\_SOURCE\_DIR the same in CMake? - Stack Overflow](https://stackoverflow.com/questions/32028667/are-cmake-source-dir-and-project-source-dir-the-same-in-cmake)

[Useful-Variables]( https://gitlab.kitware.com/cmake/community/-/wikis/doc/cmake/Useful-Variables " https://gitlab.kitware.com/cmake/community/-/wikis/doc/cmake/Useful-Variables" )

### file function

`file(GLOB SOURCES "src/*.cpp")`

> For modern CMake it is NOT recommended to use a variable for sources. Instead it is typical to directly declare the sources in the add_xxx function.
>
> This is particularly important for glob commands which may not always show you the correct results if you add a new source file.

### Including Directories

When you have different include folders, you can make your compiler aware of them using the `target_include_directories()`

When compiling this target this will add these directories to the compiler with the `-I/directory/path`

```CMake
target_include_directories(target
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)
```

### make Verbose Output

```shell
make VERBOSE=1
```

## Static Library

### Adding a Static Library

```CMake
add_library(hello_library STATIC
    src/Hello.cpp
)
```

> This will be used to create a static library with the name `libhello_library.a

### Populating Including Directories


```CMake
target_include_directories(hello_library
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)
```

This will cause the included directory used in the following places:

- When compiling the library
- When compiling any additional target that links the library.

specify scope

- PRIVATE - the directory is added to this target's include directories
- INTERFACE - the directory is added to the include directories for any targets that link this library.(但和 PRIVATE 互斥，目录不会被 include 到本目标中)
- PUBLIC - As above, it is included in this library and also any targets that link this library.

> 类比 C++：定义的是成员的可见性

> target_include_directories  也是一种属性

针对不同的头文件使用合理的关键字：

- 非导出头文件所在文件夹（只包含不会被其他编译单元 include 的头文件） -> PRIVATE
- 仅标头库 -> INTERFACE

  ```CMake
  # 制作仅标头库
  add_library(header_only INTERFACE)
  target_include_directories(header_only INTERFACE str)
    ```

- 需要导出的头文件所在文件夹 -> PUBLIC

推荐这种目录组织结构

```txt
- include
  - static
  - public
```

在源文件中从 `include` 的根目录开始指定文件名： `#include "static/Hello.h"`，可以支持多个库有同名文件。

### Linking a Library

```CMake
add_executable(hello_binary
    src/main.cpp
)

target_link_libraries(hello_binary
    PRIVATE
        hello_library
)
```

This tells CMake to link the hello_library against the hello_binary executable during link time. It will also propagate any include directories with PUBLIC or INTERFACE scope from the linked library target.

`target_link_libraries(A XXX B)`:

- PRIVATE 不传递也不使用 B 的属性
- INTERFACE A 不使用 B 的属性，将 B 的属性传递给其他链接 A 的对象
- PUBLIC A 使用 B 的属性且将 B 的属性传递给其他链接 A 的对象

> 一般用 PRIVATE


如何针对不同的 Dependencies 类型使用正确的关键字：

- 如果只有 A 的源文件 include B -> 使用 PRIVATE
- 如果 A 的头文件 include B ，源文件没有 include B -> 使用 INTERFACE
- 如果 A 的头文件和源文件都 include B -> 使用 PUBLIC


## Shared Library

### Alias Target

As the name suggests an alias target is an alternative name for a target that can be used instead of the real target name in **read-only contexts**.

```CMake
add_library(hello::library ALIAS hello_library)
```

## Installing

### install

用于安装二进制、库、头文件，使用方法 `make install`

`CMAKE_INSTALL_PREFIX` 定义了安装时的路径**前缀**，也可以用 `cmake .. -DCMAKE_INSTALL_PREFIX=/install/location` 自定义安装的路径

> 在我的电脑上，`CMAKE_INSTALL_PREFIX` 为 `usr/local`

使用方法：

```CMake
# 安装二进制
install (TARGETS cmake_examples_inst_bin
    DESTINATION bin)
# 安装库
install (TARGETS cmake_examples_inst
    LIBRARY DESTINATION lib)
# 安装头文件
install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/
    DESTINATION include)
# 安装普通文件
install(FILES cmake-examples.conf
    DESTINATION etc)
```

> This may not work on windows. On platforms that have DLL targets you may need to add the following
> ``` CMake
> install (TARGETS cmake_examples_inst
>     LIBRARY DESTINATION lib
>     RUNTIME DESTINATION bin)
> ```

After `make install` has been run, CMake generates an `install_manifest.txt` file which includes details on all installed files.

> If you run the `make install` command as root, `the install_manifest.txt` file will be owned by root.

### 改变 CMAKE_INSTALL_PREFIX

If you want to change this default location for all users you can add the following code to your top level CMakeLists.txt before adding any binaries or libraries.

```CMake
if( CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT )
  message(STATUS "Setting default CMAKE_INSTALL_PREFIX path to ${CMAKE_BINARY_DIR}/install")
  set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install" CACHE STRING "The path to use for make install" FORCE)
endif()
```

### DESTDIR

暂存所有的安装文件

If you wish to stage your install to confirm that all files are included the `make install` target supports the DESTDIR argument.

```shell
make install DESTDIR=/tmp/stage
```

This will create the install path `${DESTDIR}/${CMAKE_INSTALL_PREFIX}` for all your installation files. In this example, it would install all files under the path `/tmp/stage/usr/local`

#### Uninstall

By default CMake does not add a `make uninstall` target. For details on how to generate an uninstall target see this

For an easy way to remove the files from this example, you can use:

```shell
sudo xargs rm < install_manifest.txt
```

## Build Type

### CMAKE_BUILD_TYPE

CMake has a number of built in build configurations which can be used to compile your project. These specify the optimization levels and if debug information is to be included in the binary.

The levels provided are:

- `Release` - Adds the `-O3 -DNDEBUG` flags to the compiler
- `Debug` - Adds the `-g` flag
- `MinSizeRel` - Adds `-Os -DNDEBUG`
- `RelWithDebInfo` - Adds `-O2 -g -DNDEBUG` flags

命令行中指定：

```shell
cmake .. -DCMAKE_BUILD_TYPE=Release
```

`CMake` 中指定

```CMake
set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Choose the type of build." FORCE)
# set(CMAKE_BUILD_TYPE RelWithDebInfo)
```

> The values `CACHE STRING "Choose the type of build." FORCE` from the above command are used to force this variable to be set in the CMakeCache.txt file.

## Compile Flags

CMake supports setting compile flags in a number of different ways:

- using target_compile_definitions() function
- using the CMAKE_C_FLAGS and CMAKE_CXX_FLAGS variables.

### Set Per-Target C++ Flags

> 更推荐的方式

```CMake
target_compile_definitions(cmake_examples_compile_flags
    PRIVATE EX3
)
```

> `target_compile_definitions` 中定义宏不用 `-D`

只会在生成该目标时增加编译标志 `-DEX3`

```CMake
target_compile_options(cmake_examples_compile_flags
    PRIVATE -Wall)
```

### Set Default C++ Flags

Once set the CMAKE_C_FLAGS and CMAKE_CXX_FLAGS will **set a compiler `flag / definition` globally for all targets in this directory or any included sub-directories**.

```CMake
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DEX2" CACHE STRING "Set C++ Compiler Flags" FORCE)
```

This method is not recommended for general usage now and the target_compile_definitions function is preferred.


### 命令行中设置

```shell
cmake .. -DCMAKE_CXX_FLAGS="-DEX3"
```

与 Set Default C++ Flags 的方式相同，全局的。


## Including Third Party Library

### find_package

```CMake
find_package(Boost 1.46.1 REQUIRED COMPONENTS filesystem system)
```

- Boost - Name of the library. This is part of used to find the module file FindBoost.cmake
- 1.46.1 - The minimum version of boost to find
- REQUIRED - Tells the module that this is required and to fail if it cannot be foun
- COMPONENTS - The list of components to find in the library.

### Checking if the package is found

```CMake
if(Boost_FOUND)
    message ("boost found")
    include_directories(${Boost_INCLUDE_DIRS})
else()
    message (FATAL_ERROR "Cannot find Boost")
endif()
```

### Alias / Imported targets

Most modern CMake libraries export ALIAS targets in their module files. The benefit of imported targets are that they can also populate include directories and linked libraries.

In the case of Boost, all targets are exported using the `Boost::` identifier and then the name of the subsystem. For example you can use:

- `Boost::boost` for header only libraries
- `Boost::system` for the boost system library.
- `Boost::filesystem` for filesystem library.

To link against an imported target you can use the following:

```CMake
  target_link_libraries( third_party_include
      PRIVATE
          Boost::filesystem
  )
```

### Non-alias targets

While most modern libraries use imported targets, not all modules have been updated. In the case where a library hasn't been updated you will often find the following variables available:

- xxx_INCLUDE_DIRS - A variable pointing to the include directory for the library.
- xxx_LIBRARY - A variable pointing to the library path.

These can then be added to your `target_include_directories` and `target_link_libraries` as:

```CMake
# Include the boost headers
target_include_directories( third_party_include
    PRIVATE ${Boost_INCLUDE_DIRS}
)

# link against the boost libraries
target_link_libraries( third_party_include
    PRIVATE
    ${Boost_SYSTEM_LIBRARY}
    ${Boost_FILESYSTEM_LIBRARY}
)
```

## 指定编译器

```shell
cmake .. -DCMAKE_C_COMPILER=clang-3.6 -DCMAKE_CXX_COMPILER=clang++-3.6
```

CMake exposes options to control the programs used to compile and link your code. These programs include:

- `CMAKE_C_COMPILER` - The program used to compile c code.
- `CMAKE_CXX_COMPILER` - The program used to compile c++ code.
- `CMAKE_LINKER` - The program used to link your binary.

## 指定生成器（Generator）

As mentioned, CMake is a meta-build system that can be used to create the build files for many other build tools.

CMake [generators](https://cmake.org/cmake/help/v3.0/manual/cmake-generators.7.html "https://cmake.org/cmake.org/cmake/help/v3.0/manual/cmake-generators.7.html") are responsible for writing the input files (e.g. Makefiles) for the underlying build system. Running `cmake --help` will show the generators available.

```txt
Generators

The following generators are available on this platform (* marks default):
* Unix Makefiles               = Generates standard UNIX makefiles.
  Ninja                        = Generates build.ninja files.
  Ninja Multi-Config           = Generates build-<Config>.ninja files.
  Watcom WMake                 = Generates Watcom WMake makefiles.
  Xcode                        = Generate Xcode project files.
  CodeBlocks - Ninja           = Generates CodeBlocks project files.
  CodeBlocks - Unix Makefiles  = Generates CodeBlocks project files.
  CodeLite - Ninja             = Generates CodeLite project files.
  CodeLite - Unix Makefiles    = Generates CodeLite project files.
  Eclipse CDT4 - Ninja         = Generates Eclipse CDT 4.0 project files.
  Eclipse CDT4 - Unix Makefiles= Generates Eclipse CDT 4.0 project files.
  Kate - Ninja                 = Generates Kate project files.
  Kate - Unix Makefiles        = Generates Kate project files.
  Sublime Text 2 - Ninja       = Generates Sublime Text 2 project files.
  Sublime Text 2 - Unix Makefiles
                               = Generates Sublime Text 2 project files.
```

构建工具有：

Command-Line Build Tool Generators

- Unix Makefiles
- Ninja
- ...

IDE Build Tool Generators

- Xcode
- Visual Studio 2019

### Calling a Generator

```shell
cmake .. -G Ninja
```


## 设置 C++ 标准

### CMAKE_CXX_STANDARD

全局的

```CMake
cmake_minimum_required(VERSION 3.1)

project (hello_cpp11)

# set the C++ standard to C++ 11
set(CMAKE_CXX_STANDARD 11)

add_executable(hello_cpp11 main.cpp)

```

> The `CMAKE_CXX_STANDARD` variable falls back to the closest appropriate standard without a failure. For example, if you request `-std=gnu11` you may end up with `-std=gnu0x`.

> This can result in an unexpected failure at compile time.


### target_compile_features

根据支持的语言特性选择 C++ 标准

```CMake
target_compile_features(hello_cpp11 PUBLIC cxx_auto_type)
```

The list of available features can be found from the [CMAKE_CXX_COMPILE_FEATURES](https://cmake.org/cmake/help/v3.1/variable/CMAKE_CXX_COMPILE_FEATURES.html#variable:CMAKE_CXX_COMPILE_FEATURES "https://cmake.org/cmake/help/v3.1/variable/CMAKE_CXX_COMPILE_FEATURES.html#variable:CMAKE_CXX_COMPILE_FEATURES") variable.


## Sub-Project

### Adding a Sub-Directory

A `CMakeLists.txt` file can include and call sub-directories which include a CMakeLists.txt files.

```CMake
# top CMakeLists.txt
add_subdirectory(sublibrary1)
add_subdirectory(sublibrary2)
add_subdirectory(subbinary)
```

### Referencing Sub-Project Directories

When a project is created using the `project()` command, CMake will automatically create a number of variables which can be used to reference details about the project. These variables can then be used by other sub-projects or the main project. For example, to reference the source directory for a different project you can use.

```CMake
    ${sublibrary1_SOURCE_DIR}
    ${sublibrary2_SOURCE_DIR}
```

The variables created by CMake include:

| Variable           | Info                                                                                      |
| ------------------ | ----------------------------------------------------------------------------------------- |
| PROJECT_NAME       | The name of the project set by the current `project()`.                                   |
| CMAKE_PROJECT_NAME | the name of the first project set by the `project()` command, i.e. the top level project. |
| PROJECT_SOURCE_DIR | The source director of the current project.                                               |
| PROJECT_BINARY_DIR | The build directory for the current project.                                              |
| name_SOURCE_DIR    | The source directory of the project called "name". 通过这种方式可以引用其他子项目的 `SOURCE_DIR`          |
| name_BINARY_DIR    | The binary directory of the project called "name".通过这种方式可以引用其他子项目的 `BINARY_DIR`           |
|                    |                                                                                           |
### header only liburary

If you have a library that is created as a header only library, cmake supports the INTERFACE target to allow creating a target without any build output.

```CMake
add_library(${PROJECT_NAME} INTERFACE)
```

When creating the target you can also include directories for that target using the INTERFACE scope. The INTERFACE scope is use to make target requirements that are used in any Libraries that link this target but not in the compilation of the target itself.

```CMake
target_include_directories(${PROJECT_NAME}
    INTERFACE
        ${PROJECT_SOURCE_DIR}/include
)
```

完整项目：

```CMake
# Set the project name
project (sublibrary2)

# 区别静态/动态库 add_library(${PROJECT_NAME} 源文件列表)
add_library(${PROJECT_NAME} INTERFACE)
add_library(sub::lib2 ALIAS ${PROJECT_NAME})


target_include_directories(${PROJECT_NAME}
    INTERFACE # 普通动态库/静态库指定头文件时，属性一般为 PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)
```


### Referencing Libraries from Sub-Projects

If a sub-project creates a library, it can be referenced by other projects by calling the name of the target in the `target_link_libraries()` command. This means that you don't have to reference the full path of the new library and it is added as a dependency.

```cmake
target_link_libraries(subbinary
    PUBLIC
        sublibrary1
)
```

> link 不是 sub-project 生成的库时首先需要使用 `target_link_derectories()` 指定库的搜索路径

Alternatively, you can create an alias target which allows you to reference the target in **read only contexts**.

To create an alias target run:

```cmake
add_library(sublibrary2)
add_library( sub::lib2 ALIAS sublibrary2)
```

To reference the alias, just it as follows:

```cmake
target_link_libraries(subbinary
    PUBLIC sub::lib2
)
```

### Include directories from sub-projects

目标 A 链接目标 B 时，B 目标使用 `target_include_directores()` 包含的他文件路径会被传导到目标 A，可以使用 `PUBLIC、PRIVATE、PUBLIC` 指定传递性
### 组织项目

[如何组织你的项目 · Modern CMake](https://modern-cmake-cn.github.io/Modern-CMake-zh_CN/chapters/basics/structure.html)


## Configure Files Generation

### Configure Files

```CMake
configure_file(ver.h.in ${PROJECT_BINARY_DIR}/ver.h)

configure_file(path.h.in ${PROJECT_BINARY_DIR}/path.h @ONLY)
```

The first example above, allows the variable to be defined like a CMake variables using the `${}` syntax or an `@@` in the ver.h.in file. After generation a new file ver.h will be available in the `PROJECT_BINARY_DIR`.

```cpp
// .in
const char* ver = "${cf_example_VERSION}";
// or
const char* path = "@CMAKE_SOURCE_DIR@";
```

## unit testing

[[CMake 构建 gtest]]

### Download and Build Google Test

```CMake
add_subdirectory(3rd_party/google-test)
```

This will add the CMake files which download and build Google Test. This is the recommended way to add google test and there are more details from [google test readme](https://github.com/google/googletest/blob/master/googletest/README.md "https://github.com/google/googletest/blob/master/googletest/README.md") and [here](http://crascit.com/2015/07/25/cmake-gtest/ "http://crascit.com/2015/07/25/cmake-gtest/")


### Enabling testing

To enable testing you must include the following line in your top level CMakeLists.txt

```CMake
enable_testing()
```

This will enable testing for the current folder and all folders below it.

[unit testing - What does enable\_testing() do in cmake? - Stack Overflow](https://stackoverflow.com/questions/50468620/what-does-enable-testing-do-in-cmake)
