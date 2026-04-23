---
tags:
  - algorithm/string
  - type/concept
mastery: 5
---

## 相关

- [[DFS - atoi为例]]
- [[437. 路径总和 III]]

## 前缀数组（前缀和 / Prefix Sum）

前缀和是一种预处理技巧，用于快速计算区间和。

**一维前缀和**

```cpp
// 预处理前缀和数组
vector<int> prefix(n + 1, 0);
for (int i = 0; i < n; i++) {
    prefix[i + 1] = prefix[i] + nums[i];
}

// 查询区间 [l, r] 的和（下标从 0 开始）
int rangeSum = prefix[r + 1] - prefix[l];
```

**复杂度**：预处理 O(n)，查询 O(1)

**二维前缀和**

```cpp
// 预处理
vector<vector<int>> prefix(m + 1, vector<int>(n + 1, 0));
for (int i = 0; i < m; i++) {
    for (int j = 0; j < n; j++) {
        prefix[i + 1][j + 1] = prefix[i][j + 1] + prefix[i + 1][j] - prefix[i][j] + matrix[i][j];
    }
}

// 查询子矩阵 (r1,c1) 到 (r2,c2) 的和
int sum = prefix[r2 + 1][c2 + 1] - prefix[r1][c2 + 1] - prefix[r2 + 1][c1] + prefix[r1][c1];
```

**应用场景**

- 多次查询区间和/子矩阵和
- [[437. 路径总和 III]] 中的路径前缀和方法
- 连续子数组和问题

## KMP 字符串匹配算法

KMP 算法用于在文本串中快速查找模式串，利用已匹配的信息避免重复比较。

**核心思想**：当匹配失败时，利用模式串自身的性质（next 数组），将模式串尽可能右移，避免文本串指针回退。

**时间复杂度**：O(m + n)，其中 m 为模式串长度，n 为文本串长度

- [ ] KMP 算法详细实现，待补充

