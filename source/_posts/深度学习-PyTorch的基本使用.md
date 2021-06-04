---
title: 深度学习-PyTorch的使用
tags: 
- 深度学习
categories: [工程实践, 深度学习]
sharing: True
date: 2021-04-18 23:47:02
updated: 2021-04-28 23:52:19
---


```python
import torch
import numpy as np
import torchvision
import torchaudio
import torchtext
import torch_geometric
```


# torch.Tensor

## 概述

在PyTorch中，可以创建类似于NumPy中的多维数组（ndarray），在PyTorch中对应的数据类型是`torch.Tensor`。在一个tensor中，所有的数据具有相同的数据类型。

tensor类型的两个重要属性为：

- dtype：表示tensor的数据类型
- device：表示tensor所存放的位置

关于tensor的操作可参考文档：https://pytorch.org/docs/stable/tensors.html
<!--more-->

## 数据类型

一个tensor可以使用的数据类型包含16位、32位和64位的整数和浮点数等，且分为CPU tensor和GPU tensor详情可以查看：https://pytorch.org/docs/stable/tensor_attributes.html#torch.torch.dtype

默认情况下，PyTorch的tensor是`FloatTensor`的数据类型。使用`torch.Tensor()`创建一个tensor时便会使用默认的数据类型。


```python
x=torch.Tensor([1,2])
print(x.dtype, x.device)
```

    torch.float32 cpu


要改变默认数据类型，可以使用`torch.set_default_tensor_type()`函数。注意默认数据类型只支持浮点数类型。


```python
torch.set_default_tensor_type(torch.cuda.FloatTensor)
y=torch.Tensor([1,2])
print(y.dtype, y.device)
```

    torch.float32 cuda:0


各种数据类型之间可以互相转换，在转换数据类型之后会生成一个新的变量。


```python
a=x.type_as(y)
print(a.dtype, a.device)
```

    torch.float32 cuda:0



```python
b=y.type(torch.int32)
print(b.dtype, b.device)
```

    torch.int32 cuda:0


## Tensor的创建

创建一个Tensor主要包括下面几类函数：
- `torch.tensor()`：从一个已存在的数据中创建Tensor，可以传入的数据包括np.array、list等
- `torch.*()`：创建一个具有特定尺寸的Tensor，这类函数常用的包括`torch.ones()`, `torch.eye()`, `torch.zeros()`等，创建方法可参考https://pytorch.org/docs/stable/torch.html#tensor-creation-ops
- `torch.*_like()`：创建一个与已知Tensor尺寸相同的Tensor，这类函数包括`torch.ones_like()`, `torch.zeros_like()`等，大多数`torch.*()`函数都有一个`torch.*_like()`函数与之相对应
- `tensor.new_*()`：创建一个与已知Tensor数据格式相同但是尺寸不同的Tensor，这类函数包括`torch.new_ones()`, `torch.new_zeros()`等

在这些函数中，可以传入`dtype`属性指定Tensor的数据类型，以及`device`属性指定Tensor保存的位置。

下面为一些用法示例：


```python
torch.set_default_tensor_type(torch.FloatTensor)
```


```python
torch.tensor([[1,2,3],[4,5,6]],device='cuda') #从list中创建一个Tensor，可以手动传入device参数来设置tensor保存在哪个设备上。函数会自动根据list中的元素去推断数据类型，如此例中会自动推断为整数
```




    tensor([[1, 2, 3],
            [4, 5, 6]], device='cuda:0')




```python
torch.tensor([[1,2,3],[4,5,6]],dtype=torch.float32) #也可以手动传入dtype参数来设置元素类型
```




    tensor([[1., 2., 3.],
            [4., 5., 6.]])




```python
torch.tensor(np.random.rand(2,3)) #从NumPy的ndarray创建一个Tensor
```




    tensor([[0.3075, 0.6307, 0.0517],
            [0.1215, 0.5329, 0.5438]], dtype=torch.float64)




```python
torch.from_numpy(np.zeros((2,2))) #这一函数接受一个NumPy的ndarray，将其转为tensor
```




    tensor([[0., 0.],
            [0., 0.]], dtype=torch.float64)




```python
torch.ones((2,3)) #传入一个表示尺寸的tuple/list，构造一个元素全为1的Tensor
```




    tensor([[1., 1., 1.],
            [1., 1., 1.]])




```python
torch.zeros([3,4]) #传入一个表示尺寸的tuple/list，构造一个元素全为0的Tensor
```




    tensor([[0., 0., 0., 0.],
            [0., 0., 0., 0.],
            [0., 0., 0., 0.]])




```python
torch.arange(1,3,0.2) #生成一个元素从起始值到结束值按照步长递增的一维Tensor。第一个参数为起始值（包含），第二个为结束值（不包含），第三个为步长
```




    tensor([1.0000, 1.2000, 1.4000, 1.6000, 1.8000, 2.0000, 2.2000, 2.4000, 2.6000,
            2.8000])




```python
torch.linspace(1,2,10) #生成一个元素值均匀递增的Tensor，步长由设定的元素个数而定。第一个参数为起始值（包含），第二个为结束值（包含），第三个为元素个数
```




    tensor([1.0000, 1.1111, 1.2222, 1.3333, 1.4444, 1.5556, 1.6667, 1.7778, 1.8889,
            2.0000])




```python
torch.logspace(-2,3,5,base=2) #生成一个元素值在对数坐标下均匀递增的Tensor。第一个参数是起始的指数（包含），第二个参数是结束的指数（包含），第三个参数代表在起始和结束指数之间均匀分成多少份，base参数默认值为10
```




    tensor([0.2500, 0.5946, 1.4142, 3.3636, 8.0000])




