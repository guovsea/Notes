## c++

### 编译

cmake插件

### 调试

**普通断点**

**条件断点**

条件表达式为真时中断

![[Pasted image 20230831124040.png]]

**日志断点**

运行到该处都会输出日志

**次数断点**

该行被运行了N次时中断

**函数中断**

运行到某个函数中断（只需要函数名）

![[Pasted image 20230831124654.png]]

**变量中断**

变量值改变时中断

![[Pasted image 20230831124720.png]]

**内联断点**

在一行中有多个表达式，可以选择某个表达式作为断点

方法：选中后`shift + F9`

**需要读取标准输入**

在`launch.json`中设置`"externalConsole": true`, 打开终端，在终端中输入


### launch和tasks
配置c++调试文件

launch.json

```json
{
    "version": "0.2.0",
    "configurations": [
        {
            "name": "Debug",
            "type": "cppdbg",
            "request": "launch",
            // 生成的可执行文件的绝对路径
            "program": "${workspaceFolder}/build/TEST",
            // 传给程序的命令行参数
            "args": [],
            "stopAtEntry": false,
            "cwd": "${fileDirname}",
            "environment": [],
            "externalConsole": false,
            "MIMode": "lldb",
            "preLaunchTask" : "Build"
        }
    ]

}
```

tasks.json

用于定义task

```json
{   
    "version": "2.0.0",
    "options": {
        "cwd": "${workspaceFolder}/build"
    },
    "tasks": [
        {
        // cmake任务
            "type": "shell",
            // 这个task的名称叫“cmake”
            "label": "cmake",
            // 命令
            "command": "cmake",
            // 命令参数
            "args": [
                ".."
            ]j
        },
        {
        // make任务
            "label": "make",
            "group": {
                "kind": "build",
                "isDefault": true
            },
            "command": "make",
            "args": [
            ]
        },
        {
        // Build任务
            "label": "Build",
            "dependsOrder": "sequence", // 按列出的顺序执行任务依赖项
            "dependsOn":[
                "cmake",
                "make"
            ]
        }
    ]
}
```
