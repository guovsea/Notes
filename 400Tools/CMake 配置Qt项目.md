---
tags:
  - tools/cmake
  - type/howto
mastery: 5
---

# CMake 配置 Qt 项目

## 基本 CMake 文件

```cmake
cmake_minimum_required(VERSION 3.5)
project(static_routing LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)      # UI 文件 
set(CMAKE_AUTOMOC ON)      # MOC （处理头文件中带有 O_OBJECT 的类的信号）
set(CMAKE_AUTORCC ON)      # 资源文件

# 定义 find_package 的搜索路径，否则可能会找不到
set(CMAKE_PREFIX_PATH "/Users/guo/240_c++/Qt/5.14.2/clang_64")

# 直接将 .qrc 文件增加到 add_executable 即可
set(RESOURCES ${PROJECT_SOURCE_DIR}/res/res.qrc)

find_package(Qt5 COMPONENTS Widgets REQUIRED)

file(GLOB HEADERS "include/*")
file(GLOB SOURCES "src/*")

include_directories("include")

# 编译可执行文件
add_executable(${PROJECT_NAME}
        main.cpp
        ${HEADERS} # 让 IDE 能够识别
        ${SOURCES}
        ${RESOURCES}
        )
# 链接
target_link_libraries(${PROJECT_NAME} PRIVATE
        Qt5::Widgets
        )

add_subdirectory(tests)
```

## 支持 UI 文件

### UI 文件

UI 文件本质上是 XML 文件

### CMake 中添加支持 UI 的配置

UI 文件使用 Qt Creator 生成，并使用 Qt Creator 构建。UI 文件会被 Qt Creator 处理，如果是 CMake 项目的话，会在下面生成一个 `${PROJECT_NAME}_autogen` 文件夹，中间存放 UI 文件生成的头文件，如下：

```txt
├── CMakeCache.txt
├── CMakeFiles
├── build.ninja
├── cmake_install.cmake
├── compile_commands.json
├── mychat
└── mychat_autogen
    ├── EWIEGA46WW
    │   └── moc_mainwindow.cpp
    ├── include
    │   └── ui_mainwindow.h   # ui 文件生成的头文件
    ├── moc_predefs.h
    └── mocs_compilation.cpp
```

因此，要在 CMake 中加入 `ui_.h` 的搜索路径：

```cmake
target_include_directories(mychat PUBLIC "${CMAKE_BINARY_DIR}/${PROJECT_NAME}/include")
```

> 在 Qt Creator 中修改后需要构建才能生效
## 支持 QRC 文件

### QRC 文件

QRC 文件本质上是一个 XML 文件，用于定义资源文件在 Qt 中的索引路径和其实际存放路径之间的映射关系。

没有任何内容的 QRC 文件如下：

```xml
<!DOCTYPE RCC>
<RCC version="1.0"/>
```

添加一个资源文件之后

> 在 Qt Creator 中添加，"添加 Qt 文件，设置前缀为 res"

```xml
<RCC>
    <qresource prefix="/res">
        <file>res/addtip.png</file>
    </qresource>
</RCC>
```

> 在 Qt Creator 中修改后需要构建才能生效


### CMake 中添加支持 QRC 的配置

```cmake
set(QRC_FILE ${PROJECT_SOURCE_DIR}/res/res.qrc)

add_executable(${PROJECT_NAME}
        main.cpp
        ${HEADERS}
        ${SOURCES}
        ${QRC_FILE}
)
```

> Note

不知道为什么，这样设置无效(也就是是将 qrc 文件放在顶层，将资源文件放在子文件夹中)

```xml
<RCC>
    <qresource prefix="/res">
        <file>res/ice.png</file>
    </qresource>
</RCC>
```

必须得将 qrc 文件和资源文件夹放在一起才行

### 方便快捷地添加多个资源文件

使用 Qt Creator 打开 qrc 文件，然后添加文件。