---
tags:
  - design/pattern
  - type/concept
mastery: 5
---
## 概述


**动机（Motivation）**

- 在软件系统采用纯粹对象方案的问题在于大量细粒度的对象会很快充斥在系统中，从而带来很高的运行时代价——主要指内存需求方面的代价。
- 如何在避免大量细粒度对象问题的同时，让外部客户程序仍然能够透明地使用面向对象的方式来进行操作？

**模式定义**

> 运用共享技术有效地支持大量细粒度的对象。——《设计模式》GoF


**关键**：共享

## 代码

```cpp
class Font {
private:
    // unique object key
    string key;

    // object state
    //....

public:
    Font(const string& key) {
        //...
    }
};

class FontFactory {
private:
    map<string, Font*> fontPool;

public:
    Font* GetFont(const string& key) {
        map<string, Font*>::iterator item = fontPool.find(key);

        if (item != fontPool.end()) {
            return item->second;
        } else {
            Font* font = new Font(key);
            fontPool[key] = font;
            return font;
        }
    }

    void clear() {
        //...
    }
};

```

## 总结

**结构**

![[享元模式 FlyWidget.png]]


享元模式其实在一些语言的编译器内部有自己的实现

比如 java 的字符串常量池，可能实现方式不同，但是思想是一样的

### 要点总结

- 面向对象很好的解决了抽象性的问题，但是作为一个运行在机器中的程序实体，我们需要考虑对象的代价问题。Flyweight 主要解决面向对象的代价问题，一般不触及面向对象的抽象性问题。
- Flyweight 采用对象共享的做法来降低系统中对象的个数，从而降低细粒度对象给系统带来的内存压力。在具体实现方面，要注意对象状态的处理。**因为对象要被共享使用，对象一般是只读的**
- 对象的数量太大从而导致对象内存开销加大——什么样的数量才算大？这需要我们仔细的根据具体应用情况进行评估，而不能凭空臆断。

## Related

- [[设计模式]]
- [[单例模式 Singleton]]
- [[组合模式 Composite]]