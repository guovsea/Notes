
---

# 1. 一元牛顿迭代的数学原理

## 1.1 问题形式

我们希望求解：

$$  
f(x) = 0  
$$

例如求平方根：

$$  
x^2 - n = 0  
$$

---

## 1.2 核心思想（切线逼近）

牛顿法的核心思想：

> 用当前点的切线去逼近函数的零点

在当前点 $x_k$ 处做切线：

$$  
y = f(x_k) + f'(x_k)(x - x_k)  
$$

令 $y = 0$，得到下一次迭代点：

$$  
x_{k+1} = x_k - \frac{f(x_k)}{f'(x_k)}  
$$

---

## 1.3 直观理解

- 当前猜测：$x$
    
- 函数值：$f(x)$
    
- 斜率：$f'(x)$
    

更新：

$$  
\text{新值} = \text{当前值} - \frac{\text{偏差}}{\text{斜率}}  
$$

---

## 1.4 平方根特例

求 $\sqrt{n}$：

$$  
f(x) = x^2 - n, \quad f'(x) = 2x  
$$

代入：

$$  
x_{k+1} = x_k - \frac{x_k^2 - n}{2x_k}  
$$

化简：

$$  
x_{k+1} = \frac{x_k + \frac{n}{x_k}}{2}  
$$

---

# 2. 最小示例（Python实现）

## 2.1 代码

```python
def sqrt_newton(n, tolerance=1e-15, max_iter=1000):
    if n < 0:
        raise ValueError("n must be non-negative")

    # 初始猜测值
    current_estimate = n if n != 0 else 1.0

    for _ in range(max_iter):
        # 根据牛顿公式计算下一次估计值
        next_estimate = 0.5 * (current_estimate + n / current_estimate)

        # 判断是否收敛
        if abs(current_estimate - next_estimate) < tolerance:
            return next_estimate

        current_estimate = next_estimate

    # 如果超过最大迭代次数仍未收敛
    raise RuntimeError("Failed to converge")

if __name__ == "__main__":
    n = 8
    result = sqrt_newton(n)
    print("sqrt(", n, ") =", result)
```

---

## 2.2 算法流程

- 初始化 $x$
    
- 计算 $y = \frac{x + n/x}{2}$
    
- 判断 $|x - y|$ 是否足够小
    
- 若不满足：$x = y$，继续迭代

---

## 2.3 示例收敛过程（$\sqrt{8}$）
| 迭代  | x      | y        |
| --- | ------ | -------- |
| 初始  | 8      | 4.5      |
| 1   | 4.5    | 3.1388   |
| 2   | 3.1388 | 2.8437   |
| 3   | 2.8437 | 2.8285   |
| 4   | 2.8285 | 2.828427 |

---

# 3. 推广到多元情况

## 3.1 问题形式

从一元：

$$  
f(x) = 0  
$$

推广到多元：

$$  
F(x) = 0  
$$

其中：

$$  
x = \begin{bmatrix} x_1 \ x_2 \ \vdots \ x_n \end{bmatrix}, \quad  
F(x) = \begin{bmatrix} f_1(x) \ f_2(x) \ \vdots \ f_n(x) \end{bmatrix}  
$$

---

## 3.2 雅可比矩阵

一元：

$$  
f'(x)  
$$

多元：

$$  
J(x) = \begin{bmatrix}  
\frac{\partial f_1}{\partial x_1} & \frac{\partial f_1}{\partial x_2} & \cdots \  
\frac{\partial f_2}{\partial x_1} & \frac{\partial f_2}{\partial x_2} & \cdots \  
\vdots & \vdots & \ddots  
\end{bmatrix}  
$$

---

## 3.3 牛顿迭代公式（多元）

理论形式：

$$  
x_{k+1} = x_k - J(x_k)^{-1} F(x_k)  
$$

---

## 3.4 等价形式（实际使用）

引入增量：

$$  
\Delta x = x_{k+1} - x_k  
$$

得到线性方程：

$$  
J(x_k) \Delta x = -F(x_k)  
$$

更新：

$$  
x = x + \Delta x  
$$

---

## 3.5 示例代码（多元牛顿法）

```python
import numpy as np


def F(x):
    x1, x2 = x
    return np.array([
        x1**2 + x2**2 - 4,
        x1 - x2 - 1
    ])


def J(x):
    x1, x2 = x
    return np.array([
        [2*x1, 2*x2],
        [1, -1]
    ])


def newton_system(x0, tol=1e-10, max_iter=50):
    x = np.array(x0, dtype=float)

    for i in range(max_iter):
        Fx = F(x)

        if np.linalg.norm(Fx) < tol:
            return x

        Jx = J(x)

        dx = np.linalg.solve(Jx, -Fx)

        x = x + dx

    return x


i

$$  
\text{solve}(J, -F)  
$$

---

## 4.2 等价变换

$$  
\Delta x = -J^{-1}F  
$$

等价于：

$$  
J \Delta x = -F  
$$

---

## 4.3 为什么不求逆

### (1) 计算量大

$$  
\text{矩阵求逆复杂度} \approx O(n^3)  
$$

$$  
\text{线性方程求解} \approx O(n^3) \text{（但更高效且可复用分解）}  
$$

---

### (2) 数值不稳定

求逆会放大误差：

$$  
\text{误差传播更严重}  
$$

经验原则：

$$  
\text{Never\ compute\ the\ inverse\ explicitly}  
$$

---

### (3) 稀疏矩阵问题

工程问题中：

$$  
J \text{ 是稀疏矩阵}  
$$

但：

$$  
J^{-1} \text{ 通常是稠密矩阵}  
$$

导致：

$$  
\text{内存和计算量爆炸}  
$$

---

## 4.4 工程算法结构

$$  
\begin{aligned}  
\text{loop:} \  
&1.\ \text{组装残差 } F(x) \  
&2.\ \text{组装 Jacobian } J(x) \  
&3.\ \text{求解 } J \Delta x = -F \  
&4.\ x = x + \Delta x \  
&5.\ \text{判断收敛}  
\end{aligned}  
$$

---

## 4.5 常见线性求解方法

$$  
\text{LU},\ \text{GMRES},\ \text{CG},\ \text{Sparse Direct Solver}  
$$

---

# 总结

牛顿法的统一本质：

$$  
\text{用线性近似（切线/切平面）不断逼近非线性方程的解}  
$$

发展路径：

$$  
\text{一元} \rightarrow \text{多元} \rightarrow \text{大规模工程系统}  
$$

实现差异：

$$  
\text{数学：} J^{-1} \quad \text{工程：} \text{solve}(J, b)  
$$

但核心思想始终一致。

---

如果继续深入（TCAD / FEM）：

$$  
\text{Newton + Sparse Linear Solver}  
$$

这就是现代数值仿真的核心结构。