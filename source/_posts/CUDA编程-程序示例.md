---
title: CUDA编程-程序示例
tags: 
- CUDA
- 并行计算
categories: [工程实践, CUDA]
sharing: True
date: 2021-06-06 09:37:29
updated: 2021-06-10 23:31:32
---



# 并发

在下面一个简单的示例程序中，通过使用CUDA流来使得一些CUDA操作可以被并发执行：

```cpp
#include<cuda_runtime.h>
#include<iostream>

#define N (1024*1024)
#define FULL_DATA_SIZE (N*20)

__global__ void kernel(int* a, int* b, int* c)
{
    int idx = threadIdx.x + blockIdx.x * blockDim.x;
    if(idx < N)
    {
        int idx1 = (idx + 1) % 256;
        int idx2 = (idx + 2) % 256;
        float as = (a[idx] + a[idx1] + a[idx2]) / 3.0;
        float bs = (b[idx] + b[idx1] + b[idx2]) / 3.0;
        c[idx] = (as + bs) / 2;
    }
}

int main()
{
    cudaEvent_t start, stop;
    float elapsedTime;

    cudaEventCreate(&start);
    cudaEventCreate(&stop);
    cudaEventRecord(start, 0);

    cudaStream_t stream0, stream1;
    cudaStreamCreate(&stream0);
    cudaStreamCreate(&stream1);

    int *host_a, *host_b, *host_c;
    int *dev_a0, *dev_b0, *dev_c0;
    int *dev_a1, *dev_b1, *dev_c1;

    cudaMalloc((void**)&dev_a0, N * sizeof(int));
    cudaMalloc((void**)&dev_a1, N * sizeof(int));
    cudaMalloc((void**)&dev_b0, N * sizeof(int));
    cudaMalloc((void**)&dev_b1, N * sizeof(int));
    cudaMalloc((void**)&dev_c0, N * sizeof(int));
    cudaMalloc((void**)&dev_c1, N * sizeof(int));

    host_a = (int*)malloc(FULL_DATA_SIZE * sizeof(int));
    host_b = (int*)malloc(FULL_DATA_SIZE * sizeof(int));
    host_c = (int*)malloc(FULL_DATA_SIZE * sizeof(int));

    for(int i = 0; i < FULL_DATA_SIZE; i++)
    {
        host_a[i] = rand();
        host_b[i] = rand();
    }

    for(int i = 0; i < FULL_DATA_SIZE; i += N * 2)
    {
        cudaMemcpyAsync(dev_a0, host_a + i, N * sizeof(int), cudaMemcpyHostToDevice, stream0);
        cudaMemcpyAsync(dev_b0, host_b + i, N * sizeof(int), cudaMemcpyHostToDevice, stream0);
        kernel<<<N/256, 256, 0, stream0>>>(dev_a0, dev_b0, dev_c0);

        cudaMemcpyAsync(dev_a1, host_a + i + N, N * sizeof(int), cudaMemcpyHostToDevice, stream1);
        cudaMemcpyAsync(dev_b1, host_b + i + N, N * sizeof(int), cudaMemcpyHostToDevice, stream1);
        kernel<<<N/256, 256, 0, stream1>>>(dev_a1, dev_b1, dev_c1);
    }
    
    cudaStreamSynchronize(stream0);
    cudaStreamSynchronize(stream1);

    cudaEventRecord(stop, 0);
    cudaEventSynchronize(stop);
    cudaEventElapsedTime(&elapsedTime, start, stop);
    std::cout << elapsedTime << std::endl;

    cudaFreeHost(host_a);
    cudaFreeHost(host_b);
    cudaFreeHost(host_c);
    cudaFreeHost(dev_a0);
    cudaFreeHost(dev_b0);
    cudaFreeHost(dev_c0);
    cudaFreeHost(dev_a1);
    cudaFreeHost(dev_b1);
    cudaFreeHost(dev_c1);
    cudaStreamDestroy(stream0);
    cudaStreamDestroy(stream1);
    
    return 0;
}
```

经测试，如果将上面代码中所有涉及到CUDA流的参数全部改为相同的流（如`stream0`），相比于并发版本，多次执行的平均时间会慢约20ms左右（显卡为GTX 1650）。



# 直方图算法

## 概述

直方图算法指的是对一个数组$a=[a_1,a_2,\dots,a_n]$做它的统计直方图。在统计每个直方所对应频数的时候，需要保证它的线程安全性，因此需要使用到原子加操作。

## 程序示例

下面的程序用于计算一个数组的统计直方图，出于简便起见，其中所有元素的值为0\~255，且直方图的直方数也设为256。对于其它情况，则只需要对每个元素添加一步计算即可：$\text{bin}=\frac{a_i-\min}{\max-\min}*\text{num_bins}$

