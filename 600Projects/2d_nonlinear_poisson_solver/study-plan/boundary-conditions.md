# 狄利克雷边界条件处理详解

> 目标：理解如何在有限元系统中施加狄利克雷边界条件，掌握多种处理方法

---

## 1. 边界条件类型回顾

### 狄利克雷边界条件（第一类）

$$u = \hat{u} \quad \text{在 } \Gamma_D \text{ 上}$$

**物理意义**：直接指定边界上的场值
- 热传导：边界温度已知
- 结构力学：边界位移固定
- 静电场：边界电势给定

### 诺伊曼边界条件（第二类）

$$\frac{\partial u}{\partial n} = g \quad \text{在 } \Gamma_N \text{ 上}$$

**物理意义**：指定边界上的通量

**本项目不考虑诺伊曼边界条件**

### 罗宾边界条件（第三类）

$$\frac{\partial u}{\partial n} + \alpha u = \beta$$

**本项目不考虑**

---

## 2. 边界条件处理的挑战

### 问题描述

组装完成的全局系统：

$$K \cdot u = F$$

其中部分 $u_i$ 是已知的（边界值），需要：
1. 保持这些值不变
2. 修改系统使其余未知数可解
3. 保证修改后的系统正确

### 示例

```
节点编号：0 -- 1 -- 2 -- 3
边界条件：u₀ = 5, u₃ = 10

原系统：
┌           ┐┌   ┐   ┌   ┐
│ K₀₀ K₀₁ K₀₂ K₀₃ ││ u₀ │   │ F₀ │
│ K₁₀ K₁₁ K₁₂ K₁₃ ││ u₁ │ = │ F₁ │
│ K₂₀ K₂₁ K₂₂ K₂₃ ││ u₂ │   │ F₂ │
│ K₃₀ K₃₁ K₃₂ K₃₃ ││ u₃ │   │ F₃ │
└           ┘└   ┘   └   ┘

已知：u₀ = 5, u₃ = 10
未知：u₁, u₂
```

---

## 3. 处理方法一：直接代入法

### 基本思想

将已知值代入方程，把已知量移到右边，减少未知量个数。

### 算法步骤

对于每个边界节点 $i$（值为 $\hat{u}_i$）：

1. **修改右端项**：$F_j \leftarrow F_j - K_{ji} \hat{u}_i$（对所有 $j$）
2. **清零行和列**：$K_{i,*} = 0$，$K_{*,i} = 0$
3. **对角线设为 1**：$K_{ii} = 1$
4. **右端项设为边界值**：$F_i = \hat{u}_i$

### 示例

```
步骤 1：处理 u₀ = 5
- F₁ = F₁ - K₁₀×5
- F₂ = F₂ - K₂₀×5
- F₃ = F₃ - K₃₀×5

步骤 2：处理 u₃ = 10
- F₀ = F₀ - K₀₃×10
- F₁ = F₁ - K₁₃×10
- F₂ = F₂ - K₂₃×10

修改后的系统：
┌           ┐┌   ┐   ┌           ┐
│ 1   0   0   0  ││ u₀ │   │     5     │
│ 0  K₁₁ K₁₂  0  ││ u₁ │ = │ F₁-K₁₀×5-K₁₃×10 │
│ 0  K₂₁ K₂₂  0  ││ u₂ │   │ F₂-K₂₀×5-K₂₃×10 │
│ 0   0   0   1  ││ u₃ │   │    10     │
└           ┘└   ┘   └           ┘
```

### 代码实现

```cpp
void applyDirichletBC(SparseMatrix& K, Vector& F, 
                      const std::vector<int>& bc_nodes,
                      const std::vector<double>& bc_values) {
    // 方法：修改矩阵和向量
    for (size_t idx = 0; idx < bc_nodes.size(); idx++) {
        int i = bc_nodes[idx];
        double u_val = bc_values[idx];
        
        // 1. 修改右端项（对非边界行）
        for (int j = 0; j < K.cols(); j++) {
            if (K.coeff(i, j) != 0 && j != i) {
                F[j] -= K.coeff(i, j) * u_val;
            }
        }
        
        // 2. 清零第 i 行
        for (int j = 0; j < K.cols(); j++) {
            if (j != i) {
                K.coeffRef(i, j) = 0;
            }
        }
        
        // 3. 清零第 i 列（保持对称性）
        for (int j = 0; j < K.rows(); j++) {
            if (j != i) {
                K.coeffRef(j, i) = 0;
            }
        }
        
        // 4. 对角线设为 1
        K.coeffRef(i, i) = 1;
        
        // 5. 右端项设为边界值
        F[i] = u_val;
    }
}
```

