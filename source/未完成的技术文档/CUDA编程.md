---
title: CUDA编程
tags: 
- CUDA
categories: [CUDA]
sharing: True
---



# CUDA硬件知识

## CPU和GPU的区别

- 架构：CPU被设计用来执行少量比较复杂的任务，GPU被设计用来执行大量比较简单的任务；CPU的设计主要针对于大量离散而不相关的任务，GPU的设计主要是针对于可以分解成大量的小块且可以独立运行的问题。
- 支持线程的方式：CPU的每个核只有少量的寄存器；GPU的每个核有多个寄存器组。
- 处理器数量：CPU的核数较少，而GPU具有大量的流处理器。
- 失速状态的处理：CPU的调度策略是基于时间分片，而GPU的调度策略是使用工作池来保证一直有事可做。
- 任务执行模式：CPU会将一个单独的指令流分配到CPU核心，而GPU是将同一条指令送到N个逻辑执行单元。

![img](https://raw.githubusercontent.com/lyf35/blog_figures/main/img/20210506213653.jpeg)

<!--more-->

## 硬件组成

一个GPU是由大量的流处理器簇（Stream Multiprocessor, SM）组成的，同时包含有全局的L2缓存、显存与显存控制器、负责线程块分配的Giga Thread Engine等单元。

SM的结构与GPU的架构有关。例如NVIDIA Pascal架构中的一个SM的结构如下图所示：

![流多处理器](https://raw.githubusercontent.com/lyf35/blog_figures/main/img/20210510194519.png)

虽然不同GPU架构中的SM结构不同，但是它们所具有的组成模块基本相同。每个SM包含很多个CUDA Core（又叫流处理器，Streaming Processor，SP）。流处理器是最基本的处理单元，一个SP就对应于一个线程。同时也包含了线程束调度器、寄存器、指令缓存、L1缓存、共享内存、指令分发器等模块。

## CUDA线程

### 多维结构

从逻辑角度来说，CUDA中的线程是多维的，它以线程网格的方式组织，每个线程网格中包含有若干个线程块，而一个线程块又可以包含多个线程。其中，网格和线程块都可以是一维、二维或者三维。对应于硬件关系，一个Grid被分配到设备上运行，一个Block被分配到SM上运行，而一个线程被分配到SP上运行。

同一线程块中的众多线程拥有相同的指令地址，不仅能够并行执行，而且能够通过共享存储器（Shared memory）和栅栏（barrier）实现块内通信。这样，同一网格内的不同块之间存在不需要通信的粗粒度并行，而一个块内的线程之间又形成了允许通信的细粒度并行。

![img](https://raw.githubusercontent.com/lyf35/blog_figures/main/img/20210506233827.png)

在使用上述的多维结构时，每个线程块内的线程具有唯一的编号`threadIdx`，网格中的每个线程块也有一个唯一的编号`blockIdx`。相应地，也有`blockDim`和`gridDim`两个属性来表示线程块和线程网格的尺寸。它们都可能是一个三维的编号，具有`x`,`y`,`z`三个属性，它们按照从里到外的顺序存储，即`t[z][y][x]`。因此，一个三维的Grid中，一个线程的地址可以用如下方式计算：

```c++
int threadId_3D = threadIdx.x + threadIdx.y*blockDim.x + threadIdx.z*blockDim.x*blockDim.y; //一个Block内的线程id
int blockId_3D = blockIdx.x + blockIdx.y*gridDim.x + blockIdx.z*gridDim.x*gridDim.y; //一个Grid内的Block id
int i = threadId_3D + (blockDim.x*blockDim.y*blockDim.z)*blockId_3D; //blockId前面所有block的线程数+当前block Id内的偏移
```

### 线程调度

线程束是SM的基本调度和执行单元，每个线程束中的线程会同时执行。在一个线程束中，所有的线程按照单指令多线程（SIMT）方式执行；即，所有线程都执行相同的指令，每个线程在私有数据上进行操作。

一个线程块便是多个线程束的集合，一个线程块会被分配到某一个SM上执行。由于一个SM可以容纳的线程数有上限（这个上限主要与底层硬件的计算能力有关），因此线程块的大小不能无限大；而当一个线程块的线程数小于SM可容纳的最大线程数时，一个SM也可以同时容纳并处理多个线程块。需要注意的是，SM一次可以调度处理的线程束数量以及可以容纳的线程块数量都有上限。

在实际工程中，线程块的大小需要设置为适中的值。如果太大，则可能会导致一个线程块内的很多线程束都在等待某一个较慢的线程束执行完毕；而如果太小，则会在所有资源被充分利用之前，硬件达到每个SM的线程块数量的上限，从而造成计算资源的闲置。通常来说，线程束的大小为32，因此线程块的大小最好被设置为32的倍数（通常使用128或者256，这样大小适中，同时使得块的数量多于SM的数量，具有足够的并行度）。



## CUDA内存层次结构

### 概述

从硬件角度，CUDA的内存结构如下图所示：

<img src="https://raw.githubusercontent.com/lyf35/blog_figures/main/img/20210511211248.jpeg" alt="在这里插入图片描述" style="zoom: 33%;" />

其中每个SM包含了寄存器、本地内存、L1缓存和共享内存（二者共享一块片上内存）；而所有的SM又共享L2缓存、全局内存、常量内存和纹理内存。

从线程使用内存的角度出发，CUDA的内存层次结构也可以表示为下图：

<img src="https://raw.githubusercontent.com/lyf35/blog_figures/main/img/20210511204743.png" alt="内存结构" style="zoom: 33%;" />

也就是说，每个线程具有自己独享的寄存器和本地内存，共享内存供一个线程块内的所有线程分享；而全局内存、常量内存和纹理内存为不同线程块内的所有线程共同使用。

对于上述这些不同的内存种类，它们的特性可以总结为下表：

|     存储器      |   位置   | 拥有缓存 |          访问权限          |  变量生存周期  |
| :-------------: | :------: | :------: | :------------------------: | :------------: |
|    register     | GPU片内  |   N/A    |       Device可读/写        |  与thread相同  |
|  Local memory   | 板载显存 |    无    |       Device可读/写        |  与thread相同  |
|  Shared memory  | GPU片内  |   N/A    |       Device可读/写        |  与block相同   |
| Constant memory | 板载显存 |    有    |   Device可读，host要读写   | 可在程序中保持 |
| Texture memory  | 板载显存 |    有    |   Device可读，host要读写   | 可在程序中保持 |
|  Global memory  | 板载显存 |    无    | Device可读/写, host可读/写 | 可在程序中保持 |
|   Host memory   | Host内存 |    无    |        host可读/写         | 可在程序中保持 |
|  Pinned memory  | Host内存 |    无    |        host可读/写         | 可在程序中保持 |

### 寄存器

核函数中声明的一个没有其他修饰符的自变量，通常存储在寄存器中；在核函数声明的数组中，如果用于引用该数组的索引是常量且能在编译时确定，那么该数组也存储在寄存器中。寄存器变量对于每个线程来说都是私有的，一个核函数通常使用寄存器来保存需要频繁访问的线程私有变量。寄存器变量与核函数的生命周期相同，一旦核函数执行完毕，就 不能对寄存器变量进行访问了。

如果一个核函数使用了超过硬件限制数量的寄存器，则会用本地内存替代多占用的寄存器。 nvcc编译器使用启发式策略来最小化寄存 器的使用，以避免寄存器溢出。

### 本地内存

核函数中符合存储在寄存器中但不能进入被该核函数分配的寄存器空间中的变量将溢出到本地内存中。编译器可能存放到本地内存中的变量有：

- 在编译时使用未知索引引用的本地数组
- 可能会占用大量寄存器空间的较大本地结构体或数组
- 任何不满足核函数寄存器限定条件的变量

本地内存中的变量本质上与全局内存在同一块存储区域，都位于板载显存。因此本地内存访问的特点是高延迟和低带宽。

### 共享内存

在核函数中使用`__shared__`修饰符修饰的变量存放在共享内存中。因为共享内存是片上内存，所以与本地内存或全局内存相比，它具有更高的带宽和更低的延迟。它的使用类似于CPU一级缓存，但它是可编程的。

共享内存是线程之间相互通信的基本方式，生命周期伴随着整个线程块。当一个线程块执行结束后，其分配的共享内存将被释放并重新分配给其他线程块。访问共享内存必须同步使用`void __syncthreads()`函数来进行线程同步。

### 常量内存

使用`__constant__`修饰的常量会被存放在常量内存中。常量变量必须在全局空间内和所有核函数之外进行声明。对于所有计算能力的设备， 都只可以声明64KB的常量内存。常量内存是静态声明的，并对同一编译单元中的所有核函数可见。常量内存的一次读取会广播给所有线程束内的线程，因此当线程束中所有线程都从相同的地址取数据时，常量内存表现较好。

### 全局内存

使用修饰符号`__device__`的变量会被存放在全局内存中，这个变量应该在所有函数外定义，必须对使用这个变量的host端和device端函数都可见才能成功编译。全局内存是GPU中最大、延迟最高并且最常使用的内存。因为全局内存的性质，当有多个核函数同时执行的时候，如果使用到了同一全局变量，应注意内存竞争。

CPU、GPU都可以对全局内存进行读写访问。host端（CPU）使用`cudaMalloc`分配全局内存，`cudaFree`释放全局内存，并由`cudaMemcpy`进行主机端与设备端的数据传输。

### 纹理内存

纹理内存驻留在设备内存中，并在每个SM的只读缓存中缓存，以某种形式访问的时候可以提升性能。由于纹理内存的使用比较特殊，因此只需要在应用程序真正需要的时候再去关注其使用方法。

### 缓存

GPU缓存不可编程，其行为在出厂时已经设定好。GPU上有4种缓存：

1. 一级缓存
2. 二级缓存
3. 只读常量缓存
4. 只读纹理缓存

每个SM都有一个一级缓存，所有SM公用一个二级缓存。一级二级缓存的作用都是被用来存储本地内存和全局内存中的数据，也包括寄存器溢出的部分。



# CUDA编程基础

## 简单示例

下面为一个简单的CUDA程序示例：

```cpp
#include<cuda_runtime.h>
#include<iostream>

//Device code
__global__ void VecAdd(float* A, float* B, float* C, int N)
{
        int i = blockDim.x * blockIdx.x + threadIdx.x;
        if(i < N)
        {
                C[i] = A[i] + B[i];
        }
}

//Host code
int main()
{
        int N = 1024;
        size_t size = N * sizeof(float);
        float* h_A = (float*) malloc(size);
        float* h_B = (float*) malloc(size);
        float* h_C = (float*) malloc(size);
        for(int i = 0 ; i < N ; i++)
        {
                h_A[i] = i / 2.0;
                h_B[i] = i / 3.0;
        }

        float* d_A;
        float* d_B;
        float* d_C;
        
        cudaMalloc(&d_A, size);
        cudaMalloc(&d_B, size);
        cudaMalloc(&d_C, size);

        cudaMemcpy(d_A, h_A, size, cudaMemcpyHostToDevice);
        cudaMemcpy(d_B, h_B, size, cudaMemcpyHostToDevice);

        int threadsPerBlock = 256;
        int blocksPerGrid = (N + threadsPerBlock - 1) / threadsPerBlock;
        VecAdd<<<blocksPerGrid, threadsPerBlock>>>(d_A, d_B, d_C, N);

        cudaMemcpy(h_C, d_C, size, cudaMemcpyDeviceToHost);

        cudaFree(d_A);
        cudaFree(d_B);
        cudaFree(d_C);

        free(h_A);
        free(h_B);
        free(h_C);

}
```

在上面这个程序中，带有前缀`__global__`的函数表示一个CUDA的内核函数，这一函数会在GPU内执行，但是需要由CPU调用。而在CPU执行的主函数中，如果要使用GPU进行计算，则首先要分配GPU内存并将变量复制过去，然后调用CUDA的内核函数进行计算。在计算时，需要为内核函数设定block、grid的大小，以及每个grid中的线程数量。

## 函数类型限定符

在CUDA中，共分为三种函数类型限定符，需要作为前缀加在函数定义之前：

1. `__host__`：表示函数在CPU上调用，在CPU上执行，如果不加任何函数前缀的时候，就默认使用这种方式。CUDA支持`__host__`与`__device__`关键词联用，表示函数同时为主机和设备编译

2. `__device__`：表示函数从GPU上调用，在GPU上执行。这样的函数只能被`__global__`或者`__device__`修饰的函数调用

3. `__global__`：表示函数在CPU上调用，在GPU上执行，也就是所谓的内核函数。内核函数只能被CPU调用，需要注意它并不是一个完整的程序，只是一个数据并行的步骤，其指令流由多个线程执行

   任何内核函数在执行的时候，都需要使用`<<<Dg, Db, Ns, s>>>`形式的表达式传递内核函数的执行配置参数。四个执行配置参数的含义为：`Dg`是一个`dim3`类型的变量，指明网格的维度和大小，`Dg.x*Dg.y*Dg.z`等于启动的块数量；`Db`是一个`dim3`类型的变量，用于声明块大小，`Db.x*Db.y*Db.z`等于每个块内的线程数量；`Ns`是一个`size_t`类型的变量，声明动态分配的共享存储器大小，默认为0；`s`的类型为`cudaStream_t`，用来声明执行的流，默认为0

## 内建变量

CUDA为运行在GPU上的函数提自动生成了如下的内建变量，可以在函数运行时获得网格和块的尺寸，以及线程索引等信息：

- `gridDim`：是一个包含三个元素`x,y,z`的结构体，分别表示网格在`x,y,z`三个方向上的尺寸，也就是`x,y,z`三个方向上有多少个块
- `blockIdx`：是一个包含三个元素`x,y,z`的结构体，分别表示当前块在`x,y,z`三个方向上的索引
- `blockDim`：是一个包含三个元素`x,y,z`的结构体，分别表示当前块在`x,y,z`三个方向上的尺寸，也就是`x,y,z`三个方向上有多少个线程
- `threadIdx`：是一个包含三个元素`x,y,z`的结构体，分别表示当前线程在`x,y,z`三个方向上的索引
- `warpSize`：表示线程束的大小，这个值通常是32

## 内存管理

### 变量的内存使用

对于CUDA中不同方式的变量声明及其使用的存储器，可以总结为下表：

|      修饰符      |    变量名称    | 存储器 | 作用域 | 生命周期 |
| :--------------: | :------------: | :----: | :----: | :------: |
|                  |   float var    | 寄存器 |  线程  |   线程   |
|                  | float var[100] |  本地  |  线程  |   线程   |
|  \_\_shared\_\_  |   float var*   |  共享  |   块   |    块    |
|  \_\_device\_\_  |   float var*   |  全局  |  全局  | 应用程序 |
| \_\_constant\_\_ |   float var*   |  常量  |  全局  | 应用程序 |

其中共享内存、全局内存和常量内存的使用可以通过在声明变量的时候使用修饰符，显式地将变量存储在修饰符所对应的存储器上。而寄存器和本地内存的使用则无法手动设定，在Kernel函数中定义的变量以及较小的定长数组都可以存放在寄存器中，而当寄存器的使用到达上限之后，则会将数据放在本地内存当中去。

### 内存分配和释放

动态内存的分配和释放主要是使用下面的函数：

```cpp
cudaError_t cudaMalloc(void ** devPtr,size_t count)
```

需要注意的是，这一函数的第一个参数是一个二重指针。二重指针`devptr`存储在CPU内存中，指向一个GPU内存的地址；而解掉一重指针后的一重指针`*devptr`则是被存储在GPU内存中，它存储的是GPU中所分配内存空间的首地址。也就是说，二重指针的作用相当于完成了CPU到GPU的映射。使用这一方式分配得到的内存空间属于

如果是为二维或者三维数组分配内存，建议使用`cudaMallocPitch()`和`cudaMalloc3D()`这两个函数。这两个函数会自动对内存进行填充，从而实现数据对齐，以提高计算效率。

当分配完地址后，可以使用下面函数对GPU内存进行初始化：

```cpp
cudaError_t cudaMemset(void * devPtr,int value,size_t count)
```

它的用法和`memset`类似，但是需要注意的是，指针指向的内存空间必须在GPU上。

当分配的GPU动态内存不需要继续使用时，使用下面语句便可以将其所占的内存空间释放掉：

```cpp
cudaError_t cudaFree(void * devPtr)
```

注意这个参数一定是`cudaMalloc`类的函数（还有其他分配函数）所分配的空间。如果输入非法的指针参数，则会返回 `cudaErrorInvalidDevicePointer `错误；如果重复释放一个空间，也会报错。

### 内存传输

对于动态分配的内存空间（即`cudaMalloc`函数所分配的空间），数据传输可以使用如下函数：

```cpp
cudaError_t cudaMemcpy(void *dst,const void * src,size_t count,enum cudaMemcpyKind kind)
```

其中，第一个参数为目标地址，第二个参数为原始地址，第三个参数为拷贝的内存大小，最后一个参数为传输类型。CUDA的内存传输类型共分为以下四种：

- cudaMemcpyHostToHost
- cudaMemcpyHostToDevice
- cudaMemcpyDeviceToHost
- cudaMemcpyDeviceToDevice

对于二维或者三维数组而言，它们的内存分配可以使用`cudaMemcpy2D()`和`cudaMemcpy3D()`这两个函数。

而全局变量的内存传输则需要使用如下两个函数：

```cpp
cudaError_t cudaMemcpyToSymbol(void *dst,const void * src,size_t count); //从CPU内存传输数据到GPU内存
cudaError_t cudaMemcpyFromSymbol(void *dst,const void * src,size_t count); //从GPU内存传输数据到CPU内存
```

### 固定内存

由于主机内存采用分页式管理，应用只能看到虚拟的内存地址，而操作系统可能随时更换物理地址的页（从原始地址复制到另一个地址）。从主机传输到设备上的时候，如果此时发生了页面移动，对于传输操作来说是致命的，所以在数据传输之前，CUDA驱动会锁定页面，或者直接分配固定的主机内存，将主机源数据复制到固定内存上，然后从固定内存传输数据到设备上。

下面函数用来分配固定内存：

```cpp
cudaError_t cudaMallocHost(void ** devPtr,size_t count)
```

这一函数分配count字节的固定内存，这些内存是页面锁定的，可以直接传输到GPU设备端，这样使得传输带宽变得高很多。

如果要释放固定的主机内存，可以使用下面的函数：

```cpp
cudaError_t cudaFreeHost(void *ptr)
```







# 参考

1. CUDA并行程序设计：GPU编程指南
2. https://www.zhihu.com/question/19903344
3. https://docs.nvidia.com/cuda/cuda-c-programming-guide/index.html
4. [深入GPU硬件架构及运行机制 - 0向往0 - 博客园 (cnblogs.com)](https://www.cnblogs.com/timlly/p/11471507.html)
5. [GPU中的几个基本概念 - 知乎 (zhihu.com)](https://zhuanlan.zhihu.com/p/53763285)
6. [cuda 学习之内存层次结构_xukang95的博客-CSDN博客](https://blog.csdn.net/xukang95/article/details/102855750)
7. [【CUDA 基础】4.1 内存模型概述 | 谭升的博客 (face2ai.com)](https://face2ai.com/CUDA-F-4-1-内存模型概述/)
8. [CUDA编程笔记——chapter4 CUDA全局内存 - 知乎 (zhihu.com)](https://zhuanlan.zhihu.com/p/360032964)
9. [CUDA修饰符，运算符＜＜＜＞＞＞及变量的介绍_tangbenjun的博客-CSDN博客](https://blog.csdn.net/tangbenjun/article/details/77988885)
10. [CUDA中修饰符的解释 - 袁军峰 - 博客园 (cnblogs.com)](https://www.cnblogs.com/tibetanmastiff/p/4712618.html)
11. [关于cudaMalloc内存分配以及二重指针_fishming的blog-CSDN博客](https://blog.csdn.net/weixin_43260254/article/details/104162207)
12. 





