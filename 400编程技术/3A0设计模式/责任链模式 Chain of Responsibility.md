*很少用*
## 概述

**动机（Motivation）**

- 在软件构建过程中，一个请求可能被多个对象处理，但是每个请求在运行时只能有一个接受者，如果显式指定，将必不可少的带来请求发送者与接受者的紧耦合。
- 如何使请求的发送者不需要指定具体的接受者？让请求的接受者自己在运行时决定来处理请求，从而使两者解耦。

**模式定义**

> 使多个对象都有机会处理请求，从而避免请求的发送者和接受者之间的耦合关系。将这些对象连城一条链，并沿着这条链传递请求，直到有一个对象处理它位置。——《设计模式》GoF

*Qt 窗口中的鼠标事件处理机制，沿着窗口层级往上走，直到事件被某个对象处理*

## 代码

```cpp
#include <iostream>
#include <string>

using namespace std;

// 请求类型
enum class RequestType {
    REQ_HANDLER1,
    REQ_HANDLER2,
    REQ_HANDLER3 
};

// 请求
class Request {
    string description;
    RequestType reqType;

public:
    Request(const string &desc, RequestType type)
        : description(desc), reqType(type) {
    }
    RequestType getReqType() const {
        return reqType;
    }
    const string &getDescription() const {
        return description;
    }
};

// 链式处理
class ChainHandler {
    ChainHandler *nextChain;
    void sendReqestToNextHandler(const Request &req) {
        if (nextChain != nullptr)
            nextChain->handle(req);
    }

protected:
    virtual bool canHandleRequest(const Request &req) = 0;
    virtual void processRequest(const Request &req) = 0;

public:
    ChainHandler() {
        nextChain = nullptr;
    }
    void setNextChain(ChainHandler *next) {
        nextChain = next;
    }

    // 不断递归链表识别 req
    void handle(const Request &req) {
        // 子类的 canHandleRequest
        if (canHandleRequest(req)) {
            // 子类的 processRequest
            processRequest(req);
        } else {
            // 本类的 sendReqestToNextHandler
            sendReqestToNextHandler(req);
            /* 等价代码
            if (nextChain != nullptr)
                nextChain->handle(req);
            */
        }
    }
};

class Handler1 : public ChainHandler {
protected:
    bool canHandleRequest(const Request &req) override {
        return req.getReqType() == RequestType::REQ_HANDLER1;
    }
    void processRequest(const Request &req) override {
        cout << "Handler1 is handle Request: " << req.getDescription() << endl;
    }
};

class Handler2 : public ChainHandler {
protected:
    bool canHandleRequest(const Request &req) override {
        return req.getReqType() == RequestType::REQ_HANDLER2;
    }
    void processRequest(const Request &req) override {
        cout << "Handler2 is handle Request: " << req.getDescription() << endl;
    }
};

class Handler3 : public ChainHandler {
protected:
    bool canHandleRequest(const Request &req) override {
        return req.getReqType() == RequestType::REQ_HANDLER3;
    }
    void processRequest(const Request &req) override {
        cout << "Handler3 is handle Request: " << req.getDescription() << endl;
    }
};

int main() {
    // 构造链表
    Handler1 h1;
    Handler2 h2;
    Handler3 h3;
    h1.setNextChain(&h2);
    h2.setNextChain(&h3);

    Request req("process task ... ", RequestType::REQ_HANDLER3);
    // h1为链表头
    h1.handle(req);
    return 0;
}

```

## 总结

**结构**

![[责任链模式 Chain of Responsibility.png]]


**要点**

- Chain Of Resposibility 模式的应用场合在于 “一个请求可能有多个接受者，但是最后真正的接受者只有一个”， 这时候请求发送者接受者的耦合有可能出现 “变化脆弱” 的症状，职责链的目的就是将二者解耦，从而更好的应对变化。
- 应用了 Chain Of Resposibility 模式后，对象的职责分派将更具灵活性。我们可以在运行时动态添加 / 修改请求的处理职责。
- 如果请求传递到职责链的末尾仍得不到处理，应该有一个合理的缺省机制。这也是每一个接受者对象的责任，而不是发出请求的对象的责任。
