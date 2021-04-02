---
title: 数据分析-NumPy
tags: 
- 数据分析
categories: [数据分析]
sharing: True
---



# 简介

NumPy全称Numeric Python，它具有如下特性：
- ndarray（多维数组对象），即对n维数组的基本操作
- 与ndarray相关的各种运算
- 支持I/O操作
- 线性代数、随机数、傅里叶变换等操作
- 与C/C++/Fortran等编程语言结合

NumPy是专门为大型数组的相关操作所设计的函数库，它的底层为C语言。同时在内存分配上也是为数组开辟连续的空间进行存储。通过这些优化，使得其运行速度相比于Python的内置类型要快得多。

NumPy官方文档：https://docs.scipy.org/doc/numpy/


```python
import numpy as np
import warnings
warnings.filterwarnings("ignore")
```

<!--more-->

# ndarray

## 创建

### 非随机方式

在NumPy中，创建一个ndarray对象的方法有很多，下面为一些示例：


```python
data=[[1,2,3,4],[5,6,7,8]]
data1=np.array(data,dtype=np.int64) #将Python的list、tuple等数据类型转换为一个ndarray，数据类型可以自己指定或是让函数自动推断。默认情况下使用复制原始数据的方法创建。
data2=np.asarray(data) #与np.array()类似，但如果传入参数是一个ndarray则为引用方式而不是复制
data3=np.arange(1,100,2) #与Python内置的range函数类似，但是该函数的返回值是一个ndarray
data4=np.ones((3,3)) #创建一个全为1的ndarray，数组形状可以通过传入一个list或者tuple指定。
data5=np.ones_like(data1) #创建一个全为1的ndarray，数组形状由传入的ndarray决定
data6=np.zeros([2,2,2]) #创建一个全0的ndarray，使用方法类似于np.ones()
data7=np.zeros_like(data1) #创建一个全0的ndarray，使用方法类似于np.ones_like()
data8=np.empty((2,2)) #创建一个新的ndarray，但是数组的内容没有被初始化，即数组内的元素是随机的。使用方法与np.ones类似
data9=np.empty_like(data1) #创建一个未初始化的ndarray，使用方法与np.ones_like()类似
data10=np.eye(4) #创建一个二维的n*n对角矩阵，对角元素为1，其余元素为0。
data11=np.identity(3) #功能与np.eye()相同
data12=np.full((2,2),3) #创建一个全部由某个元素填充的ndarray
data13=np.full_like(data1,3) #与np.full类似，数组形状与传入的ndarray相同
data14=np.linspace(0.0,2.0,4) #在某个区间上生成若干个等间距的数据点，前两个参数对应于区间起点和终点，第三个参数对应于数据点的数量（注意是数量！）
# 注：以上这些创建ndarray的函数都可以传入dtype参数，从而手动指定ndarray中的元素类型。
```

其中，每个ndarray都有如下的属性：


```python
data1.ndim #输出ndarray的维度
```


    2




```python
data1.shape #输出ndarray每个维度上的大小
```


    (2, 4)




```python
data1.dtype #输出ndarray中的元素类型
```


    dtype('int64')



ndarray可以使用的数据类型如下：

|类型|描述|
|:-:|:-:|
|int8, uint8|有符号和无符号的8位整数|
|int16, uint16|有符号和无符号的16位整数|
|int32, uint32|有符号和无符号的32位整数|
|int64, uint64|有符号和无符号的64位整数|
|float16|半精度浮点数|
|float32|单精度浮点数|
|float64|双精度浮点数|
|float128|扩展精度的浮点数|
|complex64, complex128, complex256|使用32、64和128位浮点数表示的复数|
|bool|布尔类型|
|object|Python的对象类型|
|string_|固定长度的字符串类型|
|unicode_|固定长度的unicode类型|


```python
data1=data1.astype('string_') #调用ndarray的astype()方法，可以进行数据类型的转换
```


```python
data1.dtype
```


    dtype('S21')



一个ndarray的数据类型可以自己定义，例如：


```python
dtype_self=[('x',np.float64),('y',np.int32)]
```


```python
self_arr=np.array([(1.5,6),(np.pi,-2)],dtype=dtype_self)
```


```python
self_arr
```


    array([(1.5       ,  6), (3.14159265, -2)],
          dtype=[('x', '<f8'), ('y', '<i4')])




```python
self_arr[1]['x'] #在索引时，便可以使用元素所在的index，加上数据类型里面的某个名字，索引出数据结构里面的某个元素
```


    3.141592653589793




```python
self_arr[1] #如果不加数据类型里面的名字，则得到整个数据结构
```


    (3.14159265, -2)




```python
self_arr['y'] #如果不使用index，则索引数据类型里面某个名字对应的所有元素
```


    array([ 6, -2])



在数据类型的定义上面比较灵活，支持内嵌的数组甚至是数据类型的嵌套。例如：


```python
dtype=[('x',np.int64,3),('y',np.int32)]
```


```python
np.zeros(4,dtype=dtype)
```


    array([([0, 0, 0], 0), ([0, 0, 0], 0), ([0, 0, 0], 0), ([0, 0, 0], 0)],
          dtype=[('x', '<i8', (3,)), ('y', '<i4')])




```python
dtype=[('x',[('a','f8'),('b','f4')]),('y',np.int32)]
```


```python
self_arr2=np.array([((1,2),5),((3,4),6)],dtype=dtype)
```


```python
self_arr2['x']['a']
```


    array([1., 3.])

### 随机方式

此外，NumPy也提供了一些生成随机元素的方式来创建ndarray，这些随机数可以满足不同的概率分布。下面为一些使用示例：


```python
np.random.seed(0) #设置随机数种子
```


```python
np.random.permutation(test) #将一个array随机打乱，不改变原数组的内容
```


    array([[ 3.8,  2.1, -5.4,  0. ],
           [ 0.5, -0.3,  2.5,  2.8]])




```python
np.random.shuffle(test) #同样是将array随机打乱，但是对array进行就地修改
```


```python
np.random.permutation(20) #也可以传入一个整数，生成0至n-1所有数字的一个全排列
```


    array([15, 10, 14, 13,  1, 12, 11, 19,  8,  6, 16,  4,  2,  5, 17,  9,  7,
           18,  3,  0])




