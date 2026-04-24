---
---

## 作用

声明具有 C 的 linkage [[链接属性 Linkage#Language linkage (C++ only)]] 

[What is the effect of extern "C" in C++? - Stack Overflow](https://stackoverflow.com/questions/1041866/what-is-the-effect-of-extern-c-in-c/)

## 本质

C 语言不支持函数重载

C++为了支持函数重载，会对函数进行 function [[mangling 符号重整]]。

同名但参数列表不同的函数在编译时函数名会被替换成可以区分函数参数列表的函数名。

```cpp
int add(int a, int b) ;       // -> 会被编译成：add_int_int（举例，实际并不一定）
float add(float a, float b) ; // ->          add_float_float
```

因此，在 C++ 中，为了使用 C 的函数，需要声明相应的 C 函数的[[链接属性 Linkage]] 为` extern "C"`