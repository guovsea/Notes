```c
#include <arpa/inet.h>
#include <asm-generic/errno-base.h>
#include <asm-generic/errno.h>
#include <ctype.h>
#include <errno.h>
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

void sys_err(const char *str) {
    perror(str);
    exit(1);
}

typedef struct {
    int flag;
    int fd;
    struct sockaddr_in addr;
} clit_info;

int main(int argc, char *argv[]) {
    int clit_fd, listenfd;
    struct sockaddr_in serv_addr, clit_addr;
    char clit_addr_buf[INET_ADDRSTRLEN];
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, SERV_ADDR, &serv_addr.sin_addr);
    if ((bind(listenfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr))) <
        0) {
        sys_err("bind");
    }
    listen(listenfd, 10);

    socklen_t clit_len;
    int connfd, epfd;
    int nready, nread;
    char rdbuf[BUF_SIZE];
        int poll_num = 0;
    struct epoll_event ev, evs[EPOLLMAX];
    clit_info clit_set[CLIT_MAX];
    for (int i = 0; i < CLIT_MAX; i++) {
        clit_set[i].flag = 0;
    }

    epfd = epoll_create(EPOLLMAX);
    ev.events = EPOLLIN;
    ev.data.fd = listenfd;
    epoll_ctl(epfd, EPOLL_CTL_ADD, listenfd, &ev);

    printf("waiting for connectin...\n");
    while (1) {
        nready = epoll_wait(epfd, evs, EPOLLMAX, -1);
        for (int ep = 0; ep < nready; ep++) {
            if (evs[ep].data.fd == listenfd) {
                clit_len = sizeof(clit_addr);
                if ((connfd = accept(listenfd, (struct sockaddr *)&clit_addr,
                                     &clit_len)) < 0) {
                    if (errno == EAGAIN || errno == EWOULDBLOCK) {
                        continue;
                    }
                    sys_err("accept");
                }
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
                                         clit_addr_buf, sizeof(clit_addr_buf)),
                               ntohs(clit_addr.sin_port), i);
                        break;
                    }
                }
                ev.events = EPOLLIN;
                ev.data.fd = connfd;
                epoll_ctl(epfd, EPOLL_CTL_ADD, connfd, &ev);

            } else {
                                if (clit_set[i].flag == 0) {
                        clit_set[i].flag = 1;
                        clit_set[i].fd = connfd;
                        clit_set[i].addr = clit_addr;
                        printf("%s:%d connected, adapted number is %d\n",
                               inet_ntop(AF_INET, &clit_addr.sin_addr,
                                         clit_addr_buf, sizeof(clit_addr_buf)),
                               ntohs(clit_addr.sin_port), i);
                        break;
                    }
                }
                ev.events = EPOLLIN;
                ev.data.fd = connfd;
                epoll_ctl(epfd, EPOLL_CTL_ADD, connfd, &ev);

            } else {
                clit_fd = evs[ep].data.fd;
                if ((nread = read(clit_fd, rdbuf, BUF_SIZE)) > 0) {
                    for (int i = 0; i < nread; i++) {
                        rdbuf[i] = toupper(rdbuf[i]);
                    }
                    write(clit_fd, rdbuf, nread);
                } else if (nread == 0) {
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
                } else {
                    sys_err("read");
                }
            }
        }
    }
    return EXIT_SUCCESS;
}
```
 整体流程是`socket() -- bind()`，设置`listenfd`的`epoll_evnet`事件，**`EPOLL_CTL_ADD `添加`listenfd`并绑定事件**。死循环：**`epol_wait` 阻塞**，如果阻塞返回，通过循环判断`evs[ep].data.fd == listenfd`判断就绪的是`listenfd`还是`connfd`。如果是`listenfd`请加上，使用 accept 建立新的连接，并使用`EPOLL_CTL_ADD`将新连接添加到`epoll`的监听集合中。如果是`connfd`就绪，进行读写操作，**当发现客户端关闭连接时，手动使用`EPOLL_CTL_DEL`将`connfd`从`epoll`的监听集合中并关闭连接**。


**几点注意**

1. 代码并没有完善的出错检查
2. 要将`listenfd`也添加进`epoll`的监听集合中
3. 需要设置`epoll_wait`为阻塞模式
4. 客户端关闭连接时（read 返回 0），要将连接的 socket 从 epoll 中删除