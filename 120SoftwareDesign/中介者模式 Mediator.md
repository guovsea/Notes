---
tags:
  - design/pattern
  - type/concept
mastery: 5
---
## 概述

**”接口隔离” 模式**

- 在组件构建过程中，某些接口之间直接的依赖常常会带来很多问题、甚至根本无法实现。采用添加一层间接（稳定）接口，来隔离本来互相紧密关联的接口是一种常见的解决方案。
- 典型模式
    - Facade
    - Proxy
    - Adapter
    - Mediator

**动机（Motivation）**

- 在软件构建过程中，经常会出现多个对象相互关联交互的情况，对象之间常常会维持一种复杂的引用关系，如果遇到一些需求的更改，这种直接的引用关系将面临不断的变化。
- 在这种情况下，我们可以使用一个 “中介对象” 来管理对象间的关联关系，避免相互交互的对象之间的紧耦合引用关系，从而更好地抵御变化。

**模式定义**

> 用一个中介对象来封装（封装变化）一系列的对象交互。中介者使各对象不需要显式的相互引用（编译时依赖–> 运行时依赖），从而使得其耦合松散（管理变化），而且可以独立地改变它们之间的交互。——《设计模式》GoF

## 代码

### 初始代码

多个组件之间直接相互引用，形成复杂的网状依赖：

```cpp
class ComponentA {
public:
    ComponentB* b;
    ComponentC* c;
    void doSomething() {
        // 直接调用其他组件
        b->onAChanged();
        c->onAChanged();
    }
};

class ComponentB {
public:
    ComponentA* a;
    ComponentC* c;
    void doSomething() {
        a->onBChanged();
        c->onBChanged();
    }
};
// 每增加一个组件，所有现有组件都需要修改
```

### 重构代码

引入中介者，各组件只与中介者交互：

```cpp
class Mediator;

class Component {
public:
    Mediator* mediator;
    virtual void onEvent(const string& event) = 0;
    virtual ~Component() {}
};

class Mediator {
public:
    Component* a;
    Component* b;
    Component* c;

    void notify(Component* sender, const string& event) {
        if (sender == a) {
            b->onEvent(event);
            c->onEvent(event);
        }
        // ...
    }
};

class ComponentA : public Component {
public:
    void doSomething() {
        // 不再直接调用其他组件，而是通知中介者
        mediator->notify(this, "A_changed");
    }
    void onEvent(const string& event) override {
        // 处理来自其他组件的事件
    }
};
```

## 总结

![[中介者模式.png]]

Macondo 中有一个 DataCenter，就对应这里的 "M"

![[中介者模式-1.png]]

**要点总结**

- 将多个对象间复杂的关联关系解耦，Mediator 模式将多个对象间的控制逻辑进行集中管理，变 “多个对象相互关联” 为 “多个对象和一个中介者关联”，简化了系统的维护，抵御了可能的变化。
- 随着控制逻辑的复杂化，Mediator 具体对象的实现可能相当复杂。这个时候可以对 Mediator 对象进行分解处理。
- Facade 模式是解耦系统间（单向）的对象关联关系；Mediator 模式是解耦系统内各个对象之间（双向）的关联关系。

## Related

- [[设计模式]]
- [[门面模式 Faced]]
- [[观察者模式 Observer]]