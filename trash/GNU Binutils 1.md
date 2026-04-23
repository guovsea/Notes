---
tags:
  - tools
mastery: 5
---
- ar 打包生成静态库
- as 汇编器
- ld 链接器。编译过程中，gcc 会调用链接器 ld，将用户编译生成的目标文件连同系统的 libc 启动代码链接在一起形成最终的可执行文件
- nm 查看目标文件中的符号(全局变量、全局函数等)
- objcopy 将原目标文件中的内容复制到新的目标文件中，可以通过不同的命令选项调整目标文件的格式，比如去除某些ELF文件头
- objdump 用于生成反汇编文件，主要依赖 objcopy 实现，a.out 编译时需要-g，`objdump -dSsx a.out > file`
- ranlib 为静态库文件创建索引，相当于ar命令的s选项
- readelf 解读 ELF 文件头

[GNU Binutils - Wikipedia](https://en.wikipedia.org/wiki/GNU_Binutils)