容器库是类模板与算法的汇集，允许程序员简单地访问常见数据结构

## 顺序容器

- array *静态连续数组*
- vector *动态连续数组*
- deque *双链表*
- list *双链表*
- forward_list *单链表*

## 关联容器

能快速查找$O(log n)$复杂度

底层都是**红黑树**实现的，红黑树是一种子平衡二叉树，因此关联容器中的对象**必须可比较**

- set *唯一键集合，按照键排序*
- map *唯一键值对集合，按照键排序
- multiset *不唯一键集合*
- multimap *不唯一键值对集合*
## 无序关联容器

能快速查找（均摊 $O(1)$ ，最坏情况 $O(n)$ 的复杂度）的**hash**结构 (复杂度为 $O(n)$ 的情况：针对 multiset 和 multimap，所有对象的映射值都相同，就会位于同一个 bucket 中)

- unordered_set *唯一键集合，按照键生成hash散列*
- unordered_map *唯一键值对集合，按照键生成hash散列*
- unordered_multiset *键不唯一*
- unordered_multimap *键唯一，值不唯一*

## 容器适配器

为顺序容器进一步封装

- stack
- queue
- priority_queue