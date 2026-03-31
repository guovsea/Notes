# 形函数详解

> 目标：理解形函数的定义、性质和计算方法

---

## 1. 什么是形函数？

### 一句话解释

形函数是**在单元内部进行插值的基函数**，它决定了如何用单元节点的值来表示单元内部任意点的值。

### 类比理解

想象一个简单的线性插值：
- 已知点 A 的值是 3，点 B 的值是 7
- 想知道中点的值？直觉告诉我们是 (3+7)/2 = 5

形函数就是把这个直觉推广到二维，并且形式化。

### 数学表达

对于单元内的任意点 $P$，场值 $u(P)$ 可以表示为：

$$u(P) = \sum_{i=1}^{n} N_i(P) \cdot u_i$$

其中：
- $N_i(P)$ 是第 $i$ 个形函数在点 $P$ 的值
- $u_i$ 是第 $i$ 个节点的场值
- $n$ 是单元的节点数

---

## 2. 形函数的关键性质

### 性质 1：Kronecker Delta 性质

$$N_i(x_j, y_j) = \delta_{ij} = \begin{cases} 1 & \text{if } i = j \\ 0 & \text{if } i \neq j \end{cases}$$

**意义**：在第 $j$ 个节点处，只有第 $j$ 个形函数为 1，其他都为 0。

**直观理解**：这保证了插值恰好通过所有节点。

### 性质 2：单位分解性

$$\sum_{i=1}^{n} N_i(P) = 1, \quad \forall P \in \text{单元}$$

**意义**：形函数的权重和恒为 1。

**直观理解**：如果所有节点值都相同（比如都是 5），那么单元内任意点也应该是 5。

### 性质 3：完备性（一阶单元）

一阶形函数可以精确表示线性场：

$$u(x, y) = a + bx + cy$$

**意义**：如果真实解是线性的，数值解就是精确的。

---

## 3. 一阶三角形单元

### 几何定义

```
      3
     / \
    /   \
   /     \
  /       \
 1---------2
```

三个节点：1, 2, 3（逆时针编号）

### 面积坐标（自然坐标）

三角形单元的形函数可以用**面积坐标**定义：

$$N_i = \frac{A_i}{A}$$

其中：
- $A$ 是三角形总面积
- $A_i$ 是点 $P$ 与节点 $i$ 对边构成的三角形面积

```
      3
     /|\
    / | \
   /  |P \
  /___|___\
 1    A3   2
 
 A1 = 区域 P-2-3 的面积
 A2 = 区域 P-1-3 的面积  
 A3 = 区域 P-1-2 的面积
```

### 直角坐标表达式

设三个节点坐标为 $(x_1, y_1), (x_2, y_2), (x_3, y_3)$，则：

$$N_1(x, y) = \frac{1}{2A}[(x_2y_3 - x_3y_2) + (y_2 - y_3)x + (x_3 - x_2)y]$$

$N_2, N_3$ 类似，下标轮换。

其中三角形面积：

$$A = \frac{1}{2}|(x_2-x_1)(y_3-y_1) - (x_3-x_1)(y_2-y_1)|$$

### 程序员实现

