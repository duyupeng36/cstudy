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

### 日志记录器

在标准库 `logging` 中提供了一个 `Logger` 类，它用于实例化一个记录器。在实例化时，我们需要提供记录器的 **名字** 和 记录器需要记录日志的 **最低等级**

例如，如下代码就是通过实例化 `Logger` 类得到的两个记录器

```python
import logging


logger1 = logging.Logger("__main__", logging.INFO)
logger2 = logging.Logger("__main__", logging.INFO)
print(logger1 is logger2)  # False
```

> [!attention] 
> 
> 上述代码中的两个 `Logger` 类的实例是不同的。但是，对于同一个应用程序中具有相同名字的 `logger` 应该是同一个对象
> 

因此，通常我们不使用 `Logger()` 实例化，而是使用函数 `logging.getLogger(name)` 获取一个名字为 `name` 的 `Logger` 实例对象。

```python
logger3 = logging.getLogger("__main__")
logger4 = logging.getLogger("__main__")
print(logger3 is logger4)  # True
```

#### getLogger() 函数

下面我们探究一下 `getLogger()` 函数的实现。在 `logging` 包中，`getLogger()` 函数定义如下

```python title:logging.__init__.py
def getLogger(name=None):
    """
    Return a logger with the specified name, creating it if necessary.

    If no name is specified, return the root logger.
    """
    if not name or isinstance(name, str) and name == root.name:
        return root
    return Logger.manager.getLogger(name)
```

这函数就是根据参数 `name` 从 `Logger.manager` 对象中获取 `Logger()` 实例并返回；如果没有提供 `name` 参数，则返回 `root logger` 。`root logger` 和 `Logger.manager` 是什么？继续阅读代码

```python title:logging.__init__.py
class RootLogger(Logger):
    def __init__(self, level):
        """
        Initialize the logger with the name "root".
        """
        Logger.__init__(self, "root", level)

    def __reduce__(self):
        return getLogger, ()
        

root = RootLogger(WARNING)
Logger.root = root
Logger.manager = Manager(Logger.root)
```

到此，我们就明白了 `getLogger()` 函数的实现。下面我们来看 `Manager()` 对象的 `getLogger()` 方法，它是根据 `name` 获取 `Logger()` 实例的核心代码

```python
def getLogger(self, name):
	rv = None
	# 检查名字是否是 str 类型
	if not isinstance(name, str):
		raise TypeError('A logger name must be a string')
	# 加锁，避免并发执行出现竞争条件
	with _lock:
		# 检查 name 是否在存放 logger 对象的字典(loggerDict) 中 
		if name in self.loggerDict:
			# 名字为 name 的 Logger 对象已存在
			# 获取名字为 name 的 Logger 对象
			rv = self.loggerDict[name]
			if isinstance(rv, PlaceHolder):
				ph = rv
				rv = (self.loggerClass or _loggerClass)(name)
				rv.manager = self
				self.loggerDict[name] = rv
				self._fixupChildren(ph, rv)
				self._fixupParents(rv)
		else:
			# 名字为 name 的 Logger 对象不存在
			# 实例化一个 Logger 实例
			rv = (self.loggerClass or _loggerClass)(name)
			rv.manager = self
			# 将 Logger 实例放入字典
			self.loggerDict[name] = rv  
			self._fixupParents(rv)
	return rv
```

#### Logger 之间的父子关系

在阅读 `Manager.getLogger()` 方法时，我们发现了两个方法 `Manager._fixupParents()` 和 `Manager._fixupChildren()`。

下面我们首先来阅读 `Manager._fixupParents()` 方法的代码：这个方法用于修复或设置 `logger` 的父 `logger`

