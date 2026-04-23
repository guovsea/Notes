---
tags:
  - cpp/template
  - type/concept
---
(Curiously Recurring Template Pattern) 奇异递归模板模式

## 什么是 CRTP

- 继承自模板类
- 使用派生类作为基类的模板参数

这样父类就能调用子类的函数（使用 static_cast 将父类的 this 指针为传入的模板参数类型的指针）

```cpp
#include <iostream>

template <typename T>
class Base
{
public:
    void g() {
        static_cast<T*>(this)->f();
    }
};

class Derived : public Base<Derived>
{
public:
    void f(){
        std::cout << "Derived f()\n";
    }
};


int main(int argc, char const *argv[])
{
    Derived d;
    d.g();
    return 0;
}

```

但父类只能调用子类的 public 函数，要想调用非 public 函数是必须得**在子类中将基类声明为 friend**

```cpp
template <typename T>
class Base
{
public:
    void g() {
        static_cast<T*>(this)->f();
    }
};

class Derived : public Base<Derived>
{
    friend class Base<Derived>;
private:
    void f(){
        std::cout << "Derived f()\n";
    }
};
```
## 作用

[c++ - What is the curiously recurring template pattern (CRTP)? - Stack Overflow](https://stackoverflow.com/questions/4173254/what-is-the-curiously-recurring-template-pattern-crtp)

- 可以实现编译器多态，没有虚函数调用的开销
- 父类可以访问子类的所有 public 、protected 方法
- 在父类中定义供多种子类复用的方法

### Singleton template

[[Singleton template]]

### 在父类中定义供多种子类复用的方法

Yet another useful example, for Boost (I am not sure how they have implemented it, but CRTP will do too). Imagine you want to provide only operator < for your classes but automatically operator == for them!

```cpp
template<class Derived>
class Equality
{
};

template <class Derived>
bool operator == (Equality<Derived> const& op1, Equality<Derived> const & op2)
{
    Derived const& d1 = static_cast<Derived const&>(op1);//you assume this works     
    //because you know that the dynamic type will actually be your template parameter.
    //wonderful, isn't it?
    Derived const& d2 = static_cast<Derived const&>(op2); 
    return !(d1 < d2) && !(d2 < d1);//assuming derived has operator <
}
```

这样使用它：

```cpp
struct Apple:public Equality<Apple> 
{
    int size;
};

bool operator < (Apple const & a1, Apple const& a2)
{
    return a1.size < a2.size;
}
```

Now, you haven't provided explicitly operator == for Apple? But you have it! You can write

```cpp
int main()
{
    Apple a1;
    Apple a2; 

    a1.size = 10;
    a2.size = 10;
    if(a1 == a2) //the compiler won't complain! 
    {
    }
}
```