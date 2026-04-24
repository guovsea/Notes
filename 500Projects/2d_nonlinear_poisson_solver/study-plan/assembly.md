# 组装过程（直接刚度法）详解

> 目标：理解如何将单元矩阵组装成全局矩阵，以及直接刚度法的实现

---

## 1. 为什么需要组装？

### 问题背景

有限元方法将求解域离散为多个小单元：
- 每个单元有自己的单元刚度矩阵 $K^{(e)}$
- 需要把所有单元的贡献组合成全局刚度矩阵 $K$

**类比**：拼图游戏
- 每块拼图（单元）有自己的图案（单元矩阵）
- 需要按照正确位置（自由度映射）拼成完整图画（全局矩阵）

### 全局系统

$$K \cdot u = F$$

其中：
- $K$：$N \times N$ 全局刚度矩阵（$N$ = 总节点数）
- $u$：$N \times 1$ 解向量（所有节点的未知量）
- $F$：$N \times 1$ 全局荷载向量

---

## 2. 自由度映射

### Element Freedom Table (EFT)

每个单元有自己的局部节点编号（0, 1, 2, ...），对应全局节点编号：

```
全局网格：          单元 1（三角形）：
                    局部: 0--1--2
4---5---6                ↓  ↓  ↓
|   |   |           全局: 0--1--3
0---1---3                
|       |
0---2---4           EFT = [0, 1, 3]

节点坐标：
0: (0, 0)   1: (1, 0)   2: (0, 1)
3: (2, 0)   4: (1, 1)   5: (3, 0)
6: (4, 0)
```

### EFT 的作用

EFT 是局部自由度到全局自由度的映射表：
- 局部节点 $i$ ↔ 全局节点 $EFT[i]$
- 单元矩阵 $K^{(e)}_{ij}$ ↔ 全局矩阵 $K_{EFT[i], EFT[j]}$

---

## 3. 直接刚度法

### 核心思想

**叠加原理**：每个单元对其相连的自由度对全局矩阵做出贡献。

### 组装公式

$$K_{IJ} = \sum_{e} K^{(e)}_{ij} \cdot \delta_{I, EFT[i]} \cdot \delta_{J, EFT[j]}$$

简单说：**遍历所有单元，把单元矩阵的元素加到全局矩阵对应位置**。

### 示例

考虑两个三角形单元：

```
    3
   /|
  / |
 0--1--2
 
单元 1：节点 [0, 1, 3]，矩阵 K1[3][3]
单元 2：节点 [1, 2, 3]，矩阵 K2[3][3]

全局矩阵 K[4][4]（节点 0,1,2,3）
```

组装过程：

```
初始化 K = 0

处理单元 1 (EFT = [0, 1, 3]):
  K[0][0] += K1[0][0]    K[0][1] += K1[0][1]    K[0][3] += K1[0][2]
  K[1][0] += K1[1][0]    K[1][1] += K1[1][1]    K[1][3] += K1[1][2]
  K[3][0] += K1[2][0]    K[3][1] += K1[2][1]    K[3][3] += K1[2][2]

处理单元 2 (EFT = [1, 2, 3]):
  K[1][1] += K2[0][0]    K[1][2] += K2[0][1]    K[1][3] += K2[0][2]
  K[2][1] += K2[1][0]    K[2][2] += K2[1][1]    K[2][3] += K2[1][2]
  K[3][1] += K2[2][0]    K[3][2] += K2[2][1]    K[3][3] += K2[2][2]
```

注意：$K[1][1]$、$K[1][3]$、$K[3][1]$、$K[3][3]$ 被两个单元都贡献了！

---

## 4. 程序员实现

### 基础版实现

```cpp
void assemble(const Element& elem, const Matrix& Ke, SparseMatrix& K) {
    const std::vector<int>& eft = elem.getEFT();
    int n = eft.size();
    
    for (int i = 0; i < n; i++) {
        int I = eft[i];
        for (int j = 0; j < n; j++) {
            int J = eft[j];
            K(I, J) += Ke(i, j);  // 叠加到全局矩阵
        }
    }
}
```

