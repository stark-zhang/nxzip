# 一种高压缩比的游程编码方法讨论

####综述

游程编码（Run-Length Coding, RLC）常用于针对连续重复的数据进行压缩，而在数据压缩过程中，有可能产生大量连续数据的过程只有BWT、Move-To Front以及Huffman编码  

如果针对BWT的输出进行游程编码，无疑将会使前移变换失去意义——由此带来的后果就是，无法有效的降低原始数据的信息熵，从而在进行后续的Huffman编码时，加长平均码字长度，降低压缩效率

Huffman编码结果是一串二进制序列，将此序列压缩至字节流的时候，其中数据的冗余度想来不会很高，也不适合进行游程编码

因此只能对MTF的结果进行游程编码，据统计，MTF序列中大约有60%的元素都是字符`\0` [^1]，因此可以针对MTF序列进行游程编码，以缩短Huffman编码时的输入序列长度，一般地，针对MTF的游程编码，又只对`\0`进行编码，其余元素忽略不计，又称为`RLE-0`

####编码方法

一个重要公式[^2]：
$$
 y = x + 1 - 2^{[log_2(x+1)]} \tag1
$$

编码时采用变长的转义字符（Escape Character, EC）来区分连续零子序列和非连续零子序列
+ 对于连续零子序列，EC为两个字节
+ 对于非连续零子序列，EC为一个字节

转义字符中比特位的定义详见`rlc/rlc.h`，鉴于GFM的限制，此处不便列表详述

####冗余度分析

对于采用此方法的游程编码，只对三个及三个以上的连续零子序列进行编码，替换连续零子序列的EC为两个字节

在最坏情况下，每127个字节即插入一个EC，冗余度为`1/127`，否则只要有连续零子序列，即可缩减编码长度，而MTF序列中连续零子序列极为常见，因此编码长度大于原始序列长度的可能性不高

最好的情况下，两个EC字节最多可表示4095个连续零的子序列

#### Freeze(2019-04-27)

采用了上文提到的算法，即日起如无bug，即冻结RLC算法的开发

#### Update

+ 2019-04-12
初步实现了通用的RLC算法
+ 2019-04-27
将RLC算法特化为针对MTF序列的RLE-0算法

---
`created by vscode, wrote by markdown(MPE), 2019-02-05, completed in 2019-04-27`

[^1]: Improvements to Burrows–Wheeler compression algorithm, Sebastian Deorowicz, 第3.4小节

[^2]: 该公式出自于[Auxilary Compression Transforms](http://compressions.sourceforge.net/Auxilary.html)