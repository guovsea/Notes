---
tags:
  - tools/cmake
  - type/notes
mastery: 5
---

# cmake-examples 学习笔记

> 学习 [cmake-examples](https://github.com/sorush-khajepor/cmake-examples) 项目的笔记，以 geometry 项目为例演示共享库配置。

## 从一个项目开始


[download here](https://github.com/sorush-khajepor/cmake-examples/tree/master/shared_library_config/geometry)


项目结构

```
--- geometry
    ---- shape
    |      --- shape.h, shape.cpp
    |      |
    |      --- CMakeLists.txt
    ---- square
    |      |
    |      --- square.h, square.cpp, info.h
    |      |
    |      --- CMakeLists.txt
    ---- example
    |      |
    |      --- app.cpp
    ---- CMakeLists.txt
```

```cmake
target_sources(geo
    PRIVATE shape.cpp
    PUBLIC FILE_SET HEADERS
    BASE_DIRS ${PROJECT_SOURCE_DIR}
    FILES shape.h)
```

```cmake
target_sources( geo
    PRIVATE square.cpp info.h
    PUBLIC FILE_SET HEADERS
    BASE_DIRS ${PROJECT_SOURCE_DIR}
    FILES square.h)

```

这只是把原本需要在顶层 `CMakeLists.txt` 中写的内容分别写到了两个不同的 `subdirectory` 中


```cmake
# 顶层
cmake_minimum_required(VERSION 3.23)
project(geometry LANGUAGES CXX VERSION 5.4.3)

if (MSVC)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

add_library(geo SHARED)
target_include_directories(geo PRIVATE "${PROJECT_SOURCE_DIR}")
add_subdirectory("shape")
add_subdirectory("square")

install(TARGETS geo
    EXPORT geoTargets
    FILE_SET HEADERS
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include)

install(EXPORT geoTargets
    FILE geoTargets.cmake
    NAMESPACE geo::
    DESTINATION lib/cmake/geo)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "geoConfigVersion.cmake"
    VERSION ${geo_VERSION}
    COMPATIBILITY AnyNewerVersion)

install(FILES "geoConfig.cmake" "${CMAKE_CURRENT_BINARY_DIR}/geoConfigVersion.cmake"
    DESTINATION lib/cmake/geo)
```