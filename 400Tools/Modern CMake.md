---
tags:
  - tools/cmake
  - type/concept
mastery: 5
---
# Modern CMake

## 何谓 Modern

从 Modern CMake (>=3.0) 开始，使用的范式从 directory-oriented 转换到了 target-oriented。这其中最重要的有三个概念：target、properties、传递性

target 

就是编译的目标，通常有三种：

- 静态库: 使用 add_library()
- 动态库: 使用 add_library() 指定 SHARED 关键字
- 可执行文件: 使用 add_executable


properties 

每个 target 的属性，最常见的有以下五种：

- 编译标志：使用 target_compile_options
- 预处理宏标志：使用 target_compile_definitions
- 头文件目录：使用 target_include_directories
- 链接库：使用 target_link_libraries
- 链接标志：使用 target_link_options

传递性

属性在不同 target 之间的传递性。有三种：

- PRIVATE
- PUBLIC
- INTERFACE

## CMake 工作流程

CMake 工作流程分为三步：

- 配置阶段
- 生成阶段
- 构建阶段

### Configuring 配置阶段

在这个阶段，`CMake` 会读取 `CMakeLists.txt` 文件并**执行其中的命令**和**解析变量**，**根据命令和变量产生配置信息**（配置信息包括：编译选项、编译器信息、库路径等，配置信息的载体为：`CMakeCache.txt` 和 `CMakeFiles` ）。

### Generating 生成阶段

在这个阶段，`CMake` 根据配置阶段生成的配置信息生成构建系统所需的实际构建文件（比如 `Makefile` 或 `Visual Studio` 的 `.sln` 工程文件等）

### Building 构建阶段

根据构建文件进行**实际的构建操作的过程**，比如使用 `make` 命令或在 `Visual Studio` 中构建可执行方案。该过程又可以细分为[[编译过程]]

[[Morden CMake 基础语法]]