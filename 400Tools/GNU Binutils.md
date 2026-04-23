---
tags:
  - tools/gdb
  - type/reference
mastery: 5
---

# GNU Binutils

> GNU Binutils 是 GNU 二进制工具集，包含一系列用于处理目标文件的工具。

| 工具 | 说明 |
|------|------|
| **ar** | 打包生成静态库 |
| **as** | 汇编器 |
| **ld** | 链接器。编译过程中，gcc 会调用链接器 ld，将用户编译生成的目标文件连同系统的 libc 启动代码链接在一起形成最终的可执行文件 |
| **nm** | 查看目标文件中的符号（全局变量、全局函数等） |
| **objcopy** | 将原目标文件中的内容复制到新的目标文件中，可以通过不同的命令选项调整目标文件的格式，比如去除某些 ELF 文件头 |
| **objdump** | 用于生成反汇编文件，主要依赖 objcopy 实现。编译时需要 `-g`，常用命令：`objdump -dSsx a.out > file` |
| **ranlib** | 为静态库文件创建索引，相当于 `ar` 命令的 `s` 选项 |
| **readelf** | 解读 ELF 文件头 |
| **strip** | 去除目标文件中的符号信息，减小文件体积 |
| **strings** | 打印目标文件中的可打印字符串 |
| **addr2line** | 将地址转换为源码文件名和行号 |

参考：[GNU Binutils - Wikipedia](https://en.wikipedia.org/wiki/GNU_Binutils)