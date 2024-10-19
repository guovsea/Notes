```c
#include <arpa/inet.h>
#include <asm-generic/errno-base.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

#define PORT 8090
#define SERV_ADDR "192.168.10.10"
#define MAX_EVENTS 1024
#define BUF_SIZE 1024

void perr(const char *str) {
    perror(str);
    exit(1);
}

void nonblocking(int fd) {
    int opts;
    opts = fcntl(fd, F_GETFL);
    opts |= O_NONBLOCK;
    fcntl(fd, F_SETFL, opts);
}

struct ev_t {
    int fd;                          //
    int events;                      // 事件类型，EPOLLIN/EPOLLOUT
    void (*callback)(struct ev_t *); //
    int status;                      // 是否在epoll监听集合中
    char buf[BUF_SIZE];              //
    int size;                        // buf 中的数据长度
    long last_active;                // 上次激活时间
};

// 回调函数的参数都是指向ev_t结构体的指针，因为ev_t包括一个fd的所有信息
void accept_conn(struct ev_t *ev);
void send_data(struct ev_t *ev);
void recv_data(struct ev_t *ev);

int loop_fd;
struct ev_t loop_evs[MAX_EVENTS + 1];

// 初始化事件
void init_event(struct ev_t *ev, int fd, void (*callback)(struct ev_t *)) {
    ev->fd = fd;
    ev->events = 0;
    ev->callback = callback;
    ev->status = 0;
    bzero(ev->buf, sizeof(ev->buf));
    ev->size = 0;
    ev->last_active = time(NULL);
}
// 将 ev->fd 添加到 epoll 监听集合中
void polling(int epfd, int events, struct ev_t *ev) {
    if (ev->status == 1) {
        return;
    }
    ev->events = events;
    struct epoll_event event;
    event.events = events;
    event.data.ptr = ev;
    epoll_ctl(epfd, EPOLL_CTL_ADD, ev->fd, &event);
}
// 将 ev->fd 从 epoll 监听集合中删除
void delpolling(int epfd, struct ev_t *ev) {
    ev->status = 0;
    struct epoll_event epv;
    epv.events = 0;
    epv.data.ptr = NULL;
    epoll_ctl(epfd, EPOLL_CTL_DEL, ev->fd, &epv);
}

// 初始化 listenfd， eventpoll
void init_epoll_loop(const char *addr, short port, int listen_size) {
    int listenfd;
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serv_addr;
    struct epoll_event event;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    inet_pton(AF_INET, addr, &serv_addr.sin_addr);
    if (bind(listenfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perr("bind");
    }

    listen(listenfd, listen_size);
    nonblocking(listenfd);

    loop_fd = epoll_create(MAX_EVENTS);
    init_event(&loop_evs[MAX_EVENTS], listenfd, accept_conn);
    polling(loop_fd, EPOLLIN | EPOLLET, &loop_evs[MAX_EVENTS]);
}

// 处理连接事件的回调函数
void accept_conn(struct ev_t *ev) {
    int connfd;
    struct sockaddr_in clit_addr;
    socklen_t clit_len;
    int i;
    clit_len = sizeof(clit_addr);
    while ((connfd = accept(ev->fd, (struct sockaddr *)&clit_addr, &clit_len)) >0) {

        nonblocking(connfd);

        for (i = 0; i < MAX_EVENTS; i++) {
            if (loop_evs[i].status == 0) {
                break;
            }
        }
        if (i == MAX_EVENTS) {
            printf("max conect limit\n");
            return;
        }
        init_event(&loop_evs[i], connfd, recv_data);
        polling(loop_fd, EPOLLIN | EPOLLET, &loop_evs[i]);
    }
    if (connfd == -1) {
        if (errno != EAGAIN) {
            perr("accept\n");
        }
    }
}

// 处理接收数据的回调函数
void recv_data(struct ev_t *ev) {
    ev->last_active = time(NULL);
    int n, readn;
    readn = n = 0;
    while ((readn = read(ev->fd, ev->buf + n, BUF_SIZE - n)) > 0) {
        n += readn;
    }
    if (readn == -1) {
        if (errno != EAGAIN) {
            perr("read");
        }
    }
    if (readn == 0) { // 关闭连接
        close(ev->fd);
        ev->status = 0; 
        delpolling(loop_fd, ev);
    }
    delpolling(loop_fd, ev);
    // 处理完接收数据事件后，便立即将 callback 修改为 send_data。在下次
    // epoll_wait 时会被 检测到，然后调用发送数据的回调函数。因为
    // **当监听事件被修改时， ET 模式下的 epoll 也会被触发**
    ev->callback = send_data;
    ev->size = n;
    polling(loop_fd, EPOLLOUT | EPOLLET, ev);
}

// 处理发送数据的回调函数
void send_data(struct ev_t *ev) {
    ev->last_active = time(NULL);
    int n, writen;
    n = ev->size;
    writen = 0;
    for (int i = 0; i < ev->size; i++) {
        ev->buf[i] = toupper(ev->buf[i]);
    }
    while (n > 0) {
        writen = write(ev->fd, ev->buf + ev->size - n, n);
        if (writen == -1) {
            if (errno != EAGAIN) {
                perr("read");
            }
        }
        n -= writen;
    }

    delpolling(loop_fd, ev);
    // 同理，读-写-读-写...循环
    ev->callback = recv_data;
    polling(loop_fd, EPOLLIN | EPOLLET, ev);
}

int main(int argc, char *argv[]) {
    int nready;
    struct ev_t *ev;
    init_epoll_loop(SERV_ADDR, PORT, 20);
    struct epoll_event events[MAX_EVENTS];
    printf("Wait for coonection ... \n");
    int checkpos = 0, i; // checkpos 保存每次检查的起始位置
    while (1) {
        // 超时验证，每次验证 100 个ev_t，当ev_t有连接且连接 60 秒没有活跃过则关闭
        long now = time(NULL);
        for (int i = 0; i < 100; i++) {
            if (checkpos == MAX_EVENTS) { // 不验证 listenfd（listenfd在evs[MAX_EVENTS]的位置）
                checkpos == 0;
            }
            if (loop_evs[i].status == 0) { // 没有连接
                continue;
            }
            long duration = now - loop_evs[checkpos].last_active;
            if (duration >= 60) {
                close(loop_evs[i].fd);
                delpolling(loop_fd, &loop_evs[checkpos]);
            }
            checkpos++;
        }
        nready = epoll_wait(loop_fd, events, MAX_EVENTS, -1);
        for (int i = 0; i < nready; i++) {
            ev = (struct ev_t *)events[i].data.ptr;
            ev->callback(ev);
        }
    }
    return EXIT_SUCCESS;
}

```
**流程图**

