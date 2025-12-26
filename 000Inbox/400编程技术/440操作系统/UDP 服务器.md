## UDP 服务器和 TCP 服务器的对比

相较于 TCP 而言，UDP 通信的形式更像是发短信。

- **优点：** 不需要在数据传输之前建立、维护连接。只专心获取数据就好。省去了三次握手的过程，通信速度可以大大提高。**因此**，UDP 开销较小，数据传输速度快，实时性较强。多用于对实时性要求较高的通信场合，如视频会议、电话会议等。
- **缺点：** 通信的稳定性和正确率便得不到保证。我们称 UDP 为“无连接的不可靠报文传递”。

通常情况下，使用 UDP 协议进行数据传输，为保证数据的正确性，我们需要在应用层添加辅助校验协议来弥补 UDP 的不足，以达到数据可靠传输的目的。

与TCP类似的，UDP也有可能出现缓冲区被填满后，再接收数据时丢包的现象。由于它没有TCP滑动窗口的机制，通常采用如下两种方法解决：

1. 服务器应用层设计流量控制，控制发送数据速度。
2. 借助 setsockopt 函数改变接收缓冲区大小
   ```c
#include <sys/socket.h>
int setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen);
	int n = 220x1024
	setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &n, sizeof(n));

```
### UDP 服务器天然拥有并发性

UDP 协议是无连接的，一个 socket 可以接收来自多个客户端的 Dgram，服务器只需要操作这一个 socket ，便能和多个客户端进行交互。因此 UDP 服务器天生就是并发的（支持同时和多个客户端进行交互）。

## UDP 状态图


![[基础 UDP 服务器.png]]

```cpp
#include <string.h>
#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>
#include <strings.h>
#include <arpa/inet.h>
#include <ctype.h>

#define MAXLINE 80
#define SERV_PORT 6666

int main(void)
{
	struct sockaddr_in servaddr, cliaddr;
	socklen_t cliaddr_len;
	int sockfd;
	char buf[MAXLINE];
	char str[INET_ADDRSTRLEN];
	int i, n;

	sockfd = socket(AF_INET, SOCK_DGRAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERV_PORT);

	bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
	printf("Accepting connections ...\n");

	while (1) {
		cliaddr_len = sizeof(cliaddr);
		n = recvfrom(sockfd, buf, MAXLINE,0, (struct sockaddr *)&cliaddr, &cliaddr_len);
		if (n == -1)
			perror("recvfrom error");
		printf("received from %s at PORT %d\n", 
				inet_ntop(AF_INET, &cliaddr.sin_addr, str, sizeof(str)),
				ntohs(cliaddr.sin_port));
		for (i = 0; i < n; i++)
			buf[i] = toupper(buf[i]);

		n = sendto(sockfd, buf, n, 0, (struct sockaddr *)&cliaddr, sizeof(cliaddr));
		if (n == -1)
			perror("sendto error");
	}
	close(sockfd);
	return 0;
}

```