```cpp
#include<cuda_runtime.h>
#include<iostream>

#define SIZE (100*1024*1024)

void* big_random_block( int size ) 
{
    unsigned char *data = (unsigned char*)malloc( size );
    for (int i=0; i < size; i++)
        data[i] = rand();

    return data;
}

__global__ void histo_kernel(unsigned char* buffer, int size, unsigned int* histo)
{
    __shared__ unsigned int temp[256];//every block compute a local histogram
    temp[threadIdx.x] = 0;
    __syncthreads();
    int i = threadIdx.x + blockIdx.x * blockDim.x;
    int offset = blockDim.x * gridDim.x;
    while(i < size)
    {
        atomicAdd(&(temp[buffer[i]]), 1); //atomic add to ensure thread safety
        i += offset;
    }
    __syncthreads();
    atomicAdd(&(histo[threadIdx.x]), temp[threadIdx.x]); //atomic add to ensure thread safety
}

int main()
{
    unsigned char* buffer = (unsigned char*) big_random_block(SIZE);
    unsigned char* dev_buffer;
    unsigned int* dev_histo;

    cudaEvent_t start, stop;
    cudaEventCreate(&start);
    cudaEventCreate(&stop);
    cudaEventRecord(start, 0);

    cudaMalloc((void**)&dev_buffer, SIZE);
    cudaMemcpy(dev_buffer, buffer, SIZE, cudaMemcpyHostToDevice);
    cudaMalloc((void**)&dev_histo, 256 * sizeof(int));
    cudaMemset(dev_histo, 0, 256 * sizeof(int));

    unsigned int histo[256];
    histo_kernel<<<(SIZE + 255)/256, 256>>>(dev_buffer, SIZE, dev_histo);
    cudaMemcpy(histo, dev_histo, 256 * sizeof(int), cudaMemcpyDeviceToHost);
    cudaEventRecord(stop, 0);
    cudaEventSynchronize(stop);

    float elapsedTime;
    cudaEventElapsedTime(&elapsedTime, start, stop);
    std::cout<< elapsedTime << std::endl;

    cudaEventDestroy(start);
    cudaEventDestroy(stop);
    cudaFree(dev_histo);
    cudaFree(dev_buffer);

    return 0;
}
```



# 并行规约问题

## 概述

对于一个数组$a=[a_1,a_2,\dots,a_n]$，规约问题指的是计算如下表达式的值：
$$
\sum_i a_{i}=a_1 \oplus a_2  \oplus \dots\oplus a_n
$$
其中，$\oplus$指的是二元运算符，例如最小值、最大值、求和、平方和、逻辑运算（与、或）等。在上述表达式中，运算的结合律是成立的，也就是说可以按照任意顺序进行计算。

<!--more-->

## 算法

以8个元素的规约问题为例，我们可以有如下三种不同的计算顺序（当然不限于这三种）：

<img src="https://raw.githubusercontent.com/lyf35/blog_figures/main/img/20210606102236.png" alt="image-20210606102228898" style="zoom: 33%;" />

如上图所示，无论是串行的方式还是以log-step的方式进行规约，都需要做7次计算。但是区别在于，如果是按照串行的方式进行计算，总共需要7步才能执行完所有操作；而如果使用并行计算以log-step的方式去执行，则只需要3步即可完成所有运算。

在编写CUDA程序时，图中第三种的交织（interleaved）规约式的算法有着更快的计算速度。这是因为无论是对于全局内存还是共享内存来说，这种规约方式可以提高内存读写效率。

> - 使用交织规约的方式，一个线程块内的所有线程读取的是一块连续的内存，每个线程只读取这段内存中的其中一个元素
>
> - 而如果使用pairwise的规约方式，一个线程块内的所有线程在读取内存时则是间隔式的，这样不利于缓存的使用（因为线程块被分配到SM上执行，而一个SM内的所有计算单元又共享缓存），也会产生内存的bank冲突。

## 程序示例

### 向量内积的并行计算

下面的程序用于并行计算两个向量的内积：

