# 高斯-勒让德数值积分详解

> 目标：理解高斯积分的原理、实现方法，以及在有限元中的应用

---

## 1. 为什么需要数值积分？

### 问题背景

在有限元计算中，我们需要计算如下形式的积分：

$$\int_{\Omega} f(x, y) \, d\Omega$$

其中被积函数 $f$ 可能：
- 包含复杂的源函数
- 涉及形函数及其导数
- 在等参变换后形式更复杂

**解析积分通常不可行！**

### 数值积分的基本思想

用有限个点的函数值加权求和来近似积分：

$$\int_{a}^{b} f(x) \, dx \approx \sum_{i=1}^{n} w_i f(x_i)$$

---

## 2. 高斯-勒让德积分原理

### 核心思想

**选择最优的积分点和权重，使得对尽可能高次的多项式精确。**

### 关键定理

> **n 个高斯点可以精确积分 2n-1 次多项式**

这意味着：
- 1 个点：精确积分 1 次多项式（线性函数）
- 2 个点：精确积分 3 次多项式（三次函数）
- 3 个点：精确积分 5 次多项式

### 与等距点对比

梯形法则（等距点）：
- 2 个点只能精确积分 1 次多项式
- 效率远低于高斯积分

---

## 3. 一维高斯积分

### 标准区间

高斯积分定义在标准区间 $[-1, 1]$ 上：

$$\int_{-1}^{1} f(x) \, dx \approx \sum_{i=1}^{n} w_i f(\xi_i)$$

### 常用高斯点数据

#### 1 点公式（精度：1 次）

| 点 $\xi_i$ | 权重 $w_i$ |
|-----------|-----------|
| 0 | 2 |

公式：$\int_{-1}^{1} f(x) dx \approx 2f(0)$

#### 2 点公式（精度：3 次）

| 点 $\xi_i$ | 权重 $w_i$ |
|-----------|-----------|
| $-1/\sqrt{3} \approx -0.577$ | 1 |
| $+1/\sqrt{3} \approx +0.577$ | 1 |

公式：$\int_{-1}^{1} f(x) dx \approx f(-1/\sqrt{3}) + f(1/\sqrt{3})$

#### 3 点公式（精度：5 次）

| 点 $\xi_i$ | 权重 $w_i$ |
|-----------|-----------|
| 0 | 8/9 ≈ 0.889 |
| $\pm\sqrt{3/5} \approx \pm 0.775$ | 5/9 ≈ 0.556 |

公式：$\int_{-1}^{1} f(x) dx \approx \frac{8}{9}f(0) + \frac{5}{9}[f(-0.775) + f(0.775)]$

### 程序员实现

```cpp
// 高斯点和权重数据（2点公式）
const int N_GAUSS_POINTS = 2;
const double gauss_points[N_GAUSS_POINTS] = {
    -0.5773502691896258,  // -1/sqrt(3)
     0.5773502691896258   // +1/sqrt(3)
};
const double gauss_weights[N_GAUSS_POINTS] = {
    1.0, 1.0
};

// 一维高斯积分
double gaussQuadrature1D(double (*f)(double), int n_points) {
    double result = 0.0;
    for (int i = 0; i < n_points; i++) {
        result += gauss_weights[i] * f(gauss_points[i]);
    }
    return result;
}

// 示例：积分 f(x) = x^2
// 精确值 = 2/3 ≈ 0.6667
// 2点高斯积分：f(-1/sqrt(3)) + f(1/sqrt(3)) = 1/3 + 1/3 = 2/3 ✓
```

### 任意区间变换

对于区间 $[a, b]$，需要线性变换：

$$x = \frac{b-a}{2}\xi + \frac{a+b}{2}, \quad dx = \frac{b-a}{2}d\xi$$

$$
\int_{a}^{b} f(x) dx = \frac{b-a}{2} \int_{-1}^{1} f\left(\frac{b-a}{2}\xi + \frac{a+b}{2}\right) d\xi
$$

