---
tags:
  - cpp/语法
  - state/未开始
---
踩了个大坑：

**在 Qt 中**，将 inline 函数放在了源文件中，这样会始终报链接错误：

```cpp
// ip.h
class IP {
public:
    IP(QString addr);
private:
    uint32_t impl;
};
// ip.cpp 
inline
IP::IP(QString addr)
{
    qDebug() << addr;
}
// :-1: error: symbol(s) not found for architecture x86_64

```
```text
Undefined symbols for architecture x86_64:
  "IP::IP(QString)", referenced from:
      _main in main.o
ld: symbol(s) not found for architecture x86_64
clang: error: linker command failed with exit code 1 (use -v to see invocation)
make: *** [static_routing.app/Contents/MacOS/static_routing] Error 1
```

**但实际上，按照 C++ 的语法而言，inline 就应该放在实现的前面（cpp 文件中）**

Qt 声明内联只能把函数体定义在头文件中。