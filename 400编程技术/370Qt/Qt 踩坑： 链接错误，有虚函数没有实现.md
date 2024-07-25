
具有 Q_OBJECT 宏的类都需要 MOC 处理，也就是说需要把这个类所在的头文件传给编译器

所以这里一定要吧头文件也传给 编译器

```cmake
# 编译测试代码  
add_executable(${PROJECT_NAME} ${TESTS} ${SOURCES} ${HEADERS})
```