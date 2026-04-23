---
tags:
  - cpp/basics
  - type/concept
---
#cpp/机制 

## 定义

某个声明有效的那些可能并不连续的程序文本就是该**声明的作用域**

在某个声明的作用域中，能用[[无限定名字查找 unqualified_lookup]] 将名字和它的声明关联起来。

> 换句话说，在某个声明的作用域中，能用不包含限定符的名字引用该声明

> extern static 关键字并不是用来定义变量的作用域的，而是用来定义 [[链接属性 Linkage]] 的

---

> 以下内容较为混乱，网上的资料过于琐碎互相矛盾，还需从 C++ 标准中学习以达到更深入理解

> [cpp/language/ - cppreference.com](https://en.cppreference.com/w/cpp/language/)
> [作用域 - cppreference.com](https://zh.cppreference.com/w/c/language/scope)
## 分类

1. **块作用域（block scope）**
2. **函数作用域（function scope）**
3. **文件作用域（file scope）**
4. **函数原型作用域（Function prototype scope）**
5. **全局作用域（global scope）**
   或者叫全局命名空间作用域（global namespace scope）
5. **类作用域（class scope）**
6. **命名空间作用域（namespace scope）**
7. **模板参数作用域**

### 函数作用域

函数作用域（function scope），**只适用标签（label）的名字**。标签名字可以在它所出现的函数中的任何位置被 goto 语句使用。

```cpp
void f() {  
   goto label; // 如果是普通名字，那么只有在声明名字之后才能访问，label 不是
label:  
   int i  =3;  
}
```
### 枚举作用域

枚举作用域（enumeration scope）是指**枚举类型的成员**（enumerator）的名字的作用域。C 语言规定，枚举类型的成员（enumerator）的可见范围被提升至**该枚举类型所在的作用域**内。*因此，枚举作用域就是枚举类型的作用域* 。这被认为有可能污染了外部的作用域。为此，C++11 引入了 [[scopled enum (enum class)]] 解决此问题

### 模板参数作用域

- [ ] 模板参数作用域
### 函数原型作用域

[c++ - Example of function prototype scope - Stack Overflow](https://stackoverflow.com/questions/24393273/example-of-function-prototype-scope)

Here is a simple example  
这是一个简单的例子

```cpp
int a;

void f( int a, int a );
```

The compiler will issue an error for the second parameter `a` because its name coincides with the name of the first parameter. That is the compiler will report that there is a redefinition of the name `a`. The same name is defined twice in the same scope.  

编译器将对第二个参数发出错误， `a` 因为它的名称与第一个参数的名称一致。也就是说，编译器将报告名称 `a` 的重新定义。同一名称在同一作用域中定义两次。

Or another example 或者另一个例子

```cpp
struct A {};
// 在这之前 struct A 表示一个类型
void f( int A, struct A ); // 在函数原型作用域中 struct A 表示一个类（struct）类型的变量，名字为 A
```

The first parameter name hides the structure name so the second parameter is defined using the elaborated name of the structure.  

第一个参数名称隐藏结构名称，因此使用结构的详细名称定义第二个参数。

### 类作用域

类（class、struct、union）内定义的名字的作用域称为类作用域（class scope）。这些名字在当前类的定义内部，以及类定义词法范围外的类成员定义中是可见的。因此，在类内部，成员名字可以先使用后定义，不必前向声明（forward declaration）。

类静态数据成员具有外部[[链接属性 Linkage]]。

类的成员名字在其所在的类作用域内、或者派生类作用域内可见，或者通过 `.` 运算符、`->` 运算符、`::` 限定符访问。

### 文件作用域

文件作用域（file scope）是从名字声明之处直至该[[编译单元 Translation unit]]结束之处

C++引入了无名命名空间（unnamed namespace），其作用域即为当前编译单元。例如：

namespace {int i;}

也可以通过前面加上`::`限定访问文件作用域中的名字。