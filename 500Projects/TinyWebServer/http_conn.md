---
tags:
  - act/project
  - network/protocol
  - type/howto
mastery: 5
---

HTML：超文本标记语言（Hyper Text Mark-up Language），是构成网页内容的标记语言。

HTTP：超文本传输协议（ HyperText Transfer Protocol），应用最为广泛的一种网络应用层协议

## HTTP 协议基础

### HTTP 消息分类：

- **请求消息(Request)**
浏览器发送给服务器的消息。主要包括：

1. **请求行**: 说明 [[#HTTP 请求类型]], 要访问的资源, 以及使用的 http 版本
2. **请求头**: 说明服务器要使用的附加信息
3. **空  行**: 必须！即使没有请求数据
4. **请求数据**: 也叫主体, 可以添加任意的其他数据


```text
GET  /hello.c  HTTP/1.1
Host: localhost:2222
User-Agent: Mozilla/5.0 (X11; Ubuntu; Linux i686; rv:24.0) Gecko/201001    01 Firefox/24.0
Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8
Accept-Language: zh-cn,zh;q=0.8,en-us;q=0.5,en;q=0.3
Accept-Encoding: gzip, deflate
Connection: keep-alive
If-Modified-Since: Fri, 18 Jul 2014 08:36:36 GMT

// 请求消息
```

- **响应消息(Response)**

服务器发送给浏览器的消息。主要包括：

1. 状态行: 包括 http 协议版本号, 状态码, 状态信息
2. 消息报头: 说明客户端要使用的一些附加信息
3. 空  行: 必须!
4. 响应正文: 服务器返回给客户端的文本信息

```text
HTTP/1.1  200  Ok
Server: xhttpd
Date: Fri, 18 Jul 2014 14:34:26 GMT
Content-Type: text/plain; charset=iso-8859-1 (必选项)
Content-Length: 32  （ 要么不写 或者 传-1， 要写务必精确 ！ ）
Content-Language: zh-CN
Last-Modified: Fri, 18 Jul 2014 08:36:36 GMT
Connection: close

// 响应消息
```

### HTTP 请求类型

- GET
  请求指定的页面信息，并返回实体主体。
- POST
  向指定资源提交数据进行处理请求（例如提交表单或者上传文件）。数据被包含在请求体中。POST请求可能会导致新的资源的建立和/或已有资源的修改。
- HEAD
  类似于 get 请求，只不过返回的响应中没有请求内容，用于获取报头
- PUT
  从客户端向服务器传送的数据取代指定的文档的内容。
- DELETE
- CONNECT
- OPTIONS
- TRACE

### HTTP 常用状态码

HTTP 有5种类型的状态码，具体的：

- 1xx：指示信息--表示请求已接收，继续处理。
- 2xx：成功--表示请求正常处理完毕。
  - 200 OK：客户端请求被正常处理。
  - 206 Partial content：客户端进行了范围请求。
- 3xx：重定向--要完成请求必须进行更进一步的操作。
  - 301 Moved Permanently：永久重定向，该资源已被永久移动到新位置，将来任何对该资源的访问都要使用本响应返回的若干个 URI 之一。
  - 302 Found：临时重定向，请求的资源现在临时从不同的URI中获得。
- 4xx：客户端错误--请求有语法错误，服务器无法处理请求。
  - 400 Bad Request：请求报文存在语法错误。
  - 403 Forbidden：请求被服务器拒绝。
  - 404 Not Found：请求不存在，服务器上找不到请求的资源。
 - 5xx：服务器端错误--服务器处理请求出错。
   - 500 Internal Server Error：服务器在执行请求时出现错误。

### 常用网络文件类型

```text
普通文件: text/plain; charset=iso-8859-1
*.html : text/html; charset=iso-8859-1
*.jpg	: image/jpeg
*.gif	: image/gif
*.png: image/png
*.wav: audio/wav
*.avi	: video/x-msvideo
*.mov	: video/quicktime
*.mp3	: audio/mpeg
charset=iso-8859-1		西欧的编码，说明网站采用英文编码
charset=gb2312     	说明网站采用的编码是简体中文
charset=utf-8     		代表世界通用的语言编码；可以用到中文、韩文、日文等世界上所有语言编码
charset=euc-kr        说明网站采用的编码是韩文；
charset=big5          说明网站采用的编码是繁体中文；
```
## HTML 处理工具

[[正则表达式]]

## 有限状态机

## 相关

- [[http报文解析]]
- [[项目架构]]
- [[正则表达式]]
