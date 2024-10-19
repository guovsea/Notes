```cpp
#define GETSET(type, name) \  
public: \  
    type Get##name() const { return m_##name; } \  
    void Set##name(const type &value) { m_##name = value; }
```