# 包管理

在 [[Python：模块和包]] 中我们介绍了在 Python 中如何组织一个模块和包的所有源文件。当我们想要共享自己编写的 Python 库，就需要将其 **打包**，然后发布到网络。这样就可以供其他人使用了。

> [!tip] 
> 
> Python 社区提供了许多第三方库，这些第三方库可以在公共的模块存储中心 [pypi](https://pypi.org/) 查找并下载。这个存储库中包含从简单的 `Hello， World` 实现到高级深度学习库。
> 

## 安装 Python 包

### 安装 pip

安装 Python 解释器的时候，默认连同 `pip` 一起安装。如果因为特殊情况未能安装 `pip`，可以使用下面方法进行安装

```shell
python -m ensurepip --upgrade
```

或者使用下面的脚本 [git-pip](https://github.com/pypa/get-pip) 中提供的脚本安装 `pip`

请注意：虽然`pip`单独从预先构建的二进制档案安装就足够了，但最新的`setuptools`和`wheel` 有助于确保我们也可以从源码安装

```shell
python3 -m pip install --upgrade pip setuptools wheel
```

### 安装第三方包

当我们需要安装第三方包时，使用如下命令即可安装

```shell
pip install <package-name>
```

默认情况下，Python 会从  [pypi](https://pypi.org/) 中搜索 `<package-name>` 并安装到 `<PATH TO Python>/Lib/site-packages` 目录中。

### 配置国内镜像源

由于网络环境的原因，从国外下载第三方包会很慢。通常，我们会可以使用国内的镜像源；例如 [mirrors.tuna.tsinghua.edu.cn](https://mirrors.tuna.tsinghua.edu.cn/help/pypi/) 或 [mirrors.huaweicloud.com](https://mirrors.huaweicloud.com/mirrorDetail/5ea14ecab05943f36fb75ee6?mirrorName=python&catalog=tool)。使用下面的命令设置 `pip` 默认使用的镜像源

```shell
pip config set global.index https://repo.huaweicloud.com/repository/pypi
pip config set global.index-url https://repo.huaweicloud.com/repository/pypi/simple
pip config set global.trusted-host repo.huaweicloud.com
```

或者，在 pip 的配置文件中(用户根目录下的：`~/.config/pip/pip.conf` 或者`C:\Users\<UserName>\pip\pip.ini`）, 可以配置如下内容：

```
[global]
index-url = https://repo.huaweicloud.com/repository/pypi/simple
index = https://repo.huaweicloud.com/repository/pypi
trusted-host = repo.huaweicloud.com
extra-index-url = https://mirrors.tuna.tsinghua.edu.cn/pypi/web/simple
```

### 创建虚拟环境

Python **虚拟环境** 允许将 Python 包安装项目依赖 **相互隔离位置**，而不是全局安装。如果将软件包及其依赖项安装到同一全局环境 **可能会导致版本冲突并破坏作系统对 Python 软件包的依赖关系**

> [!example] 
> 
> 假设现在有一个应用程序需要 `libfoo-v1`，但另一个应用程序需要 `libfoo-v2` 。那么如何同时使用这两个应用程序？
> 
> 如果我们把所有东西都安装到 `/usr/lib/python3.6/site-packages` 中（或者你的平台的标准位置），很容易陷入无意中升级一个不应该升级的应用程序的情况
> 

为了解决这些问题，Python 引入了虚拟环境这一概念。它们有自己的安装目录，并且不与其他虚拟环境共享库。

目前，有两种常用工具可用于创建 Python 虚拟环境：**venv** 和 **virtualenv**

> [!tip] 
> 
> 默认情况下，`venv` 在 Python 3.3 及更高版本中可用，并在 Python 3.4 及更高版本中将 `pip` 安装到创建的虚拟环境中
> 
> `virtualenv` 需要单独安装，但支持 Python 2.7+ 和 Python 3.3+，并且默认情况下，`pip`、`setuptools` 和 `wheel` 始终安装在创建的虚拟环境中（无论 Python 版本如何）。

我们主要使用 `venv` 创建 Python 虚拟环境。使用方式如下

```shell
python3 -m venv <DIR>      # 创建虚拟环境
source <DIR>/bin/activate  # 激活虚拟环境
```

### 其他安装方式

`pip` 支持从 VCS 中安装，例如从 `git` 安装

```shell
python3 -m pip install -e SomeProject @ git+https://git.repo/some_pkg.git          # from git
python3 -m pip install -e SomeProject @ hg+https://hg.repo/some_pkg                # from mercurial
python3 -m pip install -e SomeProject @ svn+svn://svn.repo/some_pkg/trunk/         # from svn
python3 -m pip install -e SomeProject @ git+https://git.repo/some_pkg.git@feature  # from a branch
```

`pip` 还支持从本地文件安装

```shell
python3 -m pip install ./downloads/SomeProject-1.0.4.tar.gz
```

如果不希望查找 `pypi`，使用如下命令

```shell
python3 -m pip install --no-index --find-links=file:///local/dir/ SomeProject
python3 -m pip install --no-index --find-links=/local/dir/ SomeProject
python3 -m pip install --no-index --find-links=relative/dir/ SomeProject
```

## 管理项目依赖

之前，我们介绍了如何安装 Python 包。但是，即使对于自己的个人项目，以交互方式运行这些命令也会变得乏味，并且在尝试为具有多个贡献者的项目自动设置开发环境时，事情会变得更加困难

下面我们学习使用 [pipenv](https://pipenv.pypa.io/en/latest/index.html) 管理应用程序的依赖项。由于 Python 可以用于许多不同的目的，管理依赖项的确切方式可能会根据您决定发布软件的方式而改变。此处提供的指南最直接地适用于网络服务（包括 Web 应用程序）的开发和部署，但也非常适合管理任何类型项目的开发和测试环境。

### 安装 Pipenv

`pipenv` 是 Python 项目的依赖项管理器。它的工作方式类似于 Node.js 的 npm 包管理器。虽然单独的 `pip` 通常足以满足个人使用，但建议将 `pipenv` 用于协作项目，因为它是一个更高级别的工具，可以简化常见用例的依赖项管理。

使用如下命令安装 `pipenv`

```shell
python3 -m pip install --user pipenv
```

### 安装项目依赖

**`pipenv` 基于每个项目管理依赖项**。要安装 Python 包，请切换到项目的目录并运行：

```shell
cd myproject
pipenv install requests
```

`pipenv` 将安装 `requests` 库并在项目目录中为创建一个 `Pipfile`。`Pipfile` 用于跟踪项目需要哪些依赖项，以防重新安装它们

例如当您与他人共享您的项目时。您应该得到类似于这样的输出（尽管显示的确切路径会有所不同）

```
Creating a Pipfile for this project...
Creating a virtualenv for this project...
Using base prefix '/usr/local/Cellar/python3/3.6.2/Frameworks/Python.framework/Versions/3.6'
New python executable in ~/.local/share/virtualenvs/tmp-agwWamBd/bin/python3.6
Also creating executable in ~/.local/share/virtualenvs/tmp-agwWamBd/bin/python
Installing setuptools, pip, wheel...done.

Virtualenv location: ~/.local/share/virtualenvs/tmp-agwWamBd
Installing requests...
Collecting requests
  Using cached requests-2.18.4-py2.py3-none-any.whl
Collecting idna<2.7,>=2.5 (from requests)
  Using cached idna-2.6-py2.py3-none-any.whl
Collecting urllib3<1.23,>=1.21.1 (from requests)
  Using cached urllib3-1.22-py2.py3-none-any.whl
Collecting chardet<3.1.0,>=3.0.2 (from requests)
  Using cached chardet-3.0.4-py2.py3-none-any.whl
Collecting certifi>=2017.4.17 (from requests)
  Using cached certifi-2017.7.27.1-py2.py3-none-any.whl
Installing collected packages: idna, urllib3, chardet, certifi, requests
Successfully installed certifi-2017.7.27.1 chardet-3.0.4 idna-2.6 requests-2.18.4 urllib3-1.22

Adding requests to Pipfile's [packages]...
```

### 运行项目

现在，假设使用 `requests` 库编写了如下代码

```shell title:main.py
import requests

response = requests.get('https://httpbin.org/ip')

print('Your IP is {0}'.format(response.json()['origin']))
```

想要运行 `pipenv` 管理的依赖的项目，就需要使用如下命令

```shell
pipenv run python main.py
```

> [!tip] 
> 
> 使用 `pipenv run` 可确保已安装的软件包可用于脚本。还可以生成一个新的 `shell`，确保所有命令都可以使用 `pipenv shell` 访问已安装的软件包
> 

## 打包 Python 项目

下面我们以链表的实现来介绍在 Python 中如何打包并发布。下面是链表 `list` 的目录结构

```
list
├── READNE.md
└── src
    └── list
        ├── __init__.py
        └── linked_list.py
```

### 创建包文件

现在，我们将添加用于准备要分发的项目的文件。完成后，项目结构将如下所示

```
list/
├── LICENSE.txt     # 许可协议
├── pyproject.toml  # 构建配置文件
├── READNE.md       # README
├── src             # 项目源码
│   └── list
│       ├── __init__.py
│       └── linked_list.py
└── tests           # 测试目录：目前留空
```

### 选择构建后端

像 [`pip`](https://pip.pypa.io/en/stable/) 和 [`build`](https://build.pypa.io/en/stable/index.html) 这样的工具实际上并没有将你的源代码转换为分发包；转换为分发包的作业由 **Build 后端** 执行。Build 后端确定项目将如何指定其配置，包括元数据（有关项目的信息，例如，PyPI 上显示的名称和标签）和输入文件。构建后端具有不同级别的功能，例如它们是否支持构建扩展模块，应该选择适合我们的需求和偏好的后端。

> [!tip] 
> 
> 常见的构建后有 [flit’s flit-core](https://packaging.python.org/en/latest/key_projects/#flit), [hatch’s hatchling](https://packaging.python.org/en/latest/key_projects/#hatch), [Maturin](https://packaging.python.org/en/latest/key_projects/#maturin), [meson-python](https://packaging.python.org/en/latest/key_projects/#meson-python), [scikit-build-core](https://packaging.python.org/en/latest/key_projects/#scikit-build-core), 和 [Setuptools](https://packaging.python.org/en/latest/key_projects/#setuptools).
> 

`pyproject.toml` 告诉构建前端工具（如 `pip` 和 `build`）要用于您的项目。例如，使用 Hatchling 后端构建

```toml
[build-system]  
requires = ["hatchling >= 1.26"]  # 构建包列表，构建前端会自动安装它们
build-backend = "hatchling.build" # 构建前端将执行构建的 Python 对象的名称
```

### 配置元数据

打开 `pyproject.toml` 并输入以下内容。更改名称以包含您的用户名；这可确保您具有唯一的包名称，该名称不会与其他人按照本教程上传的包冲突。

```toml
[project]
name = "example_package_YOUR_USERNAME_HERE"  # 分发包的名称
version = "0.0.1"  # 当前版本
authors = [
  { name="Example Author", email="author@example.com" },
]  # 作者信息
description = "A small example package"  # 分发包的描述信息
readme = "README.md"  # readme 文件
requires-python = ">=3.9"  # 最低 Python 版本
classifiers = [
    "Programming Language :: Python :: 3",
    "Operating System :: OS Independent",
]  # 分类信息
license = "MIT"  # 许可协议
license-files = ["LICEN[CS]E*"]  # 相对于 pyproject.toml 的路径

[project.urls]
Homepage = "https://github.com/pypa/sampleproject"
Issues = "https://github.com/pypa/sampleproject/issues"
```

### 创建 README

打开 `README.md` 并输入以下内容。如果您愿意，您可以对其进行自定义

```md
# list: Linked list for Python
```

### 创建 LICENSE

上传到 Python Package Index 的每个包都必须包含许可证。这会告知安装您的包的用户他们可以使用您的包的条款。有关选择许可证的帮助，请参阅 [choosealicense]( https://choosealicense.com/)。选择许可证后，打开 `LICENSE` 并输入许可证文本。例如，如果您选择了 MIT 许可证

```
Copyright (c) 2018 The Python Packaging Authority

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```

### 生成分发归档

下一步是为该包生成分发包。这些是上传到 Python 包索引的存档，可以由 `pip` 安装。确保您已安装最新版本的 `PyPA build` 版本

```shell
python3 -m pip install --upgrade build
```

现在从 `pyproject.toml` 所在的同一目录运行此命令

```shell
python3 -m build
```

此时会输出很多内容，完成后应在 `dist` 目录中生成两个文件

```
(list) python$ tree list/
list/
├── dist
│   ├── list-1.0.0-py3-none-any.whl
│   └── list-1.0.0.tar.gz
├── LICENSE.txt
├── pyproject.toml
├── README.md
├── READNE.md
├── src
│   └── list
│       ├── __init__.py
│       └── linked_list.py
└── tests
```

### 上传分发存档

您需要做的第一件事是在 [TestPyPI](https://test.pypi.org/account/register/) 上注册一个帐户，该帐户是用于测试和试验的包索引的单独实例。要安全地上传您的项目，您需要一个 PyPI API 令牌。在 [api-tokens](https://test.pypi.org/manage/account/#api-tokens) 创建一个，将 `"Scope"` 设置为 `"Entire account"`。在复制并保存令牌之前，**请勿关闭页面 - 您将不会再次看到该令牌**。如果使用 `twine` 上次分发包，可以将 `token` 保存在 `$HOME/.pypirc` 中

```toml
[testpypi]
  username = __token__  # 用户名设置为 __token__
  password = token
```

现在，已完成注册，可以使用 `twine` 上传分发包。您需要安装 `Twine`：

```shell
python3 -m pip install --upgrade twine
```

安装后，运行 Twine 上传 dist 下的所有存档

```shell
python3 -m twine upload --repository testpypi dist/*
```

执行上次指令将输出如下内容

```
Uploading distributions to https://test.pypi.org/legacy/
Uploading list-1.0.0-py3-none-any.whl
100% ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━ 6.3/6.3 kB • 00:03 • ?
Uploading list-1.0.0.tar.gz
100% ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━ 5.7/5.7 kB • 00:00 • ?

View at:
https://test.pypi.org/project/list/1.0.0/
```

### 安装验证

现在可以使用 `pip` 安装软件包并验证它是否正常工作。创建虚拟环境并从 `TestPyPI` 安装包

```shell
python3 -m pip install --index-url https://test.pypi.org/simple/ --no-deps list
```

到此，我们就已经可以分发自己的 Python 项目了

> [!seealso] 
> 
> 有任何其他需要，请参考 https://packaging.python.org/en/latest/
>  

