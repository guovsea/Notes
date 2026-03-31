# 单元刚度矩阵推导详解

> 目标：完整推导公式 1.5 和 1.6，并给出离散化后的可计算形式

---

## 1. 回顾弱形式

从弱形式出发：

$$\int_\Omega \nabla v \cdot \nabla u \, d\Omega = \int_\Omega vf(u) \, d\Omega$$

### 牛顿线性化

设当前迭代解为 $u_n$，更新 $\delta u$：

$$u_{n+1} = u_n + \delta u$$

对非线性项 $f(u)$ 做泰勒展开：

$$f(u_{n+1}) \approx f(u_n) + \frac{\partial f}{\partial u}\bigg|_{u_n} \delta u$$

---

## 2. 切线刚度矩阵推导

### 残差定义

$$R(u) = \int_\Omega \nabla v \cdot \nabla u \, d\Omega - \int_\Omega vf(u) \, d\Omega$$

线性化残差方程：

$$\frac{\partial R}{\partial u}\delta u = -R(u_n)$$

### 计算切线算子

对残差关于 $u$ 求导：

$$\frac{\partial R}{\partial u} = \int_\Omega \nabla v \cdot \nabla (\cdot) \, d\Omega - \int_\Omega v \frac{\partial f}{\partial u} (\cdot) \, d\Omega$$

应用 Galerkin 离散（$v = N_i$，$\delta u = \sum_j N_j \delta u_j$）：

$$\sum_j \left[\int_\Omega \nabla N_i \cdot \nabla N_j \, d\Omega - \int_\Omega N_i N_j \frac{\partial f}{\partial u} \, d\Omega\right] \delta u_j = -R_i$$

这就是矩阵形式：$K_T \delta u = -R$

### 单元刚度矩阵（公式 1.5）

$$k_{ij}^{(e)} = -\underbrace{\int_{\Omega_K} \nabla N_i \cdot \nabla N_j \, d\Omega}_{\text{扩散项}} - \underbrace{\int_{\Omega_K} N_i N_j \frac{\partial f}{\partial u} \, d\Omega}_{\text{非线性项}}$$

**注意**：PDF 公式 1.5 中的负号是全局矩阵组装后的结果，实际是：

$$k_{ij}^{(e)} = \int_{\Omega_K} \nabla N_i \cdot \nabla N_j \, d\Omega - \int_{\Omega_K} N_i N_j \frac{\partial f}{\partial u} \, d\Omega$$

---

## 3. 荷载向量推导（公式 1.6）

### 残差向量

$$R_i = \int_\Omega \nabla N_i \cdot \nabla u_n \, d\Omega - \int_\Omega N_i f(u_n) \, d\Omega$$

### 单元荷载向量（公式 1.6）

$$f_i^{(e)} = \underbrace{\int_{\Omega_K} \nabla N_i \cdot \nabla u_n \, d\Omega}_{\text{当前解的贡献}} + \underbrace{\int_{\Omega_K} f(u_n) N_i \, d\Omega}_{\text{源项贡献}}$$

---

## 4. 完全离散化

### 4.1 三角形单元的离散化

#### 形函数梯度（常数）

对于一阶三角形单元，形函数梯度在单元内是常数：

$$\frac{\partial N_i}{\partial x} = \frac{y_j - y_k}{2A}, \quad \frac{\partial N_i}{\partial y} = \frac{x_k - x_j}{2A}$$

其中 $(i,j,k)$ 是循环排列，$A$ 是三角形面积。

#### 第一项计算

$$\int_{\Omega_K} \nabla N_i \cdot \nabla N_j \, d\Omega = A \cdot \left(\frac{\partial N_i}{\partial x}\frac{\partial N_j}{\partial x} + \frac{\partial N_i}{\partial y}\frac{\partial N_j}{\partial y}\right)$$

由于梯度是常数，可以直接解析计算！

#### 第二项计算

$$\int_{\Omega_K} N_i N_j \frac{\partial f}{\partial u} \, d\Omega$$

需要用数值积分（形函数不是常数）：

