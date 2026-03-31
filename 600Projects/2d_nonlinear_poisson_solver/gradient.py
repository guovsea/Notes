import numpy as np 
import matplotlib.pyplot as plt

# 定义二元函数 f(x, y) = x² - y²
def f(x, y):
    return x**2 - y**2

# 对 x 求数值偏导数（中心差分法）
def grad_x(f, x, y):
    h = 1e-4
    return (f(x + h/2, y) - f(x - h/2, y)) / h

# 对 y 求数值偏导数（中心差分法）
def grad_y(f, x, y):
    h = 1e-4
    return (f(x, y + h/2) - f(x, y - h/2)) / h

# 计算多点的数值梯度
def numerical_gradient(f, P):
    grad = np.zeros_like(P)
    for i in range(P[0].size):
        grad[0][i] = grad_x(f, P[0][i], P[1][i])
        grad[1][i] = grad_y(f, P[0][i], P[1][i])
    return grad

# 生成网格坐标
x = np.arange(-2, 2, 0.5)
y = np.arange(-2, 2, 0.5)
X, Y = np.meshgrid(x, y)

# 展平为一维数组，方便批量计算梯度
X = X.flatten()
Y = Y.flatten()

# 计算所有点的梯度
grad = numerical_gradient(f, np.array([X, Y]))

# 绘制梯度向量场
plt.quiver(X, Y, grad[0], grad[1])  # grad[0]：x方向梯度，grad[1]：y方向梯度
plt.xlim([-2, 2])
plt.ylim([-2, 2])
plt.xlabel('x')
plt.ylabel('y')
plt.grid()
plt.show()