---
tags:
  - act/project
  - compiler/parsing
  - type/notes
---

# Grammar Parsing 学习笔记

## 准备 C++ 运行时

1. 下载源码 [antlr4-4.13.2](https://github.com/antlr/antlr4/releases/tag/4.13.2)
2. 修改 `thirdparty\antlr4-4.13.2\runtime\Cpp\runtime\src\atn\ProfilingATNSimulator.cpp` 文件，添加 `#include <chrono>`

> 此修改解决某些编译器下缺少 `<chrono>` 头文件导致的编译错误。

## ANTLR 简介

ANTLR (ANother Tool for Language Recognition) 是一个强大的语法分析器生成器：
- 根据语法规则文件（`.g4`）自动生成词法分析器和语法分析器
- 支持多种目标语言（Java、C++、Python 等）
- 广泛用于编译器、DSL、配置文件解析等场景

## 相关

- [[Learning Path]]
- [[flex + bison]]