![[基于 ET+nonblocking 模式，使用回调函数的服务器.png]]

**要点**

在这个模式中，**向 epoll 注册 fd 和 epoll_event 时，`event.data.ptr`指向某个保存连接信息的 ev_t**，当有 epoll_wait 返回时，将返回`event.data.ptr`，也就形成了一个`fd发生事件 -> ev_t`的映射。

因此，每个事件设置不同的处理流程，都可以有不同的信息，非常灵活。

1. 当有连接到来时，因为 listenfd 绑定的是 accept_conn，因此它就会创建一个新的`ev_t`结构体，保存连接信息，设置好它的 callback。在下次 epoll_wait 时如果它就绪了，就会调用它的 callback。
2. 对于连接 fd，首先设置的是`recv`回调函数，recv 会将 fd 的回调函数设置为 send，send 会将 fd 的回调函数设置为 send。这样就能够形成`connected -> recv -> send -> recv -> ...`一收一发的模型。当然，**每次设置后都要将 fd 重新加入到 epoll 监听集合中**。
3. 函调函数使用的参数可以单单只是一个指向 ev_t 的指针，因为 ev_t 包含了一个连接的所有信息，每个连接的也都有一个单独的 buf。
## Reactor 模式

![[基于单线程 Reactor 模式的服务器.png]]

在代码中，main 函数就是 Reactor，accept_conn 就是 Acceptor，recv_data 和 send_data 就是 Hander。