## 概述

定义对象间的一种一对多（变化）的依赖关系，以便当一个对象 (Subject) 的状态发生改变时，所有依赖于它的对象都得到通知并自动更新。 ——《设计模式》GoF

**动机**

- 在软件构建过程中，我们需要为某些对象建立一种 “通知依赖关系” ——一个对象（目标对象）的状态发生改变，所有的依赖对象（观察者对象）都将得到通知。如果这样的依赖关系过于紧密，将使软件不能很好地抵御变化。
    
- 使用面向对象技术，可以将这种依赖关系弱化，并形成一种稳定的依赖关系。从而实现软件体系结构的松耦合。
## 代码

### 初始代码

```cpp
// Sender
class FileSplitter {
    string m_filePath;
    int m_fileNumber;
	// 具体的进度条
    ProgressBar* m_progressBar;

public:
    FileSplitter(const string& filePath, int fileNumber,
                 ProgressBar* progressBar)
        : m_filePath(filePath),
          m_fileNumber(fileNumber),
          m_progressBar(progressBar) {
    }

    void split() {
        // 1.读取大文件

        // 2.分批次向小文件中写入
        for (int i = 0; i < m_fileNumber; i++) {
            //...
            float progressValue = m_fileNumber;
            progressValue = (i + 1) / progressValue;
            m_progressBar->setValue(progressValue);
        }
    }
};

// Receiver
class MainForm : public Form {
    TextBox* txtFilePath;
    TextBox* txtFileNumber;
    ProgressBar* progressBar;

public:
    void Button1_Click() {
        string filePath = txtFilePath->getText();
        int number = atoi(txtFileNumber->getText().c_str());

        FileSplitter splitter(filePath, number, progressBar);

        splitter.split();
    }
};

```
存在问题：

