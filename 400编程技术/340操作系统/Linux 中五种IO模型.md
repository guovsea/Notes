
网络 IO 的本质是 socket 的读取，socket 在 linux 系统被抽象为流，IO 可以理解为对流的操作。在 Linux 中，对于一次 IO 访问（以 read 举例），数据会先被拷贝到操作系统内核的缓冲区中，然后才会从操作系统内核的缓冲区拷贝到应用程序的地址空间。所以说，当一个 read 操作发生时，它会经历两个阶段：

1. **第一阶段：等待数据准备 (Waiting for the data to be read)**
2. **第二阶段：将数据从内核拷贝到进程中 (Copying the data from the kernel to the process)**

对于socket流而言，

1. 第一步：通常涉及等待网络上的数据分组到达，然后被复制到内核的某个缓冲区。
2. 第二步：把数据从内核缓冲区复制到应用进程缓冲区。

网络应用需要处理的无非就是两大类问题，**网络 IO，数据计算**。相对于后者，网络 IO 的延迟，给应用带来的性能瓶颈大于后者。**网络 IO 的模型大致有如下几种**：

**同步模型**：将数据从内核拷贝到进程中这个阶段，进程会被阻塞

- **阻塞 IO（bloking IO）**
- **非阻塞 IO（non-blocking IO）**
- **多路复用 IO（multiplexing IO）**
- **信号驱动式 IO（signal-driven IO）**

**异步模型**：将数据从内核拷贝到进程中这个阶段，进程不会被阻塞。拷贝完成后才会使用信号通知进程。

- **异步IO（asynchronous IO）**

synchronous IO 和 asynchronous IO 的定义：

>A synchronous I/O operation causes the requesting process to be blocked until that I/O operation completes;
>
>An asynchronous I/O operation does not cause the requesting process to be blocked;

定义中所指的 "IO operation" 是指真实的 IO 操作。
  
## 阻塞 IO

同步阻塞 IO 模型是最常用的一个模型，也是最简单的模型。在 linux 中，默认情况下所有的 socket 和文件描述符都是 blocking 。**阻塞就是进程 "被" 休息, CPU 处理其它进程去了**。

**在阻塞 IO 模型中，用户空间的应用程序执行一个系统调用（recvfrom），系统将会从用户态陷入到内核态，内核将数据准备好后，再将数据从内核复制到进程中，整个过程进程都将被阻塞，数据复制完成后进程才会被唤醒**。

流程如图所示：

![[Linux 中五种IO模型_阻塞IO.png]]
  
所以，**blocking IO的特点就是在IO执行的两个阶段都被block了**。

## 非阻塞 IO（nonblocking IO）

**非阻塞 IO 就是 “每个一段时间检查数据是否准备好” 的轮询（polling）方式**。在这种模型中，**设备是以非阻塞的形式打开的**。这意味着 IO 操作不会立即完成，read 操作会返回-1，并设置 errno，说明这个系统调用尚未完成（EAGAIN 或 EWOULDBLOCK）。

也就是说非阻塞的 recvfrom 在进程调用之后，进程并没有被阻塞，内核会马上将结果返回给进程。进程在返回之后，可以干点别的事情，然后再发起 recvfrom 系统调用。重复上面的过程，循环往复的进行检查，这个过程通常被称之为轮询。在某次调用 recvfrom 时，数据准备好了，操作系统就会将内核中准备好的数据拷贝到进程，进行数据处理。**需要注意，拷贝数据的这个过程中，进程仍然是属于阻塞的状态**。

流程如图所示：

![[Linux 中五种IO模型_non-blocking IO.png]]

*非阻塞指的是：数据准备阶段不会被阻塞*

##  IO 多路复用（ IO multiplexing）


阻塞和非阻塞 IO 模型中，进程都只能处理一个 IO 流。如果一个进程有同时有多个 IO 流，便**可以循环检查这些 IO 流中的数据是否准备好了**。如果这个任务由用户完成的话，每次检查都需要进行一次调用，这是非常巨大的消耗。因为每次系统调用都要在用户态和内核态之间进行两次切换。

而 **IO 多路复用便是让操作系统完成循环检查多个 IO 流 (socket) 的任务**，这样只需要一次系统调用的开销。UNIX/Linux 下的 select、poll、epoll 就是干这个的（epoll 比 poll、select 效率高，但做的事情是一样的）。

**当用户调用 IO 多路复用中 select/poll/epoll 系统调用时，进程会被阻塞**。阻塞过程中，IO 多路复用能实现同时对多个 socket 进行监听，当其中任何一个 socket 的数据准好了，便能返回某种数据结构引用这些 socket。然后进程再进行 recvform 系统调用，将数据由内核拷贝到用户进程，这个过程当然也阻塞的。

流程如下：

![[Linux 中五种IO模型_IO mutiplexing.png]]

