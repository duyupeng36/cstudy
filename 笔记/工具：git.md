# 版本控制：git

[Git](https://git-scm.com/) 是一个免费的开源 **分布式版本控制系统**，旨在快速高效地处理从小到大的项目。下图给出了分布式 GIT 和 SVN 的差异

![Pasted image 20250105182546|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755788025731-9906d0534fdc4056af540cf990571f95.png)

## 安装

### Windows 上安装 Git

使用 scoop 安装 Git

```shell
scoop install git
```

或者在 [git-downloads](http://git-scm.com/downloads) 上下载安装包

或者使用 `winget` 安装

```shell
winget install --id Git.Git -e --source winget
```

### Linux 上安装 Git

各大 Linux 平台可以使用包管理器（apt-get、yum 等）进行安装。

```shell
# Ubuntu 
sudo apt install git
```

## 基础配置

Git 提供了命令 `git config` 用于配置或读取响应的工作环境变量。这些变量存储 `$HOME/.gitconfig` `/ect/gitconfig` `.git/config` 文件中

### 用户信息

配置个人的用 **户名称** 和 **电子邮件地址**，这是为了 **在每次提交代码时记录提交者的信息**：

```shell
git config --global user.name "name"
git config --global user.email "email address"
```

选项 `--global` 指定更改位于 `$HOME/.gitconfig` 文件中的信息

### 文本编辑器

Git 默认使用的文本编辑器，一般可能会是 vi 或者 vim，如果你有其他偏好设置，比如 VS Code 的话，可以重新设置：

```shell
git config --global core.editor "code --wait"
```

### 差异分析工具

还有一个比较常用的是，在解决合并冲突时使用哪种差异分析工具。比如要改用 vimdiff 的话：

```shell
git config --global merge.tool vimdiff
```

### 查看配置信息

要检查已有的配置信息，可以使用 `git config --list` 命令

```shell
$ git config --list
http.postbuffer=2M
user.name=runoob
user.email=test@runoob.com
```

## Git 基本概念

学习 Git 之前，我们需要先了解基本概念

| 概念  | 描述                              |
| :-- | :------------------------------ |
| 工作区 | 当前工作目录                          |
| 暂存区 | 存放在 `.git` 中的 `index` 文件中。也称为索引 |
| 版本库 | 工作目录中的隐藏目录 `.git`               |
下面这个图展示了工作区、版本库中的暂存区和版本库之间的关系

![Pasted image 20250105195227|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755788025731-29d3281d00de40ce98747c803094b382.png)

## Git 基本操作

Git 的工作就是创建和保存你项目的快照及与之后的快照进行对比。Git 常用的是以下 6 个命令：
+ **git clone**：克隆远程仓库到本地
+ **git push**：将本地仓库推送到远程仓库
+ **git add**：将工作区的更改添加到暂存区
+ **git commit**：将暂存区提交到本地仓库
+ **git checkout**：从本地仓库迁出
+ **git pull**：从远程仓库拉去更新

下图展示了这些命令操作关系

![Pasted image 20250105201555|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755788025732-47b6b151d3a74bfc8bef7b8307f7a686.png)

### 创建仓库

下表列出了 `git` 创建仓库的命令

| 命令          | 描述     |
| :---------- | ------ |
| `git init`  | 初始化仓库  |
| `git clone` | 拷贝远程仓库 |

### 提交与修改

Git 的工作就是创建和保存你的项目的快照及与之后的快照进行对比。

下表列出了有关创建与提交你的项目的快照的命令

| 命令              | 描述                 |
| :-------------- | :----------------- |
| `git add`       | 添加文件到暂存区           |
| `git status`    | 查看仓库当前转态，显示变更的文件   |
| `git diff`      | 比较文件的不同，即暂存区与工作区差异 |
| `git difftool`  | 使用外部差异工具查看和比较文件的更改 |
| `git range-dif` | 比较两个提交范围之内的差异      |
| `git commit`    | 提交暂存区到本地仓库         |
| `git reset`     | 回退版本               |
| `git rm`        | 将文件从暂存区和工作区中删除     |
| `git mv`        | 移动或重命名工作区文件        |
| `git notes`     | 添加注释               |
| `git checkout`  | 分支切换               |
| `git switch`    | 分支切换               |
| `git restore`   | 恢复或撤销文件的更改         |
| `git show`      | 显示 Git 对象的详细信息     |

### 提交日志


| 命令                 | 描述                                  |
| ------------------ | ----------------------------------- |
| `git log`          | 查看历史提交记录                            |
| `git blame <file>` | 以列表形式查看指定文件的历史修改记录                  |
| `git shortlog`     | 生成简洁的提交日志摘要                         |
| `git describe`     | 生成一个可读的字符串，该字符串基于 Git 的标签系统来描述当前的提交 |

### 远程操作

| 命令              | 描述               |
| --------------- | ---------------- |
| `git remote`    | 远程仓库操作           |
| `git fetch`     | 从远程获取代码库         |
| `git pull`      | 下载远程代码并合并        |
| `git push`      | 上传远程代码并合并        |
| `git submodule` | 管理包含其他 Git 仓库的项目 |

### 文件转态

下表列出文件状态

| 状态               | 描述                                  |
| :--------------- | ----------------------------------- |
| 未跟踪(`Untracked`) | 新创建的文件最初是未跟踪的。它们存在于工作目录中            |
| 已跟踪(`Tracked`)   | 通过 `git add` 命令将文件添加到暂存区后，文件状态变为已跟踪 |
| 已修改(`Modified`)  | 对已跟踪的文件进行修改                         |
| 已暂存(`Staged`)    | 使用 `git add` 命令将修改过的文件添加到暂存区后       |
| 已提交(`Commited`)  | 使用 `git commit` 命令将暂存区的更改提交到本地仓库后   |

## 分支

Git 分支管理是 Git 强大功能之一，能够让多个开发人员并行工作，开发新功能、修复 bug 或进行实验，而不会影响主代码库

> [!tip] 
> 
> 几乎每一种版本控制系统都以某种形式支持分支，一个分支代表一条独立的开发线
> 
> 使用分支意味着你可以从开发主线上分离开来，然后在不影响主线的同时继续工作
> 

![Pasted image 20250105213538|600|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755788025732-ac8b047439e54498adb22ce7e4ee678d.png)

> [!important] 
> 
> Git 分支实际上是指向更改快照的指针
> 

### 创建和切换分支

创建新分支并切换到该分支：

```shell
git checkout -b <branchname>  # 创建分支
git checkout <branchname>  # 切换分支
```

### 查看分支

查看所有分支

```shell
git branch     # 查看本地分支
git branch -r  # 查看远程分支
git branch -a  # 查看本地和远程分支
```

### 合并分支

将其他分支合并到当前分支

```shell
git merge <branchname>
```

例如，切换到 main 分支并合并 feature-xyz 分支

```shell
git checkout main
git merge feature-xyz
```

### 删除分支

删除分支

```shell
git branch -d <branchname>  # 删除分支
git branch -D <branchname>  # 强制删除未合并分支

git push origin --delete <branchname>  # 删除远程分支
```

## Git 工作流

![Pasted image 20250105190755|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755788025732-5fd79c7390df468faf8caf044392909d.png)

### 克隆仓库

如果要参与一个已有的项目，首先需要将远程仓库克隆到本地：

```shell
git clone https://github.com/username/repo.git
cd repo
```

### 创建新分支

为了避免直接在 `main` 或 `master` 分支上进行开发，通常会创建一个新的分支

```shell
git checkout -b new-feature
```

之后，就可以在工作目录中进行代码编辑、添加文件或删除不需要的文件

### 暂存文件

将修改过的文件添加到暂存区，以便进行下一步的提交操作

```shell
git add filename
# 或者添加所有修改的文件
git add .
```

### 提交更改

将暂存区的更改提交到本地仓库，并添加提交信息

```shell
git commit -m "Add new feature"
```

### 拉去最新更新

在推送本地更改之前，最好从远程仓库拉取最新的更改，以避免冲突

```shell
git pull origin main
# 或者如果在新的分支上工作
git pull origin new-feature
```

### 推送更改

将本地的提交推送到远程仓库

```shell
git push origin new-feature
```

### 创建 Pull Request

在 GitHub 或其他托管平台上创建 Pull Request，邀请团队成员进行代码审查。PR 合并后，你的更改就会合并到主分支。

### 合并更改

在 PR 审核通过并合并后，可以将远程仓库的主分支合并到本地分支

```shell
git checkout main
git pull origin main
git merge new-feature
```

### 删除分支

如果不再需要新功能分支，可以将其删除

```shell
git branch -d new-feature
```

或者从远程仓库删除分支

```shell
git push origin --delete new-feature
```
