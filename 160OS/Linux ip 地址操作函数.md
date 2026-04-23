---
tags:
  - os
  - os/network
  - type/reference
mastery: 5
---## inet_pton/inet_ntop



### inet_pton

*inet_pton 转换后的直接是网络字节序的 addr. sin_addr *
```c
#include <arpa/inet.h>
int inet_pton(int af, const char *src, void *dst);
```
**参数**

- af address family
  - AF_INET
  - AF_INET6
- src ip 字符串的地址
- dst 存放转换后的 ip 序列的地址，应该为 `struct sin_addr &`类型

**返回值**

- 1 成功
- 0 dst 的大小无法容纳转换后的 ip 序列
- -1 ip 地址形式非法，设置 errno 为 EAFNOSUPPORT

### inet_ntop
```c
const char *inet_ntop(int af, const void *src, char *dst, socklen_t size);
```
- af address family
- src **`sin_addr`结构体的地址，应该为 `struct sin_addr &`类型** *应该为 sockaddr_in 的 sin_addr 成员*
- dst 存放 ip 字符串的字符数组
- size `sizeof(dst)`

**返回值**

- NULL 失败
- 返回存放 ip 字符串数组的地址 

**本地字节序转网络字节序**

- hotnl

### 演示

```c
inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr);

char str[INET_ADDRSTRLEN];          /* #define INET_ADDRSTRLEN 16 */
printf("received from %s at PORT %d\n",
						inet_ntop(AF_INET, &cliaddr.sin_addr, str, sizeof(str)),
						ntohs(cliaddr.sin_port));

```
### inet_addr

```c
in_addr_t inet_addr(const char *cp); 
```
**返回值**

网络字节序的 ip 地址