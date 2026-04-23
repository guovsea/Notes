---
tags:
  - cpp/basics
  - type/concept
---
## 核心原则

> Failing to open a file or to reach the end of an iteration are expected events and not exceptional; 《A Tour of C++, 4.6》

异常应用于**真正异常的、不可预期的错误**，而非正常的控制流。

## 何时使用异常

- 构造函数失败（构造函数没有返回值，无法通过返回值报错）
- 无法通过返回值报告错误的场景
- 需要跨多层调用传播错误的场景

## 何时不使用异常

- 预期可能发生的情况（文件打开失败、到达迭代末尾）
- 性能极度敏感的热路径（异常有运行时开销）
- 与禁用异常的代码交互时（`-fno-exceptions`）