```cpp
#include<cuda_runtime.h>
#include<iostream>
#include<cmath>

const int N = 33 * 1024;
const int threadsPerBlock = 256;
const int blocksPerGrid = min(32, (N + threadsPerBlock - 1) / threadsPerBlock);

__global__ void dot(float *a, float *b, float *c)
{
    __shared__ float cache[threadsPerBlock]; 
    int tid = threadIdx.x + blockIdx.x * blockDim.x; //thread ID
    int cacheIndex = threadIdx.x; //0 ~ threadsPerBlock-1
    float temp = 0;
    while(tid < N) //reduce in interleave way 
    {
        temp += a[tid] * b[tid];
        tid += blockDim.x * gridDim.x;
    }
    cache[cacheIndex] = temp;
    __syncthreads(); //fill the cache array

    int i = blockDim.x / 2; 
    while(i != 0) //reduction in parallel way. 
    {
        if(cacheIndex < i)
        {
            cache[cacheIndex] += cache[cacheIndex + i];
        }
        __syncthreads();
        i /= 2;
    }
    if(cacheIndex == 0)
    {
        c[blockIdx.x] = cache[0]; //the sum of values within the block
    }
}

int main()
{
    float *a, *b, c, *partial_c;
    float *dev_a, *dev_b, *dev_partial_c;

    a = (float*)malloc(N * sizeof(float));
    b = (float*)malloc(N * sizeof(float));
    partial_c = (float*)malloc(blocksPerGrid * sizeof(float));

    cudaMalloc((void**)&dev_a, N * sizeof(float));
    cudaMalloc((void**)&dev_b, N * sizeof(float));
    cudaMalloc((void**)&dev_partial_c, blocksPerGrid * sizeof(float));

    for(int i = 0; i < N; i++)
    {
        a[i] = i;
        b[i] = i * 2;
    }

    cudaMemcpy(dev_a, a, N * sizeof(float), cudaMemcpyHostToDevice);
    cudaMemcpy(dev_b, b, N * sizeof(float), cudaMemcpyHostToDevice);

    dot<<<blocksPerGrid, threadsPerBlock>>>(dev_a, dev_b, dev_partial_c);
    cudaMemcpy(partial_c, dev_partial_c, blocksPerGrid * sizeof(float), cudaMemcpyDeviceToHost);

    c = 0;
    for(int i = 0; i < blocksPerGrid; i++)
    {
        c += partial_c[i];
    }

    std::cout << c << "   " << (2 * float(N - 1) * float(N) * float(2 * N - 1) / 6)  << std::endl;
    
    cudaFree(dev_a);
    cudaFree(dev_b);
    cudaFree(dev_partial_c);

    free(a);
    free(b);
    free(partial_c);

    return 0;
}
```

### 循环展开优化

对于上面的核函数来说，可以使用循环展开的方法，对程序做进一步的优化：

```cpp
__global__ void dot(float *a, float *b, float *c)
{
    __shared__ float cache[threadsPerBlock]; 
    int tid = threadIdx.x + blockIdx.x * blockDim.x; //thread ID
    int cacheIndex = threadIdx.x; //0 ~ threadsPerBlock-1
    float temp = 0;
    while(tid < N) 
    {
        temp += a[tid] * b[tid];
        tid += blockDim.x * gridDim.x;
    }
    cache[cacheIndex] = temp;
    __syncthreads(); //fill the cache array

	if(threadsPerBlock >= 1024)
    {
        if(threadIdx.x < 512)
        {
            cache[threadIdx.x] += cache[threadIdx.x + 512];
        }
        __syncthreads();
    }
    
   	if(threadsPerBlock >= 512)
    {
        if(threadIdx.x < 256)
        {
            cache[threadIdx.x] += cache[threadIdx.x + 256];
        }
        __syncthreads();
    }
    
    if(threadsPerBlock >= 256)
    {
        if(threadIdx.x < 128)
        {
            cache[threadIdx.x] += cache[threadIdx.x + 128];
        }
        __syncthreads();
    }
    
    if(threadsPerBlock >= 128)
    {
        if(threadIdx.x < 64)
        {
            cache[threadIdx.x] += cache[threadIdx.x + 64];
        }
        __syncthreads();
    }
    
    if(threadIdx.x < 32)
    {
        volatile float *dotSum = cache; //use volatile to ensure thread safety
        if(threadsPerBlock >= 64){ dotSum[threadIdx.x] += dotSum[threadIdx.x + 32]; }
        if(threadsPerBlock >= 32){ dotSum[threadIdx.x] += dotSum[threadIdx.x + 16]; }
        if(threadsPerBlock >= 16){ dotSum[threadIdx.x] += dotSum[threadIdx.x + 8]; }
        if(threadsPerBlock >= 8){ dotSum[threadIdx.x] += dotSum[threadIdx.x + 4]; }
        if(threadsPerBlock >= 4){ dotSum[threadIdx.x] += dotSum[threadIdx.x + 2]; }
        if(threadsPerBlock >= 2){ dotSum[threadIdx.x] += dotSum[threadIdx.x + 1]; }
        if(threadIdx.x == 0)
        {
            c[blockIdx.x] = dotSum[0];
        }
    }
}
```

### 原子操作

如果使用原子操作，则可以使得程序变得更加简洁：

```cpp
#include<cuda_runtime.h>
#include<iostream>
#include<cmath>

const int N = 33 * 1024;
const int threadsPerBlock = 256;
const int blocksPerGrid = min(32, (N + threadsPerBlock - 1) / threadsPerBlock);

__global__ void dot(float *a, float *b, float *c)
{
    int tid = threadIdx.x + blockIdx.x * blockDim.x; //thread ID
    int cacheIndex = threadIdx.x; //0 ~ threadsPerBlock-1
    float temp = 0;
    while(tid < N) //reduce in interleave way 
    {
        temp += a[tid] * b[tid];
        tid += blockDim.x * gridDim.x;
    }
    atomicAdd(c, temp);
}

int main()
{
    float *a, *b, *c;
    float *dev_a, *dev_b, *dev_c;

    a = (float*)malloc(N * sizeof(float));
    b = (float*)malloc(N * sizeof(float));
    c = (float*)malloc(sizeof(float));

    cudaMalloc((void**)&dev_a, N * sizeof(float));
    cudaMalloc((void**)&dev_b, N * sizeof(float));
    cudaMalloc((void**)&dev_c, sizeof(float));

    for(int i = 0; i < N; i++)
    {
        a[i] = i;
        b[i] = i * 2;
    }

    cudaMemcpy(dev_a, a, N * sizeof(float), cudaMemcpyHostToDevice);
    cudaMemcpy(dev_b, b, N * sizeof(float), cudaMemcpyHostToDevice);

    dot<<<blocksPerGrid, threadsPerBlock>>>(dev_a, dev_b, dev_c);
    cudaMemcpy(c, dev_c, sizeof(float), cudaMemcpyDeviceToHost);

    std::cout << c << "   " << (2 * float(N - 1) * float(N) * float(2 * N - 1) / 6)  << std::endl;
    
    cudaFree(dev_a);
    cudaFree(dev_b);
    cudaFree(dev_c);

    free(a);
    free(b);
    free(c);

    return 0;
}
```

