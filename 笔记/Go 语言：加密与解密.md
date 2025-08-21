# 加密与解密

在网络被发明之前，人们通过 **信件** 传递消息。消息被封装在 **信封** 中，避免消息在传递过程中被 **第三人(邮递员)** 查看或修改。网络被发明之后，人们通过网络进行消息的传递。消息在传递过程中，发送消息的用户称为 **发送者**，接受消息的用户称为 **接收者**，在消息传递过程中窃取消息的用户称为 **窃听者**

例如，Alice 向 Bob 发送电子邮件的场景。在这个场景中， 发出邮件的 Alice 称为 **发送者（sender）**， 而收到邮件的 Bob 则称为 **接收者（receiver）**

![Pasted image 20250702222028|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755787589779-0336053ea33e4ec0bfc4b39c2302ffcf.png)

邮件是通过网络从 Alice 的计算机发送到 Bob 的计算机的。在发送邮件时， 邮件会 **经过许多台计算机和通信设备进行中转**，在这个过程中，就存在被恶意 **窃听者**（eavesdropper） **偷看** 或者 **串改** 的可能性

![Pasted image 20250702222111|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755787589779-6b246b5b54bd4a808839a743ad7f892e.png)

> [!hint] 
> 
> **窃听者 `Eve` 并不一定是人类**，有可能是安装在通信设备上的某种窃听器， 也可能是安装在邮件软件和邮件服务器上的某些程序
> 

尽管邮件内容原本应该只有发送者和接收者两个人知道，但如果不采取相应的对策，就存在被第三方知道的风险。因此，需要采用某种 **措施** 来保护网络上传递的消息。这个措施就是 **加密** 和 **解密**

## 基本概念

Alice 不想让别人看到邮件的内容，于是她决定将邮件进行 **加密(encrypt)** 后再发送出去。加密之前的消息称为 **明文(plaintext)**，加密之后的消息称为 **密文(cipher-text)**

> [!tip] 
> 
> 我们看到明文可以理解其中的含义，而看到密文则无法理解其中的含义。也就是说，**明文被加密之后就会变成我们看不懂的密文**
> 

Bob 收到了来自 Alice 的加密邮件后，但作为接收者的 Bob 也是无法直接阅读密文的，于是 Bob 需要对密文进行 **解密(decrypt)** 之后再阅读。**解密就是将密文恢复成明文的过程**。也就是说，**密文解密之后就变成了原来的明文**

如何将一个明文加密得到一个密文呢？如何从密文解密还原名为呢？这两个问题的答案就是 **密码算法**。密码算法想要运行起来，往往需要提供一个初始的值，这个初始值称为 **密钥**。密钥就相当于保险箱的钥匙，只要钥匙不丢失，保险箱内的物品就是安全的，即使别人获得的保险箱也无法取得保险箱中保存的东西。

> [!tip] 
> 
> 密码算法可以分为 **加密算法** 和 **解密算法**
> + 从明文生成密文的步骤，也就是加密的步骤，称为 _“加密算法"_
> + 从密文生成明文的步骤，也就是解密的步骤，称为 _“解密算法"_
> 
> 加密和解密都需要知道密钥，否则算法无法执行
> 


根据加密步骤和解密步骤使用的密钥是否相同将密码算法分为 **对称** 和 **非对称** 两种。我们后续会详细介绍这两种密码算法

## 信息安全面临的威胁

下表总结了信息安全面临的威胁，及应对措施

| 威胁          | 特性          | 措施               |
| :---------- | :---------- | :--------------- |
| 窃听(信息泄露)    | 威胁信息的机密性要求  | 加密(对称加密和非对称加密)   |
| 篡改(信息被修改)   | 威胁信息的完整性要求  | 签名(单向散列函数和消息认证码) |
| 伪装(伪装信息发送者) | 威胁信息的可信性要求  | 签名(消息认证码和数字签名)   |
| 否认(发送者否则信息) | 威胁信息的不可否认要求 | 签名(消息认证码和数字签名)   |

