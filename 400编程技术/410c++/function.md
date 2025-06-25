---
tags:
  - cpp/STL
---
## std::ref

原理：调用`std::ref(t)`时，会生成一个 ref 对象，ref 对象持有传入对象 t 的地址，并且重载了 ref 对象到对象所属类型 T 的**类型转换运算符**为：返回这个对象。当需要 T 类型的对象时，并且传入的是 ref 对象，则会返回 ref 所绑定的对象。

以值传递的形式传递 ref 对象， 就会传递 ref 所绑定对象的地址。
```cpp
template <class _Tp>
class _LIBCPP_TEMPLATE_VIS reference_wrapper
    : public __weak_result_type<_Tp>
{
public:
    // types
    typedef _Tp type;
private:
    type* __f_;

public:
    _LIBCPP_INLINE_VISIBILITY reference_wrapper(type& __f) _NOEXCEPT
        : __f_(_VSTD::addressof(__f)) {}
    private: reference_wrapper(type&&); public: // = delete; // do not bind to 
    // 重载类型转换运算符
    operator type&    () const _NOEXCEPT {return *__f_;}
    type& get() const _NOEXCEPT {return *__f_;}
```

## std::mem_fn

顾名思义：保存 T 类型的 member function，使得可以像调用普通函数一样 std:: mem_fund 对象。传入的参数为： **T 类型的对象**和 **mem_function 所需的参数**，

原理：

成员函数和普通函数的区别是：成员函数都有一个隐藏的函数指针，要想调用成员函数则必须传入对象的`this`指针。

```cpp
class X {
public:
void f(int a ) { std::cout << a << std::endl; }
};
int main(int argc, char const *argv[]) {
    X x;
    x.f(3);
    void (X::*fptr)(int) = &X::f;      // 声明成员函数指针
    (x.*fptr)(4);                      // 只能这么调用成员函数指针
    auto mf = std::mem_fn(&X::f);
    mf(x, 5);
    std::invoke(&X::f, x, 4);        // 使用 std::invoke 调用成员函数 
}
```

std:: mem_fn 底层会调用 std::invoke
```cpp
// LLVM
template <class _Tp>
class __mem_fn
    : public __weak_result_type<_Tp>
{
public:
    // types
    typedef _Tp type;
private:
    type __f_;             // 保存成员函数

public:
    _LIBCPP_INLINE_VISIBILITY __mem_fn(type __f) _NOEXCEPT : __f_(__f) {}

#ifndef _LIBCPP_CXX03_LANG
    // invoke
    template <class... _ArgTypes>
    _LIBCPP_INLINE_VISIBILITY
    typename __invoke_return<type, _ArgTypes...>::type
    // 重载调用运算符，将所有的参数全都转发给 invoke，让 invoke 调用成员函数
    operator() (_ArgTypes&&... __args) const {
        return __invoke(__f_, _VSTD::forward<_ArgTypes>(__args)...);
    }
#else
```
## std::function

类模板 `std::function` 是通用多态函数包装器。 `std::function` 的实例能存储、复制及调用任何可复制构造的**可调用对象**

- 函数（通过其指针）
- lambda 表达式
- bind 表达式
- **指向成员函数指针**和**指向数据成员指针**


**存储到` std::bind `调用的结果**

*std::bind* 可以绑定函数的参数[[bind]]

```cpp
    std::function<void()> f_display_31337 = std::bind(print_num, 31337);
    f_display_31337();
```

**指向成员函数指针**

```cpp
    // 存储到成员函数的调用
    std::function<void(const Foo&, int)> f_add_display = &Foo::print_add;
    const Foo foo(314159);
    f_add_display(foo, 1);
    f_add_display(314159, 1);
    
    // 存储到成员函数及对象的调用
    using std::placeholders::_1;
    std::function<void(int)> f_add_display2 = std::bind( &Foo::print_add, foo, _1 );
    f_add_display2(2);
    
    // 存储到成员函数和对象指针的调用
    std::function<void(int)> f_add_display3 = std::bind( &Foo::print_add, &foo, _1 );
    f_add_display3(3);
 
    // 存储到函数对象的调用
    std::function<void(int)> f_display_obj = PrintNum();
    f_display_obj(18);
```

可以得出一个结论：*成员函数的参数包括class对象*。要么调用成员函数时传入类对象，要么使用bind绑定。


**指向数据成员指针**

```cpp
    // 存储到数据成员访问器的调用
    std::function<int(Foo const&)> f_num = &Foo::num_;
    std::cout << "num_: " << f_num(foo) << '\n';
```

这样就可以封装一个返回类成员的函数

**示例代码**
```cpp
#include <functional>
#include <iostream>
 
struct Foo {
    Foo(int num) : num_(num) {}
    void print_add(int i) const { std::cout << num_+i << '\n'; }
    int num_;
};
 
void print_num(int i)
{
    std::cout << i << '\n';
}
 
struct PrintNum {
    void operator()(int i) const
    {
        std::cout << i << '\n';
    }
};
 
int main()
{
    // 存储自由函数
    std::function<void(int)> f_display = print_num;
    f_display(-9);
 
    // 存储 lambda
    std::function<void()> f_display_42 = []() { print_num(42); };
    f_display_42();
 
    // 存储到 std::bind 调用的结果
    std::function<void()> f_display_31337 = std::bind(print_num, 31337);
    f_display_31337();
 
    // 存储到成员函数的调用
    std::function<void(const Foo&, int)> f_add_display = &Foo::print_add;
    const Foo foo(314159);
    f_add_display(foo, 1);
    f_add_display(314159, 1);
 
    // 存储到数据成员访问器的调用
    std::function<int(Foo const&)> f_num = &Foo::num_;
    std::cout << "num_: " << f_num(foo) << '\n';
 
    // 存储到成员函数及对象的调用
    using std::placeholders::_1;
    std::function<void(int)> f_add_display2 = std::bind( &Foo::print_add, foo, _1 );
    f_add_display2(2);
 
    // 存储到成员函数和对象指针的调用
    std::function<void(int)> f_add_display3 = std::bind( &Foo::print_add, &foo, _1 );
    f_add_display3(3);
 
    // 存储到函数对象的调用
    std::function<void(int)> f_display_obj = PrintNum();
    f_display_obj(18);
 
    auto factorial = [](int n) {
        // 存储 lambda 对象以模拟“递归 lambda ”，注意额外开销
        std::function<int(int)> fac = [&](int n){ return (n < 2) ? 1 : n*fac(n-1); };
        // note that "auto fac = [&](int n){...};" does not work in recursive calls
        return fac(n);
    };
    for (int i{5}; i != 8; ++i) { std::cout << i << "! = " << factorial(i) << ";  "; }
}
```
