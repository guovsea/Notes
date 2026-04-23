---
tags:
  - tools
  - tools/git
  - type/concept
mastery: 5
---

# Git

## 初始化

1. 初始化配置用户名和邮箱

在提交的时候会显示用户名和邮箱

```shell
git config --global user.name "用户名"
git config --global user.email "邮箱"
```
2. 查看
```shell
git config --list
```

3. 新建仓库

```shell
git init [路径名]
git clone URL
```

## git 结构

- **工作区** 
  git 仓库所在的文件夹
- **暂存区**
  使用`git add `将工作区的文件添加到暂存区。*只要 git add 了的文件都会保存在暂存区中，而不是只保存每一次的 git add 的文件*
- **本地仓库**
  `git commit`将暂存区的内容存放到本地仓库

文件状态

![[Git 文件状态.png]]

## 文件控制

查看当前 git 仓库状态

```shell
git status
```

添加到暂存区

```shell
git add 表达式[支持用通配符、文件夹]

```
查看版本库中的文件

```shell
git ls-files
```

提交到本地仓库

```shell
git commit -m
git commit # 会进入交互模式，使用 vim 编辑
# 如果不是vim，可以进行配置：
git config --global core.editor vim
```
查看提交记录

```shell
git log [--oneline # 显示简洁的提交记录]
```

回退版本 `git reset 版本号`

*`HEAD^`可以表示上一次 commit

- **`git reset --soft`** 
  保留工作区和暂存区的内容 *相当于只撤销了 git commit*
- **`git reset --hard`**
  清空工作区和暂存区的内容 *相当于撤销了 git commit 、git add 并删除未跟踪文件*
- **`git reset --mixed`**
  只保留工作区的内容，清空暂存区的内容 *相当于撤销了 git commit 、git add*

**当有多次没有意义的 commit 时，可以 reset 后从新提交**

查看 git 日志，*git 的任何操作都可以撤销*

```shell
git reflog # 查看每个操作后的版本号

git reset --head 版本号  # 回退
```

比较文件差异 `git diff`

- **`git diff`**
  比较工作区和暂存区的差异
- **`git diff HEAD`**
  比较工作区和本地仓库的差异
- **`git diff --cached`**
  比较暂存区和本地仓库的差异
- **`git diff 节点id 节点id [文件名]`** 
  比较两个版本的差异 
  - `git log --oneline`可以查看节点id
  - `HEAD` 可以表示当前分支的最新节点
  - `HEAD^/HEAD~` 最新节点的上一个节点
  - `HEAD~n` HEAD 前 n 个节点
  ```shell
  git diff id HEAD
  git diff HEAD~2 HEAD file.txt
  ```

删除文件

方法一：

1. 在工作区中删除文件并添加到暂存区中 *将工作区同步到暂存区*
2. commit

方法二：

1. `git rm` *会删除工作区、暂存区中的文件*
   `git rm --cached ` *只删除暂存区中的文件*
2. `git commit` *还是要提交*

## .gitignore 文件


生效的前提：文件没有被添加到暂存区中

如果编写`.gitignore`之前文件已经被添加到暂存区中，git 仍然会继续跟踪这个文件。

*git 不会将空文件纳入版本控制（如果文件夹中只有被忽略的文件也同理）*
## 远程仓库

1. 将本地 ssh 公钥添加到 GitHub 账户中
```shell
cat < ~/.ssh/id_rsa.pub   # 如果没有，则使用 ssh-keygen 生成
```
2. 关联远程仓库的地址

`git remote add 关联远程仓库的别名 地址`

```shell
git remote add origin git@github.com:guovsea/TinyWebServer.git
```

3. push

`git push 远程仓库的别名 branch 名`

```shell
git push origin master
```