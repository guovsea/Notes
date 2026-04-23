---
tags:
  - tools/cmake
  - type/reference
mastery: 5
---
## `CMAKE_ *_DIR`

有用的参考： [cmake - Difference between CMAKE\_CURRENT\_SOURCE\_DIR and CMAKE\_CURRENT\_LIST\_DIR - Stack Overflow](https://stackoverflow.com/questions/15662497/difference-between-cmake-current-source-dir-and-cmake-current-list-dir)

### CMAKE_SOURCE_DIR

执行 `cmake -S <path>` / `cmake <path> ` 时所对应的 path，也就是执行 CMake 命令时的入口 `CMakeLists.txt` 所在路径

> 运行 CMake 的时候，该变量的值始终不会改变 

### CMAKE_CURRENT_SOURCE_DIR

当前正在处理的 `CMakeLists.txt` 的路径

### CMAKE_CURRENT_LIST_DIR

包含 `CMAKE_CURRENT_LIST_DIR` [[系统学习CMake-基础语法#Modeules|模块文件]]所在的路径


实验：

```cmake
# topLevel
# 位于 C:/WorkSpace/lab/leanCMake
cmake_minimum_required(VERSION 3.18)

project(test)

message(topLevel-CMAKE_CURRENT_LIST_DIR: ${CMAKE_CURRENT_LIST_DIR})
message(topLevel-CMAKE_SOURCE_DIR : ${CMAKE_SOURCE_DIR})

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/modules)
include(config)
```

```cmake
# config-cmake
# 位于 C:/WorkSpace/lab/leanCMake/modules
message(config-CMAKE_CURRENT_LIST_DIR: ${CMAKE_CURRENT_LIST_DIR})
message(config-CMAKE_CURRENT_SOURCE_DIR: ${CMAKE_CURRENT_SOURCE_DIR})
```

```
topLevel-CMAKE_CURRENT_LIST_DIR:C:/WorkSpace/lab/leanCMake
topLevel-CMAKE_SOURCE_DIR:C:/WorkSpace/lab/leanCMake
config-CMAKE_CURRENT_LIST_DIR:C:/WorkSpace/lab/leanCMake/modules
config-CMAKE_CURRENT_SOURCE_DIR:C:/WorkSpace/lab/leanCMake
```

### PROJECT_SOURCE_DIR

距离 `PROJECT_SOURCE_DIR` 最近的 `project()` 的 `CMakeLists.txt` 所在的路径

 The nearest folder that contains CMakeList.txt file, **with its own scope**, that contains project() command.


实验：

目录树结构：

```
topLevel # 目录leanCMake
    CMakeLists.txt
    projA
        CMakeLists.txt 
        sub
            CMakeLists.txt  # proj(a)
    projB
        CMakeLists.txt # proj(b)
        sub
            CMakeLists.txt
```

```
# topLevel/CMakeLists.txt的输出
topLevel-PROJECT_SOURCE_DIR:leanCMake
topLevel-CMAKE_SOURCE_DIR:leanCMake

# topLevel/projA/sub/CMakeLists.txt的输出
projA-PROJECT_SOURCE_DIR:leanCMake/projA/sub
projA-CMAKE_SOURCE_DIR:leanCMake

# topLevel/projB/CMakeLists.txt的输出
projB-PROJECT_SOURCE_DIR:leanCMake/projB
projB-CMAKE_SOURCE_DIR:leanCMake

# topLevel/projB/sub/CMakeLists.txt的输出
projB-sub-PROJECT_SOURCE_DIR:eanCMake/projB
projB-sub-CMAKE_SOURCE_DIR:leanCMake
```

## `*_BINARY_DIR`

CMake 在构建过程中会保持源文件的目录结构，每个 `CMakeLists.txt` 都会对应一个输出路径

```
CMAKE_SOURCE_DIR                -->  CMAKE_BINARY_DIR
    CMAKE_CURRENT_SOURCE_DIR    -->      CMAKE_CURRENT_BINARY_DIR
```

### CMAKE_BINARY_DIR

顶级构建目录，对应 `CMAKE_SOURCE_DIR` 的输出路径，即是 `cmake -B <path>` 的路径

### CMAKE_CURRENT_BINARY_DIR

`CMAKE_CURRENT_BINARY_DIR` **当前正在处理的** `CMakeLists.txt` 的输出路径

### PROJECT_BINARY_DIR

当前项目的输出路径，距离最近的包含 `project()` 的 `CMakeLists.txt` 的输出路径

## OUTPUT_PATH

### EXECUTABLE_OUTPUT_PATH

如果未指定，默认为 `CMAKE_BINARY_DIR`
### LIBRARY_OUTPUT_PATH

默认也为