---
tags:
  - design/pattern
  - type/concept
mastery: 5
---

## 一、核心概念

### 1.1 工厂方法的作用

> Creator 类声明了工厂方法，该方法返回新的产品对象。该方法的返回类型必须匹配**产品接口（Product Interface）**。

```cpp
// 产品接口
class Product {
public:
    virtual ~Product() = default;
    virtual std::string Operation() const = 0;
};

// Creator 声明工厂方法
class Creator {
public:
    virtual ~Creator() = default;
    // 工厂方法——返回类型是 Product 接口（指针）
    virtual Product* FactoryMethod() const = 0;
};
```

**关键点**：工厂方法的返回类型是**抽象接口指针**，而不是具体类指针。这样调用者不需要知道具体创建了哪个类。

---

### 1.2 两种实现方式

> 你可以将工厂方法声明为**纯虚函数**，强制所有子类实现自己的版本。或者，基类的工厂方法可以返回某个**默认产品类型**。

**方式一：纯虚函数（强制子类实现）**

```cpp
class Creator {
public:
    virtual Product* FactoryMethod() const = 0; // 子类必须实现
};
```

**方式二：提供默认实现**

```cpp
class Creator {
public:
    virtual Product* FactoryMethod() const {
        return new DefaultProduct(); // 返回一个默认产品
    }
};
```

---

### 1.3 最核心的观点 ⭐

> 尽管名字叫"工厂"，但创建产品并不是 Creator 的主要职责。Creator 类通常已经有一些与产品相关的**核心业务逻辑**。工厂方法的作用是将这些逻辑与具体的产品类**解耦**。

```
❌ 错误理解：Creator 只是用来创建对象的
✅ 正确理解：Creator 有自己的核心业务逻辑，工厂方法只是辅助解耦的手段
```

---

### 1.4 类比

| 类比 | 对应模式中的角色 |
|------|----------------|
| 软件开发公司 | Creator 类 |
| 培训部门 | 工厂方法 |
| 程序员 | Product（产品） |
| 写代码（主要业务） | Creator 的核心业务逻辑 |
| 培养程序员（辅助功能） | 创建产品对象 |

> 公司需要程序员来写代码（核心业务），培训部门只是**提供合适的人才**。同样，Creator 需要产品来执行业务逻辑，工厂方法只是**提供合适的产品实例**。

---

## 二、工厂方法的本质

### 2.1 它解决的问题到底是什么？

```
工厂方法要解决的问题 ≠ "怎么创建对象"
工厂方法要解决的问题 = "谁来决定创建哪个对象"
```

### 2.2 没有工厂方法时 ❌

```cpp
class DocumentEditor {
public:
    void EditDocument() {
        // 客户代码直接绑死了具体的产品类型
        PdfDocument* doc = new PdfDocument();  // 耦合！
        doc->Open();
        doc->Save();
        delete doc;
    }
};
```

> 客户代码（业务逻辑）和具体产品**紧耦合**，想换一个产品类型就要改业务代码。

### 2.3 有工厂方法后 ✅

```cpp
class DocumentEditor {
public:
    void EditDocument() {
        // 客户代码只认识 Product 接口，不关心具体是哪个类
        Document* doc = CreateDocument();
        doc->Open();
        doc->Save();
        delete doc;
    }

    virtual Document* CreateDocument() const = 0;  // 工厂方法
};
```

> **"创建什么"这个决定被推迟到了子类**，客户代码完全不受影响。

---

## 三、完整 C++ 示例

### 3.1 产品接口与具体产品

```cpp
#include <iostream>
#include <string>

// ==================== 产品接口 ====================
class Document {
public:
    virtual ~Document() = default;
    virtual void Open() const = 0;
    virtual void Save() const = 0;
};

// ==================== 具体产品：PDF ====================
class PdfDocument : public Document {
public:
    void Open() const override {
        std::cout << "打开 PDF 文档" << std::endl;
    }
    void Save() const override {
        std::cout << "保存 PDF 文档" << std::endl;
    }
};

// ==================== 具体产品：Word ====================
class WordDocument : public Document {
public:
    void Open() const override {
        std::cout << "打开 Word 文档" << std::endl;
    }
    void Save() const override {
        std::cout << "保存 Word 文档" << std::endl;
    }
};

// ==================== 具体产品：Markdown ====================
class MarkdownDocument : public Document {
public:
    void Open() const override {
        std::cout << "打开 Markdown 文档" << std::endl;
    }
    void Save() const override {
        std::cout << "保存 Markdown 文档" << std::endl;
    }
};
```