```python
def _fixupParents(self, alogger):
	# 当前 logger 的名字
	name = alogger.name
	# 在名字中查找分隔字符 `"."`
	i = name.rfind(".")
	rv = None
	# 找到了 `"."` 并且 rv 未被设置
	while (i > 0) and not rv:
		# 获取名字的前 i 个字符
		substr = name[:i]
		if substr not in self.loggerDict:
			# 名字的前 i 个字符不在 logger 字典中
			# 添加一个 PlaceHolder 实例(本质就是一个占位的对象)
			self.loggerDict[substr] = PlaceHolder(alogger)
		
		else:
			# 名字的前 i 个字符在 logger 字典中
			# 获取 对象 obj
			obj = self.loggerDict[substr]
			if isinstance(obj, Logger):
				# 如果 obj 是 Logger 的实例，则设置 rv 
				rv = obj
			else:
				# 如果 obj 不是 Logger 的实例，
				# 则断言 obj 是否是 PlaceHolder。
				# 如果不是则抛出错误
				assert isinstance(obj, PlaceHolder)
				# 在 ph 中保存其子 Logger 
				obj.append(alogger)
		# 查找下一个字符 `"."` 的位置
		i = name.rfind(".", 0, i - 1)
	if not rv:
		# 最终 rv 没有被设置，则将其设置为 root
		rv = self.root
	# 将 logger 的父logger 设置为 rv
	alogger.parent = rv
```

> [!important] 
> 
> 通过阅读源码，我们知道 `logging` 包中维护 `Logger` 实例之间的父子关系是通过 **点分隔名字** 的约定进行维护的
> 
> 例如，名字为 `"a.b.c"` 这样的 `logger`，其父`logger` 的名字是 `"a.b"`；而 名字为 `"a.b"` 的 `logger` 的父 `logger` 的名字是 `"a"`。最后，名字为 `"a"` 的 `logger` 的父 `logger` 是 `root`
> 

显然，当我们直接使用 `"a.b.c"` 这样的名字调用 `getLogger()` 时，就会创建名字为 `"a.b"` 和 `"a"` 的 `PlaceHolder` 实例用于占位

当我们下一次以 `"a.b"` 或 `"a"` 这样的名字调用 `getLogger()` 时，在检查 `loggerDict` 时，就会获取到 `PlaceHolder` 的实例对象；此时，就会创建一个全新的 `Logger` 实例，并加入到 `loggerDict` 中。

用于对应名字的 `Logger` 实例已经创建，原来用于占位 `PlaceHolder` 实例就必须替换为新的 `Logger` 实例，从而需要修改父子关系。我们已经知道如何设置和修改 `Logger` 的父亲，下面我们来看 `Manager._fixupChildren()` 方法，该方法用于修改 `Logger` 对象的儿子

```python
    def _fixupChildren(self, ph, alogger):
        # 当前 logger 的名字
        name = alogger.name
        # 名字的长度
        namelen = len(name)
        for c in ph.loggerMap.keys():
            #The if means ... if not c.parent.name.startswith(nm)
            if c.parent.name[:namelen] != name:
                alogger.parent = c.parent
                c.parent = alogger
```

#### Logger's Level

我们知道，每一个 `Logger` 实例都有对应的级别，称为 `Logger's Level`。这个级别决定了 `Logger` 将会处理日志的最低级别。

> [!tip] 
> 
> 换句话说，如果日志级别(`Message's Level`) 低于了 `Logger's Level`，那么该 `Logger` 实例就会忽略该日志
> 

当我们使用 `getLogger()` 创建 `Logger` 实例时，我们并未提供 `Logger's Level`，那么这个 `Logger` 实例的 `level` 属性显然就是 `NOTSET(0)`

```python
import logging


alogger = logging.getLogger("a")

print(alogger.level)  # 0
```

我们使用 `Logger` 实例的方法 `Logger.setLevel(level)` 设置 `Logger` 实例的 `Logger's Level`

```python
import logging


alogger = logging.getLogger("a")

alogger.setLevel(logging.ERROR)

print(alogger.level)  # 40
```

> [!attention] 
> 
> 方法 `setLevel()` 接收任意整数，但是通常我们使用 `logging` 包提供的 **日志等级常量**
> 

#### Logger's Level 的层次

我们刚使用 `logging.getLogger(name)` 创建出一个名字为 `name` 的 `Logger` 实例时，它的 Logger's Level 为 `NOTSET`，即是最低的日志等级。

按照我们之前介绍的，此时的 `Logger` 实例可以将任何等级的日志输出。如下所示代码执行是并未表现出这样的情况

