## 概述

继承和组合同时使用

动态（组合）地给一个对象增加一些额外的职责。就增加功能而言，Decorator 模式比生成子类（继承）更为灵活（消除重复代码 & 减少子类个数）。 ——《设计模式》GoF

**动机**

- 在某些情况下我们可能会 “过度地使用继承来扩展对象的功能”，由于**继承为类型引入的静态特质，使得这种扩展方式缺乏灵活性**；并且随着子类的增多（扩展功能的增多），各种子类的组合（扩展功能的组合）会导致更多子类的膨胀。
    
- 如何**使 “对象功能的扩展” 能够根据需要来动态地实现**？同时**避免 “扩展功能的增多” 带来的子类膨胀问题**？从而使得任何 “功能扩展变化” 所导致的影响将为最低？

## 代码

### 初始代码

```cpp
//主题类
class Stream {
public:
    virtual char Read(int number) = 0;
    virtual void Seek(int position) = 0;
    virtual void Write(char data) = 0;

    virtual ~Stream() {
    }
};

//主体类
class FileStream : public Stream {
public:
    virtual char Read(int number) {
        //读文件流
    }
    virtual void Seek(int position) {
        //定位文件流
    }
    virtual void Write(char data) {
        //写文件流
    }
};

class NetworkStream : public Stream {
public:
    virtual char Read(int number) {
        //读网络流
    }
    virtual void Seek(int position) {
        //定位网络流
    }
    virtual void Write(char data) {
        //写网络流
    }
};

class MemoryStream : public Stream {
public:
    virtual char Read(int number) {
        //读内存流
    }
    virtual void Seek(int position) {
        //定位内存流
    }
    virtual void Write(char data) {
        //写内存流
    }
};

//扩展操作
class CryptoFileStream : public FileStream {
public:
    virtual char Read(int number) {
        //额外的加密操作...
        FileStream::Read(number);  //读文件流
    }
    virtual void Seek(int position) {
        //额外的加密操作...
        FileStream::Seek(position);  //定位文件流
        //额外的加密操作...
    }
    virtual void Write(byte data) {
        //额外的加密操作...
        FileStream::Write(data);  //写文件流
        //额外的加密操作...
    }
};

class CryptoNetworkStream : public NetworkStream {
public:
    virtual char Read(int number) {
        //额外的加密操作...
        NetworkStream::Read(number);  //读网络流
    }
    virtual void Seek(int position) {
        //额外的加密操作...
        NetworkStream::Seek(position);  //定位网络流
        //额外的加密操作...
    }
    virtual void Write(byte data) {
        //额外的加密操作...
        NetworkStream::Write(data);  //写网络流
        //额外的加密操作...
    }
};

class CryptoMemoryStream : public MemoryStream {
public:
    virtual char Read(int number) {
        //额外的加密操作...
        MemoryStream::Read(number);  //读内存流
    }
    virtual void Seek(int position) {
        //额外的加密操作...
        MemoryStream::Seek(position);  //定位内存流
        //额外的加密操作...
    }
    virtual void Write(byte data) {
        //额外的加密操作...
        MemoryStream::Write(data);  //写内存流
        //额外的加密操作...
    }
};开门,，k

class BufferedFileStream : public FileStream {
    //...
};

class BufferedNetworkStream : public NetworkStream {
    //...
};

class BufferedMemoryStream : public MemoryStream {
    //...
};

class CryptoBufferedFileStream : public FileStream {
public:
    virtual char Read(int number) {
        //额外的加密操作...
        //额外的缓冲操作...
        FileStream::Read(number);  //读文件流
    }
    virtual void Seek(int position) {
        //额外的加密操作...
        //额外的缓冲操作...
        FileStream::Seek(position);  //定位文件流
        //额外的加密操作...
        //额外的缓冲操作...
    }
    virtual void Write(byte data) {
        //额外的加密操作...
        //额外的缓冲操作...
        FileStream::Write(data);  //写文件流
        //额外的加密操作...
        //额外的缓冲操作...
    }
};

void Process() {
    //编译时装配
    CryptoFileStream *fs1 = new CryptoFileStream();

    BufferedFileStream *fs2 = new BufferedFileStream();

    CryptoBufferedFileStream *fs3 = new CryptoBufferedFileStream();
}

```
存在问题：