```cpp
struct TriangleElement {
    int nodes[3];  // 三个节点的全局索引
    
    // 计算形函数值
    void shapeFunctions(double x, double y, double N[3]) {
        // 获取节点坐标
        double x1 = nodeCoords[nodes[0]].x;
        double y1 = nodeCoords[nodes[0]].y;
        double x2 = nodeCoords[nodes[1]].x;
        double y2 = nodeCoords[nodes[1]].y;
        double x3 = nodeCoords[nodes[2]].x;
        double y3 = nodeCoords[nodes[2]].y;
        
        // 计算面积
        double A = 0.5 * ((x2-x1)*(y3-y1) - (x3-x1)*(y2-y1));
        
        // 计算形函数
        N[0] = ((x2*y3 - x3*y2) + (y2-y3)*x + (x3-x2)*y) / (2*A);
        N[1] = ((x3*y1 - x1*y3) + (y3-y1)*x + (x1-x3)*y) / (2*A);
        N[2] = ((x1*y2 - x2*y1) + (y1-y2)*x + (x2-x1)*y) / (2*A);
    }
    
    // 计算形函数梯度（直角坐标系）
    void shapeFunctionDerivatives(double dN[3][2]) {
        double x1 = nodeCoords[nodes[0]].x;
        double y1 = nodeCoords[nodes[0]].y;
        double x2 = nodeCoords[nodes[1]].x;
        double y2 = nodeCoords[nodes[1]].y;
        double x3 = nodeCoords[nodes[2]].x;
        double y3 = nodeCoords[nodes[2]].y;
        
        double A = 0.5 * ((x2-x1)*(y3-y1) - (x3-x1)*(y2-y1));
        
        // dN[i][0] = dN_i/dx, dN[i][1] = dN_i/dy
        dN[0][0] = (y2 - y3) / (2*A);
        dN[0][1] = (x3 - x2) / (2*A);
        dN[1][0] = (y3 - y1) / (2*A);
        dN[1][1] = (x1 - x3) / (2*A);
        dN[2][0] = (y1 - y2) / (2*A);
        dN[2][1] = (x2 - x1) / (2*A);
    }
};
```

### 重要性质：梯度是常数

对于一阶三角形单元，形函数的梯度是**常数**（与位置无关）！

这意味着：
- 单元内的 $\nabla u$ 是常数
- 刚度矩阵计算更简单

---

## 4. 一阶四边形单元

### 几何定义

```
4---------3
|    s    |
|    ↑    |
| -1 ←→ 1 |
|    ↓    |
|   -1    |
1---------2
     
标准单元：s ∈ [-1, 1], t ∈ [-1, 1]
```

### 等参空间中的形函数

四边形单元在**等参空间**（标准单元）中定义形函数：

$$
\begin{aligned}
N_1(s, t) &= \frac{1}{4}(1-s)(1-t) \\
N_2(s, t) &= \frac{1}{4}(1+s)(1-t) \\
N_3(s, t) &= \frac{1}{4}(1+s)(1+t) \\
N_4(s, t) &= \frac{1}{4}(1-s)(1+t)
\end{aligned}
$$

### 验证 Kronecker Delta 性质

在节点 1 (s=-1, t=-1)：
- $N_1 = \frac{1}{4}(2)(2) = 1$ ✓
- $N_2 = \frac{1}{4}(0)(2) = 0$ ✓
- $N_3 = \frac{1}{4}(0)(0) = 0$ ✓
- $N_4 = \frac{1}{4}(2)(0) = 0$ ✓

其他节点类似验证。

### 等参变换

实际单元可能是任意四边形，需要映射到标准单元：

$$
x(s, t) = \sum_{i=1}^{4} N_i(s, t) \cdot x_i
$$

$$
y(s, t) = \sum_{i=1}^{4} N_i(s, t) \cdot y_i
$$

这就是**等参变换**：用形函数进行坐标映射。

### 程序员实现