### 3.2 Creator 基类与具体 Creator

```cpp
// ==================== Creator 基类 ====================
class DocumentEditor {
public:
    virtual ~DocumentEditor() = default;

    // 核心业务逻辑 —— 与具体产品解耦
    void EditDocument() const {
        // 工厂方法提供产品实例，业务逻辑不关心具体类型
        Document* doc = CreateDocument();
        std::cout << "=== 开始编辑 ===" << std::endl;
        doc->Open();
        doc->Save();
        std::cout << "=== 编辑结束 ===" << std::endl;
        delete doc;
    }

    // 工厂方法
    virtual Document* CreateDocument() const = 0;
};

// ==================== 具体 Creator：PDF 编辑器 ====================
class PdfEditor : public DocumentEditor {
public:
    Document* CreateDocument() const override {
        return new PdfDocument();
    }
};

// ==================== 具体 Creator：Word 编辑器 ====================
class WordEditor : public DocumentEditor {
public:
    Document* CreateDocument() const override {
        return new WordDocument();
    }
};

// ==================== 具体 Creator：Markdown 编辑器 ====================
class MarkdownEditor : public DocumentEditor {
public:
    Document* CreateDocument() const override {
        return new MarkdownDocument();
    }
};
```

### 3.3 客户端调用

```cpp
int main() {
    // 创建 PDF 编辑器
    DocumentEditor* editor = new PdfEditor();
    editor->EditDocument();
    delete editor;

    std::cout << std::endl;

    // 切换为 Word 编辑器，客户代码（EditDocument）完全不变
    editor = new WordEditor();
    editor->EditDocument();
    delete editor;

    return 0;
}
```

**输出：**

```
=== 开始编辑 ===
打开 PDF 文档
保存 PDF 文档
=== 编辑结束 ===

=== 开始编辑 ===
打开 Word 文档
保存 Word 文档
=== 编辑结束 ===
```

---

## 四、结构图

```
┌─────────────────────┐          ┌──────────────────┐
│      «abstract»     │          │    «interface»   │
│      Creator        │─────────►│     Product      │
├─────────────────────┤  uses    ├──────────────────┤
│ + FactoryMethod()   │          │ + Operation()    │
│                     │          │    : Product      │
│ + SomeOperation()   │          └──────────────────┘
│    （核心业务逻辑）   │                 ▲
└─────────┬───────────┘                 │ implements
          ▲ extends            ┌────────┴────────┐
   ┌──────┴───────┐             │                 │
   │              │     ┌───────┴──┐    ┌─────────┴──┐
┌──┴──────────┐  …    │Concrete   │    │Concrete     │
│Concrete     │        │Product A  │    │Product B    │
│Creator A    │        └──────────┘    └─────────────┘
│             │
│FactoryMethod│──► new ConcreteProductA()
└─────────────┘
```

---

## 五、对应的设计原则

### 5.1 单一职责原则（SRP）📐

> 每个类只承担一个职责

```
❌ 没有 FactoryMethod：
   DocumentEditor 既负责业务逻辑，又负责决定创建哪个具体产品
   → 一个类承担了两个职责

✅ 有 FactoryMethod：
   DocumentEditor  → 专注业务逻辑（EditDocument）
   工厂方法/子类   → 专注决定创建哪个产品（CreateDocument）
```

---

### 5.2 开闭原则（OCP）🔄

> 对扩展开放，对修改关闭

```cpp
// ✅ 想支持一种新的文档类型？
// 只需要新增两个类，DocumentEditor::EditDocument 完全不用改

class TxtDocument : public Document {
    // ... 实现接口
};

class TxtEditor : public DocumentEditor {
    Document* CreateDocument() const override {
        return new TxtDocument();  // 新产品
    }
};
```

> **加一个新产品 = 加子类，老代码一行不动。**

---

### 5.3 依赖倒置原则（DIP）⭐ 最核心

