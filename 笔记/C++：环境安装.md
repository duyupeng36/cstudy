# 环境安装

目前最流行的 C++ 编译器有两种，分别是 [MSVC(Microsoft Visual C++)](https://visualstudio.microsoft.com/zh-hans/downloads/) 和 [GCC(the GNU Compiler Collection)](https://gcc.gnu.org/)。下面我们介绍如何安装这两种编译器

## MSVC

MSVC 编译器在 [Visual Studio](https://visualstudio.microsoft.com/zh-hans/downloads/) 中提供，我们需要现在 Visual Studio 安装程序进行安装。访问 Visual Studio 官方网站，我们可以看见如下页面

![image.png|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755793161008-967a0905206a458a8cdc72f26a5453bc.png)

Visual Studio 提供了三个版本，作为学习用途，我们只需要选择社区版本即可。点击免费下载，等待安装程序下载完成

![image.png|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755793315274-c7d0b0083123434f91ffa54f7181152f.png)

双击该文件即可触发安装，一会之后就会展示如下界面

![image.png|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755793384861-d46414eaba224ecfb95778d5bab2fd5d.png)

点击继续，会出现如下界面

![image.png|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755793436200-dbf58ca599b64d12be8cc10044fcf77d.png)

此时会下载一些内容，等待下载结束会弹出如下页面

![image.png|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755793497758-fa83aa700d7d400a8e2ded9db77b9b9e.png)

滑动窗口，找到使用 C++ 的桌面开发选项，选择后点击右下角的安装按钮

![image.png|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755793603836-935e0650830b4aeca640d726a9fa3cef.png)

此时，就开始下载 MSVC 编译器等待即可

![image.png|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755793700441-3b3eb929de7f4c05993703e99cd547f1.png)

## GCC

GCC 编译器主要是应用在 Unix/Linux 系统中，在Windows系统上需要使用 [msys2](https://www.msys2.org/)。我们可以使用 scoop 安装 msys2 

```powershell
$ scoop install msys2  
```

等待安装完成后，只需要输入 `msys2` 就可以进入 msys2 环境中，然后就可以安装 GCC 

```powershell
$ msys2
```


然后使用如下命令安装 GCC

```powershell
$ pacman -S --needed base-devel mingw-w64-ucrt-x86_64-toolchain mingw-w64-clang-x86_64-toolchain mingw-w64-x86_64-toolchain
```

到此，我们的 GCC 编译器就安装完成了。

## hello world 程序





