
[[CRTP 奇异递归模板模式]]

[[单例模式 Singleton]]


```cpp
#include <iostream>

template <typename T>
class Singleton
{
public:
    static T* GetIns()
    {
        static T instance;
        return &instance;
    }
protected:
    Singleton() = default;
    Singleton(const Singleton&) = delete;
    Singleton& operator= (const Singleton&) = delete;

};

class A : public Singleton<A>
{
    friend Singleton<A>;
public:
    void addr() {std::cout << this  << "\n";}
private:
    A() {}
};


int main(int argc, char const *argv[])
{
    {
        A::GetIns()->addr();
    }
    A::GetIns()->addr();

}

```
