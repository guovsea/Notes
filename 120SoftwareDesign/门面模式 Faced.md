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

**系统间耦合的复杂度**

![[门面模式 Faced.png]]

**动机（Motivation）**

- 上述 A 方案的问题在于组件的客户和组件各种复杂的子系统有了过多的耦合，随着外部客户程序和各子系统的演化，这种过多的耦合面临很多变化的挑战。
- 如何简化外部客户程序和系统间的交互接口？如何将外部客户程序的演化和内部子系统的变化之间的依赖相互解耦？

**模式定义**

> 为子系统的一组接口提供一个一致（稳定）的界面，Facade 模式定义了一个高层接口，这个接口使得这一子系统更加容易使用（复用）。——《设计模式》GoF

## 代码

### 初始代码

多个子系统直接暴露给客户端，客户端需要了解每个子系统的接口：

```cpp
// 子系统A
class SubSystemA {
public:
    void operationA() {
        // ...
    }
};

// 子系统B
class SubSystemB {
public:
    void operationB() {
        // ...
    }
};

// 子系统C
class SubSystemC {
public:
    void operationC() {
        // ...
    }
};

// 客户端直接与多个子系统交互，耦合度高
int main() {
    SubSystemA sysA;
    SubSystemB sysB;
    SubSystemC sysC;

    sysA.operationA();
    sysB.operationB();
    sysC.operationC();
    return 0;
}
```

### 重构代码

引入 Facade 类，为客户端提供统一的简化接口：

```cpp
// 子系统（不变）
class SubSystemA {
public:
    void operationA() { /* ... */ }
};
class SubSystemB {
public:
    void operationB() { /* ... */ }
};
class SubSystemC {
public:
    void operationC() { /* ... */ }
};

// Facade：提供统一的高层接口
class Facade {
private:
    SubSystemA sysA;
    SubSystemB sysB;
    SubSystemC sysC;

public:
    void operation() {
        sysA.operationA();
        sysB.operationB();
        sysC.operationC();
    }
};

// 客户端只需要与 Facade 交互
int main() {
    Facade facade;
    facade.operation();
    return 0;
}
```

## 总结

**结构（Structure）**

![[门面模式 Faced-1.png]]

**要点总结**

- 从客户程序的角度来看，Facade 模式简化了整个组件系统的接口，对于组件内部与外部客户程序来说，达到了一种 “解耦” 的效果——内部子系统的任何变化不会影响到 Facade 接口的变化。
- Facade 设计模式更注重从架构的层次去看整个系统，而不是单个类的层次。Facade 很多时候更是一种架构设计模式。
- Facade 设计模式并非一个集装箱，可以任意的放进任何多个对象。Facade 模式中组件的内部应该是 “相互耦合关系比较大的一系列组件”，而不是一个简单的功能集合。

## Related

- [[设计模式]]
- [[代理模式 Proxy]]
- [[适配器模式 Adapter]]
- [[中介者模式 Mediator]]