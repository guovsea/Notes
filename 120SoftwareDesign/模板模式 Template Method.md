---
tags:
  - design/pattern
  - type/concept
mastery: 5
---
## 概述

**动机**

- 对于一项任务，常常有稳定的框架结构，但是子步骤会有一些变化。

Template Method 是一种非常基础的设计模式。定义一个操作中的算法框架（稳定），将一些步骤延迟到子类中。Template Method 使得子类可以复用一个算法的框架，即：子类可以 override 该算法的某些特定步骤。

## 代码

### 初始代码

```cpp
//程序库开发人员
class Library {
public:
    void Step1() {
        //...
    }

    void Step3() {
        //...
    }

    void Step5() {
        //...
    }
};

```
```cpp
//应用程序开发人员
class Application {
public:
    bool Step2() {
        //...
    }

    void Step4() {
        //...
    }
};

int main() {
    Library lib();
    Application app();

    lib.Step1();

    if (app.Step2()) {
        lib.Step3();
    }

    for (int i = 0; i < 4; i++) {
        app.Step4();
    }

    lib.Step5();
}

```
存在的问题

- 违背 [[面相对象设计原则#接口隔离原则（ISP）]]
### 改进代码
```cpp
//程序库开发人员
class Library {
public:
    //稳定的 template method
    void Run() {
        Step1();
        if (Step2()) {  //支持变化 ==> 虚函数的多态调用
            Step3();
        }
        for (int i = 0; i < 4; i++) {
            Step4();  //支持变化 ==> 虚函数的多态调用
        }
        Step5();
    }
    virtual ~Library() {
    }

protected:
    void Step1() {  //稳定
        //.....
    }
    void Step3() {  //稳定
        //.....
    }
    void Step5() {  //稳定
        //.....
    }

    virtual bool Step2() = 0;  //变化
    virtual void Step4() = 0;  //变化
};

```
应用程序
```cpp
//应用程序开发人员
class Application : public Library {
protected:
    virtual bool Step2() {
        //... 子类重写实现
    }

    virtual void Step4() {
        //... 子类重写实现
    }
};

int main() {
    Library* pLib = new Application();
    pLib->Run();

    delete pLib;
}

```
## 总结

类图：

![[模板模式 Template Method.png]]

**要点**

- **它用最简洁的机制（虚函数的多态性）为很多应用程序框架提供了灵活的扩展点，是代码复用方面的基本实现结构**。
- 除了可以灵活应对子步骤的变化外，**“不要调用我，让我来调用你” 的反向控制结构是 Template Method 的典型应用**。
  - 早绑定：Liburary <- Application
  - 晚绑定：Liburary -> Application （调用）

- 在具体实现方面，被 Template Method 调用的虚方法可以具有实现，也可以没有任何实现（抽象方法、纯虚方法），**但一般推荐将它们设置为 protected 方法**。*单独调用没有意义，在整个流程中才有意义*

## Related

- [[设计模式]]
- [[策略模式 Strategy]]
- [[建造者模式 Builder]]

