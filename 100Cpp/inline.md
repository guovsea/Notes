---
tags:
  - cpp/basics
  - type/pitfall
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

原因是 inline 不会出符号，导致在头文件中的函数声明找不到定义。