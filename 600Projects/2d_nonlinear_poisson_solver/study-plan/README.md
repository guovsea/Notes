# 二维非线性泊松方程求解 - 学习路线图

> 面向对象：计算机专业本科 + 2年开发经验，数学基础一般，物理基础薄弱
> 
> 目标：理解并实现二维非线性泊松方程的有限元求解器

---

## 第一阶段：建立直觉（约 3-5 天）

这个阶段的目标是建立对问题的直觉，不需要深入数学细节。重点理解**"为什么要这样做"**。

### 1.1 泊松方程是什么？（1天）

**一句话理解**：泊松方程描述的是"某种场在空间中的分布规律"。

#### 直观类比

想象一个橡皮膜：
- 你在某些点**向下按压**橡皮膜（这就是"源"）
- 橡皮膜会形成一个凹陷的形状
- 泊松方程就是描述：给定按压力分布，求橡皮膜的形状

**生活中的泊松方程**：
- 热传导：给定热源，求温度分布
- 静电场：给定电荷分布，求电势分布
- 流体：给定压力源，求压力分布

#### 需要理解的要点

| 符号 | 含义 | 类比 |
|------|------|------|
| $u$ | 待求解的场（温度/电势/高度） | 橡皮膜的高度 |
| $f$ | 源函数 | 按压力的大小 |
| $\nabla^2 u$ | 拉普拉斯算子 | 橡皮膜的"弯曲程度" |

#### 边界条件的直观理解

- **狄利克雷边界条件**：直接指定边界上的值
  - 类比：橡皮膜边缘被固定在某一高度
  - 例：边缘固定在 0 度（温度问题）

- **诺伊曼边界条件**：指定边界上的变化率
  - 类比：橡皮膜边缘可以自由滑动，但倾斜角度固定
  - 本项目**不考虑**这种边界条件

#### 线性 vs 非线性

- **线性**：源 $f$ 与解 $u$ 无关
  - 类比：按压力固定，与橡皮膜形状无关
  
- **非线性**：源 $f$ 依赖于解 $u$
  - 类比：按压力会根据橡皮膜的高度自动调整
  - 本项目处理的是非线性情况

#### 学习资源

