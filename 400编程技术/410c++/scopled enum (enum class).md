---
tags:
  - state/未开始
---
枚举在满足下列条件时都能用列表初始化从一个整数初始化而无需转型：

- 初始化是**直接列表初始化**
- 初始化器列表仅有单个元素
- 转换是非窄化转换

```cpp
#include <iostream>
enum class Color {
    red, blue, green
};

int main() {
    Color x = Color{1}; // ! c++17 起，OK
}
```