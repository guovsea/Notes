## 从 (1.5) 到 (1.6) 的推导（Galerkin 方法）

### 🔹 第一步：定义残差

原方程：

$$  
a \frac{d^2}{dx^2}[N]{u} + b = \psi  
$$

定义残差：

$$  
R = a \frac{d^2}{dx^2}[N]{u} + b - \psi  
$$

由于 $\psi$ 表示残差项，可简化理解为：

$$  
R = a \frac{d^2}{dx^2}[N]{u} + b  
$$

---

### 🔹 第二步：Galerkin 加权余量法

Galerkin 方法要求：

> 残差在加权意义下为 0（权函数取形函数）

$$  
\int_{x_1}^{x_2} [N]^T \cdot R dx = 0  
$$

---

### 🔹 第三步：代入残差表达式

$$  
\int_{x_1}^{x_2} [N]^T \left( a \frac{d^2}{dx^2}[N]{u} + b \right) dx = 0  
$$

---

### 🔹 第四步：拆分积分

利用积分的线性性：

$$  
\int_{x_1}^{x_2} [N]^T a \frac{d^2}{dx^2}[N]{u} \cdot dx  
+  
\int_{x_1}^{x_2} [N]^T b \cdot dx  
= 0  
$$

这就是式 (1.6)