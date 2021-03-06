# BWT及其逆变换的算法分析

---

[BWT算法](https://en.wikipedia.org/wiki/Burrows%E2%80%93Wheeler_transform)于1994年由Michael Burrows和David Wheeler提出，它的作用主要是改善序列的局部相关性，以便使用MTF算法降低其信息熵，从而获得更高的压缩比

#### 原理
可以在wiki上看到，BWT的原理实现中，对一个长度为n的序列，需要一个大小为(n+1)^2^的矩阵进行变换，于是在序列增长的情况下，占用的内存急剧增加，在n=10^6^B时，矩阵占用的内存大于1TB，这是无法实现的；同时，对于这么大的矩阵进行遍历和排序的时间复杂度也是不可接受的，因此对于该算法的设计，应当**着眼于时间复杂度和空间复杂度的优化**

首先要明确：
1. BWT的结果是唯一的
2. 循环移位的方式（左移或者右移）不影响最后结果
3. 相近的序列进行BWT的结果也许差别很大（插入结束符与否的情况）
4. 时间复杂度不可能无线优化，目前最好情况的估计就是O($nlogn$)到O($n^2logn$)

#### 方法
> 一种基于后缀排序快速实现Burrows-Wheeler变换的方法

基于后缀排序的BWT实现，基本上解决了空间复杂度的问题
对于一个长度为$n$的数组，实现基于后缀排序的BWT的过程应该如下：
+ 插入标志元素，这个标志元素的字典序应该大于数组中的所有元素（即值最小） 
+ 取后缀数组，并对取到的后缀数组进行字典序排序
+ 排序依次取每个后缀数组前的一个元素（对于长度为$n+1$的后缀数组，就取标志元素）
+ 将上一步中取到的元素依次存储，即是所求的`Burrows-Wheeler`转换的结果，标志元素的位置（从零开始数）就是所求的索引值  

在取后缀数组的时候，不必将后缀数组的元素存储到别处，只需取原数组的指针偏移，使用结构体记录下后缀数组相对原数组的位置和长度即可
```c++
/* Logic Suffix Array Definition */
typedef struct 
{
	int16_t* nnstr;			/* !< the cache pointer */
	uint32_t position;		/* !< the position in origin Array */
	uint32_t length;		/* !< the length of nnstr */
}SuffixArray_TypeDef;
```
假设在`32-bit`系统上，每个结构体占用的内存是12字节，共取出n+1个后缀数组，则占用的总内存应该是12(n+1)字节，相对原理算法的(n+1)^2字节来说，极大的降低了内存的占用

对后缀数组的字典序排序，目前使用的是冒泡排序（bubble sort），该算法的时间复杂度在最好情况下O(n\^2)（原数组已经按正序排序好），最坏情况下为O(n\^3)（原数组已经按倒序排序好），这样整个BWT算法的时间复杂度在O(n\^2)到O(n^3)之间，且实际上取不到极限情况

~~若将这部分更换为快排序（quick sort），算法的时间复杂度最好情况下应该是O(nlogn)，最坏情况下应该是O(n^2^logn)，最好与最坏情况同上，也取不到极限情况~~

此部分已经更换为归并排序（merge sort），算法时间复杂度同快排序，但是应当注意**函数调用栈（Call Stack）是否会越界**

综上，BWT的算法的时间复杂度和空间复杂度的优化可以告一段落，应该考虑具体的工程实现了

==即日起废弃基于添加唯一后缀字符的BWT算法，仅进行基于next值的新方法开发与总结，bwt.cpp冻结于2019.02.27==

#### 方法再思考
在上述方法中，考虑到了带有后缀结束符的情况，但是，也可以仅凭原序列进行后缀排序，以获得更好的空间复杂度，具体见`bwt2.cpp`

通过观察，BWT变换的索引值在轮转矩阵中表现为原序列的行数，在后缀数组中则相应的表现为排序后完整序列所在的行数，因此只需在排序前，标记完整序列并在排序后遍历得到完整序列所在的行号即可 

这种基于next值的方法运算量和存储需求明显低于添加唯一后缀字符的方法，而且便于解码获得原序列

#### BWT解码的思考
原理实现见[wiki](https://en.wikipedia.org/wiki/Burrows%E2%80%93Wheeler_transform)，然而实际上，该方法并不适用于没有内建矩阵类型的c++，退一步说，即使c++内建矩阵类型也不能用，否则又要回退到BWT空间复杂度的难点之中了

但是BWT变换之后得到的序列总是满足这样几条性质：

1. BWT结果即BWT矩阵最后一列（L列），对该列进行字典序排序后即为BWT矩阵的第一列（F列）
2. BWT矩阵的第一列和最后一列中，同一种元素的相对位置不变，即F列中第一个出现的‘A'，一定对应L列第一个出现的'A'
3. 同一行中，F列的元素是L列元素的下一个元素  

通过以上三条性质，就可以逆序写出原序列了，当然在编程实现的时候，可以一次写出正确的原序列

#### Freeze(2019-04-21)
之前的方法相当于分拆实现了构建后缀树（Suffix Tree）的算法，复杂且不便于调试
现在使用新的算法，直接构建后缀数组（Suffix Array, SA），SA的每个元素减去1即为BWT序列中每个元素在原序列中的索引，例如，对于序列`banana`：
```
suffix array = {5, 3, 1, 0, 4, 2}
BWT array = {4, 2, 0, 5, 3, 1} --> {n, n. b, a, a, a}, index = 2
```
显然上述关系是成立的，如果SA中的元素为0，则对应的索引为`n-1`，而BWT的索引值（用于解码）显然是SA中零元素的索引减一

#### Reference
1. video: [how to create a suffix array](https://www.youtube.com/watch?v=m2-N853rS6U)
2. paper: [Suffix Array in Stanford Universuty](https://web.stanford.edu/class/cs97si/suffix-array.pdf)
3. book: Introduction to Data Compression, 3rd Edition, Elsevier

#### Update
+ 2019-02-05
初步实现基于插入唯一结束字符的BWT算法  
+ 2019-02-19 
实现基于next值的BWT算法  
废弃基于唯一结束符的BWT算法  
+ 2019-02-27
实现基于next值的Inverse-BWT算法  
+ 2019-03-02
重构BWT算法，将排序方法由冒泡排序更换为归并排序，时间复杂度下降至O(n^2logn)
+ 2019-04-21
重构BWT算法，整体方法更换为构建后缀数组，时间复杂度下降至O(nlogn)

---
`created by vscode, wrote by markdown(MPE), 2019-02-05, completed in 2019-02-27`