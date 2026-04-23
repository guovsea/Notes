---
tags:
  - tools/cmake
  - type/howto
mastery: 5
---

# CMake 构建 gtest

> Google Test (gtest) 是 Google 开源的 C++ 单元测试框架。

## 安装

1. 下载
2. CMake 编译
3. 将 build 输出的静态库 lib 库拷贝到项目中

```text
└── lib
    ├── libgmock.a
    ├── libgmock_main.a
    ├── libgtest.a
    └── libgtest_main.a
```

4. 将 googletest 和 googlemock 中的头文件拷贝到项目中

```text
├── googlemock
│   ├── include
│   │   └── gmock  # 把这个文件夹拷贝过去

├── googletest
│   ├── include
│   │   └── gtest  # 把这个文件夹拷贝过去
```

5. 配置测试项目的 `CMakeLists.txt`

```cmake
cmake_minimum_required(VERSION 3.21)
project(gtest)

include_directories(./include)

set(CMAKE_CXX_STANDARD 17)
link_directories(./lib)

add_executable(${PROJECT_NAME} src/main.cpp)

target_link_libraries(${PROJECT_NAME} gtest gtest_main)

```

## 使用

```cpp
#include <iostream>
#include <gtest/gtest.h>

int add(int a, int b) {
    return a + b;
}
TEST(testCase1, should_3_when_given_2_and_1) {
    EXPECT_EQ(add(2, 1), 3);
}
int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
```

官方方法：

[googletest/googletest/README.md at main · google/googletest · GitHub](https://github.com/google/googletest/blob/main/googletest/README.md#generic-build-instructions)