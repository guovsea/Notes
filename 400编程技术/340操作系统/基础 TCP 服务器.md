## TCP 服务器模型

![[S模型-TCP.png]]

和 [[Socket 编程#TCP socket 传输模型|TCP 状态图]] 联系起来理解

服务器调用 socket()、bind()、listen()完成初始化后，调用 accept()阻塞等待，处于监听端口的状态，客户端调用 socket()初始化后，调用 connect()发出 SYN 段并阻塞等待服务器应答，服务器应答一个 SYN-ACK 段，客户端收到后从 connect()返回，同时应答一个 ACK 段，服务器收到后从 accept()返回。

数据传输的过程：

建立连接后，TCP 协议提供全双工的通信服务，但是一般的客户端/服务器程序的流程是由客户端主动发起请求，服务器被动处理请求，一问一答的方式。因此，服务器从 accept () 返回后立刻调用 read()，读 socket 就像读管道一样，如果没有数据到达就阻塞等待，这时客户端调用 write()发送请求给服务器，服务器收到后从 read()返回，对客户端的请求进行处理，在此期间客户端调用 read()阻塞等待服务器的应答，服务器调用 write()将处理结果发回给客户端，再次调用 read()阻塞等待下一条请求，客户端收到后从 read()返回，发送下一条请求，如此循环下去。

如果客户端没有更多的请求了，就调用 close() 关闭连接，就像写端关闭的管道一样，服务器的 read()返回0，这样服务器就知道客户端关闭了连接，也调用 close()关闭连接。注意，任何一方调用 close()后，连接的两个传输方向都关闭，不能再发送数据了。如果一方调用 shutdown()则连接处于半关闭状态，仍可接收对方发来的数据。

在学习 socket API 时要注意应用程序和 TCP 协议层是如何交dd 应用程序调用某个 socket 函数时 TCP 协议层完成什么动作，比如调用 connect()会发出 SYN 段 应用程序如何知道 TCP 协议层的状态变化，比如从某个阻塞的 socket 函数返回就表明 TCP 协议收到了某些段，再比如 read()返回0就表明收到了 FIN 段


## 程序结构

### 服务器程序结构

```c
listenfd = socket(AF_INET, SOCK_STREAM, 0);
bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
listen(listenfd, 20);
while (1) {
	connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &cliaddr_len);
	n = read(connfd, buf, MAXLINE);
	// ...
	close(connfd);
}
```

**`socket()`创建好一个`socket`后，使用`bind()`和`listen()`函数对`这个socket`的一些属性进行填充。然后进入 while 死循环使用`accept()`等待客户端的连接。** 没有客户端建立连接时，`accept()`将被阻塞，直到有客户端和服务器完成三次握手建立连接，`accept`将返回建立连接的文件描述符，然后对文件描述符进行读写操作传输数据。每次循环处理一个客户端连接。


### 客户端程序结构

```c
sockfd = socket(AF_INET, SOCK_STREAM, 0);
connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
write(sockfd, str, strlen(str));
close(sockfd)
```

### TCP 服务器天然不具有并发性

TCP 协议是面向连接的，一个 socket 只能和一个客户端建立连接。在“普通”的处理流程（指未使用并发技术、未使用 IO 多路复用技术）中只能操作一个 socketfd，只能和一个客户端交互。因此，要想让 TCP 服务器具有并发性：

1. 一个进程/线程：本质上需要轮询所有建立了连接的 socketfd，分时间段对不同客户端交互。
2. 多个进程/线程：分别处理 socketfd，同时对多个客户端交互

## 示例

### server
```c
#include <arpa/inet.h>
#include <ctype.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define PORT 8091
#define ADDRESS "192.168.10.10"
#define BUFF_SIZE 1024

int main(int argc, char *argv[]) {
    int listenfd, connfd;
    struct sockaddr_in serv_addr, client_addr;
    char client_addr_str[INET_ADDRSTRLEN];
    socklen_t client_addr_len;
    char data[BUFF_SIZE];
    int data_len;
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd < 0) {
        perror("socket error");
    }

    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    inet_pton(AF_INET, ADDRESS, &serv_addr.sin_addr);
    serv_addr.sin_port = htons(PORT);

    if (bind(listenfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("bind error");
    }
    listen(listenfd, 2);

    while (1) {
        client_addr_len = sizeof(client_addr_str);
        connfd =
            accept(listenfd, (struct sockaddr *)&client_addr, &client_addr_len);
        if (connfd < 0) {
            perror("accept error");
        }
        inet_ntop(AF_INET, &client_addr, client_addr_str.sin_addr, sizeof(client_addr));
        printf("%s connected\n", client_addr_str);
        while (1) {
            if ((data_len = read(connfd, data, BUFF_SIZE)) > 0) {
                for (int i = 0; i < data_len; i++) {
                    data[i] = toupper(data[i]);
                }
                write(connfd, data, data_len);
            } else if (data_len == 0) {
                printf("%s disconnected\n", client_addr_str);
                break;
            } else {
                perror("read from connfd error");
                break;
            }
        }
        close(connfd);
    }
    return 0;
}
```
几点注意：

- **如果没有客户端连接到服务器，服务器将在 accept 阻塞**，直到有客户端和服务器三次握手完成，accept 才会返回这次 TCP 连接的文件描述符
- **最外层的 while 死循环是为了处理完某一次连接后，可以继续处理下一次连接**（也将会到阻塞在 accept）
- accept 的第三个参数`addrlen`的类型为`socklen_t size`，同时他是个传入传出参数，accept 每次返回之后它的值都会被改变，因此**在每次循环调用 accept 之前都要重新初始化`addrlen`**
- 建立连接后如果在内层循环中对 connfd 持续进行 read 和 write，那么服务器不会主动关闭连接，可以和客户端进行持续的交互，直到客户端主动关闭连接。*客户端关闭连接时，再对 connfd 使用 read，read 会返回 0*
- **注意：如果不使用 `close(connfd)` 关闭连接套接字的话，下一次循环时`connfd`会被新的连接套接字覆盖，也就将失去对上一次的连接套接字的控制** *产生了资源泄露，相当于开辟了一块内存空间，但并没有指针指向它。这样这块内存既不能被访问也不能被释放。*

### client

```c
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define MAXLINE 80
#define SERV_PORT 8091
#define SERV_ADDRESS "192.168.10.10"

int main(int argc, char *argv[]) {
    struct sockaddr_in servaddr;
    char buf[MAXLINE];
    int sockfd, n;
    short port = SERV_PORT;
    if (argc != 2) {
        fputs("use default port\n", stderr);
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket error");
    }

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    inet_pton(AF_INET, SERV_ADDRESS, &servaddr.sin_addr);
    servaddr.sin_port = htons(SERV_PORT);

    if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("connect error");
    }
    while (1) {
        if ((n = read(STDIN_FILENO, buf, sizeof(buf))) < 0) {
            perror("read error");
        }
        write(sockfd, buf, n);

        n = read(sockfd, buf, MAXLINE);
        printf("Response from server:\n");
        write(STDOUT_FILENO, buf, n);
    }
    close(sockfd);

    return 0;
}
```