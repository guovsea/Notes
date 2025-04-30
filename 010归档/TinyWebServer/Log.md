## block queue

线程安全的阻塞队列

```cpp
    MutexLock mutex_;
    Condition queue_not_empty_;
    Condition queue_not_full_;

    T *queue_;
    int size_;
    int capacity_;
    int front_;
    int back_;
```

队列满时，push 操作阻塞在条件变量 `queue_not_full_`，等待 `queue_not_full_` 满足；队列空时，pop 操作阻塞在条件变量 `queue_not_empty_`，等待 `queue_not_empty_` 满足。在执行任何操作时都是用 mutex 锁住阻塞队列，保证线程安全。

## Log

```cpp
// 接口
    bool init(const char *file_name, int log_buf_size = 8192,
              int split_lines = 5000000, int queue_capacity = 0);
    void write_log(int level, const char *format, ...);
    void flush(void);
// 成员
    char dir_name[LOG_DIR_LEN];  // 路径名
    char log_name[LOG_NAME_LEN]; // log文件名
    int split_lines_;            // 单个日志文件最大行数
    int log_buf_size_;           // 日志缓冲区大小
    long long count_;            // 日志行数记录
    int today_;                  // 按天分类,记录当前时间是那一天
    FILE *fp_;                   // 打开log的文件指针
    char *buf_;
    block_queue<string> *log_queue_; // 阻塞队列
    bool is_async_;                  // 是否异步标志位
    MutexLock mutex_;
```

Log 日志模块有两种运行方式

- 同步方式
- 异步方式

**异步方式**

调用`Log::init`初始化 Log 对象时，如果指定 `queue_capacity > 0`，则会为 Log 日志分配一个阻塞队列，同时创建一个日志线程，用于异步地将日志内容写入文件中。

其大致思路是：如果是异步模式，则在使用 write_log 方法时，并不会将日志内容直接写入文件中；而是将日志内容添加到阻塞队列中。添加到阻塞队列时，并没有直接 push buf_指针，而是使用了一个 string 对象进行管理，保证其生命周期结束后可以自动释放 buffer。日志线程会异步地从阻塞队列中取出 buffer，并将其写入文件中。

**同步方式**

调用`Log::init`初始化 Log 对象时，如果指定 `queue_capacity = 0`，则会使用同步的方式记录日志。使用 write_log 方法时，会直接将日志内容写入文件中。

## Log 接口

采用了 4 个宏函数作为 Log 的接口

```cpp
#define LOG_DEBUG(format, ...)                                                 \
    Log::instance()->write_log(0, format, ##__VA_ARGS__)
#define LOG_INFO(format, ...)                                                  \
    Log::instance()->write_log(1, format, ##__VA_ARGS__)
#define LOG_WARN(format, ...)                                                  \
    Log::instance()->write_log(2, format, ##__VA_ARGS__)
#define LOG_ERROR(format, ...)                                                 \
    Log::instance()->write_log(3, format, ##__VA_ARGS__)

```
其中的 va_args 是可变参数，可以传入任意格式化字符串。