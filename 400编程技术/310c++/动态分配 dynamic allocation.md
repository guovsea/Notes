---
tags:
  - cpp/语法
---
## 什么是动态分配（dynamic allocation）

在程序运行时动态地分配内存空间的操作，所分配的内存块在堆区中[[堆和栈]]

## 什么时候应该使用动态分配

- 自动对象（automatic object）
- 动态对象（dynamic object）

自动对象和动态分配的对象的区别是：自动对象的存储期（storage duration）是自动存储期（automatic storage duration）（超出 scope 会被销毁）；动态分配对象来存储器是 dynamic storage duration（直到使用 delete 才会被销毁）。

需要动态分配的情况：

- **需要让对象的生存时间超过当前作用域**--在该特定内存位置的特定对象，而不是它的副本。如果可以复制/移动，那应该使用自动对象
- **需要分配很多的内存**

在 C++ 中，动态分配应该使用[[智能指针]]。除非：

## 使用裸指针而非智能指针的场景

（但在 C++ 总是有替代方案，因为 C++ 尽量避免使用指针）

- 需要引用语义，更好的方法是使用引用
- 需要多态性，但更好的方法是使用引用
- 支持传入 `nullptr`，来表示参数是可选的，但更好的方式是使用 `std::optional`
- 解耦编译单元以缩短编译时间 [[PIMP]]
- 需要与 C 库或 C 风格的库进行交互

[c++ - Why should I use a pointer rather than the object itself? - Stack Overflow](https://stackoverflow.com/questions/22146094/why-should-i-use-a-pointer-rather-than-the-object-itself/)

