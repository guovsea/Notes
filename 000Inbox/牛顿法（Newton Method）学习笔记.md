
# 📘 牛顿法（Newton Method）学习笔记

> 参考：知乎文章整理与扩展 ([知乎专栏](https://zhuanlan.zhihu.com/p/13304929959?utm_source=chatgpt.com "牛顿法（Newton's Method) - 知乎"))

---

# 一、问题背景

我们希望求解：

- 方程求根：  
    $$  
    f(x) = 0  
    $$
    
- 或多元方程组：  
    $$  
    \mathbf{f}(\mathbf{x}) = \mathbf{0}  
    $$
    

牛顿法的核心思想：

👉 用**局部线性近似（泰勒展开）**替代原函数，从而逐步逼近解。

---

# 二、一元牛顿法

## 2.1 推导

在 $x_k$ 处做一阶泰勒展开：

$$  
f(x) \approx f(x_k) + f'(x_k)(x - x_k)  
$$

令 $f(x) = 0$，得到：

$$  
0 = f(x_k) + f'(x_k)(x - x_k)  
$$

解得：

$$  
x = x_k - \frac{f(x_k)}{f'(x_k)}  
$$

👉 得到迭代公式：

$$  
x_{k+1} = x_k - \frac{f(x_k)}{f'(x_k)}  
$$

---

## 2.2 几何意义

- 在点 $(x_k, f(x_k))$ 处作切线
    
- 切线与 x 轴交点作为下一次迭代点
    

👉 本质：**用切线代替曲线**

---

## 2.3 Python 示例

```python
def newton(f, df, x0, tol=1e-6, max_iter=100):
    x = x0
    for _ in range(max_iter):
        x_new = x - f(x)/df(x)
        if abs(x_new - x) < tol:
            return x_new
        x = x_new
    return x

# 示例：求 sqrt(2)
f = lambda x: x*x - 2
df = lambda x: 2*x

root = newton(f, df, 1.0)
print(root)
```

---

# 三、多元牛顿法

## 3.1 问题形式

求解：

$$  
\mathbf{f}(\mathbf{x}) =  
\begin{bmatrix}  
f_1(x_1,\dots,x_n) \  
\vdots \  
f_n(x_1,\dots,x_n)  
\end{bmatrix}  
= \mathbf{0}  
$$

---

## 3.2 泰勒展开

在 $\mathbf{x}_k$ 处展开：

$$  
\mathbf{f}(\mathbf{x}) \approx  
\mathbf{f}(\mathbf{x}_k) + J(\mathbf{x}_k)(\mathbf{x} - \mathbf{x}_k)  
$$

其中：

- $J(\mathbf{x})$：Jacobian 矩阵
    

$$  
J =  
\begin{bmatrix}  
\frac{\partial f_1}{\partial x_1} & \cdots & \frac{\partial f_1}{\partial x_n} \  
\vdots & & \vdots \  
\frac{\partial f_n}{\partial x_1} & \cdots & \frac{\partial f_n}{\partial x_n}  
\end{bmatrix}  
$$

---

## 3.3 迭代公式

令 $\mathbf{f}(\mathbf{x}) = 0$：

$$  
J(\mathbf{x}_k)(\mathbf{x}_{k+1} - \mathbf{x}_k)  
= -\mathbf{f}(\mathbf{x}_k)  
$$

👉 得到：

$$  
\mathbf{x}_{k+1} = \mathbf{x}_k - J^{-1}(\mathbf{x}_k)\mathbf{f}(\mathbf{x}_k)  
$$

---

## 3.4 工程实现（关键！）

实际中**不会求逆矩阵**，而是解线性方程：

$$  
J(\mathbf{x}_k)\Delta \mathbf{x} = -\mathbf{f}(\mathbf{x}_k)  
$$

然后：

$$  
\mathbf{x}_{k+1} = \mathbf{x}_k + \Delta \mathbf{x}  
$$

👉 优点：

- 数值稳定
    
- 计算更高效
    
- 可用 LU / QR 分解
    

---

## 3.5 Python 示例

```python
import numpy as np

def newton_multi(f, J, x0, tol=1e-6, max_iter=50):
    x = np.array(x0, dtype=float)

    for _ in range(max_iter):
        Fx = np.array(f(x))
        Jx = np.array(J(x))

        # 解线性方程 J * dx = -F
        dx = np.linalg.solve(Jx, -Fx)

        x_new = x + dx

        if np.linalg.norm(dx) < tol:
            return x_new

        x = x_new

    return x

# 示例
def f(x):
    return [
        x[0]**2 + x[1] - 3,
        x[0] + x[1]**2 - 3
    ]

def J(x):
    return [
        [2*x[0], 1],
        [1, 2*x[1]]
    ]

root = newton_multi(f, J, [1, 1])
print(root)
```

---

# 四、优化问题中的牛顿法

## 4.1 从“求根”到“求极值”

极值条件：

$$  
\nabla f(x) = 0  
$$

 可转化为求根问题

---

## 4.2 迭代公式

$$  
x_{k+1} = x_k - H^{-1}(x_k)\nabla f(x_k)  
$$

其中：

- $\nabla f$：梯度
    
- $H$：Hessian 矩阵
    

> Hessian 矩阵 = 梯度的导数（也就是二阶导数矩阵）
---

## 4.3 Hessian 条件

- 必须可逆
    
- 最好正定（保证极小值）
    

---

[[优化问题中的牛顿法（Newton Method for Optimization）]]

# 五、收敛性

## 5.1 二次收敛

若初值足够接近解：

$$  
|e_{k+1}| \approx C |e_k|^2  
$$

👉 误差平方级下降

---

## 5.2 优点

- 收敛速度极快（二次收敛）
    
- 精度高
    

---

## 5.3 缺点

- 依赖初值
    
- 需要导数 / Jacobian / Hessian
    
- 可能不收敛
    

---

# 六、工程实践总结（重点）

结合你做 C++/数值软件，这部分最重要：

### ✅ 推荐实现方式

1. **永远不要显式求逆**
    
    ```cpp
    solve(J, -F)
    ```
    
2. 使用：
    
    - LU 分解
        
    - QR 分解
        
    - 稀疏矩阵求解器（Eigen / KLU）
        
3. 加入：
    
    - 阻尼（Line Search）
        
    - Trust Region
        

---

### ✅ 常见改进方法

|方法|作用|
|---|---|
|阻尼牛顿法|防止发散|
|拟牛顿法（BFGS）|避免求 Hessian|
|数值 Jacobian|避免手推导数|

---

# 七、核心总结

👉 牛顿法本质：

> **用线性近似，逐步逼近非线性问题的解**

---

如果你愿意，我可以帮你再做一版 **“C++工程级牛顿法框架（支持稀疏矩阵 + 非线性求解器设计）”**，会更贴近你现在在做的 TCAD / 求解器系统。