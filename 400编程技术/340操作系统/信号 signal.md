## 信号机制

**信号是异步的通信方式**，因为应用程序虽然需要信号，但是可以继续向下执行。收到信号之前执行自己的代码，收到信号后，不管执行到程序的什么位置，都要暂停运行去处理信号，处理完毕再继续执行。

**同步**：阻塞和非阻塞，这两种方式都是属于应用程序主动去查看驱动层资源。

- 阻塞方式：进程睡眠等待资源有效；
- 非阻塞方式：进程不断轮询的查看资源是否有效。

**异步**：可以去做别的事情 (不用睡眠，也不用轮询)。

### 产生-未决-递达

信号产生后，操作系统并不会直接将信号递送给某进程（信号通常指定接收者），操作系统会先检查信号是否被屏蔽，如果被屏蔽，则信号不会递达，直到进程解除对该信号的屏蔽。当信号递达后会根据信号的处理方式来进行处理。

**产生信号**

1. 按键产生，如：Ctrl+c、Ctrl+z、Ctrl+\
2. 系统调用产生，如：kill、raise、abort
3. 软件条件产生，如：定时器 alarm 
4. 硬件异常产生，如：非法访问内存 (段错误)、除 0 (浮点数例外)、内存对齐出错 (总线错误)
5. 命令产生，如：kill 命令

**未决**

从产生到递达之间的状态。主要由于阻塞 (屏蔽)导致该状态。*POSIX 标准保证信号在未决状态时不会丢失，只要后面取消屏蔽信号就一定会递达*

**递达**

信号到达进程。

**查看信号**

`kill -l` 命令，查看所有的信号

```shell
guo@ubuntu:~/lean/IPC$ kill -l
```

**硬件来源**信号

- eg：`Ctrl+C` 产生 `SIGINT` 信号，表示终止该进程；
- eg：`Ctrl+Z` 产生 `SIGTSTP` 信号，表示停止该进程，但还未结束；

**软件来源**信号，

- eg：`kill -9 PID`

### 信号处理方式

1. **执行默认操作。** Linux  对每种信号都规定了默认操作，例如，上面列表中的 SIGTERM 信号，会终止进程
2. **捕捉信号。** 我们可以为信号定义一个信号处理函数。当信号发生时，我们就执行相应的信号处理函数。
3. **忽略信号。** **注意** `SIGKILL` 和 `SEGSTOP`，它们用于在任何时候中断或结束某一进程

**阻塞信号集 (信号掩码 mask)**： 信号被屏蔽，在这个集合中的信号将处于**未决**状态

**未决信号集**:

1. 信号产生，未决信号集中描述该信号的位立刻翻转为 1，表信号处于未决状态。当信号被处理对应位翻转回为 0
2. 信号产生后由于某些原因 (主要是阻塞)不能抵达。这类信号的集合称之为未决信号集。**在屏蔽解除前，信号一直处于未决状态**

### 信号四要素

1. 编号（值） 
2. 名称 
3. 事件 
4. 默认处理动作 

默认动作：

- Term：终止进程
- Ign： 忽略信号 (默认即时对该种信号忽略操作)
- Core：终止进程，生成 Core 文件。(查验进程死亡原因， 用于 gdb 调试)
- Stop：暂停进程 *SIGSTOP，WIFSTOllPPED 中 stop 的含义都是暂停*
- Cont：继续运行进程

### 信号捕捉特性


## 信号的产生

### kill 函数/命令产生信号

kill 函数：给指定进程发送指定信号 (不一定杀死)

```c
int kill(pid_t pid, int sig); //
```
**返回值** 

- 成功：0；
- 失败：-1 (ID 非法，信号非法，普通用户杀 init 进程等权级问题)，设置 errno

**参数**

pid

- pid > 0：发送信号给指定的进程。
- pid = 0：发送信号给 与调用 kill 函数进程属于同一进程组的所有进程。
- pid < 0：取|pid|发给对应进程组。
- pid = -1：发送给进程有权限发送的系统中所有进程。

sig：不推荐直接使用数字，应使用宏名，因为不同操作系统信号编号可能不同，但名称一致。

### alarm 函数

设置定时器 (闹钟)。在指定 seconds 后，内核会给当前进程发送 14）SIGALRM 信号。进程收到该信号，默认动作终止。

```c
#include <unistd.h>
unsigned int alarm(unsigned int seconds); 返回0或剩余的秒数，无失败。
```

**每个进程有且只有一个 alarm**

### setitimer 函数

设置定时器 (闹钟)。 可代替 alarm 函数。精度微秒 us，可以实现周期定时。

```c
#include <unistd.h>
int setitimer(int which, const struct itimerval *new_value, struct itimerval *old_value);
```
**返回值**

- 成功：0；
- 失败：-1，设置 errno260 数据库

**参数**

- which：指定定时方式
  - ITIMER_REAL *真实时间*
  - ITIMER_VIRTUAL *虚拟空间计时 (用户空间)*
  - ITIMER_PROF *运行时计时 (用户+内核)*

## 信号处理方式

### 操作信号掩码

**信号集合**

