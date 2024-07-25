bind好用且功能强大

## 基本使用方法

`std::bind(函数对象, Args&&... args)`

args可以是参数：

1. 普通参数

2. 占位符
   `_1, _2, ... _n` 在命令空间：`std::placeholders` 中。注意，**bind的args参数列表和函数对象的参数列表是一一对应**的。placeholders表示该位置上的参数在bind返回的函数对象的参数列表的什么位置。
   ```cpp
   void print_num(int first, int second) {
    std::cout << first << "  " << second << std::endl;
    }
    int main()
    {
        using namespace std::placeholders;
        // _2表示这个位置上的参数应该在f的第二个位置
        auto f = std::bind(print_num, _2, _1);
        f(1, 2);   //cout : 2  1
    }
   ```

*==待学习和补充==*

```cpp
#include <functional>
#include <iostream>
#include <memory>
#include <random>
 
void f(int n1, int n2, int n3, const int& n4, int n5)
{
    std::cout << n1 << ' ' << n2 << ' ' << n3 << ' ' << n4 << ' ' << n5 << '\n';
}
 
int g(int n1)
{
    return n1;
}
 
struct Foo
{
    void print_sum(int n1, int n2)
    {
        std::cout << n1 + n2 << '\n';
    }
 
    int data = 10;
};
 
int main()
{
    using namespace std::placeholders;  // 对于 _1, _2, _3...
 
    std::cout << "1) 参数重排序和按引用传递：";
    int n = 7;
    // （ _1 与 _2 来自 std::placeholders ，并表示将来会传递给 f1 的参数）
    auto f1 = std::bind(f, _2, 42, _1, std::cref(n), n);
    n = 10;
    f1(1, 2, 1001); // 1 为 _1 所绑定， 2 为 _2 所绑定，不使用 1001
                    // 进行到 f(2, 42, 1, n, 7) 的调用
 
    std::cout << "2) 使用 lambda 达成相同效果：";
    n = 7;
    auto lambda = [&ncref = n, n](auto a, auto b, auto /* 未使用 */)
    {
        f(b, 42, a, ncref, n);
    };
    n = 10;
    lambda(1, 2, 1001); // 等同于调用 f1(1, 2, 1001)
 
    std::cout << "3) 嵌套 bind 子表达式共享占位符：";
    auto f2 = std::bind(f, _3, std::bind(g, _3), _3, 4, 5);
    f2(10, 11, 12); // 进行到 f(12, g(12), 12, 4, 5); 的调用
 
    std::cout << "4) 以分布绑定随机数生成器：";
    std::default_random_engine e;
    std::uniform_int_distribution<> d(0, 10);
    std::function<int()> rnd = std::bind(d, e); // e 的一个副本存储于 rnd
    for (int n = 0; n < 10; ++n)
        std::cout << rnd() << ' ';
    std::cout << '\n';
 
    std::cout << "5) 绑定指向成员函数指针：";
    Foo foo;
    auto f3 = std::bind(&Foo::print_sum, &foo, 95, _1);
    f3(5);
 
    std::cout << "6) 绑定是指向成员函数指针的 mem_fn：";
    auto ptr_to_print_sum = std::mem_fn(&Foo::print_sum);
    auto f4 = std::bind(ptr_to_print_sum, &foo, 95, _1);
    f4(5);
 
    std::cout << "7) 绑定指向数据成员指针：";
    auto f5 = std::bind(&Foo::data, _1);
    std::cout << f5(foo) << '\n';
 
    std::cout << "8) 绑定是指向数据成员指针的 mem_fn：";
    auto ptr_to_data = std::mem_fn(&Foo::data);
    auto f6 = std::bind(ptr_to_data, _1);
    std::cout << f6(foo) << '\n';
 
    std::cout << "9) 使用智能指针调用被引用对象的成员：";
    std::cout << f6(std::make_shared<Foo>(foo)) << '\n'
              << f6(std::make_unique<Foo>(foo)) << '\n';
}
```