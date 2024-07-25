每个STL容器都有一下某些类别的接口

- 迭代器
- 容量
- 修改器
- 元素访问 *随机访问*
- 查找
- 桶接口
- 哈希策略

不同数据类型的容器具有不通的接口

## 迭代器

- `begin/cbegin`
  **forward_list, unordered_set, unordered_map**
- end/cend
  **forward_list, unordered_set, unordered_map**
- rbegin/rend
  
- rend/crend

*array, vector, list, set, map, multiset, multimap* 有序容器，二叉树组织容器，都能双向遍历

**forward_list** 单向链表只能向后遍历，**无序关联容器** 顺序没有意义

## 容量

- size
- empty
- max_size
--- 
针对动态容器*vector*
- reserve
- capacity
- shrink_to_fit

## 修改器

*共同*
- clear
- insert
- emplace
- erase
- swap
---
*有序容器插入/删除单个元素
- push_back
- emplace_back
- pop_back
- push_front *forward_list是单链表，只能在头部插入，不能在尾部插入*
- emplace_front
- pop_front
以上插入都能够保证时间复杂度为 $O(1)$，可以根据不通的容器性质理解记忆
- **resize**(count, 可选 value_type&) *改变容器中的数量为count，多则删除，少则默认/拷贝构造*
---
*无序容器相关*
- c++17引入*待补充*
---

*list和forward_list特殊操作*
- merge *合并两链表*
- splice_after *从一个链表中转移元素到另一个链表*
- remove/remove_if *根据一元谓词，移除为true的元素*
- reverse 
- unique *根据二元谓词(若元素应被当做相等则返回true)，只保留第一个元素，移除其他相等元素*
- sort *std::sort要求随机访问迭代器，因此不能用于list和forward_list。将采用适合链表的排序算法*

## 元素访问

语意：访问某个位置上的元素，因此只有**顺序容器**据有此类接口

仅能用于支持随机访问的数组
- at
- operater[]
-  data *直接访问底部数组*
---
- front 
- back *forward_list为单链表，只能通过front访问第一个元素*

## 查找

仅能用于所有关联容器
- count
- find
- - equal_range(key) *返回匹配特定键的元素范围 返回类型为**pair<iterator,iterator>***
---

有序关联容器(因为有序关联容器才有大小关系，红黑树)
- lower_bound(key) *返回指向首个_不小于_给定键的元素的迭代器*
- uper_bound(key)  *返回指向首个_大于_给定键的元素的迭代器*

*为什么不用标准库中的通用算法：因为红黑树返回 `lower_bound` 和 `uper_bound` 有更高效的方法*

## 桶接口

针对hashmap和hashset，在需要管理桶提高性能时可能才会用到

- begin/cbegin
- end/cend
- bucket_count
- max_bucket_count

## hash策略

- load_factor
- max_load_factor
- rehash
- reserve