![Pasted image 20240721154807|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755787589779-042e7ef36663428c89355cdb95cf0a80.png)


当我们在解决信息安全面临的威胁时，我们应该知道如下几点
+ 不要使用保密的密码算法（普通公司和个人）
+ 使用低强度的密码比不进行任何加密更危险
+ 任何密码总有一天都会被破解
+ 密码只是信息安全的一部分

## 对称密码算法

加密步骤和解密步骤使用相同的密钥的密码算法称为密码算法，也称单密钥密码算法。如下图所示

![Pasted image 20250702225049|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755787589779-3d5ed65d64ef4d94a1a96a7f8f4dce53.png)


对称密码算法分为两种类型：**分组密码**（Block Cipher）和 **流密码**（Stream Cipher）。

> [!tip] 
> 
> 流密码：对输入元素进行连续处理，同时产生连续单个输出元素
> 
> 分组密码：将明文分成固定长度的分组，各个分组在密钥控制下使用算法变换成等长的密文分组。常见的分组密码算法有：DES 3DES AES 
> 

### DES

DES（`Data Encryption Standard`）是 1977 年美国联邦信息处理标准中所采用的一种对称密码。DES 一直以来被美国以及其他国家的政府和银行等广泛使用。然而，随着计算机的进步，现在 DES 已经能够 被暴力破解，强度大不如前了

RSA 公司举办过破泽 DES 密钥的比赛（`DESChallenge`）， 我们可以看一看 RSA公司官方公布的比赛结果：

1. 1997年的 DES Challenge1 中用了 96 天破译密钥
2. 1998年的 DES ChallengeIl-I 中用了 41 天破译密钥
3. 1998年的 DES ChallengeII-2 中用了 56 小时破译密钥
4. 1999年的 DES ChallengeIll 中只用了 22 小时 15 分钟破译密钥

> [!attention] 
> 
> 由于 DES 的密文可以在短时间内被破译，因此除了用它来解密以前的密文以外， 现在我们不应该再使用 DES 了
> 

#### DES 分组密码算法

DES 是一种将 $64$ 位的明文加密成 $64$ 位的密文的对称密码算法，**它的密钥长度是 $56$ 位**。尽管从规格上来说，**DES 的密钥长度是 $64$ 位**，由于每隔 $7$ 位会设置一个用于错误检查位，因此 **实质上其密钥长度是 $56$ 位**

> [!tip] DES 密钥长度为 $56$ 位
> 
> 从规格上说，**DES 密钥长度为 $64$ 位**，由于每隔 $7$ 就需要设置一个错误检查位，因此实际上密钥长度是 $56$ 位
> 

DES 是以 $64$ 位的明文为一个组来进行加密的， **这个 $64$ 比特的单位称为分组**。一般来说，以分组为单位进行处理的密码算法称为 **分组密码（block cipher）**， DES 就是分组密码的一种

> [!important] 分组迭代的方式称为 **模式**
> 
> DES 每次只能加密 $64$ 位的数据，如果要加密的明文比较长，就需要对 DES 加密进行 **迭代**（反复），而 **迭代的方式就称为 模式**（`mode`）

> [!tip] DES 算法流程
> 
> 加密：
> 
> + 使用输入的 $64$ 位密钥，经过一些列的处理，得到 $16$ 个 $48$ 为子密钥
> + 将明文分组(`Block`)，每组 $64$ 位，每组使用 $16$ 个子密钥通过加密函数处理得到 $64$ 位密文
> 
> 解密：解密过程与加密 **过程相同**，但 **密钥应用次序和加密时相反**
> 

### 3DES

3DES 密码算法在加密过程中执行了 3 次 DES 加密算法

加密过程：
+ 首先，将完整的秘钥 **拆分** 为三个秘钥
+ 其次，使用第一个秘钥对明文 **进行一次加密**
+ 然后，使用第二个秘钥对 **第一次加密的密文进行解密**
+ 最后，使用第三个密码对 **第二个秘钥解密的密文进行加密**