```python
import logging


alogger = logging.getLogger("a")

print(alogger.level)  # 0

alogger.info("info message")
alogger.debug("debug message")
alogger.warning("warnging message")
```

上述代码执行时，最终只有一个 `warning` 等级的消息输出。下面我们追踪 `Logger.warning()` 方法的实现，探究 `Logger's Level` 是如何确定的

```python
    def warning(self, msg, *args, **kwargs):
	    # isEnabledFor(WARNING) 方法
	    # 用于检查当前 Logger 对象是否能处理 WARNING 消息
        if self.isEnabledFor(WARNING):
	        # 能处理，就处理
            self._log(WARNING, msg, args, **kwargs)
        # 不能处理，则忽略
```

现在，我们重点去阅读 `Logger.isEnabledFor(LEVEL)` 方法

```python
def isEnabledFor(self, level):
	# 检查当前 Logger 是否被禁用
	if self.disabled:
		# 如果 Logger 被禁用，返回 False，从而禁止处理日志
		return False

	try:
		# 尝试获取当前 level 能够通过
		# 即，获取一个 bool 值，True 代表能够处理日志；False 代表不能处理日志
		return self._cache[level]
	except KeyError:
		# 如果没有缓存，就尝试获取 Logger 的有效 Level
		with _lock:
			if self.manager.disable >= level:
				# 如果 Logger 管理器的 disable 属性高于当前 level，
				# 则禁止当前 logger 处理日志
				is_enabled = self._cache[level] = False
			else:
				# 否则，检查当前 level 是否高于 Logger 的有效 level
				is_enabled = self._cache[level] = (
					level >= self.getEffectiveLevel()
				)
		return is_enabled
```

> [!tip] 
> 
> `Logger.disabled` 属性只有记录器配置代码才能修改，对于用户而言，这个属性是只读的
> 
> `Manager.disable` 控制 `Manager` 对象管理的 `Logger` 实例能够处理的消息等级，默认是 `NOTSET(0)`
> 

`Logger` 想要处理日志，就需要知道自己的 **有效** `Logger's Level`。这个值有方法 `Logger._getEffectiveLevel()` 返回，我们继续阅读该方法

```python
def getEffectiveLevel(self):
	logger = self
	while logger:
		if logger.level:
			return logger.level
		logger = logger.parent
	return NOTSET
```

这个方法很简单，就像从当前 `Logger` 开始，逐步查找其父亲。只要找到了一个 `Logger's Level` 的值不是 `NOTSET` 时就返回。

> [!summary] 
> 
> 综上所述，我们稍做总结：一条日志被记录器记录需要比较 **日志消息等级(`Message's Level`)** 和 **管理器等级(`Manager's disable`)** 以及 **记录器的有效等级(`Logger's Efective Level`)**。只要当当前消息的等级满足下列关系时，日志记录器才会记录当前消息
> 
> $\text{Messagel's Level} \ge \text{Manager's disable}$ 并且 $\text{Messagel's Level} \ge \text{Logger's Efective Level}$
> 
> 简单来说，就是 **消息等级必须大于或等于管理器等级，并且还要大于或等于记录器的有效等级**。 由于管理器等级默认设置为 $0$，并且通常不会修改；我们可以简单的理解为 **消息等级必须大于或等于记录器的有效等级**
> 
> 如果记录器有自己的 `Logger's Level`，那么有效等级就是这个值。否则，就去查找该 `Logger` 的父亲链上的 `Logger's Level`
> 

### 日志处理器

日志记录器只是向应用程序提供接口，日志最终被记录是使用 **日志处理器(`Handler`)**。`Logger` 将日志消息发送给 **日志处理器**，它决定将日志消息发送到那个位置。例如，在开篇遇见的 `StreamHandler` 用于将日志消息发送给控制台；而 `FileHandler` 用于将日志发送给文件

> [!attention] 
> 
> 请注意：日志处理器也有自己的 `Handler's Level`，记录器发送过来的消息日志等级低于 `Handler's Level` 时，`Handler` 会忽略该日志
> 

