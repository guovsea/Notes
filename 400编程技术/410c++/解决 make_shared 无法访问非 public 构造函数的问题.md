---
tags:
  - cpp/技巧
---

[c++ - How do I call ::std::make\_shared on a class with only protected or private constructors? - Stack Overflow](https://stackoverflow.com/questions/8147027/how-do-i-call-stdmake-shared-on-a-class-with-only-protected-or-private-const?rq=1)

## 方法一 make_shared_enabler

派生标准库 `make_shared_enabler`，返回 `std::make_shard<make_shared_enabler>()`，绑定到指向实际类型的指针。

原理：**动态绑定，派生类指针可以赋值给基类指针**

```cpp
#include <memory>

class A
{
public:
    static std::shared_ptr<A> create()
    {
        struct make_shared_enabler : public A {};
        return std::make_shared<make_shared_enabler>();
    }

private:
    A() {}  
};
```
## 方法二  在构造函数中使用 protected 变量

原理：**在构造函数中使用 protected 变量，外部无法使用该 protected 类型，也就用法用来调用 A 的构造函数。
```cpp
#include <memory>
#include <string>

class A {
 protected:
   struct this_is_private;

 public:
   explicit A(const this_is_private &) {}
   A(const this_is_private &, 参数列表) {}

   template <typename... T>
   static ::std::shared_ptr<A> create(T &&...args) {
      return ::std::make_shared<A>(this_is_private{0},
                                   ::std::forward<T>(args)...);
   }

 protected:
   struct this_is_private {
       explicit this_is_private(int) {}
   };

   A(const A &) = delete;
   const A &operator =(const A &) = delete;
};
```
