---
tags:
  - design/pattern
  - type/concept
mastery: 5
---
## 概述

- 通过 “对象创建” 模式绕开 new，来避免对象创建（new）过程中所导致的紧耦合（依赖具体类），从而支持对象创建的稳定。它是接口抽象之后的第一步工作。
    
- 典型模式
    
    - Factory Method
    - Abstract Factory
    - Prototype
    - Builder

**动机（Motivation）**

- 在软件系统中，有时候面临着 “一个复杂对象” 的创建工作，其通常由各个部分的子对象用一定的算法构成；由于需求的变化，这个复杂对象的各个部分经常面临着剧烈的变化，但是将它们组合在一起的算法却相对稳定。
    
- 如何应对这种变化？如何提供一种 “封装机制” 来隔离出 “复杂对象的各个部分” 的变化，从而保持系统中的 “稳定构建算法” 不随着需求改变而改变？
    

**模式定义**

> 将一个复杂对象的构建与其表示相分离，使得同样的构建过程 (稳定) 可以创建不同的表示(变化)。 ——《设计模式》GoF

## 代码


### 初始代码

直接在客户端中构建复杂对象，构建过程与表示耦合：

```cpp
class House {
public:
    string foundation;
    string walls;
    string roof;
    string interior;
};

// 客户端直接负责构建步骤
House* buildStoneHouse() {
    House* house = new House();
    // 步骤1：打地基
    house->foundation = "Stone Foundation";
    // 步骤2：砌墙（重复4次）
    house->walls = "Stone Walls";
    // 步骤3：根据条件选择屋顶
    house->roof = "Stone Roof";
    // 步骤4：装修
    house->interior = "Basic Interior";
    return house;
}
// 问题：构建算法硬编码在客户端中，无法复用和扩展
```



### 重构代码

```cpp
class House {
    //....
};

class HouseBuilder {
public:
    House* GetResult() {
        return pHouse;
    }
    virtual ~HouseBuilder() {
    }

protected:
    House* pHouse;
    virtual void BuildPart1() = 0;
    virtual void BuildPart2() = 0;
    virtual void BuildPart3() = 0;
    virtual void BuildPart4() = 0;
    virtual void BuildPart5() = 0;
};

class StoneHouse : public House {};

class StoneHouseBuilder : public HouseBuilder {
protected:
    virtual void BuildPart1() {
        // pHouse->Part1 = ...;
    }
    virtual void BuildPart2() {
    }
    virtual void BuildPart3() {
    }
    virtual void BuildPart4() {
    }
    virtual void BuildPart5() {
    }
};

// 构造函数不能调用纯虚函数
// 是静态绑定 (C++基本功)
class HouseDirector {
public:
    HouseBuilder* pHouseBuilder;

    HouseDirector(HouseBuilder* pHouseBuilder) {
        this->pHouseBuilder = pHouseBuilder;
    }

    House* Construct() {
        pHouseBuilder->BuildPart1();

        for (int i = 0; i < 4; i++) {
            pHouseBuilder->BuildPart2();
        }

        bool flag = pHouseBuilder->BuildPart3();

        if (flag) {
            pHouseBuilder->BuildPart4();
        }

        pHouseBuilder->BuildPart5();

        return pHouseBuilder->GetResult();
    }
};

```

## 总结

**结构**

![[建造者模式 Builder.png]]

此模式和`模板方法`非常像，但不同的是在这个过程是含在这个对象的构造的概念中的

因此是属于创建类的设计模式

**注意 C++ 中不能在构造函数中用纯虚函**

## Related

- [[设计模式]]
- [[模板模式 Template Method]]
- [[工厂方法模式（Factory Method Pattern）]]
- [[抽象工厂模式 Abstract Factory]]