
## 算法思路

- 双端双链表[[DLinkedList双端双链表]]

用于维护最近最少使用信息。最近访问的节点越靠近头部，最久未被访问的越靠近尾部

- 哈希表

用于在O(1)时间内通过key，获得节点，从而获得value。以key为键，node地址为值。

## 算法流程

int get(int key)

- 如果key存在，则通过key获得node信息，返回node->val
- 如果key不存在，则返回-1

void put(int key, int value);

- 如果key存在，则将节点移动到头部
- 如果key不存在，则生成新的节点并插入到头部**并将信息存入hash表**，如果size>capacity则删除最后一个节点**同时删除在哈希表中的键值对**

## 详细流程

要构造一个LRU类，则要**生成一个双链表**并且对size和capacity初始化

操作DLinkedList需要有以下方法：
- 摘除节点（并不delete）
- 在头部插入节点
- 移动节点到头部（摘除再插入头部）
- 得到最后一个节点
- 销毁最后一个节点（摘除后delete）

## 代码

```cpp
struct DLinkedList {
    int _key, _val;
    DLinkedList* _prev;
    DLinkedList* _next;
    DLinkedList(int key = 0, int val = 0) : _key(key), _val(val), _prev(nullptr), _next(nullptr) { }
};

class LRUCache {
public:
    LRUCache(int capacity)  
     : _size(0), _capacity(capacity), _head(nullptr), _rear(nullptr)  {
         _head = new DLinkedList();
         _rear = new DLinkedList();
         _head->_next = _rear;
         _rear->_prev = _head;
    }
    
    int get(int key) {
        if (cache.count(key) > 0) {
            DLinkedList* p = cache[key];
            moveToHead(p);
            return p->_val;
        } else {
            return -1;
        }
    }
    
    void put(int key, int value) {
        if (cache.count(key) > 0) {
            DLinkedList* p = cache[key];
            p->_val = value;
            moveToHead(p);
        } else {
            DLinkedList* p = new DLinkedList(key, value);
            cache[key] = p;
            insertFirst(p);
            _size++;
            if (_size > _capacity) {
                DLinkedList* p = LastNode();
                cache.erase(p->_key);
                destoryLastNode();
                _size--;
            }
        }
    }

    void moveToHead(DLinkedList* node) {
        dropNode(node);
        insertFirst(node);
    }
    void insertFirst(DLinkedList* node) {
        node->_next = _head->_next;
        node->_prev = _head;
        _head->_next->_prev = node;
        _head->_next = node;
    }
    void dropNode(DLinkedList* node) {
        node->_next->_prev = node->_prev;
        node->_prev->_next = node->_next; 
    }
    DLinkedList* LastNode() {
        return _rear->_prev;
    }
    void destoryLastNode() {
        DLinkedList* p = _rear->_prev;
        dropNode(p);
        cache.erase(p->_key);
        delete p;
    }
private:
    DLinkedList* _head;
    DLinkedList* _rear;
    int _size;
    int _capacity;
    unordered_map<int, DLinkedList*> cache;
};

 
```