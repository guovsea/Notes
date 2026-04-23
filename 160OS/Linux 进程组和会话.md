---
tags:
  - os/process
  - os/linux
  - type/concept
mastery: 5
---
## 进程组（Process Group）

进程组是一个或多个进程的集合，这些进程共享同一个**进程组 ID（PGID）**。进程组 ID 等于该组中**组长进程（Group Leader）的 PID**。

- 组长进程可以创建一个进程组，创建该组中的进程，然后终止。**进程组的存在与组长进程是否终止无关**，只要组内还有进程存在，进程组就存在。
- 一个进程组内的所有进程可以来自同一个会话，也可以来自不同会话。
- 进程组的生命周期：从组长进程创建进程组开始，到组内最后一个进程终止或转移到其他进程组为止。

```c
#include <unistd.h>
pid_t getpgrp(void);           // 获取当前进程的进程组 ID
pid_t getpgid(pid_t pid);      // 获取指定进程的进程组 ID
int setpgid(pid_t pid, pid_t pgid); // 设置指定进程的进程组 ID
```

## 会话（Session）

会话是一个或多个进程组的集合。一个会话中的进程不能跨会话组成进程组。

- **会话首进程（Session Leader）**：创建会话的进程，其 PID 即为**会话 ID（SID）**。
- 一个会话可以有一个控制终端（Controlling Terminal），这通常是在会话首进程打开终端设备时建立的。
- **前台进程组（Foreground Process Group）**：与控制终端交互的进程组，同一时刻只能有一个前台进程组。
- **后台进程组（Background Process Group）**：会话中的其他进程组。

```c
#include <unistd.h>
pid_t getsid(pid_t pid);       // 获取会话 ID
pid_t setsid(void);            // 创建新会话（调用进程不能是进程组组长）
```

**setsid 的作用**：
1. 创建一个新会话，调用进程成为会话首进程
2. 创建一个新进程组，调用进程成为组长进程
3. 断开与控制终端的连接

这是守护进程（Daemon）创建过程中的关键步骤。

## 相关

- [[Linux 多进程编程]]
- [[进程]]
