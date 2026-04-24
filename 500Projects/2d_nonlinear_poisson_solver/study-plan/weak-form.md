# 弱形式与加权残差法详解

> 目标：理解偏微分方程的弱形式推导过程，以及它在有限元方法中的作用

---

## 1. 强形式 vs 弱形式

### 泊松方程的强形式

$$-\nabla^2 u = f \quad \text{在 } \Omega \text{ 内}$$

边界条件：
- 狄利克雷：$u = \hat{u}$ 在 $\Gamma_D$ 上
- 诺伊曼：$\frac{\partial u}{\partial n} = g$ 在 $\Gamma_N$ 上

**强形式的要求**：解 $u$ 必须是二阶连续可微（$C^2$）

### 为什么要弱化？

1. **光滑性要求过高**：实际问题中解可能不光滑（如材料界面）
2. **数值离散困难**：直接离散二阶导数精度受限
3. **边界条件处理**：诺伊曼边界条件可以自然融入

### 弱形式的思想

**用一个"测试"函数 $v$ 乘以方程，然后积分**，通过分部积分降低导数阶数。

---

## 2. 弱形式推导过程

### 第一步：乘测试函数并积分

选择测试函数 $v$（在狄利克雷边界上 $v = 0$），乘以方程两边并在域上积分：

$$-\int_\Omega v \nabla^2 u \, d\Omega = \int_\Omega vf \, d\Omega$$

### 第二步：分部积分

对左边使用格林第一恒等式（分部积分的多维形式）：

$$\int_\Omega v \nabla^2 u \, d\Omega = \int_{\partial\Omega} v \frac{\partial u}{\partial n} \, d\Gamma - \int_\Omega \nabla v \cdot \nabla u \, d\Omega$$

代入原式：

$$-\left(\int_{\partial\Omega} v \frac{\partial u}{\partial n} \, d\Gamma - \int_\Omega \nabla v \cdot \nabla u \, d\Omega\right) = \int_\Omega vf \, d\Omega$$

### 第三步：整理并应用边界条件

$$\int_\Omega \nabla v \cdot \nabla u \, d\Omega - \int_{\partial\Omega} v \frac{\partial u}{\partial n} \, d\Gamma = \int_\Omega vf \, d\Omega$$

**处理边界项**：

在狄利克雷边界 $\Gamma_D$ 上：$v = 0$，所以这部分积分为 0

在诺伊曼边界 $\Gamma_N$ 上：$\frac{\partial u}{\partial n} = g$（已知）

因此：

$$\int_\Omega \nabla v \cdot \nabla u \, d\Omega = \int_\Omega vf \, d\Omega + \int_{\Gamma_N} vg \, d\Gamma$$

### 第四步：本项目简化

本项目**不考虑诺伊曼边界条件**，所以最终弱形式为：

$$\boxed{\int_\Omega \nabla v \cdot \nabla u \, d\Omega = \int_\Omega vf \, d\Omega}$$

边界条件通过其他方式处理（见边界条件文档）

---

## 3. 函数空间的概念

### 什么是函数空间？

函数空间是把函数当作"向量"的抽象空间。

类比：
- $\mathbb{R}^n$：n维向量空间，元素是向量 $(x_1, ..., x_n)$
- 函数空间：无穷维空间，元素是函数 $u(x)$

### 解空间和测试空间

**解空间 $V$**：
- 包含所有可能的解函数
- 满足狄利克雷边界条件：$V = \{u \mid u = \hat{u} \text{ on } \Gamma_D\}$

**测试空间 $V_0$**：
- 包含所有测试函数
- 在狄利克雷边界上为零：$V_0 = \{v \mid v = 0 \text{ on } \Gamma_D\}$

### 能量范数

弱形式与"能量最小化"原理等价，这在物理上有直观意义（最小势能原理）。

---

## 4. 加权残差法视角

### 另一种理解方式

**残差**：$R(u) = -\nabla^2 u - f$

强形式要求 $R(u) = 0$ 处处成立。

**加权残差法**：要求残差的加权积分为零

$$\int_\Omega v \cdot R(u) \, d\Omega = 0, \quad \forall v \in V_0$$

这就是弱形式的另一种表述。

### 不同加权函数的选择

| 方法 | 加权函数 | 特点 |
|------|----------|------|
| Galerkin 方法 | $v = N_i$（形函数） | 最常用，对称性好 |
| 配置法 | $v = \delta(x-x_i)$ | 在特定点满足 |
| 最小二乘法 | $v = \partial R/\partial u$ | 总是对称正定 |

**本项目使用 Galerkin 方法**。

---

## 5. Galerkin 离散化

### 有限维近似

将无限维空间 $V$ 用有限维空间 $V_h$ 近似：

$$u_h = \sum_{j=1}^{N} u_j N_j$$

其中：
- $N_j$ 是形函数
- $u_j$ 是节点处的未知值

### 离散弱形式

取测试函数 $v = N_i$，代入弱形式：

$$\int_\Omega \nabla N_i \cdot \nabla \left(\sum_{j=1}^{N} u_j N_j\right) d\Omega = \int_\Omega N_i f \, d\Omega$$

整理得：