### 优缺点

| 优点 | 缺点 |
|------|------|
| 保持矩阵大小 | 需要修改矩阵结构（稀疏矩阵不友好） |
| 概念直观 | 破坏矩阵对称性（如果不处理列） |
| | 边界点多时效率低 |

---

## 4. 处理方法二：划行划列法

### 基本思想

从系统中移除边界自由度，求解缩减后的系统。

### 算法步骤

1. 标记所有非边界自由度（活动自由度）
2. 提取子矩阵 $K_{aa}$（活动-活动部分）
3. 提取子向量 $F_a^{\text{eff}} = F_a - K_{ab} \hat{u}_b$
4. 求解 $K_{aa} u_a = F_a^{\text{eff}}$
5. 合并结果：$u = [u_a, \hat{u}_b]^T$

### 代码实现

```cpp
void applyDirichletBC_Elimination(const SparseMatrix& K, const Vector& F,
                                  const std::vector<int>& bc_nodes,
                                  const std::vector<double>& bc_values,
                                  Vector& u) {
    int N = K.rows();
    int n_bc = bc_nodes.size();
    int n_active = N - n_bc;
    
    // 1. 创建映射：全局索引 → 活动索引
    std::vector<int> globalToActive(N, -1);
    std::vector<int> activeToGlobal;
    int activeIdx = 0;
    
    // 标记边界节点
    std::vector<bool> isBC(N, false);
    for (int node : bc_nodes) {
        isBC[node] = true;
    }
    
    // 建立映射
    for (int i = 0; i < N; i++) {
        if (!isBC[i]) {
            globalToActive[i] = activeIdx;
            activeToGlobal.push_back(i);
            activeIdx++;
        }
    }
    
    // 2. 构建边界值向量
    Vector u_bc(N);
    for (size_t i = 0; i < bc_nodes.size(); i++) {
        u_bc[bc_nodes[i]] = bc_values[i];
    }
    
    // 3. 计算有效荷载 F_eff = F - K * u_bc
    Vector F_eff = F - K * u_bc;
    
    // 4. 提取子矩阵 K_aa
    std::vector<Eigen::Triplet<double>> triplets;
    for (int k = 0; k < K.outerSize(); ++k) {
        for (SparseMatrix::InnerIterator it(K, k); it; ++it) {
            int i = it.row();
            int j = it.col();
            int ia = globalToActive[i];
            int ja = globalToActive[j];
            if (ia >= 0 && ja >= 0) {
                triplets.emplace_back(ia, ja, it.value());
            }
        }
    }
    
    SparseMatrix K_aa(n_active, n_active);
    K_aa.setFromTriplets(triplets.begin(), triplets.end());
    
    // 5. 提取子向量 F_a
    Vector F_a(n_active);
    for (int i = 0; i < n_active; i++) {
        F_a[i] = F_eff[activeToGlobal[i]];
    }
    
    // 6. 求解缩减系统
    Vector u_a = solve(K_aa, F_a);
    
    // 7. 合并结果
    u.resize(N);
    for (int i = 0; i < n_active; i++) {
        u[activeToGlobal[i]] = u_a[i];
    }
    for (size_t i = 0; i < bc_nodes.size(); i++) {
        u[bc_nodes[i]] = bc_values[i];
    }
}
```

### 优缺点

| 优点 | 缺点 |
|------|------|
| 系统规模减小 | 需要重新构建矩阵 |
| 保持矩阵性质 | 实现复杂 |
| 适合边界节点多的情况 | 每次迭代都要重建（非线性问题） |

---

## 5. 处理方法三：罚函数法

### 基本思想

用很大的惩罚数近似强制边界条件。

### 算法

$$K_{ii} \leftarrow K_{ii} + \beta$$
$$F_i \leftarrow F_i + \beta \hat{u}_i$$

其中 $\beta$ 是很大的数（如 $10^{12}$）。

### 原理

从方程看：
$$(K_{ii} + \beta) u_i + \sum_{j \neq i} K_{ij} u_j = F_i + \beta \hat{u}_i$$

如果 $\beta \gg |K_{ij}|$，则近似有：
$$\beta u_i \approx \beta \hat{u}_i \Rightarrow u_i \approx \hat{u}_i$$

### 代码实现

