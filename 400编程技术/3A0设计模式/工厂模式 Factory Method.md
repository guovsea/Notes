## 概述

**动机**

- 在软件系统中，经常面临着创建对象的工作；由于需求的变化，需要创建的对象的具体类型经常变化。

- 如何应对这种变化？如何绕过常规的对象创建方法 (new)，提供一种“封装机制” 来避免客户程序和这种 “具体对象创建工作” 的紧耦合？
## 代码

### 初始代码

```cpp
// FileSplitter.cpp
class ISplitter {
public:
    virtual void split() = 0;
    virtual ~ISplitter() {
    }
};

class BinarySplitter : public ISplitter {

};

class TxtSplitter : public ISplitter {

};

class PictureSplitter : public ISplitter {

};

class VideoSplitter : public ISplitter {
    
};

// MainForm.cpp
class MainForm : public Form {
public:
    void Button1_Click() {
        ISplitter* splitter = new BinarySplitter();  // 依赖具体类
        splitter->split();
    }
};

```
存在问题：`MainForm.cpp`依赖于具体类`BinarySplitter`。

*当 MainForm 要使用其他的 Splitter 是，需要修改所有出现 BinarySplitter 的地方*
### 重构代码

修改思路：

1. 产品基类和工厂基类，都应该为虚基类
2. 实现具体产品类和具体工厂类
3. 在需要创建产品的对象中保存工厂基类对象指针
4. 将 `new ConcreteProject` 修改为`产品基类 *p = 工厂基类对象->create();` *多态 new*


ISplitterFactory.cpp

```cpp
// 产品基类
class ISplitter {
public:
    virtual void split() = 0;
    virtual ~ISplitter() {
    }
};

// 工厂基类
class SplitterFactory {
public:
    // 虚函数，延迟到运行时依赖
    virtual ISplitter* CreateSplitter() = 0;
    virtual ~SplitterFactory() {
    }
};

```
FileSplitter.cpp

```cpp
// 具体产品
class BinarySplitter : public ISplitter {

};

class TxtSplitter : public ISplitter {
    
};

class PictureSplitter : public ISplitter {

};

class VideoSplitter : public ISplitter {
    
};

// 具体工厂
class BinarySplitterFactory : public SplitterFactory {
public:
    virtual ISplitter* CreateSplitter() {
        return new BinarySplitter();
    }
};

class TxtSplitterFactory : public SplitterFactory {
public:
    virtual ISplitter* CreateSplitter() {
        return new TxtSplitter();
    }
};

class PictureSplitterFactory : public SplitterFactory {
public:
    virtual ISplitter* CreateSplitter() {
        return new PictureSplitter();
    }
};

class VideoSplitterFactory : public SplitterFactory {
public:
    virtual ISplitter* CreateSplitter() {
        return new VideoSplitter();
    }
};

```
MainForm.cpp

```cpp
// MainForm 没有具体类的依赖
// 只依赖于抽象工厂和抽象产品
// 把依赖具体类转移了
class MainForm : public Form {
    SplitterFactory* factory;  // 工厂

public:
    MainForm(SplitterFactory* factory) {
        this->factory = factory;
    }

    void Button1_Click() {
        ISplitter* splitter = factory->CreateSplitter();  // 多态new
        splitter->split();
    }
};

```
## 总结

![[工厂模式 Factory Method.png]]

- Factory Method 模式用于隔离类对象的使用者和具体类型之间的耦合关系。面对一个经常变化的具体类型，紧耦合关系 (new) 会导致软件的脆弱。

- Factory Method 模式通过面向对象的手法，将所要创建的具体对象工作**延迟**到子类，从而实现一种扩展（而非更改）的策略，较好地解决了这种紧耦合关系。

- Factory Method 模式解决 “**单个对象**” 的需求变化。**缺点在于要求创建方法的参数相同**。