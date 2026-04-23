---
tags:
  - algorithm/linked-list
  - type/snippet
mastery: 5
---

## 相关

- [[206. 反转链表]]
- [[82. 删除排序链表中的重复元素 II]]

反转链表

```cpp
ListNode* reverse(ListNode* head) {
    ListNode *prev = nullptr;
    ListNode *curr = head;
    while (curr != nullptr) {
        ListNode* next = curr->next;
        curr->next = prev;
        prev = curr;
        curr = next;   
    }
    return prev;
}
```

要达能一气呵成默写出来的

K组反转链表

```cpp
ListNode* reverseKGroup(ListNode* head, int k) {
    ListNode* dummy = new ListNode();
    dummy->next = head;
    // end从pre(被反转序列的前一个节点)开始遍历，类似于从0开始遍历0-1-2-3
    ListNode* pre = dummy;
    ListNode* end = pre;
    while (true) {
        // 找到被反转的链表的2个端点
        for (int i = 0; i < k && end != nullptr; i++) {
            end = end->next;
        }
        if (end == nullptr) {
            break;  // 已经到达链表尾部
        }
        // 反转序列的第一个节点
        ListNode* start = pre->next;
        // 记录反转序列之后的第一个节点
        ListNode* next = end->next;
        // ?
        end->next = nullptr;
        reverse(start);
        pre->next = end;
        start->next = next;
        pre = start;
        end = pre;
    }
    return dummy->next;
}
void reverse(ListNode* head) {
    ListNode *prev = nullptr;
    ListNode *curr = head;
    while (curr != nullptr) {
        ListNode* next = curr->next;
        curr->next = prev;
        prev = curr;
        curr = next;   
    }
    // return prev;
}
```

图示

![[WechatIMG58.jpeg]]

步骤
```
构造dummy节点

初始化pre、end（实际上是为了在while里初始化end）

while True

    为end赋值（for循环）
    
    判断end是否为nullptr，是否达到链表尾部
    
    初始化start，作为反转链表的头部
    
    初始化next，记录反转序列后一个节点
    
    反转start为头节点的链表
    
    将反转之后的链表放接到整个链表上

    恢复初始状态，方便下一次反转
    
返回dummy->next
```