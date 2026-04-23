---
tags:
  - cpp/basics
  - type/concept
---
- [ ] lean aggregate initialization

## 定义

聚合初始化（Aggregate initialization）是用花括号 `{}` 直接初始化聚合类型对象的初始化方式。

**聚合类型（Aggregate）**需要满足：
- 数组类型，或者满足以下条件的类类型：
  - 没有用户声明的构造函数（C++17 之前）/ 没有用户声明或继承的构造函数（C++20）
  - 没有私有或受保护的非静态数据成员
  - 没有虚函数
  - 没有虚基类

```cpp
struct Point { int x; int y; };
Point p = {1, 2}; // 聚合初始化
```

## C++17 扩展聚合初始化

C++17 放宽了聚合类型的定义，允许基类的聚合初始化：

```cpp
struct Base { int a; };
struct Derived : Base { int b; };
Derived d{{1}, 2}; // C++17: OK
```

## 注意事项：基类构造函数可见性

If the constructor of a base class is non-public, but accessible to a derived class, then you can't use empty braces to initialize an object of the derived type under **`/std:c++17`** mode and later in Visual Studio 2017 and later.

The following example shows C++14 conformant behavior:

```cpp
struct Derived;

struct Base {
    friend struct Derived;
private:
    Base() {}
};

struct Derived : Base {};

Derived d1; // OK. No aggregate init involved.
Derived d2 {}; // OK in C++14: Calls Derived::Derived()
               // which can call Base ctor.
```

In C++17, `Derived` is now considered an aggregate type. It means that the initialization of `Base` via the private default constructor happens directly, as part of the extended aggregate initialization rule. Previously, the `Base` private constructor was called via the `Derived` constructor, and it succeeded because of the `friend` declaration.

The following example shows C++17 behavior in Visual Studio 2017 and later in **`/std:c++17`** mode:

```cpp
struct Derived;

struct Base {
    friend struct Derived;
private:
    Base() {}
};

struct Derived : Base {
    Derived() {} // add user-defined constructor
                 // to call with {} initialization
};

Derived d1; // OK. No aggregate init involved.

Derived d2 {}; // error C2248: 'Base::Base': can't access
               // private member declared in class 'Base'
```



[c++ - Why does aggregate initialization not work anymore since C++20 if a constructor is explicitly defaulted or deleted? - Stack Overflow](https://stackoverflow.com/questions/57271400/why-does-aggregate-initialization-not-work-anymore-since-c20-if-a-constructor)