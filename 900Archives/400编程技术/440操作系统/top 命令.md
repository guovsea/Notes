
**信息说明**

```shell
top - 08:46:57 up 12:59,  3 users,  load average: 0.12, 0.17, 0.18
             系统运行时间              系统1分钟、5分钟、15分钟的负载情况  
Tasks: 200 total,   1 running, 199 sleeping,   0 stopped,   0 zombie

%Cpu(s):  0.4 us,  0.2 sy,  0.0 ni, 99.4 id,  0.0 wa,  0.0 hi,  0.0 si,  0.0 st
           用户态   内核态  改优先级进程   空闲      等待    硬中断   软中断  Steal Time
MiB Mem :    933.3 total,    183.7 free,    226.6 used,    523.0 buff/cache
MiB Swap:   1924.0 total,   1529.2 free,    394.8 used.    546.9 avail Mem

PID USER      PR  NI    VIRT    RES    SHR S  %CPU  %MEM     TIME+ COMMAND
           
```

1. `0.4 us`：表示用户空间（User Space）中的 CPU 使用率。
2. `0.2 sy`：表示系统空间（System Space）中的 CPU 使用率。
3. `0.0 ni`：改变过优先级的进程占用CPU的百分比
4. `99.4 id`：PU 空闲的时间百分比
5. `0.0 wa`：CPU 等待（Wait）的时间百分比。这表示 CPU 正在等待某些输入/输出操作完成，这可能是磁盘、网络或其他外部设备操作。
6. `0.0 hi`：硬件中断（Hardware Interrupts）的 CPU 使用率。
7. `0.0 si`：软件中断（Software Interrupts）的 CPU 使用率。
8. `0.0 st`：表示 Steal Time，通常在虚拟化环境中出现，表示其他虚拟机或容器抢占了当前虚拟机的 CPU 时间的百分比。在非虚拟化环境中，通常不会有 Steal Time。


- PID — 进程 id
- USER — 进程所有者
- PR — 进程优先级
- NI — nice 值。负值表示高优先级，正值表示低优先级
- VIRT — 进程使用的虚拟内存总量，单位 kb。VIRT=SWAP+RES- RES — 进程使用的、未被换出的物理内存大小，单位 kb。RES=CODE+DATA
- SHR — 共享内存大小，单位 kb位
- S — 进程状态。D=不可中断的睡眠状态 R=运行 S=睡眠 T=跟踪/停止 Z=僵- %CPU — 上次更新到现在的 CPU 时间占用百分比用百分比
- %MEM — 进程使用的物理
- TIME+ — 进程使用的 CPU 时间总计，单位1/100秒
- COMMAN\D — 进程名称（命令名/命令行）


**命令**


```shell
top -p 484   # 显示特定进程的信息
top -n 5     # n 设置更新次数，更新 n 次后停止
top -d 5     # 设置更新周期，单位 秒

```

**交互操作**

改变内存显示单位

E/e 改变控制面板/进程信息的内存显示单位


切换排序列


- 按下 "P" 键：按 CPU 使用率（%CPU）排序。
- 按下 "M" 键：按内存使用率（%MEM）排序。
- 按下 "N" 键：按 PID（进程标识符）排序。
- 按下 "T" 键：按运行时间（TIME+）排序。
- ”shift + >”或”shift + <”向右或左改变排序列

高亮排序列 x

显示完整命令 c


设置更新选项

- n 设置更新次数，更新 n 次后停止
- d 设置更新周期，单位 秒


h 显示帮助画面，给出一些简短的命令总结说明

k 终止一个进程。

i 忽略闲置和僵死进程。这是一个开关式命令。

q 退出程序

r 重新安排一个进程的优先级别

S 切换到累计模式

s 改变两次刷新之间的延迟时间（单位为s），如果有小数，就换算成m s。输入0值则系统将不断刷新，默认值是5 s

f或者F 从当前显示中添加或者删除项目

o或者O 改变显示项目的顺序

l 切换显示平均负载和启动时间信息

m 切换显示内存信息

t 切换显示进程和CPU状态信息

c 切换显示命令名称和完整命令行

M 根据驻留内存大小进行排序

P 根据CPU使用百分比大小进行排序

T 根据时间/累计时间进行排序




