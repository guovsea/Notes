---
tags:
  - cpp/stl
  - type/howto
---
Great question! `std::reference_wrapper` provides some unique benefits that normal references (`T&`) do not. Although normal references are often sufficient, `std::reference_wrapper` is particularly useful in certain situations. Here's why you might use `std::reference_wrapper` over a regular reference:

### 1. **Storing References in Standard Containers**

- **Problem with Regular References:** Standard containers (like `std::vector`, `std::list`, etc.) require the elements they store to be copyable and assignable. Since references (`T&`) are neither copyable nor assignable (you cannot store a `T&` directly in a container), you can't directly store references in containers.
    
- **Solution with `std::reference_wrapper`:** `std::reference_wrapper<T>` is a copyable and assignable object that internally holds a reference to a `T` object. You can store `std::reference_wrapper` objects in containers, allowing you to effectively store references to objects.

```cpp
int a = 1, b = 2, c = 3;
std::vector<std::reference_wrapper<int>> refVec = {std::ref(a), std::ref(b), std::ref(c)};
refVec[0].get() = 10;  // Modifies 'a'

```

    Here, `refVec` contains references to `a`, `b`, and `c`, and modifying the elements of the vector modifies the original variables.
    

### 2. **Making References Copyable**

- **Problem with Regular References:** Normal references cannot be copied or reassigned after initialization. Once a reference is bound to an object, it cannot be changed to refer to a different object.
    
- **Solution with `std::reference_wrapper`:** A `std::reference_wrapper` can be copied and assigned. This allows more flexibility in situations where you need to copy an object that behaves like a reference (e.g., passing it around or storing it), but still refer to the original object.
    
    **Example:**

    
```cpp
int a = 1;
std::reference_wrapper<int> refA = std::ref(a);
std::reference_wrapper<int> refB = refA;  // This works, refB now also refers to 'a'

```
    
    You can copy `refA` to `refB`, and both will refer to the same object.
    

### 3. **Avoiding Object Slicing**

- **Problem with Regular References:** When dealing with polymorphism (e.g., pointers or references to base classes pointing to derived class objects), regular references are limited because you cannot store them in a container without losing polymorphic behavior (object slicing).
    
- **Solution with `std::reference_wrapper`:** You can store `std::reference_wrapper<Base>` in containers and maintain polymorphic behavior.
    
    **Example:**

    复制代码
    
```cpp
class Base { /*...*/ };
class Derived : public Base { /*...*/ };
Derived d;
std::vector<std::reference_wrapper<Base>> refs = {std::ref(d)};
// No object slicing occurs; refs stores references to the original derived object.

```

### Summary: When to Use `std::reference_wrapper`

- **Use normal references (`T&`)** when you only need a simple reference to an object, and you don't need to store or copy the reference.
- **Use `std::reference_wrapper<T>`** when:
    - You need to store references in containers.
    - You need a reference-like object that can be copied and assigned.
    - You want to avoid object slicing in polymorphic containers.
    - You need to pass references through `std::bind` or other callables in a context that might not handle references correctly.

`std::reference_wrapper` is a useful tool for advanced reference management, making it possible to store and pass references in situations where normal references can't be used directly.