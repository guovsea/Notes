---
tags:
  - compiler/parsing
  - type/howto
mastery: 5
---
## 相关

- [[编译过程]] -- flex/bison 对应编译过程中的词法分析和语法分析阶段

## 概述

**Flex**（Fast Lexical Analyzer Generator）和 **Bison**（GNU 版 Yacc）是一对经典的编译器前端工具：
- **Flex** 负责词法分析（Lexical Analysis），将字符流分割为 Token 流
- **Bison** 负责语法分析（Syntax Analysis / Parsing），根据文法规则将 Token 流组织为语法树（AST）

它们分别替代了早期的 **lex** 和 **yacc**（Yet Another Compiler Compiler）。

## 基本工作流程

```
源代码 → [Flex 词法分析器] → Token 流 → [Bison 语法分析器] → AST / 执行动作
```

1. 编写 `.l` 文件（Flex 词法规则）和 `.y` 文件（Bison 语法规则）
2. Flex 生成 `lex.yy.c`，Bison 生成 `*.tab.c` 和 `*.tab.h`
3. 编译链接生成的 C/C++ 文件

## 参考资源

- [StackOverflow: Bison/Flex tutorial with C AST and re-entrant](https://stackoverflow.com/questions/10732932/bison-flex-tutorial-with-c-ast-and-re-entrant-both-lexer-and-parser)
- [phc -- Writing a Reentrant Parser with Flex and Bison -- Edsko de Vries](https://web.archive.org/web/20120520150851/http://www.phpcompiler.org/articles/reentrantparser.html)
