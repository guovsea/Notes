---
tags:
  - cpp
  - cpp/机制
date: 2026-01-03
---
## `std::move`

本质：将一个作为左值的变量 `static_cast` 为右值。

作用：将左值变量强转为右值，让编译器选择**移动构造函数和移动赋值运算符**构造新对象，从而进行移动。

> 如何移动是由移动构造和移动赋值运算符决定的。

注意：`std::move` 只是产生右值（请求编译器移动），是否真的发生移动，取决于是否有地方消费了这个右值（移动构造/移动赋值）

### 移动条件和时机分析

```cpp
template<typename T>  
void foo1(T& x) {  
    // 不接受右值  
};  
  
template<typename T>  
void foo2(const T& x) {  
    std::cout << "foo2\n";  
    T tmp{x};  
};  
  
template<typename T>  
void foo3(T&& x) {  
    std::cout << "foo3\n";  
    T tmp{x};  
};  
  
template<typename T>  
void foo4(T&& x) {  
    std::cout << "foo4\n";  
    T tmp{std::forward<T>(x)};  
};  
  
struct Moveable {  
    Moveable(Moveable&&) { std::cout << "moving\n"; }  
    Moveable(const Moveable& ) { std::cout << "copying\n"; }  
    Moveable() {}  
};  
  
int main(int argc, char* argv[]) {  
    Moveable m1;  
    // foo1(std::move(m1));  // error: no matching function for call to 'foo1'  
    Moveable m2;  
    foo2(std::move(m2));  // copying
    Moveable m3;  
    foo3(std::move(m3));  // copying
    Moveable m4;  
    foo4(std::move(m4));  // moving
    return 0;
}
```

### 结论

- **即使函数参数是右值引用，也不一定会发生移动**
- **只有在“右值被用来初始化 / 赋值对象，并匹配到移动构造 / 移动赋值”时，才会发生移动**
- **在转发引用（`T&&`）中，直接用形参构造对象会走拷贝**
- **要想保留调用者的值类别，必须使用 `std::forward<T>`**

## `std::forward`

### 背景

模板函数的函数体中无法知道模板参数的值类别，因为函数型参都是左值。

```cpp
template<typename T>
void bar(T && arg) {
    // arg 永远是左值，因为它是一个变量
}
```

需要一种机制：能够根据实参的值类型，来将形参 arg 转成相同的值类型。

1. 能够记录实参的值类型 => [[Forwarding Reference & Universal Reference]]
2. 将形参 arg 转成相同的值类型 => static_cast

因此， **`std::forward ` 必须当形参是 Forwarding Reference 才有用**。


