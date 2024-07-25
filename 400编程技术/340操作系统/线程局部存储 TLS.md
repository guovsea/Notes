线程局部变量的特性特性：
- 对象的存储是在线程开始时分配，线程结束时回收，每个线程有该对象自己的实例
- **只有静态变量和全局变量可以使用线程局部存储**
  ```cpp
  void run() {
    __thread int a = 1; // ERROR 只有静态变量和全局变量可以使用线程局部存储
    ++a;
    print_a();
}
  ```

c/c++中使用方法：`__thread/thread_local`

1. **修饰全局变量**

```cpp
#include <iostream>
#include <thread>

// int a = 1; // 全局变量，线程之间共享
__thread int a = 1; // 线程局部变量

void print_a() { std::cout << a << std::endl; }

void run() {
    ++a;
    print_a();
}

int main(void) {
    thread t1(run); // a = 2
    t1.join();

    thread t2(run); // a = 2
    t2.join();
    return 0;
}

```

2. **修饰静态变量** （[[存储期 Storage Duration]]）

```cpp
#include <iostream>
#include <thread>

class TLS {
private:
    static thread_local int a;

    void run() {
        std::cout << ++a << std::endl;
    }
public:
    void test() {
        std::thread t1(&TLS::run, this);
        std::thread t2(&TLS::run, this);

        t1.join();
        t2.join();
    }
};

// 初始化线程局部存储变量a
thread_local int TLS::a = 1;

int main() {
    TLS tls;
    tls.test();
    return 0;
}

    ```


不同语言的使用 TLS 的方法

[线程局部存储 - 维基百科，自由的百科全书](https://zh.wikipedia.org/wiki/%E7%BA%BF%E7%A8%8B%E5%B1%80%E9%83%A8%E5%AD%98%E5%82%A8)