**1 点高斯积分**（三角形单元）：
- 积分点：重心 $(\frac{1}{3}, \frac{1}{3}, \frac{1}{3})$
- 权重：1
- 形函数值：$N_1 = N_2 = N_3 = \frac{1}{3}$

```cpp
// 三角形单元 1 点高斯积分
const double gauss_tri_point[2] = {1.0/3.0, 1.0/3.0};  // 面积坐标
const double gauss_tri_weight = 1.0;

// 计算第二项
double N[3] = {1.0/3.0, 1.0/3.0, 1.0/3.0};
double u_at_gp = 0;
for (int i = 0; i < 3; i++) u_at_gp += N[i] * u_node[i];
double df_du = source_derivative(u_at_gp);

for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
        k_nonlinear[i][j] = N[i] * N[j] * df_du * A;  // A 是单元面积
    }
}
```

### 4.2 四边形单元的离散化

#### 等参变换

在标准单元 $[-1,1]\times[-1,1]$ 上计算，需要雅可比矩阵：

$$J = \begin{bmatrix} \frac{\partial x}{\partial s} & \frac{\partial x}{\partial t} \\ \frac{\partial y}{\partial s} & \frac{\partial y}{\partial t} \end{bmatrix}$$

$$d\Omega = |J| \, dsdt$$

#### 梯度转换

$$\begin{bmatrix} \frac{\partial N_i}{\partial x} \\ \frac{\partial N_i}{\partial y} \end{bmatrix} = J^{-1} \begin{bmatrix} \frac{\partial N_i}{\partial s} \\ \frac{\partial N_i}{\partial t} \end{bmatrix}$$

#### 2×2 高斯积分

```cpp
// 计算单元刚度矩阵（四边形）
Matrix4d computeQuadStiffnessMatrix(Element& elem, const Vector4d& u) {
    Matrix4d K = Matrix4d::Zero();
    
    // 2x2 高斯点
    const double gp[2] = {-0.577350269189626, 0.577350269189626};
    const double w[2] = {1.0, 1.0};
    
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            double s = gp[i], t = gp[j];
            double weight = w[i] * w[j];
            
            // 1. 形函数在 (s,t) 处的值
            Vector4d N;
            N << 0.25*(1-s)*(1-t), 0.25*(1+s)*(1-t),
                 0.25*(1+s)*(1+t), 0.25*(1-s)*(1+t);
            
            // 2. 形函数对 s,t 的导数
            Matrix<double, 4, 2> dN_st;
            dN_st << -0.25*(1-t), -0.25*(1-s),
                      0.25*(1-t), -0.25*(1+s),
                      0.25*(1+t),  0.25*(1+s),
                     -0.25*(1+t),  0.25*(1-s);
            
            // 3. 雅可比矩阵
            Matrix2d J = dN_st.transpose() * elem.coords;  // 2x2
            double detJ = J.determinant();
            Matrix2d Jinv = J.inverse();
            
            // 4. 形函数对 x,y 的导数 (4x2)
            Matrix<double, 4, 2> dN_xy = dN_st * Jinv.transpose();
            
            // 5. 当前 u 值
            double u_gp = N.dot(u);
            double df_du = source_derivative(u_gp);
            
            // 6. 累加刚度矩阵
            for (int a = 0; a < 4; a++) {
                for (int b = 0; b < 4; b++) {
                    // 扩散项
                    double diff_term = dN_xy.row(a).dot(dN_xy.row(b));
                    
                    // 非线性项
                    double nonlin_term = N[a] * N[b] * df_du;
                    
                    K(a,b) += weight * (diff_term - nonlin_term) * detJ;
                }
            }
        }
    }
    
    return K;
}
```

---

## 5. 荷载向量的计算

### 公式 1.6 的离散化

$$f_i^{(e)} = \int_{\Omega_K} \nabla N_i \cdot \nabla u_n \, d\Omega + \int_{\Omega_K} f(u_n) N_i \, d\Omega$$

#### 第一项（扩散项）

这是当前解的梯度与形函数梯度的点积积分：

