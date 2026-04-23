---
tags:
  - cpp/basics
  - type/howto
---
## 出现情况

两个类型的头文件互相 `include`，但均未前置声明另一类。

## 前置声明

前面的类型使用到后面的类型时，就要在前面的类型定义之前声明后面的类型，比如：

```cpp

class B;
class A {
     void method(B* )
}

class B {
    void func(A* b);
}

```

一个头文件中容易查看，但两个头文件中就难以察觉了

## 两个头文件中

```cpp
###### A.h ######
#include B.h
class A {
     void method(B* pB)
}

```

```cpp
###### B.h ######
#include A.h
class B {
    void func(A* pA);
}
```

会出现某个类型未定义的报错

```
error: ‘类型’ has not been declared
```

## 结论

1. 头文件不应该互相 include，而是在一个头文件中使用前置声明，在另一个头文件中 include 
2. **包含前置声明的头文件中，需要该声明的类的函数一定要定义在源文件中**

```cpp
###### A.h ######
class B;
class A {
//    void method(B* pB) {
//        pB->func();   // 错误，类型不完整
//    }
    // 只能声明
    void method(B* pB);
    B* m_pB;
}
```

```cpp
###### A.cpp ######
void A::method(B* pB)
{
    pB->func(this);
}
```

```cpp
###### B.h ######
#include A.h
class B {
    void func(A* pA);
}
```