```python
a=torch.eye(5) #5*5的单位矩阵，对角线元素全为1，其余元素全为0
torch.randn_like(a)
```




    tensor([[ 0.4063,  0.7966, -0.1545, -0.1510, -0.3982],
            [ 2.4264,  0.8100,  0.8010, -0.4190, -1.2513],
            [-0.0121,  0.3558, -0.3074,  1.0870, -0.4386],
            [ 0.5292,  0.5246,  0.7431, -0.1588, -0.2249],
            [-1.6491,  0.0356, -0.3694,  0.7242,  0.3513]])




```python
a.new_full((2,2),4) #按照full()函数的方式生成一个Tensor，其数据类型由a的数据类型来确定。full()函数需要传入一个表示尺寸的tuple/list，以及要填充的值，Tensor中的所有元素都会拿这个值进行填充
```




    tensor([[4., 4.],
            [4., 4.]])



PyTorch中，CPU tensor和NumPy的ndarray之间可以互相转换，且共享内存。如果是GPU tensor则无法直接转换为NumPy的ndarray，需要先转为CPU tensor才可以继续转为NumPy的ndarray。


```python
test=torch.tensor([[2,3,4],[5,6,7]])
test
```




    tensor([[2, 3, 4],
            [5, 6, 7]])




```python
test_np=test.numpy()
```


```python
test_np
```




    array([[2, 3, 4],
           [5, 6, 7]], dtype=int64)




```python
test_np[0,0]=10
test_np
```




    array([[10,  3,  4],
           [ 5,  6,  7]], dtype=int64)




```python
test #可以看到二者共享内存
```




    tensor([[10,  3,  4],
            [ 5,  6,  7]])




```python
test2=torch.tensor([[2,3,4],[5,6,7]],device='cuda')
test2.numpy() #GPU tensor无法直接转为NumPy的array
```


    ---------------------------------------------------------------------------
    
    TypeError                                 Traceback (most recent call last)
    
    <ipython-input-23-916e847cb1c4> in <module>
          1 test2=torch.tensor([[2,3,4],[5,6,7]],device='cuda')
    ----> 2 test2.numpy() #GPU tensor无法直接转为NumPy的array


    TypeError: can't convert cuda:0 device type tensor to numpy. Use Tensor.cpu() to copy the tensor to host memory first.



```python
test2.cpu().numpy()
```




    array([[2, 3, 4],
           [5, 6, 7]], dtype=int64)



也可以将tensor转为一个list：


```python
test.tolist()
```




    [[10, 3, 4], [5, 6, 7]]



要通过一个CPU tensor生成元素相同的CUDA tensor，可以用`to('cuda')`或者`cuda()`函数。而相反地，如果要通过一个CUDA tensor生成一个cpu tensor，则可以用`to('cpu')`或者`cpu()`函数。用法如下：


```python
test.to('cuda') #生成一个副本
```




    tensor([[10,  3,  4],
            [ 5,  6,  7]], device='cuda:0')




```python
test=test.cuda() #同样是生成一个副本，要对原变量做修改需要重新赋值
```


```python
test.cpu() #同样是生成一个副本
```




    tensor([[10,  3,  4],
            [ 5,  6,  7]])




```python
test=test.to('cpu')
```

## 维度操作

与NumPy中对ndarray的维度操作类似，tensor也支持一系列的维度转换操作。PyTorch中关于tensor的维度操作可参考https://pytorch.org/docs/stable/tensor_view.html

下面为一些常用的维度操作函数：


```python
test=torch.tensor([[1,2,3,4,5,6],[7,8,9,10,11,12]])
```


```python
test.size() #查看一个tensor的形状
```




    torch.Size([2, 6])




```python
test.view(1,-1,2,3) #修改一个tensor的形状，需要传入若干个表示维度尺寸的整数。尺寸中可以有一个-1，这样的话该维度上的尺寸会根据其它维度上的尺寸自动推断。这一操作会生成一个副本
```




    tensor([[[[ 1,  2,  3],
              [ 4,  5,  6]],
    
             [[ 7,  8,  9],
              [10, 11, 12]]]])




```python
test.unsqueeze(0) #给tensor增加一个维度，需要传入一个整数，代表增加的是哪个维度。在这个维度上的尺寸为1。这一函数会生成一个副本
```




    tensor([[[ 1,  2,  3,  4,  5,  6],
             [ 7,  8,  9, 10, 11, 12]]])




```python
test.unsqueeze_(1) #如果要做就地操作可以使用unsqueeze_()函数
test.unsqueeze_(3)
```




    tensor([[[[ 1],
              [ 2],
              [ 3],
              [ 4],
              [ 5],
              [ 6]]],


​    
​            [[[ 7],
​              [ 8],
​              [ 9],
​              [10],
​              [11],
​              [12]]]])




```python
test.squeeze() #squeeze函数会将尺寸为1的维度删除，如果不传入任何参数则是删除掉所有尺寸为1的维度，也可以传入一个数字代表删除掉指定的维度。这一函数生成一个副本，如果要做就地操作可以使用squeeze_()函数
```




    tensor([[ 1,  2,  3,  4,  5,  6],
            [ 7,  8,  9, 10, 11, 12]])




```python
test.resize(3,4) #与view函数类似，改变一个tensor的形状，但是不支持传入-1做尺寸推算
```

    D:\software\Anaconda\envs\pytorch18\lib\site-packages\torch\tensor.py:474: UserWarning: non-inplace resize is deprecated
      warnings.warn("non-inplace resize is deprecated")





    tensor([[ 1,  2,  3,  4],
            [ 5,  6,  7,  8],
            [ 9, 10, 11, 12]])



## 索引操作

PyTorch支持NumPy风格的索引操作，包括切片、bool语句、list索引等，同时也有一些特殊的索引函数。


