## 使用方法

```cpp
class HttpMgr :  public QObject,public Singleton<HttpMgr>
// class HttpMgr : public Singleton<HttpMgr>, public QObject // NO !!!!!!!!!!!
{
    Q_OBJECT;
    // friend class Singleton<HttpMgr>;
public:
    ~HttpMgr(){}


private:
    HttpMgr(){}
};
```

>注意：要先继承 QObject 再继承其他类，否则 MOC 生成的文件会报错

```
no member named 'qt_metacast' in 第一个类  did you mean simply 'qt_metacall'
```

> 很难 debug