```cpp
void applyDirichletBC_Penalty(SparseMatrix& K, Vector& F,
                              const std::vector<int>& bc_nodes,
                              const std::vector<double>& bc_values,
                              double beta = 1e12) {
    for (size_t idx = 0; idx < bc_nodes.size(); idx++) {
        int i = bc_nodes[idx];
        K.coeffRef(i, i) += beta;
        F[i] += beta * bc_values[idx];
    }
}
```

### 优缺点

| 优点 | 缺点 |
|------|------|
| 实现最简单 | 引入近似误差 |
| 不破坏矩阵结构 | 条件数恶化 |
| 适合快速原型 | 需要选择合适的 $\beta$ |

---

## 6. 方法对比与选择建议

| 方法 | 实现难度 | 精度 | 效率 | 适用场景 |
|------|---------|------|------|----------|
| 直接代入 | 中等 | 精确 | 中 | 边界节点少，稀疏矩阵不优化 |
| 划行划列 | 复杂 | 精确 | 高（单次） | 边界节点多，静态问题 |
| 罚函数 | 简单 | 近似 | 高 | 快速原型，迭代求解 |

### 本项目推荐

**非线性问题 + 每次迭代都要处理边界条件** → 建议用**直接代入法**

原因：
- 非线性问题每次牛顿迭代都要重新组装矩阵
- 划行划列每次都要重建子矩阵，开销大
- 直接代入法可以利用稀疏矩阵的 `coeffRef`

---

## 7. 边界条件的识别与标记

### 几何识别

对于矩形区域，识别各边：

```
D(0, ly) -------- C(lx, ly)
   |                 |
   |    区域 Ω       |
   |                 |
A(0, 0) -------- B(lx, 0)

边 AB: y = 0, 0 ≤ x ≤ lx
边 BC: x = lx, 0 ≤ y ≤ ly  
边 CD: y = ly, 0 ≤ x ≤ lx
边 DA: x = 0, 0 ≤ y ≤ ly
```

### 代码实现

```cpp
enum BoundaryEdge { BOTTOM, RIGHT, TOP, LEFT };

struct BoundaryCondition {
    BoundaryEdge edge;
    double value;
};

class BoundaryConditionApplier {
public:
    void markBoundaryNodes(Mesh& mesh, double lx, double ly) {
        const double tol = 1e-10;
        
        for (Node& node : mesh.nodes) {
            node.isBoundary = false;
            
            // 下边 (y = 0)
            if (std::abs(node.y) < tol) {
                node.isBoundary = true;
                node.boundaryEdge = BOTTOM;
            }
            // 上边 (y = ly)
            else if (std::abs(node.y - ly) < tol) {
                node.isBoundary = true;
                node.boundaryEdge = TOP;
            }
            // 左边 (x = 0)
            else if (std::abs(node.x) < tol) {
                node.isBoundary = true;
                node.boundaryEdge = LEFT;
            }
            // 右边 (x = lx)
            else if (std::abs(node.x - lx) < tol) {
                node.isBoundary = true;
                node.boundaryEdge = RIGHT;
            }
        }
    }
    
    void applyBCs(SparseMatrix& K, Vector& F, 
                  const Mesh& mesh,
                  const std::vector<BoundaryCondition>& bcs) {
        for (const auto& bc : bcs) {
            for (const Node& node : mesh.nodes) {
                if (node.isBoundary && node.boundaryEdge == bc.edge) {
                    applySingleBC(K, F, node.id, bc.value);
                }
            }
        }
    }
};
```

---

## 8. 学习检查清单

- [ ] 理解狄利克雷边界条件的物理意义
- [ ] 掌握三种处理方法的基本思想
- [ ] 能推导直接代入法的修改公式
- [ ] 理解划行划列法的缩减过程
- [ ] 理解罚函数法的近似原理
- [ ] 能根据场景选择合适的方法
- [ ] 能实现边界节点的几何识别

---

## 参考资源

1. **书籍**：
   - 《有限元方法》第 2 章（边界条件）
   - 《Finite Elements: A Gentle Introduction》Chapter 4

2. **在线资源**：
   - [deal.II 边界条件处理](https://www.dealii.org/current/doxygen/deal.II/group__constraints.html)

---

## 练习题

1. 手算直接代入法对一个 4 节点系统的修改
2. 比较三种方法的数值误差（罚函数法的 β 影响）
3. 实现边界节点的自动识别
4. 思考：如果同一节点有多个边界条件冲突怎么办？
5. 分析直接代入法对矩阵条件数的影响