```cpp
struct QuadrilateralElement {
    int nodes[4];  // 四个节点的全局索引
    
    // 在等参空间中计算形函数值
    void shapeFunctions(double s, double t, double N[4]) {
        N[0] = 0.25 * (1 - s) * (1 - t);
        N[1] = 0.25 * (1 + s) * (1 - t);
        N[2] = 0.25 * (1 + s) * (1 + t);
        N[3] = 0.25 * (1 - s) * (1 + t);
    }
    
    // 在等参空间中计算形函数对 s,t 的偏导
    void shapeFunctionDerivatives(double s, double t, double dN[4][2]) {
        // dN[i][0] = dN_i/ds, dN[i][1] = dN_i/dt
        dN[0][0] = -0.25 * (1 - t);
        dN[0][1] = -0.25 * (1 - s);
        dN[1][0] =  0.25 * (1 - t);
        dN[1][1] = -0.25 * (1 + s);
        dN[2][0] =  0.25 * (1 + t);
        dN[2][1] =  0.25 * (1 + s);
        dN[3][0] = -0.25 * (1 + t);
        dN[3][1] =  0.25 * (1 - s);
    }
    
    // 计算雅可比矩阵（等参变换的导数）
    void jacobian(double s, double t, double J[2][2]) {
        double dN[4][2];
        shapeFunctionDerivatives(s, t, dN);
        
        // 获取节点坐标
        double x[4], y[4];
        for (int i = 0; i < 4; i++) {
            x[i] = nodeCoords[nodes[i]].x;
            y[i] = nodeCoords[nodes[i]].y;
        }
        
        // J = [dx/ds, dx/dt; dy/ds, dy/dt]
        J[0][0] = J[0][1] = J[1][0] = J[1][1] = 0;
        for (int i = 0; i < 4; i++) {
            J[0][0] += dN[i][0] * x[i];  // dx/ds
            J[0][1] += dN[i][1] * x[i];  // dx/dt
            J[1][0] += dN[i][0] * y[i];  // dy/ds
            J[1][1] += dN[i][1] * y[i];  // dy/dt
        }
    }
    
    // 计算形函数对 x,y 的偏导（需要逆雅可比）
    void shapeFunctionDerivativesXY(double s, double t, double dN_xy[4][2]) {
        double dN[4][2];
        shapeFunctionDerivatives(s, t, dN);
        
        double J[2][2];
        jacobian(s, t, J);
        
        // 计算 J 的逆
        double detJ = J[0][0]*J[1][1] - J[0][1]*J[1][0];
        double Jinv[2][2] = {
            { J[1][1]/detJ, -J[0][1]/detJ},
            {-J[1][0]/detJ,  J[0][0]/detJ}
        };
        
        // [dN/dx]   [J11 J12]^-1 [dN/ds]
        // [dN/dy] = [J21 J22]    [dN/dt]
        for (int i = 0; i < 4; i++) {
            dN_xy[i][0] = Jinv[0][0] * dN[i][0] + Jinv[0][1] * dN[i][1];
            dN_xy[i][1] = Jinv[1][0] * dN[i][0] + Jinv[1][1] * dN[i][1];
        }
    }
};
```

---

## 5. 三角形 vs 四边形对比

| 特性 | 三角形单元 | 四边形单元 |
|------|-----------|-----------|
| 节点数 | 3 | 4 |
| 形函数阶次 | 线性 | 双线性 |
| 梯度 | 常数（简单） | 随位置变化（复杂） |
| 网格生成 | 更灵活 | 对畸变敏感 |
| 积分计算 | 简单 | 需要等参变换 |
| 适合形状 | 任意三角形 | 接近矩形 |

---

## 6. 形函数的应用

### 在单元刚度矩阵计算中

公式 1.5 和 1.6 涉及：
- $\nabla N_i$：形函数的梯度
- 数值积分时需要计算 $N_i$ 在高斯点的值

### 在结果插值中

求解得到节点值后，用形函数可以计算单元内任意点的场值。

---

## 7. 学习检查清单

- [ ] 理解形函数的插值作用
- [ ] 掌握 Kronecker Delta 性质的验证
- [ ] 能写出三角形单元的形函数表达式
- [ ] 理解面积坐标的概念
- [ ] 能在等参空间中写出四边形单元的形函数
- [ ] 理解等参变换的意义
- [ ] 能计算雅可比矩阵
- [ ] 理解梯度从 (s,t) 到 (x,y) 的转换

---

## 参考资源

1. **视频**：
   - [有限元形函数 - MIT OCW](https://www.youtube.com/watch?v=Nr5QjR6MPMY)
   - [等参单元 - Coursera](https://www.coursera.org/learn/finite-element-method)

2. **书籍**：
   - 《有限元方法基础教程》第 3 章
   - 《The Finite Element Method》Zienkiewicz, Chapter 3

3. **在线交互**：
   - [形函数可视化工具](https://defele.sourceforge.net/)（有限元教育工具）

---

## 练习题

1. 验证三角形单元 $N_1 + N_2 + N_3 = 1$
2. 推导四边形单元在 (0,0) 点的形函数值
3. 证明：当四边形是矩形时，雅可比矩阵是常数矩阵
4. 计算一个具体三角形单元的形函数梯度
