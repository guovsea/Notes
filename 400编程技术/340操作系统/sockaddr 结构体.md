`strcut sockaddr` 很多网络编程函数诞生早于 IPv4 协议，那时候都使用的是 `sockaddr` 结构体，为了向前兼容，现在 `sockaddr` 退化成了`void *`。传递一个指向地址结构体的指针，至于结构体是 `sockaddr_in` 还是 `sockaddr_in6`由地址族确定，再强制类型转化为所需的地址类型。

![[sockaddr结构体.png]]


**sockaddr**
```c
struct sockaddr {
	sa_family_t sa_family; 		/* address family, AF_xxx */
	char sa_data[14];			/* 14 bytes of protocol address */
};
```

**scokaddr_in**
```c
struct sockaddr_in {
	__kernel_sa_family_t sin_family; 			/* Address family */  	地址结构类型
	__be16 sin_port;					 		/* Port number */		端口号
	struct in_addr sin_addr;					/* Internet address */	IP地址
	/* Pad to size of `struct sockaddr'. */
	unsigned char __pad[__SOCK_SIZE__ - sizeof(short int) -
	sizeof(unsigned short int) - sizeof(struct in_addr)];
};

struct in_addr {						/* Internet address. */
	__be32 s_addr;
};
```

**sockaddr_un**
```c
#define UNIX_PATH_MAX 108

struct sockaddr_un {
  __kernel_sa_family_t sun_family; /* AF_UNIX */
  char sun_path[UNIX_PATH_MAX]; /* pathname */
};
```

**sockaddr_in6**
```c
struct sockaddr_in6 {
	unsigned short int sin6_family; 		/* AF_INET6 */
	__be16 sin6_port; 					/* Transport layer port # */
	__be32 sin6_flowinfo; 				/* IPv6 flow information */
	struct in6_addr sin6_addr;			/* IPv6 address */
	__u32 sin6_scope_id; 				/* scope id (new in RFC2553) */
};
struct in6_addr {
	union {
		__u8 u6_addr8[16];
		__be16 u6_addr16[8];
		__be32 u6_addr32[4];
	} in6_u;
	#define s6_addr 		in6_u.u6_addr8
	#define s6_addr16 		in6_u.u6_addr16
	#define s6_addr32	 	in6_u.u6_addr32
};

#define UNIX_PATH_MAX 108
	struct sockaddr_un {
	__kernel_sa_family_t sun_family; 	/* AF_UNIX */
	char sun_path[UNIX_PATH_MAX]; 	/* pathname */
};

```