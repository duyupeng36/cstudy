# 日志处理

**日志** 是指系统所指定 **_对象的某些操作_** 和其 **_操作结果_** 按时间有序的集合。通常记录在一个文件中，称为 **Log 文件**(**日志文件**)

> [!tip] 
> 
> 通常，日志文件采用 `.log` 作为扩展名，日志文件的格式并没有任何要求
> 

日志文件为服务器、工作站、防火墙和应用软件等 IT 资源相关活动记录 **必要的**、**有价值** 的信息，这对系统监控、查询、报表和安全审计是十分重要的。日志文件中的记录可提供以下用途
+ **监控系统资源**
+ **审计用户行为**
+ 对可疑行为进行告普
+ 确定入侵行为的范围
+ 为恢复系统提供帮助
+ 生成调查报告
+ 为打击计算机犯罪提供证据来源

在 Python 中，通过标准库 `logging` 提供了强大的日志处理能力。

## 日志级别

> [!attention] 
> 
> 之前，我们只是简单的使用 `print()` 函数输出程序的执行行为，这本质上就是一种日志。然而，这样记录的消息有一个缺点：**消息没有等级**。
> 

日志是用来记录程序在 **运行过程中发生的状况**，在程序开发过程中添加日志能够帮助我们了解程序运行过程中发生了哪些状况。请注意，这些 **状况也有轻重之分**。

在 Python 中，标准库 `logging` 将日志分为了 $7$ 个级别。下表列出了这些日志级别

| 级别         | 数字   | 描述                      |
| :--------- | :--- | :---------------------- |
| `CRITICAL` | $50$ | 严重错误，程序不能继续执行           |
| `ERROR`    | $40$ | 严重问题，程序的某些功能不能正常hi下     |
| `WARNING`  | $30$ | 意外事件，程序仍按预期执行(**默认级别**) |
| `INFO`     | $20$ | 确认程序按预期执行               |
| `DEBUG`    | $10$ | 细节信息，用于诊断问题             |
| `NOTSET`   | $0$  | 未设置级别                   |

在标准库 `logging` 中，提供了产生对应级别的消息的函数，如下表所示

| 函数                  | 描述                  |
| :------------------ | :------------------ |
| `logging.debug()`   | 产生 `DEBUG` 级别的消息    |
| `logging.info()`    | 产生 `INFO` 级别的消息     |
| `logging.warning()` | 产生 `WARNING` 级别的消息  |
| `loging.error()`    | 产生 `ERROR` 级别的消息    |
| `loging.critical()` | 产生 `CRITICAL` 级别的消息 |

例如，如下例程用于输出 $5$ 条日志

```python
import logging

logging.debug("debug 级别的日志")
logging.info("info 级别的日志")
logging.warning("warning 级别的日志")
logging.error("error 级别的日志")
logging.critical("critical 级别的日志")
```

但是，执行上述例程时，输出的结果如下

```
WARNING:root:warning 级别的日志
ERROR:root:error 级别的日志
CRITICAL:root:critical 级别的日志
```

> [!attention] 
> 
> 请注意：这里只有 $3$ 条日志被输出
>  
> 出现上述情况的原因在于，记录日志是由 **日志记录器(logger)** 完成，并且日志记录只会记录 **消息等级(`mLevel`)** 大于或等于 **logger 级别(`logger.Level`)** 的消息。`logging` 模块提供了一个默认的日志记录器，称为 `root` 日志记录器 ，它的 `logger.Level` 是 `WARNING`。
> 
> 上表列出的函数本质上都是在调用根日志记录器对应的方法。因此，上述例程只输出了级别在 `WARNGING` 以上的日志
> 

## 配置根记录器

`logging.basicConfig(**kwargs)` 函数会使用默认的 **格式化器** 创建一个 **流式处理器** 并将其加入到 **根日志记录器** 中，从而为日志记录系统提供默认基本配置。如果根日志记录器已配置了处理器则此函数将不执行任何操作，除非关键字参数  `force`  被设为 `True`

`logging.basicConfig()` 支持以下关键字参数