解密过程
+ 首先，将完整的密钥 **拆分** 为三个秘钥
+ 其次，使用第一个密钥对密文进行一次 **解密**
+ 然后，使用第二个密钥对第一次解密的密文进行 **加密**
+ 最后，使用第三个密钥对第二个密钥加密的密文进行 **解密**


> [!tip] 3DES 密钥
> 明文经过三次 DES 处理才能变成最后的密文，由于 **DES 密钥的长度实质上是 56 位**，因此 3DES 的密钥长度就是  $56 \times 3 = 168$ 位, 加上用于错误检测的标志位 $8 \times 3= 24$ 位  , 共  $192$ 位

### AES

AES（Advanced Encryption Standard）是取代其前任标准（DES）而成为新标准的一种对称密码算法。全世界的企业和密码学家提交了多个对称密码算法作为 AES 的候选，最终在 2000 年从这些候选算法中选出了一种名为 **Rijndael** 的对称密码算法，并将其确定为了 AES

Rijndael 是由比利时密码学家 Joan Daemen 和 Vincent Rijmen 设汁的 **分组密码算法**，今后会有越来越多的密码软件支持这种算法。**Rijndael 的分组长度为 $128$ 比特**，密钥长度可以 $32$ 位为单位在 $128$ 位到 $256$ 位的范围内进行选择

> [!tip] 
> 
> AES 密钥长度只规定了 $128$, $192$ 和 $256$ 三种
> 

### 分组模式

分组密码的模式是 **指分组密码算法是如何迭代各个分组的**

分组密码的模式是 **指分组密码是如何迭代的**

> [!tip] **常见模式如下**
> 
> **ECB 模式**：Electronic Code Book mode（电子密码本模式），明文被分为固定大小的分组，并且**每个分组单独加密**
> + 优点：可以并行计算，不需要初始化向量(iv)
> + 缺点：有一个块被破解，其余块可以使用同样的方式破解
> 
> **CBC 模式**：Cipher Block Chaining mode（密码分组链接模式），明文被分为固定大小的分组，**每个分组都与前一个分组的密文进行 XOR**，然后再加密
> + 优点：每个分组单独加密
> + 缺点：无法并行计算，加密和解密速度比 ECB 慢；需要初始化向量从当第一个分组的前一个分组的密文
> 
> **CFB 模式**：Cipher FeedBack mode（密文反馈模式），明文被分组固定大小的分组，**前一个分组的密文加密后和当前分组进行 XOR** 得到当前分组的密文
> + 优点：每个分组单独加密；明文不需要填充
> + 缺点：无法并行计算；需要初始化向量从当第一个分组的前一个分组的密文
> 
> **OFB 模式**：Output FeedBack mode（输出反馈模式），明文被分组固定大小的分组，**前一个分组在密码算法中的输出与当前明文进行 XOR** 得到当前分组的密文
> + 优点：每个分组单独加密；明文不需要填充
> + 缺点：无法并行计算；需要初始化向量从当第一个分组的前一个分组的密文
> 
> **CTR 模式**：CounTeR mode（计数器模式），明文被分组固定大小的分组， **逐次累加的计数器** 进行加密来生成密钥流的 **流密码**
> + 优点：每个分组单独加密；明文不需要填充
> + 缺点：无法并行计算；需要初始化向量从当第一个分组的前一个分组的密文
> 

#### ECB模式：电子密码本模式

明文消息被分成固定大小的分组，并且 **每个分组被单独加密**。下图描述了 ECB 模式下的加密过程

![Pasted image 20240721192137|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755787589779-53231ebee5c44e4590dcbaefa3b8869a.png)

下图描述了 ECB 模式下的解密过程

![Pasted image 20240721192221|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755787589779-9dae16d8031f402da4dae4865b300885.png)

> [!hint] ECB 模式的优缺点
> 
> **_优点_**：每个块的加密和解密都是独立的，且使用相同的方法进行加密，所以可以进行 **并行计算**
>
> **_缺点_**：但是这种方法一旦 **有一个块被破解**，使用相同的方法 **可以解密所有的明文数据**。**安全性比较差**
> 
> 适用于数据较少的情形，**_加密前需要把明文数据填充到块大小的整倍数_**

