## 文件数据结构

**task_struct -> files_struct -> file**

![[Linux 文件数据结构.png]]

**进程控制块 PCB**
```c
struct task_struct {
 ...
/* open file information */
 struct files_struct *files;
 ...
};
```

**files_struct** 

*位于用户空间中* fork 时会复制到子进程中，用来管理该进程**打开的所有文件**
```c
```text
/*
 * Open file table structure
 */
struct files_struct {
    // 读相关字段
    atomic_t count;
    bool resize_in_progress;
    wait_queue_head_t resize_wait;

    // 打开的文件管理结构
    struct fdtable __rcu *fdt; // 动态扩容的数组
    struct fdtable fdtab;

    // 写相关字段
    unsigned int next_fd;
    unsigned long close_on_exec_init[1];
    unsigned long open_fds_init[1];
    unsigned long full_fds_bits_init[1];
    struct file * fd_array[NR_OPEN_DEFAULT];  // 静态数组
};
```

- `files_struct` ：用于管理进程打开的 open 文件列表，内部以数组的方式实现（静态数组和动态数组结合）。返回给用户的 `fd` 就是这个数组的**编号索引**而已，索引元素为 `file` 结构；   
- `files_struct` 只从属于某进程；

数组：
1. `struct file * fd_array[NR_OPEN_DEFAULT]` 是一个静态数组，随着 `files_struct` 结构体分配出来的，在 64 位系统上，静态数组大小为 64；
2. `struct fdtable` 也是个数组管理结构，只不过这个是一个动态数组，数组边界是用字段描述的；

*为什么会有这种静态 + 动态的方式？*

大部分进程只会打开少量的文件，所以静态数组就够了，这样就不用另外分配内存。如果超过了静态数组的阈值，那么就动态扩展。


**struct file**

抽象的表述，但在不同的系统中具体实现有区别。

位于内核空间中，由操作系统进行管理，当且仅当使用 open 打开一个文件操作系统就会维护一个这种结构。

```c
struct file {
    // ...
    struct path                     f_path;
    struct inode                    *f_inode;     // 文件的inode
    const struct file_operations    *f_op;

    atomic_long_t                    f_count;
    unsigned int                     f_flags;
    fmode_t                          f_mode;
    struct mutex                     f_pos_lock;
    loff_t                           f_pos;     // 文件偏移量
    struct fown_struct               f_owner;
    // ...
}
```
捋一捋多个进程中的 `struct files_struct` 和 `struct file`

- **只有使用 open 打开一个文件时，操作系统才会生成一个`struct file`
- **fork 子进程会继承父进程的 `files_struct`。** 也就是说它们的文件描述符表中保存的`file`指针都指向同一个 `struct file`
- **`struct files_struct` 中保存着指向 `struct file`的指针**，也就是说每个 fd 都对应一个`struct file`

**因此**

- 不同的进程间不共享`struct file`（因为不同进程即使打开同一个文件也会调用两次 open）
- fork 并不会创建新的`struct file`。使用 open 打开文件后再 fork，那么父进程和子进程共享该文件的`struct file`
- 文件描述符表中的元素可以修改，不通的 fd 可以指向相同的`struct file`

*struct file 中保存着文件偏移量* **共享 struct file 则共享文件偏移量

## dup

- open 会创建一个`struct file`，保存这着件的所有信息
- fd 是指向`struct file`的指针`

```c
#include <unistd.h>
int dup(int oldfd);               // 将 oldfd 中的struct file*复制到返回值（返回最小可用fd）中
int dup2(int oldfd, int newfd);   // 将 oldfd 中的struct file* 复制到 newfd中
```
**dup 并不会复制`sturuct file`**

## 测试

1. 使用不同 open 打开同一个文件不共享`struct file`

![[不同进程不共享struct file.png]]
2. fork 并不会复制`struct file`
![[fork不复制file.png]]

3. dup 不复制文件`struct file`

![[dup不复制struct file.png]]
## inode

inode 和具体进程无关，是文件系统级别的资源；

inode 并没有直接指向具体文件系统的 inode ，而是操作系统抽象出来的一层虚拟文件系统，叫做 **VFS （ Virtual File System ）**，然后在 VFS 之下才是真正的文件系统，比如 ext4 之类的。

ext4 `inode` 结构体（指代具体文件系统 inode ）：后端文件系统的 `inode` 结构，不同文件系统自定义的结构体，ext2 有 `ext2_inode_info`，ext4 有`ext4_inode_info`，minix 有 `minix_inode_info`，这些结构里都是内嵌了一个 vfs `inode` 结构体，原理相同。

![[Linux 文件结构.png]]

**为什么会有这一层封装呢？**

解耦。如果让 `struct file` 直接和不通的文件系统对接，那么会导致 `struct file` 的处理逻辑非常复杂，因为每对接一个具体的文件系统，就要考虑一种实现。所以操作系统必须把底下文件系统屏蔽掉，对外提供统一的 `inode` 概念，对下定义好接口进行**回调注册**。这样让 `inode` 的概念得以统一，Unix 一切皆文件的基础就来源于此。

[[VFS]]

## 总结


- inode 这个具体进程无关，是文件系统级别的资源；

1. ext4 `inode` 结构体（指代具体文件系统 inode ）：后端文件系统的 `inode` 结构，不同文件系统自定义的结构体，ext2 有 `ext2_inode_info`，ext4 有`ext4_inode_info`，minix 有 `minix_inode_info`，这些结构里都是内嵌了一个 vfs `inode` 结构体，原理相同；  
    

**完整的架构图：**

![[Linux 文件数据完整架构图.png]]


## 拓展

**文件读写（ IO ）的时候会发生什么？**

- 在完成 write 操作后，在文件 `file` 中的当前文件偏移量会增加所写入的字节数，如果这导致当前文件偏移量超处了当前文件长度，则会把 inode 的当前长度设置为当前文件偏移量（也就是文件变长）
- `O_APPEND` 标志打开一个文件，则相应的标识会被设置到文件 `file` 状态的标识中，每次对这种具有追加写标识的文件执行 `write` 操作的时候，`file` 的当前文件偏移量首先会被设置成 `inode` 结构体中的文件长度，这就使得每次写入的数据都追加到文件的当前尾端处（该操作对用户态提供原子语义）；
- 若一个文件 `lseek` 定位到文件当前的尾端，则 `file` 中的当前文件偏移量设置成 `inode` 的当前文件长度；
- `lseek` 函数值修改 `file` 中的当前文件偏移量，不进行任何 `I/O` 操作；
- 每个进程对有它自己的 `file`，其中包含了当前文件偏移，当多个进程写同一个文件的时候，由于一个文件最终是落到全局唯一的一个 `inode` 上，这种并发场景则可能产生用户不可预期的结果；

**总结：**

**文件结构概述**
- 结构体 `task_struct` 对应一个抽象的**进程**
- `files_struct` 是进程管理它所打开的文件的管理器，本质上是一个数组，被叫做——**文件描述符表**。
- `fd` 则对应了这个数组的元素编号，可以通过 fd 引用 **`struct file` 结构体**。`struct file`内含当前偏移、`inode`等信息；

**struct file 和 inode**
3. `file` 结构体可以为进程间共享，属于系统级资源，同一个文件可能对应多个 `file` 结构体，`file` 内部有个 `inode` 指针，指向文件系统的 `inode`；
4. `inode` 是文件系统级别的概念，只由文件系统管理维护，不因进程改变（`file` 是进程出发创建的，进程 `open` 同一个文件会导致多个 `file` ，指向同一个 `inode` ）；