| 格式         | 描述                                                                                                                                                      |
| ---------- | ------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `filename` | 日志被写入的文件                                                                                                                                                |
| `filemode` | 打开文件的模式，默认为 `"a"` 模式                                                                                                                                    |
| `format`   | 日志格式字符串                                                                                                                                                 |
| `datefmt`  | 日期格式字符串                                                                                                                                                 |
| `style`    | 日志格式化风格。 `'%'`(C 风格，**默认的**), `'{'`(`format`风格) 或 `'$'`(模板)                                                                                             |
| `level`    | 设置根记录器级别为指定的级别                                                                                                                                          |
| `stream`   | 流式处理器 [`StreamHandler`](https://docs.python.org/zh-cn/3.13/library/logging.handlers.html#logging.StreamHandler "logging.StreamHandler")。与 `filename` 冲突 |
| `handlers` | 日志处理器，与 `filename` 和 `stream` 冲突                                                                                                                        |
| `force`    | 强制执行 `basicConfig` 配置                                                                                                                                   |
| `encoding` | 指定文本编码方式。与 `filename` 一起使用                                                                                                                              |
| `errors`   | 编码错误处理。与 `filename` 一起使用                                                                                                                                |

### 日志格式化

下表列出了日志格式化支持的属性

| C风格               | format风格        | 模板               | 描述                |
| :---------------- | :-------------- | :--------------- | :---------------- |
| `%(asctime)s`     | `{asctime}`     | `${asctime}`     | 人类易读的时间           |
| `%(created)f`     | `{created}`     | `${created}`     | 日志消息创建的时间戳        |
| `%(filename)s`    | `{filename}`    | `${filename}`    | 文件名               |
| `%(funcName)s`    | `{funcNmae}`    | `${funcNmae}`    | 函数名               |
| `%(levelname)`    | `{levelname}`   | `${levelname}`   | 消息级别的名字           |
| `%(levelno)s`     | `{levelno}`     | `${levelno}`     | 消息级别的数字           |
| `%(lineno)d`      | `{lineno}`      | `${lineno}`      | 发出日志记录调用的行号       |
| `%(message)s`     | `{message}`     | `${message}`     | 日志消息              |
| `%(module)s`      | `{module}`      | `${module}`      | 模块名               |
| `%(msecs)d`       | `{msecs}`       | `${msecs}`       | 日志创建时间的毫秒部分       |
| `%(name)s`        | `{name}`        | `${name}`        | 日志记录器的名字          |
| `%(pathname)s`    | `{pathname}`    | `${pathname}`    | 日志记录调用的源文件名       |
| `%(process)d`     | `{process}`     | `${process}`     | 进程 ID             |
| `%(processName)s` | `{processName}` | `${processName}` | 进程名               |
| `%(thread)d`      | `{thread}`      | `${thread}`      | 线程 ID             |
| `%(threadName)d`  | `{threadName}`  | `${threadNmae}`  | 线程名               |
| `%(taskName)`     | `{taskName}`    | `${taskName}`    | `asyncio.Task` 名称 |

> [!attention] 
> 
> 对于日志而言，最重要的就是 **什么时间?** **什么位置?** **发送了什么事?**。因此，我们通常只会配置 `%(asctime)s %(pathname)s:%(lineno)d %(message)s` 即可
> 

```python
import logging  
  
logging.basicConfig(
	level=logging.INFO, 
	format="%(asctime)s - %(pathname)s:%(lineno)d %(message)s", 
	datefmt="%Y-%m-%d %H:%M:%S"
)  
  
logging.debug("debug 级别的日志")  
logging.info("info 级别的日志")  
logging.warning("warning 级别的日志")  
logging.error("error 级别的日志")  
logging.critical("critical 级别的日志")
```

上述代码执行的结果为

```
2025-05-07 20:31:18 - C:\Users\duyup\Documents\github.com\duyupeng36\roadmap\python\test.py:6 info 级别的日志
2025-05-07 20:31:18 - C:\Users\duyup\Documents\github.com\duyupeng36\roadmap\python\test.py:7 warning 级别的日志
2025-05-07 20:31:18 - C:\Users\duyup\Documents\github.com\duyupeng36\roadmap\python\test.py:8 error 级别的日志
2025-05-07 20:31:18 - C:\Users\duyup\Documents\github.com\duyupeng36\roadmap\python\test.py:9 critical 级别的日志
```

### 输出到文件

```python
import logging

logging.basicConfig(
    level=logging.INFO,
    format="%(asctime)s - %(pathname)s:%(lineno)d %(message)s",
    datefmt="%Y-%m-%d %H:%M:%S",
    filename="test.log"  # 将日志输出到文件
)

logging.debug("debug 级别的日志")
logging.info("info 级别的日志")
logging.warning("warning 级别的日志")
logging.error("error 级别的日志")
logging.critical("critical 级别的日志")
```

> [!attention] 
> 
> 请注意，一旦设置了 `filename`，那么输出到控制台就不在生效。也就是说，一旦设置了 `filename`，则 `stream` 就不会生效
> 

## 模块化组件

日志库采用模块化方法，并提供几类组件：**记录器(Logger)**、**处理器(Handler)**、**过滤器(Filter)** 和 **格式器(Formater)**

| 组件  | 描述                      |
| :-- | :---------------------- |
| 记录器 | 暴露了应用程序代码直接使用的接口        |
| 处理器 | 将日志记录（由记录器创建）发送到适当的目标   |
| 过滤器 | 提供了更细粒度的功能，用于确定要输出的日志记录 |
| 格式器 | 指定最终输出中日志记录的样式          |

### 记录器

在标准库 `logging` 中提供了一个 `Logger` 类，它用于实例化一个记录器。在实例化时，我们需要提供记录器的 **名字** 和 记录器需要记录日志的 **最低等级**

```python

```