#### CBC模式：密码分组链接模式

**CBC** (Cipher Block Chaining, 密码块链)模式：每一个分组要先和前一个分组的密文进行 **XOR 异或** 操作，然后再进行加密。  这样每个密文块依赖该块之前的所有明文块。下图是 CBC 模式的加密过程

![Pasted image 20240721192510|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755787589780-658acb08a88e479381ae161f5b0e36a2.png)

下图描述了 CBC 模式的解密过程

![Pasted image 20240721192537|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755787589780-cefa9f5f8c454da08ab67a9fd43713a6.png)


> [!important] 第一个分组需要初始化向量
> 
> 当加密第一个明文分组时，由于不存在 “前一个密文分组"。因此，<font color="red">需要事先准备一个长度为一个分组的比特序列来代替 "前一个密文分组"</font>， 这个比特序列称为 **初始化向量**（initialization vector, iv）
>
> 通常缩写为 IV 一般来说，每次加密时都会随机产生一个不同的比特序列来作为初始化向量

#### CFB 模式：密文反馈模式

CFB 模式的全称是 Cipher FeedBack 模式（密文反馈模式）。在 CFB 模式中，前一个分组的 **密文加密** 后和当前分组 **的明文XOR异或** 操作生成当前分组的密文。

> [!tip] 
> 
> 所谓反馈，这里指的就是返回输入端的意思，即 **_前一个密文分组会被送回到密码算法的输入端_**

CFB 模式的解密和 CBC 模式的加密在流程上其实是非常相似的。下图描述了 CFB 模式的加密过程

![Pasted image 20240721192828|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755787589780-c10c13c95a1a4e4190722769edb9f027.png)

下图描述了 CFB 模式的解密过程

![Pasted image 20240721192937|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755787589780-ae9cd3ff1d3141818aaba31c7a15f937.png)

> [!tip] 
> 明文不需要填充

#### OFB 模式：输出反馈模式

OFB 式的全称是 Output-Feedback 模式（输出反馈模式）。在OFB模式中，**密码算法的输出会反馈到密码算法的输入** 中， 即 **上一个分组的密文是当前分组算法的输入**。

OFB模式并不是通过密码算法对明文直接进行加密的，而是通过将 “明文分组"  和 “密码算法的输出” 进行 XOR 来产生 “密文分组” 的，在这一点上 OFB 模式和 CFB 模式非常相似

下图描述了 OFB 模式的加密过程

![Pasted image 20240721193250|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755787589780-a6ca140bd2dc4813bfe6f41b71bcfdf5.png)

下图描述了 OFB 模式的解密过程

![Pasted image 20240721193355|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755787589780-c67b03fac2f9442d9c90b6416824ad9d.png)

> [!tip] 
> 无需对明文进行填充

#### CTR 模式：计数器模式

CTR 模式的全称是 CounTeR 模式（计数器模式）。CTR 摸式是一种通过将 **逐次累加的计数器** 进行加密来生成密钥流的 **流密码**

CTR模式中，每个分组对应一个逐次累加的计数器，并通过对计数器进行加密来生成密钥流。也就是说，最终的密文分组是通过将计数器加密得到的比特序列，与明文分组进行 XOR 而得到的。

下图描述了 CTR 模式的加密过程

![Pasted image 20240721193605|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755787589780-2fc3fd6f8cbd4b168a776e49d822e7ed.png)

下图描述了 CTR 模式的解密过程

![Pasted image 20240721193631|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755787589780-37ac6b976fe04859b52fc4f80805ceba.png)

> [!tip] 
> 不需要对明文进行填充

### Go 语言提供的接口

#### 分组填充处理

分组加密时，要求分组（块）大小必须是固定大小，例如为 $16$ 字节，如果明文长度正好不是 $16$ 字节的倍数，就需要补齐为 $16$ 字节

> [!tip] 待填充字节数的计算
> $$
> n = blockSize - dataLength \% blockSize 
>$$
>+ $n$：待填充的字节数
>+ $blockSize$：分组大小
>+ $dataLength$：明文数据长度