但不是所有的二元运算符都有对应的原子操作，不支持原子操作的二元运算符无法按照上面的代码去实现。



# 扫描算法

## 概述

对于一个长度为N的数组$[a_0,a_1,\dots,a_{N-1}]$，以及二元运算符$\oplus$，扫描算法分为两种不同的类型：

- inclusive：返回数组$[a_0,(a_0 \oplus a_1),\dots,(a_0 \oplus a_1 \oplus \dots \oplus a_{N-1})]$
- exclusive：返回数组$[id_{\oplus},a_0,(a_0 \oplus a_1),\dots,(a_0 \oplus a_1 \oplus \dots \oplus a_{N-2})]$，其中$id_{\oplus}$指的是对于运算符$\oplus$的单位元素，例如对于加法来说为0，而对于乘法来说为1

需要说明的是，对于inclusive和exclusive这两种不同的结果类型，可以通过改变原始数组来实现互相转化。如果扫描算法是inclusive类型的，我们可以将其计算结果的最后一个元素删除，并在第一个元素前添加一个运算单位元（例如乘法运算添加一个1），从而得到exclusive的计算结果；相对应地，如果扫描算法是exclusive类型的，我们可以在原数组的末尾添加一个运算单位元，并在扫描结束之后删除结果的第一个元素，即可得到inclusive的计算结果。

## 算法

### Hillis Steele算法

下图所示为Hillis Steele算法的计算过程：

