---
tags:
  - compiler/linking
  - type/concept
mastery: 5
---
## 相关

- [[库 Lib(DLL)]] -- DLL 属于 PE 文件格式
- [[链接过程]] -- PE 是 Windows 上的可执行目标文件格式

[PE文件结构详解 --（完整版）\_pe结构图下载-CSDN博客](https://blog.csdn.net/freeking101/article/details/102752048)

DLL、EXE 都属于 PE 文件
## 导入表/导出表

> 对应 ELF 的 .symtab

DLL 都有导入导出表、EXE 有导入表，一般没有导出表

1. **导出表（Export Table）**：用来描述 PE 中的导出函数的结构，如果一个 PE 导出了函数，那么这个函数会被记录在导出表中，这样通过 GetProcAddress 函数就能动态获取到函数的地址

2. **导入表（Import Table）**：在 PE 文件加载时，会根据这个表里的内容加载依赖的 DLL ( 模块 )，并填充所需函数的地址。
### 编译动态库为什么会有 DLL、LIB 和 EXP 文件

1. **DLL 文件**：动态链接库的主要文件，它包含了库的代码和资源，它就是 PE 文件

2. **LIB 文件**：并不是 PE 文件，而是（Import Library）。PE 文件 A 链接 B 时，需要 B 的 LIB 文件生成其导入表。因此编译是要链接 lib 文件

3. **EXP 文件**：仅仅用于帮助生成 LIB 文件，非必需



### `__declspec(dllexport)` 和 `__declspec(dllimport)`

1. `dllexport`

告诉编译器，要将这些符号放到 PE 的导出表中。只有使用 `__declspec(dllexport)` 声明的符号才会出现在 PE 的导出表中，使得这些符号可以被其他程序或 DLL 访问。

> 因此，如果生成共享库（也就是共享目标文件）的源文件中没有声明任何 `__declspec(dllexport)` ，它就不会有导出的符号，就不会生成 LIB 文件，就不能被其他共享库链接（因为没有导出符号供其他 PE 导入）


2. `dllimport`

告诉编译器，该符号来自外部 DLL（即位于导入表中），这样编译器会生成更高效的间接调用指令，而不是在运行时通过 `GetProcAddress` 动态查找。使用 `__declspec(dllimport)` 声明的符号在加载 PE 时会被自动解析到对应 DLL 中的实际地址。
