## 向线程中传入参数

### 在当前线程完成线程参数的构造

向线程中传入参数时，会将参数拷贝到线程栈中（初始化函数的形参，形参在函数栈帧中）。

在 std:: thread 的构造函数中传入线程函数的参数时，**要保证在当前线程内完成线程函数参数构造，不要在新线程创建后再进行构造**。

```cpp
#include <iostream>
#kinclude <string.h>
#include <thread>

void f(int i, const std::string &s){

}
void oops(void) {
    char buffer[1024];
    sprintf(buffer, "%s","string");
    // 向新线程拷贝的是 char* 指针，会在新线创建后构造std::string, 但构造途中该函数可能会结束，buffer 变量可能会被释放，对 buffer 的引用也会失效，无法使用 buffer 构造 std::string
    // std::thread t(f, 3, buffer);  // Segmentation fault
    std::thread t(f, 3, std::string(buffer));
    t.detach();
}

int main(int argc, const char *argv[]) {

    std::thread thd(oops);
    thd.detach();
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << "main()" << std::endl;
    return 0;
}

```
### 传入引用参数

要想向函数参数传入引用时，必须**使用 std:: ref 将普通类型参数转成引用类型。

`std::thread` 构造函数会将传给线程参数的实参都以值传递的方式保存到一个`std::tuple`中。

`std::ref`：包装引用，使其能够**像普通对象以值传递的形式传递给值接受值的模板函数**

[[function#std ref|std::ref原理]]

```cpp
void f(int &i){
    i = 4;
}
void oops(void) {
    int num = 3;
    // std::thread t(f,num);      // 编译错误 
    std::thread t(f, std::ref(num));  // 要想传入引用，必须得使用std::ref包装
    t.join();
    std::cout << num << std::endl; 
}
int main(int argc, char const *argv[]) {
    oops();
    return 0;
}

```
### 成员函数作为线程函数时要传入对象的`this`指针

```cpp
class X {
  public:
    void f(int a ) { std::cout << a << std::endl; }
};

int main(int argc, char const *argv[]) {

    X my_x;
    int num(3);
    // std::thread t(&X::f, num);     // 编译错误
    std::thread t(&X::f, &my_x, num);
    t.join();
    return 0;
}
```
**`std::thread t(X::f, &my_x, num);` 会报错**。原理不太理解，可能是因为`X::f`会被解释为 X 类中的静态成员。

*如果传递给 thread 构造函数的实参和线程函数的形参不匹配时（数量不相等或类型不匹配），会出现错误：* ` ~__nat() = delete;`

> `~__nat() = delete;` is only a random difference between the old and new standard, one of the uninteresting things that fail because of the type error.
> `~__nat() = delete;` 只是新旧标准的随机差异，是因类型错误而失败的无趣事物之一。

### 以移动的方式传入参数

std::thread 的构造函数中指定 std::move(arg)
```cpp
void process_big_object(std::unique_ptr<big_object>);

std::unique_ptr<big_object> p(new big_object); 
// 操作 big_object  
std::thread t(process_big_object,std::move(p));
```
## `std::thread` 对象只能移动，不能拷贝

```cpp
class X {
  public:
    void f(int a ) { std::cout << a << std::endl; }
};

int main(int argc, char const *argv[]) {

    X my_x;
    int num(3);
    std::thread t1(&X::f, &my_x, num);
    // std::thread t2(t1);           // 没有拷贝构造函数
    std::thread t2(std::move(t1));   // 只能移动，不能拷贝
    std::thread t3;
    t3 = std::thread(std::move(t2)); // 创建了一个匿名对象，是右值。会调用移动赋值运算符
    t3.join();
    return 0;
}

```

**一个`std::thread`对象被移动赋值或拷贝时，它会先调用`std::terminate()`终止当前线程，再去管理另一个线程**

可以在函数中返回一个 thread 对象，因为可以对 thread 对象进行 RVO 或者移动赋值。

```cpp
void run(void) {
    while (true) {
        std::cout << "running" << std::endl;
    }
}
std::thread f() { 
    return std::thread(run); // 匿名对象，移动（但实际上可能会是RVO）
}
std::thread g() {
    std::thread t(run);
    return t;                 // RVO
}

int main(int argc, char const *argv[]) {
    std::thread t1 = f();
    t1.join();
    std::thread t2 = f();
    t2.join();
```

## thread_guard

**thread_guard**：thread_guard 对象析 构时会自动调用 join，自动地让主线程阻塞等待线程执行完成后再销毁 thread 对象。防止在线程执行的过程中 thread 对象析构。



```cpp
void run(void) {
    while (true) {
        std::cout << "running" << std::endl;
    }
}

class ThreadGuard {
  public:
    explicit ThreadGuard(std::thread &t) : t_(t) {}
    ~ThreadGuard() {
        if (t_.joinable()) {
            t_.join();
        }
    }
    ThreadGuard(const ThreadGuard &) = delete;
    ThreadGuard &operator=(const ThreadGuard &) = delete;

  private:
    std::thread &t_;
};

int main(int argc, char const *argv[]) {
    std::thread t(run);
    ThreadGuard thread_guard(t);
}
```

使用移动语义后，可以让类接管 thread 对象的所有权，原来的 thread 对象被移动后，就不能再通过原来的 thread 对象对实际的线程进行管理了，比如不能 join 或者 detach。

```cpp
class ScopedThread {
  public:
    // explicit ScopedThread(std::thread&& t) : t_(std::move(t)) {}
    explicit ScopedThread(std::thread&& t) : t_(std::move(t)) {
        if (!t_.joinable()) 
            throw std::logic_error("No thread");
    }
    // ... 其他的和 ThreadGuard 一致
int main(int argc, char const *argv[]) {
    std::thread t(run);
    ScopedThread scoped(std::move(t));
    std::cout << t.joinable()<< std::endl;  // 输出 0
    // t.detach(); 报错
}
```

*std::jthread：C++20 提供的，在析构函数中会调用 `t_.join() `*


## 线程数量

不要超过硬件支持的线程数量

`std::thread::hardware_concurrency()`获取系统硬件支持的线程数量。当系统信息无法获取时可能返回 0

## 标识线程

**标识符类型**

`std::thraed::id`

**获取`std::thread`对象的线程 id：**

`get_id()`。当`std::thread`对象没有与任何执行的线程相关时（被 move 了），会返回`std::thraed::id`的默认构造对象。--> 值为 0x0 

**获取当前线程的线程 id：**

`std::this_thread::get_id()`

**`std::thraed::id`对象可以自由的拷贝和对比**，标准库也提 std::hash<std::thread::id> 容器，所以 **`std::thread::id` 也可以作为无序容器的键值**


## 使用 `std::mutex` 保护共享数据

没那么简单

使用类封装`std::mutex `和共享数据时：

- 切勿将受保护数据的指针或引用传递到互斥锁作用域之外，无论是函数返回值，还是存储在外部可见内存
- 亦或是以参数的形式传递到用户提供的函数中去。

```cpp
class data_wrapper {
private:
  some_data data;
  std::mutex m;
public:
template<typename Function>
void process_data(Function func) {
    std::lock_guard<std::mutex> l(m);
    func(data);    // 以参数的形式传递到用户提供的函数中去
}
};
```

