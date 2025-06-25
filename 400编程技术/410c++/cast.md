 ## const_cast

**仅用于底层 const 转换为底层非 const** 

- `T* p = const_cast<T*>pConst`
- `T& ref = const_cast<T&>rConst`

```cpp
int main(int argc, const char** argv) {
    int i = 1;
    const int *pConst = &i;
    // int* pi = pConstInt; // error
    int* p = const_cast<int*>(pConst);
    *p = 2;
    std::cout << *p << std::endl;
    const int &rConst = i;
    // int &r = rConst; // error
    int &r = const_cast<int&>(rConst);
    r = 4;
    std::cout << r << std::endl;

    return 0;
}
```

## static_cast

使用场景：

- 可用于替换任何隐式转换
- 继承链中两个类之间相互转换（向下：不安全；向上：安全）
- 可以显式转换为 void 类型，将空指针值转换为目标类型的空指针值

特点：

- 无法强制转换掉 `const`、`volatile`
- 不进行运行时类型检查，向下转换仍有返回值
```cpp
class B {
public:
   virtual void Test(){}
};
class D : public B {};
void f(B* pb) {
   D* pd1 = dynamic_cast<D*>(pb);
   if (pd1 == nullptr) {
       std::cout << "pd1 == nullptr" << std::endl;
   }
   D* pd2 = static_cast<D*>(pb);
   if (pd2 == nullptr) {
       std::cout << "pd2 == nullptr" << std::endl;
   }
}

int main(int argc, const char** argv) {
    B* pB= new B();
    f(pB);  // pd1 == nullptr
}
```

## dynamic_cast

使用场景：
- 必须有多态性（基类有 virtual 函数）

特点：

- 会进行动态类型检查，如果 expection 的**动态类型(实际指向对象的类型)** 不是目标类型的基类时（向下转换，不安全），则 nullptr
- 无法强制转换掉 `const`、`volatile`

> dynamic_cast 进行转换，如果转换不安全的话会返回 nullptr；static_cast 不会

```cpp
#include <iostream>

class B {
   virtual void Test(){}
};
class D : public B {};

class F : public D {};

void f(B* pb) {
   D* pd1 = dynamic_cast<D*>(pb);
   if (pd1 == nullptr) {
       std::cout << "pd1 == nullptr" << std::endl;
   }
}

int main(int argc, const char** argv) {
    B* pB = new B();
    B* pF = new F();
    f(pB);
    f(pF);
}
```

dynamic_cast 用于引用时，并且转换不安全时，会 `raise std::bad_cast`
```cpp
#include <iostream>

class B {
   virtual void Test(){}
};
class D : public B {};

class F : public D {};

void g(B& rb) {
   D& rd = dynamic_cast<D&>(rb);
   // if (rd) {
   //     std::cout << "pd1 == nullptr" << std::endl;
   // }
}

int main(int argc, const char** argv) {
    B b = B();
    F f = F();
    B& rB = b;
    B& rF = f;
    g(rF);
    std::cout << "g(rF)" << std::endl;
    g(rB);
}
```
```text
g(rF)
terminate called after throwing an instance of 'std::bad_cast'
  what():  std::bad_cast
```

[这下可以安心使用 dynamic\_cast 了：dynamic\_cast 的实现方法分析以及性能优化 - 知乎](https://zhuanlan.zhihu.com/p/580330672)


## reinterpret_cast

仅仅只是对内存单元进行重新解释，不会编译成任何 CPU 指令