此外，`Handler` 持有 **日志格式器(`Formater`)** 和 **日志过滤器(`Filter`)**。当消息从 `Logger` 发送到  `Handler` 时，`Handler` 会调用 `Formater` 格式化日志，并且调用 `Filter` 过滤一些我们不需要的日志

> [!important] 
> 
> 也就是说，`Logger` 一定会持有 `Handler`。然而，在之前记录日志时并没有设置 `Handler`。这意味着，有一个默认的配置
> 

#### 为 Logger 添加 Handler

为 `root logger` 提供默认配置的就是函数 `logging.basicConfig()`，下面我们阅读这个函数的代码

```python
def basicConfig(**kwargs):
    with _lock:
        force = kwargs.pop('force', False)
        encoding = kwargs.pop('encoding', None)
        errors = kwargs.pop('errors', 'backslashreplace')
        if force:
	        # 如果设置了 force 值，就会清空 handlers，
            for h in root.handlers[:]:
                root.removeHandler(h)
                h.close()
                
        if len(root.handlers) == 0:
	        # 当 root 没有handlers 时，就会执行如下配置代码
	        # 获取提供的 handlers
            handlers = kwargs.pop("handlers", None)
            
			# 如果 handler 是 None,检查 stream 和 filename 是否同时提供
            if handlers is None:
                if "stream" in kwargs and "filename" in kwargs:
                    raise ValueError("'stream' and 'filename' should not be "
                                     "specified together")
            else:
                if "stream" in kwargs or "filename" in kwargs:
                    raise ValueError("'stream' or 'filename' should not be "
                                     "specified together with 'handlers'")
            if handlers is None:
	            # 获取 filename
                filename = kwargs.pop("filename", None)
                # 获取文件打开模式
                mode = kwargs.pop("filemode", 'a')
                if filename:
                    if 'b' in mode:
                        errors = None
                    else:
                        encoding = io.text_encoding(encoding)
                    # 存在 filename 则创建 FileHandler
                    h = FileHandler(filename, mode,
                                    encoding=encoding, errors=errors)
                else:
	                # 不存在 filename 则创建 StreamHandler
                    stream = kwargs.pop("stream", None)
                    h = StreamHandler(stream)
                handlers = [h]
            # 设置日期格式
            dfs = kwargs.pop("datefmt", None)
            # 设置日志格式风格
            style = kwargs.pop("style", '%')
            if style not in _STYLES:
                raise ValueError('Style must be one of: %s' % ','.join(
                                 _STYLES.keys()))
            # 获取日志格式
            fs = kwargs.pop("format", _STYLES[style][1])
            # 创建格式器
            fmt = Formatter(fs, dfs, style)
            for h in handlers:
                if h.formatter is None:
	                # 检查 handler 没有格式器，则添加格式器
                    h.setFormatter(fmt)
                root.addHandler(h)
            # 获取 root Logger‘s Level
            level = kwargs.pop("level", None)
            if level is not None:
	            # 设置 root Logger's Level
                root.setLevel(level)
            if kwargs:
                keys = ', '.join(kwargs.keys())
                raise ValueError('Unrecognised argument(s): %s' % keys)
```

> [!tip] 
> + 想要为 `Logger` 添加 `Handler`，则需要调用 `Logger.addHandler()`
> + 想要要为 `Handler` 添加格式器，则需要调用 `Handler.setFormatter()`
> 

请注意：只要 `root logger` 持有任意一个 `Handler` 时，该函数` basicConfig()`  就不被不执行

#### 日志流转

在 Python 的官方文档中，关于日志流转的流程图如下

![[Pasted image 20250511211627.png]]

应用程序调用日志函数，例如，调用 `logger.info(...)`。首先，检查 `Logger's level <= Message's Level`；只要满足这个条件，就创建 `LogRecord`，否则结束。

当 `LogRecord` 创建成功后，检查 `Logger` 的过滤器是否拒绝这个新创建的  `LogRecord`。当 `LogRecord` 被过滤器接受时，就会调用当前 `Logger` 的所有  `Handlers` 处理日志；否则结束