> [!tip] 填充算法
> Zero：待填充字节的每个字节都补零 `0x00`
> 
> PKCS7 是当下各大加密算法都遵循的填充算法
> + 如果需要补齐，待填充字节 $n$ 的每个字节都填充为待填充字节数 $n$
> + 如果不需要补齐，需要追加一个块大小的数据，每个字节填充为块大小


例如，AES 128，数据块为 `16bytes`（数据长度需要为 `16bytes` 的倍数）。假设数据为 0x11223344556677889900AA，共 $11$ 个字节，缺了 $5$ 个字节，采用 PKCS7Padding 方式填充之后的数据为0x11223344556677889900AA0505050505。

如果是数据刚好满足数据块长度，也需要在数据后面按 PKCS7 规则填充一个数据块数据，这样做的目的是为了区分有效数据和补齐数据。假设数据为 0x11223344556677889900AABBCCDDEEFF，共 $16$ 个字节，采用 PKCS7Padding 方式，仍需要在数据后面补充一个 $16$ 个字节的块，填充之后的数据为0x11223344556677889900AABBCCDDEEFF10101010101010101010101010101010

```go
package main

import (
	"bytes"
	"fmt"
)

// PaddingLastGroup 明文填充
func PaddingLastGroup(plainText []byte, blockSize int) []byte {

	// 1. 字节数刚好就填充一个分组大小的字节，每个字节的值为 blockSize
	// 2. 字节数差几个就填充几个字节，每个字节的值就是待填充的字节数

	// len(plainText) % blockSize 表示最后一个分组的字节数
	padNum := blockSize - len(plainText)%blockSize

	// 创建一个字节切片，长度为要填充的字节数，每个字节都是填充的字节数
	padBytes := bytes.Repeat([]byte{byte(padNum)}, padNum)

	// 将填充的字节切片追加到原始明文中
	return append(plainText, padBytes...)
}

// UnPaddingLastGroup 去除明文填充
func UnPaddingLastGroup(plainText []byte) []byte {

	// 最后一个字节是填充的字节数
	padNum := int(plainText[len(plainText)-1])

	// 去除最后一个字节，即去除填充的字节
	return plainText[:len(plainText)-padNum]
}

func main() {
	text := []byte("hello, world")
	key := []byte("abcdefgh")

	text = PaddingLastGroup(text, len(key))
	fmt.Printf("%v\n", text) // [104 101 108 108 111 44 32 119 111 114 108 100 4 4 4 4]

	text = UnPaddingLastGroup(text)
	fmt.Printf("%v\n", text) // [104 101 108 108 111 44 32 119 111 114 108 100]
}
```

#### CBC 模式的加密和解密

```go
package main

import (
	"bytes"
	"crypto/cipher"
	"crypto/des"
	"fmt"

	"github.com/rs/zerolog/log"
)

// DES CBC 加密
func DesEncryptCBC(plainText, key []byte) []byte {
	// 1. 创建 DES Cipher
	block, err := des.NewCipher(key)

	if err != nil {
		log.Fatal().Err(err).Msg("创建 DES 加密失败")
	}

	// 2. 填充明文
	plainText = PaddingLastGroup(plainText, block.BlockSize())

	// 3. 创建 CBC 分组加密接口
	iv := []byte("abcdefgh")
	blockMode := cipher.NewCBCEncrypter(block, iv)

	// 4. 加密
	result := make([]byte, len(plainText))
	blockMode.CryptBlocks(result, plainText)
	return result
}

func DesDecryptCBC(cipherText, key []byte) []byte {
	// 1. 创建 DES 加密接口
	block, err := des.NewCipher(key)
	if err != nil {
		log.Fatal().Err(err).Msg("创建 DES 加密失败")
	}
	// 2. 创建模式接口
	iv := []byte("abcdefgh")
	blockMode := cipher.NewCBCDecrypter(block, iv) // cbc模式
	// 3. 解密
	result := make([]byte, len(cipherText))
	blockMode.CryptBlocks(result, cipherText)
	result = UnPaddingLastGroup(result)
	return result
}

func main() {
	text := []byte("hello, 世界")
	key := []byte("abcdefgh")

	result := DesEncryptCBC(text, key)
	fmt.Printf("%s\n", result)

	result = DesDecryptCBC(result, key)
	fmt.Printf("%s\n", result)
}
```