- [ ] 📺 视频：[泊松方程的直观解释](https://www.youtube.com/watch?v=ly4S0oi3Yz8)（英文，8分钟）
- [ ] 📄 文章：[从物理直觉理解泊松方程](https://zhuanlan.zhihu.com/p/401288463)

---

### 1.2 牛顿迭代法（1-2天）

**一句话理解**：用切线逼近来逐步逼近方程的根。

#### 核心思想

想象你在找函数 $F(x) = 0$ 的解：
1. 先猜一个初始值 $x_0$
2. 在当前点画切线，找到切线与 x 轴的交点
3. 用这个交点作为新的猜测值
4. 重复直到收敛

#### 核心公式

$$x_{n+1} = x_n - \frac{F(x_n)}{F'(x_n)}$$

**从程序员角度理解**：
- 这是一个循环，直到满足停止条件
- 需要计算函数值 $F(x)$ 和导数值 $F'(x)$
- 停止条件：变化量很小或函数值接近零

#### 本项目中的牛顿迭代

```
初始猜测 u₀ → 计算残差 → 求解线性系统得 δu → 更新 u → 检查收敛 → 重复
```

#### 学习资源

- [ ] 📺 视频：[牛顿法几何直观](https://www.bilibili.com/video/BV1Y7411d7Ys)（中文，15分钟）
- [ ] 📄 文章：[牛顿迭代法详解](https://zh.wikipedia.org/wiki/%E7%89%9B%E9%A1%BF%E6%B3%95)

---

### 1.3 有限元方法概述（1-2天）

**一句话理解**：把连续问题离散成有限个小块来近似求解。

#### 核心思想

想象你要画一条复杂的曲线：
- 直接画精确的曲线很难
- 改用很多小段直线来近似
- 段数越多，近似越精确

有限元就是这样：
- 把求解区域划分成很多小单元（网格）
- 在每个单元上用简单的函数近似解
- 把所有单元拼起来得到全局近似解

#### 关键概念

| 概念 | 通俗解释 | 代码中的体现 |
|------|----------|--------------|
| 网格 (Mesh) | 把区域切分成小块 | Node + Element 的数据结构 |
| 节点 (Node) | 网格的顶点 | 存储坐标信息 |
| 单元 (Element) | 网格的小块（三角形/四边形） | 包含节点索引 |
| 形函数 (Shape Function) | 单元内的插值函数 | 计算基函数值 |
| 刚度矩阵 | 描述单元间相互作用的矩阵 | 稀疏矩阵 |

#### 一阶单元 vs 高阶单元

- **一阶三角形**：3个节点，线性插值
- **一阶四边形**：4个节点，双线性插值
- 本项目只用一阶单元

#### 学习资源

- [ ] 📺 视频：[有限元方法入门](https://www.bilibili.com/video/BV1Db411s7fH)（中文，系列视频，建议看前3集）
- [ ] 📄 交互式教程：[有限元方法可视化](https://www.simscale.com/blog/2016/10/what-is-finite-element-method/)

---

## 第二阶段：数学基础（约 5-7 天）

这个阶段需要掌握完成项目所需的数学工具。我们会跳过严格证明，专注于"怎么用"。

### 2.1 多元微积分回顾（1-2天）

本项目需要的微积分知识：

#### 梯度 (Gradient)

对于函数 $u(x, y)$：
$$\nabla u = \left(\frac{\partial u}{\partial x}, \frac{\partial u}{\partial y}\right)$$

**直观理解**：梯度指向函数增长最快的方向，大小是增长速率。

#### 散度 (Divergence)

对于向量场 $\mathbf{F} = (F_x, F_y)$：
$$\nabla \cdot \mathbf{F} = \frac{\partial F_x}{\partial x} + \frac{\partial F_y}{\partial y}$$

**泊松方程中的散度**：$\nabla \cdot (\nabla u) = \nabla^2 u$

#### 拉普拉斯算子

$$\nabla^2 u = \frac{\partial^2 u}{\partial x^2} + \frac{\partial^2 u}{\partial y^2}$$

**直观理解**：描述函数在某点的"弯曲程度"。

#### 积分换元（等参变换的基础）

$$
\int_{\Omega} f(x,y) \, dxdy = \int_{\hat{\Omega}} f(x(s,t), y(s,t)) \, |J| \, dsdt
$$

其中 $J$ 是雅可比矩阵的行列式。

#### 学习资源

- [ ] 📄 复习材料：[梯度、散度、旋度](https://www.zhihu.com/question/24049633)（知乎）
- [ ] 📺 视频：[多元微积分直觉](https://www.youtube.com/playlist?list=PLZHQObOWTQDMsr9K-rj53DwVRMYO3t5Yr)（3Blue1Brown，英文，强烈推荐）

---

### 2.2 形函数与等参变换（2天）

这是有限元的核心内容，需要深入理解。

**进阶学习**：[形函数详解](./shape-functions.md)

#### 核心要点

1. **形函数的作用**：在单元内部插值
2. **Kronecker delta 性质**：$N_i(x_j, y_j) = \delta_{ij}$
3. **等参变换**：把任意形状的单元映射到标准单元

#### 程序员视角

```cpp
// 形函数计算
// 输入：局部坐标 (s, t)
// 输出：4个形函数在 (s, t) 处的值（四边形单元）
double N1 = 0.25 * (1 - s) * (1 - t);
double N2 = 0.25 * (1 + s) * (1 - t);
double N3 = 0.25 * (1 + s) * (1 + t);
double N4 = 0.25 * (1 - s) * (1 + t);
```

---

### 2.3 数值积分：高斯求积法（1-2天）

**进阶学习**：[高斯积分详解](./gauss-quadrature.md)

#### 核心思想

用少数几个点的函数值加权求和来近似积分：

$$\int_{-1}^{1} f(x) dx \approx \sum_{i=1}^{n} w_i f(x_i)$$

#### 关键事实

- n 个高斯点可以精确积分 2n-1 次多项式
- 二维情况用双重求积

#### 常用高斯点

| 点数 | 点坐标 | 权重 |
|------|--------|------|
| 1 | 0 | 2 |
| 2 | ±1/√3 | 1 |

---

### 2.4 线性代数：稀疏矩阵（1天）

**进阶学习**：[稀疏矩阵详解](./sparse-matrix.md)

#### 为什么需要稀疏矩阵

- 全局刚度矩阵大部分元素为零
- 存储所有元素浪费内存
- 稀疏矩阵只存储非零元素

#### 常用格式

- **CSR (Compressed Sparse Row)**：行压缩格式
- **CSC (Compressed Sparse Column)**：列压缩格式

#### 代码中的使用（Eigen）

```cpp
#include <Eigen/Sparse>

Eigen::SparseMatrix<double> K(N, N);  // N×N 稀疏矩阵
K.insert(i, j) = value;  // 插入元素
```

---

## 第三阶段：算法推导（约 5-7 天）

这是项目的核心，需要完整理解有限元求解的每一步。

### 3.1 弱形式与加权残差法（2天）

**进阶学习**：[弱形式推导详解](./weak-form.md)

#### 核心思想

原方程（强形式）：$-\nabla^2 u = f$

乘以测试函数并积分，通过分部积分得到弱形式：

$$\int_\Omega \nabla u \cdot \nabla v \, d\Omega = \int_\Omega f v \, d\Omega$$

#### 为什么需要弱形式

1. 降低对解的光滑性要求
2. 便于数值离散
3. 自然引入边界条件

---

### 3.2 单元刚度矩阵推导（2天）

**进阶学习**：[单元刚度矩阵推导](./element-stiffness.md)

#### 公式 1.5 的离散化

$$k_{ij}^{(e)} = -\int_{\Omega_K} \nabla N_i \cdot \nabla N_j \, d\Omega - \int_{\Omega_K} N_i N_j \frac{\partial f}{\partial u} \, d\Omega$$

**离散化步骤**：
1. 计算形函数的梯度
2. 等参变换
3. 数值积分

---

### 3.3 组装过程（直接刚度法）（1天）

**进阶学习**：[组装过程详解](./assembly.md)

#### 核心思想

把每个单元对全局矩阵的贡献叠加起来。

#### 关键数据结构

- **Element Freedom Table (EFT)**：单元自由度表，记录单元节点对应的全局索引

```cpp
// 示例：三角形单元，节点全局编号为 [10, 15, 23]
// EFT = [10, 15, 23]
// 单元刚度矩阵 k(3×3) 组装到全局 K(N×N) 中
for (i = 0; i < 3; i++)
    for (j = 0; j < 3; j++)
        K[EFT[i]][EFT[j]] += k[i][j];
```

---

### 3.4 边界条件处理（1-2天）

**进阶学习**：[边界条件处理详解](./boundary-conditions.md)

#### 狄利克雷边界条件的施加方法

1. **直接代入法**：修改矩阵，直接设置已知值
2. **罚函数法**：用大的惩罚数近似强制条件
3. **拉格朗日乘子法**：增加约束方程

本项目推荐使用直接代入法。

---

## 第四阶段：C++ 实现（约 7-10 天）

把理论转化为代码。利用你丰富的编程经验，这部分应该相对轻松。

### 4.1 技术栈概览

| 功能 | 推荐库 | 学习资源 |
|------|--------|----------|
| 线性代数 | Eigen3 | [Eigen 快速入门](https://eigen.tuxfamily.org/dox/GettingStarted.html) |
| 表达式解析 | muParser / ExprTK | [muParser 文档](https://beltoforion.de/en/muparser/) |
| 稀疏矩阵 | Eigen::Sparse | [Eigen 稀疏矩阵教程](https://eigen.tuxfamily.org/dox/group__TutorialSparse.html) |
| 高斯积分 | Boost.Math | [Boost Gauss 文档](https://www.boost.org/doc/libs/1_85_0/libs/math/doc/html/math_toolkit/gauss.html) |
| 可视化 | VTK | [VTK 文件格式](https://docs.vtk.org/en/latest/design_documents/VTKFileFormats.html) |
| 单元测试 | GoogleTest / Catch2 | [GoogleTest 入门](https://google.github.io/googletest/) |
| 构建系统 | CMake | [CMake 教程](https://cmake.org/cmake/help/latest/guide/tutorial/) |
| 文档生成 | Doxygen | [Doxygen 手册](https://www.doxygen.nl/manual/) |

### 4.2 项目结构设计

```
poisson2d/
├── CMakeLists.txt          # 构建配置
├── src/
│   ├── main.cpp            # 客户端（<100行）
│   └── ...                 # 其他源文件
├── include/
│   └── poisson2d/          # 库头文件
│       ├── mesh.h
│       ├── solver.h
│       └── ...
├── lib/                    # 核心库（编译为动态库）
├── tests/                  # 单元测试
│   └── test_*.cpp
├── docs/                   # 文档
│   ├── Doxyfile
│   └── algorithms/         # 算法文档
└── examples/               # 示例输入文件
    └── input.txt
```

### 4.3 核心类设计

```cpp
// 网格相关
class Node { /* 坐标 + 边界标记 */ };
class Element { /* 节点索引数组 + 类型 */ };
class Mesh { /* Node数组 + Element数组 + 方法 */ };

// 求解器
class PoissonSolver {
    Mesh mesh;
    Eigen::SparseMatrix<double> K;  // 全局刚度矩阵
    Eigen::VectorXd F;              // 全局荷载向量
    Eigen::VectorXd u;              // 解向量
public:
    void assemble();                // 组装
    void applyBoundaryConditions(); // 施加边界条件
    void solve();                   // 求解
    void iterate();                 // 牛顿迭代
};
```

### 4.4 实现步骤建议

1. **基础框架**（2天）
   - [ ] CMake 项目搭建
   - [ ] 网格数据结构（Node, Element, Mesh）
   - [ ] 简单的网格生成（矩形剖分）

2. **核心算法**（3天）
   - [ ] 形函数计算
   - [ ] 单元刚度矩阵计算
   - [ ] 组装过程
   - [ ] 边界条件处理

3. **求解器**（2天）
   - [ ] 牛顿迭代实现
   - [ ] 表达式解析集成
   - [ ] 配置文件读取

4. **测试与可视化**（2天）
   - [ ] 单元测试
   - [ ] VTK 输出
   - [ ] 与解析解对比

5. **优化与文档**（1-2天）
   - [ ] 稀疏矩阵优化
   - [ ] Doxygen 文档
   - [ ] 流程图和 UML

---

## 第五阶段：测试与验证（约 2-3 天）

### 5.1 与解析解对比

PDF 提供的解析解（当 $f(u) = 0$ 时）：

$$u(x, y) = \frac{4\hat{u}}{\pi} \sum_{n=1,3,5,...} \frac{\sin(\frac{n\pi}{l_x}x) \sinh(\frac{n\pi}{l_x}y)}{n \sinh(n\pi \frac{l_y}{l_x})}$$

**验证步骤**：
1. 设置边界条件：$u_{CD} = \hat{u}$，其他边为 0
2. 设置 $f(u) = 0$
3. 计算数值解与解析解的误差
4. 分析误差随网格加密的变化

### 5.2 单元测试示例

```cpp
TEST(MeshTest, RectangularMesh) {
    Mesh mesh(2.0, 1.0, 4, 2);  // 2x1 矩形，4x2 网格
    EXPECT_EQ(mesh.numNodes(), 15);  // (4+1)*(2+1) = 15
    EXPECT_EQ(mesh.numElements(), 8); // 4*2 = 8
}

TEST(ShapeFunctionTest, Quadrilateral) {
    // 在标准单元中心 (0,0)，四个形函数应各为 0.25
    auto N = QuadrilateralElement::shapeFunctions(0, 0);
    for (int i = 0; i < 4; i++) {
        EXPECT_NEAR(N[i], 0.25, 1e-10);
    }
}
```

---

## 附录：推荐学习顺序

### 第一周
- [ ] Day 1-2：泊松方程物理意义 + 牛顿迭代法
- [ ] Day 3-4：有限元方法概述 + 多元微积分回顾
- [ ] Day 5-7：形函数基础

### 第二周
- [ ] Day 8-9：高斯积分
- [ ] Day 10-11：弱形式推导
- [ ] Day 12-14：单元刚度矩阵推导

### 第三周
- [ ] Day 15-16：组装过程 + 边界条件处理
- [ ] Day 17-21：C++ 实现基础框架

### 第四周
- [ ] Day 22-28：完整实现 + 测试 + 文档

---

## 疑难解答

**Q: 数学推导太复杂，看不懂怎么办？**

A: 先看代码实现，再回头补数学。很多工程师都是"用中学"。

**Q: 物理背景薄弱会影响吗？**

A: 不会。本项目本质上是数值计算问题，物理只是背景。把 $u$ 和 $f$ 当成抽象的数学对象即可。

**Q: 需要学完所有数学才能开始写代码吗？**

A: 不需要。可以并行推进：一边学习数学，一边实现已理解的部分。

---

## 参考资源汇总

### 书籍
1. 《有限元方法基础教程》（曾攀）- 中文入门首选
2. 《The Finite Element Method: Its Basis and Fundamentals》（Zienkiewicz）- 经典教材
3. 《C++ 程序设计语言》（Bjarne Stroustrup）- 第1章关于程序设计技术

### 在线课程
1. [有限元方法 - MIT OpenCourseWare](https://ocw.mit.edu/courses/mechanical-engineering/2-092-finite-element-analysis-of-solids-and-fluids-i-fall-2009/)
2. [Scientific Computing with C++](https://www.youtube.com/playlist?list=PLmWm8L1lnuJEwJ1dr7U2C_6h_1HSp1AKx)

### 代码参考
1. [deal.II](https://www.dealii.org/) - 开源有限元库
2. [FEniCS](https://fenicsproject.org/) - 高级有限元框架

---

*祝学习顺利！遇到问题随时记录，后续可以补充到这份文档中。*
