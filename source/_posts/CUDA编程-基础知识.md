---
title: CUDA编程-基础知识
tags: 
- 并行计算
- CUDA
categories: [工程实践, CUDA]
date: 2021-04-28 21:54:32
updated: 2021-06-17 22:28:20
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

<img src="https://raw.githubusercontent.com/lyf35/blog_figures/main/img/20210510194519.png" alt="流多处理器" style="zoom:67%;" />

虽然不同GPU架构中的SM结构不同，但是它们所具有的组成模块基本相同。每个SM包含很多个CUDA Core（又叫流处理器，Streaming Processor，SP）。流处理器是最基本的处理单元，一个SP就对应于一个线程。同时也包含了线程束调度器、寄存器、指令缓存、L1缓存、共享内存、指令分发器等模块。



# CUDA线程

## 多维结构

从逻辑角度来说，CUDA中的线程是多维的，它以线程网格的方式组织，每个线程网格中包含有若干个线程块，而一个线程块又可以包含多个线程。其中，网格和线程块都可以是一维、二维或者三维。如下图所示：

![img](https://raw.githubusercontent.com/lyf35/blog_figures/main/img/20210506233827.png)

对应于硬件，一个Grid被分配到设备上运行，一个Block被分配到SM上运行，而一个线程被分配到SP上运行。它们的关系可以表示为下图：

<img src="https://raw.githubusercontent.com/lyf35/blog_figures/main/img/20210616205539.png" alt="image-20210616202028995" style="zoom: 33%;" />

同一线程块中的众多线程拥有相同的指令地址，不仅能够并行执行，而且能够通过共享存储器（Shared memory）和栅栏（barrier）实现块内通信。这样，同一网格内的不同块之间存在不需要通信的粗粒度并行，而一个块内的线程之间又形成了允许通信的细粒度并行。

在使用上述的多维结构时，每个线程块内的线程具有唯一的编号`threadIdx`，网格中的每个线程块也有一个唯一的编号`blockIdx`。相应地，也有`blockDim`和`gridDim`两个属性来表示线程块和线程网格的尺寸。它们都可能是一个三维的编号，具有`x`,`y`,`z`三个属性，它们按照从里到外的顺序存储，即`t[z][y][x]`。因此，一个三维的Grid中，一个线程的地址可以用如下方式计算：

```c++
int threadId_3D = threadIdx.x + threadIdx.y*blockDim.x + threadIdx.z*blockDim.x*blockDim.y; //一个Block内的线程id
int blockId_3D = blockIdx.x + blockIdx.y*gridDim.x + blockIdx.z*gridDim.x*gridDim.y; //一个Grid内的Block id
int i = threadId_3D + (blockDim.x*blockDim.y*blockDim.z)*blockId_3D; //blockId前面所有block的线程数+当前block Id内的偏移
```

## 线程调度

**线程束**是SM的基本调度和执行单元，每个线程束中的线程会同时执行。在一个线程束中，所有的线程按照单指令多线程（SIMT）方式执行；即，所有线程都执行相同的指令，每个线程在私有数据上进行操作。

一个线程块便是多个线程束的集合，一个线程块会被分配到某一个SM上执行。由于一个SM可以容纳的线程数有上限（这个上限主要与底层硬件的计算能力有关），因此线程块的大小不能无限大；而当一个线程块的线程数小于SM可容纳的最大线程数时，一个SM也可以同时容纳并处理多个线程块。需要注意的是，SM一次可以调度处理的线程束数量有上限，可以容纳的线程块数量与线程数量也都有上限。

在实际工程中，线程块的大小需要设置为适中的值。SM可容纳的线程束有限，如果线程块太大则会导致一些线程束需要排队等待，导致一个线程块内的很多线程束都在等待某些较慢的线程束执行完毕；而如果太小，则会在所有资源被充分利用之前，硬件达到每个SM的线程块数量的上限，从而造成计算资源的闲置。通常来说，线程束的大小为32，因此线程块的大小最好被设置为32的倍数（通常使用128或者256，这样大小适中，同时使得块的数量多于SM的数量，具有足够的并行度）。

## 分支与线程

我们以如下的代码为例，说明GPU对于分支的处理方式：

```cpp
__global__ demo_function()
{
	if(condition)
    {
        action_a();
    }
    else
    {
        action_b();
    }
}
```

GPU在执行上述代码时，首先会将不满足分支条件`condition`的线程设置为未激活，剩余处于激活状态的线程执行`action_a()`；之后，将满足分支条件`condition`的线程设置为未激活，剩余处于激活状态的线程执行`action_b()`。最后，所有的线程聚合，继续向下执行。

由于硬件每次只能为一个线程束获取一条指令，因此在执行分支的两步操作中，每一次都会有一部分线程被阻塞，另一部分线程处于工作状态，这样使得硬件的利用率无法达到100%。

实质上，在指令执行层，硬件的调度是基于半个线程束，而不是整个线程束。因此，如果能将线程束中一半的连续线程划分到同一个分支，这样硬件就可以同时执行分支结构中两个不同条件的分支块。就例如下面的代码段：

```cpp
__global__ demo_function()
{
	if((threadIdx.x % 32) < 16)
    {
        action_a();
    }
    else
    {
        action_b();
    }
}
```



# CUDA内存

## 层次结构

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

在核函数中使用`__shared__`修饰符修饰的变量存放在共享内存中。因为共享内存是片上内存，所以与本地内存或全局内存相比，它具有更高的带宽和更低的延迟。它的使用类似于CPU一级缓存，但它是可编程的，也就是说可以把它当成用户手动管理的缓存来使用。

共享内存是线程之间相互通信的基本方式，生命周期伴随着整个线程块。当一个线程块执行结束后，其分配的共享内存将被释放并重新分配给其他线程块。访问共享内存必须同步使用`void __syncthreads()`函数来进行线程同步。

每一个SM都有一块片上内存，供L1缓存和共享内存同时使用。在需要的情况下，可以通过使用`cudaDeviceSetCacheConfig()`函数来调整二者各自分配的大小。具体的用法可参考[CUDA Runtime API :: CUDA Toolkit Documentation (nvidia.com)](https://docs.nvidia.com/cuda/cuda-runtime-api/group__CUDART__DEVICE.html#group__CUDART__DEVICE_1g6c9cc78ca80490386cf593b4baa35a15)

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

## 全局内存访问

### 内存对齐

CUDA的内存传输可能会同时使用L1与L2缓存，也可能只使用L2缓存。如果只使用L2缓存，那么内存传输以32个字节为单位；而如果使用了L1缓存，则内存传输以128字节为单位。L1缓存的每一行为128字节，对应于设备内存中某个对齐的128字节的分块。

针对于缓存的特性，在使用设备内存时有如下两个优化的方向：一是在内存交换时尽量做到内存对齐，即内存交换操作的内存起始地址是32或128的偶数倍；二是在内存交换时尽量以合并的方式进行，即一个线程束内的32个线程访问一段连续的内存块。通过这两种优化，使得内存的带宽可以被充分利用。

上述内容也可以总结为如下两幅示意图。内存的访问最好以下图的方式进行：

<img src="https://raw.githubusercontent.com/lyf35/blog_figures/main/img/20210616230152.png" alt="image-20210616230145256" style="zoom: 67%;" />

尽量避免下图所示的内存访问方式：

<img src="https://raw.githubusercontent.com/lyf35/blog_figures/main/img/20210616230242.png" alt="image-20210616230237050" style="zoom:67%;" />

### 结构体

在使用结构体时，数据的组成方式有两种：一种是结构体构成的数组（Array of Strucrures），另一种是多个数组构成一个结构体（Structure of Arrays）。对于这两种不同的方式，数据在内存中的排布方式如下图所示：

<img src="https://raw.githubusercontent.com/lyf35/blog_figures/main/img/20210616225434.png" alt="image-20210616225426286" style="zoom: 80%;" />

通常情况下，使用SoA方式的内存访问效率会更高一些，因为这样访问的是内存中的连续区域。但是这一点并不绝对，需要根据具体应用而定。

## 共享内存访问

### Bank及其地址模式

为了取得高带宽，共享内存被划分为32个大小相等的内存模组，被称为存储体（Bank），而数据便被存储在不同的Bank中。对于一个线程束中的32个线程，它们可以同时访问不同的Bank，从而提高共享内存的读写速度。

一个Bank有两种不同的地址模式：32位模式或者64位模式，对应于4字节或者8字节的字段大小。我们以4字节的数据为例，在32位模式下，Bank的索引与共享内存的地址映射关系如下图所示：

<img src="https://raw.githubusercontent.com/lyf35/blog_figures/main/img/20210617210140.png" alt="image-20210617210130805" style="zoom: 50%;" />

而在64位模式下，二者的映射关系如下图：

<img src="https://raw.githubusercontent.com/lyf35/blog_figures/main/img/20210617212516.png" alt="image-20210617212511616" style="zoom:50%;" />

### Bank冲突

如果一个线程束内有多个线程要同时访问同一个Bank内的不同字段时，便会引起Bank冲突。以上面两幅图为例，如果有两个线程要同时访问索引为1和65的内存地址时，便会出现冲突。

需要注意的是，如果多个线程同时访问同一个Bank内的同一字段时，由于这一字段的数据可以通过广播的方式给到每个请求的线程上，因此不会造成冲突。仍以上面两幅图为例，如果有两个线程同时访问索引为1的内存地址，便不会形成冲突。此外，在64位模式下，如果两个线程同时访问索引为1和33的内存地址，也不会形成冲突，但是在32位模式下便会造成冲突。

在上面两幅图所示的情形中，如果不得不让多个线程同时访问同一个Bank中的不同字段，例如四个线程要同时访问共享内存的0,64,128,196这四个地址（它们都在Bank0），则可以使用空白填充的办法来避免Bank冲突。比如我们在数组的每32个元素之后添加一个空白元素，这样便可以将这四个地址放到不同的Bank中，从而避免了冲突。



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

## 核函数

### 函数类型限定符

在CUDA中，共分为三种函数类型限定符，需要作为前缀加在函数定义之前：

1. `__host__`：表示函数在CPU上调用，在CPU上执行，如果不加任何函数前缀的时候，就默认使用这种方式。CUDA支持`__host__`与`__device__`关键词联用，表示函数同时为主机和设备编译
2. `__device__`：表示函数从GPU上调用，在GPU上执行。这样的函数只能被`__global__`或者`__device__`修饰的函数调用
3. `__global__`：表示函数在CPU上调用，在GPU上执行，也就是所谓的内核函数。内核函数只能被CPU调用，需要注意它并不是一个完整的程序，只是一个数据并行的步骤，其指令流由多个线程执行。内核函数的返回类型必须是`void`

### 函数参数

任何内核函数在执行的时候，都需要使用`<<<Dg, Db, Ns, s>>>`形式的表达式传递内核函数的执行配置参数。四个执行配置参数的含义为：

- `Dg`是一个`dim3`类型的变量，指明网格的维度和大小，`Dg.x*Dg.y*Dg.z`等于启动的块数量
- `Db`是一个`dim3`类型的变量，用于声明块大小，`Db.x*Db.y*Db.z`等于每个块内的线程数量
- `Ns`是一个`size_t`类型的变量，声明动态分配的共享内存大小，默认为0。此时在内核函数中，共享内存的声明为如下格式：`extern __shared__ int Array[]`
- `s`的类型为`cudaStream_t`，用来声明执行该核函数的CUDA流，默认为0，代表使用默认流

### 内建变量

CUDA为运行在GPU上的核函数自动生成了如下的内建变量，可以在函数运行时获得网格和块的尺寸，以及线程索引等信息：

- `gridDim`：是一个包含三个元素`x,y,z`的结构体，分别表示网格在`x,y,z`三个方向上的尺寸，也就是`x,y,z`三个方向上有多少个块
- `blockIdx`：是一个包含三个元素`x,y,z`的结构体，分别表示当前块在`x,y,z`三个方向上的索引
- `blockDim`：是一个包含三个元素`x,y,z`的结构体，分别表示当前块在`x,y,z`三个方向上的尺寸，也就是`x,y,z`三个方向上有多少个线程
- `threadIdx`：是一个包含三个元素`x,y,z`的结构体，分别表示当前线程在`x,y,z`三个方向上的索引
- `warpSize`：表示线程束的大小，这个值通常是32

## 内存管理与使用

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

需要注意的是，这一函数的第一个参数是一个二重指针。二重指针`devptr`存储在CPU内存中，指向一个GPU内存的地址；而解掉一重指针后的一重指针`*devptr`则是被存储在GPU内存中，它存储的是GPU中所分配内存空间的首地址。也就是说，二重指针的作用相当于完成了CPU到GPU的映射。

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

- `cudaMemcpyHostToHost`
- `cudaMemcpyHostToDevice`
- `cudaMemcpyDeviceToHost`
- `cudaMemcpyDeviceToDevice`

对于二维或者三维数组而言，它们的内存分配可以使用`cudaMemcpy2D()`和`cudaMemcpy3D()`这两个函数。

而全局变量（带有`__device__`或者`__constant__`修饰符）的内存传输则需要使用如下两个函数：

```cpp
cudaError_t cudaMemcpyToSymbol(void *dst,const void * src,size_t count); //从CPU内存传输数据到GPU内存
cudaError_t cudaMemcpyFromSymbol(void *dst,const void * src,size_t count); //从GPU内存传输数据到CPU内存
```

### 固定内存

由于主机内存采用分页式管理，应用只能看到虚拟的内存地址，而操作系统可能随时更换物理地址的页（从原始地址复制到另一个地址）。从主机传输到设备上的时候，如果此时发生了页面移动，对于传输操作来说是致命的。所以在数据传输之前，CUDA会锁定页面，或者直接分配固定的主机内存，将主机源数据复制到固定内存上，然后从固定内存传输数据到设备上。因此固定内存又叫锁页内存。而CUDA提供了直接分配固定内存的函数，通过直接使用固定内存，便可加快主机到设备端的内存传输速度。如下图所示：

<img src="https://raw.githubusercontent.com/lyf35/blog_figures/main/img/20210616205548.png" alt="image-20210616205523050" style="zoom:67%;" />

下面函数用来分配固定内存：

```cpp
cudaError_t cudaMallocHost(void ** devPtr,size_t count)
```

这一函数分配`count`字节的固定内存，这些内存是页面锁定的，可以直接传输到GPU设备端，这样使得传输带宽变得高很多。

也可以使用下面的函数来分配固定内存：

```cpp
cudaError_t cudaHostAlloc(void **pHost, size_t count, unsigned int flags)
```

其中`flags`参数支持的四种设置如下：

- `cudaHostAllocDefault`：函数功能与`cudaMallocHost`等价。
- `cudaHostAllocPortable`：所返回的内存空间可以被系统中的所有设备使用。
- `cudaHostAllocWriteCombined`：将固定内存声明为写联合内存，这类内存不使用缓存，此外在传输数据时也可以通过PCIE总线，因此写入较快但是读取较慢。适用于主机端写入，设备端读取的情况。
- `cudaHostAllocMapped`：将所分配的内存空间映射到设备端的地址空间上，也就是零复制内存。

> 备注—零复制内存：
>
> 通常来说，主机端无法直接访问设备端的内存，同样设备端也无法直接访问主机端的内存。但是零复制内存例外，它属于主机端的固定内存，其地址被映射到设备端的地址空间上，可以同时被主机端和设备端访问（也就是说，零复制指的是无需显式的复制过程）。在使用零复制内存时，主机端和设备端不能同时进行操作，否则将会导致未知的结果。
>
> 通过使用零复制内存，可以在设备内存不足时使用主机内存，同时避免了主机和设备之间的显式数据传输。由于隐式复制是异步的，可以与计算并行，因此也隐藏了内存传输的延时。
>
> 在使用零复制内存时需要检查设备是否支持：
>
> ```cpp
> cudaDeviceProp device_prop
> cudaGetDeviceProperties(&device_prop, device_num);
> zero_copy_supported = device_prop.canMapHostMemory;
> ```
>
> 对于零复制内存，设备端和主机端各自有一个地址，主机端地址在分配的时候被保存在`pHost`参数中，设备端地址可以使用如下函数获取：
>
> ```cpp
> cudaError_t cudaHostGetDevicePointer ( void** pDevice, void* pHost, unsigned int flags )
> ```

如果要释放固定内存，可以使用下面的函数：

```cpp
cudaError_t cudaFreeHost(void *ptr)
```

### 共享内存

在核函数中，使用`__shared__`修饰符修饰的变量会被存放在共享内存中。共享内存具有更高的带宽和更低的延迟。共享内存可以当成用户手动管理的缓存来使用，合理地使用共享内存可以提高程序的执行速度。

下面的代码是使用共享内存实现的矩阵乘法（来自于NVIDIA官方文档）：

```cpp
#include<cuda_runtime.h>
#include<iostream>

typedef struct{
        int width;
        int height;
        int stride; //用于内存对齐
        float* elements;
} Matrix;

# define BLOCK_SIZE=16

__device__ float GetElement(const Matrix A, int row, int col)
{
        return A.elements[row * A.stride + col];
}

__device__ void SetElement(Matrix A, int row, int col, float value)
{
        A.elements[row * A.stride + col] = value;
}

__device__ Matrix GetSubMatrix(Matrix A, int row, int col)
{
        Matrix Asub;
        Asub.width = BLOCK_SIZE;
        Asub.height = BLOCK_SIZE;
        Asub.stride = stride;
        Asub.elements = &A.elements[A.stride * BLOCK_SIZE * row + BLOCK_SIZE * col];
        return Asub;
}

__global__ void MatMulKernel(Matrix A, Matrix B, Matrix C)
{
        int blockRow = blockIdx.y;
        int blockCol = blockIdx.x;

        Matrix Csub = GetSubMatrix(C, blockRow, blockCol);

        float Cvalue = 0;

        int row = threadIdx.y;
        int col = threadIdx.x;

        for(int m = 0; m < (A.width / BLOCK_SIZE); ++m)
        {
                Matrix Asub = GetSubMatrix(A, blockRow, m);
                Matrix Bsub = GetSubMatrix(B, m, blockcol);

                __shared__ float As[BLOCK_SIZE][BLOCK_SIZE];
                __shared__ float Bs[BLOCK_SIZE][BLOCK_SIZE];
                As[row][col] = GetElement(Asub, row, col);
                Bs[row][col] = GetElement(Bsub, row, col);

                __syncthreads();
            
                for(int e = 0; e < BLOCK_SIZE; ++e)
                {
                        Cvalue += As[row][e] * Bs[e][col];
                }

                __syncthreads();
        }

        SetElement(Csub, row, col, Cvalue);
}

void MatMul(const Matrix A, const Matrix B, Matrix C)
{
        Matrix d_A;
        d_A.width = A.width;
        d_A.height = A.height;
        size_t size = A.width * A.height * sizeof(float);
        cudaMalloc(&d_A.elements, size);
        cudaMemcpy(d_A.elements, A.elements, size, cudaMemcpyHostToDevice);

        Matrix d_B;
        d_B.width = B.width;
        d_B.height = B.height;
        size_t size = B.width * B.height * sizeof(float);
        cudaMalloc(&d_B.elements, size);
        cudaMemcpy(d_B.elements, B.elements, size, cudaMemcpyHostToDevice);

        Matrix d_C;
        d_C.width = C.width;
        d_C.height = C.height;
        size_t size = C.width * C.height * sizeof(float);
        cudaMalloc(&d_C.elements, size);

        dim3 dimBlock(BLOCK_SIZE, BLOCK_SIZE);
        dim3 dimGrid(B.width / dimBlock.x, A.height/dimBlock.y);
        MatMulKernel<<<dimGrid, dimBlock>>>(d_A, d_B, d_C);

        cudaMemcpy(C.elements, d_C.elements, size, cudaMemcpyDeviceToHost);

        cudaFree(d_A.elements);
        cudaFree(d_B.elements);
        cudaFree(d_C.elements);
}
```

上述代码所对应的矩阵运算步骤可以用下图来表示：

<img src="https://raw.githubusercontent.com/lyf35/blog_figures/main/img/20210531214456.png" alt="共享内存矩阵乘法" style="zoom:50%;" />

## 异步并发执行

### 支持的操作

对于主机来说，GPU设备端的如下操作是异步执行的：

- Kernel函数的执行。如果要禁止kernel函数的异步执行，可以将环境变量`CUDA_LAUNCH_BLOCKING`设置为1。需要注意的是，这种方法只能应用于debug，不应该将其应用于释放版本的软件中
- 设备内部的内存复制操作
- 从主机到设备进行64KB及以下的内存复制操作
- 执行带有`Async`后缀的内存复制函数
- 内存设置函数，例如`cudaMemset`

CUDA支持的并发执行操作包括：

- 多个kernel的并发执行，可以通过查看设备属性`concurrentKernels`来确认设备是否支持这一特性。最大支持的并发数与设备的计算能力有关。但是两个来自不同CUDA context的kernel函数无法并发执行
- 内存复制操作与kernel的执行也可以实现并发，可以查看设备属性`asyncEngineCount`来确认是否支持这一特性，如果大于0则代表支持。但是内存复制操作如果涉及到主机内存，它必须是固定内存（锁页内存）
- 多个内存复制操作也可以并发执行，可以查看设备属性`asyncEngineCount`来确认是否支持这一特性，如果等于2则代表支持。同样地，内存复制操作如果涉及到主机内存，它必须是固定内存（锁页内存）

### CUDA流

上述这些并发操作的执行顺序可以通过使用CUDA流来进行管理。对于同一个CUDA流中的指令，可以保证这些指令按照加入到流中的顺序被执行，而不同CUDA流中的指令则可以被并发执行。

CUDA流的使用方法如下：

```cpp
cudaStream_t stream; //初始化一个CUDA流的变量
cudaStreamCreate(&stream); //创建一个CUDA流
cudaMemcpyAsync(dest, src, cudaMemcpyDeviceToHost, stream); //使用示例：在异步的内存复制操作中使用CUDA流
cudaStreamDestroy(stream); //使用完成之后删除CUDA流
```

含有CUDA流作为参数之一的函数会将这一参数的默认值设置为0，数值0代表使用默认流。在编译CUDA程序时，将`--default-stream`编译选项设置为不同的值时，也会对默认流进行不同的设置：

- `per-thread`：每一个主机的线程都有一个属于自己的默认流。
- `legacy`：对于每一个主机的线程，都使用NULL这个特殊的默认流。这也是该编译选项的默认值。在这种情况下，指令会做隐式的同步。

下图所示为使用CUDA流之后，指令的执行顺序示意图：

<img src="https://raw.githubusercontent.com/lyf35/blog_figures/main/img/20210531215525.png" alt="CUDA流" style="zoom:50%;" />

在CUDA流中，可以使用`cudaLaunchHostFunc()`插入一个主机函数（即CPU上执行的函数）。在主机函数之后的命令会在主机函数执行完成之后才能接着开始执行。需要注意的是，插入到CUDA流中的主机函数不能调用CUDA相关的函数（无论是直接或者间接调用），否则会造成死锁。

CUDA流可以有不同的优先级，对于等待执行的指令，具有高优先级的流会优先执行。如果要为CUDA流赋予不同的优先级，首先要使用`cudaDeviceGetStreamPriorityRange()`函数获得最高与最低优先级的范围，然后使用`cudaStreamCreateWithPriority()`函数创建带有优先级的CUDA流。

### 事件

CUDA中的事件用于记录程序执行过程中的任意一个状态点。在一个CUDA流中，只有当一个事件之前的所有指令全部执行完成之后，一个事件才会处于完成状态。因此CUDA事件可用于并发流的管理。

事件的创建和删除方法如下：

```cpp
cudaEvent_t event;
cudaEventCreate(&event); //创建一个事件
cudaEventDestroy(&event); //删除一个事件
cudaEventRecord(event, stream) //记录某个流中的一个事件
```

### 显式同步与隐式同步

CUDA提供了一些函数，可以实现不同CUDA流之间的显式同步：

- `cudaDeviceSynchronize()`：等待所有主机线程内的所有CUDA流内的未完成指令执行完毕
- `cudaStreamSynchronize()`：需要传入一个CUDA流作为参数，等待这个流中的所有指令执行完成之后才会继续执行后面的指令
- `cudaStreamWaitEvent()`：传入一个CUDA流和一个事件（下文会介绍），只有在这个事件被执行完成之后，在这个事件之后的指令才会被执行
- `cudaStreamQuery()`：传入一个CUDA流，这一函数用于查询该CUDA流内的指令是否执行完成

而对于下面的操作而言，如果主机线程将其放入了不同的CUDA流中，那么它们便不会并发执行（即隐式同步）：

- 为主机分配锁页内存（固定内存）
- GPU内存的分配或设置
- 从两个不同的内存地址，向同一块设备内存做复制操作
- 任何在NULL流上的CUDA指令

### CUDA图

#### 简介

CUDA图对应于一系列的操作，这些操作被按照依赖关系连接起来。CUDA图的定义与执行过程是分开的，这使得CUDA图可以在定义之后被重复执行。通过构造CUDA图，可以减少执行阶段的CPU资源消耗，同时图中的一些操作也可以做进一步的优化。

使用CUDA图的主要流程可以划分为三个阶段：

- 定义：创建CUDA图，定义图中每个节点所对应的操作以及节点之间的依赖关系（节点之间的边）
- 初始化：验证CUDA图的有效性，并对图中所包含的一些设置以及初始化操作进行优化，从而减少启动阶段所需要的工作量。这一步骤最终会生成一个执行图
- 执行：类似于其它的CUDA操作一样，执行图可以被放入一个CUDA流中执行。执行图可以被重复运行，并且每次运行不需要重复初始化动作。

需要注意的是，cudaGraph_t类型的变量不是线程安全的，因此在使用过程中需要确保多个线程不会并发执行同一个cudaGraph_t类型的变量。

关于CUDA图的API可参考：[CUDA Runtime API :: CUDA Toolkit Documentation](https://docs.nvidia.com/cuda/cuda-runtime-api/group__CUDART__GRAPH.html#group__CUDART__GRAPH)

#### 图结构

在CUDA图中，节点由CUDA操作组成，边由操作之间的依赖关系所决定。这些依赖关系决定了节点所对应操作的执行顺序。在执行过程中，如果某个节点所依赖的操作全部被执行完成，那么它就可能会被调度到任意的时刻执行，具体的调度情况由CUDA流决定。

图中节点所对应的操作可以是：

- Kernel函数
- CPU函数调用
- 复制或者设置内存
- 空节点
- 等待或记录一个事件
- 发送或等待一个外部的信号量
- 一个子图

#### 图的创建

图的创建有两种方法。一种是使用显式构造图的函数：

```cpp
cudaGraphCreate(&graph, 0); //创建一个CUDA图
cudaGraphAddKernelNode(&node, graph, NULL, 0, &nodeParams); //添加一个节点
cudaGraphAddDependencies(graph, &node_src, &node_dst, 1); //为两节点之间加入一条边
```

另一种是使用流进行抓取：

```cpp
cudaGraph_t graph;
cudaStreamBeginCapture(stream);
//execution
cudaStreamEndCapture(stream, &graph);
```

借助`cudaEventRecord()`和`cudaStreamWaitEvent()`两个函数，可以将多个流抓取到同一个计算图中。下面为一个伪代码示例：

```cpp
cudaStreamBeginCapture(stream1); //stream1为开始抓取的流
kernel_A<<<……, stream1>>>(……); 

cudaEventRecord(event1, stream1);
cudaStreamWaitEvent(stream2, event1); //stream2也被设置为抓取模式，开始抓取

kernel_B<<<……, stream1>>>(……);
kernel_C<<<……, stream2>>>(……);

cudaEventRecord(event2, stream2); 
cudaStreamWaitEvent(stream1, event2);//结束stream2的抓取

kernel_D<<<……, stream1>>>(……);

cudaStreamEndCapture(stream1, &graph); //最终需要同样以stream1结束抓取
```

## 原子操作

CUDA的原子操作可以理解为对一个变量进行“读取-修改-写入”这三个操作的一个最小单位的执行过程，这个执行过程不能够再分解为更小的部分。原子操作确保了在多个并行线程间共享的内存的读写保护，每次只能有一个线程对该变量进行读写操作。一个线程对该变量操作的时候，其他线程如果也要操作该变量，只能等待前一线程执行完成。

基于这个机制，原子操作实现了对在多个线程间共享的变量的互斥保护，确保任何一次对变量的操作的结果的正确性。原子操作确保了安全，代价是牺牲了性能。

对于常用的逻辑运算与算数操作，CUDA实现了它们的原子操作（CUDA实现的原子操作可参考https://docs.nvidia.com/cuda/cuda-c-programming-guide/index.html#atomic-functions），可以直接调用相应的函数进行计算。



# 程序优化

## 基本策略

总的来说，程序优化的基本策略主要分为三个方向：

1. 最大化并行程度，使得CUDA计算资源可以被最大程度地利用
2. 优化内存使用，获得最大化的内存吞吐量
3. 优化程序指令，获得最大化的指令吞吐量

下面为一些示例和更详细的解释。

## 最大化资源利用

从不同的层级来说，最大化资源利用的办法包括但不限于：

- 应用层级：代码应该尽可能地使用异步并发的方式来执行，以充分利用GPU，CPU以及总线资源。并尽可能地在CPU上执行串行操作，在GPU上执行并行操作。
- 设备层级：在GPU设备端，代码应该尽可能地在多处理器上并行执行。由于多个Kernel函数可以并发执行，因此可以通过使用CUDA流来确保足够的Kernel函数最大化地占用资源。
- 多处理器层级：在一个多处理器单元内，代码也应该尽可能地实现并行化执行

## 优化内存使用

### 使用共享内存

共享内存相当于是手动调配的缓存，合理地使用共享内存可以加快程序执行的速度。比如在GPU上执行矩阵乘法运算时，可以借助共享内存来保存一些中间的计算结果，从而加速矩阵乘法的运算速度。

在调用共享内存时，每个块内的线程按照如下的步骤操作：

1. 从设备内存中读取数据到共享内存
2. 进行块内的线程同步操作，保证每个线程都可以安全地读取共享内存的数据
3. 对共享内存中的数据进行运算
4. 如果需要的话，再进行一次线程同步操作，确保共享内存中的数据已经被更新
5. 将数据写回设备内存

### 主机与设备的数据传输

由于主机与设备之间的数据传输较为缓慢，因此程序需要尽可能地减少设备与主机之间的应用传输。可以使用的办法包括：

- 将临时数据保存在GPU上
- 将许多小规模的数据传输组合为一个大批量的数据传输
- 使用锁页内存

### 内存对齐

在分配及使用内存时，尽可能地让内存的大小满足内存对齐的要求，这样可以加快指令执行速度。对于二维和三维数组来说，可以在分配内存的时候使用二维和三维的内存分配函数，函数会自动对内存进行填充，从而使其满足内存对齐的要求。

## 优化指令

可以从如下一些方面去做指令上的优化：

- 算数操作：如果一些算数操作有CUDA内置函数已经实现过，则优先使用函数操作进行计算，例如单精度除法运算等；在运算精度允许的条件下，尽可能使用单精度浮点数；等等。对于CUDA的内置数学函数，可以参考https://docs.nvidia.com/cuda/cuda-c-programming-guide/index.html#mathematical-functions-appendix。而对于矩阵运算而言，则可以使用cuBLAS库中的函数。
- 条件语句：如果判断条件与线程ID有关，则最好是将条件语句改写为若干条顺序执行的指令。否则将会导致同一线程束内出现分支，导致线程束内执行的总指令数增加。
- 循环语句：通过使用循环展开，可以加速循环体的执行速度。可以通过在for循环语句的前面加`#pragma unroll Num`来简单实现（`Num`指的是循环展开数，可以设置为任意正整数）
- 同步操作：同步操作`__syncthreads()`会强制一些多处理器处于不工作状态，同时在一个时钟周期内也需要做若干指令操作，在设计程序时需要考虑这一点。



# 扩展库

CUDA提供了一些扩展库，用于加速一些特定的运算：

- cuFFT：用于快速傅里叶变换的函数库，[cuFFT :: CUDA Toolkit Documentation](https://docs.nvidia.com/cuda/cufft/index.html#introduction)
- 数学函数库：用于加速一些特定的数学运算，[CUDA Math API :: CUDA Toolkit Documentation](https://docs.nvidia.com/cuda/cuda-math-api/modules.html#modules)
- cuBLAS：用于线性代数运算的函数库，[cuBLAS :: CUDA Toolkit Documentation ](https://docs.nvidia.com/cuda/cublas/index.html#using-the-cublas-api)
- 更多的函数库可见[NVIDIA Developer Documentation](https://docs.nvidia.com/cuda-libraries/index.html)



# 参考

1. CUDA并行程序设计：GPU编程指南
2. Professional CUDA C Programming
3. 
4. https://www.zhihu.com/question/19903344
5. https://docs.nvidia.com/cuda/cuda-c-programming-guide/index.html
6. [深入GPU硬件架构及运行机制 - 0向往0 - 博客园 (cnblogs.com)](https://www.cnblogs.com/timlly/p/11471507.html)
7. [GPU中的几个基本概念 - 知乎 (zhihu.com)](https://zhuanlan.zhihu.com/p/53763285)
8. [cuda 学习之内存层次结构_xukang95的博客-CSDN博客](https://blog.csdn.net/xukang95/article/details/102855750)
9. [【CUDA 基础】4.1 内存模型概述 | 谭升的博客 (face2ai.com)](https://face2ai.com/CUDA-F-4-1-内存模型概述/)
10. [CUDA编程笔记——chapter4 CUDA全局内存 - 知乎 (zhihu.com)](https://zhuanlan.zhihu.com/p/360032964)
11. [CUDA修饰符，运算符＜＜＜＞＞＞及变量的介绍_tangbenjun的博客-CSDN博客](https://blog.csdn.net/tangbenjun/article/details/77988885)
12. [CUDA中修饰符的解释 - 袁军峰 - 博客园 (cnblogs.com)](https://www.cnblogs.com/tibetanmastiff/p/4712618.html)
13. [关于cudaMalloc内存分配以及二重指针_fishming的blog-CSDN博客](https://blog.csdn.net/weixin_43260254/article/details/104162207)
14. [“CUDA Tutorial” (jhui.github.io)](https://jhui.github.io/2017/03/06/CUDA/)
15. [CUDA学习笔记(5) 原子操作_GongBingjian-CSDN博客_cuda 原子操作](https://blog.csdn.net/q583956932/article/details/78826987)
16. [cublas 实战_thesby的专栏-CSDN博客_cublas](https://blog.csdn.net/thesby/article/details/52728529)
17. The CUDA Handbook