```python
test=torch.tensor([[[1,2,3,4,5,6],[7,8,9,10,11,12]],[[13,14,15,16,17,18],[19,20,21,22,23,24]]])
```


```python
test[:,1,::2] #切片索引
```




    tensor([[ 7,  9, 11],
            [19, 21, 23]])




```python
test[test>5] #使用布尔语句索引
```




    tensor([ 6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23,
            24])




```python
test[:,:,[0,2,3]] #list索引与切片索引结合
```




    tensor([[[ 1,  3,  4],
             [ 7,  9, 10]],
    
            [[13, 15, 16],
             [19, 21, 22]]])




```python
test.index_select(2,torch.tensor([0,2])) #第一个参数为指定的维度；第二个参数需要传入一个整数类型的tensor，作为选取的index
```




    tensor([[[ 1,  3],
             [ 7,  9]],
    
            [[13, 15],
             [19, 21]]])




```python
test.masked_select(test>12) #使用bool类型的tensor作为掩码来索引
```




    tensor([13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24])




```python
test.gather(2,torch.tensor([[[0,2,1,2,3,0],[0,1,2,3,2,1]],[[0,1,2,3,2,1],[0,2,1,2,3,0]]])) #gather操作需要传入一个维度，以及一个与tensor的大小完全相同的整数类型tensor作为index。gather函数会在这个维度上，按照传入的整数tensor进行索引，得到一个与原tensor大小完全相同的tensor。
# 以二维tensor为例，gather操作可以写成如下的表达式：
# out[i][j]=input[index[i][j]][j] (dim=0)
# out[i][j]=input[i][index[i][j]] (dim=1)
```




    tensor([[[ 1,  3,  2,  3,  4,  1],
             [ 7,  8,  9, 10,  9,  8]],
    
            [[13, 14, 15, 16, 15, 14],
             [19, 21, 20, 21, 22, 19]]])



## 数学运算

tensor的运算与NumPy中ndarray的运算类似，可以使用运算符，也可以使用函数。

tensor支持的逐元素运算包括
- `abs`, `sqrt`, `div`, `exp`, `fmod`, `log`, `pow`等基本数学运算
- `cos`, `sin`, `asin`, `atan2`, `cosh`等三角函数运算
- `ceil`, `round`, `floor`, `trunc`等小数的舍入运算和`clamp`截断运算
- `sigmoid`, `tanh`等激活函数运算

这些函数分为两类，一类是不带下划线版本的，如`exp`，表示生成一个副本；另一类是带有下划线版本的，如`exp_`，表示就地修改。


```python
test=torch.randn((3,3))
```


```python
test
```




    tensor([[ 0.2565,  1.5239, -1.3304],
            [ 1.6938,  0.3045,  0.6899],
            [-0.8050,  1.0737, -0.4119]])




```python
torch.exp(test)
```




    tensor([[1.2923, 4.5901, 0.2644],
            [5.4402, 1.3559, 1.9934],
            [0.4471, 2.9261, 0.6624]])




```python
test
```




    tensor([[ 0.2565,  1.5239, -1.3304],
            [ 1.6938,  0.3045,  0.6899],
            [-0.8050,  1.0737, -0.4119]])




```python
torch.tanh_(test)
```




    tensor([[ 0.2510,  0.9094, -0.8693],
            [ 0.9346,  0.2954,  0.5979],
            [-0.6668,  0.7908, -0.3901]])




```python
test
```




    tensor([[ 0.2510,  0.9094, -0.8693],
            [ 0.9346,  0.2954,  0.5979],
            [-0.6668,  0.7908, -0.3901]])



tensor支持的归并运算包括：
- `mean`, `sum`, `median`, `mode`, `std`, `var`等统计量
- `norm`, `dist`计算范数和距离
- `cumsum`, `cumprod`累加与累乘运算

以上的大多数函数都有一个参数`dim`，用于指定归并操作在哪个维度上执行。默认情况下，最终得到的tensor维度会减小，如果不想让tensor维度发生变化则可以传入`keepdim=True`来保留归并操作的维度。


```python
torch.norm(test) #如果不指定维度则计算矩阵范数
```




    tensor(2.0460)




```python
torch.norm(test,dim=1) #指定维度则计算向量范数
```




    tensor([1.2829, 1.1482, 1.1056])



tensor支持的比较与排序函数包括：

- `gt`, `lt`, `ge`, `le`, `eq`, `ne`等比较函数
- `topk`找出最大的k个数与`max`, `min`求最值
- `sort`排序函数


```python
torch.gt(test,0.0)
```




    tensor([[ True,  True, False],
            [ True,  True,  True],
            [False,  True, False]])




```python
torch.max(test,dim=1)
```




    torch.return_types.max(
    values=tensor([0.9094, 0.9346, 0.7908]),
    indices=tensor([1, 0, 1]))




```python
torch.min(test)
```




    tensor(-0.8693)



tensor支持的线性代数运算包括：
- `trace`：计算对角线元素和
- `diag`：获得对角线元素
- `triu`, `tril`：获得上/下三角矩阵
- `mm`, `bmm`：矩阵乘法，batch版本的矩阵乘法
- `t`：转置运算
- `dot`, `cross`：点积与叉积
- `inverse`：矩阵求逆
- `svd`：奇异值分解

在torch.linalg模块中提供了更多的线性代数运算，详情可查看https://pytorch.org/docs/stable/linalg.html


```python
torch.svd(test)
```




    torch.return_types.svd(
    U=tensor([[ 0.6798,  0.5495, -0.4858],
            [-0.3496,  0.8250,  0.4440],
            [ 0.6448, -0.1320,  0.7529]]),
    S=tensor([1.5809, 1.1861, 0.5292]),
    V=tensor([[-0.3707,  0.8406, -0.3950],
            [ 0.6482,  0.5388,  0.5381],
            [-0.6651,  0.0566,  0.7446]]))



