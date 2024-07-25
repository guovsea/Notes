## 启动调试

### 常规启动

#### 调试可执行文件

`gdb 可执行文件`

#### 调试已经运行的进程

`ps -ef | grep ` -> 查看 pid

重新编译一个带调试信息的可执行文件，a.out

`file a.out` -> 加载符号信息

`attach pid` -> 开始调试进程pid

#### 调试带参数的程序

`run 参数列表`  或

`set args=...`然后再`run`

## 设置断点

### 常规断点

#### 普通断点

`b [file]:行号  # break的简写`

`b [file]:函数名` -> 设置函数断点，完成形参列表赋值后停住

`rb [file]:函数名的regex` -> 根据正则表达式批量设置函数断点

#### 条件断点

`普通断点 if 表达式` -> 当表达式为真时，在该断点停住，否则不停住

`condition 断点号 表达式` -> 给普通断点增加条件

### 表达式值变化断点

`watch var` -> 当表达式值发生变化时停住 *程序必须运行起来*

### 禁用启用断点

```text
enable/disable -> 禁用启用所有断点
enable/disable 断点号 -> 禁用启用某个断点

enable delete 断点号 -> 弃用断点后删除

```

### 删除断点

```text
clear
clear [file:]位置（函数名或者文件行号）

delete 
delete 断点号
```

### 跳过断点多次

`ignore 断点号 次数`
## 查看变量

### 普通变量

#### 基本类型、数组、字符串

`p ['file'::]var` -> 打印某个文件中的变量

`p ['main'::]var`-> 打印某个函数中的变量

#### 指针

`p pointer` -> 打印指针的值

`p *pointer` -> 打印指针所指的对象

`p *pointer@偏移量*` -> 打印指针向后偏移 n 个位置的元素

```text
p *head
p *$.next   -> 不断打印链表的值

set $index = 0
p arr[index++] -> 不断打印数组值
```
#### 格式化打印

`p/f 打印表达式`

f 的取值

- x 16 进制
- d 10 进制
- - u 10 进制无符号整数
- o  8 进制
- t  2 进制
- f 浮点数形式
- c 字符形式

使用更直观的格式打印结构体

`set print pretty off`

### 查看内存内容

`x/[n][f][u] addr` -> examine，查看内存

n ： 单元数量，默认值 1

u ： 单元类型

- b 字节
- h 半字节， 0.5 字节
- w 字，4 字节
- g 宽字，8 字节

### 跟踪显示变量

`display 变量名`

```text
info display  -> 查看所有display的信息
disable/enable 标号 -> 关闭/打开 标号的display
delete display 标号
```

### 查看寄存器内容

`info registers`
## 单步调试

### 执行

`n [步数]`

### 步入

`s [步数]`

### 只执行一条汇编指令

`si` 步入 instruction

`display/i $pc` 查看汇编指令

### 继续执行到下一断点

`continue [次数]`

### 运行到指定位置停住

`until [file:]行号`

### 跳过某函数或文件

```text
skip [funtion] [函数名] -> 默认指定的内容为函数名
skip file [文件名]
```
```text
info skip
skip delete
skip enable
skip disable
```
## 调试多线程程序

### 查看线程信息

`info threads` -> 显示线程号等线程信息
### 切换线程

`thread  线程号`

### 针对特定线程设置断点

```shell
break[b] 断点表达式 thread [线程号]
#  线程号可以指定一个，多个（用, 分隔），全部（all）
```

### 运行线程

```shell
thread apply 线程号 命令(如 s，n) -> 在某个线程上执行gdb命令
#  线程号可以指定一个，多个（用, 分隔），全部（all）
```
设置线程运行方式：
```
set scheduler-locking off|on|step
```

- off：不锁定其他线程，也就是所有的线程都执行，默认值。
- on：锁定其他线程，只有当前被调试的线程能够执行。
- step：阻止其他线程在当前线程单步调试时，抢占当前线程。只有当 next、continue、util 以及 finish 的时候，其他线程才会获得重新运行的机会。

## 调试多进程程序

在 gdb 中，进程被称为 inferior 

### 查看进程信息

`info inferior`

### 设置进程调试选项

`set detach-on-fork on|off`

- on：断开调试 follow-fork-mode 指定的进程，默认值
- off：gdb 将控制父进程和子进程。follow-fork-mode 指定的进程将被调试，另一个进程置于暂停（suspended）状态。

`set follow-fork-mode parent|child`

- parent：fork 之后继续调试父进程，子进程不受影响。
- child：fork 之后调试子进程，父进程不受影响。


例如：如果需要调试子进程，那么 gdb 启动之后，执行`(gdb) set follow-fork-mode child`并在子进程处设置断点。

### 使用 attach的方式调试多进程

问题是，子进程一直在运行，attach上去后都不知道运行到哪里了

一个办法是，在要调试的子进程初始代码中，比如 main 函数开始处，加入一段特殊代码，使子进程在某个条件成立时便循环睡眠等待，attach 到进程后在该代码段后设上断点，再把成立的条件取消，使代码可以继续执行下去。

至于这段代码所采用的条件，看你的偏好了。比如我们可以检查一个指定的环境变量的值，或者检查一个特定的文件存不存在。以文件为例，其形式可以如下：

```c
void debug_wait(char *tag_file)
{
    while(1)   {
        if (tag_file存在)
            睡眠一段时间;
        else
            break;
    }
}
```
当 attach 到进程后，在该段代码之后设上断点，再把该文件删除就 OK 了。当然你也可以采用其他的条件或形式，只要这个条件可以设置/检测即可。

Attach进程方法还是很方便的，它能够应付各种各样复杂的进程系统，比如孙子/曾孙进程，比如守护进程（daemon process），唯一需要的就是加入一小段代码。

[GDB多线程多进程调试-腾讯云开发者社区-腾讯云](https://cloud.tencent.com/developer/article/1142947)

[Fetching Title#3ys1](https://www.jianshu.com/p/8fc05dbf777e)
## 源码查看

```text
l 行号 -> 列出行号附近的代码
l 起始位置, 结束位置 （可以省略其中一个）
l [file:]函数名      -> 列出某个文件中的函数
l [file:]行号表达式   -> 列出文件中的某些行

dir 路径名 -> 指定源码路径
```

## GDB QUICK REFERENCE
![[gdb-reference 1.pdf]]