A constructor performs its work in this order:

1. It calls base class and member constructors in the order of declaration.
2. If the class is derived from virtual base classes, it initializes the object's virtual base pointers.
3. If the class has or inherits virtual functions, it initializes the object's virtual function pointers. Virtual function pointers point to the class's virtual function table to enable correct binding of virtual function calls to code.
4. It executes any code in its function body.
    

The following example shows the order in which base class and member constructors are called in the constructor for a derived class. First, the base constructor is called. Then, the base-class members are initialized in the order in which they appear in the class declaration. Finally, the derived constructor is called.

```cpp
#include <iostream>

using namespace std;

class Contained1 {
public:
    Contained1() { cout << "Contained1 ctor\n"; }
};

class Contained2 {
public:
    Contained2() { cout << "Contained2 ctor\n"; }
};

class Contained3 {
public:
    Contained3() { cout << "Contained3 ctor\n"; }
};

class BaseContainer {
public:
    BaseContainer() { cout << "BaseContainer ctor\n"; }
private:
    Contained1 c1;
    Contained2 c2;
};

class DerivedContainer : public BaseContainer {
public:
    DerivedContainer() : BaseContainer() { cout << "DerivedContainer ctor\n"; }
private:
    Contained3 c3;
};

int main() {
    DerivedContainer dc;
}
```

Here's the output:

```
Contained1 ctor
Contained2 ctor
BaseContainer ctor
Contained3 ctor
DerivedContainer ctor
```

A derived class constructor always calls a base class constructor, so that it can rely on completely constructed base classes before any extra work is done. The base class constructors are called in order of derivation—for example, if `ClassA` is derived from `ClassB`, which is derived from `ClassC`, the `ClassC` constructor is called first, then the `ClassB` constructor, then the `ClassA` constructor.

If a base class doesn't have a default constructor, you must supply the base class constructor parameters in the derived class constructor:


```cpp
class Box {
public:
    Box(int width, int length, int height){
       m_width = width;
       m_length = length;
       m_height = height;
    }

private:
    int m_width;
    int m_length;
    int m_height;
};

class StorageBox : public Box {
public:
    StorageBox(int width, int length, int height, const string label&) : Box(width, length, height){
        m_label = label;
    }
private:
    string m_label;
};

int main(){

    const string aLabel = "aLabel";
    StorageBox sb(1, 2, 3, aLabel);
}
```

If a constructor throws an exception, the order of destruction is the reverse of the order of construction:

1. The code in the body of the constructor function is unwound.
    
2. Base class and member objects are destroyed, in the reverse order of declaration.
    
3. If the constructor is non-delegating, all fully constructed base class objects and members are destroyed. However, because the object itself isn't fully constructed, the destructor isn't run.