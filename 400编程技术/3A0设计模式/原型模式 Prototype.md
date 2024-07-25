## 概述

**动机（Motivation）**

- 在软件系统中，经常面临着 “某些结构复杂的对象” 的创建工作；由于需求的变化，这些对象经常面临着剧烈的变化，但是他们却拥有比较稳定一致的接口。
- 如何应对这种变化？如何向 “客户程序（使用这些对象的程序）” 隔离出 “这些易变对象”，从而使得“依赖这些易变对象的客户程序” 不随着需求改变而改变？

**模式定义**

使用原型实例指定创建对象的种类，然后通过**拷贝**这些原型来创建新的对象。——《设计模式》GoF
## 代码

### 初始代码

```cpp
// 工厂模式
```
[[工厂模式 Factory Method#重构代码]]

### 重构代码

**让产品自己克隆自己**

原型模式和工厂模式的区别主要在于

原型模式可以直接克隆一个比较复杂的，有过一定变化的状态

而工厂模式是从0开始创建对象

```cpp
// 抽象类
class ISplitter {
public:
    virtual void split() = 0;
    virtual ISplitter* clone() = 0;  // 通过克隆自己来创建对象

    virtual ~ISplitter() {
    }
};

// 具体类
class BinarySplitter : public ISplitter {
public:
    virtual ISplitter* clone() {
        // 通过 拷贝构造函数
        return new BinarySplitter(*this);
    }
};

class TxtSplitter : public ISplitter {
public:
    virtual ISplitter* clone() {
        return new TxtSplitter(*this);
    }
};

class PictureSplitter : public ISplitter {
public:
    virtual ISplitter* clone() {
        return new PictureSplitter(*this);
    }
};

class VideoSplitter : public ISplitter {
public:
    virtual ISplitter* clone() {
        return new VideoSplitter(*this);
    }
};

class MainForm : public Form {
    ISplitter* prototype;  // 先有一个原型对象

public:
    MainForm(ISplitter* prototype) {
        this->prototype = prototype;
    }

    // 此处未考虑内存释放
    void Button1_Click() {
        // 原型对象不是用来使用的
        // 是专门用来克隆的
        ISplitter* splitter = prototype->clone();  // 克隆原型
        splitter->split();
    }
};

```
## 总结