```c
#include <signal.h>
// 操作信号集合
sigset_t  set;			// typedef unsigned long sigset_t; 
int sigemptyset(sigset_t *set);
int sigfillset(sigset_t *set);
int sigaddset(sigset_t *set, int signum);
int sigdelset(sigset_t *set, int signum);
// 成功返回 0，失败返回-1
```
```c
#include <signal.h>
// 判断某个信号是否在信号集中	 返回值：在集合：1；不在：0；出错：-1  
int sigismember(const sigset_t *set, int signum);
// 返回值：在集合：1；不在：0；出错：-1  
```

**sigprocmask 函数**

用来操作信号掩码

**注意：屏蔽信号只是将信号处理延后执行 (延至解除屏蔽)，而忽略表示将信号丢处理。**

```c
#include <signal.h>
int sigprocmask(int how, const sigset_t *set, sigset_t *oldset);
```
**参数**

- how 参数取值：
  - SIG_BLOCK       mask & set，添加 set 集合的屏蔽
  - SIG_UNBLOCK     mask & ～set，解除 set 集合的屏蔽
  - SIG_SETMASK     mask = set，设置 set 集合的屏蔽
- set：传入参数，是一个位图，set 中哪位置 1，就表示当前进程屏蔽哪个信号。
- oldset：传出参数，保存旧的信号屏蔽集。

**返回值**

- 成功：0；
- 失败：-1，设置 errno

**sigpending 函数**

读取当前进程的未决信号集
```c
#include <signal.h>
int sigpending(sigset_t *set);	  // set传出参数。   返回值：成功：0；失败：-1，设置errno
```

### 信号捕捉

**signal 函数**

```c
typedef void (*sighandler_t)(int);
sighandler_t signal(int signum, sighandler_t handler);
```
*该函数由 ANSI 定义，由于历史原因在不同版本的 Unix 和不同版本的 Linux 中可能有不同的行为。因此应该尽量避免使用它，取而代之使用 sigaction 函数*

**sigaction 函数**	
```c
#include <signal.h>
int sigaction(int signum, const struct sigaction *act, struct sigaction *oldact);
struct sigaction {
        void     (*sa_handler)(int);
        // 指定信号捕捉后的注册函数。也可赋值为 SIG_IGN 表忽略 或 SIG_DFL 表执行默认动作
        void     (*sa_sigaction)(int, siginfo_t *, void *);
        // 当sa_flags被指定为SAkk_SIGINFO标志时，使用该信号处理程序。(很少使用)
        sigset_t   sa_mask; 
        // **调用信号处理函数期间** ，所要屏蔽的信号集合 (信号屏蔽字)
        int       sa_flags;
        // 通常设置为 0，表使用默认kkestorer)(void);  
        // 该元素是过时的，不应该使用，POSIX.1标准将不指定该元素。(弃用)
}
```
**参数**

- signum 信号编号，使用信号宏
- act：传入参数，新的处理方式。
  **SIG_IGN** - 忽略动作
  **SIG_DFL** - 默认动作
- oldact：传出参数，旧的处理方式。

### 信号捕捉特性

1. 进程正常运行时，默认 PCB 中有一个信号屏蔽字。信号处理函数执行期间的信号屏蔽，由 sa_mask 来决定，处理完成后再由 PCB 中的信号屏蔽字决定。
2. 某个信号的捕捉函数执行期间该信号自动被屏蔽。
3. 阻塞的常规信号不支持排队，产生多次只记录一次。（后32个实时信号支持排队）
## 信号使用场景

### 借助 SIGCHLD 信号回收子进程

SIGCHLD 的产生

**子进程的运行状态发生改变时**（终止、暂停、暂停后被唤醒）

- 子进程终止时
- 子进程接收到 SIGSTOP 信号停止时
- 子进程处在停止态，接受到 SIGCONT 后唤醒时

子进程结束运行，其父进程会收到 SIGCHLD 信号。该信号的默认处理动作是忽略。可以捕捉该信号，在捕捉函数中完成子进程状态的回收。

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

void sys_err(char *str)
{
    perror(str);
    exit(1);
}
void do_sig_child(int signo)
{
    int status;    
    pid_t pid;
    while ((pid = waitpid(0, &status, WNOHANG)) > 0) {
        if (WIFEXITED(status))
            printf("child %d exit %d\n", pid, WEXITSTATUS(status));
        else if (WIFSIGNALED(status))
            printf("child %d cancel signal %d\n", pid, WTERMSIG(status));  
    }
}
int main(void)
{
    pid_t pid;    int i;
    // 循环创建多个子进程
    for (i = 0; i < 10; i++) {
        if ((pid = fork()) == 0)
            break;
        else if (pid < 0)
            sys_err("fork");
    }
    if (pid == 0) {    
        int n = 1;
        while (n--) {
            printf("child ID %d\n", getpid());
            sleep(1);
        }
        return i + 1;
    } else if (pid > 0) {
        // 信号处理
        struct sigaction act;
        act.sa_handler = do_sig_child;
        sigemptyset(&act.sa_mask);
        act.sa_flags = 0;
        sigaction(SIGCHLD, &act, NULL);
        
        while (1) {
            printf("Parent ID %d\n", getpid());
            sleep(1);
        }
    }
    return 0;
}
```

**注意**

1.	子进程继承父进程的信号屏蔽字和信号处理动作，但子进程没有继承未决信号集 sigpending。
2.	注意注册信号捕捉函数的位置。
3.	应该在 fork 之前，阻塞 SIGCHLD 信号。注册完捕捉函数后解除阻塞。

**多线程中信号编写范式：不用信号,,,😂**