![39fig02.jpg](https://raw.githubusercontent.com/lyf35/blog_figures/main/img/20210606231608.jpg)

从中可以看出，这种算法只需要执行$\log N$步，但是总体的计算量为$\mathcal{O}(N\log N)$。这种算法适用于在处理器的数量足够的情况下（即数据量较小），需要减少计算时间的场景。需要注意的是，Hillis Steele算法最终得到是inclusive的扫描结果。

### Blelloch算法

Blelloch算法需要分为两步来计算，首先第一步是向上的规约，如下图所示：

![39fig03.jpg](https://raw.githubusercontent.com/lyf35/blog_figures/main/img/20210606232657.jpg)

在规约完成之后，接下来一步则需要做向下的扫描：

![39fig04.jpg](https://raw.githubusercontent.com/lyf35/blog_figures/main/img/20210606232347.jpg)

从中可以看出，这种算法只需要执行$2\log N$步，但是总体的计算量为$\mathcal{O}(N)$。这种算法适用于处理器的数量不足（即数据量较大）的情况。从图中也可以看出，Blelloch算法最终得到是exclusive的扫描结果。

### 分块扫描

由于输入数据可能规模很大，扫描通常需要采取分块的方式，因此在分块扫描之后需要对每一块的结果再做进一步的处理。常用的方式有两种，第一种方式称为Scan-then-fan，指的是先分块做扫描，然后再取出每一块的最后一个值并对它们做扫描，最后将这些值扇入到对应的块中。示意图如下：

<img src="https://raw.githubusercontent.com/lyf35/blog_figures/main/img/20210606233408.png" alt="image-20210606233403977" style="zoom: 33%;" />

而第二种方式称为reduce-then-scan，这种方式则是先对每一个分块做规约，然后取出每个分块规约后的值并对它们做扫描，最后再对每个分块做扫描操作并将上一步得到的结果扇入对应的分块中：

<img src="https://raw.githubusercontent.com/lyf35/blog_figures/main/img/20210606233447.png" style="zoom: 50%;" />

在以上两种分块扫描方式中，如果原始数据的量特别大时，第二步操作可能需要进行多次，此时相对应的扇入操作也需要进行多次。

## 程序示例

下面的程序是使用Hillis Steele扫描算法，然后使用Scan-then-fan的方式进行规约。由于扫描操作的数据量较大，因此在实际操作中对分块和做了两次的扫描操作，相应地也做了两次的扇入操作：

```cpp
#include<cuda_runtime.h>
#include<iostream>
#include<vector>

#define SIZE (100*256*256)
#define BLOCKSIZE 256

float* big_random_block(int size) 
{
    float *data = (float*)malloc(size * sizeof(float));
    for (int i = 0; i < size; i++)
    {
        //data[i] = rand() / 100000.0;
        data[i] = 0.0001;
    }
    return data;
}

__global__ void Hillis_Steele_scan(float* a, float* block_sum)
{
    int offset = blockDim.x * blockIdx.x;
    int thread_id = threadIdx.x;
    __shared__ float b[BLOCKSIZE];
    for(int i = 1; i < BLOCKSIZE; i *= 2)
    {
        if(thread_id < i)
        {
            b[thread_id] = a[thread_id + offset];
        }
        else
        {
            b[thread_id] = a[thread_id + offset] + a[thread_id + offset - i];
        }
        __syncthreads();
        a[thread_id + offset] = b[thread_id];
        __syncthreads();
    }
    if(thread_id == BLOCKSIZE - 1)
    {
        block_sum[blockIdx.x] = b[BLOCKSIZE - 1];
    }
}

__global__ void fan_elements(float* a, float* block_sum_scanned)
{
    int offset = blockDim.x * blockIdx.x;
    int thread_id = threadIdx.x;
    if(blockIdx.x > 0)
    {
        a[offset + thread_id] += block_sum_scanned[blockIdx.x - 1];
    }
}

int main()
{
    float* a = big_random_block(SIZE);
    float* res = (float*)malloc(SIZE * sizeof(float));

    cudaEvent_t start, stop;
    cudaEventCreate(&start);
    cudaEventCreate(&stop);
    cudaEventRecord(start, 0);

    int size = SIZE;
    float *dev_a;
    cudaMalloc((void**)&dev_a, size * sizeof(float));
    cudaMemcpy(dev_a, a, size * sizeof(float), cudaMemcpyHostToDevice);

    float *dev_blocksum_layer1;
    size = (size + BLOCKSIZE - 1) / BLOCKSIZE;
    cudaMalloc((void**)&dev_blocksum_layer1, size * sizeof(float));
    cudaMemset(dev_blocksum_layer1, 0.0, size * sizeof(float));
    Hillis_Steele_scan<<<size, BLOCKSIZE>>>(dev_a, dev_blocksum_layer1);

    float *dev_blocksum_layer2;
    size = (size + BLOCKSIZE - 1) / BLOCKSIZE;
    cudaMalloc((void**)&dev_blocksum_layer2, size * sizeof(float));
    cudaMemset(dev_blocksum_layer2, 0.0, size * sizeof(float));
    Hillis_Steele_scan<<<size, BLOCKSIZE>>>(dev_blocksum_layer1, dev_blocksum_layer2);

    float* dev_sum;
    cudaMalloc((void**)&dev_sum, sizeof(float));
    cudaMemset(dev_sum, 0.0, sizeof(float));
    Hillis_Steele_scan<<<1, BLOCKSIZE>>>(dev_blocksum_layer2, dev_sum);

    fan_elements<<<size, BLOCKSIZE>>>(dev_blocksum_layer1, dev_blocksum_layer2);
    fan_elements<<<size * BLOCKSIZE, BLOCKSIZE>>>(dev_a, dev_blocksum_layer1);
    
    cudaMemcpy(res, dev_a, SIZE * sizeof(float), cudaMemcpyDeviceToHost);
    cudaEventRecord(stop, 0);
    cudaEventSynchronize(stop);
    
    float elapsedTime;
    cudaEventElapsedTime(&elapsedTime, start, stop);
    std::cout<< elapsedTime << std::endl;

    cudaEventDestroy(start);
    cudaEventDestroy(stop);
    cudaFree(dev_a);

    return 0;
}
```

下面的程序使用了Blelloch扫描算法，并使用Reduce-then-scan的分块扫描策略：

```cpp
#include<cuda_runtime.h>
#include<iostream>
#include<vector>

#define SIZE (100*256)
#define BLOCKSIZE 256

float* big_random_block(int size) 
{
    float *data = (float*)malloc(size * sizeof(float));
    for (int i = 0; i < size; i++)
    {
        //data[i] = rand() / 100000.0;
        data[i] = 0.0001;
    }
    return data;
}

__global__ void Blelloch_scan(float* a)
{
    int offset = blockDim.x * blockIdx.x;
    __shared__ float temp[BLOCKSIZE];
    temp[threadIdx.x] = a[offset + threadIdx.x];
    __syncthreads();
    //up reduce
    for(int i = 1; i < BLOCKSIZE; i *= 2)
    {
        if((threadIdx.x + 1) % (2*i) == 0)
        {
            temp[threadIdx.x] += temp[threadIdx.x - i];
        }
        __syncthreads();
    }
    
    if(threadIdx.x == BLOCKSIZE - 1)
    {
        temp[threadIdx.x] = 0;
    }
    __syncthreads();
    //down scan
    for(int i = BLOCKSIZE / 2; i >= 1; i /= 2)
    {
        if((threadIdx.x + 1) % (2*i) == 0)
        {
            float temp_val = temp[threadIdx.x];
            temp[threadIdx.x] += temp[threadIdx.x - i];
            temp[threadIdx.x - i] = temp_val;
        }
        __syncthreads();
    }
    a[offset + threadIdx.x] = temp[threadIdx.x];
}

__global__ void reduce_partial_sum(float *a, float* partial_sum)
{
    __shared__ float cache[BLOCKSIZE]; 
    int tid = threadIdx.x + blockIdx.x * blockDim.x; //thread ID
    int cacheIndex = threadIdx.x; 
    cache[cacheIndex] = a[tid];
    __syncthreads(); //fill the cache array

    int i = blockDim.x / 2; 
    while(i != 0) //reduction in parallel way. 
    {
        if(cacheIndex < i)
        {
            cache[cacheIndex] += cache[cacheIndex + i];
        }
        __syncthreads();
        i /= 2;
    }
    if(cacheIndex == 0)
    {
        partial_sum[blockIdx.x] = cache[0]; //the sum of values within the block
    }
}

__global__ void fan_elements(float* a, float* block_sum_scanned)
{
    int offset = blockDim.x * blockIdx.x;
    int thread_id = threadIdx.x;
    a[offset + thread_id] += block_sum_scanned[blockIdx.x];
}

int main()
{
    float* a = big_random_block(SIZE);
    float* res = (float*)malloc(SIZE * sizeof(float));

    cudaEvent_t start, stop;
    cudaEventCreate(&start);
    cudaEventCreate(&stop);
    cudaEventRecord(start, 0);

    int size = SIZE;
    float *dev_a;
    cudaMalloc((void**)&dev_a, size * sizeof(float));
    cudaMemcpy(dev_a, a, size * sizeof(float), cudaMemcpyHostToDevice);

    size = (size + BLOCKSIZE - 1) / BLOCKSIZE;
    float* dev_partial_sum;
    cudaMalloc((void**)&dev_partial_sum, size * sizeof(float));
    cudaMemset(dev_partial_sum, 0.0, size * sizeof(float));

    reduce_partial_sum<<<size, BLOCKSIZE>>>(dev_a, dev_partial_sum);
    Blelloch_scan<<<1, BLOCKSIZE>>>(dev_partial_sum);

    Blelloch_scan<<<size, BLOCKSIZE>>>(dev_a);
    fan_elements<<<size, BLOCKSIZE>>>(dev_a, dev_partial_sum);
    
    cudaMemcpy(res, dev_a, SIZE * sizeof(float), cudaMemcpyDeviceToHost);
    cudaEventRecord(stop, 0);
    cudaEventSynchronize(stop);
    
    float elapsedTime;
    cudaEventElapsedTime(&elapsedTime, start, stop);
    std::cout<< elapsedTime << std::endl;

    for(auto i = 0; i < SIZE; i++)
    {
        std::cout << res[i] <<"  ";
    }
    std::cout << std::endl;

    cudaEventDestroy(start);
    cudaEventDestroy(stop);
    cudaFree(dev_a);

    return 0;
}
```

在上面的两个程序中，待规约数组中所有的数据都被设置为0.0001，以检查输出是否正确。通过检查程序的输出，上面的两个程序都可以正确地对数组进行规约。



# N-Body算法

## 概述

N-Body问题指的是在一个含有N个物体（可以是天体、分子等）的体系中，编号为$i(1\le i \le N)$的物体具有自己的位置$\boldsymbol{x}_i$和速度$\boldsymbol{v}_i$，并且每个物体都受到其它所有物体的力。给定这些条件，去模拟整个体系的演化过程。

以天体模拟为例，两个天体之间的作用力（即万有引力）$\boldsymbol{f}_{ij}$可以用下面的公式计算：
$$
\boldsymbol{f}_{ij}=G\frac{m_i m_j \boldsymbol{d}_{ij}}{(||\boldsymbol{d}_{ij}||^2+\epsilon^2)^{3/2}}
$$
其中$\epsilon$为一个松弛变量，用于保证数值计算的稳定性。

故每个天体受到其它所有天体的合力为$\boldsymbol{F}_{i}=\sum_{1\le j \le N, j\ne i}^{N}\boldsymbol{f}_{ij}$。根据牛顿第二定律，可以计算出加速度$\boldsymbol{a}_{i}=\frac{\boldsymbol{F}_{i}}{m_i}$。

根据leapfrog Verlet数值算法，下一时刻的位置可以写为：
$$
\boldsymbol{v}_i(t+\frac{1}{2}\delta t)=\boldsymbol{v}_i(t-\frac{1}{2}\delta t)+\delta t \boldsymbol{a}_{i} \\
\boldsymbol{x}_i(t+\delta t)=\boldsymbol{x}_i(t)+\delta t\boldsymbol{v}_i(t+\frac{1}{2}\delta t)
$$


不难看出，在模拟演化过程时，最重要也是计算量最大的部分在于计算每个天体受到的合力。以8个物体的体系为例，我们可以得到如下图所示的作用力矩阵，以及如何通过作用力矩阵计算每个天体所受的合力：

<img src="https://raw.githubusercontent.com/lyf35/blog_figures/main/img/20210609201930.png" alt="image-20210608150046402" style="zoom: 33%;" />

从上图中可以看出，这一算法的复杂度为$\mathcal{O}(N^2)$。如果我们使用串行的算法，那么程序运行时间也相应地为$\mathcal{O}(N^2)$。但是由于GPU有强大的并行计算能力，对于上图中作用力矩阵的每一行都可以并行地进行计算。也就是在GPU中使用多个线程并行计算不同天体之间的力，而每个线程串行地计算某个天体所受的合力。这样便可以将时间复杂度降低到$\mathcal{O}(N)$。

但是在实际的计算过程中，每个线程都需要从内存中读取其它天体的位置信息来计算。如果将天体位置信息的数据保存在GPU的全局内存上，由于全局内存的读写速度较慢，这样会影响整体的性能。考虑到每个线程在计算天体合力的时候，都是按照$0～N$的顺序依次进行计算，因此可以使用共享内存来加速内存访问。

由于共享内存被一个线程块内的所有线程共享，而且其大小有限，因此需要进行多批次的加载与计算。我们设线程块内的线程数量为$p$，那么每个线程块分配$p*\text{sizeof(body)}$的共享内存空间。在计算时，$p$个线程先将$p$个天体的数据加载到共享内存当中，在后面的计算过程中便可读取共享内存中的数据进行计算。计算完成之后，再加载新的$p$个天体的数据并接着计算。如此重复进行直到计算完成。这一过程可以表示为下图：

![image-20210610213830574](https://raw.githubusercontent.com/lyf35/blog_figures/main/img/20210610213846.png)

## 程序示例

下面的程序模拟了一个包含两个团簇，共25600个粒子的体系，以0.1的步长迭代1000步。每一步迭代的结果会记录下来，之后便可以使用保存下来的结果来作图。

```cpp
#include <cuda_runtime.h>
#include <iostream>
#include <fstream>
#include <random>

__constant__ float EPS2 = 1e-10;
const int N = 100 * 256;
const int BLOCKSIZE = 256;

//CUDA already defined float4 and float3 datatype.

//calculate the interaction between two bodies, adding to the existing acceleration vector.
__device__ float3 bodyBodyInteraction(float4 body_i, float4 body_j, float3 accel_i)
{
	float3 radius;

	//calculate the distance vector
	radius.x = body_j.x - body_i.x;
	radius.y = body_j.y - body_i.y;
	radius.z = body_j.z - body_i.z;

	//calculate the acceleration
	float distSqr = radius.x * radius.x + radius.y * radius.y + radius.z * radius.z + EPS2;

	float distSixth = distSqr * distSqr * distSqr;
	float invDistCube = 1.0 / sqrtf(distSixth);

	float s = body_j.w * invDistCube;

	accel_i.x += radius.x * s;
	accel_i.y += radius.y * s;
	accel_i.z += radius.z * s;

	return accel_i;
}

//calculate the sum of acceleration vector of one body with all the bodies in the shared memory.
__device__ float3 tile_calculation(float4 Position, float3 accel, float4* sharedPosition)
{
#pragma unroll 8
	for (int i = 0; i < blockDim.x; i++)
	{
		accel = bodyBodyInteraction(Position, sharedPosition[i], accel);
	}
	return accel;
}

//calculate the acceleration of all particles
__global__ void calculate_accel(float4* global_pos, float4* global_accel)
{
	__shared__ float4 sharedPosition[BLOCKSIZE]; //usd to store the position of bodies within.

	float3 accel = { 0.0f, 0.0f, 0.0f };
	int global_threadId = blockIdx.x * blockDim.x + threadIdx.x;

	//get the position of a body
	float4 Position = global_pos[global_threadId];

	for (int i = 0, tile = 0; i < N; i += blockDim.x, tile++)
	{
		//store the position of a body in the shared memory. The shared memory is used repeatedly.
		int idx = tile * blockDim.x + threadIdx.x;
		sharedPosition[threadIdx.x] = global_pos[idx];
		__syncthreads();

		//calculate the acceleration of one body with all the bodies in the shared memory.
		accel = tile_calculation(Position, accel, sharedPosition);
		
		__syncthreads();
		
	}

	float4 accel4 = { accel.x, accel.y, accel.z, 0.0f };
	global_accel[global_threadId] = accel4;
}

//the kernel function for the evolution of the system.
__global__ void evolve(float4* pos, float4* velo, float4* accel, float step)
{
	int global_threadId = blockIdx.x * blockDim.x + threadIdx.x;
	auto velo_new = velo[global_threadId];
	velo_new.x += step * accel[global_threadId].x;
	velo_new.y += step * accel[global_threadId].y;
	velo_new.z += step * accel[global_threadId].z;
	velo[global_threadId] = velo_new;

	auto pos_new = pos[global_threadId];
	pos_new.x += step * velo_new.x;
	pos_new.y += step * velo_new.y;
	pos_new.z += step * velo_new.z;
	pos[global_threadId] = pos_new;
}

float4* generate_rand_float4(int size, float min_val, float max_val)
{
	std::default_random_engine e(time(0));
	std::uniform_real_distribution<float> u(min_val, max_val);
	float4* temp = (float4*)malloc(size * sizeof(float4));
	for (int i = 0; i < size; i++)
	{
		temp[i].x = u(e);
		temp[i].y = u(e);
		temp[i].z = u(e);
		temp[i].w = 1.0;
	}
	return temp;
}

float4* generate_two_cluster(int size1, int size2, float center_pos1, float center_pos2, int r1, int r2)
{
	std::default_random_engine e(time(0));
	std::uniform_real_distribution<float> u(center_pos1-r1,center_pos1+r1);
	float4* temp = (float4*)malloc((size1+size2) * sizeof(float4));
	for (int i = 0; i < size1; i++)
	{
		temp[i].x = u(e);
		temp[i].y = u(e);
		temp[i].z = u(e);
		temp[i].w = 1.0;
		if((temp[i].x-center_pos1)*(temp[i].x-center_pos1)+(temp[i].y-center_pos1)*(temp[i].y-center_pos1)+(temp[i].z-center_pos1)*(temp[i].z-center_pos1)>r1*r1)
		{
			i--;
		}
	}
	std::uniform_real_distribution<float> u2(center_pos2-r2,center_pos2+r2);
	for (int i = size1; i < size1+size2; i++)
	{
		temp[i].x = u2(e);
		temp[i].y = u2(e);
		temp[i].z = u2(e);
		temp[i].w = 1.0;
		if((temp[i].x-center_pos2)*(temp[i].x-center_pos2)+(temp[i].y-center_pos2)*(temp[i].y-center_pos2)+(temp[i].z-center_pos2)*(temp[i].z-center_pos2)>r2*r2)
		{
			i--;
		}
	}
	return temp;
}

int main()
{
    std::string filepath = "./nbody/output";
	float4* pos = generate_two_cluster(N-10000,10000,-400,300,200,300);
	float4* velo = generate_rand_float4(N, -1.0, 1.0);
	float4* dev_pos, * dev_velo, * dev_accel;

	cudaEvent_t start, stop;
    float elapsedTime;

    cudaEventCreate(&start);
    cudaEventCreate(&stop);
    cudaEventRecord(start, 0);

	cudaMalloc((void**)(&dev_pos), N * sizeof(float4));
	cudaMalloc((void**)(&dev_velo), N * sizeof(float4));
	cudaMalloc((void**)(&dev_accel), N * sizeof(float4));
	cudaMemcpy(dev_pos, pos, N * sizeof(float4), cudaMemcpyHostToDevice);
	cudaMemcpy(dev_velo, velo, N * sizeof(float4), cudaMemcpyHostToDevice);
	float step = 0.1;

	calculate_accel <<<(N + BLOCKSIZE - 1) / BLOCKSIZE, BLOCKSIZE >>> (dev_pos, dev_accel);
	evolve <<<(N + BLOCKSIZE - 1) / BLOCKSIZE, BLOCKSIZE >>> (dev_pos, dev_velo, dev_accel, step);
    cudaMemcpy(pos, dev_pos, N * sizeof(float4), cudaMemcpyDeviceToHost);

	for (int i = 0; i < 1000; i++)
	{
		calculate_accel <<<(N + BLOCKSIZE - 1) / BLOCKSIZE, BLOCKSIZE >>> (dev_pos, dev_accel);
		evolve <<<(N + BLOCKSIZE - 1) / BLOCKSIZE, BLOCKSIZE >>> (dev_pos, dev_velo, dev_accel, step);
        cudaMemcpy(pos, dev_pos, N * sizeof(float4), cudaMemcpyDeviceToHost);
        std::ofstream outfile(filepath + std::to_string(i));
		for (int j = 0; j < N; j++)
		{
			outfile << pos[j].x << " " << pos[j].y << " " << pos[j].z << std::endl;
		}
        outfile.close();
	}

	cudaEventRecord(stop, 0);
    cudaEventSynchronize(stop);
    cudaEventElapsedTime(&elapsedTime, start, stop);
    std::cout << elapsedTime << std::endl;
	return 0;
}
```

上面的程序在GTX 1650显卡上需要2-3分钟的时间执行完成。将输出的数据绘制成二维动画如下：

![test_animation](https://raw.githubusercontent.com/lyf35/blog_figures/main/img/20210610232644.gif)



# 参考

1. The CUDA Handbook
2. CUDA By Example
3. [共享内存 Bank冲突_Dezeming的博客-CSDN博客](https://blog.csdn.net/tiao_god/article/details/110854209)
4. [Chapter 39. Parallel Prefix Sum (Scan) with CUDA | NVIDIA Developer](https://developer.nvidia.com/gpugems/gpugems3/part-vi-gpu-computing/chapter-39-parallel-prefix-sum-scan-cuda)
5. [GPU_gems3_ch31.qxd (nvidia.cn)](https://developer.download.nvidia.cn/compute/cuda/1.1-Beta/x86_website/projects/nbody/doc/nbody_gems3_ch31.pdf)