```cpp
double gaussQuadrature1D(double (*f)(double), double a, double b, int n_points) {
    double result = 0.0;
    double scale = (b - a) / 2.0;
    double shift = (a + b) / 2.0;
    
    for (int i = 0; i < n_points; i++) {
        double x = scale * gauss_points[i] + shift;
        result += gauss_weights[i] * f(x);
    }
    return result * scale;
}
```

---

## 4. 二维高斯积分

### 矩形区域

$$
\int_{-1}^{1}\int_{-1}^{1} f(s, t) \, dsdt \approx \sum_{i=1}^{n}\sum_{j=1}^{m} w_i w_j f(\xi_i, \eta_j)
$$

这是两个一维积分的张量积。

### 程序员实现

```cpp
// 二维高斯积分（使用 2x2 点）
double gaussQuadrature2D(double (*f)(double, double), int n) {
    double result = 0.0;
    
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            result += gauss_weights[i] * gauss_weights[j] 
                    * f(gauss_points[i], gauss_points[j]);
        }
    }
    return result;
}

// 等参空间中的积分（考虑雅可比行列式）
double integrateOverElement(
    double (*integrand)(double, double),  // 在 (s,t) 处的被积函数
    double (*jacobianDet)(double, double), // 雅可比行列式 |J|
    int n
) {
    double result = 0.0;
    
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            double s = gauss_points[i];
            double t = gauss_points[j];
            double weight = gauss_weights[i] * gauss_weights[j];
            double detJ = jacobianDet(s, t);
            
            result += weight * integrand(s, t) * detJ;
        }
    }
    return result;
}
```

### 2×2 高斯点分布图

```
t ↑
  |
1 ●-------●
  |   ↑   |
  |   ●---|--→ s  (4个高斯点)
  |       |
-1●-------●
  |
 -1       1

点坐标：
(-0.577, -0.577), (+0.577, -0.577)
(-0.577, +0.577), (+0.577, +0.577)
```

---

## 5. 在有限元中的应用

### 单元刚度矩阵计算

回顾公式 1.5：

$$k_{ij}^{(e)} = -\int_{\Omega_K} \nabla N_i \cdot \nabla N_j \, d\Omega - \int_{\Omega_K} N_i N_j \frac{\partial f}{\partial u} \, d\Omega$$

#### 计算步骤

1. **等参变换**：将实际单元映射到标准单元
2. **雅可比计算**：计算 $|J|$ 和 $J^{-1}$
3. **高斯积分**：在标准单元上数值积分

```cpp
void computeElementStiffnessMatrix(Element& elem, double K[4][4]) {
    // 初始化为 0
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            K[i][j] = 0;
    
    // 2x2 高斯积分
    for (int gi = 0; gi < 2; gi++) {
        for (int gj = 0; gj < 2; gj++) {
            double s = gauss_points[gi];
            double t = gauss_points[gj];
            double w = gauss_weights[gi] * gauss_weights[gj];
            
            // 1. 计算雅可比行列式
            double J[2][2], detJ;
            elem.jacobian(s, t, J);
            detJ = J[0][0]*J[1][1] - J[0][1]*J[1][0];
            
            // 2. 计算形函数对 x,y 的导数
            double dN[4][2];  // dN[i][0]=dN_i/dx, dN[i][1]=dN_i/dy
            elem.shapeFunctionDerivativesXY(s, t, dN);
            
            // 3. 计算形函数值（用于第二项）
            double N[4];
            elem.shapeFunctions(s, t, N);
            
            // 4. 累加贡献
            for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 4; j++) {
                    // 第一项：-∇N_i · ∇N_j
                    double grad_dot = dN[i][0]*dN[j][0] + dN[i][1]*dN[j][1];
                    
                    // 第二项：-N_i * N_j * ∂f/∂u
                    // 假设已知 df_du 在当前点的值
                    double nonlinear_term = N[i] * N[j] * df_du;
                    
                    K[i][j] += w * (grad_dot + nonlinear_term) * detJ;
                }
            }
        }
    }
}
```