### 完整组装流程

```cpp
class Assembler {
public:
    void assembleSystem(const Mesh& mesh, 
                        SparseMatrix& K, 
                        Vector& F,
                        const Vector& u_current) {
        int N = mesh.numNodes();
        K.resize(N, N);
        F.resize(N);
        K.setZero();
        F.setZero();
        
        // 遍历所有单元
        for (const Element& elem : mesh.elements) {
            // 1. 计算单元刚度矩阵
            Matrix Ke = computeElementStiffness(elem, u_current);
            
            // 2. 计算单元荷载向量
            Vector Fe = computeElementLoad(elem, u_current);
            
            // 3. 组装到全局
            assembleMatrix(elem, Ke, K);
            assembleVector(elem, Fe, F);
        }
    }

private:
    void assembleMatrix(const Element& elem, const Matrix& Ke, SparseMatrix& K) {
        const auto& eft = elem.getEFT();
        for (int i = 0; i < eft.size(); i++) {
            for (int j = 0; j < eft.size(); j++) {
                K.coeffRef(eft[i], eft[j]) += Ke(i, j);
            }
        }
    }
    
    void assembleVector(const Element& elem, const Vector& Fe, Vector& F) {
        const auto& eft = elem.getEFT();
        for (int i = 0; i < eft.size(); i++) {
            F[eft[i]] += Fe[i];
        }
    }
};
```

### 稀疏矩阵优化

#### 问题：向稀疏矩阵插入元素很慢

Eigen 的稀疏矩阵插入操作 `coeffRef(i,j)` 如果位置不存在，需要重新分配内存。

#### 解决方案：预分配模式

```cpp
// 步骤 1：预分析，确定非零模式
std::vector<std::vector<int>> sparsityPattern(N);

for (const Element& elem : mesh.elements) {
    const auto& eft = elem.getEFT();
    for (int i : eft) {
        for (int j : eft) {
            sparsityPattern[i].push_back(j);
        }
    }
}

// 去重并排序
for (auto& row : sparsityPattern) {
    std::sort(row.begin(), row.end());
    row.erase(std::unique(row.begin(), row.end()), row.end());
}

// 步骤 2：用模式创建稀疏矩阵
K.setFromTriplets(triplets.begin(), triplets.end());

// 步骤 3：填充数值（快速！）
K.makeCompressed();  // 压缩存储

// 后续组装时直接修改值
for (...) {
    K.valuePtr()[index] += value;  // 直接访问底层数组
}
```

#### Eigen 推荐方式

```cpp
// 使用三元组列表预构建
std::vector<Eigen::Triplet<double>> triplets;
triplets.reserve(estimated_non_zeros);

for (const Element& elem : mesh.elements) {
    const auto& eft = elem.getEFT();
    Matrix Ke = computeElementStiffness(elem);
    
    for (int i = 0; i < eft.size(); i++) {
        for (int j = 0; j < eft.size(); j++) {
            triplets.emplace_back(eft[i], eft[j], Ke(i, j));
        }
    }
}

SparseMatrix K(N, N);
K.setFromTriplets(triplets.begin(), triplets.end());
```

---

## 5. 数据结构建议

### 网格类设计

```cpp
struct Node {
    double x, y;
    int id;
    bool isBoundary;  // 标记边界节点
    double boundaryValue;  // 边界值（如果 isBoundary）
};

struct Element {
    std::vector<int> nodeIds;  // EFT: 局部到全局的映射
    ElementType type;  // TRIANGLE 或 QUADRILATERAL
    
    // 获取 EFT
    const std::vector<int>& getEFT() const { return nodeIds; }
    
    // 获取节点数
    int numNodes() const { return nodeIds.size(); }
};

class Mesh {
public:
    std::vector<Node> nodes;
    std::vector<Element> elements;
    
    int numNodes() const { return nodes.size(); }
    int numElements() const { return elements.size(); }
    
    // 构建邻接关系（用于优化）
    void buildConnectivity();
};
```

### 矩形网格生成器

