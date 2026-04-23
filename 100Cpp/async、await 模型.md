---
tags:
  - cpp/concurrency
  - type/concept
---
## 概念

async/await 模型是基于 [[Promise 模型]] 的上层异步编程模式：
- `async`：启动一个异步任务，立即返回一个 `std::future`
- `await`（C++ 中为 `future.get()`）：阻塞等待异步结果

```cpp
#include <future>
#include <iostream>

int heavy_computation() { return 42; }

int main() {
    // async 启动异步任务
    std::future<int> result = std::async(std::launch::async, heavy_computation);
    // await（get）等待结果
    std::cout << result.get() << std::endl;
}
```

## 参考

[Python Async/Await入门指南 - 知乎](https://zhuanlan.zhihu.com/p/27258289)
