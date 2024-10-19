---
tags:
  - cpp/机制
---
## 原理

- [ ] enable_shared_from_this 原理

## 使用方法

```cpp
#include <iostream>
#include <memory>
struct A  : public std::enable_shared_from_this<A> {
    A() {}
    ~A() { std::cout << "A 被销毁" << std::endl; }
    std::shared_ptr<A> f() { return shared_from_this(); }
};

int main() {
    {
        auto pa = std::make_shared<A>();
        auto p1 = pa->f();
        auto p2 = pa->f();
        std::cout << "p1.use_count = " << p1.use_count() << " "
                  << "p2.use_count = " << p2.use_count() << "\n";
    }
    std::cout << "into external scope\n";
}
```