```python
np.random.rand(1,2,2) #传入n个数字，生成一个n维数组，数组中的元素服从0到1的均匀分布
```


    array([[[0.3927848 , 0.83607876],
            [0.33739616, 0.64817187]]])




```python
np.random.uniform(low=-2.0,high=4.0,size=(3,4)) #生成一个n维数组，所有的元素在区间[low,high)中间均匀分布，size代表产生ndarray的规模（下同）
```


    array([[ 0.20944924,  3.74293095, -1.15789532,  3.22052355],
           [ 0.84164827,  2.80546451,  1.12286488,  2.07327718],
           [ 2.32379593,  1.49211875,  1.22423938,  2.55169375]])




```python
np.random.randn(3,3) #传入n个数字，生成一个n维数组，数组中的元素服从均值为0，标准差为1的正态分布
```


    array([[-0.06491034, -0.96898025,  0.59124281],
           [-0.7827755 , -0.44423283, -0.34518616],
           [-0.88180055, -0.44265324, -0.5409163 ]])




```python
np.random.normal(loc=-2.0,scale=2.0,size=(3,3)) #生成元素服从正态分布的随机数组，loc代表均值，scale代表标准差
```


    array([[-4.64645474, -2.22559784, -0.18530811],
           [-0.36946018, -1.5418041 , -4.05235756],
           [-1.04494906,  0.58539646, -3.46291647]])




```python
np.random.randint(0,5,size=(3,4)) #生成均匀分布的随机整数，第一个参数代表下限（可取到），第二个代表上限（不能取到）
```


    array([[3, 4, 1, 2],
           [4, 3, 4, 4],
           [4, 3, 4, 4]])




```python
np.random.binomial(10,0.3,size=(4,3)) #生成元素服从二项分布的数组，第一个参数为整数，第二个参数为概率
```


    array([[3, 3, 3],
           [3, 2, 4],
           [2, 4, 5],
           [4, 0, 3]])




```python
np.random.beta(3,4,size=(2,2)) #生成元素服从beta分布的数组，前两个参数对应于beta分布的参数
```


    array([[0.30826832, 0.46266575],
           [0.68918031, 0.67705037]])




```python
np.random.chisquare(2,size=(2,2)) #生成元素服从chi-square分布的数组，第一个参数为chi-square分布的参数
```


    array([[4.97116465, 5.08060897],
           [0.17354098, 0.65068082]])




```python
np.random.gamma(3,scale=1.0,size=(2,2)) #生成元素服从gamma分布的数组，第一个参数对应于gamma分布的shape
```


    array([[4.6552704 , 3.43428893],
           [1.11959827, 2.99619096]])



## ndarray的内部构造

### ndarray的数据结构

ndarray的内部包含如下内容：一个指向数据的指针，数据类型dtype，一个shape属性表示数组形状的tuple，一个表示stride（用于指示应进行维度转换的字节数）的tuple

### 数据类型的关系

