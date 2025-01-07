# 环境安装

## Windows

### scoop

在 Windows 上安装 [Scoop](https://scoop.sh/) 包管理器。根据官网的提示，我们使用下面的命令进行安装

```powershell
> Set-ExecutionPolicy RemoteSigned -Scope CurrentUser
> irm get.scoop.sh | iex
```

如果需要修改安装路径等配置信息，使用离线安装脚本进行安装

```powershell
# 下载安装脚本
irm get.scoop.sh -outfile 'install.ps1'
# 安装到指定目录
.\install.ps1 -ScoopDir 'C:\scoop' -ScoopGlobalDir 'C:\scoop\global' -NoProxy
```

#### 安装和卸载软件

```powershell
scoop install nodejs   # 安装
scoop uninstall nodejs  # 卸载
```

#### 更新

```powershell
scoop update --all
```

#### 查找

```powershell
scoop search neovim
```

#### 添加 buckets

buckets 可以被 scoop 识别

```powershell
scoop bucket add virson

scoop bucket add extras
```

buckets 不可以被 scoop 识别时

```poweshell
scoop bucket add java https://gitee.com/scoop-bucket/java.git
```

### GCC

在 Windows 上使用 GCC 编译器，需要安装 [msys2](https://www.msys2.org/)

```powershell
scoop install msys2
```

我们打开命令行窗口键入 `msys2` 进入环境。然后完成整个环境的安装

```shell
pacman -S --needed base-devel mingw-w64-ucrt-x86_64-toolchain mingw-w64-clang-x86_64-toolchain mingw-w64-x86_64-toolchain
```

最后，将安装好的编译器路径添加到系统 PATH 目录下并重启系统

### visual studio 2022

访问 [visual studio](https://visualstudio.microsoft.com/zh-hans/) 现在安装器。安装器启动之后，选择 `使用 C++ 的桌面开发`， 然后点击安装

![[Pasted image 20240908163108.png|900]]

等待安装完成即可

## Linux

### 安装 Linux

安装 [virtualBox](https://www.virtualbox.org/wiki/Downloads)。然后，下载 [ubuntu-virtualbox-imag](https://www.osboxes.org/ubuntu/)。**新建虚拟机**，不要选择镜像，在创建硬盘阶段 **选择现有虚拟硬盘文件**。最后启动虚拟机即可

### 安装开发工具包

```shell
# Ubuntu
sudo apt install build-essential
```

### 安装不同版本的 GCC

```shell
sudo add-apt-repository ppa:ubuntu-toolchain-r/ppa
sudo apt update
sudo apt install gcc-11 g++-11 gcc-12 g++-12 gcc-14 g++-14
```

使用 `update-alternatives` 管理 `gcc`

```shell
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-11 1
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-12 2
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-13 3
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-14 4
```

自动选择优先级为 `4` 的 `gcc`

> [!tip] GCC-14 支持 C23 标准

## CLion

[CLion](https://www.jetbrains.com/zh-cn/clion/) 是一个适用于 C 和 C++ 的集成开发环境。它默认提供了 GCC 编译器，也可以使用 **远程编译器**。下载 [Jetbrains Toolbox](https://www.jetbrains.com/zh-cn/toolbox-app/)，然后安装 Clion