#### CTR 模式的加密和解密

```go
package main

import (
	"crypto/aes"
	"crypto/cipher"
	"fmt"

	"github.com/rs/zerolog/log"
)

func AesEncryptDecrypt(plantText, key []byte) []byte {
	// 创建 AES 加密接口
	block, err := aes.NewCipher(key)
	if err != nil {
		log.Fatal().Err(err).Msg("创建 AES 加密失败")
	}

	// 创建 CTR 分组模式
	iv := []byte("12345678abcdefgh")
	stream := cipher.NewCTR(block, iv)
	// 加密解密
	result := make([]byte, len(plantText))

	stream.XORKeyStream(result, plantText)
	return result
}

func main() {
	text := []byte("hello, 世界")
	key := []byte("12345678abcdefgh")

	result := AesEncryptDecrypt(text, key)

	fmt.Printf("%s\n", result)

	result = AesEncryptDecrypt(result, key)

	fmt.Printf("%s\n", result)
}
```


### 总结

下面的表格总结了三种对称加密算法

| 算法   | 分组大小                          | 密钥长度                                                                    |
| :--- | :---------------------------- | ----------------------------------------------------------------------- |
| DES  | $64$ 位一组                      | 需要 $64$ 位，其中每隔 $7$ 位是一个校验位，真实长度为 $56$ 位                                 |
| 3DES | $64$ 位一组                      | 需要 $64 \times 3 =196$ 位，其中，每隔 $7$ 位是一个错误检测位，真实长度为 $56 \times 3 = 168$ 位 |
| AES  | $128 \sim 256$ 中以 $32$ 位为步长选择 | 密钥长度需要根据分组大小进行选择。常用分组为 $128, 192, 256$ 位                                |

下面的表格总结了五种分组模式

![Pasted image 20240721193810|900|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755787589780-e56ce318e62a4c579b4d5ce6a8b7ec4c.png)

> [!tip] 
> 
> 使用 AES 加密算法时，推荐选用分组模式有
> + CBC
> + OFB
> + CTR
> 

## 非对称密码算法

> [!WARNING] 对称加密的风险 
> 由于加密和解密使用相同的密钥，在发送加密数据后，还需要给解密方提供密钥。**在提供密钥的过程中，密钥可能被第三方进行拦截**，导致数据安全得不到保障
> 

为了解决对称加密的风险，从而导致非对称加密被提出。加密和解密密钥不同且其中一个密钥不能通过另一个密钥推算出来。这种算法需要 $2$ 个密钥，公开密钥和私有密钥。如果公钥对数据加密，就只能使用私钥才能解密；如果使用私钥对数据加密，也只能使用公钥解密。这就是非对称加密方式。