> 高层模块不应依赖低层模块，**两者**（高层和底层模块）都应依赖抽象

```
高层模块（客户代码 / DocumentEditor）
        │
        │ 依赖的是 Document* （抽象指针）
        ▼
低层模块（PdfDocument / WordDocument / MarkdownDocument）
        │
        │ 实现了 Document 接口
        ▼
    Document（抽象接口）
```

```
❌ 没有工厂方法：
   DocumentEditor ──依赖──→ PdfDocument（具体类）

✅ 有工厂方法：
   DocumentEditor ──依赖──→ Document（抽象接口）
   PdfDocument    ──实现──→ Document（抽象接口）
```

> **客户代码只跟抽象接口打交道，从不直接接触具体类。**

---

### 5.4 迪米特法则 / 最少知识原则（LoD）🔌

> 一个对象应该对其他对象保持最少的了解

```cpp
void EditDocument() const {
    // 客户代码不需要知道：
    // - 具体产品类叫什么名字
    // - 产品怎么构造的（需要什么参数）
    // - 产品怎么初始化的
    Document* doc = CreateDocument();  // 给我就行，细节我不关心
    doc->Open();
    doc->Save();
    delete doc;
}
```

> **客户代码只知道"我拿到一个 Document\*"，其余一概不知。**

---

## 六、总结

### 一句话

> **工厂方法 = 客户代码说"我要一个产品"，但不关心"谁来造、怎么造、造哪个"。**

### 设计原则对照表

| 原则 | 体现 |
|------|------|
| **单一职责（SRP）** | 创建逻辑和业务逻辑分家 |
| **开闭原则（OCP）** | 新增产品不改老代码 |
| **依赖倒置（DIP）** ⭐ | 客户代码依赖抽象，不依赖具体类 |
| **最少知识（LoD）** | 客户代码对产品细节一无所知 |

### GoF 原著定义

> *"Define an interface for creating an object, but let subclasses decide which class to instantiate. Factory Method lets a class defer instantiation to subclasses."*
>
> **定义一个创建对象的接口，但让子类决定实例化哪个类。工厂方法使一个类的实例化延迟到其子类。**

## 七、补充示例：文件分割器（Splitter）

这个示例展示了工厂方法在文件分割场景中的应用。

### 初始代码（存在耦合）

```cpp
// FileSplitter.cpp
class ISplitter {
public:
    virtual void split() = 0;
    virtual ~ISplitter() {}
};

class BinarySplitter : public ISplitter {};
class TxtSplitter : public ISplitter {};
class PictureSplitter : public ISplitter {};
class VideoSplitter : public ISplitter {};

// MainForm.cpp
class MainForm : public Form {
public:
    void Button1_Click() {
        ISplitter* splitter = new BinarySplitter();  // 依赖具体类
        splitter->split();
    }
};
```

存在问题：`MainForm.cpp` 依赖于具体类 `BinarySplitter`，当需要使用其他 Splitter 时，需要修改所有出现 BinarySplitter 的地方。

### 重构代码（工厂方法）

```cpp
// 产品基类
class ISplitter {
public:
    virtual void split() = 0;
    virtual ~ISplitter() {}
};

// 工厂基类
class SplitterFactory {
public:
    virtual ISplitter* CreateSplitter() = 0;
    virtual ~SplitterFactory() {}
};

// 具体产品
class BinarySplitter : public ISplitter {};
class TxtSplitter : public ISplitter {};
class PictureSplitter : public ISplitter {};
class VideoSplitter : public ISplitter {};

// 具体工厂
class BinarySplitterFactory : public SplitterFactory {
public:
    virtual ISplitter* CreateSplitter() {
        return new BinarySplitter();
    }
};

// MainForm 没有具体类的依赖，只依赖于抽象工厂和抽象产品
class MainForm : public Form {
    SplitterFactory* factory;
public:
    MainForm(SplitterFactory* factory) : factory(factory) {}
    void Button1_Click() {
        ISplitter* splitter = factory->CreateSplitter();  // 多态 new
        splitter->split();
    }
};
```

## Related

- [[设计模式]]
- [[抽象工厂模式 Abstract Factory]]
- [[原型模式 Prototype]]