## 布尔运算

torch中的BoolTensor类型支持使用布尔运算符进行逻辑运算，支持的布尔运算符包括：与操作&，或操作|，异或操作^，和取反操作~。


```python
test1=test.ge(0)
test2=test.le(0.2)
```


```python
test1&test2
```




    tensor([[False, False, False],
            [False, False, False],
            [False, False, False]])




```python
~test2
```




    tensor([[ True,  True, False],
            [ True,  True,  True],
            [False,  True, False]])



## 注意事项

1. 使用=操作符将一个tensor给变量赋值的时候，变量保存的是tensor的地址（即浅拷贝）。因此通过一个变量对tensor进行修改之后，在使用其它变量访问这个tensor的时候也会返回修改后的tensor。因此如果要获得一个tensor的副本时，需要使用`deepcopy()`函数。
2. 不同的tensor可能使用的是相同的内存空间，可以通过`id(tensor.storate())`函数查询内存地址来比较
3. torch中的部分函数分为两个版本，如果是以下划线_结尾的函数，代表就地操作，会直接把变量的值做就地修改；如果没有下划线，则返回一个新的变量
4. 与NumPy类似，tensor的向量化计算经过底层优化，效率比python自带的for循环要高得多。因此应当尽可能地使用向量化的表达式进行计算。



# torch.autograd

## 简介

torch.autograd模块提供了一些函数和类，可以用来做自动微分。在torch中，只支持对浮点数类型的自动求导。详细用法可参考官方文档：https://pytorch.org/docs/stable/autograd.html

## 使用方法

tensor有一个`requires_grad`参数，可以在定义tensor的时候传入这一参数。在创建tensor时这一参数默认为False，将其设置为True之后，tensor会包含一个`grad`属性，保存tensor对应的梯度；同时也包含一个`grad_fn`属性，指向一个Function对象来通过反向传播计算梯度。对于叶结点来说，它的`grad_fn`属性的值为None。如果某个变量与某个`require_grad=True`的变量具有依赖关系（例如通过计算式`y=x*x`产生依赖关系），则这个变量的require_grad参数会自动被设置为True，同时根据运算类型生成相应的`grad_fn`属性。

如果某个tensor的`requires_grad`参数为True，则可以在计算完成之后调用`tensor.backward()`函数，通过反向传播计算梯度，并将变量的梯度值保存到变量的grad属性中。对于一个标量，可以直接调用`backward()`函数；而对于一个tensor，如果要调用`backward()`，则需要手动设置`gradient`参数，这个参数需要传入一个与原tensor大小相同的tensor，其中的元素代表原tensor中每个元素的梯度值。

在反向传播过程中，计算图中非叶结点的梯度在反向传播计算结束之后会被清空。如果要查看的话，需要使用`autograd.grad()`函数或者`hook`方法；或者是调用`tensor.retain_grad()`函数，设置保留一个tensor的梯度值。


```python
x=torch.randn((2,3),requires_grad=True)
y=torch.exp(x)
z=y*y*3 #y和z的requires_grad参数被自动设置为True，因为它们是通过x计算而得的
```


```python
print(x.is_leaf, x.grad_fn) #x为叶结点，故grad_fn函数为None
print(y.is_leaf, y.grad_fn) 
print(z.is_leaf, z.grad_fn)
```

    True None
    False <ExpBackward object at 0x000001B7590254F0>
    False <MulBackward0 object at 0x000001B75905D550>



```python
y.retain_grad()
z.backward(gradient=torch.ones_like(z),retain_graph=True) #如果对z直接调用backward()函数，则需要传入gradient参数表示z中每个元素的梯度。需要注意的是，retain_graph参数默认为False，它表示对一幅计算图第一次使用backward操作之后，计算图的缓冲区会被清除掉。如果要对同一幅计算图进行多次反向传播，需要将retain_graph参数设置为True
```


```python
print(z.grad) #z不是叶结点，其梯度会被清除掉
print(y.grad) #y不是叶结点，但是由于我们使用retain_grad()函数设定保留其梯度的值，因此我们可以查看其梯度
print(x.grad) #x是叶结点，因此可以查看其梯度
```

    None
    tensor([[ 2.9371,  7.8643,  8.8528],
            [ 7.7700, 26.3906,  5.1546]])
    tensor([[  1.4377,  10.3078,  13.0621],
            [ 10.0622, 116.0772,   4.4284]])



```python
temp=y.register_hook(lambda grad:grad)
z.backward(gradient=torch.ones_like(z),retain_graph=True)
y.grad
```


    tensor([[ 5.8741, 15.7286, 17.7057],
            [15.5401, 52.7812, 10.3093]])




```python
torch.autograd.grad(z,y,grad_outputs=torch.ones_like(z),retain_graph=True) #使用autograd.grad函数也可以查看非叶结点的梯度
```




    (tensor([[ 2.9371,  7.8643,  8.8528],
             [ 7.7700, 26.3906,  5.1546]]),)




```python
a=z.sum()
a.backward() #由于a是一个标量，因此可以直接调用backward()函数
```

`tensor.backward()`函数中可以传入如下三个参数：
- `gradient`：形状与tensor一致，对于`y.backward()`，`grad_variable`相当于是链式求导法则$\frac{\partial z}{\partial x}=\frac{\partial z}{\partial y}\cdot \frac{\partial y}{\partial x}$中的$\frac{\partial z}{\partial y}$。
- `retain_graph`：反向传播需要保存一些中间结果，在反向传播之后这些中间结果就会被清除。如果将其设置为True，则可以不清空缓存，用来多次反向传播。
- `create_graph`：对反向传播过程再次构建计算图，这样便可通过backward of backward来求高阶导数。