![Pasted image 20250702233833|600](http://cdn.jsdelivr.net/gh/duyupeng36/images@master/obsidian/1755787589781-a03991685545415fa8c809f63db818e9.png)

### RSA 密码算法

RSA 是一种非对称加密算法，它的名字是由它的三位开发者，即 RonRivest、AdiShamir 和 LeonardAdleman 的姓氏的首字母组成的（Rivest-Shamir-Leonard）。RSA 可以被用于 **非对称加密** 和 **数字签名**

RAS 密码算法的安全性 **基于大数分解的困难性**。计算一对大素数的乘积很容易，但是要对这个乘积进行因式分解则非常困难。因此，把一对大素数的乘积公开作为公钥，把素数作为私钥，那么从公钥和密文中恢复出（暴力破解）明文的难度等价于大素数乘积的质因数分解。

> [!tip] 
> **对极大整数做 _质因数分解_ 的难度决定了 RSA 算法的可靠性**。换言之，对一极大整数做因数分解愈困难，RSA 算法愈可靠
> 
> 假如有人找到一种快速因数分解的算法，那么 RSA 的可靠性就会极度下降。但找到这样的算法的可能性是非常小的
> 
> 今天只有短的 RSA 密钥才可能被暴力破解。到 2008 年为止，世界上还没有任何可靠的攻击 RSA 算法的方式。 **只要密钥长度足够长，用 RSA 加密的信息实际上是不能被破解的**

RSA 目前推荐长度至少 $2048$ 位，相对安全，据称目前使用超级计算机破解也需要 $80$ 年。但是，有研究表示使用了量子计算机破解 $2048$ 位只需要数小时

#### RSA 加密

在 RSA 密码算法中，**明文、密钥、密文都是数字**。RSA 加密的公式如下

$$
密文 = 明文^E  \mod{N}
$$

在公式中，出现的 $E$ 和 $N$ 就是 RSA 的加密密钥，也即是 $(E, N)$ 为加密密钥（公钥）通常公钥表示为 $公钥(E,N)$


#### RSA 解密

RSA 解密的公式如下

$$
明文 = 密文^D \mod{N}
$$

公式中出现的 $D$ 和 $N$ 的组合就是 RSA 的解密密钥（私钥），通常表示为 $私钥(D, N)$。其中 $N$ 与加密密钥中的 $N$ 是同一个

#### RSA 密钥的计算

密钥对是由 **公钥(E, N)**  和 **私钥(D, N)** 组成的。只需要计算出 $E, D, N$ 这三个数就可以生成密钥对。具体步骤如下

1. 求 N
2. 求 L（L 是仅在生成密钥对的过程中使用的数）
3. 求 E
4. 求 D

##### 求 N

需要准备两个很大的素数 $P$ 和 $Q$
- 如果 $P$ 和 $Q$ 过小的，密码容易破解
- 如果 $P$ 和 $Q$ 过大的，会导致计算时间变得很长

下面做一个假设，$P$ 和 $Q$ 都是 $512 \text{bit}$，相当于 $155$ 位十进制数。使用伪随机数生成器生成一个 `512 bit` 大小的数，在判断该数是否为素数，如果不是，则另外在生成一个直到得到的是素数为止。

 > [!hint] 
 > 
 > 判断一个数是否为素数的方法有 *费马测试* 和 *米勒.拉宾测试* 等
 > 

计算 $N$ 的公式如下

$$
N = P \times Q
$$

##### 求 L

$L$ 在 RSA 加密和解密过程都不会出现，而是仅在生成密钥的时候使用。$L$ 是 $P-1$ 和 $Q-1$ 的最小公倍数

$$
L = lcm(P-1, Q-1)
$$

##### 求 E

$E$ 是一个比 $1$ 大，比 $L$ 小的数，且 $E$ 和 $L$ 的最大公因必须为 $1$，也就是所 $E$ 与 $L$ 互为素数。也就是说，$E$ 和 $L$ 满足下列条件

$$
\begin{cases}
& 1 \lt E \lt L\\
\\
& gcd(E, L) = 1
\end{cases}
$$

求 $E$ 的方法是通过伪随机数生成在 $1 \lt E \lt L$ 范围内的 $E$，然后使用辗转相除法求最大公因数，如果 $E$ 和 $L$ 的最大公因数等于 $1$ ，$E$ 就确定下来了。此时就已经确定了 $公钥(E, N)$

##### 求D

数 $D$ 是 $E$  计算得到，$D, E, L$ 之间满足下列关系

$$
\begin{cases}
1 \lt D \lt L \\ \\ E \times D \mod{L} = 1
\end{cases}
$$


只要 $D$ 满足上面两条要求，则 $私钥(D, N)$ 确定


### Go 语言提供的接口

#### Go 生成密钥

```go
package main

import (
	"crypto/rand"
	"crypto/rsa"
	"crypto/x509"
	"encoding/pem"
	"os"
)

// RsaGenKey 生成公钥和私钥
func RsaGenKey(bits int) error  {
	// 1. 使用rsa中的GenerateKey方法生成私钥
	// GenerateKey函数使用随机数据生成器random生成一对具有指定字位数的RSA密钥
	// 参数1: Reader是一个全局、共享的密码用强随机数生成器
	// 参数2: 秘钥的位数 - bit 推荐2048 最少1024
	privateKey, err := rsa.GenerateKey(rand.Reader, bits)
	if err != nil {
		return err
	}
	// 2. 通过x509标准将得到的ras私钥序列化为ASN.1 PKCS#1 DER编码字符串
	derStream := x509.MarshalPKCS1PrivateKey(privateKey)
	// 3. 将私钥字符串设置到pem格式块中
	pemBlock := &pem.Block{
		Type: "RSA PRIVATE KEY",
		Bytes: derStream,
	}

	// 4. 创建文件
	privFile, err := os.Create("private.pem")
	if err != nil{
		return err
	}
	defer privFile.Close()

	// 5. 使用pem编码, 并将数据写入文件中
	 pem.Encode(privFile, pemBlock)

	// 6. 获取公钥
	publicKey := privateKey.PublicKey
	// 7. 通过x509标准将得到的ras将公钥序序列化为PKIX格式DER编码。
	derPkix, err := x509.MarshalPKIXPublicKey(&publicKey)
	if err != nil{
		return err
	}
	// 8. 将公钥字符串设置到pem格式块中
	block := pem.Block{
		Type: "RSA PUBLIC KEY",//"PUBLIC KEY",
		Bytes: derPkix,
	}
	// 9. 创建公钥文件
	pubFile, err := os.Create("public.pem")
	if err != nil{
		return err
	}
	defer pubFile.Close()
	// 10. 将公钥编码成字符串写入文件
	err = pem.Encode(pubFile, &block)
	if err != nil {
		return err
	}

	return nil
}
```

#### Go 中实现加密和解密

```go
package main

import (
	"crypto/rand"
	"crypto/rsa"
	"crypto/x509"
	"encoding/pem"
	"fmt"
	"io"
	"os"
)

// RSAEncrypt RSA加密（使用公钥加密）
func RSAEncrypt(src, filename []byte) []byte {
	// 1. 根据文件名将文件内容从文件中读出
	file, err := os.Open(string(filename))
	if err != nil {
		return nil
	}
	// 2. 读文件
	allText, err := io.ReadAll(file)
	if err != nil {
		return nil
	}

	// 3. 关闭文件
	file.Close()

	// 4. 从数据中查找到下一个 PEM 格式的块
	block, _ := pem.Decode(allText)
	if block == nil {
		return nil
	}

	// 5. 解析一个 DER 编码的公钥
	pubInterface, err := x509.ParsePKIXPublicKey(block.Bytes)
	if err != nil {
		return nil
	}
	pubKey := pubInterface.(*rsa.PublicKey)

	// 6. 公钥加密
	result, _ := rsa.EncryptPKCS1v15(rand.Reader, pubKey, src)
	return result
}

// RSADecrypt RSA解密（私钥解密）
func RSADecrypt(src, filename []byte) []byte {
	// 1. 根据文件名将文件内容从文件中读出
	file, err := os.Open(string(filename))
	if err != nil {
		return nil
	}

	// 2. 读文件
	allText, err := io.ReadAll(file)
	if err != nil {
		return nil
	}

	// 3. 关闭文件
	file.Close()

	// 4. 从数据中查找到下一个PEM格式的块
	block, _ := pem.Decode(allText)

	// 5. 解析一个pem格式的私钥
	privateKey, err := x509.ParsePKCS1PrivateKey(block.Bytes)
	if err != nil {
		return nil
	}

	// 6. 私钥解密
	result, _ := rsa.DecryptPKCS1v15(rand.Reader, privateKey, src)

	return result
}

func main() {
	result := RSAEncrypt([]byte("hello, world"), []byte("public.pem"))

	result = RSADecrypt(result, []byte("private.pem"))

	fmt.Println(string(result))
}
```
