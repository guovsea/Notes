## 解析 HTTP 请求
### 从状态机


三种状态，标识解析一行的读取状态。

```cpp
    enum LINE_STATUS {
        LINE_OK = 0, // 完整读取一行
        LINE_BAD,    // 报文语法有误
        LINE_OPEN    // 读取的行不完整
    };
```
事件：

read_buf 中的字符

动作：

返回 LINE_OPEN

在 HTTP 报文中，每一行的数据由`\r\n` 作为结束字符，空行则是仅仅是字符`\r\n`。因此，可以通过`查找\r`\n 将报文拆解成单独的行进行解析。

从状态机负责读取 buffer 中的数据，将每行数据末尾的`\r\n` 置为`\0\0`，并更新从状态机在 buffer 中读取的位置 checked_idx，以此来驱动主状态机解析。


```cpp
// 返回值为行的读取状态，有LINE_OK,LINE_BAD,LINE_OPEN(不完整)
http_conn::LINE_STATUS http_conn::parse_line() {
    char temp;
    for (; checked_idx_ < read_idx_; ++checked_idx_) {
        temp = read_buf_[checked_idx_];
        if (temp == '\r') {
            if ((checked_idx_ + 1) == read_idx_)
                return LINE_OPEN;
            else if (read_buf_[checked_idx_ + 1] == '\n') {
                read_buf_[checked_idx_++] = '\0';
                read_buf_[checked_idx_++] = '\0';
                return LINE_OK;
            }
            return LINE_BAD;
        } else if (temp == '\n') {
            if (checked_idx_ > 1 && read_buf_[checked_idx_ - 1] == '\r') {
                read_buf_[checked_idx_ - 1] = '\0';
                read_buf_[checked_idx_++] = '\0';
                return LINE_OK;
            }
            return LINE_BAD;
        }
    }
    return LINE_OPEN;
}
```

**HTTP_CODE含义**

表示HTTP请求的处理结果，在头文件中初始化了八种情形，在报文解析时只涉及到四种。

```cpp
    enum HTTP_CODE {
        NO_REQUEST = 0, // 请求不完整，需要继续读取请求报文数据
        GET_REQUEST,    // 获得了完整的HTTP请求
        BAD_REQUEST,    // HTTP请求报文有语法错误
        NO_RESOURCE,
        FORBIDDEN_REQUEST,
        FILE_REQUEST,
        INTERNAL_ERROR, // 服务器内部错误
        CLOSED_CONNECTION
    };
```

### 主状态机

三种状态，标识解析状态。

```cpp
    enum CHECK_STATE {
        CHECK_STATE_REQUESTLINE = 0,  // 解析请求行
        CHECK_STATE_HEADER,           // 解析请求头
        CHECK_STATE_CONTENT           // 解析消息体，仅用于解析POST请求 
    };
```
事件：

从状态机的返回值

动作：

使用不同的解析方法，返回`do_request`



## 发送 HTTP 响应