### 如何选择积分点数？

| 被积函数特点 | 推荐点数 | 原因 |
|-------------|---------|------|
| 一阶三角形单元刚度（线性形函数梯度） | 1 点 | 梯度为常数 |
| 一阶四边形单元刚度（双线性形函数） | 2×2 点 | 需要精确积分双线性函数乘积 |
| 含高次源函数 | 增加点数 | 源函数可能非多项式 |

**本项目建议**：
- 三角形单元：1 点积分（形函数梯度是常数）
- 四边形单元：2×2 点积分

---

## 6. 使用 Boost.Math 获取高斯点

Boost 库提供了方便的高斯点计算功能：

```cpp
#include <boost/math/quadrature/gauss.hpp>

// 使用 4 点高斯-勒让德积分
typedef boost::math::quadrature::gauss<double, 4> gauss_rule;

// Boost 只返回非负的高斯点
// 对于 4 点，返回 2 个点（正数）和对应的权重
std::pair<std::array<double, 2>, std::array<double, 2>> data = 
    gauss_rule::abscissa_and_weights();

// data.first = {0.339981, 0.861136}  // 正的高斯点
// data.second = {0.652145, 0.347855} // 对应的权重

// 完整的高斯点（包括负数）
double full_points[4] = {
    -data.first[1], -data.first[0], data.first[0], data.first[1]
};
double full_weights[4] = {
    data.second[1], data.second[0], data.second[0], data.second[1]
};
```

---

## 7. 精度验证

### 测试方法

```cpp
// 测试 1：积分常数
// ∫∫ 1 dsdt = 4（标准单元面积）
auto f1 = [](double s, double t) { return 1.0; };
double result1 = gaussQuadrature2D(f1, 2);
// 期望：4.0

// 测试 2：积分 s^2
// ∫_{-1}^{1} s^2 ds = 2/3
// 2 点高斯积分结果 = 2 * (1/sqrt(3))^2 = 2/3 ✓
auto f2 = [](double s, double t) { return s*s; };
double result2 = gaussQuadrature2D(f2, 2);
// 期望：4/3（因为还有 t 积分 ∫1 dt = 2）

// 测试 3：积分 s^2 * t^2（4 次多项式）
// 精确值：(2/3) * (2/3) = 4/9
// 2 点高斯积分不够精确（只能积分 3 次）
// 需要 3 点高斯积分
```

---

## 8. 学习检查清单

- [ ] 理解数值积分的必要性
- [ ] 掌握 n 点高斯积分的精度（2n-1 次多项式）
- [ ] 能写出 1 点和 2 点高斯积分公式
- [ ] 理解区间变换的原理
- [ ] 掌握二维高斯积分（张量积）
- [ ] 理解雅可比行列式在等参积分中的作用
- [ ] 能在代码中实现单元矩阵的高斯积分计算
- [ ] 知道如何选择合适的积分点数

---

## 参考资源

1. **在线高斯点计算器**：
   - [Gauss-Legendre Quadrature Calculator](https://pomax.github.io/bezierinfo/legendre-gauss.html)

2. **Boost 文档**：
   - [Boost.Math Quadrature](https://www.boost.org/doc/libs/1_85_0/libs/math/doc/html/math_toolkit/gauss.html)

3. **书籍**：
   - 《有限元方法》第 5 章（数值积分）
   - 《Numerical Recipes》第 4 章

4. **视频**：
   - [Gaussian Quadrature Explained](https://www.youtube.com/watch?v=IAE1s6GpH1A)

---

## 练习题

1. 用 2 点高斯积分计算 $\int_{-1}^{1} x^3 dx$，验证结果精确
2. 解释为什么 2 点高斯积分不能精确计算 $\int_{-1}^{1} x^4 dx$
3. 推导任意区间 $[a,b]$ 的高斯积分变换公式
4. 编写代码验证三角形单元的 1 点积分对常数梯度的精确性
