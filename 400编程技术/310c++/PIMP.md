PIMP（Pointer to Implementation）的思想是：设计一个接口类，该类中包含一个指向实现类的指针，通过这个指针调用实现类的方法。

## 示例代码

```cpp
// Interface. h
class Interface {
   public:
    Interface();
    ~Interface();
    int publicMethod();

   private:
    class Impl;
    Impl* impl_;
};
```

```cpp
// Interface.h
#include "Interface.h"
#include "Impl.h"

MyInterface::MyInterface() 
    : pimpl_(new Impl()) {}

MyInterface::~MyInterface() {
    delete pimpl_;
}

int MyInterface::publicMethod() {
    return impl_->method();
}
```


```cpp
// Impl.h 
#include <string>
#include <list>
#include "..."

class Interface::Impl {
public:
    int method();
    int method();
private:
    // ...
};
```


```cpp
// Impl.cpp
#include "Impl.h"

int Interface::Impl::method() {
    // ...
}

// 其他 Interface::Impl 类的方法实现
```

## 优点

- 不暴露非 public 的**接口信息**。——不需要将实现类的头文件提供给他人
- 降低接口头文件被修改的可能性，提高编译速度。——添加 `private` 接口只需要改动 `Impl.h` 的头文件，暴露给使用者的 `Interface.h ` 无需改动
  > 提高编译速度：某个头文件被修改，所有 `include` 该头文件的 [[编译单元 Translation unit]] 都会被重新编译。（头文件会在预处理阶段被替换进源文件中）

## 使用智能指针实现PIMP

现代 C++ 应尽量避免使用裸指针，而使用智能指针。
### 使用 unique_ptr 带代替裸指针后出现的问题

`Interface` 拥有 `Impl` 对象的专属所有权，`unique_ptr` 是最自然的选择。如果直接将上述的裸指针替换成 `unique_ptr`：

```cpp
#include <memory>

class Interface {
   public:
    Interface(){}
    int publicMethod(){}

   private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

// main.cpp
int main() {
    Interface i;
}
```

会看到这样的报错：

```shell
In file included from test.cpp:1:
/Library/Developer/CommandLineTools/usr/bin/../include/c++/v1/memory:2364:19: error: invalid application of 'sizeof' to an incomplete type 'Interface::Impl'
    static_assert(sizeof(_Tp) > 0,
                  ^~~~~~~~~~~
/Library/Developer/CommandLineTools/usr/bin/../include/c++/v1/memory:2623:7: note: in instantiation of member function 'std::__1::default_delete<Interface::Impl>::operator()' requested here
      __ptr_.second()#(_#_tmp) ;
       ^
/Library/Developer/CommandLineTools/usr/bin/../include/c++/v1/memory:2577:19: note: in instantiation of member function 'std::__1::unique_ptr<Interface::Impl, std::__1::default_delete<Interface::Impl> >::reset' requested here
  ~unique_ptr() { reset(); }
                  ^
test.cpp:5:5: note: in instantiation of member function 'std::__1::unique_ptr<Interface::Impl, std::__1::default_delete<Interface::Impl> >::~unique_ptr' requested here
```

### 原因分析

问题出在 `Interface` 的析构函数。在没有显式声明析构函数的情况下，编译器会自动合成一个**隐式内联**的析构函数（[[C++中自动合成的函数]]），等效代码如下：

```cpp
#include <memory>

class Interface {
   public:
    Interface(){}
    ~Interface(){} // 编译器默认合成的析构函数！
    int publicMethod(){}

   private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};
```


编译器自动合成的析构函数会进行以下操作：

1. 执行空的析构函数体
2. **按照构造的相反顺序，依次销毁 Interface 的成员** *这是 C++ 语义层面上确定的*
   1. 销毁 `unique_ptr<Impl> impl_` 成员
      1. 调用 unique_ptr 的析构函数
         1. unique_ptr 的析构函数调用默认的删除器（`delete`），删除它所指向的 `Impl` 对象
    2. 销毁其他成员

我们所看到报错发生在编译期，因为未通过 `static_assert(sizeof(_Tp) > 0` 。具体发生在 `unique_ptr` 删除它所指向的 `Impl` 对象时，因为此时的 `Impl` 还不是完整类型的对象。 [[unique_ptr#默认删除器]]

### 解决方法

**在 `Impl` 定义对编译器可见的地方使用定义析构函数**

```cpp
// Interface.h 
#include <memory>

class Interface {
   public:
    Interface();
    ~Interface();  // 声明析构函数
    int publicMethod();

   private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

```

```cpp
// Interface.cpp
#include <memory>
#include "Interface.h"
#include "Impl.h"

Interface::Interface()
    : impl_(std::make_unique<Impl>()) { }

// 定义析构函数
Interface::~Interface() = default;

int Interface::publicMethod() {
    return impl_->method();
}

```

### 可执行测试

```cpp
#include <memory>

class Interface {
   public:
    Interface() : impl_(std::make_unique<Impl>()){ }
    ~Interface();
    int publicMethod(){}

   private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

struct Interface::Impl{

};

Interface::~Interface() = default;

// main.cpp
int main() {
    Interface i;
}
```

- [ ] 为什么将 unique_ptr 换成 shared_ptr 不会遇到上面的 `static_assert(sizeof(_Tp)>0` 编译错误？