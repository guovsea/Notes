---
tags:
  - cpp/basics
  - type/concept
---

## 简介  

RVO，return value optimization. NRVO, named return value optimization.

返回值优化，是编译器的一种优化技术。

**RVO**：在函数调用点**直接构造**返回值对象（然后进行函数中的某些操作），避免临时变量（包括左值和右值）的创建，拷贝，销毁开销。

> RVO 在 C++11 之前就有了， NRVO 是 C++11 标准引入的（只是说 C++11 写入了标准，不代表之前的编译器不支持）。 从 c++17 以及之后，RVO 和 NRVO 不再是编译器的一种优化技术，而是一项设计准则，意思就是你的编译器必须要支持的，如果不支持就是不符合标准。

## 优化机制

### 匿名 RVO

1. 优化场景

它作用于右值对象，即临时对象。比如：

```cpp
vector<int> GetArray() {
    return vector<int>{1, 2, 3, 4};    // 这里就定义了一个临时对象
}
int main() {
    vector<int> arr = GetArry(); // 返回值要勇于初始化
    return 0;
```

它对左值对象不起作用，NRVO 作用于左值对象。```

2. 优化原理

定义临时对象时编译器不会立刻在内存中创建它，当**把临时对象赋值给一个左值对象时才会创建这个对象**。

### 有名 RVO

1. 优化场景

以下场景：
```cpp
vector<int> GetArray() {
    vector<int> array;   // 这里定义了一个有名字的对象，左值对象
    array.push_back(1);
    array.push_back(2);
    return array;   // 返回的对象是有名字的
int main() {
    vector<int> arr = GetArry(); // 返回值要用于初始化操作
    return 0;
}
```

*容器作为返回值一般都会进行 RVO，因为容器拷贝的开销很大*

### 2. 优化原理

在函数的调用点处先创建这个对象，然后通过引用传递到函数内，对该对象进行相关操作。 上面例子中的 NRVO 原理类似于：
```cpp
void GetArray(vector<int>& ans) {
    ans.push_back(1);
    ans.push_back(2);
    ans.push_back(3);
}

int main() {
    vector<int> result;
    GetArray(result);
    return 0;
}
```

## RVO 失效的场景

- **函数返回结果用于赋值** *而不是初始化*
  因为赋值是在操作已经存在的对象，不能的返回点直接构造（必须在初始化表达式中才能构造对象）。
- 被初始化的对象类型与返回值类型不一致 *不能直接构造其他类型的对象，要进行类型转换*
- 函数中不同的分支会返回不同的**左值对象** 
  编译器可能无法确定哪个对象会被返回，因此无法进行 RVO。例如：

  ```cpp
  Data func2(void) { // RVO失效
    Data d1;
    Data d2;
    if (true) {
        return d1;
    } else {
        return d2;
    }
}
```
```cpp
Data func3(void) { //因为返回的是右值对象，所以RVO不会失效
    if (true) {
        return Data(1);
    } else {
        return Data(2);
    }
}
```
- **使用 `std::move`** *因此不要始终返回 `return std:: move(value);`*
  移动语义通常表示对象的资源被移动，而不是复制，因此编译器不会尝试进行 RVO

```cpp
  Data func4(void) {
    return std::move(Data());
}

```

- 返回值是成员变量 *并不能直接构造成员变量，只能构造整个对象*
- 返回值是已经存在的对象 *因为不需要创建，拷贝，销毁，也就不需要 RVO*

## 测试 RVO

```cpp
#include <iostream>
struct A {
	A() : val_(0){
		std::cout << "A();" << std::endl;
	}
	A(const int i) : val_(0) {
		std::cout << "A(const int i);" << std::endl;
	}
	A(const A & a) : val_(a.val_) {
		std::cout << "A(const A & a);" << std::endl;
	}
	A(A && a) : val_(a.val_) {
		std::cout << "A(A && a);" << std::endl;
	}
	~A() {
		std::cout << "~A();" << std::endl;
	}
	int val_;
};

A f1() {
	std::cout << "---- f1 ----" << std::endl;
	// 局部对象 RVO
	A result;
	return result;
}
A f2(const A & a) {
	std::cout << "---- f2 ----" << std::endl;
	return a;
}
A f3() {
	std::cout << "---- f3 ----" << std::endl;
	// 匿名对象 RVO
	return {};
}
A fu4() {
	std::cout << "---- f4 ----" << std::endl;
	return {1};
}
A f5() {
	std::cout << "---- f5 ----" << std::endl;
	int i = 3;
	return i;
}
A f6() {
	std::cout << "---- f6 ----" << std::endl;
	A result;
	return std::move(result);
}
A f7() {
	std::cout << "---- f7 ----" << std::endl;
	return A();
}
A& f8(A & a) {
	std::cout << "---- f8 ----" << std::endl;
	return a;
}

int main(void) {
	A a;
	std::cout << "--------" << std::endl;
	A a1 = f1();  // RVO 
	A a2 = f2(a); // 拷贝构造
	A a3 = f3();   // 默认构造
	A a4 = f4();  // 有参构造
	A a5 = f5();  // 有参构造
	A a6 = f6();   // 移动构造
	A a7 = f7();   // RVO
	A a8 = f8(a);  // 返回引用类型
}
```