## 计算图的特点

PyTorch中计算图的特点可以总结如下：
- autograd会根据tensor之间的关系构建计算图。一个tensor默认的`requires_grad`属性为False，如果某个节点（即tensor）的`requires_grad`被设置为True，则计算图中所有依赖它的结点`requires_grad`参数都为True。
- 计算图中，由用户创建的结点被称为叶结点，它的`grad_fn`属性为None。
- 多次反向传播时，梯度是累加的。反向传播的中间缓存会被清空，如果要进行多次反向传播需要将`retain_graph`参数设置为True
- 非叶结点的梯度在计算完成之后就会被清空，可以使用`autograd.grad`或者`hook`来获取非叶结点的梯度值
- 反向传播函数`backward`的参数`grad_variable`可以看作是链式求导的中间结果，如果是标量则可以省略，默认为1
- PyTorch采用的是动态图设计，可以在计算过程中随时查看中间层的输出，允许动态地设计计算图的结构。



# torch.nn

## 简介

`torch.nn`是PyTorch中专门用于搭建神经网络的模块，其中包含了绝大多数的**神经网络层**的实现。它们都是通过继承`nn.Module`的派生类来实现的，因此可以方便地用它们来搭建复杂的神经网络结构。例如：
- 卷积层：`Conv*d`, `ConvTranspose*d`等
- 池化层：`MaxPool*d`, `MaxUnpool*d`, `AvgPool*d`, `AdaptiveAvgPool*d`等
- 激活函数层：`Sigmoid`, `ReLU`, `Tanh`, `ELU`等
- 循环层：`RNN`, `LSTM`, `GRU`（直接处理整个序列）以及对应的`RNNCell`, `LSTMCell`, `GRUCell`（每次只处理序列的一个位置）
- Transformer层：`Transformer`, `TransformerEncoder`, `TransformerDecoder`
- Loss层：`MSELoss`, `CrossEntropyLoss`, `BCELoss`等
- 其他：全连接层`Linear`, Dropout层`Dropout*d`, Normalization层`BatchNorm*d`等

详细用法见官方文档：https://pytorch.org/docs/stable/nn.html 。

`torch.nn.Module`是`torch.nn`模块中最重要的基类，包含网络各层的定义以及forward方法。我们可以用它来实现自定义的网络层，或者构造一个神经网络的派生类。一般需要将具有可学习参数的层放在初始化函数`__init()__`中，并实现`forward()`函数。而`backward()`函数则会通过Autograd方式自动实现。

## torch.nn.functional

值得一提的是，`torch.nn`模块中的神经网络层在`torch.nn.functional`模块中都有相应的函数实现，例如`nn.Conv2d`与`nn.functional.Conv2d`。也就是说，`torch.nn.functional.*`中包含了一系列的函数接口，而`torch.nn.*`使用`nn.Module`基类对这些函数进行了类封装。详情可查看官方文档https://pytorch.org/docs/stable/nn.functional.html 。

对于`torch.nn.functional.*`，在调用函数的时候，则需要在传入输入数据的同时，也传入权重、偏置以及一些其它参数；而在使用对应的`torch.nn.*`类时，需要先传入初始化参数生成一个实体，这些初始化参数会被保存为类的属性，而权重、偏置等参数则会被自动生成。然后通过函数调用的方式调用这个实体，在调用函数的时候只需要传入输入数据即可，函数所需的其他参数会自动从类的属性中获取。

例如同样是`Conv2d`，二者的调用方式分别如下：


```python
inputs = torch.rand(64, 3, 244, 244)
conv = torch.nn.Conv2d(in_channels=3, out_channels=64, kernel_size=3, padding=1)
out = conv(inputs)
```


```python
weight = torch.rand(64,3,3,3)
bias = torch.rand(64) 
out = torch.nn.functional.conv2d(inputs, weight, bias, padding=1)
```

一般来说，对于包含有可学习参数的层，如`Conv`, `Linear`, `BatchNorm`等，建议使用`torch.nn.*`；而对于没有可学习参数的层，如损失函数、池化层、激活函数层等，使用`torch.nn.*`或者`torch.nn.functional.*`都可以。但是对于Dropout操作，则建议使用`torch.nn.*`：如果以`torch.nn.functional.*`方式调用Dropout，在模型的推理阶段需要手动将这一功能关闭；而使用`torch.nn.*`方式调用的话，则执行`model.eval()`命令之后，所有的Dropout层都会自动关闭。

此外，`torch.nn.*`由于继承自`torch.nn.Module`，因此能够很好地与`torch.nn.Sequential`结合使用，方便网络的构造；而`torch.nn.functional.*`则无法与`torch.nn.Sequential`结合使用。

## 示例

下面是一个使用`torch.nn`模块搭建ResNet的示例：


