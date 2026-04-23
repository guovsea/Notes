---
tags:
  - cpp/concurrency
  - type/concept
---
## 概念

Promise 模型是一种异步编程模型：生产者（Promise）负责设置结果，消费者（Future）负责获取结果。两者通过共享状态关联。

C++11 中对应的标准库组件：
- `std::promise<T>`：存储一个值或异常，供将来检索
- `std::future<T>`：从 `std::promise` 获取值，只能调用一次 `get()`
- `std::shared_future<T>`：可多次 `get()` 的 future

```cpp
#include <future>
#include <iostream>

int main() {
    std::promise<int> p;
    std::future<int> f = p.get_future();

    // 生产者设置值
    p.set_value(42);

    // 消费者获取值（阻塞直到值可用）
    std::cout << f.get() << std::endl; // 42
}
```

## 与 async/await 的关系

Promise 模型是 async/await 模型的底层支撑。[[async、await 模型]]

## 参考

[Promise难懂？一篇文章让你轻松驾驭\_promise很难理解-CSDN博客](https://blog.csdn.net/m0_52040370/article/details/127197204)
