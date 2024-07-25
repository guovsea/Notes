## RPC

### 含义
分布式计算中，远程过程调用（英语：Remote Procedure Call，RPC）是一个计算机通信协议。该协议**允许运行于一台计算机的程序调用另一个地址空间的子程序，而程序员就像调用本地程序一样**，无需额外地为这个交互作用编程（无需关注细节）。RPC 是一种服务器-客户端（Client/Server）模式，经典实现是一个通过**发送请求-接受回应**进行信息交互的系统。

如果涉及的软件采用面向对象编程，那么远程过程调用亦可称作远程调用或远程方法调用，例：Java RMI。

RPC是一种进程间通信的模式，程序分布在不同的地址空间里。如果在同一主机里，RPC可以通过不同的虚拟地址空间（即便使用相同的物理地址）进行通讯，而在不同的主机间，则通过不同的物理地址进行交互。许多技术（通常是不兼容）都是基于这种概念而实现的。

![[RPC调用过程.png]]

### 流程
1. 客户端调用客户端 stub（client stub）。这个调用是在本地，并将调用参数 push 到栈（stack）中。
2. 客户端 stub（client stub）将这些参数包装，并通过系统调用发送到服务端机器。打包的过程叫 marshalling。（常见方式：XML、JSON、二进制编码）
3. 客户端本地操作系统发送信息至服务器。（可通过自定义 TCP 协议或 HTTP 传输）
4. 服务器系统将信息传送至服务端 stub（server stub）。
5. 服务端 stub（server stub）解析信息。该过程叫 unmarshalling。
6. 服务端 stub（server stub）调用程序，并通过类似的方式返回给客户端。

### RPC 框架

[[gRPC框架]]


## REST

Representational State Transfer 是一种万维网软件架构风格，目的是便于不同软件/程序在网络（例如互联网）中互相传递信息。REST 基于超文本传输协议（HTTP）之上而确定的一组约束和属性，是一种设计提供万维网络服务的软件构建风格。

符合或兼容于这种架构风格（简称为 REST 或 RESTful）的网络服务，允许客户端发出以统一资源标识符访问和操作网络资源的请求，而**与预先定义好的无状态操作集一致化**。
REST 是设计风格而**不是**标准。REST 通常基于 HTTP、URL、XML 及 HTML 这些现有的广泛流行的协议和标准。

###  特征
- **Uniform Interface：统一接口**

1. 以资源为基础
2. 通过客户端可以修改原资源的状态。
3. 返回信息足够描述自己

- **Stateless：无状态**
- **Cacheable：可缓存**
- **Client-Server：客户服务器分离模式**，任何一个客户端与服务器都是可替换的。
- **Layered System：分层的系统**，客户端不知道他联系的是不是最终服务器。
- **Code on Demand（可选）：服务器可以将能力扩展到客户端**，如果客户端可以执行的话。这个功能是可选择的。

[wiki 百科 REST](https://en.wikipedia.org/wiki/REST)


## SOAP

SOAP（原为Simple Object Access Protocol的首字母缩写，即简单对象访问协议）是交换数据的一种协议规范，使用在计算机网络Web服务（web service）中，交换带结构的信息。SOAP为了简化网页服务器（Web Server）从XML数据库中提取数据时，节省去格式化页面时间，以及不同应用程序之间按照HTTP通信协议，遵从XML格式执行资料互换，使其抽象于语言实现、平台和硬件。

### 语法规则

SOAP 消息必须用 XML 来编码
SOAP 消息必须使用 SOAP Envelope 命名空间
SOAP 消息必须使用 SOAP Encoding 命名空间
SOAP 消息不能包含 DTD 引用
SOAP 消息不能包含 XML 处理指令


## GraphQL

[POST]( https://zh.wikipedia.org/w/index.php?title=POST_ (HTTP)&action=edit&redlink=1)请求：

```json
{
    orders {
        id
        productsList {
            product {
                name
                price
            }
            quantity
        }
        totalAmount
    }
}
```

响应：
```json
{
    "data": {
        "orders": [
            {
                "id": 1,
                "productsList": [
                    {
                        "product": {
                            "name": "orange",
                            "price": 1.5
                        },
                        "quantity": 100
                    }
                ],
                "totalAmount": 150
            }
        ]
    }
}```
