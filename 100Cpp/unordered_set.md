---
tags:
  - cpp/stl
  - type/reference
---

## C++11

### 构造函数

主要的构造函数，只有`bucket_count`是必须的，其他的可以缺省

```cpp
unordered_set( std::initializer_list
               size_type bucket_count = /*implementation-defined*/,  
               const Hash& hash = Hash(),  
               const key_equal& equal = key_equal(),  
               const Allocator& alloc = Allocator() );
```


### 用于自定义类型

#### 1. 特例化 `hash<T>`

[[模板#类模板特例化]]

```cpp
class Person {
    public:
    Person(const int & ID, const std::string name) 
        : ID(ID),name(name) {}
    friend std::hash<Person>;
    friend bool operator==(const Person &lhs, const Person &rhs);
    int ID;
    std::string name;
};
bool operator==(const Person &lhs, const Person &rhs) {
    return lhs.ID == rhs.ID;
}
// 特例化 hash<T>
// 打开 std 命名空间
namespace std {
template<>
struct hash<Person> {
    typedef Person argument_type;
    typedef size_t result_type;
    size_t operator()(const Person &p) const;

};
// 没有模板形参列表
size_t hash<Person>::operator()(const Person &p) const {
    // hash<int> 是一个类，hash<int>()创建一个对象，再调用它的 operator()
    return hash<int>()(p.ID);
}
} // 关闭 std namespace , 不以分号结尾

int main(void) {
    std::unordered_set<Person> set(32); // bucket_count 
    set.insert({456, "jack"});
    std::cout << set.count({123, "milk"}) << std::endl;
}
```
#### 2. 使用模板参数


```cpp
class Person {
    public:
    Person(const int & ID, const std::string name) 
        : ID(ID),name(name) {}
    friend bool operator==(const Person &lhs, const Person &rhs);
    int ID;
    std::string name;
};
bool operator==(const Person &lhs, const Person &rhs) {
    return lhs.ID == rhs.ID;
}
size_t hasher(Person p) {
    return std::hash<int>()(p.ID);
}
bool eqPerson(const Person & lhs, const Person & rhs) {
    return lhs.ID == rhs.ID;
}

int main(void) {
    std::unordered_set<int> intSet({})
    std::unordered_set<
        Person, decltype(hasher)*, decltype(eqPerson)*> 
            set({},32, hasher, eqPerson);
    set.insert({123, "milk"});
    set.insert({456, "jack"});
    std::cout << set.count({123, "milk"}) << std::endl;
```

`decltype(函数)`推断出来的类型是函数类型，`decltype (函数)*`才是函数指针类型

[[decltype]]