日志被发送到 `Handler` 时，首先检查 `Handler's Level <= Record's Level`；只要满足条件，调用 `Handler` 的过滤器过滤 `Record`；通过过滤器检查的 `Record` 就会被发送给对应的位置，此时会调用格式器处理日志格式问题

最后，检查 `Logger` 的 `propagate` 属性是否为 `True`。如果是，则查找 `Logger` 的 `Parent Logger`，然后再将 `Record` 处理

下面的代码就是 `Logger` 用于处理 `Record` 的源码

```python
    def callHandlers(self, record):
        c = self   # 当前的 logger
        found = 0  # 当前 logger 的 handler 个数(包括父 logger 的)
        while c:
	        # 对于每一个 handler
            for hdlr in c.handlers:
                found = found + 1
	            # 如果 record.level >= handler.level 就开始处理日志
                if record.levelno >= hdlr.level:
                    hdlr.handle(record)
            # 如果 logger.propagate 为 False 表示不调用父 Logger
            if not c.propagate:
                c = None    #break out
            else:
                c = c.parent
        # 最终都没有 handler 就使用 lastResort handler
        if (found == 0):
            if lastResort:
                if record.levelno >= lastResort.level:
                    lastResort.handle(record)
            elif raiseExceptions and not self.manager.emittedNoHandlerWarning:
                sys.stderr.write("No handlers could be found for logger"
                                 " \"%s\"\n" % self.name)
                self.manager.emittedNoHandlerWarning = True
```

### 格式器

格式器(`Formater`) 被 `Handler` 持有，也就在创建 `Handler` 时，我们就需要给它添加一个 `Formater`

> [!tip] 
> 
> 请注意：`Handler` 默认会自动创建一个格式器，这个格式器只会输出消息
> 

下面我们阅读 `handler.format()` 方法的源码验证上述说话。

```python
    def format(self, record):
	    # 检查 handler 是否设置了 formatter 
        if self.formatter:
            fmt = self.formatter 
        else:
	        # 如果没有设置就使用默认的 formatter
            fmt = _defaultFormatter
        return fmt.format(record)
```

其中 `_defaultFormatter = Formatter()`，也就是没有提供格式的默认格式器。

### 过滤器

过滤器(`Filter`) 用于过滤过滤哪个 `Handler` 发送来的日志。我们来看一下 `Filter` 的实现

```python
class Filter(object):
    def __init__(self, name=''):

        self.name = name
        self.nlen = len(name)

    def filter(self, record):

        if self.nlen == 0:
            return True
        elif self.name == record.name:
            return True
        elif record.name.find(self.name, 0, self.nlen) != 0:
            return False
        return (record.name[self.nlen] == ".")
```

> [!tip] 
> 
> `Filter` 类的实现非常简单，它只是根据 `name` 属性检查 `Record` 对象的 `name` 属性中是否包含了 `filter.name` 属性
> 
> 如果 `filter.name` 属性为 `""`，则不会过滤任何记录
> 

我们可以为  `logger` 和 `handler` 添加 `filter`。因为 `Logger` 和 `Handler` 都是下面类的子类

```python
class Filterer(object):
    """
    A base class for loggers and handlers which allows them to share
    common code.
    """
    def __init__(self):
        """
        Initialize the list of filters to be an empty list.
        """
        self.filters = []

    def addFilter(self, filter):
        """
        Add the specified filter to this handler.
        """
        if not (filter in self.filters):
            self.filters.append(filter)

    def removeFilter(self, filter):
        """
        Remove the specified filter from this handler.
        """
        if filter in self.filters:
            self.filters.remove(filter)

    def filter(self, record):
        for f in self.filters:
            if hasattr(f, 'filter'):
                result = f.filter(record)
            else:
                result = f(record)
            if not result:
                return False
            if isinstance(result, LogRecord):
                record = result
        return record

```

> [!tip] 
> 
> 过滤器会根据 `filter.name` 属性与 `logger.name` 属性的关系过滤 `Record`。
> 
> 只有 `logger.name` 属性的前缀与 `filter.name` 属性的值相等，则 `Record` 就通过过滤器 
