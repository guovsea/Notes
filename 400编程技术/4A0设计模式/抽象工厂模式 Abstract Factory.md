## 概述

**动机**

- 在软件系统中，经常面临着 “**一系列相互依赖的对象**” 的创建工作；同时，由于需求的变化，往往存在更多系列对象的创建工作。 *有很多组对象，但每组之间又是相互依赖的*
   
- 如何应对这种变化？如何绕过常规的对象创建方法 (new)，提供一种“封装机制” 来避免客户程序和这种“多系列具体对象创建工作” 的紧耦合？

**定义**

提供一个接口，让该接口负责创建一系列 “相关或者相互依赖的对象”，无需指定它们具体的类。 ——《设计模式》GoF
## 代码

### 初始代码

```cpp
class EmployeeDAO {
public:
    vector<EmployeeDO> GetEmployees() {
        SqlConnection* connection = new SqlConnection();
        connection->ConnectionString("...");

        SqlCommand* command = new SqlCommand();
        command->CommandText("...");
        command->SetConnection(connection);

        SqlDataReader* reader = command->ExecuteReader();
        while (reader->Read()) {
        }
    }
};

```

### 重构代码

**工厂模式**

```cpp
// 数据库访问有关的基类
class IDBConnection {};
class IDBConnectionFactory {
public:
    virtual IDBConnection* CreateDBConnection() = 0;
};

class IDBCommand {};
class IDBCommandFactory {
public:
    virtual IDBCommand* CreateDBCommand() = 0;
};

class IDataReader {};
class IDataReaderFactory {
public:
    virtual IDataReader* CreateDataReader() = 0;
};

// 支持SQL Server
class SqlConnection : public IDBConnection {};
class SqlConnectionFactory : public IDBConnectionFactory {};

class SqlCommand : public IDBCommand {};
class SqlCommandFactory : public IDBCommandFactory {};

class SqlDataReader : public IDataReader {};
class SqlDataReaderFactory : public IDataReaderFactory {};

// 支持Oracle
class OracleConnection : public IDBConnection {};
class OracleConnectionFactory : public IDBConnectionFactory {};

class OracleCommand : public IDBCommand {};
class OracleCommandFactory : public IDBCommandFactory {};

class OracleDataReader : public IDataReader {};
class OracleDataReaderFactory : public IDataReaderFactory {};


// 工厂方法 
class EmployeeDAO {
    IDBConnectionFactory* dbConnectionFactory;
    IDBCommandFactory* dbCommandFactory;
    IDataReaderFactory* dataReaderFactory;

public:
    vector<EmployeeDO> GetEmployees() {
        IDBConnection* connection = dbConnectionFactory->CreateDBConnection();
        connection->ConnectionString("...");

        IDBCommand* command = dbCommandFactory->CreateDBCommand();
        command->CommandText("...");
        command->SetConnection(connection);  // 关联性

        IDBDataReader* reader = command->ExecuteReader();  // 关联性
        while (reader->Read()) {
        }
    }
};

```
存在的问题：三个抽象工厂可以指定不同系列的具体工厂，比如 IDBConnection 指定 MySQL 的，IDBCommandFactory 指定 Oracle 的。*这样就存在具体工厂生产的产品不是同意系列，会产生不适配的问题*

**Abstract Facotry 模式**

**解决办法**：将创建一系列的对象（的操作）放一个工厂中，这样工厂每次生产的都将是同一系列的产品
```cpp
// 数据库访问有关的基类
class IDBConnection{};

class IDBCommand {};

class IDataReader {};

// 三个创建操作，绑定到一起
// 此处是这个模式的稳定部分
class IDBFactory {
public:
    virtual IDBConnection* CreateDBConnection() = 0;
    virtual IDBCommand* CreateDBCommand() = 0;
    virtual IDataReader* CreateDataReader() = 0;
};

// 支持SQL Server
class SqlConnection : public IDBConnection {};
class SqlCommand : public IDBCommand {};
class SqlDataReader : public IDataReader {};

class SqlDBFactory : public IDBFactory {
public:
    IDBConnection* CreateDBConnection() override {
        return new SqlConnection;
        }
     IDBCommand* CreateDBCommand() override {
        return new SqlCommand; 
     }
    IDataReader* CreateDataReader() override {
        return new SqlDataReader;
    }
};

// 支持Oracle
class OracleConnection : public IDBConnection {};
class OracleCommand : public IDBCommand {};
class OracleDataReader : public IDataReader {};

class OracleDBFactory : public IDBFactory {
public:
    IDBConnection* CreateDBConnection() {}
    IDBCommand* CreateDBCommand() {}
    IDataReader* CreateDataReader() {}
};

class EmployeeDAO {
    // 保证是同一个工厂
    // 是一个 family
    IDBFactory* dbFactory;

public:
    vector<EmployeeDO> GetEmployees() {
        IDBConnection* connection = dbFactory->CreateDBConnection();
        connection->ConnectionString("...");

        IDBCommand* command = dbFactory->CreateDBCommand();
        command->CommandText("...");
        command->SetConnection(connection);  // 关联性

        IDBDataReader* reader = command->ExecuteReader();  // 关联性
        while (reader->Read()) {
        }
    }
};

```

## 总结

![[抽象工厂模式 Abstract Factory.png]]

- 如果没有应对 “**多系列对象构建**” 的需求变化，则没有必要使用 Abstract Factory 模式，这时候使用简单的工厂完全可以。
  
- “系列对象” 指的是在某一特定系列下的对象之间有相互依赖、或作用的关系。不同系列的对象之间不能相互依赖。

- Abstract Factory 模式主要在于应对 “新系列” 的需求变动。其缺点在于难以应对 “新对象” 的需求变动。