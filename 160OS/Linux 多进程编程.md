---
tags:
  - os
  - os/process
  - os/linux
  - type/howto
mastery: 5
---## 文件共享

子进程会复制父进程的文件描述符表和文件表项。[[Linux 文件数据结构]]

fork 之后处理文件描述符

1. 父进程等待子进程完成，父进程不操作文件描述符
2. 父进程和子进程各自执行不同的程序段。**在 fork 之后，父进程和子进程各自关闭它们不需使用的文件描述符。** *网络服务器经常使用这种处理方式*

子进程继承父进程如下主要属性：

1. **内存空间**
2. **文件描述符**
3. **用户和组标识**：子进程通常以相同的用户和组标识继承父进程。这意味着子进程通常以与父进程相同的用户权限运行。
4. **信号处理**：子进程会继承父进程的信号处理方式。这**包括已注册的信号处理程序和信号掩码**。
5. **进程组**：子进程通常被分配到与父进程相同的进程组。
6. **工作目录**：子进程会继承父进程的当前工作目录。可以随后更改子进程的工作目录。
7. **资源限制**：子进程会继承父进程的资源限制，例如 CPU 时间限制、内存限制等。

父进程和子进程的区别：

- fork 的返回值不同
- 进程的父进程 id 不同
- 进程的 tms_utime、tms_stime、tms_cutim、tmus_ustime 的值设置为 0 *记录了进程的用户态和内核态的 CPU 时间*
- 子进程不继承父进程设置的文件锁
- 子进程的未处理闹钟信号被清除
- 子进程的未处理信号集设置为空集

## 获取进程标识符函数

```c
#include <unistd.h>

pid_t getpid(void);

pid_t getppid(void); // 父进程 ID

uid_t getuid(void); // 实际用户 ID

uid_t geteuid(void); // 有效用户 ID

gid_t getgid(void); // 实际组 ID

gid_t getegid(void);
```

## fork

```c
#include <unistd.h>
pid_t fork(void); // 子进程中返回0，父进程中返回子进程的pid，失败返回-1
```

fork 后的子进程是父进程的副本。父进程和子进程不共享存储空间部分（栈、堆、数据段），子进程也不会直接复制，而是采用**写时复制 (Copy-On-Write, COW)** 技术。内核将这些部分的权限变为只读，当父进程或子进程试图修改时，只为修的那块内存制作一个副本，通常是虚拟内存中的一页（Page）。

**循环创建 N 个子进程

```c
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>

#define N 10
int main(void) {
	// 创建子进程
    int i;
	for (i = 0; i < N; i++) {
        pid = fork();
        if (pid == -1) {
            perror("fork");
        } else if (pid == 0) {
            break; // 子进程退出循环
        } else {
           ; // 父进程，do nothing
        }
	}
	// 最后的循环变量就相当于进程的编号，子进程编号为 0～N-1，父进程编号为 N
    if (i == N) { // 父进程
        ;
    } else {
        // 子进程
    }
}

```

## wait 和 waitpid

父进程调用 wait/waitpid 函数可以回收子进程终止信息。有三个功能：

- 阻塞等待子进程退出 
- 回收子进程残留资源 
- 获取子进程终止状态信息 (退出原因)。

**强调：wait 和 waitpid 都只能回收一个子进程，要回收多个子进程必须使用循环**

### wait 函数
```c
#include <sys/wait.h>
pid_t wait(int *status); 	

```
**参数**

- **status**，传出参数，返回子进程的终止状态信息
  位图，高 8 位保存 exit 退出码，低 8 位保存 signal 编号

| 宏                    | 说明                                                      |
| --------------------- | --------------------------------------------------------- |
| `WIFEXITED(status)`   | 进程正常结束时为真                                        |
| `WEXITSTUTAS(status)` | 当`WIFEXITED(status)`为真时调用，返回进程终止状态码       |
| `WIFSIGNALED(status)` | 当子进程异常结束时返回为真                                |
| `WTERMSIG(status)`    | 当`WIFSOGNALED(status)`为真时调用，返回引起终止的信号代码 |

上述宏是两两对应使用的，还有几个宏需要注意的：
  
- `WCOREDUMP(status)`：核心转储  
- `WIFSTOPPED(status)`: 当子进程收到暂时停止的信号时为真

用法示例：

```c
#include <sys/wait.h>
int status;
wait(&status);
if WIFEXITED(status) {
    // 进程正常结束
    pritnf("exited with status: %d\n", WEXITSTATUS(status));
}
```
**返回值**

- 成功等待到进程终止，返回终止子进程的进程 ID 
- -1， 没有子进程可等待，或者在等待过程中出现错误

### waitpid 函数

```c
#include <sys/wait>
pid_t waitpid(pid_t pid, int *status, int options);
```

- **pid**

| 参数值 | 说明                                                                                                  |
| ------ | ----------------------------------------------------------------------------------------------------- |
| pid<-1 | 等待进程**组号为 pid 绝对值**的任何子进程。                                                               |
| pid=-1 | 等待**任何子进程**，退化成了普通的 wait ()函数。                                   |
| pid=0  | 等待调用进程**所在进程组**的所有进程 |
| pid>0  | 等待**进程号**为 pid 的子进程                                                                           |

- **options：**
  - WNOHANG  
    没有子进程结束，立即返回
  - WUNTRACED  
    如果子进程由于被停止产生的SIGCHLD，waitpid则立即返回
  - WCONTINUED  
    如果子进程由于被 SIGCONT 唤醒而产生的 SIGCHLD，waitpid 则立即返回

**返回值**

- 成功等待到指定进程的终止，返回终止子进程的进程 ID。
- `pid` 参数指定的进程不存在或不是当前进程的子进程，返回 0。
- 指定的进程尚未终止，但设置了 `WNOHANG` 选项，返回 0。
- 出现错误，返回 -1。

**信号回收子进程**

```c
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>

void do_sigchild(int num) {
    while (waitpid(0, NULL, WNOHANG) > 0)  
		;
}

int main(void) {
	struct sigaction newact;
	newact.sa_handler = do_sigchild;
	sigemptyset(&newact.sa_mask);
	newact.sa_flags = 0;
	sigaction(SIGCHLD, &newact, NULL);

    fork();
    ; // ...
}

```
**剖析 SIGCHILD 的信号处理函数**
```c

void do_sigchild(int num) {
    while (waitpid(0, NULL, WNOHANG) > 0)  
		;
}
```
**while 循环会回收进程组中所有已经终止的子进程**。因为循环回收一个子进程会返回其进程 ID，所以循环会继续，继续试图回收下一个子进程，直到没有子进程能回收，返回 0。因为 `option=WNOHANG`。

因为[[信号 signal#信号捕捉特性]]，多个子进程终止后，可能只会调用一次信号处理函数，因此信号处理函数需要回收所有终止的子进程。
## exit

## exec

## 进程间通信

[[Linux 进程间通信]]