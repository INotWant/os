## 疑问及解答

#### 1. os-image.bin 的组成及作用

首先，明确 `.bin` 文件是由 **代码** 和 **数据** 组成的二进制 **内存映射** 文件，这意味着它被加载到内存的过程中内容不用被解析、内容不变。

从 Makefile 文件中，可看到 `os-image.bin` 是由 `boot_sec.bin` 和 `kernel.bin` 拼接而成。所以，`os-image.bin ` 包含了它们的代码和数据。因为 `boot_sec.bin` 的大小为 512 bits（即第 0 扇区），所以从地址 **0x0000200** 开始来源于 `kernel.bin`。由于 `kernel_entry.asm` 是 `kernel.bin` 的入口，所以猜测地址 **0x0000200** 存放的是 `kernel_entry.asm` 对应的机器码。下面对比验证了此：

![1](./1.png)

![2](./2.png)

另外值得一提的是，`os-image.bin` 要被烧录到硬盘 或 flash 类型的 ROM 中。

#### 2. 地址空间的地址全映射到内存上了吗？

<img src="./3.png" alt="3" style="zoom:50%;" />

![4](./4.png)

参考：[实模式下的1MB内存布局](https://book.51cto.com/art/201604/509566.htm)

#### 3. 段描述符中段限长只有20位，那一段的大小不能超过 1M ？

段描述符数据结构中有个标志为G，但 `G=1` 时，段边界的单位为 **4KBytes**。因而一个段的最大长度可达 **4GBytes**。

参考：[描述符表和描述符高速缓存](http://hengch.blog.163.com/blog/static/107800672009028105929795/)