```cpp
class RectangularMeshGenerator {
public:
    Mesh generate(double lx, double ly, int Nx, int Ny, ElementType type) {
        Mesh mesh;
        
        // 1. 生成节点
        for (int j = 0; j <= Ny; j++) {
            for (int i = 0; i <= Nx; i++) {
                Node node;
                node.x = i * lx / Nx;
                node.y = j * ly / Ny;
                node.id = j * (Nx + 1) + i;
                mesh.nodes.push_back(node);
            }
        }
        
        // 2. 生成单元
        if (type == QUADRILATERAL) {
            for (int j = 0; j < Ny; j++) {
                for (int i = 0; i < Nx; i++) {
                    Element elem;
                    elem.type = QUADRILATERAL;
                    int base = j * (Nx + 1) + i;
                    elem.nodeIds = {base, base+1, base+Nx+2, base+Nx+1};
                    mesh.elements.push_back(elem);
                }
            }
        } else if (type == TRIANGLE) {
            // 每个矩形分成两个三角形
            for (int j = 0; j < Ny; j++) {
                for (int i = 0; i < Nx; i++) {
                    int base = j * (Nx + 1) + i;
                    
                    // 第一个三角形
                    Element elem1;
                    elem1.type = TRIANGLE;
                    elem1.nodeIds = {base, base+1, base+Nx+2};
                    mesh.elements.push_back(elem1);
                    
                    // 第二个三角形
                    Element elem2;
                    elem2.type = TRIANGLE;
                    elem2.nodeIds = {base, base+Nx+2, base+Nx+1};
                    mesh.elements.push_back(elem2);
                }
            }
        }
        
        return mesh;
    }
};
```

---

## 6. 组装优化技巧

### 优化 1：预计算不变部分

对于线性泊松方程（非线性项为 0），扩散项 $\int \nabla N_i \cdot \nabla N_j$ 可以预计算：

```cpp
class Element {
    Matrix Ke_linear;  // 预计算的线性部分
    
    void precompute() {
        Ke_linear = computeLinearStiffness();
    }
    
    Matrix computeTangentMatrix(const Vector& u) {
        Matrix Ke = Ke_linear;  // 从预计算部分开始
        // 只添加非线性修正
        Ke += computeNonlinearCorrection(u);
        return Ke;
    }
};
```

### 优化 2：并行组装

```cpp
// OpenMP 并行
#pragma omp parallel for
for (int e = 0; e < mesh.numElements(); e++) {
    Element& elem = mesh.elements[e];
    Matrix Ke = computeElementStiffness(elem);
    
    // 需要锁来保护全局矩阵
    #pragma omp critical
    {
        assembleMatrix(elem, Ke, K);
    }
}
```

### 优化 3：分块组装

对于大规模问题，可以分块处理：

```cpp
// 将网格分成若干子域
for (auto& subdomain : mesh.subdomains) {
    SparseMatrix K_local;
    // 组装子域矩阵
    // ...
    
    // 合并到全局
    mergeSubdomainMatrix(K_local, K_global);
}
```

---

## 7. 学习检查清单

- [ ] 理解为什么需要组装过程
- [ ] 掌握 EFT 的概念和作用
- [ ] 理解直接刚度法的叠加原理
- [ ] 能画出组装的示意图
- [ ] 能写出完整的组装代码
- [ ] 了解稀疏矩阵的存储优化
- [ ] 理解预分配模式的重要性

---

## 参考资源

1. **书籍**：
   - 《有限元方法》第 3 章（组装过程）
   - 《Programming the Finite Element Method》Smith et al., Chapter 3

2. **代码参考**：
   - [Eigen 稀疏矩阵文档](https://eigen.tuxfamily.org/dox/group__TutorialSparse.html)

---

## 练习题

1. 画出 2×2 矩形网格（四边形单元）的 EFT 表
2. 手算组装一个简单网格的全局矩阵
3. 比较稠密矩阵和稀疏矩阵在 100×100 网格上的内存占用
4. 实现带预分配的稀疏矩阵组装
5. 分析组装过程的时间复杂度
