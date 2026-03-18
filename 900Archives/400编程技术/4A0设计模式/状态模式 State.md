## 概述

**动机（Motivation）**

- 在软件构建过程中，某些对象的状态如果改变，其行为也会随之而发生变化，比如文档处于只读状态，其支持的行为和读写状态支持的行为就可能完全不同。
- 如何在运行时根据对象的状态来透明的更改对象的行为？而不会为对象操作和状态转化之间引入紧耦合？

**模式定义**

> 允许一个对象在其内部状态改变时改变它的行为。从而使对象看起来似乎修改了其行为。——《设计模式》GoF

## 代码

### 初始代码

```cpp
enum NetworkState {
    Network_Open,
    Network_Close,
    Network_Connect,
};

class NetworkProcessor {
    NetworkState state;

public:
    void Operation1() {
        if (state == Network_Open) {
            //**********
            state = Network_Close;
        } else if (state == Network_Close) {
            //..........
            state = Network_Connect;
        } else if (state == Network_Connect) {
            //$$$$$$$$$$
            state = Network_Open;
        }
    }

public:
    void Operation2() {
        if (state == Network_Open) {
            //**********
            state = Network_Connect;
        } else if (state == Network_Close) {
            //.....
            state = Network_Open;
        } else if (state == Network_Connect) {
            //$$$$$$$$$$
            state = Network_Close;
        }
    }

public:
    void Operation3() {
    }
};

```
### 重构代码

```cpp
class NetworkState {
public:
    NetworkState* pNext;
    virtual void Operation1() = 0;
    virtual void Operation2() = 0;
    virtual void Operation3() = 0;

    virtual ~NetworkState() {
    }
};

class OpenState : public NetworkState {
    static NetworkState* m_instance;

public:
    // 类似单例模式
    static NetworkState* getInstance() {
        if (m_instance == nullptr) {
            m_instance = new OpenState();
        }
        return m_instance;
    }

    void Operation1() {
        //**********
        pNext = CloseState::getInstance();
    }

    void Operation2() {
        //..........
        pNext = ConnectState::getInstance();
    }

    void Operation3() {
        //$$$$$$$$$$
        pNext = OpenState::getInstance();
    }
};

class CloseState : public NetworkState {};
//...

class NetworkProcessor {
    NetworkState* pState;

public:
    NetworkProcessor(NetworkState* pState) {
        this->pState = pState;
    }

    // 用链表的形式不断往后走
    void Operation1() {
        //...
        pState->Operation1();
        pState = pState->pNext;
        //...
    }

    void Operation2() {
        //...
        pState->Operation2();
        pState = pState->pNext;
        //...
    }

    void Operation3() {
        //...
        pState->Operation3();
        pState = pState->pNext;
        //...
    }
};

```
**只关心当前模式的下一个模式**
## 总结


**结构**

![[状态模式 State.png]]

**要点总结**

- State 模式将所有与一个特定状态相关的行为都放入一个 State 的子类对象中，在对象状态切换时，切换相应的对象；但同时维持 State 的接口，这样实现了具体操作与状态转换之间的解耦。
- 为不同的状态引入不同的对象使得状态转换变得更加明确，而且可以保证不会出现状态不一致的情况，因为转换是原子性的——即要么彻底转换过来，要么不转换。
- 如果 State 对象没有实例变量，那么各个上下文可以共享同一个 State 对象，从而节省对象开销。