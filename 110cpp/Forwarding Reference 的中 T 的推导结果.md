---
tags:
  - 误区纠正
  - cpp/机制
---
**在 [[Forwarding Reference & Universal Reference]] 中，传入的表达式为左值时，T = &A；表达式为右值时，T = A**


## 实验

```cpp
#include <iostream>
#include <type_traits>

template<typename T>
void f(T&& param) {
    if constexpr (std::is_reference_v<T>)
        std::cout << "T is reference\n";
    else
        std::cout << "T is NOT reference\n";
}

int main() {
    int x = 42;
    f(x);             // 左值 → T = int& → 输出 "T is reference"
    f(42);            // 右值 → T = int  → 输出 "T is NOT reference"
    f(std::move(x));  // 右值 → T = int  → 输出 "T is NOT reference"
}

```