对于单个 IO 事件而言，IO mutiplexing 比 blocking IO 的性能还更差一些。因为 IO mutiplexing 需要使用两个 syslltem call (select 和 recvfrom)，而 blocking IO 只调用了一个 system call (recvfrom)。但是，用 select 的优势在于它可以同时处理多个 connection。

所以，如果处理的连接数不是很高的话，使用 select/epoll 的 web server 不一定比使用 multi-threading + blocking IO 的 web server 性能更好，可能延迟还更大。（**select/epoll 的优势并不是对于单个连接能处理得更快，而是在于能处理更多的连接。**）

在 IO multiplexing Model 中，对于每一个 socket，一般都设置成为 non-blocking，但是，如上图所示，整个用户的 process 其实是一直被 block 的。只不过 process 是被 select 这个函数 block，而不是被 socket IO 给 block。所以**IO 多路复用是阻塞在 select，epoll 这样的系统调用之上，而没有阻塞在真正的 I/O 系统调用如 recvfrom 之上**。

与传统的多线程/多进程模型比，I/O多路复用的最大优势是系统开销小，系统不需要创建新的额外进程或者线程，也不需要维护这些进程和线程的运行，降底了系统的维护工作量，节省了系统资源，I/O多路复用的主要应用场景如下：

> 服务器需要同时处理多个处于监听状态或者多个连接状态的套接字。
> 
> 服务器需要同时处理多种网络协议的套接字。

## 信号驱动IO（signal-driven IO）

注册一个 SIGIO 信号的捕捉函数，进程继续运行并不阻塞。当数据准备好时，进程会收到一个 SIGIO 信号，可以在信号处理函数中调用 I/O 操作函数处理数据。过程如下图所示：

![[Linux 中五种IO模型_signal-driven IO.png]]

高并发的程序一般使用**同步非阻塞方式**而非多线程 + 同步阻塞方式。在这种高并发的情况下，为每个任务（用户请求）创建一个进程或线程的开销非常大。同步非阻塞方式可以由一个线程检测多个 IO 流，**多个 IO 流会在不同时刻准备好数据**，**因此进程可以分时处理大量 IO 流**。

这和 CPU 分时处理有点类似：不可能为每个 IO 流 (任务)都分配一个线程（CPU），而是处理一会儿这个 IO 流，再去处理一会儿哪个 IO 流，如此轮转。
## 异步 IO （Asynchronous IO）

同步 IO 的各种 模型中，**它们在等待数据到来的时候，处理的方式不一样：直接等待，轮询，使用 IO mutiplexing 轮询，但将数据从内核拷贝到进程中都会阻塞等待，因此是同步 IO**。

也就是说，同步模型中：

> 第一个阶段有的阻塞, 有的不阻塞
> 
> 第二个阶段都是阻塞的。

**从整个IO过程来看，他们都是顺序执行的，因此可以归为同步模型(synchronous)。都是进程主动等待且向内核检查状态。**


**相对于同步 IO，异步 IO 不是顺序执行。用户进程进行 aio_read 系统调用之后，无论内核数据是否准备好，都会直接返回给用户进程，然后用户态进程可以去做别的事情**。等到 socket 数据准备好了，内核直接复制数据给进程，然后从内核向进程发送通知`。`**IO 两个阶段，进程都是非阻塞的**。

异步过程如下图所示：

![[Linux 中五种IO模型_Asio.png]]

**异步 IO 流程**

用户进程发起 aio_read 操作之后，立刻就可以开始去做其它的事。而另一方面，从 kernel 的角度，当它受到一个 asynchronous read 之后，首先它会立刻返回，所以不会对用户进程产生任何 block。然后，kernel 会等待数据准备完成，然后将数据拷贝到用户内存，当这一切都完成之后，kernel 会给用户进程发送一个 signal 或执行一个基于线程的回调函数来完成这次 IO 处理过程，告诉它 read 操作完成了。

**在 Linux 中，通知的方式是信号（通常是`SIGIO`或`SIGEV`），当收到信号时**

- 如果这个进程正在用户态忙着做别的事，那就强行打断之，调用事先注册的信号处理函数`，这个函数可以决定何时以及如何处理这个异步任务。由于信号处理函数是突然闯进来的，因此跟中断处理程序一样，有很多事情是不能做的，因此保险起见，`一般是把事件 “登记” 一下放进队列，然后返回该进程原来在做的事`。
- 如果这个进程正在内核态忙着做别的事，例如以同步阻塞方式读写磁盘，那就只好把这个通知挂起来了，等到内核态的事情忙完了，快要回到用户态的时候，再触发信号通知`。
- 如果这个进程现在被挂起了，例如 sleep 了，那就把这个进程唤醒，下次有 CPU 空闲的时候，就会调度到这个进程，触发信号通知。

  
## 总结

![[Linux 中五种IO模型总结.png]]

[聊聊Linux 五种IO模型 - 简书](https://www.jianshu.com/p/486b0965c296)