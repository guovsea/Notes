## GCC 常用选项

| 选项                | 作用                                                                                    |
| ------------------- | --------------------------------------------------------------------------------------- |
| -E                  | 生成预处理代码，通常保存为`.i`文件                                                      |
| -S **大写**         | 生成汇编代码，通常保存为`.s`文件。                                                      |
| -c                  | 生成二进制代码，不进行链接，通常保存为`.o`文件                                          |
| -o                  | 生成可执行文件 或者 指定输出文件名                                                      |
| -g                  | 包含调试信息，支持 GDB 调试                                                             |
| -On, n=0,1,2,3      | 编译优化等级，n 越大优化得越多                                                          |
| -Wall               | 提示所有警告信息                                                                        |
| `-D宏名[=可选的值]` | 编译时定义宏 D 和宏名之间没有空格，可以用来 DEBUG                                       |
| `-l库的SONAME`      | 链接第三方库，多个库使用多个-l 选项，默认使用动态库，可以用`–static -l`强制指使用静态库 |
| -Werror             | 将警告信息视作错误信息                                                                  |
| –I                  | 指定头文件路径                                                                          |
| -L                  | 指定的库文件搜索路径。                                                                  |
| -MM                 | 由编译器自动生成的依赖关系 (不包括标准库头文件)                                                                                        |

[[编译过程]]