![datatypes](https://raw.githubusercontent.com/lyf35/blog_figures/main/img/20210105233630.png)



# 数组的I/O操作


```python
np.save('test_array',test) #保存一个ndarray，第一个参数为文件名，文件后缀为.npy，默认保存格式为未压缩的二进制文件
np.savez('test_array2',a=test1,b=test2) #同时将多个ndarray保存在一个文件中，但是不同的ndarray要使用不同的命名参数区分开
```


```python
arch1=np.load('test_array.npy') #读取.npy文件
arch2=np.load('test_array2.npz') #如果原始的.npy文件中包含了多个ndarray，可以用保存时为它们添加的命名参数进行索引，例如arch2['a']
np.loadtxt('txtname.txt',delimiter='') #从.txt中读取一个ndarray
```



# 切片和索引操作

NumPy的ndarray也支持类似于list、tuple的切片和索引操作，但是稍有不同。具体表现为：

1. 一个ndarray的切片或者索引是对原始array的引用，在对切片和索引得到的ndarray进行修改的时候会直接对原始的ndarray进行修改。如果要使得切片或索引部分是一个副本，则可以调用ndarray的`copy()`方法。


```python
test_slice1=np.array([0,1,2,3,4,5,6,7,8,9])
```


```python
arr_slice=test_slice1[5:8]
arr_slice[1]=12345
test_slice1
```


    array([    0,     1,     2,     3,     4,     5, 12345,     7,     8,
               9])




```python
arr_slice[:]=32 #赋值时使用标量，这一标量会自动做broadcast操作
test_slice1
```


    array([ 0,  1,  2,  3,  4, 32, 32, 32,  8,  9])




```python
test_slice1.put([5,6,7],33) #与上面的赋值操作等价。put函数输入一个元素全部为整数的list，以及一个数字或者一个list，将相应位置上的元素修改为所传入的数字或者list中的值。
#注意：如果是多维数组，索引的时候元素对应的编号是按照从里到外的顺序将ndarray展开成一维之后所对应位置的编号。
```


```python
test_slice1
```


    array([ 0,  1,  2,  3,  4, 33, 33, 33,  8,  9])



2. 多维数组也可以做切片和索引操作。如果要对多个维度同时做切片操作，则需要在每一个维度的切片操作之间，用逗号把它们隔开。而对于多维数组的索引，规则比起多维数组的切片更加复杂，下面以具体的代码来进行说明。


```python
test_slice2=np.array([[ 0, 1, 2, 3],[ 4, 5, 6, 7],[ 8, 9, 10, 11],[12, 13, 14, 15],[16, 17, 18, 19],[20, 21, 22, 23],[24, 25, 26, 27],[28, 29, 30, 31]])
```


```python
test_slice2[-5:-1:2,:2] #对ndarray做切片时，最终结果是维度相同的ndarray。切片时支持倒序切片
```


    array([[12, 13],
           [20, 21]])




```python
test_slice2[:,2:] #如果要在某个维度上不做切片，则分成两种情况。如果这个维度及其之后的所有维度都不切片，那么后面的切片部分就可以省略，比如test_slice2[:1]就是合法的，代表只在维度0上面切片。
```


    array([[ 2,  3],
           [ 6,  7],
           [10, 11],
           [14, 15],
           [18, 19],
           [22, 23],
           [26, 27],
           [30, 31]])




```python
test_slice2[...,:2] #可以使用省略号来表示任意多个维度上都不做切片操作。省略号具体省略哪些维度需要根据ndarray的维度大小以及表达式共同决定。但是这种用法可读性不好，尽量不要使用。
```


    array([[ 0,  1],
           [ 4,  5],
           [ 8,  9],
           [12, 13],
           [16, 17],
           [20, 21],
           [24, 25],
           [28, 29]])




```python
test_slice2[1,:2] #索引与slice可以合用
```


    array([4, 5])




```python
test_slice2[[2,-1]] #对一个ndarray进行索引时可以传入list，表示在第一个维度上同时索引多个元素，此时得到的是ndarray。同时索引的顺序可以任意，也支持倒序索引。下面会对这种索引方式进行详细说明。
```


    array([[ 8,  9, 10, 11],
           [28, 29, 30, 31]])




```python
test_slice2.take([2,-1],axis=0) #这一操作等价于上面的操作。take函数传入一个元素全为整数的list，代表在某个维度上挑选相应位置的元素，其中list中的元素可以重复，例如[2,2,-1]是可以的。
```


    array([[ 8,  9, 10, 11],
           [28, 29, 30, 31]])




```python
test_slice2[1][3] #等同于test_slice2[1,3]，但是它们的原理不一样。test_slice2[1][3]相当于在test_slice2[1]的结果基础之上，再使用[3]进行索引；而test_slice2[1,3]则是直接选取坐标为(1,3)的元素。
```


    7



注意下面三种不同的索引方式得到的结果也有很大差别：


```python
test_slice2[[2,-1],[0,1]] #索引位置为(2,0)和(-1,1)的元素
```


    array([ 8, 29])




```python
test_slice2[[2,-1]][[0,1]] #在test_slice2[[2,-1]]的基础之上再索引[[0,1]]。与上面的结果进行对比可以发现它们在运算时的不同之处。此处使用test_slice2[[2,-1]][:,[0,1]]可以得到与上面相同的结果。
```


    array([[ 8,  9, 10, 11],
           [28, 29, 30, 31]])




```python
test_slice2[np.ix_([2,-1],[0,1])] #相当于在维度0上索引2和-1，然后在维度0索引结果的基础上，在维度1上索引0和1。
```


    array([[ 8,  9],
           [28, 29]])



3. ndarray在索引时支持传入一个布尔类型的ndarray进行索引，只选出布尔值为真的位置上对应的那些值。


```python
test_slice2[test_slice2>5] #如果布尔类型的ndarray与被索引的ndarray大小相同，则输出一个1维数组，里面包含了布尔类型的ndarray上为true的元素
```


    array([ 6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22,
           23, 24, 25, 26, 27, 28, 29, 30, 31])




```python
test_slice2[test_slice2[:,0]<10] #如果布尔类型的ndarray在某个维度上大小为1，则输出结果是一个array。
```


    array([[ 0,  1,  2,  3],
           [ 4,  5,  6,  7],
           [ 8,  9, 10, 11]])




```python
np.where(np.array([True,False,False,True]),np.array([1.1,1.2,1.3,1.4]),np.array([2.1,2.2,2.3,2.4])) #这一函数传入三个参数，第一个为一个布尔类型的ndarray，后两个为其他类型，可以是一个数字，也可以是一个相同规模的ndarray。按照布尔类型数组的值，如果为True则取第二个ndarray中对应位置的元素或是取相应的数字参数，False则从第三个ndarray中取值或是取相应的数字。
```


    array([1.1, 2.2, 2.3, 1.4])



# ndarray的运算

## ndarray的一元运算

ndarray的一元运算是对其中的每一个元素都进行运算操作。常用的操作如下：


```python
test=np.array([[0.5,-0.3,2.5,2.8],[3.8,2.1,-5.4,0.0]])
```


```python
test
```


    array([[ 0.5, -0.3,  2.5,  2.8],
           [ 3.8,  2.1, -5.4,  0. ]])




```python
np.abs(test) #输出ndarray内所有元素的绝对值，函数的返回值是一个与输入参数规模相同的ndarray。np.fabs()函数作用相同。
```


    array([[0.5, 0.3, 2.5, 2.8],
           [3.8, 2.1, 5.4, 0. ]])




```python
np.sqrt(test) #输出所有元素的二次方根，等同于arr**0.5，如果遇到负数则返回nan
```


    array([[0.70710678,        nan, 1.58113883, 1.67332005],
           [1.94935887, 1.44913767,        nan, 0.        ]])




```python
np.square(test) #输出所有元素的平方，等同于arr**2
```


    array([[ 0.25,  0.09,  6.25,  7.84],
           [14.44,  4.41, 29.16,  0.  ]])




```python
np.exp(test) #输出所有元素的e^x的值
```


    array([[1.64872127e+00, 7.40818221e-01, 1.21824940e+01, 1.64446468e+01],
           [4.47011845e+01, 8.16616991e+00, 4.51658094e-03, 1.00000000e+00]])




```python
np.log(test) #输出所有元素的ln(x)的值，类似的函数还有以10为底的np.log10()，以2为底的np.log2()，以及计算ln(1+x)的np.log1p()。如果数字不在函数的定义域内，则输出nan。
```


    array([[-0.69314718,         nan,  0.91629073,  1.02961942],
           [ 1.33500107,  0.74193734,         nan,        -inf]])




```python
np.sign(test) #输出每个元素的符号，1代表正数，0代表0，-1代表负数
```


    array([[ 1., -1.,  1.,  1.],
           [ 1.,  1., -1.,  0.]])




```python
np.ceil(test) #对每个元素向上取整
```


    array([[ 1., -0.,  3.,  3.],
           [ 4.,  3., -5.,  0.]])




```python
np.floor(test) #对每个元素向下取整
```


    array([[ 0., -1.,  2.,  2.],
           [ 3.,  2., -6.,  0.]])




```python
np.rint(test) #对每个元素做四舍五入求近似
```


    array([[ 0., -0.,  2.,  3.],
           [ 4.,  2., -5.,  0.]])




```python
np.modf(test) #返回两个ndarray，第一个包含每个元素的小数部分，第二个包含每个元素的整数部分
```


    (array([[ 0.5, -0.3,  0.5,  0.8],
            [ 0.8,  0.1, -0.4,  0. ]]),
     array([[ 0., -0.,  2.,  2.],
            [ 3.,  2., -5.,  0.]]))




```python
np.isnan(np.log2(test)) #返回一个布尔类型的ndarray，指示每个元素是否为nan
```


    array([[False,  True, False, False],
           [False, False,  True, False]])




```python
np.isinf(test) #返回一个布尔类型的ndarray，指示每个元素是否为inf。np.isfinite()的作用相同
```


    array([[False, False, False, False],
           [False, False, False, False]])




```python
np.cos(test) #np.cos()、np.sin()和np.tan()分别求每个元素的三角函数值；np.cosh()、np.sinh()和np.tanh()分别求每个元素的双曲三角函数值
```


    array([[ 0.87758256,  0.95533649, -0.80114362, -0.94222234],
           [-0.79096771, -0.5048461 ,  0.63469288,  1.        ]])




```python
np.arccos(test) #np.arccos()、np.arcsin()和np.arctan()分别求每个元素的反三角函数值；np.arccosh()、np.arcsinh()和np.arctanh()分别求每个元素的反双曲三角函数值。如果元素的值不合法，则返回nan。
```


    array([[1.04719755, 1.87548898,        nan,        nan],
           [       nan,        nan,        nan, 1.57079633]])




```python
np.logical_not(test) #返回每个元素取非逻辑运算之后的值。如果ndarray中的元素不是bool类型，则0为false，非零为true
```


    array([[False, False, False, False],
           [False, False, False,  True]])



## ndarray的二元运算

对于ndarray的二元运算，它是对两个ndarray相对应位置上的元素分别进行计算，输出结果是一个形状相同的二维数组。如果它们形状不同，则首先判断能否对ndarray做broadcast使之变为形状相同。如果可以则按照broadcast之后的ndarray计算，反之则报错。broadcast操作将在下面进行详细描述。

因此，如果是一个ndarray与一个标量进行计算，则相当于自动把标量做broadcast使之与ndarray具有相同的形状，然后再进行计算。

下面是一些常用的二元操作：


```python
test1=np.array([[1,2,3,4],[5,6,7,8]])
test2=np.array([[3,4,5,6],[7,8,9,0]])
```


```python
test1+test2 #等同于np.add(arr1,arr2)
```


    array([[ 4,  6,  8, 10],
           [12, 14, 16,  8]])




```python
test1-test2 #等同于np.subtract(arr1,arr2)
```


    array([[-2, -2, -2, -2],
           [-2, -2, -2,  8]])




```python
test1*test2 #等同于np.multiply(arr1,arr2)
```


    array([[ 3,  8, 15, 24],
           [35, 48, 63,  0]])




```python
test1/test2 #等同于np.divide(arr1,arr2)
```


    array([[0.33333333, 0.5       , 0.6       , 0.66666667],
           [0.71428571, 0.75      , 0.77777778,        inf]])




```python
test1//test2 #等同于np.floor_divide(arr1,arr2)
```


    array([[0, 0, 0, 0],
           [0, 0, 0, 0]], dtype=int32)




```python
test1**test2 #等同于np.power(arr1,arr2)
```


    array([[       1,       16,      243,     4096],
           [   78125,  1679616, 40353607,        1]], dtype=int32)




```python
test1%test2 #等同于np.mod(arr1,arr2)
```


    array([[1, 2, 3, 4],
           [5, 6, 7, 0]], dtype=int32)




```python
np.maximum(test1,test2) #输出每个位置上的最大值，可以传入任意多个参数。如果使用np.fmax()则忽略nan。相应地，求每个位置上的最小值有np.minimum()和np.fmin()两个函数。
```


    array([[3, 4, 5, 6],
           [7, 8, 9, 8]])




```python
test1>=test2 #等同于np.greater_equal(arr1,arr2)，返回一个布尔类型的ndarray，表示每个位置上第一个数组中的元素是否大于等于第二个数组中的元素。如果是则为True，反之为False。此外还有一些类似的用法：
# >和np.greater()：每个位置上第一个数组中的元素是否大于第二个
# <和np.less()：每个位置上第一个数组中的元素是否小于第二个
# <=和np.less_equal()：每个位置上第一个数组中的元素是否小于等于第二个
# ==和!=：判断两个数组中对应位置上的元素是否相等或不等
```


    array([[False, False, False, False],
           [False, False, False,  True]])




```python
np.logical_and(test1,test2) #对两个ndarray中的值做逻辑与运算。如果传入的两个ndarray不是布尔类型，则会自动做类型转换。如果两个ndarray是布尔类型，那么等同于arr1&arr2，注意如果不是布尔类型则这样做会出现非期望的结果。与之类似的用法还有：
# arr1|arr2和np.logical_or()：每个位置做或运算
# arr1^arr2和np.logical_xor()：每个位置做异或运算
```


    array([[ True,  True,  True,  True],
           [ True,  True,  True, False]])




```python
np.copysign(np.array([-2,2,-2,2]),np.array([1,-1,-1,1])) #将第二个ndarray中元素的符号赋给第一个ndarray的对应元素。相当于取第一个ndarray中元素的绝对值与第二个ndarray中元素的符号，组成一个新的ndarray。
```


    array([ 2., -2., -2.,  2.])



## ndarray的统计运算

ndarray支持一些类型的统计运算，例如：


```python
test3=np.array([[0.3,1.5,-0.5,2.4],[3.3,-5.5,8.1,-2.8]])
```


```python
np.sum(test3) #在没有指定维度的情况下计算所有元素的和，输出结果为一个数字。
```


    6.8




```python
np.sum(test3,axis=1) #计算指定维度上元素的和，输出结果为一个ndarray
```


    array([3.7, 3.1])




```python
np.mean(test3) #计算算数平均数。没有指定维度时对所有元素做计算，如果指定维度则按指定的维度进行计算
```


    0.85




```python
np.std(test3) #计算标准差，没有指定维度时对所有元素做计算，如果指定维度则按指定的维度进行计算
```


    3.826878623630491




```python
np.var(test3) #计算方差，没有指定维度时对所有元素做计算，如果指定维度则按指定的维度进行计算
```


    14.645000000000001




```python
np.min(test3) #计算最小值，没有指定维度时对所有元素做计算，如果指定维度则按指定的维度进行计算。计算最大值用np.max()
```


    -5.5




```python
np.argmin(test3) #输出最小值的indice，没有指定维度时对所有元素做计算，如果指定维度则按指定的维度进行计算。最大值的indice使用np.argmax()计算
```


    5




```python
np.cumprod(test3) #从0开始，计算元素的累乘结果，输出为一个ndarray，每一个元素表示从起始位置计算到当前位置时的值。没有指定维度时对所有元素做计算，如果指定维度则按指定的维度进行计算
```


    array([   0.3    ,    0.45   ,   -0.225  ,   -0.54   ,   -1.782  ,
              9.801  ,   79.3881 , -222.28668])




```python
np.cumsum(test3,axis=0) #从0开始计算元素的累加结果，输出一个ndarray。没有指定维度时对所有元素做计算，如果指定维度则按指定的维度进行计算
```


    array([[ 0.3,  1.5, -0.5,  2.4],
           [ 3.6, -4. ,  7.6, -0.4]])



对于布尔类型的ndarray有如下这些特殊的统计运算操作：


```python
test_bool=np.array([False,True,True,False,False,True,False])
```


```python
test_bool.sum() #统计True的个数
```


    3




```python
test_bool.any() #计算所有元素的或
```


    True




```python
test_bool.all() #计算所有元素的与
```


    False

> 备注：在NumPy的一些函数中，如果传入`axis=……`参数，则代表这一函数操作在这个传入的维度上面进行。假设有一个三维的array，它的元素可以用$a_{ijk}$来表示，当我们在`np.sum()`函数中传入`axis=1`，那么最终得到的结果便可以用数学公式表示为$\sum_{j=1}^{n}a_{ijk}$，也就是说这个操作是在维度$j$上进行的。



## 聚合运算

NumPy中的一些函数支持进一步的聚合操作，支持聚合操作的函数包括`add`、`multiply`、`subtract`、`logical_and`等。

`reduce`函数是对某个维度上的数据做连续的累计操作，最终得到的ndarray相比于传入的ndarray维度减1。例如：


```python
np.multiply.reduce(test3,axis=1)
```


    array([ -0.54 , 411.642])



`reduceat`函数与`reduce`函数类似，但是需要额外传入一个list，用于确定参与聚合计算的范围。例如：


```python
np.subtract.reduceat(test3,[0,2,3],axis=1) #假设传入的list中的元素为[a,b,c]，axis方向的元素个数为n，那么参与reduce运算的范围分别为[a,b-1],[b,c-1],[c,n-1]。
```


    array([[-1.2, -0.5,  2.4],
           [ 8.8,  8.1, -2.8]])



`accumulate`函数与`reduce`函数类似，但是会保留在这个维度上进行计算时的中间结果，例如：


```python
np.add.accumulate(test3,axis=1)
```


    array([[ 0.3,  1.8,  1.3,  3.7],
           [ 3.3, -2.2,  5.9,  3.1]])



`outer`函数传入两个ndarray，代表第一个与第二个ndarray中所有的元素可以组成的元素对进行计算。假设传入的两个ndarray为x和y，那么`outer`函数生成的ndarray的规模为(x.shape,y.shape)。例如：


```python
np.add.outer(test1,test2)
```


    array([[[[ 4,  5,  6,  7],
             [ 8,  9, 10,  1]],
    
            [[ 5,  6,  7,  8],
             [ 9, 10, 11,  2]],
    
            [[ 6,  7,  8,  9],
             [10, 11, 12,  3]],
    
            [[ 7,  8,  9, 10],
             [11, 12, 13,  4]]],
    
           [[[ 8,  9, 10, 11],
             [12, 13, 14,  5]],
    
            [[ 9, 10, 11, 12],
             [13, 14, 15,  6]],
    
            [[10, 11, 12, 13],
             [14, 15, 16,  7]],
    
            [[11, 12, 13, 14],
             [15, 16, 17,  8]]]])



## 数组的线性代数运算

NumPy提供了一些线性代数运算。需要注意的是，本节中的一些操作可以对大于二维的array进行计算，如果要这样使用需要注意维度大小上的要求，并清楚运算的数学含义。


```python
test_linalg1=np.random.rand(3,4)
test_linalg2=np.random.rand(4,3)
test_linalg3=np.random.rand(4,4)
```


```python
test_linalg1
```


    array([[0.67166004, 0.67521641, 0.92385308, 0.17172312],
           [0.08287326, 0.91389572, 0.50537867, 0.44940627],
           [0.49319114, 0.24830729, 0.7845657 , 0.0541078 ]])




```python
test_linalg2
```


    array([[0.39854758, 0.88405379, 0.46193288],
           [0.17077872, 0.48180729, 0.4009363 ],
           [0.21988187, 0.74787762, 0.12843288],
           [0.87389188, 0.35833092, 0.58089653]])




```python
test_linalg3
```


    array([[0.91681047, 0.0559406 , 0.90109644, 0.85956406],
           [0.5495678 , 0.01228935, 0.40510975, 0.00860076],
           [0.63290332, 0.88460151, 0.58796459, 0.25524597],
           [0.0586652 , 0.16806904, 0.28365241, 0.29483814]])




```python
np.dot(test_linalg1,test_linalg2) #计算矩阵乘法，注意两个矩阵的大小必须要满足矩阵乘法的要求!
```


    array([[0.73620706, 1.67157054, 0.7993871 ],
           [0.69295897, 1.0525836 , 0.73066163],
           [0.45876187, 1.16179138, 0.45957167]])



在NumPy中还有一个功能与`np.dot`相似的函数：`np.matmul`。当array为二维时，它与`np.dot`的计算结果相同。但是在多维的情况下，`matmul`会将array看作是多个二维数组的堆叠，在计算时取两个array的最后两个维度，按照矩阵乘法的规则来计算。而`dot`则是按照如下规则来计算：dot(a, b)[i,j,k,m] = sum(a[i,j,:] * b[k,:,m])。

例如一个array的维度为(9,5,7,4)，另一个为(9,5,4,3)，那么使用`matmul`得到的结果是维度为(9,5,7,3)的数组，而使用`dot`得到的则是(9,5,7,9,5,3)。




```python
test_linalg1.dot(test_linalg2) #与上面的语句等价
```


    array([[0.73620706, 1.67157054, 0.7993871 ],
           [0.69295897, 1.0525836 , 0.73066163],
           [0.45876187, 1.16179138, 0.45957167]])




```python
test_linalg1.T #矩阵的转置操作
```


    array([[0.67166004, 0.08287326, 0.49319114],
           [0.67521641, 0.91389572, 0.24830729],
           [0.92385308, 0.50537867, 0.7845657 ],
           [0.17172312, 0.44940627, 0.0541078 ]])




```python
test_transpose=np.random.rand(2,5,4,3)
np.shape(test_transpose.T) #ndarray.T转置操作的实质相当于是将原数组的维度倒过来，例如原来位于位置(0,1,2,3)的元素在转置之后的位置变为(3,2,1,0)
```


    (3, 4, 5, 2)




```python
np.linalg.inv(test_linalg3) #求矩阵的逆，必须传入一个正方形的数组
```


    array([[ 1.41156052, -0.98749103,  0.83882032, -4.81260336],
           [-0.21076796, -1.03011343,  1.23951372, -0.42854889],
           [-1.94482628,  3.90240586, -1.18111701,  6.57857386],
           [ 1.71032402, -2.97066486,  0.26283316, -1.73542744]])




```python
np.linalg.pinv(test_linalg2) #求矩阵的伪逆，伪逆矩阵指的是，对于一个矩阵A，存在一个矩阵B，满足(AB)^T=AB，(BA)^T=BA
```


    array([[-0.46907312, -1.92370125,  1.09489796,  1.45867813],
           [ 0.48237103, -0.25944216,  1.15432715, -0.45973217],
           [ 0.54234896,  2.95669978, -2.44534867, -0.20987198]])




```python
np.diag(test_linalg2) #第一种用法：取一个矩阵的对角线元素组成一个一维数组（可以不是正方形矩阵）
```


    array([0.39854758, 0.48180729, 0.12843288])




```python
np.diag(np.diag(test_linalg3)) #第二种用法：将一个一维的数组转变为一个二维的正方形对角矩阵，非对角的元素全为0
```


    array([[0.91681047, 0.        , 0.        , 0.        ],
           [0.        , 0.01228935, 0.        , 0.        ],
           [0.        , 0.        , 0.58796459, 0.        ],
           [0.        , 0.        , 0.        , 0.29483814]])




```python
np.trace(test_linalg2) #计算所有对角元素的和，传入的矩阵可以不必是正方形矩阵
```


    1.0087877438514739




```python
np.linalg.det(test_linalg3) #计算正方形矩阵对应行列式的值
```


    -0.060734760345177434




```python
np.linalg.qr(test_linalg2) #对矩阵做QR分解
```


    (array([[-0.39853949, -0.55826311,  0.1397547 ],
            [-0.17077525, -0.34967478,  0.76189453],
            [-0.2198774 , -0.58700406, -0.63012748],
            [-0.87387414,  0.4706337 , -0.05408067]]),
     array([[-1.0000203 , -0.91218862, -0.78843843],
            [ 0.        , -0.93237507, -0.20007854],
            [ 0.        ,  0.        ,  0.2576841 ]]))




```python
np.linalg.svd(test_linalg2)#对矩阵做奇异值分解
```


    (array([[-0.6124607 ,  0.31110915,  0.13463208, -0.71412688],
            [-0.36061856,  0.14909045,  0.76134499,  0.51776452],
            [-0.4109163 ,  0.50488233, -0.60516287,  0.45827887],
            [-0.57095879, -0.79125025, -0.18975224,  0.10919333]]),
     array([1.72222085, 0.64558318, 0.21609566]),
     array([[-0.51967203, -0.71251265, -0.4714517 ],
            [-0.66761255,  0.68299619, -0.29632698],
            [-0.53313644, -0.16075423,  0.83061641]]))




```python
test_linalg4=np.random.rand(4,1)
```


```python
np.linalg.lstsq(test_linalg3,test_linalg4) #解方程Ax=b，A为正方形矩阵，b为一维矩阵，或者第二个维度为1的二维矩阵
```


    (array([[-0.85081594],
            [ 0.23595236],
            [ 2.3445102 ],
            [-0.76479464]]),
     array([], dtype=float64),
     4,
     array([1.97814445, 0.75043168, 0.40950042, 0.09991112]))




```python
np.linalg.norm(test_linalg2,ord=2,axis=1) #求矩阵的范数，ord代表范数的阶数，axis代表在某个维度上求范数（结果为一维数组），如果不传入axis参数则对整个矩阵求范数（结果是一个数字）
```


    array([1.07413838, 0.64965648, 0.79004049, 1.10884122])



对于numpy中的ndarray类，如果要对其做线性代数运算，需要写成上面那些函数调用的形式，比较麻烦。为了便于对一个矩阵做线性代数运算，我们可以将一个ndarray转换为一个matrix类。这样，就可以像matlab一样，直接使用运算符。例：


```python
X1=np.matrix(test_linalg1)
X2=np.matrix(test_linalg2)
```


```python
X1*X2 #矩阵乘法可以直接表示为公式计算
```


    matrix([[0.73620706, 1.67157054, 0.7993871 ],
            [0.69295897, 1.0525836 , 0.73066163],
            [0.45876187, 1.16179138, 0.45957167]])



## 自定义函数

NumPy可以将一个自定义函数从标量运算转变为支持numpy数组运算的函数，从而使得ndarray可以作为函数的参数。


```python
def add_elements(x,y):
    return x+y
```


```python
add_them=np.frompyfunc(add_elements,2,1) #第一个参数为函数名，第二个和第三个参数分别代表输入和输出参数的个数
```


```python
add_them=np.vectorize(add_elements) #与上面的函数等价
```


```python
add_them(np.arange(4),np.arange(4))
```


    array([0, 2, 4, 6], dtype=object)



# 排序与筛选操作

## 排序

numpy中的排序函数可以对元素进行排序。同时，这些排序函数支持三种类型的排序：quicksort、mergesort、heapsort，可以使用kind参数传入排序函数内。


```python
test_sort1=np.array([2,3,4,1,4,2,5,8,3])
test_sort2=np.array([6,3,7,3,5,2,6,2,7,1])
```


```python
test_sort2.sort(kind='mergesort') #对ndarray的元素做排序，这一命令会将ndarray中的元素变为有序。可以传入一个数字表示在哪个维度上排序，默认为数值最大方向。
```


```python
test_sort2
```


    array([1, 2, 2, 3, 3, 5, 6, 6, 7, 7])




```python
np.sort(test_sort1) #同样是排序，但是输出结果是一个新的ndarray
```


    array([1, 2, 2, 3, 3, 4, 4, 5, 8])




```python
test_sort1.argsort(kind='quicksort') #生成有序排列所对应的元素序号顺序，也即test_sort1[test_sort1.argsort()]可以得到排序后的数组。如果要稳定排序则只能使用mergesort
```


    array([3, 0, 5, 1, 8, 2, 4, 6, 7], dtype=int64)



numpy中的searchsorted函数可以在已经排序好的array中做二分查找，返回一个表示位置的整数，被查找元素插入到这个位置上时不会影响到array中的元素排序。

如果有相同元素，在返回位置时会默认返回最左端那个元素的位置。也可以传入参数使其返回右端元素的位置。例：


```python
test_sort2.searchsorted([2,3])
```


    array([1, 3], dtype=int64)




```python
test_sort2.searchsorted([2,3],side='right')
```


    array([3, 5], dtype=int64)



## 筛选操作


```python
np.unique(test_sort1) #去除重复元素并排序
```


    array([1, 2, 3, 4, 5, 8])




```python
np.intersect1d(test_sort1,test_sort2) #求两个ndarray中元素的交集，结果为一个1维的array
```


    array([1, 2, 3, 5])




```python
np.union1d(test_sort1,test_sort2) #求两个ndarray中元素的并集，结果为一个1维的array
```


    array([1, 2, 3, 4, 5, 6, 7, 8])




```python
np.in1d(test_sort1,test_sort2) #输出一个布尔类型的ndarray，表示第一个ndarray中的元素是否出现在第二个ndarray中
```


    array([ True,  True, False,  True, False,  True,  True, False,  True])




```python
np.setdiff1d(test_sort1,test_sort2) #输出第一个ndarray中，所有与第二个ndarray的元素都不相同的值，结果是一个1维的array
```


    array([4, 8])




```python
np.setxor1d(test_sort1,test_sort2) #输出只出现在其中一个ndarray中的所有元素，结果是一个1维的array
```


    array([4, 6, 7, 8])



# ndarray的形状改变


```python
test_shape=np.arange(20)
```


```python
test_shape
```


    array([ 0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16,
           17, 18, 19])




```python
test_shape.reshape((2,2,5)) #将原数组按照reshape传入的tuple改变形状，tuple中元素个数对应于维度，每个元素的值对应于该维度上的大小，所有维度大小的乘积必须等于总的元素个数。
# 对于reshape前后元素是如何对应到具体的位置，只需要记住在print出ndarray中内容的时候，将print的内容按照从上到下从左到右的顺序排列起来之后，这一顺序始终保持不变。
```


    array([[[ 0,  1,  2,  3,  4],
            [ 5,  6,  7,  8,  9]],
    
           [[10, 11, 12, 13, 14],
            [15, 16, 17, 18, 19]]])




```python
test_shape.reshape(5,-1) #在使用reshape函数时，某一个维度上的值可以为-1，然后会自动根据其他维度的值以及元素总个数来推断这个维度的大小。
```


    array([[ 0,  1,  2,  3],
           [ 4,  5,  6,  7],
           [ 8,  9, 10, 11],
           [12, 13, 14, 15],
           [16, 17, 18, 19]])




```python
test_shape2=test_shape.reshape((2,2,5))
test_shape2.transpose((2,0,1)) #任意维数组的转置，需要传入包含0—n-1的n个整数的一个tuple。这种操作可以理解为改变了矩阵元素的“坐标值”，比如之前位置为(3,1,2)的元素在按照(2,0,1)的参数进行转置时，这一元素的新位置为(2,3,1)
```


    array([[[ 0,  5],
            [10, 15]],
    
           [[ 1,  6],
            [11, 16]],
    
           [[ 2,  7],
            [12, 17]],
    
           [[ 3,  8],
            [13, 18]],
    
           [[ 4,  9],
            [14, 19]]])




```python
test_shape2
```


    array([[[ 0,  1,  2,  3,  4],
            [ 5,  6,  7,  8,  9]],
    
           [[10, 11, 12, 13, 14],
            [15, 16, 17, 18, 19]]])




```python
test_shape2.flatten('C') #将一个高维数组变成一维，可以传入参数'C'或'F'。其中，'C'代表按照shape属性中从后往前的顺序进行遍历，即先内后外；'F'代表按照shape属性中从前往后的顺序进行遍历，即先外后内
```


    array([ 0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16,
           17, 18, 19])




```python
test_shape2.ravel('F') #功能同flatten()函数
```


    array([ 0, 10,  5, 15,  1, 11,  6, 16,  2, 12,  7, 17,  3, 13,  8, 18,  4,
           14,  9, 19])



# 数组的合并与分离

## 数组的合并

将两个数组合并，可以使用下面这些操作：


```python
np.concatenate([test_shape2,test_shape2],axis=2) #将两个ndarray在axis的方向上进行拼接
```


    array([[[ 0,  1,  2,  3,  4,  0,  1,  2,  3,  4],
            [ 5,  6,  7,  8,  9,  5,  6,  7,  8,  9]],
    
           [[10, 11, 12, 13, 14, 10, 11, 12, 13, 14],
            [15, 16, 17, 18, 19, 15, 16, 17, 18, 19]]])




```python
np.vstack((test_shape2,test_shape2)) #将两个数组在垂直方向进行拼接，即axis=0的方向，等同于row_stack()。
```


    array([[[ 0,  1,  2,  3,  4],
            [ 5,  6,  7,  8,  9]],
    
           [[10, 11, 12, 13, 14],
            [15, 16, 17, 18, 19]],
    
           [[ 0,  1,  2,  3,  4],
            [ 5,  6,  7,  8,  9]],
    
           [[10, 11, 12, 13, 14],
            [15, 16, 17, 18, 19]]])




```python
np.hstack([test_shape2,test_shape2]) #将两个数组在水平方向进行拼接，即axis=1的方向。与之相似的函数为column_stack()，但是这一函数会默认先将一维数组转为二维数组，然后再做拼接。
```


    array([[[ 0,  1,  2,  3,  4],
            [ 5,  6,  7,  8,  9],
            [ 0,  1,  2,  3,  4],
            [ 5,  6,  7,  8,  9]],
    
           [[10, 11, 12, 13, 14],
            [15, 16, 17, 18, 19],
            [10, 11, 12, 13, 14],
            [15, 16, 17, 18, 19]]])




```python
np.dstack([test_shape2,test_shape2]) #将两个数组在深度方向进行拼接，即axis=2的方向
```


    array([[[ 0,  1,  2,  3,  4,  0,  1,  2,  3,  4],
            [ 5,  6,  7,  8,  9,  5,  6,  7,  8,  9]],
    
           [[10, 11, 12, 13, 14, 10, 11, 12, 13, 14],
            [15, 16, 17, 18, 19, 15, 16, 17, 18, 19]]])



## 数组的复制

下面这些函数可以将数组中的元素复制一定的次数，从而形成一个更大的数组：


```python
test_shape2.repeat([2,3],axis=1) #repeat函数将原数组中的元素在某个方向上按照从0到n-1的顺序进行复制并拼接起来。需要传入一个list（或一个数字）和一个axis参数。
#其中，axis参数代表复制的方向。而list中需要传入1个或者n个整数参数：传入1个参数代表将所有值都按照这个大小进行复制（与传入一个数字的作用相同）；传入n个参数代表每个元素相应地重复不同的次数。
```


    array([[[ 0,  1,  2,  3,  4],
            [ 0,  1,  2,  3,  4],
            [ 5,  6,  7,  8,  9],
            [ 5,  6,  7,  8,  9],
            [ 5,  6,  7,  8,  9]],
    
           [[10, 11, 12, 13, 14],
            [10, 11, 12, 13, 14],
            [15, 16, 17, 18, 19],
            [15, 16, 17, 18, 19],
            [15, 16, 17, 18, 19]]])




```python
np.tile(test_shape2,(2,1,2)) #tile()函数则是对整个数组进行复制并拼接。第一个参数代表待复制的数组，第二个参数为一个tuple，代表每个维度上面复制的次数。
```


    array([[[ 0,  1,  2,  3,  4,  0,  1,  2,  3,  4],
            [ 5,  6,  7,  8,  9,  5,  6,  7,  8,  9]],
    
           [[10, 11, 12, 13, 14, 10, 11, 12, 13, 14],
            [15, 16, 17, 18, 19, 15, 16, 17, 18, 19]],
    
           [[ 0,  1,  2,  3,  4,  0,  1,  2,  3,  4],
            [ 5,  6,  7,  8,  9,  5,  6,  7,  8,  9]],
    
           [[10, 11, 12, 13, 14, 10, 11, 12, 13, 14],
            [15, 16, 17, 18, 19, 15, 16, 17, 18, 19]]])



## 数组的分割

下面的函数可以将一个ndarray分成多个：


```python
np.split(test_shape2,[0,2],axis=2) #传入三个参数，第一个为待分离的数组，第二个代表分割的位置，第三个代表进行分割操作的维度。
#假设传入的分割位置为[a1,a2,…,an]，axis参数所对应的那个维度的大小为a，则最终分割之后在axis方向上所对应的原数组内容为：[0,a1-1],[a1,a2-1],……,[an,a-1]。如果区间内左边的数字大于右边，则形成空数组。
```


    [array([], shape=(2, 2, 0), dtype=int32),
     array([[[ 0,  1],
             [ 5,  6]],
     
            [[10, 11],
             [15, 16]]]),
     array([[[ 2,  3,  4],
             [ 7,  8,  9]],
     
            [[12, 13, 14],
             [17, 18, 19]]])]




```python
np.hsplit(test_shape2,[1]) #在水平方向，即axis=0的方向上做分割
```


    [array([[[ 0,  1,  2,  3,  4]],
     
            [[10, 11, 12, 13, 14]]]),
     array([[[ 5,  6,  7,  8,  9]],
     
            [[15, 16, 17, 18, 19]]])]




```python
np.vsplit(test_shape2,[1]) #在竖直方向，即axis=1的方向上做分割
```


    [array([[[0, 1, 2, 3, 4],
             [5, 6, 7, 8, 9]]]),
     array([[[10, 11, 12, 13, 14],
             [15, 16, 17, 18, 19]]])]




```python
np.dsplit(test_shape2,[2]) #在深度方向，即axis=2的方向上做分割
```


    [array([[[ 0,  1],
             [ 5,  6]],
     
            [[10, 11],
             [15, 16]]]),
     array([[[ 2,  3,  4],
             [ 7,  8,  9]],
     
            [[12, 13, 14],
             [17, 18, 19]]])]





# Broadcast操作

前面已经提到，在对两个形状不同的数/数组进行运算操作的时候，可能会对数字或者数组做一些扩展操作。

只有当两个array相容时才能做数据扩展，即它们每个维度上的长度按照从右往左的顺序进行比较时，要么在某个维度上二者维度相等，要么某个维度上其中一个array的长度为1（如果缺失则可以看作为1）。如果两个array相容，会在长度为1的维度上通过复制元素将array扩展至与另一个array长度相同。

下面为一些broadcast操作的图例：

![1](https://raw.githubusercontent.com/lyf35/blog_figures/main/img/20210105233413.png)

![2](https://raw.githubusercontent.com/lyf35/blog_figures/main/img/20210105233455.png)

![3](https://raw.githubusercontent.com/lyf35/blog_figures/main/img/20210105233518.png)

可以在slide操作中结合np.newaxis，手动添加一个新的维度，例如：


```python
test_shape2[:,np.newaxis,:,:]
```


    array([[[[ 0,  1,  2,  3,  4],
             [ 5,  6,  7,  8,  9]]]
           [[[10, 11, 12, 13, 14],
             [15, 16, 17, 18, 19]]]])



# 参考

1. Python for Data Analysis, 2nd Edition.