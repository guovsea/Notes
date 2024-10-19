---
tags:
  - cpp/机制
  - cpp/语法
---
## 解决循环引用

`std::weak_ptr`，`std::weak_ptr` 是一种弱引用，其目的是解决循环引用问题。

```cpp
struct A;  
struct B;  
  
struct A {  
    std::shared_ptr<B> pointer;  
    ~A() {  
        std::cout << "A 被销毁" << std::endl;  
    }  
};  
struct B {  
    std::weak_ptr<A> pointer;  
    ~B() {  
        std::cout << "B 被销毁" << std::endl;  
    }  
};  
int main() {  
    auto pa = std::make_shared<A>();  
    auto pb = std::make_shared<B>();  
    pa->pointer = pb;  
    pb->pointer = pa;  
}
```

**弱引用不会引起引用计数增加**，当换用弱引用时候，最终的释放流程如图。

![[智能指针-1.png]]

> A 是被 `weak_ptr` 所指向的，因此 A 的引用计数还是 1。离开作用域时，pa 被销毁，A的引用计数变为 0，A 被析构。
## 使用方法

`weak_ptr` 比较特殊，它主要是为了配合 `shared_ptr` 而存在的。它本身是不能直接调用原生指针的方法的。**如果想要使用原生指针的方法，需要将其先转换为一个 `shared_ptr`**。


```cpp
if(!weak_p.expired())
{
    auto shared_p = weak_p.lock();
    shared_p->doSomething();
    
} else {
    // 对象已经被析构
}


if(std::shared_ptr<A> shared_p = weak_a1.lock())
{
//此时可以通过shared_a进行原生指针的方法调用
    shared_p->doSomething()
    
} else {
    // 对象已经被析构 
}
```


## weak_ptr 不会影响指向对象的析构时机

当对象的引用计数为 0 时，即使有 `weak_ptr` 指向该对象，它也会被析构。

当所有 `weak_ptr` 和 `shared_ptr` 都销毁时，控制块才会被释放， 

> `make_shared` 产生的控制块和对象在同一块内存中，当最后一个 `shared_ptr` 销毁但仍然有 `weak_ptr` 指向该对象时，**该对象会被析构，但分配的内存仍然保留直到所有 `weak_ptr` 都被销毁**

