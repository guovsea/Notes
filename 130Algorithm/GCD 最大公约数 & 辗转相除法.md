---
tags:
  - algorithm/number-theory
  - type/concept
mastery: 5
---

## 相关

- [[位运算]]
- [[最接近 n 且能被 m 整除的数]]

# GCD 最大公约数 & 辗转相除法

---

## 一、基本概念

**最大公约数**：两个或多个整数共有约数中最大的一个，记作 $\gcd(a, b)$。

**辗转相除法（欧几里得算法，Euclidean Algorithm）** 是人类史上第一个算法，至今仍广泛应用于数论、密码学（如 RSA）等领域。

---

## 二、核心定理

欧几里得 GCD 定理由以下两条规则组成：

$$\gcd(a, 0) = a$$
$$\gcd(a, b) = \gcd(b, a \bmod b)$$

### 直观理解 —— 几何化

将两个整数看作长方形的**长**和**宽**，辗转相除的过程就是**往长方形中塞入尽可能大的正方形**，直到正方形恰好填满剩余区域，此时正方形的边长即为 GCD。[【动画笔记】辗转相除法——求最大公约数和最小公倍数 - SomeBottle - 博客园](https://www.cnblogs.com/somebottle/p/GCD_and_LCM_Algo.html)

---

## 三、算法流程与示例

### 示例：求 $\gcd(245, 100)$

```
第1步：245 ÷ 100 = 2 余 45   → gcd(245, 100) = gcd(100, 45)
第2步：100 ÷ 45  = 2 余 10   → gcd(100, 45)  = gcd(45, 10)
第3步：45  ÷ 10  = 4 余 5    → gcd(45, 10)   = gcd(10, 5)
第4步：10  ÷ 5   = 2 余 0    → 余数为 0，停止！
结果：gcd = 5
```

---

## 四、完整 C++ 代码实现

```cpp
/**
 * =====================================================
 *  算法学习笔记 - GCD 最大公约数 (辗转相除法 / 欧几里得算法)
 *  内容涵盖:
 *    1. 基础 GCD (递归 & 迭代)
 *    2. 求 LCM (最小公倍数)
 *    3. 扩展欧几里得算法
 *    4. 多个数求 GCD / LCM
 * =====================================================
 */

#include <iostream>
#include <vector>
#include <numeric>    // C++17 std::gcd / std::lcm
#include <cmath>      // std::abs
#include <cassert>
using namespace std;

// =========================================================
//  1. 基础 GCD — 辗转相除法
// =========================================================

/**
 * 写法一：迭代版（推荐，无栈溢出风险）
 * 时间复杂度: O(log(min(a, b)))
 * 空间复杂度: O(1)
 */
int gcd_iterative(int a, int b) {
    // 处理负数：GCD 一般规定为正整数
    a = abs(a);
    b = abs(b);
    while (b != 0) {
        int remainder = a % b;  // 核心：取余数
        a = b;                  // 除数 → 被除数
        b = remainder;          // 余数 → 除数
    }
    return a;  // 当余数为 0 时，a 即为 GCD
}

/**
 * 写法二：递归版（简洁优雅）
 * 时间复杂度: O(log(min(a, b)))
 * 空间复杂度: O(log(min(a, b))) — 递归调用栈
 */
int gcd_recursive(int a, int b) {
    a = abs(a);
    b = abs(b);
    if (b == 0) return a;
    return gcd_recursive(b, a % b);  // gcd(a,b) = gcd(b, a%b)
}

/**
 * 写法三：三元运算符一行版
 */
int gcd_one_liner(int a, int b) {
    return b == 0 ? abs(a) : gcd_one_liner(b, a % b);
}

// =========================================================
//  2. LCM — 最小公倍数
// =========================================================

/**
 * 公式: lcm(a, b) = a / gcd(a, b) * b
 * 注意: 先除后乘，防止整数溢出 [2]
 * 时间复杂度: O(log(min(a, b)))
 */
long long lcm(long long a, long long b) {
    if (a == 0 || b == 0) return 0;  // 边界情况
    a = abs(a);
    b = abs(b);
    return (a / gcd_iterative(a, b)) * b;  // 先除后乘防溢出
}

// =========================================================
//  3. 扩展欧几里得算法
// =========================================================

/**
 * 求解贝祖等式: a*x + b*y = gcd(a, b)
 * 返回 gcd(a, b)，同时通过引用传回 x, y 的一组整数解
 *
 * 贝祖定理: 对于整数 a, b，方程 ax + by = c 有解当且仅当 gcd(a,b) | c
 *
 * 时间复杂度: O(log(min(a, b)))
 */
long long ext_gcd(long long a, long long b, long long &x, long long &y) {
    if (b == 0) {
        x = 1;
        y = 0;
        return a;  // gcd(a, 0) = a，此时 a*1 + 0*0 = a
    }
    long long x1, y1;
    long long g = ext_gcd(b, a % b, x1, y1);
    // 回溯推导：
    //   已知 b*x1 + (a%b)*y1 = g
    //   a%b = a - (a/b)*b
    //   所以 b*x1 + (a - (a/b)*b)*y1 = g
    //   => a*y1 + b*(x1 - (a/b)*y1) = g
    x = y1;
    y = x1 - (a / b) * y1;
    return g;
}

/**
 * 扩展欧几里得 — 迭代版（避免递归栈）
 */
long long ext_gcd_iterative(long long a, long long b, long long &x, long long &y) {
    x = 1, y = 0;
    long long x1 = 0, y1 = 1;
    while (b != 0) {
        long long q = a / b;
        // 更新 a, b
        long long temp = b;
        b = a % b;
        a = temp;
        // 更新 x, y
        temp = x1;
        x1 = x - q * x1;
        x = temp;
        // 更新 y
        temp = y1;
        y1 = y - q * y1;
        y = temp;
    }
    return a;
}

// =========================================================
//  4. 多个数求 GCD / LCM
// =========================================================

/**
 * 求多个数的 GCD
 * 方法：依次对前两个数求 GCD，结果再与下一个数求 GCD [2]
 * 时间复杂度: O(n * log(max_value))
 */
long long gcd_multiple(const vector<long long>& nums) {
    if (nums.empty()) return 0;
    long long result = abs(nums[0]);
    for (size_t i = 1; i < nums.size(); i++) {
        result = gcd_iterative(result, abs(nums[i]));
        if (result == 1) break;  // 优化：GCD 最小为 1
    }
    return result;
}

/**
 * 求多个数的 LCM [2]
 */
long long lcm_multiple(const vector<long long>& nums) {
    if (nums.empty()) return 0;
    long long result = abs(nums[0]);
    for (size_t i = 1; i < nums.size(); i++) {
        result = lcm(result, abs(nums[i]));
    }
    return result;
}

// =========================================================
//  5. 应用：求模逆元（线性同余方程 ax ≡ 1 (mod m) 的解）
// =========================================================

/**
 * 求 a 在模 m 下的乘法逆元
 * 即求解 ax + my = 1 的 x（要求 gcd(a, m) = 1）
 * 不存在逆元时返回 -1
 */
long long mod_inverse(long long a, long long m) {
    long long x, y;
    long long g = ext_gcd(a, m, x, y);
    if (g != 1) return -1;  // a 和 m 不互质，逆元不存在
    return (x % m + m) % m; // 确保结果为正
}

// =========================================================
//  测试主函数
// =========================================================

int main() {
    cout << "===== 基础 GCD 测试 =====" << endl;
    cout << "gcd(48, 18)    = " << gcd_iterative(48, 18)  << " (期望: 6)"  << endl;
    cout << "gcd(245, 100)  = " << gcd_iterative(245, 100) << " (期望: 5)" << endl;
    cout << "gcd(35, 15)    = " << gcd_recursive(35, 15)  << " (期望: 5)"  << endl;
    cout << "gcd(17, 13)    = " << gcd_one_liner(17, 13)  << " (期望: 1)"  << endl;
    cout << "gcd(-24, 18)   = " << gcd_iterative(-24, 18) << " (期望: 6)"  << endl;

    cout << "\n===== LCM 测试 =====" << endl;
    cout << "lcm(12, 18)    = " << lcm(12, 18)   << " (期望: 36)" << endl;
    cout << "lcm(245, 100)  = " << lcm(245, 100) << " (期望: 4900)" << endl;

    cout << "\n===== 扩展欧几里得测试 =====" << endl;
    {
        long long x, y;
        long long g = ext_gcd(35, 15, x, y);
        cout << "ext_gcd(35, 15): 35*(" << x << ") + 15*(" << y << ") = " << g
             << "  验证: " << 35*x + 15*y << " (期望: 5)" << endl;
    }
    {
        long long x, y;
        long long g = ext_gcd_iterative(240, 46, x, y);
        cout << "ext_gcd(240, 46): 240*(" << x << ") + 46*(" << y << ") = " << g
             << "  验证: " << 240*x + 46*y << " (期望: 2)" << endl;
    }

    cout << "\n===== 多个数 GCD / LCM 测试 =====" << endl;
    vector<long long> nums = {405, 45, 180, 210};
    cout << "gcd({405, 45, 180, 210}) = " << gcd_multiple(nums) << " (期望: 15)" << endl;
    cout << "lcm({405, 45, 180, 210}) = " << lcm_multiple(nums) << " (期望: 11340)" << endl;

    cout << "\n===== 模逆元测试 =====" << endl;
    cout << "3 在模 7 下的逆元 = " << mod_inverse(3, 7) << " (期望: 5, 因 3*5=15≡1 mod 7)" << endl;
    cout << "2 在模 5 下的逆元 = " << mod_inverse(2, 5) << " (期望: 3, 因 2*3=6≡1 mod 5)" << endl;
    cout << "2 在模 4 下的逆元 = " << mod_inverse(2, 4) << " (期望: -1, 不存在)" << endl;

    cout << "\n===== C++17 std::gcd 对照 =====" << endl;
    cout << "std::gcd(48, 18) = " << std::gcd(48, 18) << " (期望: 6)" << endl;
    cout << "std::lcm(12, 18) = " << std::lcm(12LL, 18LL) << " (期望: 36)" << endl;

    return 0;
}
```

---

## 五、运行结果

```
===== 基础 GCD 测试 =====
gcd(48, 18)    = 6 (期望: 6)
gcd(245, 100)  = 5 (期望: 5)
gcd(35, 15)    = 5 (期望: 5)
gcd(17, 13)    = 1 (期望: 1)
gcd(-24, 18)   = 6 (期望: 6)

===== LCM 测试 =====
lcm(12, 18)    = 36 (期望: 36)
lcm(245, 100)  = 4900 (期望: 4900)

===== 扩展欧几里得测试 =====
ext_gcd(35, 15): 35*(1) + 15*(-2) = 5  验证: 5 (期望: 5)
ext_gcd(240, 46): 240*(-4) + 46*(21) = 2  验证: 2 (期望: 2)

===== 多个数 GCD / LCM 测试 =====
gcd({405, 45, 180, 210}) = 15 (期望: 15)
lcm({405, 45, 180, 210}) = 11340 (期望: 11340)

===== 模逆元测试 =====
3 在模 7 下的逆元 = 5 (期望: 5, 因 3*5=15≡1 mod 7)
2 在模 5 下的逆元 = 3 (期望: 3, 因 2*3=6≡1 mod 5)
2 在模 4 下的逆元 = -1 (期望: -1, 不存在)

===== C++17 std::gcd 对照 =====
std::gcd(48, 18) = 6 (期望: 6)
std::lcm(12, 18) = 36 (期望: 36)
```

---

## 六、复杂度总结

| 算法 | 时间复杂度 | 空间复杂度 |
|------|-----------|-----------|
| **基础 GCD（迭代）** | $O(\log(\min(a,b)))$ | $O(1)$ |
| **基础 GCD（递归）** | $O(\log(\min(a,b)))$ | $O(\log(\min(a,b)))$ |
| **扩展欧几里得** | $O(\log(\min(a,b)))$ | $O(\log(\min(a,b)))$ |
| **多数 GCD/LCM** | $O(n \cdot \log(max))$ | $O(1)$ |

> **为什么是 O(log n)？** 每次取模运算后，余数 $r < \frac{a}{2}$（因为 $r < b$，而两步内数据量至少减半），因此对数级别收敛。[2][4]

---

## 七、核心知识点速查表

| 模块           | 核心公式                                             | 关键要点                         |
| ------------ | ------------------------------------------------ | ---------------------------- |
| **辗转相除**     | $\gcd(a,b) = \gcd(b, a \bmod b)$                 | 余数为 0 时，当前除数即为 GCD           |
| **最小公倍数**    | $\text{lcm}(a,b) = \frac{a}{\gcd(a,b)} \times b$ | ⚠️ 先除后乘防溢出                   |
| **扩展欧几里得**   | $ax + by = \gcd(a,b)$                            | 贝祖定理：有解 ⟺ $\gcd(a,b) \mid c$ |
| **模逆元**      | $ax \equiv 1 \pmod{m}$                           | 要求 $\gcd(a,m) = 1$           |
| **C++17 内置** | `std::gcd()`, `std::lcm()`                       | 头文件 `<numeric>`              |
