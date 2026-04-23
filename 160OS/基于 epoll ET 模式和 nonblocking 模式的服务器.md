---
tags:
  - os
  - os/network
  - os/io
  - type/howto
mastery: 5
---```c
#include <arpa/inet.h>
#include <asm-generic/errno-base.h>
#include <asm-generic/errno.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUF_SIZE 256
#define CLIT_MAX 3
#define EPOLLMAX CLIT_MAX + 1
#define PORT 8090
#define SERV_ADDR "192.168.10.10"

typedef struct {
    int flag;
    int fd;
    struct sockaddr_in addr;
} clit_info;

void sys_err(const char *str) {
    perror(str);
    exit(1);
}

void nonblocking(int fd) {
    int opts;
    if ((opts = fcntl(fd, F_GETFL)) < 0) {
        sys_err("F_GETFL");
    }
    opts = opts | O_NONBLOCK;
    if ((fcntl(fd, F_SETFL, opts)) < 0) {
        sys_err("F_SETFL");
    }
}

int main(int argc, char *argv[]) {
    int clit_fd, listenfd;
    struct sockaddr_in serv_addr, clit_addr;
    char clit_addr_buf[INET_ADDRSTRLEN];
    listenfd = socket(AF_INET, SOCK_STREAM, 0);

    nonblocking(listenfd);
    
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, SERV_ADDR, &serv_addr.sin_addr);
    if ((bind(listenfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr))) <
        0) {
        sys_err("bind");
    }
    listen(listenfd, 10);

    socklen_t clit_len;
    int connfd, epfd, sockfd;
    int nready, nread, nwrite, n;
    char buf[BUF_SIZE];
    int poll_num = 0;
    struct epoll_event ev, evs[EPOLLMAX];
    clit_info clit_set[CLIT_MAX];
    for (int i = 0; i < CLIT_MAX; i++) {
        clit_set[i].flag = 0;
    }

    epfd = epoll_create(EPOLLMAX);
    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = listenfd;
    epoll_ctl(epfd, EPOLL_CTL_ADD, listenfd, &ev);

    printf("waiting for connectin...\n");
    for (;;) {
        nready = epoll_wait(epfd, evs, EPOLLMAX, -1);
        for (int ep = 0; ep < nready; ep++) {
            if (evs[ep].data.fd == listenfd) {
                clit_len = sizeof(clit_addr);
                while ((connfd = accept(listenfd, (struct sockaddr *)&clit_addr,&clit_len)) > 0) {
                    poll_num++;
                    if (poll_num == EPOLLMAX) {
                        perror("too many connection");
                        exit(1);
                    }

                    for (int i = 0; i < CLIT_MAX; i++) {
                        if (clit_set[i].flag == 0) {
                            clit_set[i].flag = 1;
                            clit_set[i].fd = connfd;
                            clit_set[i].addr = clit_addr;
                            printf("%s:%d connected, adapted number is %d\n",
                                   inet_ntop(AF_INET, &clit_addr.sin_addr,
                                             clit_addr_buf,
                                             sizeof(clit_addr_buf)),
                                   ntohs(clit_addr.sin_port), i);
                            break;
                        }
                    }
                    nonblocking(connfd);
                    ev.events = EPOLLIN | EPOLLET;
                    ev.data.fd = connfd;
                    epoll_ctl(epfd, EPOLL_CTL_ADD, connfd, &ev);
                }

                if (connfd == -1) {
                    if (errno == EAGAIN || errno == EWOULDBLOCK) {
                        continuej;
                    }
                    sys_err("accept");
                }

            } else {
                clit_fd = evs[ep].data.fd;
                n = 0;
                while ((nread = read(clit_fd, buf + n, BUF_SIZE)) > 0) {
                    n += nread;
                }
                if (nread == -1) {
                    if (errno == EAGAIN) {
                        printf("read EAGAIN\n");
                    } else {
                        sys_err("read");
                    }
                }
                if (nread == 0) {
                    ev.events = EPOLLIN;
                    ev.data.fd = clit_fd;
                    epoll_ctl(epfd, EPOLL_CTL_DEL, clit_fd, &ev);
                    close(clit_fd);
                    poll_num--;
                    for (int i = 0; i < CLIT_MAX; i++) {
                        if (clit_set[i].flag == 1 &&
                            clit_set[i].fd == clit_fd) {
                            clit_set[i].flag = 0;
                            printf(
                                "number:%d, %s:%d connected\n", i,
                                inet_ntop(AF_INET, &clit_set[i].addr.sin_addr,
                                          clit_addr_buf, sizeof(clit_addr_buf)),
                                ntohs(clit_addr.sin_port));
                            break;
                        }
                }
                for (int i = 0; i < n; i++) {
                    buf[i] = toupper(buf[i]);
                }
                int data_size = n;
                while (n > 0) {
                    nwrite = write(clit_fd, buf + data_size - n, n);
                    if (nwrite < n) {
                        if (errno == EAGAIN) {
                            printf("write EAGAIN\n");
                            continue;
                        }
                        sys_err("write");
                    }
                    n -= nwrite;
                }
            }
        }
    }
    return EXIT_SUCCESS;
}
```
**几点注意**

1. 在使用 ET 模式时，要设置文件描述符的属性为`O_NONBLOCK`
2. 读 O_NONBLOCK 文件时，要使用`while`轮训，直到读到-1，`errno == EAGAIN` 为止
3. 

## 常用代码片段

**设置 fd 为 O_NONBLOCK**

```c
void nonblocking(int fd) {
    int opts;
    if ((opts = fcntl(fd, F_GETFL)) < 0) {
        sys_err("F_GETFL");
    }
    opts = opts | O_NONBLOCK;
    if ((fcntl(fd, F_SETFL, opts)) < 0) {
        sys_err("F_SETFL");
    }
}
```

**非阻塞读 accept**
```c
while ((connfd = accept()) > 0) { // 有新的连接产生
    // 将新的连接添加到 epoll 中
    }

if (connfd == -1) {
    if (errno == EAGAIN || errno == EWOULDBLOCK) {
        continue;   
        // 继续下一次for (int i = 0; i < nready; ep++)的遍历
        // 查看下一个events[i]事件
    }
    sys_err("accept");
}

```

**非阻塞读 socket**

```c
// n 表示累计读到的字符数
while ((nread = read(clit_fd, buf + n, BUF_SIZE - n)) > 0) {
    n += nread;
}
if (nread == -1) {
    if (errno == EAGAIN) {
        printf("read EAGAIN\n");
    } else {
        sys_err("read");
    }
}
if (nread == 0) {
    // socket 连接关闭
    // 关闭 socket，在红黑树中删除 socket
}

```
**非阻塞写 socket**

```c
// data_size 表示需要写的字符数
// n 表示当前剩余需要写入的字符数
while (n > 0) {
    nwrite = write(clit_fd, buf + data_size - n, n);
    if (nwrite < 0) {
        if (errno == EAGAIN) {
            printf("write EAGAIN");
        } else {
            sys_err("write");
    }
    n -= write;
}

```
