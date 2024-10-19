## 概述

**动机（Motivation）**

- 在软件构建过程中，由于需求的改变，某些类层次结构中常常需要增加新的行为（方法），如果直接在基类中做这样的更改，将会给子类带来很繁重的变更负担，甚至破坏原有设计。

- 如何在不更改类层次结构的前提下，在运行是根据需要透明的为类层次结构上的各个类动态添加新的操作，从而避免上述问题？

**模式定义**

> 表示一个作用于某对象结构中的各元素的操作。使得可以在不改变（稳定）各元素的类的前提下定义（扩展）作用于这些元素的新操作（变化）。——《设计模式》GoF

## 代码

### 初始代码

```cpp
#include <iostream>
using namespace std;

class Visitor;

class Element {
public:
    virtual void Func1() = 0;

    virtual void Func2(int data) = 0;
    virtual void Func3(int data) = 0;
    //...

    virtual ~Element() {
    }
};

class ElementA : public Element {
public:
    void Func1() override {
        //...
    }

    void Func2(int data) override {
        //...
    }
};

class ElementB : public Element {
public:
    void Func1() override {
        //***
    }

    void Func2(int data) override {
        //***
    }
};

```

### 重构代码

```cpp
#include <iostream>
using namespace std;

class Visitor;

class Element {
public:
    virtual void accept(Visitor& visitor) = 0;  // 第一次多态辨析

    virtual ~Element() {
    }
};

class ElementA : public Element {
public:
    void accept(Visitor& visitor) override {
        visitor.visitElementA(*this);
    }
};

class ElementB : public Element {
public:
    void accept(Visitor& visitor) override {
        visitor.visitElementB(*this);  // 第二次多态辨析
    }
};

class Visitor {
public:
    // 必须得知道 Element 有哪些子类才能确定 Visitor 的结构
    virtual void visitElementA(ElementA& element) = 0;
    virtual void visitElementB(ElementB& element) = 0;

    virtual ~Visitor() {
    }
};

//==================================

// 扩展1
class Visitor1 : public Visitor {
public:
    void visitElementA(ElementA& element) override {
        cout << "Visitor1 is processing ElementA" << endl;
    }

    void visitElementB(ElementB& element) override {
        cout << "Visitor1 is processing ElementB" << endl;
    }
};

// 扩展2
class Visitor2 : public Visitor {
public:
    // 增加的新的行为
    void visitElementA(ElementA& element) override {
        cout << "Visitor2 is processing ElementA" << endl;
    }

    void visitElementB(ElementB& element) override {
        cout << "Visitor2 is processing ElementB" << endl;
    }
};

int main() {
    Visitor v1 = new Visitor1();
    Visitor v2 = new Visitor2();

    Element eA = new ElementA();

    eA.accept(v1);  // 调用方法为 v1 的 ElementA 方法
    eA.accept(v2);  // 调用新方法
    
    Element eB = new ElementB();
 
    eB.accept(v1);  // 调用方法为 v1 的 ElementB 方法
    eB.accept(v2);  // 调用新新方法

    // 老师写的代码， 感觉有点问题
    // Visitor2 visitor;
    // ElementB elementB;
    // elementB.accept(visitor);  // double dispatch

    // ElementA elementA;
    // elementA.accept(visitor);

    return 0;
}

```
Visotor 模式下想给某各个 Element 子类增加新的行为，只需要创建一个新的 Visotor 的派生类，并 override Visotor 中适用于改 Elemenet 子类的方法。
## 总结



**结构**

![[访问者模式 Visotor.png]]


**要点**

- Visitor 模式通过所谓双重分发（double dispatch）来实现在不更改（不添加新的操作 - 编译时）Element 类层次结构的前提下，在运行时透明的为类层次结构上的各个类动态添加新的操作（支持变化）。

- 所谓双重分发即 Visitor 模式中间包括了两个多态分发（注意其中的多态机制）：第一个为 accept 方法的多态辨析；第二个为 visitElementX 方法的多态辨析。

- Visitor 模式的最大缺点在于扩展类层次结构（增添新的 Element 子类），会导致 Visitor 类的改变。因此 Visitor 模式适用于 **“Element 类层次结构稳定，而其中的操作却经常面临频繁改动”**。*Element 类层次结构稳定：Element 的子类数量不变**
  