```python
class ResidualBlock(torch.nn.Module):
    def __init__(self,in_channel,
                 out_channel,
                 stride=1,
                 shortcut=None):
        super(ResidualBlock,self).__init__()
        self.left=torch.nn.Sequential(
            torch.nn.Conv2d(in_channel,out_channel,3,stride,1,bias=False),
            torch.nn.BatchNorm2d(out_channel),
            torch.nn.ReLU(inplace=True),
            torch.nn.Conv2d(out_channel,out_channel,3,1,1,bias=False),
            torch.nn.BatchNorm2d(out_channel)
        )
        self.right=shortcut
    def forward(self,x):
        out=self.left(x)
        residual=x if self.right is None else self.right(x)
        out+=residual
        return torch.nn.functional.relu(out)
    
class ResNet(torch.nn.Module):
    def __init__(self,num_classes=20):
        super(ResNet,self).__init__()
        self.pre=torch.nn.Sequential(
            torch.nn.Conv2d(3,64,7,2,3,bias=False),
            torch.nn.BatchNorm2d(64),
            torch.nn.ReLU(inplace=True),
            torch.nn.MaxPool2d(3,2,1)
        )
        self.layer1=self.make_layer(64,128,3)
        self.layer2=self.make_layer(128,256,4,stride=2)
        self.layer3=self.make_layer(256,512,6,stride=2)
        self.layer4=self.make_layer(512,512,3,stride=2)
        self.fc=torch.nn.Linear(512,num_classes)
        
    def make_layer(self,in_channel,out_channel,block_num,stride=1):
        shortcut=nn.Sequential(
            torch.nn.Conv2d(in_channel,out_channel,1,stride,bias=False),
            torch.nn.BatchNorm2d(out_channel)
        )
        layers=[]
        layers.append(ResidualBlock(in_channel,out_channel,stride,shortcut))
        for i in range(1,block_num):
            layers.append(ResidualBlock(out_channel,out_channel))
        return torch.nn.Sequential(*layers)
    
    def forward(self,x):
        x=self.pre(x)
        x=self.layer1(x)
        x=self.layer2(x)
        x=self.layer3(x)
        x=self.layer4(x)
        
        x=torch.nn.functional.avg_pool2d(x,7)
        x=x.view(x.size(0),-1)
        
        return self.fc(x)
```

## torch.nn.init

`torch.nn.init`模块中包含了一些神经网络参数的初始化函数，例如常用的Xavier初始化和He初始化等。这些函数接受一个tensor作为参数，然后按照一定的概率分布生成这些tensor的值作为它的初始化值。对于自定义的神经网络层或者神经网络模块，可以使用这些初始化函数对其中的参数进行初始化。详情可查看官方文档：https://pytorch.org/docs/stable/nn.init.html



# torch.optim

torch.optim中包含了许多不同的优化器，以及可以调整这些优化器学习率的调度器。在神经网络的训练过程中，可以使用这些优化器和调度器来帮助我们更新神经网络的参数。详细内容可参考官方文档：https://pytorch.org/docs/stable/optim.html

在PyTorch中，无论是哪一种优化器和调度器，其使用方法都基本一致。例如我们使用Adam优化器以及StepLR调度器，对上一节中搭建的ResNet的优化过程如下。下面的代码对于大部分的模型、优化器和调度器都是通用的，可以根据实际需要进行改动并直接使用。
```Python
model=ResNet()
optimizer=torch.optim.Adam(model.parameters(),lr=0.001) #此处可以换成任意一种优化器，如torch.optim.SGD等
scheduler=torch.optim.lr_scheduler.StepLR(optimizer,step_size=10,gamma=0.5) #此处可以换成任意一种调度器，如torch.optim.lr_scheduler.ExponentialLR等
for epoch in range(100):
    for input, target in dataset: #一轮的训练过程
        optimizer.zero_grad() #清空上一组数据的梯度
        output = model(input) 
        loss = loss_fn(output, target) #计算预测值以及损失
        loss.backward() #反向传播计算梯度
        optimizer.step() #优化器会根据反向传播的梯度值来优化模型参数
    scheduler.step() #在一轮训练完成之后，调度器也前进一个步长，按照预先设定的参数对优化器的学习率进行调整
```



# 数据的保存与加载

PyTorch中，tensor、nn.Module和optimizer都可以保存到硬盘中去，并通过相应的办法加载到内存中。

对于一个tensor、nn.Module或者optimizer的实体（此处将其用obj代替），我们可以使用`torch.save(obj,file_name)`将他们保存起来，然后使用`tensor=torch.load(obj)`加载保存后的对象。

而对于nn.Module对象和optimizer，也可以保存对应的state_dict，这样在加载的时候更方便。此时可以使用`torch.save(model.state_dict(),file_name)`和`model.load_state_dict(torch.load(file_name))`。其中model/optimizer对应的是model/optimizer的一个实体。其中`state_dict`是一个字典类型的变量，可以用字典的相关操作来查看其内容。因此，也可以将不同的module或者optimizer的数据合并为一个dict，然后保存到一起。

需要注意的是，`torch.load()`实质使用的是Python的pickle方式进行读取。因此在使用`torch.load()`加载一个模型时，该代码所在的Python脚本文件必须要与代码定义的Python脚本文件放在一起，否则读取模型时会因为找不到模型定义而导致读取失败。



# torch.utils

`torch.utils`模块包含了若干子模块，每个子模块内是一些PyTorch的辅助函数。这些子模块以及它们的作用如下：