- 违背 [[面相对象设计原则#组合优于继承]]

继承会静态地确定了父类的具体类型，因而如果要给`Stream`的子类`FileStream`，`NetworkStream`修改其函数添加新的功能时，就无可避免的要为每个类都派生出添加了新功能的类，然后重写方法。

### 重构代码

重构版本一

```cpp
//业务操作
class Stream {
public:
    virtual char Read(int number) = 0;
    virtual void Seek(int position) = 0;
    virtual void Write(char data) = 0;

    virtual ~Stream() {
    }
};

//主体类
class FileStream : public Stream {
public:
    virtual char Read(int number) {
        //读文件流
    }
    virtual void Seek(int position) {
        //定位文件流
    }
    virtual void Write(char data) {
        //写文件流
    }
};

class NetworkStream : public Stream {
public:
    virtual char Read(int number) {
        //读网络流
    }
    virtual void Seek(int position) {
        //定位网络流
    }
    virtual void Write(char data) {
        //写网络流
    }
};

class MemoryStream : public Stream {
public:
    virtual char Read(int number) {
        //读内存流
    }
    virtual void Seek(int position) {
        //定位内存流
    }
    virtual void Write(char data) {
        //写内存流
    }
};

//扩展操作
// 这里的继承是为了完成接口规范
class CryptoStream : public Stream {
    // 具体实现的发起方
    // 组合引出多态
    Stream* stream;  //...

public:
    // 不能直接实例 Stream
    // 可以实例主体的分支
    // - FileStream
    // - NetworkStream
    // - MemoryStream
    CryptoStream(Stream* stm) : stream(stm) {
    }

    virtual char Read(int number) {
        //额外的加密操作...
        stream->Read(number);  //读文件流
    }
    virtual void Seek(int position) {
        //额外的加密操作...
        stream->Seek(position);  //定位文件流
        //额外的加密操作...
    }
    virtual void Write(byte data) {
        //额外的加密操作...
        stream->Write(data);  //写文件流
        //额外的加密操作...
    }
};

class BufferedStream : public Stream {
    Stream* stream;  //...

public:
    BufferedStream(Stream* stm) : stream(stm) {
    }
    //...
};

void Process() {
    //运行时装配
    FileStream* s1 = new FileStream();

    CryptoStream* s2 = new CryptoStream(s1);
    MemoryStream* s3 = new MemoryStream(s1);
    BufferedStream* s4 = new BufferedStream(s2);
}

```
重构版本 2

```cpp
//业务操作
class Stream {
public:
    virtual char Read(int number) = 0;
    virtual void Seek(int position) = 0;
    virtual void Write(char data) = 0;

    virtual ~Stream() {
    }
};

//主体类
class FileStream : public Stream {
public:
    virtual char Read(int number) {
        //读文件流
    }
    virtual void Seek(int position) {
        //定位文件流
    }
    virtual void Write(char data) {
        //写文件流
    }
};

class NetworkStream : public Stream {
public:
    virtual char Read(int number) {
        //读网络流
    }
    virtual void Seek(int position) {
        //定位网络流
    }
    virtual void Write(char data) {
        //写网络流
    }
};

class MemoryStream : public Stream {
public:
    virtual char Read(int number) {
        //读内存流
    }
    virtual void Seek(int position) {
        //定位内存流
    }
    virtual void Write(char data) {
        //写内存流
    }
};

//扩展操作
// 中间的装饰类，存放共同所有的基类指针
DecoratorStream : public Stream {
protected:
    Stream* stream;  //...

    DecoratorStream(Stream * stm) : stream(stm) {
    }
};

class CryptoStream : public DecoratorStream {
public:
    CryptoStream(Stream* stm) : DecoratorStream(stm) {
    }

    virtual char Read(int number) {
        //额外的加密操作...
        stream->Read(number);  //读文件流
    }
    virtual void Seek(int position) {
        //额外的加密操作...
        stream->Seek(position);  //定位文件流
        //额外的加密操作...
    }
    virtual void Write(byte data) {
        //额外的加密操作...
        stream->Write(data);  //写文件流
        //额外的加密操作...
    }
};

class BufferedStream : public DecoratorStream {
public:
    BufferedStream(Stream* stm) : DecoratorStream(stm) {
    }
    //...
};

void Process() {
    //运行时装配
    FileStream* s1 = new FileStream();

    CryptoStream* s2 = new CryptoStream(s1);
    MemoryStream* s3 = new MemoryStream(s1);
    BufferedStream* s4 = new BufferedStream(s2);
}

```
这种设计模式的特殊形式：**既继承基类，又有基类指针成员**。但是这两者的作用是不一样的

- **继承是为了定义接口规范** *，使得装饰器类对象能被基类指针所指向*
- **基类指针成员是为了实现具体操作**

*最后一层装饰器类不使用继承也可以，但是这样该装饰器类就不能再构造其他装饰器类（因为基类指针只能存放其派生类对象）*

Decorator 类中保存一个基类指针并使用动态绑定（`new Decorator(基类的派生对象);`）。 Decorator 对象能够将基类指针指向构造时传入的派生对象，根据派生对象的动态类型动态调用不同派生类的方法。
## 总结

![[装饰器模式 Decorator.png]]

- **通过采用组合而非继承的手法， Decorator 模式实现了在运行时动态扩展对象功能的能力**，而且可以根据需要扩展多个功能。避免了使用继承带来的 “灵活性差” 和“多子类衍生问题”。
- Decorator 类在接口上表现为 is-a Component 的继承关系，即 Decorator 类继承了 Component 类所具有的接口。但在实现上又表现为 has-a Component 的组合关系，即 Decorator 类又使用了另外一个 Component 类。
- Decorator 模式的目的并非解决 “多子类衍生的多继承” 问题，Decorator 模式应用的要点在于解决 “主体类在多个方向上的扩展功能”——是为“装饰” 的含义。 *从抽象到具体方向，功能拓展方向*

```cpp
// 简单实现
class Stream {
  public:
    virtual void print(std::string s) = 0;
    // virtual ~Stream() = 0;
};
class FileStream : public Stream {
  public:
    void print(std::string s) { std::cout << "FileStream : " << s << "\n"; }
};

class CryptoStream : public Stream {
  public:
    CryptoStream(Stream *stream) : stream_(stream) {}
    Stream *stream_;
    void print(std::string s) {
        std::cout << "Crypto  {\n";
        stream_->print(s);
        std::cout << "}\n";
    }
};
class BufferedStream : public Stream {
  public:
    BufferedStream(Stream *stream) : stream_(stream) {}
    Stream *stream_;
    void print(std::string s) {
        std::cout << "Buffered{\n";
        stream_->print(s);
        std::cout << "}\n";
    }
};

int main(void) {
    FileStream *fs = new FileStream();
    CryptoStream *cs = new CryptoStream(fs);
    // cs->print("test");
    BufferedStream *bs = new BufferedStream(cs);
    bs->print("test");

```