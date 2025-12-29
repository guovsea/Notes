---
tags:
  - cpp
  - cpp/语法
---
## 概念

constexpr 函数并非必然编译期执行，需要一定条件。

constexpr 函数并非 “必然编译期执行”，需同时满足以下条件才会触发编译期计算：

- 函数调用的**所有参数**必须是「编译期常量」（constexpr 类型或字面量）；

- 函数调用的**结果必须用于编译期上下文**（如赋值给 constexpr 变量、作为模板参数等）。

```cpp
#include <numeric>
#include <vector>
#include <iostream>
#include <concepts>

template <typename T> 
requires std::integral<T> || std::floating_point<T>
constexpr double Average(std::vector<T> const &vec) {
    const double sum = std::accumulate(vec.begin(), vec.end(), 0.0);        
    return sum / vec.size();
}

int main() {
    std::vector ints { 1, 2, 3, 4, 5};
    std::cout << Average(ints) << '\n';                                      
}
```

上述代码中，由于 ints 不是编译期常量，所以并没有在编译期执行，而是变成普通函数，在运行期执行。