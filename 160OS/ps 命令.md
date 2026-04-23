---
tags:
  - os
  - os/linux
  - os/process
  - type/reference
mastery: 5
---ps

打印当前 bash 的所有进程
```shell
guo@ubuntu:~$ ps
    PID TTY          TIME CMD
  43139 pts/2    00:00:00 bash
  45148 pts/2    00:00:00 ps
```

-f

显示当前系统完整格式的输出

```shell
guo@ubuntu:~$ ps -f
UID          PID    PPID  C STIME TTY          TIME CMD
guo        43139   17110  0 07:04 pts/2    00:00:00 -bash
guo        45177   43139  0 07:56 pts/2    00:00:00 ps -f
```
-e

等价于 -A，显示所有用户的所有进程
```shell
guo@ubuntu:~$ ps -e | head -n 10
    PID TTY          TIME CMD
      1 ?        00:00:06 systemd
      2 ?        00:00:00 kthreadd
      3 ?        00:00:00 rcu_gp
      4 ?        00:00:00 rcu_par_gp
      6 ?        00:00:00 kworker/0:0H-kblockd
      8 ?        00:00:00 mm_percpu_wq
      9 ?        00:00:05 ksoftirqd/0
     10 ?        00:00:12 rcu_sched
     11 ?        00:00:00 migration/0
```
**-ef**
```shell
guo@ubuntu:~$ ps -ef | head -n 10
UID          PID    PPID  C STIME TTY          TIME CMD
root           1       0  0 Nov07 ?        00:00:06 /sbin/init auto automatic-ubiquity noprompt
root           2       0  0 Nov07 ?        00:00:00 [kthreadd]
root           3       2  0 Nov07 ?        00:00:00 [rcu_gp]
root           4       2  0 Nov07 ?        00:00:00 [rcu_par_gp]
root           6       2  0 Nov07 ?        00:00:00 [kworker/0:0H-kblockd]
root           8       2  0 Nov07 ?        00:00:00 [mm_percpu_wq]
root           9       2  0 Nov07 ?        00:00:05 [ksoftirqd/0]
root          10       2  0 Nov07 ?        00:00:12 [rcu_sched]
root          11       2  0 Nov07 ?        00:00:00 [migration/0]
```
**-aux**

```shell
guo@ubuntu:~$ ps aux | head -n 10
USER         PID %CPU %MEM    VSZ   RSS TTY      STAT START   TIME COMMAND
root           1  0.0  0.8 170236  7756 ?        Ss   Nov07   0:06 /sbin/init auto automatic-ubiquity noprompt
root           2  0.0  0.0      0     0 ?        S    Nov07   0:00 [kthreadd]
root           3  0.0  0.0      0     0 ?        I<   Nov07   0:00 [rcu_gp]
root           4  0.0  0.0      0     0 ?        I<   Nov07   0:00 [rcu_par_gp]
root           6  0.0  0.0      0     0 ?        I<   Nov07   0:00 [kworker/0:0H-kblockd]
root           8  0.0  0.0      0     0 ?        I<   Nov07   0:00 [mm_percpu_wq]
root           9  0.0  0.0      0     0 ?        S    Nov07   0:05 [ksoftirqd/0]
root          10  0.0  0.0      0     0 ?        I    Nov07   0:12 [rcu_sched]
root          11  0.0  0.0      0     0 ?        S    Nov07   0:00 [migration/0]
```
VSZ (Virtual Set Size) 占用虚拟内存大小，单位 KB 

RSS (Resident Set Size) 占用物理内存大小，单位 KB 

-a == ps

-u 以 BSD 格式打印

**-al**

```shell
guo@ubuntu:~/lean/binutils/gdb$ ps -al
F S   UID     PID    PPID  C PRI  NI ADDR SZ WCHAN  TTY          TIME CMD
0 S  1000   65535   65519  0  80   0 -  1784 poll_s pts/1    00:00:00 tmux: client
0 S  1000   66763   17261  0  80   0 -  4758 futex_ pts/0    00:00:00 a.out
0 R  1000   66836   66766  0  80   0 -  2633 -      pts/4    00:00:00 ps
```
**-aL**

```shell
guo@ubuntu:~/lean/binutils/gdb$ ps -aL
    PID     LWP TTY          TIME CMD
  65535   65535 pts/1    00:00:00 tmux: client
  66763   66763 pts/0    00:00:00 a.out
  66763   66764 pts/0    00:00:00 a.out
  66763   66765 pts/0    00:00:00 a.out
  66857   66857 pts/4    00:00:00 ps
```

**--sort**

```shell
guo@ubuntu:~$ ps -aux --sort -vsz  | head -n 5
USER         PID %CPU %MEM    VSZ   RSS TTY      STAT START   TIME COMMAND
mysql        949  0.7  1.4 1349256 13412 ?       Ssl  Nov07   5:39 /usr/sbin/mysqld
root         866  0.0  2.0 1245704 19564 ?       Ssl  Nov07   0:14 /usr/lib/snapd/snapd
root         707  0.0  1.8 411432 18136 ?        SLsl Nov07   0:17 /sbin/multipathd -d -s
root         870  0.0  0.5 393288  5472 ?        Ssl  Nov07   0:00 /usr/lib/udisks2/udisksd

guo@ubuntu:~$ ps -aux --sort +vsz | head -n 5
USER         PID %CPU %MEM    VSZ   RSS TTY      STAT START   TIME COMMAND
root           2  0.0  0.0      0     0 ?        S    Nov07   0:00 [kthreadd]
root           3  0.0  0.0      0     0 ?        I<   Nov07   0:00 [rcu_gp]
root           4  0.0  0.0      0     0 ?        I<   Nov07   0:00 [rcu_par_gp]
root           6  0.0  0.0      0     0 ?        I<   Nov07   0:00 [kworker/0:0H-kblockd]

guo@ubuntu:~$ ps -aux --sort -pcpu | head -n 5
USER         PID %CPU %MEM    VSZ   RSS TTY      STAT START   TIME COMMAND
mysql        949  0.7  1.4 1349256 13412 ?       Ssl  Nov07   5:39 /usr/sbin/mysqld
root       44787  0.2  0.0      0     0 ?        I    07:42   0:03 [kworker/0:1-events]
root       45356  0.2  0.0      0     0 ?        I    08:02   0:01 [kworker/0:0-events]
root         758  0.1  0.4 310756  4336 ?        Ssl  Nov07   0:46 /usr/bin/vmtoolsd

guo@ubuntu:~$ ps -aux --sort -pmem | head -n 5
USER         PID %CPU %MEM    VSZ   RSS TTY      STAT START   TIME COMMAND
root         866  0.0  2.0 1245704 19564 ?       Ssl  Nov07   0:14 /usr/lib/snapd/snapd
root         707  0.0  1.8 411432 18136 ?        SLsl Nov07   0:17 /sbin/multipathd -d -s
root         484  0.0  1.7 176132 16512 ?        S<s  Nov07   0:12 /lib/systemd/systemd-journald
mysql        949  0.7  1.4 1349256 13412 ?       Ssl  Nov07   5:39 /usr/sbin/mysqld
```
--sort +或者-列名，列名都是小写，%用 p 代替

**-pcpu**，对`%CPU`倒序排列