- 违背 [[面相对象设计原则#依赖倒置原则 (DIP)]]

`FileSplitter`依赖与具体的实现细节：`ProgressBar`，如果要是用其他显示进度的方式则需要修改`FileSplitter`的代码
### 改进代码

**改进后，subject 中处理事件的对象和方法（抽象）并不依赖于`ProgressBar`（具体类对象），而是依赖于`IProgress`（抽象基类）**。*处理事件的方法和对象是抽象的，因为可以有各种不同的处理方式*
**subject**

1. 使用一个抽象基类作为处理事件的 observer，将事件处理方法作为抽象方法
2. 在subject（一）中存储 observer（多）的指针
3. subject 中实现 notify 机制，调用 oberver 的抽象方法
4. subject 中实现添加、删除 oberver 的接口

**当前observer**

1. 继承 observer 并override 事件处理方法
2. 将当前 observer 注册到 subject 中

```cpp
class IProgress {
public:
    virtual void DoProgress(float value) = 0;
    virtual ~IProgress() {
    }
};

class FileSplitter {
    string m_filePath;
    int m_fileNumber;

    List<IProgress*> m_iprogressList;  // 抽象通知机制，支持多个观察者

public:
    FileSplitter(const string& filePath, int fileNumber)
        : m_filePath(filePath), m_fileNumber(fileNumber) {
    }

    void split() {
        // 1.读取大文件

        // 2.分批次向小文件中写入
        for (int i = 0; i < m_fileNumber; i++) {
            //...

            float progressValue = m_fileNumber;
            progressValue = (i + 1) / progressValue;
            onProgress(progressValue);  //发送通知
        }
    }

    void addIProgress(IProgress* iprogress) {
        m_iprogressList.push_back(iprogress);
    }

    void removeIProgress(IProgress* iprogress) {
        m_iprogressList.remove(iprogress);
    }

protected:
    virtual void onProgress(float value) {
        List<IProgress*>::iterator itor = m_iprogressList.begin();

        while (itor != m_iprogressList.end()) {
            (*itor)->DoProgress(value);  //更新进度条
            itor++;
        }
    }
};

// observer
class MainForm : public Form, public IProgress {
    TextBox* txtFilePath;
    TextBox* txtFileNumber;

    ProgressBar* progressBar;

public:
    void Button1_Click() {
        string filePath = txtFilePath->getText();
        int number = atoi(txtFileNumber->getText().c_str());

        ConsoleNotifier cn;

        FileSplitter splitter(filePath, number);

        splitter.addIProgroProgress(float value) {
        progressBar->setValue(value);
    }
};

class ConsoleNotifier : public IProgress {
public:
    virtual void DoProgress(float value) {
        cout << ".";
    }
};

```

简单实现了一下 Oberver 模式。

```cpp
#include <iostream>
#include <list>

class Observer {
  public:
    virtual void update (const std::string &message) = 0;
};

class ConcreteObserver : public Observer {
  public:
    ConcreteObserver(const std::string &name) : name(name) {}

    void update(const std::string &message) override {
        std::cout << name << " received message: " << message << std::endl;
    }

  private:
    std::string name;
};

class Subject {
  public:
    void attach(Observer *observer) { observerList.push_back(observer); }
    void detach(Observer *observer) {
        observerList.remove(observer);
    }
    void notify(const std::string &message){
        for (auto observer : observerList) {
            observer->update(message);
        }
    }
    std::list<Observer *> observerList;
};

class ConcreteSubject : public Subject {
  public:
    void setState(const std::string &state) {
        this->state = state;
        notify("State changed to: " + state);
    }

  private:
    std::string state;
};

int main() {
    ConcreteObserver observer1("Observer 1");
    ConcreteObserver observer2("Observer 2");

    ConcreteSubject subject;
    subject.attach(&observer1); // 订阅通知
    subject.attach(&observer2);

    subject.setState("Hello");

    subject.detach(&observer1);

    subject.setState("world");

    return 0;
```

通过访问器访问的版本（better）


```cpp
#include <iostream>
#include <list>

class Observer {
  public:
    Observer(Subject* sub){ m_sub = sub;}
    virtual void update () = 0;
  protected:
    Subject m_sub;
};

class ConcreteObserver : public Observer {
  public:
    ConcreteObserver(Subject* sub, const std::string &name) : m_sub(sub), name(name) {}

    void update() override {
        ConcreteSubject* pSub = static_cast<ConcreteSubject*>(m_sub);
        // 通过访问器访问所需要的数据
        std::string mesesage = pSub->getState;
        std::cout << name << " received message: " << message << std::endl;
    }

  private:
    std::string name;
};

class Subject {
  public:
    void attach(Observer *observer) { observerList.push_back(observer); }
    void detach(Observer *observer) {
        observerList.remove(observer);
    }
    void notify(){
        for (auto observer : observerList) {
            observer->update();
        }
    }
    std::list<Observer *> observerList;
};

class ConcreteSubject : public Subject {
  public:
    void setState(const std::string &state) {
        this->state = state;
        notify();
    }
    std::string getState() {
        return state;
    }

  private:
    std::string state;
};

int main() {
    ConcreteObserver observer1("Observer 1");
    ConcreteObserver observer2("Observer 2");

    ConcreteSubject subject;
    subject.attach(&observer1); // 订阅通知
    subject.attach(&observer2);

    subject.setState("Hello");

    subject.detach(&observer1);

    subject.setState("world");

    return 0;
```

## 总结


![[观察者模式 Observer.png]]

- 使用面向对象的抽象，Observer 模式使得我们可以独立地改变目标与观察者，从而使二者之间的依赖关系变成松耦合。
- 目标发送通知时，无需指定观察者，通知（可以携带通知信息作为参数）会自动传播。*不需要指定具有具体实现细节的观察者对象，而是直接将通知发送给抽象的 Observer 基类，具体的观察者对象就会收到通知*
- 观察者自己决定是否需要订阅通知
- Observer 模式是基于事件的 UI 框架中非常常用的设计模式，也是 MVC 模式的一个重要组成部分。