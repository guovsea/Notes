---
tags:
  - math/numerical-methods
  - type/notes
mastery: 5
---

# 牛顿法（Newton Method）学习笔记

> 参考：[知乎专栏](https://zhuanlan.zhihu.com/p/13304929959)

# 一、问题背景

我们希望求解：

- 方程求根： $f(x) = 0$
- 或多元方程组： $\mathbf{f}(\mathbf{x}) = \mathbf{0}$

核心思想：用**局部线性近似（泰勒展开）**替代原函数，逐步逼近解。

---

# 二、一元牛顿法

## 2.1 推导

在 $x_k$ 处做一阶泰勒展开：

$$ f(x) \approx f(x_k) + f'(x_k)(x - x_k) $$

令 $f(x) = 0$，解得迭代公式：

$$ x_{k+1} = x_k - \frac{f(x_k)}{f'(x_k)} $$

## 2.2 几何意义

在点 $(x_k, f(x_k))$ 处作切线，切线与 x 轴交点作为下一次迭代点。本质：**用切线代替曲线**。

## 2.3 平方根特例

求 $\sqrt{n}$，令 $f(x) = x^2 - n$，$f'(x) = 2x$：

$$ x_{k+1} = \frac{x_k + \frac{n}{x_k}}{2} $$

### 收敛过程示例（$\sqrt{8}$）

| 迭代 | x        | y        |
|------|----------|----------|
| 初始 | 8        | 4.5      |
| 1    | 4.5      | 3.1388   |
| 2    | 3.1388   | 2.8437   |
| 3    | 2.8437   | 2.8285   |
| 4    | 2.8285   | 2.828427 |

### Python 示例

```python
def newton(f, df, x0, tol=1e-6, max_iter=100):
    x = x0
    for _ in range(max_iter):
        x_new = x - f(x)/df(x)
        if abs(x_new - x) < tol:
            return x_new
        x = x_new
    return x

f = lambda x: x*x - 2
df = lambda x: 2*x
root = newton(f, df, 1.0)
print(root)  # 1.4142135623730954
```

---

# 三、多元牛顿法

## 3.1 问题形式

$$ \mathbf{f}(\mathbf{x}) = \begin{bmatrix} f_1(x_1,\dots,x_n) \\ \vdots \\ f_n(x_1,\dots,x_n) \end{bmatrix} = \mathbf{0} $$

## 3.2 Jacobian 矩阵

在 $\mathbf{x}_k$ 处展开：

$$ \mathbf{f}(\mathbf{x}) \approx \mathbf{f}(\mathbf{x}_k) + J(\mathbf{x}_k)(\mathbf{x} - \mathbf{x}_k) $$

其中 Jacobian 矩阵：

$$ J = \begin{bmatrix} \frac{\partial f_1}{\partial x_1} & \cdots & \frac{\partial f_1}{\partial x_n} \\ \vdots & & \vdots \\ \frac{\partial f_n}{\partial x_1} & \cdots & \frac{\partial f_n}{\partial x_n} \end{bmatrix} $$

## 3.3 迭代公式

$$ \mathbf{x}_{k+1} = \mathbf{x}_k - J^{-1}(\mathbf{x}_k)\mathbf{f}(\mathbf{x}_k) $$

### Python 示例

```python
import numpy as np

def newton_system(f, J, x0, tol=1e-10, max_iter=50):
    x = np.array(x0, dtype=float)
    for _ in range(max_iter):
        Fx = np.array(f(x))
        if np.linalg.norm(Fx) < tol:
            return x
        Jx = np.array(J(x))
        dx = np.linalg.solve(Jx, -Fx)
        x = x + dx
    return x

def f(x):
    return [x[0]**2 + x[1] - 3, x[0] + x[1]**2 - 3]

def J(x):
    return [[2*x[0], 1], [1, 2*x[1]]]

root = newton_system(f, J, [1, 1])
print(root)
```

---

# 四、优化问题中的牛顿法

## 4.1 从"求根"到"求极值"

极值条件 $\nabla f(x) = 0$ 可转化为求根问题。

## 4.2 迭代公式

$$ x_{k+1} = x_k - H^{-1}(x_k)\nabla f(x_k) $$

其中 $\nabla f$ 为梯度，$H$ 为 Hessian 矩阵（二阶导数矩阵）。

## 4.3 Hessian 矩阵

$$ H(x) = \begin{bmatrix} \frac{\partial^2 f}{\partial x_1^2} & \cdots \\ \vdots & \ddots \end{bmatrix} $$

几何意义：

| Hessian 性质 | 含义   |
|-------------|--------|
| 正定         | 极小值 |
| 负定         | 极大值 |
| 不定         | 鞍点   |

## 4.4 直观理解

| 信息          | 作用                |
|---------------|---------------------|
| 梯度 $\nabla f$ | 指方向              |
| Hessian $H$    | 控制步长 + 曲率修正 |

不只是"往下降"，而是"按曲面形状加速下降"。

---

# 五、收敛性

## 5.1 二次收敛

若初值足够接近解：$|e_{k+1}| \approx C |e_k|^2$，误差平方级下降。

## 5.2 优缺点

- **优点**：收敛极快（二次收敛），精度高
- **缺点**：依赖初值，需要导数/Jacobian/Hessian，可能不收敛

## 5.3 收敛条件

- 初值足够接近最优解
- Jacobian/Hessian 可逆
- Hessian 正定（用于最小值）

---

# 六、工程实现要点

## 6.1 永远不要显式求逆

理论公式：$x_{k+1} = x_k - J^{-1}F$

实际实现（解线性方程）：

$$ J(x_k)\Delta x = -F(x_k), \quad x_{k+1} = x_k + \Delta x $$

原因：
- **计算量**：矩阵求逆 $O(n^3)$，不如直接求解可复用分解
- **数值稳定**：求逆会放大误差
- **稀疏性**：工程中 $J$ 是稀疏矩阵，但 $J^{-1}$ 通常是稠密的

## 6.2 常用求解方法

- LU 分解、QR 分解
- Cholesky 分解（正定时）
- 稀疏求解器（Eigen / KLU）
- 迭代求解器（CG / GMRES）

## 6.3 C++ 工程算法结构

```cpp
while (!converged) {
    compute_residual(F, x);
    compute_jacobian(J, x);
    solve(J, dx, -F);  // 解线性方程 J * dx = -F
    x += dx;
}
```

---

# 七、常见改进方法

| 方法               | 作用             |
|--------------------|------------------|
| 阻尼牛顿法         | 防止发散         |
| Trust Region       | 限制更新范围     |
| 拟牛顿法（BFGS）   | 避免求 Hessian   |
| L-BFGS             | 大规模问题       |
| Gauss-Newton       | 最小二乘专用     |
| LM 方法            | 稳定性改进       |

## Gauss-Newton 方法

适用于最小二乘问题 $\min |r(x)|^2$：

$$ H \approx J^T J $$

不需要完整 Hessian，计算更简单且数值稳定。

## Levenberg-Marquardt（LM）

在梯度下降和牛顿法之间平滑过渡：

$$ (J^T J + \lambda I)\Delta x = -J^T r $$

工程中非常常用，稳定性好。

---

# 八、在 TCAD/PDE 中的应用

离散后非线性 PDE → $F(x) = 0$，使用牛顿法：

$$ J(x_k)\Delta x = -F(x_k) $$

| 优化          | 方程     |
|---------------|----------|
| Hessian       | Jacobian |
| 梯度          | 残差     |

在大规模问题中（$n$ 可达百万级），Jacobian 非常稀疏，必须使用稀疏矩阵存储和迭代求解器。

---

# 核心总结

> 牛顿法本质：用线性近似，逐步逼近非线性问题的解。在优化中，利用二阶曲率信息加速寻找梯度为 0 的点。

发展路径：一元 → 多元 → 大规模工程系统（Newton + Sparse Linear Solver）
