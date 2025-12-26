## 语法

`$ <>` 

> 区别于获取普通变量：`${}`

## 使用场景

**只有生成器才完全知道的内容**

- The name and path of target outputs (mainly when cross-compiling and in multi-configuration environments)
- Or more generally any target property that the generator evaluates to mingle together the compiler/linker calls

## 参考

[cmake-generator-expressions(7) — CMake 3.29.2 Documentation](https://cmake.org/cmake/help/latest/manual/cmake-generator-expressions.7.html)

[【09】生成器表达式 | Cccolt 个人主页](https://www.cccolt.top/tutorial/cmake/09.html)
1. 布尔生成器表达式
   - `$<BOOL:string>` 将字符串转化为 `Boolean`
   - `$<NOT:condition>` 
   - `$<AND:conditions>` `conditions` 为 `,` 分割的生成器表达式
   - `$<OR:conditions>`
2. 字符串比较
   - `$<STREQUAL:string1,string2>`
   - `$<EQUAL:value1,value2>`
   - `$<IN_LIST:string,list>`
3. 变量查询
   - `$<TARGET_EXISTS:target>` 判断 `target` 是否存在
   - `$<CONFIG:cfgs>`  判断编译类型配置是否包含在 `cfgs` 列表（比如"Release，Debug"）中；不区分大小写
   - `$<PLATFORM_ID:platform_ids>` 判断 CMake 定义的平台 ID 是否包含在 `platform_ids` 列表中
   - `$<COMPILE_LANGUAGE:languages>` 判断编译语言是否包含在 `languages` 列表中

## 常用场景

复制可执行文件旁边的文件

区分例如 `DEBUG` 或 `RELEASE` 配置

[c++ - Cmake generator expressions - Stack Overflow](https://stackoverflow.com/questions/46206495/cmake-generator-expressions)