[Linux GCC常用命令 - ggjucheng - 博客园](https://www.cnblogs.com/ggjucheng/archive/2011/12/14/2287738.html#_Toc311642840)

## makefile

### 最基本的makefile

所有`.c`文件都在同一个文件夹下，生成的`.o`文件也在在当前文件夹下

```makefile
CXX = g++
server:ConnPool.o Log.o HttpConn.o ConnPool.h HttpConn.h Log.h Timer.h ThreadPool.h sync.h
        $(CXX) main.o ConnPool.o Log.o HttpConn.o -o server -lpthread -lmysqlclient

ConnPool.o:ConnPool.cpp ConnPool.h
        $(CXX) -c ConnPool.cpp -o ConnPool.o

Log.o:Log.cpp Log.h
        $(CXX) -c Log.cpp -o Log.o

HttpConn.o:HttpConn.cpp HttpConn.h
        $(CXX) -c HttpConn.cpp -o HttpConn.o

main.o:main.cpp
        $(CXX) -c main.cpp -o main.o

.PHONY: clean

clean:
        rm -f main.o ConnPool.o Log.o HttpConn.o
```
**解释**

```makefile
foo = abc      # 类似 C 中的宏定义
$(foo)         # 使用变量，直接将变量的值展开
```

`.PHONY: clean` 声明了 `clean` 是一个伪目标，意在告之`clean`并是一个文件。因此`make clean`都会执行生成`clean`的规则，无论当前目录中是否有"`clean`"这个文件。


更加简洁，更具拓展性
```makefile
CXX = g++
SRC = $(wildcard *.cpp)
OBJ = $(patsubst %.cpp,%.o,$(SRC))
HEADERS = $(wildcard *.h)
LIBS = -lpthread -lmysqlclient

server:$(OBJ) $(HEADERS)
    $(CXX) $(OBJ) -o server $(LIBS)

$(OBJ):%.o:%.cpp
    $(CXX) -c $< -o $@

.PHONY: clean

clean:
    -rm -f $(OBJ) server
```
**解释**

**函数**， makefile 中使用`$(函数明 参数...)`的方式来调用函数

- `wildcard` 匹配当前文件夹中的文件夹
- `patsubst` 替换，用法`$(patsubst 匹配模式,替换模式,需要被替换的文本)`  
  `%` 的意思是匹配零或若干字符   
  `$(patsubst %.cpp,%.o,$(SRC))`会将 `$(SRC)`中的所有字符串都使用`%.cpp`进行匹配，然后将`%`所匹配的内容替换掉`%.o`中的`%`.

**自动变量**：`$<`表示第一个依赖的文件，`$^`表示整个依赖列表，`$@`表示规则中目标。

**模式规则/静态规则**：

在规则中，可以使用`[$(OBJ):]%.o:%.cpp`来指定指定一种通用的模式。语意是逐一取出\[在`$(OBJ)`中取出\]符合`:%o`的所有目标，都使用如下模式生成：

它们的依赖都是 `%.cpp`，命令由自动变量定义。


在这条模式规则中：
```makefile
$(OBJ):%.o:%.cpp
    $(CXX) -c $< -o $@
```
会依次取出：`filename1.o`、`filename2.o`、`filename3.o` ...，`%`匹配都是各个文件名除去后缀部分，因此`%.cpp`每次都只会匹配一个`.cpp`文件。规则中的依赖始终都只有一个文件，因此使用`$<`和`$^`并没有区别。

但是

```makefile
a.out:main.c test.c
    gcc $< -o $@   # gcc main.c -o a.out 
    gcc $^ -o $@   # gcc main.c test.c -o a.out
```
就存在区别。


### L2, 将生成的目标文件放在 obj 文件夹下

```makefile
CXX = g++
SRC = $(wildcard *.cpp)
OBJ = $(patsubst %.cpp,obj/%.o,$(SRC))
HEADERS = $(wildcard *.h)
LIBS = -lpthread -lmysqlclient

server:$(OBJ) $(HEADERS)
    $(CXX) $(OBJ) -o server $(LIBS)

$(OBJ):obj/%.o:%.cpp
    $(CXX) -c $^ -o $@
    echo $<

.PHONY: clean

clean:
        -rm -f $(OBJ) server
```
**注意**

```makefile
OBJ = $(patsubst %.cpp,obj/%.o,$(SRC))

$(OBJ):obj/%.o:%.cpp
```

### L3，分模块编写

```text
.
├── CGImysql
│   ├── ConnPool.cpp
│   └── ConnPool.h
├── http
│   ├── HttpConn.cpp
│   ├── HttpConn.cpp.bkp
│   ├── HttpConn.h
│   └── HttpConn.h.bkp
├── log
│   ├── BlockQueue.h
│   ├── Log.cpp
│   ├── Log.h
│   └── bq_test.cpp
├── main.cpp
├── makefile
├── obj
│   ├── ConnPool.o
│   ├── HttpConn.o
│   └── Log.o
├── sync.h
├── threadpool
│   ├── ThreadPool.h
│   └── test.cpp
└── timer
    └── Timer.h

```
```makefile
CC = g++
CFLAGS = -g -Wall

# 定义模块目录
MODULES = CGImysql http log threadpool

# 生成目标文件的目录
OBJ_DIR = obj

# 所有.cpp文件
SRCS = $(foreach dir, $(MODULES), $(wildcard $(dir)/*.cpp))

# 生成.o文件列表
OBJS = $(patsubst %.cpp, $(OBJ_DIR)/%.o, $(notdir $(SRCS)))

# 编译目标
TARGET = server

# 编译规则
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# 生成.o文件的规则
$(OBJ_DIR)/%.o: $(foreach dir, $(MODULES), $(wildcard $(dir)/*.cpp))
	$(CC) $(CFLAGS) -c $< -o $@

# 清理生成的文件
clean:
	rm -f $(TARGET) $(OBJ_DIR)/*.o

# 声明伪目标，防止与文件名冲突
.PHONY: clean
```
**缺陷**：每个`.o`文件都依赖与所有`.cpp`文件，只要有一个源文件更新后，就会重写编译所有文件

**实际上应该为每个模块都写一个 makefile，负责对模块的的编译，然后主 makefile 再编译所有模块生成可执行文件**

**解释**

**函数**， `$(notdir $(SRCS)` 除去`$(SRCS)`中所有元素的路径，只保留文件名

`$(foreach 循环变量, 列表(变量), 对循环变量的表达式)`，`foreach`类似 `shell` 中的循环，每次都回在列表变量中取出循环变量，然后对循环变量进行某些操作，最后拼接在一起形成一个字符串返回。

**在目标和依赖中也可以使用函数**

### L3，分 include 和src

```makefile
# 声明编译器和编译选项
CC = g++
CCFLAGS = -std=c++11 -Wall -Wextra

# 声明库路径
LIBS = -lpthread -lmysqlclient

# 声明目标文件名和源文件名
TARGET = server
SRCS = $(wildcard src/*.cpp)
OBJS = $(patsubst src/%.cpp,obj/%.o,$(SRCS))

# 默认目标
all:$(TARGET)

# 编译目标
$(TARGET):$(OBJS)
        $(CC) $(CCFLAGS) $^ -o $@ $(LIBS)

# 生成目标的依赖关系
obj/%.o:src/%.cpp
        $(CC) $(CCFLAGS) -c $< -o $@

$(OBJS):|obj

# 创建obj文件夹
obj:
        mkdir -p obj

# 清理中间文件和可执行文件
clean:
        -rm -f $(TARGET) $(OBJS)

.PHONY: all clean
```
**解释**

 `$(OBJS):|obj` 在 Makefile 中使用`|`来指定先决条件时，**表示在构建目标之前，先决条件必须存在**。*规定了一个顺序，先决条件存在后，才构建目标*

## 参考

[跟我一起写 Makefile](https://seisman.github.io/how-to-write-makefile/index.html)

