---
tags:
  - cpp/语法
  - cpp/STL
---
## 典型实现

在典型的实现中，**std::shared_ptr** 只保有两个指针：

- 所持有对象的指针（`get()` 所返回的指针）
- 指向**控制块**﻿的指针

控制块是一个动态分配的对象，其中包含：

- 指向被管理对象的指针或被管理对象本身
- 删除器（类型擦除）
- 分配器（类型擦除）
- 持有被管理对象的 `shared_ptr` 的数量
- 涉及被管理对象的 `weak_ptr` 的数量

## make_shared

### make_shared 的效率更高

`std::shared_ptr<T>(new T(args...))` 会分配两次内存（一次为 `T` 对象，另一次为共享指针的控制块），而 `std::make_shared<T>` 仅分配一次内存

### 异常安全

使用 `std::shared_ptr<T> (new T)` 在发生异常时可能会资源泄露。

```cpp
f(std::shared_trr<Widget>(new Widget), g());
```

若 `g` 在 `new Widget` 后得到调用且抛出异常, 则会导致内存泄漏。

在这个表达式中，有三个动作：

1. `new Widget`，在堆上创建 Widget 对象
2. 执行 `std::shared_ptr` 的构造函数
3. 执行 `g() `

1 肯定要比 2 先执行，但是 3 和 1、2 执行的顺序并不确定，因为**编译器并不保证函数调用时各个表达式的执行顺序**。执行顺序可能会出现 1 3 2，如果 ` g()` 抛出异常了，则会在堆上创建一个 ` Widget ` 对象，但并没有智能指针管理它，造成了资源泄露。

而是用 `make_shared` 则在是异常安全的。先执行 `make_shared`，再执行 `g()` 时如果 `g()` 抛出异常，智能指针会释放 `Widget `；先执行 ` g() ` 时抛出异常则 ` Widget ` 不会被创建。

```cpp
f(std::make_shared<Widget>(), g()));
```

和使用 `make_shared` 的原因相同，也应该使用 `make_unique`。

### 使用 make_shared 的缺点

*细微缺点，最好还是用 `make_shared`*

[c++ - Are there any downsides with using make\_shared to create a shared\_ptr - Stack Overflow](https://stackoverflow.com/questions/2147600/are-there-any-downsides-with-using-make-shared-to-create-a-shared-ptr)

- **不能调用私有构造函数**

**`make_shared` is not compatible with the factory pattern.** 因为 `make_shared` 会调用 `new`，调用 `new` 的时候需要保证构造函数可见，而 `private` 的构造函数仅在类作用域中可见。[[作用域 Scope#类作用域]]

```cpp
class Factory  {
private:
    Factory(int value) : m_value(value) {}
    int m_value;

public:
    static std::shared_ptr<Factory> create(int value) {
        return std::shared_ptr<Factory>(new Test(value));
        // return std::make_shared<Factory>(3); // error
    }
};
```

解决方法： [[解决 make_shared 无法访问非 public 构造函数的问题]]

- 不能自定义删除器（`shared_ptr` 的构造函数可以）
- `make_shared` 产生的控制块和对象在同一块内存中，当最后一个 `shared_ptr` 销毁但仍然有 `weak_ptr` 指向该对象时，**该对象会被析构，但分配的内存仍然保留直到所有 `weak_ptr` 都被销毁**
  >If you use `weak_ptr` s that live after all the `shared_ptr` s to a given object have died, then this object's memory will live in memory along with the control block until the last weak_ptr dies. In other words the object is destroyed but not deallocated until the last `weak_ptr` is destroyed.

## 循环引用

**不要让两个对象中的 shared_ptr 相互指向**

两个类中都包含 shared_ptr 指针互相指向，使用两个 shared_ptr 在堆上分别创建这两个类对象，它们不会被自动析构。

```cpp
#include <iostream>
#include <memory>
struct A;
struct B;

struct A {
    std::shared_ptr<B> pointer;
    ~A() { std::cout << "A 被销毁" << std::endl; }
};
struct B {
    std::shared_ptr<A> pointer;
    ~B() { std::cout << "B 被销毁" << std::endl; }
};
int main() {
    {
        auto a = std::make_shared<A>();
        auto b = std::make_shared<B>();
        a->pointer = b;
        b->pointer = a;
    }
    std::cout << "into external scope\n";
}
```

运行结果是 A, B 都不会被销毁，这是因为 a,b 内部的 pointer 同时又引用了 `a,b`，这使得 `a,b` 的引用计数均变为了 2，而离开作用域时，`a,b` 智能指针被析构，却只能造成这块区域的引用计数减一，这样就导致了 `a,b` 对象指向的内存区域引用计数不为零，而外部已经没有办法找到这块区域了，也就造成了内存泄露。

![[智能指针.png]]

**解决这个问题的办法就是在其中一个类中使用弱引用指针** [[weak_ptr]]

## 返回拥有 this 指针的 shared_ptr

**直接使用 this 构造的多个 shared_ptr 时，每个 `shared_ptr` 对象都持有独自的引用计数，将导致类对象被多次析构**。因为 this 指针也是普通指针。

```cpp
#include <iostream>
#include <memory>
struct A {
    A() {}
    ~A() { std::cout << "A 被销毁" << std::endl; }
    std::shared_ptr<A> f() { return std::shared_ptr<A>(this); }
};

int main() {
    {
        A *pa = new A;
        auto p1 = pa->f();
        auto p2 = pa->f();
        std::cout << "p1.use_count = " << p1.use_count() << " "
                  << "p2.use_count = " << p2.use_count() << "\n";
    }
    std::cout << "into external scope\n";
}
``` 

执行结果：

```text
p1.use_count = 1 p2.use_count = 1
A 被销毁
A 被销毁
/bin/sh: line 1: 24221 Abort trap: 6  
```

解决方法: 继承 `std::enable_shared_from_this`，需要返回 this 指针时调用 `shared_from_this()` 函数。 [[enable_shared_from_this 原理]]

**注意事项**：**调用 `shared_from_this()` 的对象必须由 `shared_ptr` 对象拥有

```cpp
// ！ error
#include <iostream>
#include <memory>
struct A  : public std::enable_shared_from_this<A> {
    A() {}
    ~A() { std::cout << "A 被销毁" << std::endl; }
    std::shared_ptr<A> f() { return shared_from_this(); }
};

int main() {
    {
        A *pa = new A;
        auto p1 = pa->f();
        auto p2 = pa->f();
        std::cout << "p1.use_count = " << p1.use_count() << " "
                  << "p2.use_count = " << p2.use_count() << "\n";
    }
    // throw：bad_weak_ptr
    std::cout << "into external scope\n";
}
```

### 正确用法

```cpp
#include <iostream>
#include <memory>
struct A  : public std::enable_shared_from_this<A> {
    A() {}
    ~A() { std::cout << "A 被销毁" << std::endl; }
    std::shared_ptr<A> f() { return shared_from_this(); }
};

int main() {
    {
        auto pa = std::make_shared<A>();
        auto p1 = pa->f();
        auto p2 = pa->f();
        std::cout << "p1.use_count = " << p1.use_count() << " "
                  << "p2.use_count = " << p2.use_count() << "\n";
    }
    std::cout << "into external scope\n";
}
```

```text
p1.use_count = 3 p2.use_count = 3
A 被销毁
into external scope
```

