---
tags:
  - act/project
  - tools/cmake
  - type/notes
---

## GCC 工具链

适用场景：Linux、macOS、Windows（通过 MinGW 或 Cygwin）
组成元素：

- 编译器：gcc（C 语言）、g++（C++），负责将源代码编译为机器码。

- 调试器：gdb（GNU Debugger），支持调试 GCC 编译的程序，兼容 DWARF 调试格式。

- 辅助工具：

- ld：GNU 链接器，负责将目标文件链接为可执行程序。

- objdump、readelf：用于查看目标文件或可执行程序的信息（如符号表、调试信息）。

- make：构建执行者，执行编译命令，管理文件依赖关系。可作为 CMake 工具的后端使用，也可以直接管理小型项目。

## MSVC 工具链

适用场景：Windows 系统
组成元素：

- 编译器：cl.exe（C/C++ 编译器），是 Visual Studio 的核心编译器。

- 调试器：Visual Studio 内置调试器（集成在 IDE 中），支持调试 MSVC 编译的程序，依赖 .pdb 符号文件（PE/COFF 调试格式）。

- 辅助工具：

- link.exe：微软链接器，生成 Windows 可执行程序（.exe）或动态链接库（.dll）。

- lib.exe：用于创建和管理静态链接库（.lib）。

- msbuild：微软的构建工具，配合 .sln 或 .vcxproj 项目文件自动化编译。

## LLVM/Clang 工具链

适用场景：Linux、macOS、Windows（跨平台支持较好）
组成元素：

- 编译器：clang（C 语言）、clang++（C++），基于 LLVM 架构，兼容 GCC 语法，生成的调试信息默认采用 DWARF（Linux/macOS）或可适配 MSVC 的 .pdb（Windows）。

- 调试器：lldb，LLVM 配套调试器，原生支持 Clang 生成的程序，兼容 DWARF 格式，调试体验更现代化（如支持更灵活的表达式解析）。

- 辅助工具：

- lld：LLVM 链接器，可替代 ld，性能和兼容性较好。

- llvm-objdump、llvm-readelf：LLVM 版本的目标文件分析工具。


上述工具链的核心是编译器，某些调试器只能调试特定的编译器生成的可执行程序。[[调试器能否调试取决于编译器生成的调试信息的格式]]

## 平台无关工具

-  ninja：构建执行者，执行编译命令，管理文件依赖关系。作为 CMake 、gn 工具的后端使用，速度很快（并行编译调度优化，build.inja 文件格式设计简单）。