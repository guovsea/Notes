
## 变量和 if-condition

- A variable can be put as a condition to test if it is set

```cmake
unset(a) # emphasizing a is not set
set(b "sample.cpp")

if(a)
    # the code here not run
endif()

if (b)
    # the code here is run
endif()
```

**Note:** In the above example, `if-condition` checks whether its argument is a set variable, _DO NOT_ use `${}`. Otherwise, `if-condition` checks whether the value of the variable is a variable.

总结：Always use the name of a variable without `${}` in conditions.

## 数字和 if-confition

Every variable is a string but CMake can compare numbers with `LESS`, `EQUAL`, `GREATER`, and so forth.

```cmake
if(1 EQUAL 01.0)  # a true condition
	message("1 equal to 01.0") 
endif()
```

See the manual for [if() here](https://cmake.org/cmake/help/latest/command/if.html).


## foreach 遍历列表

```cmake
set(SOURCES a.cpp b.cpp)

foreach( FILE IN LISTS SOURCES)
    message(${FILE})
endforeach()
```

形式：**迭代变量 `IN LISTS` 列表变量**  `IN LISTS` 是必须的，而且都要大写

输出：

```
a.cpp
b.cpp
```

---

```cmake
set(SOURCES a.cpp b.cpp)

foreach( FILE In LISTS SOURCES)
    message(${FILE})
endforeach()
```

形式：`迭代变量 列表变量` 

> 在这里 In LISTS SOURCES 作为一个列表变量

输出：

```
In
LISTS
SOURCES
```


```cmake
# 不要这样做，要用 foreach(FILE IN LISTS SOURCES)
foreach( FILE  ${SOURCES})
    message(${FILE})
endforeach()
```

输出：

```
a.cpp
b.cpp
```

Notice here, in foreach, the same as if-condition, we drop `${}` from variable.

**也不要在 foreach 中使用 ${} !!!** 非常让人困惑

## Cache 变量

命令行中的定义的普通变量是一次性的，再次运行时变量的值会消失

The state of normal variables is lost after a `cmake` run. To overcome this, we have cached variables which are written in `CMakeCache.txt` file. Whenever we run `cmake` command they are loaded from that file. These variables aim to store user preferences on disk. Some examples of user preferences are:

```cmake
set(<variable> <value>  CACHE <type> <docstring>)
```

After the first time, anymore `cmake` is called, the line above will be ignored because `libAPath` is already in the cache.

> 因此，如果修改了/删除了 CACHE 变量，要删除掉 CmakeCache.txt 重新执行

用户可以修改默认变量的值: `cmake -D` ，在 `cmake-gui` 中修改 `cache`

`<type>` 是提供给 `cmake-gui` 的

```
- FILEPATH: GUI shows a file selector dialog.
- PATH: GUI shows a directory selector dialog.
- STRING: GUI shows a textbox.
- BOOL: GUI shows a checkbox.
- INTERNAL: Hidden from GUI, for the developer.
```

You can also set a cached variable when running cmake with `-D` flag:

```bash
cmake -D <var>:<type>=<value>

cmake -D name:STRING=Guo
```

**`<type>` 只是提供给 GUI 的，在命令行中无任何意义，可也以不输入**

> 原因：CMake 中的变量都只是字符串，实际上并没有类型

```cmake
set(NAME "name" CACHE STRING "user name")
message(${NAME})
```

```
Mac:build guo$ cmake .. -DNAME:BOOL=ON
ON
```

```
Mac:build guo$ cmake .. -DNAME:STRING=Guo
Guo
```

```
Mac:build guo$ cmake .. -DNAME=ON
ON
```

BOOL 变量的快捷定义方式

```cmake
option(<var> <docstring> ON/OFF)
```

FORCE 可以强制让变量变成脚本中的值，而不是用户定义的值


```cmake
set(NAME "name" CACHE STRING "user name" FORCE)
message(${NAME})
```

```
Mac:build guo$ cmake .. -DNAME=apple
name
```

## function

### 形式

```cmake
function(NameOfFunction arg1 arg2)
    # body of function
endfunction()
```
### 作用域

函数中定义的变量只在函数中可见，除非使用 `PARENT_SCOPE`

函数可以使用**调用函数时**，它父作用域的**拷贝**（不会修改原来的值，除非使用 `PARENT_SCOPE`）

```cmake
function(myfunc )
   message(====myfunc====)
   message(${name}) 
   set(name Chao)
   message(${name}) 
   message(====myfunc====)
   set(age 22 PARENT_SCOPE)
endfunction()

set(name Guo)
set(age 18)

myfunc()
```

```
====myfunc====
Guo
Chao
====myfunc====
Guo
22
```
### 可变参数

在函数体中，ARGV 表示传入函数的所有参数

```cmake
function(myfunc )
    foreach(arg IN LISTS ARGV)
        message(${arg})    
    endforeach()
   set(age 22 PARENT_SCOPE)
endfunction()

myfunc(a b c)
```

## Macro

和函数用法相同，但在父作用域中

```cmake
macro(setName)
    set(name "Sara")
endmacro()

setName()

message(${name}) 
```

## add_subdirectory

**原理**：Running `cmake`, it will set `CMAKE_CURRENT_SOURCE_DIR` variable to the `myProject` path. When it reaches the `add_subdirectory` line, it will jump into `library1/CMakeLists.txt`, set `CMAKE_CURRENT_SOURCE_DIR` to `library1` path, and runs the command there. Afterward, CMake comes back to the project scope and sets `CMAKE_CURRENT_SOURCE_DIR` to `myProject` path. **The variables in subdirectory scope are private and not visible to the project scope.** subdirectory 的作用域对于 project 的作用域不可见

While `CMAKE_CURRENT_SOURCE_DIR` is dependent on the location of focused `CMakeLists.txt`, `PROJECT_SOURCE_DIR` is always set to the top-level folder containing `CMakeLists.txt` which has `project()` command in it.

A subdirectory usually contains some source files that need to be compiled.

```
-- myProject
    |
    ----- build
    |
    ----- library1
    |        |
    |        ---- CMakeLists.txt  #  subdirectory scope
    |
    ------ CMakeLists.txt  # project scope
```

## Include

We can include CMake scripts from another file by `include`. **No new scope is created as if the content of the file is pasted at the `include()` line.**

Include **不会**创建一个新的作用域

### Modeules

If the file has the extension of `.cmake`, it is called a module and we don’t need to mention its extension. It is common to add the folder containing modules to `CMAKE_MODULE_PATH` list variable. So, `include` command automatically search those folders for the mentioned module.

添加新的模块搜索路径，最好使用 `list APPEND`，防止覆盖已经设置的路径：

```cmake
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/mymodules")
```