```cpp
// 计算梯度 ∇u_n
Vector2d grad_u(0, 0);
for (int j = 0; j < numNodes; j++) {
    grad_u[0] += dN_xy(j, 0) * u_node[j];  // du/dx
    grad_u[1] += dN_xy(j, 1) * u_node[j];  // du/dy
}

// 计算第一项贡献
for (int i = 0; i < numNodes; i++) {
    Vector2d grad_Ni(dN_xy(i, 0), dN_xy(i, 1));
    f[i] += weight * grad_Ni.dot(grad_u) * detJ;
}
```

#### 第二项（源项）

```cpp
// 当前点的源函数值
double f_source = source_function(u_gp);

for (int i = 0; i < numNodes; i++) {
    f[i] += weight * f_source * N[i] * detJ;
}
```

### 完整代码

```cpp
Vector4d computeQuadLoadVector(Element& elem, const Vector4d& u) {
    Vector4d F = Vector4d::Zero();
    
    // 2x2 高斯积分（同上）
    for (每个高斯点) {
        // ... 计算 N, dN_xy, detJ, u_gp ...
        
        double f_val = source_function(u_gp);  // f(u_n)
        
        // 计算 ∇u_n
        Vector2d grad_u = dN_xy.transpose() * u;
        
        for (int i = 0; i < 4; i++) {
            // 第一项：∇N_i · ∇u_n
            double term1 = dN_xy.row(i).dot(grad_u);
            
            // 第二项：f(u_n) * N_i
            double term2 = f_val * N[i];
            
            F[i] += weight * (term1 + term2) * detJ;
        }
    }
    
    return F;
}
```

---

## 6. 算法流程总结

### 单元刚度矩阵计算算法

```
输入：单元信息，当前解 u_node[]
输出：单元刚度矩阵 K[][]

1. 初始化 K = 0
2. 对每个高斯点 gp:
   a. 计算形函数值 N[]
   b. 计算形函数梯度 dN/ds, dN/dt
   c. 计算雅可比矩阵 J 和 detJ
   d. 计算形函数梯度 dN/dx, dN/dy
   e. 计算当前 u 值：u_gp = Σ N[i]*u_node[i]
   f. 计算 ∂f/∂u 在 u_gp 处的值
   g. 对每个 i, j:
      K[i][j] += w * (dN_i·dN_j - N_i*N_j*∂f/∂u) * detJ
3. 返回 K
```

### 单元荷载向量计算算法

```
输入：单元信息，当前解 u_node[]
输出：单元荷载向量 F[]

1. 初始化 F = 0
2. 对每个高斯点 gp:
   a. 计算形函数值 N[]
   b. 计算形函数梯度 dN/dx, dN/dy（同上）
   c. 计算当前 u 值：u_gp
   d. 计算 f(u_gp) 和 ∇u_n
   e. 对每个 i:
      F[i] += w * (dN_i·∇u_n + f(u_gp)*N_i) * detJ
3. 返回 F
```

---

## 7. 学习检查清单

- [ ] 理解切线刚度矩阵的两项来源
- [ ] 能推导公式 1.5 的完整过程
- [ ] 能推导公式 1.6 的完整过程
- [ ] 掌握三角形单元梯度的解析计算
- [ ] 掌握四边形单元的等参变换
- [ ] 能写出完整的离散化代码
- [ ] 理解雅可比行列式的几何意义（面积缩放因子）

---

## 参考资源

1. **书籍**：
   - 《有限元方法》第 4 章（单元推导）
   - 《Nonlinear Finite Element Methods》Wriggers, Chapter 4

2. **代码参考**：
   - [deal.II Step-6](https://www.dealii.org/current/doxygen/deal.II/step_6.html)（非线性泊松方程）

---

## 练习题

1. 验证三角形单元 1 点高斯积分对 $\int N_i N_j d\Omega$ 的精度
2. 证明当四边形是矩形时，雅可比矩阵是常数对角矩阵
3. 推导线性与非线性项对刚度矩阵的具体贡献
4. 编写代码计算一个具体单元的刚度矩阵，并验证对称性
5. 思考：如果源函数是 $f(u) = e^{-u}$，写出牛顿迭代的具体计算步骤
