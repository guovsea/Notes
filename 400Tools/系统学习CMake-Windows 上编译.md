---
tags:
  - tools/cmake
  - type/reference
mastery: 5
---
## 导出符号 `decl-specifier`

### dellimport / dllexport

#### dllimport

`dllimport`关键字用于告诉编译器，某个符号（函数、类、变量等）是从DLL中导入的。当你在一个应用程序中使用一个DLL中的符号时，你需要使用`dllimport`来声明这个符号。这样，编译器会知道这个符号在编译时是不可用的，需要在运行时从DLL中加载。
#### dllexport

相对地，`dllexport`关键字用于告诉编译器，某个符号是要被导出到DLL中的。当你创建一个DLL并希望其他程序能够使用这个DLL中的符号时，你需要使用`dllexport`来声明这个符号。这样，编译器会生成必要的导入库（`.lib`文件），供其他程序链接。

#### 使用

```cpp
#ifdef MYDLL_EXPORTS
#define MYDLL_API __declspec(dllexport)
#else
#define MYDLL_API __declspec(dllimport)
#endif

MYDLL_API void doSomething();
```

#### 原理

- **Linux 中**，符号地址和函数/变量都在 `.so` 文件中。`.so` 文件在运行时动态加载，符号解析由动态链接器负责，根据需要解析并链接到相应的库中。

- **Windows 中**，`.dll` 文件包含了实际的代码和数据，以及导出表（Export Table），记录了可以供外部程序调用的函数和变量；而 `.lib` 文件则包含了符号的导入表信息。编译器在编译时使用 `.lib` 文件来生成程序代码，使得程序在运行时可以正确地加载和调用 `.dll` 文件中的函数和变量。

> 链接的知识还需要学习 csapp 才能更清晰 