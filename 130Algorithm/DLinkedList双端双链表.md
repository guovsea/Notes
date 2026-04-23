---
tags:
  - algorithm/linked-list
  - type/snippet
mastery: 5
---

## 相关

- [[LRU最近最少使用]]
- [[707. 设计链表]]

## 结构

节点结构

```c++
struct DLinkedNode {
    int _val;
    DLinkedNode *_prev;
    DLinkedNode *_next;
    DLinkedNode(int val = 0) : _val(val), _prev(nullptr), _next(nullptr) {}
};
```

链表结构

```c++
class DLinkedList {
  private:
    DLinkedNode *_head;
    DLinkedNode *_rear;
  public:
    // 操作函数
};
// 构造函数
DLinkedList::DLinkedList() {
    _head = new DLinkedNode;
    _rear = new DLinkedNode;
    // 用于形成双端链表
    _head->_next = _rear;
    _rear->_prev = _head;
}
```
链表初始化时**new出head节点和rear节点，然后让它们相互指向对方**。这两个节点都不保存数据，但需要动态申请。

DLinkedList.cpp
```c++
#include <iostream>
#include <vector>

struct DLinkedNode {
    int _val;
    DLinkedNode *_prev;
    DLinkedNode *_next;
    DLinkedNode(int val = 0) : _val(val), _prev(nullptr), _next(nullptr) {}
};
class DLinkedList {
  private:
    DLinkedNode *_head;
    DLinkedNode *_rear;

  public:
    DLinkedList();
    ~DLinkedList();
    void insertFirst(int);
    void insertLast(int);
    void dropFirst();
    void dropLast();
    void dropNode(DLinkedNode *);
    DLinkedNode *get(int);
    void displayForward();
    void displayBackward();
};
DLinkedList::DLinkedList() {
    _head = new DLinkedNode;
    _rear = new DLinkedNode;
    // 用于形成双端链表
    _head->_next = _rear;
    _rear->_prev = _head;
}
DLinkedList::~DLinkedList() {
    for (DLinkedNode *n = _rear->_prev; n != _head; n = n->_prev) {
        delete n;
    }
    if (_head) {
        delete _head;
    }
    if (_rear) {
        delete _rear;
    }
}
void DLinkedList::insertFirst(int val) {
    DLinkedNode *node = new DLinkedNode(val);
    node->_next = _head->_next;
    node->_prev = _head;
    _head->_next->_prev = node;
    _head->_next = node;
}
void DLinkedList::insertLast(int val) {
    DLinkedNode *node = new DLinkedNode(val);
    node->_next = _rear;
    node->_prev = _rear->_prev;
    // NOTE 下面两行不能交换位置
    //_rear->_prev = node;
    //_rear->_prev->_next = node;
    _rear->_prev->_next = node;
    _rear->_prev = node;
}
void DLinkedList::dropFirst() {
    if (_head->_next != _rear) {
        DLinkedNode* p = _head->_next->_next;
        delete _head->_next;
        p->_prev = _head; // 怎么能忘记呢！！！ 每次增删一个节点，就需要改变两个其他节点的指针
        _head->_next = p;
    } else {
        std::cout << "DLinkedList empty" << std::endl;
        exit(1);
    }
}
void DLinkedList::dropLast() {
    if (_head->_next != _rear) {
        DLinkedNode* p = _rear->_prev;
        p->_prev->_next = _rear;
        _rear->_prev = p->_prev;
        delete p;
    } else {
        std::cout << "DLinkedList empty" << std::endl;
        exit(1);
    }
}
void DLinkedList::dropNode(DLinkedNode *n) {
    if (n != nullptr && n != _head && n != _rear) {
        n->_prev->_next = n->_next;
        n->_next->_prev = n->_prev;
        delete n;
    }
}
DLinkedNode *DLinkedList::get(int num) {
    DLinkedNode *n = _head;
    for (int i = 0; n != _rear && i < num; n = n->_next, i++) {
        //
    }
    return n;
}
void DLinkedList::displayForward() {
   std::cout <<  "L<->";
    for (DLinkedNode *n = _rear->_prev; n != _head; n = n->_prev) {
        std::cout << n->_val << "<->";
    }
    std::cout <<  "H";
    std::cout << std::endl;
}
void DLinkedList::displayBackward() {
    std::cout <<  "H<->";
    for (DLinkedNode *n = _head->_next; n != _rear; n = n->_next) {
        std::cout << n->_val << "<->";
    }
    std::cout <<  "L";
    std::cout << std::endl;
}

int main(int argc, const char **argv) {
    // 双端双链表
    DLinkedList dl1;
    DLinkedList dl2;
    for (int i = 1; i <= 10; i++) {
        dl1.insertFirst(i);
        dl2.insertLast(i);
    }
    dl1.dropFirst();
    DLinkedNode* n = dl1.get(3);
    dl1.dropNode(n);
    dl1.displayBackward();
    dl1.displayForward();
    dl2.displayForward();
    dl2.displayBackward();
    DLinkedList dl3;
    dl3.displayBackward();
    dl3.displayForward();
    return 0;
}
```