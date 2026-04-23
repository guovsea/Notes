---
tags:
  - cpp/basics
  - type/concept
---
## 函数 mangling

**编译器会将每个唯一的函数和参数列表组合编码成一个对连接器来说唯一的名字**

```cpp
class Apple {
public:
    void move();
    inline void f(){}
    int heavy;
private:
    void fly() {}
};
void Apple::move() {}

void func(int ){}
void func(Apple a){}
void func(Apple a, int i){}
extern "C" {
    void function() {}
}
```

> 一个被重整的类/函数的名字是有名字中字符的数量后跟上原始名字组成的。func->4func, Apple->5Apple
> 重载的函数会在重整后的函数名字后面跟上重整后的参数列表名字

```shell
➜  name_magling objdump -t libmine.so

libmine.so:     file format Mach-O 64-bit x86-64

SYMBOL TABLE:
0000000000003f90 g     F __TEXT,__text __Z4func5Apple
0000000000003fa0 g     F __TEXT,__text __Z4func5Applei
0000000000003f80 g     F __TEXT,__text __Z4funci
0000000000003f70 g     F __TEXT,__text __ZN5Apple4moveEv
0000000000003fb0 g     F __TEXT,__text _function
0000000000000000         *UND* dyld_stub_binder
```

- `libmine.so` 是一个64位的 Mach-O 格式的文件，这表明它可能是为 macOS 或 iOS 系统编译的。
    
- `SYMBOL TABLE` 列出了库中的一些符号。每个符号条目包括以下信息：
    
    - 地址：符号在内存中的起始地址。
    - 权限：例如 `g` 表示全局符号，`F` 表示文件本地符号。
    - 类别：例如 `__TEXT` 表示代码所在的段，`__text` 表示具体的代码部分。
    - 符号名称：例如 `_Z4func5Apple` 表示一个名为 `func` 的函数，它接受一个 `Apple` 类型的参数。
- `_ZN5Apple4moveEv` 表示一个名为 `Apple` 的类的成员函数 `move`，没有参数且返回类型为 `void`。


`extern "C"` 可以兼容 C 代码，该作用域内的符号不会进行 name mangling

\
## 恢复 demangling

```shell
guo@ubuntu ~/ % c++filt
_ZN5Apple4moveEv
Apple::move()
_Z4func5Applei
func(Apple, int)
```