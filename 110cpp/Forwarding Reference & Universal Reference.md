---
tags:
  - cpp/概念
  - cpp/技巧
---
## 定义

变量的类型是 `T&&` ，并且在 template deduction 语境下 T 是一个模板参数，则该变量是一个**Forwarding Reference**

[Reference declaration - cppreference.com](https://en.cppreference.com/w/cpp/language/reference.html#Forwarding_references)

[Universal References in C++11 -- Scott Meyers : Standard C++](https://isocpp.org/blog/2012/11/universal-references-in-c11-scott-meyers)

## 作用

可以绑定左值和右值

## 原理

1. Forwarding Reference 的特殊推导推规则
2. 引用折叠

```cpp
template<typename T>
void f(T&& arg) {

}
f(expr);
```

Forwarding Reference 推导规则：

**参数为左值时，Forwarding Reference  中的 T 被推导为 A&，为右值时被推导为 A**


```
expr : A
    右值 => T : A
    左值 => T : A&
```

引用折叠

```
expr : A
    右值 => 
        带入 T = A 得到：f(A &&)，折叠为 f(A&&) 可绑定右值
    左值 =>
        带入 T = A& 得到：f(A& &&)，折叠为 f(A&) 可绑定左值
```



## 引用折叠规则

**右值引用的右值引用折叠为右值引用，其余情况折叠为左值引用**

> 主要在 typedef 语境下