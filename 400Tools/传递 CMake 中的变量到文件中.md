---
tags:
  - tools/cmake
  - type/howto
mastery: 5
---
原理：用 CMake 中定义的变量替换某个文件中以 `@` 包裹的变量

向头文件中添加版本号：

```CMake
cmake_minimum_required(VERSION 3.5)

project(test
    VERSION 3.1.2)

message(${PROJECT_VERSION})

configure_file (
    "${PROJECT_SOURCE_DIR}/Version.h.in" # 被替换的文件
    "${PROJECT_BINARY_DIR}/Version.h"    # 替换后生成的文件
)
add_executable(app main.cpp)

# 执行 CMake 之后会生成 ${CMAKE_BINARY_DIR}/Version.h 文件
target_include_directories(app PUBLIC ${CMAKE_BINARY_DIR})
```

```cpp
// Version.h.in

#define MY_VERSION_MAJOR @PROJECT_VERSION_MAJOR@
#define MY_VERSION_MINOR @PROJECT_VERSION_MINOR@
#define MY_VERSION_PATCH @PROJECT_VERSION_PATCH@
#define MY_VERSION "@PROJECT_VERSION@"
```

```cpp
// other files
#include <iostream>
#include "Version.h"

int main(int argc, char *argv[])
{
   std::cout << MY_VERSION << "\n";
   return 0;
}
```