- [`torch.utils.benchmark`](https://pytorch.org/docs/stable/benchmark_utils.html)：其中包含了一些用于评估模型性能的辅助函数，可以在模型的训练与推理过程中使用这些辅助函数来得到执行时间等信息。官方也给出了这个模块的使用教程：https://pytorch.org/tutorials/recipes/recipes/benchmark.html   
- [`torch.utils.bottleneck`](https://pytorch.org/docs/stable/bottleneck.html)：这一模块提供了寻找模型瓶颈的函数，它的用法比较简单，在命令行执行`python -m torch.utils.bottleneck script.py [args for script.py]`即可。   
- [`torch.utils.checkpoint`](https://pytorch.org/docs/stable/checkpoint.html)：这一模块提供了一些添加检查点的函数，可以用来获得模型某一处的输出结果。
- [`torch.utils.cpp_extension`](https://pytorch.org/docs/stable/cpp_extension.html)：这一模块提供了在PyTorch中执行C++扩展的接口
- [`torch.utils.dlpack`](https://pytorch.org/docs/stable/dlpack.html)：提供了tensor和dlpack之间互相转换的函数
- [`torch.utils.tensorboard`](https://pytorch.org/docs/stable/tensorboard.html)：其中主要实现了`SummaryWriter`类，在这个类中包含了一些不同的函数，可以向tensorboard中写入不同种类的数据。在之后便可以打开tensorboard看到这些数据。
- [`torch.utils.data`](https://pytorch.org/docs/stable/data.html)：这一模块定义了Dataset类和DataLoader类，可以方便地实现数据加载。

    使用Dataset类作为基类，我们可以自定义一个派生类，从而构造自定义的数据集。在这个派生类中，需要实现`__getitem__`和`__len__`两种方法，其中`__getitem__`可以根据索引得到相对应的数据，而`__len__`则可以得到样本的数量。
    
    DataLoader可以辅助Dataset类的操作，可以在加载数据时实现数据打乱、batch加载、多进程加载等功能。它是一个可迭代的对象，在创建一个DataLoader的实体之后便可以像使用迭代器一样使用它。
    
    例如我们自定义一个Dataset，并构造一个DataLoader来从这个Dataset中加载数据：
    
    ```Python
    class DogCat(torch.utils.data.Dataset):
        def __init__(self,root):
            imgs=os.listdir(root) #返回文件夹中包含文件的列表
            self.imgs=[os.path.join(root,imgs) for img in imgs]
    
        def __getitem__(self,index):
            img_path=self.imgs[index]
            label=1 if 'dog' in img_path.split('/')[-1] else 0
            pil_img=Image.open(img_path)
            array=np.asarray(pil_img)
            data=torch.from_numpy(array)
            return data,label
    
        def __len__(self):
            return len(self.imgs)
        
    DogCatDataLoader=torch.utils.data.DataLoader(DogCat(rootdir), batch_size=32, shuffle=True, num_workers=2)
    ```



# torch.onnx

ONNX（Open Neural Network Exchange，开放神经网络交换）是一种针对于机器学习所设计的开放式的文件格式，方便神经网络结构及其参数的保存与解析。ONNX文件可以在TensorFlow、PyTorch、Caffe等深度学习框架中解析，也方便使用TensorRT进行部署。而常用的深度学习框架也提供了将训练好的模型保存为ONNX文件的函数。

`torch.onnx`模块主要用于将PyTorch模型转为onnx格式储存。其中最重要的函数为`torch.onnx.export`，例如下面是将AlexNet转为onnx格式的代码：
```Python
import torch
import torchvision

dummy_input = torch.randn(10, 3, 224, 224, device='cuda')
model = torchvision.models.alexnet(pretrained=True).cuda()

input_names = [ "actual_input_1" ] + [ "learned_%d" % i for i in range(16) ]
output_names = [ "output1" ]

torch.onnx.export(model, dummy_input, "alexnet.onnx", verbose=True, input_names=input_names, output_names=output_names)
```

onnx格式只支持PyTorch中一部分运算的导出，如果模型中包含一些特殊的运算，则可能会导致无法将模型成功导出为onnx格式。此时需要对模型中的一些特殊运算转换为可以导出的等价运算，或者是借助一些额外工具如onnx-simplifier对模型进行简化，然后再导出。



# torchvision

torchvision函数库包含了计算机视觉相关的数据集、模型结构和一些常用的图像处理函数等。其中常用的几个模块为：
- `torchvision.datasets`：包含了计算机视觉领域中常用的数据集，例如CIFAR、COCO、ImageNet等
- `torchvision.transforms`：包含了常用的图片预处理功能，例如图片的翻转、裁剪、旋转等操作
- `torchvision.models`：包含了计算机视觉一些常用的模型，如ResNet、Faster R-CNN等。在调用这些模型时，可以只生成这些模型本身，也可以使用训练好的模型
- `torchvision.ops`：包含了计算机视觉领域一些常用的运算，如NMS、IoU等
- `torchvision.io`：包含了一些图片与视频的读取和写入操作函数

详情可参考：https://pytorch.org/vision/stable/

例如使用torchvision加载Faster R-CNN模型：


```python
model = torchvision.models.detection.fasterrcnn_resnet50_fpn(pretrained=True) #之后可以像使用其它神经网络模型一样使用它
```

使用torchvision加载CIFAR10数据集。这些加载数据集的函数返回值是一个`Dataset`类型的变量，可以传入`DataLoader`中使用：


```python
mnist_dataset=torchvision.datasets.CIFAR10(root='/mnist',download=True) #第一次使用需要下载，之后再使用的时候就可以指定缓存路径直接读取
```




# torchtext

torchtext是专门用来做文本处理的函数库，它主要分为如下几个模块：
- `torchtext.data`：它又分为functional, metrics和utils三个子模块，可以用来做数据的预处理
- `torchtext.datasets`：包含了自然语言处理领域一些常用的数据集
- `torchtext.vocab`：主要定义了Vocab类，通过使用这一数据类型可以建立词汇表和词向量
- `torchtext.utils`：定义了一些辅助函数
- `torchtext.nn.modules.multiheadattention`：定义了Multihead Attention的神经网络类，可以直接调用来生成一个具有Multihead Attention结构的网络

详细可参考：https://pytorch.org/text/stable/index.html



# torchaudio

torchaudio是一个与音频信号相关的函数库，其中包含了音频信号读取和处理的函数，以及一些与音频相关的数据集和神经网络。官方文档：https://pytorch.org/audio/stable/index.html



# torch_geometric

torch_geometric是一个与图神经网络相关的函数库，其中主要的子模块有：
- `torch_geometric.nn`：定义了图神经网络中的卷积层、池化层等，以及一些图神经网络的模型
- `torch_geometric.data`：主要包含了针对于图结构的`Data`数据类型，批量加载`Data`数据类型的`Batch`类型，以及针对于`Data`数据类型专门定义的`DataLoader`
- `torch_geometric.datasets`：包含了图神经网络领域经常使用的一些数据集
- `torch_geometric.transforms`：包含了一些对图结构做变换以及计算图结构性质的函数
- `torch_geometric.utils`：包含了一些对图结构进行操作的辅助函数，以及其他数据格式与`Data`数据格式互相转换的函数

在使用torch_geometric自定义图卷积操作时，需要从`MessagePassing`类进行继承，并自定义`message`和`update`函数。有如下几点需要注意：

1. `message`函数的参数中，可以使用`x_i`和`x_j`分别对应于`x[edge_index[:,0]]`和`x[edge_index[:,1]]`，基类在调用`propagate`函数时会自动识别这两个参数，并生成相应的tensor
2. `update`函数中，可以使用`aggr_out`参数，代表邻接结点聚合之后的值
3. 需要自定义`forward`函数，`forward`函数中调用`propagate`函数可以自动完成`message`，`aggregate`和`update`三个步骤。`propagate`的第一个参数为`edge_index`（必须参数），后面的可选参数中，必须把`message`和`update`两个函数中使用到的参数全部写进去
4. `MessagePassing`这个父类在初始化时，需要传入`aggr`和`flow`两个参数，`flow`代表信息流向，可以为`source_to_target`(默认)或`target_to_source`，`aggr`可以为`add`，`mean`和`max`

下面是定义CGConv的一个例子：


```python
def CGConv(MessagePassing):
    #channels: the dimension of node features
    #dim: the dimension of edge features
    def __init__(self,channels,dim,aggragate='add',bias=True,**kwargs):
        super(self,CGConv).__init__(aggr=aggragate)
        self.channels=channels
        self.dim=dim
        self.wf=nn.Linear(channels*2+dim,channels,bias=bias)
        self.ws=nn.Linear(channels*2+dim,channels,bias=bias)
        
    def reset_parameters(self):
        self.wf.reset_parameters()
        self.ws.reset_parameters()
        
    def message(self,x_i,x_j,edge_attr):
        z=torch.cat([x_i,x_j,edge_attr],dim=0)
        temp1=self.wf(z)
        temp2=self.ws(z)
        temp1=nn.functional.sigmoid(temp1)
        temp2=nn.functional.softplus(temp2)
        return torch.mul(temp1,temp2)
    
    def forward(self,x,edge_index,edge_attr):
        return self.propagate(edge_index,x=x,edge_attr=edge_attr)
        
    def update(self,aggr_out,x):
        return aggr_out+x
```

`Data`类一般包含`x`，`edge_index`，`edge_attr`和`y`四部分，同时也可以加入自定义的其他变量。但是一定要注意，所有的变量必须都为`torch.tensor`格式，且可以做concat操作。并且`edge_index`一定要是`torch.int64`格式的数据。否则在`DataLoader`中加载`mini-batch`的时候会报错。



# libtorch

## 简介

libtorch相当于是PyTorch的C++版本，PyTorch中的多数函数和类在libtorch中都有对应的实现。使用libtorch也可以实现tensor的创建、网络结构的定义、网络的训练与推理等任务，也可以读取PyTorch训练好的模型文件进行推理。关于C++ API可以查看官方文档：https://pytorch.org/cppdocs/

要在C++中读取PyTorch训练好的模型，需要将模型转换为TorchScript文件，然后在libtorch中读取文件内容即可。

## 安装与环境配置

libtorch的安装过程比较简单，只需要从官网下载对应CUDA版本的压缩包，解压后把\*/lib/文件夹添加到系统的环境变量即可。后续使用的时候，需要像OpenCV、CUDA等函数库一样，在Visual Studio中添加相应的路径以及链接库等内容。

要在VS中配置支持CUDA的libtorch，需要在Project->Properties中添加如下的内容（配置时记得选x64，以及根据需要选择Debug还是Release模式，下面的内容是Release模式的设置）：

- C/C++->General->Additional Include Directories中加入\*/libtorch/include和\*/libtorch/include/torch/csrc/api/include两个文件路径
- Linker->General->Additional Library Directories中加入\*/libtorch/lib路径
- Linker->Additional Dependencies中加入c10.lib;c10_cuda.lib;libprotobuf.lib;mkldnn.lib;torch.lib;torch_cuda.lib;torch_cpu.lib;（此处可能会缺少某些函数库，可以根据需要添加）
- Linker->Command Line里面的Additional Options中加入-INCLUDE:?warp_size@cuda@at@@YAHXZ 



# 注意事项

1. 在计算累计误差的时候，记得将误差的tensor做detach操作，否则计算图会越加越长。

2. 注意tensor的共享！由于python的语言特性，在使用=赋值的时候某个变量获取的是tensor的地址，而不是复制一个tensor。

3. 利用早停技术的时候，最佳模型需要用torch.save()保存。同样是因为python的语言特性，与tensor类似，不能用=复制！



# 参考

1. TorchScript的官方文档：https://pytorch.org/docs/stable/jit.html ，libtorch读取TorchScript文件的官方教程：https://pytorch.org/tutorials/advanced/cpp_export.html
2. libtorch CUDA环境配置：https://blog.csdn.net/weixin_40514095/article/details/109352479
3. C++加载PyTorch模型的教程：https://github.com/apachecn/pytorch-doc-zh/blob/master/docs/1.0/cpp_export.md，https://www.jianshu.com/p/7cddc09ca7a4
4. PyTorch JIT和TorchScript：https://blog.csdn.net/MumuziD/article/details/113280207
5. Nvidia Jetson使用Libtorch：https://blog.csdn.net/qizhen816/article/details/103566646