$$\sum_{j=1}^{N} u_j \int_\Omega \nabla N_i \cdot \nabla N_j \, d\Omega = \int_\Omega N_i f \, d\Omega$$

这就是线性方程组 $Ku = F$！

其中：
- $K_{ij} = \int_\Omega \nabla N_i \cdot \nabla N_j \, d\Omega$（刚度矩阵）
- $F_i = \int_\Omega N_i f \, d\Omega$（荷载向量）

---

## 6. 非线性问题的弱形式

### 非线性泊松方程

$$-\nabla^2 u = f(u)$$

弱形式：

$$\int_\Omega \nabla v \cdot \nabla u \, d\Omega = \int_\Omega vf(u) \, d\Omega$$

### 牛顿线性化

定义残差：

$$R(u) = \int_\Omega \nabla v \cdot \nabla u \, d\Omega - \int_\Omega vf(u) \, d\Omega$$

牛顿迭代：

$$u_{n+1} = u_n + \delta u$$

线性化（泰勒展开）：

$$R(u_{n+1}) \approx R(u_n) + \frac{\partial R}{\partial u}\bigg|_{u_n} \delta u = 0$$

得到线性系统：

$$K_T \delta u = -R$$

其中切线刚度矩阵：

$$K_T = \underbrace{\int_\Omega \nabla N_i \cdot \nabla N_j \, d\Omega}_{\text{线性部分}} - \underbrace{\int_\Omega N_i N_j \frac{\partial f}{\partial u} \, d\Omega}_{\text{非线性修正}}$$

这就是 PDF 中的公式 1.5！

---

## 7. 程序员视角

### 弱形式在代码中的体现

```cpp
class WeakForm {
public:
    // 计算切线刚度矩阵（公式 1.5）
    void computeTangentMatrix(
        const Vector& u_current,  // 当前迭代值
        SparseMatrix& K_tangent   // 输出切线刚度矩阵
    ) {
        K_tangent.setZero();
        
        for (Element& elem : mesh.elements) {
            // 组装单元刚度矩阵
            Matrix Ke = computeElementTangentMatrix(elem, u_current);
            
            // 组装到全局矩阵
            assemble(Ke, elem, K_tangent);
        }
    }
    
    // 计算残差向量（公式 1.6 相关）
    void computeResidual(
        const Vector& u_current,
        Vector& residual
    ) {
        residual.setZero();
        
        for (Element& elem : mesh.elements) {
            Vector Fe = computeElementResidual(elem, u_current);
            assemble(Fe, elem, residual);
        }
    }
    
private:
    Matrix computeElementTangentMatrix(Element& elem, const Vector& u) {
        Matrix Ke(elem.numNodes, elem.numNodes);
        Ke.setZero();
        
        // 高斯积分
        for (GaussPoint gp : gaussPoints) {
            // 形函数值和梯度
            Vector N = elem.shapeValues(gp);
            Matrix dN = elem.shapeDerivatives(gp);
            
            // 当前点的 u 值
            double u_gp = N.dot(elem.extractNodalValues(u));
            
            // 源函数导数
            double df_du = sourceFunction.derivative(u_gp);
            
            // 雅可比
            double detJ = elem.jacobianDeterminant(gp);
            
            // 累加贡献
            for (int i = 0; i < elem.numNodes; i++) {
                for (int j = 0; j < elem.numNodes; j++) {
                    // 第一项：∇N_i · ∇N_j
                    double grad_term = dN.row(i).dot(dN.row(j));
                    
                    // 第二项：-N_i * N_j * ∂f/∂u
                    double nonlinear_term = -N[i] * N[j] * df_du;
                    
                    Ke(i,j) += gp.weight * (grad_term + nonlinear_term) * detJ;
                }
            }
        }
        
        return Ke;
    }
};
```

---

## 8. 学习检查清单

- [ ] 理解强形式和弱形式的区别
- [ ] 掌握分部积分的推导过程
- [ ] 理解为什么弱形式降低了光滑性要求
- [ ] 知道解空间和测试空间的定义
- [ ] 理解 Galerkin 方法的思想
- [ ] 能从弱形式推导出离散线性系统
- [ ] 理解非线性问题的线性化过程
- [ ] 能解释公式 1.5 和 1.6 的物理/数学意义

---

## 参考资源

1. **书籍**：
   - 《有限元方法》第 2 章（变分原理与弱形式）
   - 《Partial Differential Equations》Evans, Chapter 6

2. **视频**：
   - [Weak Form of PDEs - MIT OCW](https://www.youtube.com/watch?v=H4x7N1a1P-U)
   - [Galerkin Method Introduction](https://www.youtube.com/watch?v=1MHX2C7sKMw)

3. **在线教程**：
   - [Deal.II Tutorial - Weak Form](https://www.dealii.org/current/doxygen/deal.II/step_3.html)

---

## 练习题

1. 用分部积分推导一维泊松方程的弱形式
2. 解释为什么测试函数在狄利克雷边界上必须为零
3. 对比强形式和弱形式对解的光滑性要求
4. 推导非线性项的切线矩阵表达式
5. 思考：如果 $f(u) = e^{-u}$，写出弱形式和牛顿迭代的具体公式
