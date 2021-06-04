---
title: 深度学习-TensorRT模型部署
tags: 
- 深度学习
categories: [工程实践, 深度学习]
sharing: True
date: 2021-03-18 20:46:09
updated: 2021-05-09 23:29:07
---

# 概述

本文以部署目标检测模型YOLOv5为例，说明如何使用TensorRT C++ API部署训练好的神经网络模型，并进行推理。YOLOv5模型的输入为`(batch_size, channels, image_height, image_width)`，用于推理的模型输出为`(batch_size, image_height / 32 * image_width / 32 * 21, num_classes + 5)`。其中输出已经被转化为实际的像素值以及概率值，而不是像训练阶段一样为转换过的数值。

YOLOv5的GitHub repo：https://github.com/ultralytics/yolov5。
<!--more-->




# 生成ONNX文件

ONNX（Open Neural Network Exchange，开放神经网络交换）是一种针对于机器学习所设计的开放式的文件格式，方便神经网络结构及其参数的保存与解析。ONNX文件可以在TensorFlow、PyTorch、Caffe等深度学习框架中解析，也方便使用TensorRT进行部署。而常用的深度学习框架也提供了将训练好的模型保存为ONNX文件的函数。

下面为PyTorch中将模型转换为onnx文件的代码：

```python
# 注：以下代码中的model变量指的是要导出的神经网络模型，它的创建过程省略
# 首先需要定义一个哑变量，onnx文件所包含的神经网络结构就是根据这个哑变量在网络中的计算流程而生成的。
dummy_variables=torch.randn(3,3,320,320,dtype=torch.float16).to(device) 
# 导出onnx文件。需要注意的是，下面的input_names和output_names这两个变量可以不手动设置，函数会自动生成名称。如果要是手动设置的话，则需要与输入和输出的个数对应起来。如果输入/输出是一个单独的tensor，那么输入/输出的个数为1；如果是一个list/tuple这样的数据结构，那么输入/输出个数则指的是其中的元素个数
torch.onnx.export(model, #要导出的模型
                  dummy_variables, #哑变量
                  'output_onnx_model_batch.onnx', #导出onnx文件的名称
                  input_names=['input'], #网络的输入名称
                  output_names=['output'], #网络的输出名称
                  opset_version=12 #所支持运算集合的版本，通常高版本支持的运算也越多，但是也需要使用越新的PyTorch、onnx与TensorRT版本
                 )
# 注意：onnx文件支持动态尺寸输入，可以在export的时候传入dynamic_axes参数来指定在输入和输出的哪些维度上使用动态尺寸。dynamic_axes参数需要传入一个dict，其中key为输入/输出层的名称，value是一个包含动态尺寸所有维度的list。动态尺寸的onnx模型在TensorRT中解析时，相应维度上的尺寸会被解析为-1.

# TensorRT无法直接解析一些结构复杂的网络，必要的话需要借用onnx simplifier这个函数库对网络结构进行简化
onnx_model=onnx.load('output_onnx_model_batch.onnx')
onnx_model_simplified,check=simplify(onnx_model,input_shapes={'input':[3,3,320,320]})
onnx.save(onnx_model_simplified,'output_onnx_model_simplified_batch.onnx')

# 检查ONNX模型是否包含错误
onnx.checker.check_model(onnx_model_simplified)

# 可视化网络结构
import netron
netron.start('output_onnx_model_simplified_batch.onnx')
```



# TensorRT推理

TensorRT是NVIDIA开发的深度学习模型部署框架，它可以对训练好的模型进行优化，从而提高了模型的推理速度。网络结构可以使用TensorRT自己的API进行搭建，也可以通过其它文件格式如ONNX导入。TensorRT会将模型转化为一个engine，然后调用这个engine生成一个context进行模型推理。

首先我们需要先定义一些辅助函数：

```cpp
//以下为检查CUDA状态的辅助函数，可以用来判断与CUDA相关的函数调用是否正确
#ifndef CUDA_CHECK
#define CUDA_CHECK(callstr)\
    {\
        cudaError_t error_code = callstr;\
        if (error_code != cudaSuccess) {\
            std::cerr << "CUDA error " << error_code << " at " << __FILE__ << ":" << __LINE__;\
            assert(0);\
        }\
    }
#endif  // CUDA_CHECK
```

```cpp
//TensorRT logger的定义，用于记录错误信息
class Logger : public nvinfer1::ILogger
{
    void log(Severity severity, const char* msg) override
    {
        // suppress info-level messages
        if (severity != Severity::kINFO)
            std::cout << msg << std::endl;
    }
} gLogger;
```

## Engine生成

要解析onnx文件并生成一个engine，可以用下面的函数。需要注意的是，onnx文件中的尺寸分为静态和动态两种，需要使用不同的方法进行解析。

```cpp
//解析固定尺寸的onnx文件
nvinfer1::ICudaEngine* parseStaticOnnxModel(const std::string model_path, int batch_size = 1)
{
    TRTUniquePtr<nvinfer1::IBuilder> builder{ nvinfer1::createInferBuilder(gLogger) }; //builder类可以创建网络结构，以及生成一个engine
    TRTUniquePtr<nvinfer1::INetworkDefinition> network{ builder->createNetworkV2(1U << static_cast<uint32_t>(nvinfer1::NetworkDefinitionCreationFlag::kEXPLICIT_BATCH)) }; //定义一个网络
    TRTUniquePtr<nvonnxparser::IParser> parser{ nvonnxparser::createParser(*network,gLogger) }; //parser类可以解析onnx的文件内容，根据onnx文件内容定义网络结构

    if (!parser->parseFromFile(model_path.c_str(), static_cast<int>(nvinfer1::ILogger::Severity::kINFO))) //解析onnx文件
    {
        std::cerr << "Error: could not parse the model" << std::endl;
        return nullptr;
    }

    //设置engine的一些参数
    TRTUniquePtr<nvinfer1::IBuilderConfig> config{ builder->createBuilderConfig() };
    config->setMaxWorkspaceSize(1ULL << 31);
    builder->setMaxBatchSize(batch_size);
    if (builder->platformHasFastFp16()) //是否支持半精度浮点数，如果支持则开启该功能
    {
        config->setFlag(nvinfer1::BuilderFlag::kFP16);
    }

    auto engine = builder->buildEngineWithConfig(*network, *config); //生成engine
    return engine;
}
```

```c++
//解析动态尺寸的onnx文件
nvinfer1::ICudaEngine* parseDynamicOnnxModel(
    const std::string model_path, 
    const std::vector<std::string> dynamic_name,
    const std::vector<std::vector<std::vector<uint32_t>>> dynamic_sizes, 
    int max_batchsize = 1
)
{
    TRTUniquePtr<nvinfer1::IBuilder> builder{ nvinfer1::createInferBuilder(gLogger) }; //builder类可以创建网络结构，以及生成一个engine
    TRTUniquePtr<nvinfer1::INetworkDefinition> network{ builder->createNetworkV2(1U << static_cast<uint32_t>(nvinfer1::NetworkDefinitionCreationFlag::kEXPLICIT_BATCH)) }; //为了支持动态尺寸
    TRTUniquePtr<nvonnxparser::IParser> parser{ nvonnxparser::createParser(*network,gLogger) }; //parser类可以解析onnx的文件内容

    if (!parser->parseFromFile(model_path.c_str(), static_cast<int>(nvinfer1::ILogger::Severity::kINFO))) //解析onnx文件
    {
        std::cerr << "Error: could not parse the model" << std::endl;
        return nullptr;
    }

    //设置engine的一些参数
    builder->setMaxBatchSize(max_batchsize);
    TRTUniquePtr<nvinfer1::IBuilderConfig> config{ builder->createBuilderConfig() }; //config保存了模型的一些参数，在之后生成engine的时候使用
    config->setMaxWorkspaceSize(1UL << 31); //设置模型可使用的最大内存空间，此处相当于2GB
    if (builder->platformHasFastFp16()) //是否支持半精度浮点数，如果支持则开启该功能
    {
        config->setFlag(nvinfer1::BuilderFlag::kFP16);
    }

    //设置动态尺寸的大小
    nvinfer1::IOptimizationProfile* profile{ builder->createOptimizationProfile() };
    assert(dynamic_sizes.size() == dynamic_name.size()); 
    for (int s = 0; s < dynamic_sizes.size(); s++)
    {
        assert(dynamic_sizes[s].size() == 3);//对应于最小值、最优值和最大值
        auto min_shape = nvinfer1::Dims(); //创建三个tensor，保存动态尺寸
        auto opt_shape = nvinfer1::Dims();
        auto max_shape = nvinfer1::Dims();
        min_shape.nbDims = opt_shape.nbDims = max_shape.nbDims = dynamic_sizes[s][0].size();
        for (int i = 0; i < dynamic_sizes[s][0].size(); i++)
        {
            min_shape.d[i] = dynamic_sizes[s][0][i];
            opt_shape.d[i] = dynamic_sizes[s][1][i];
            max_shape.d[i] = dynamic_sizes[s][2][i];
        }

        profile->setDimensions(dynamic_name[s].c_str(), nvinfer1::OptProfileSelector::kMIN, min_shape);
        profile->setDimensions(dynamic_name[s].c_str(), nvinfer1::OptProfileSelector::kOPT, opt_shape);
        profile->setDimensions(dynamic_name[s].c_str(), nvinfer1::OptProfileSelector::kMAX, max_shape);

        config->addOptimizationProfile(profile); //将上述设置的动态尺寸保存到config中
    }

    auto engine = builder->buildEngineWithConfig(*network, *config); //生成engine
    return engine;
}
```

需要注意的一点是，使用TensorRT生成engine的过程很慢，因此最好将生成engine与调用engine的步骤分开来，否则debug的时候较慢。而且生成engine之后，最好将其转换为二进制数据保存下来，以方便之后的使用，这样就不需要每次都花费大量时间解析ONNX文件并生成engine。保存engine文件的函数如下：

```cpp
void saveEngine(nvinfer1::ICudaEngine* engine, const std::string file_name)
{
    TRTUniquePtr<nvinfer1::IHostMemory> model_stream{ engine->serialize() }; //生成engine序列化的结果
    std::ofstream serialized_output_stream(file_name, std::ios::binary | std::ios::out); //Refer to https://forums.developer.nvidia.com/t/trt-yolo-app/108645

    std::string serialized_strs;
    serialized_strs.resize(model_stream->size());
    memcpy((void*)serialized_strs.data(), model_stream->data(), model_stream->size()); //将序列化之后的结果复制到字符串中去，方便写入文件
    
    serialized_output_stream << serialized_strs; //将序列化的engine写入文件中
    serialized_output_stream.close();
}

```

读取序列化engine文件的函数如下：

```cpp
nvinfer1::ICudaEngine* loadSerializedEngine(const std::string file_name)
{
    std::ifstream filestream(file_name, std::ios::binary | std::ios::in); //使用序列化的engine文件生成文件输入流
    std::string serialized_engine = "";
    while (filestream.peek() != EOF) //用字符串保存序列化的engine
    {
        std::stringstream buffer;
        buffer << filestream.rdbuf();
        serialized_engine.append(buffer.str());
    }
    filestream.close();
    auto runtime = nvinfer1::createInferRuntime(gLogger);
    auto engine = runtime->deserializeCudaEngine(serialized_engine.data(), serialized_engine.size(), nullptr); //将序列化的engine反序列化
    runtime->destroy();
    return engine;
}
```

对于构造完成的engine，我们可以使用如下命令查看模型输入和输出的总个数：

```cpp
engine->getNbBindings() //返回模型的输入和输出总个数，它的值为onnx文件中input_names和output_names中元素的总个数
```

同时我们可以使用如下命令查看输入与输出的尺寸信息，如果是固定尺寸则为一个正整数，如果是动态尺寸则为-1：

```cpp
engine->getBindingDimensions(0) //返回一个nvinfer1::Dims类型的变量，保存了尺寸信息
```

## 模型推理

在进行模型推理之前，首先需要创建一个context：

```cpp
context = TRTUniquePtr<nvinfer1::IExecutionContext>(engine->createExecutionContext()); //从engine中构造context
context->setOptimizationProfile(0);
```

同时创建一个CUDA stream：

```cpp
cudaStream_t stream;
CUDA_CHECK(cudaStreamCreate(&stream));
```

以YOLOv5模型为例，其推理过程如下：

```cpp
//在YOLOv5Image类中，它的frame属性保存的是cv::Mat类型的图片数据
void YOLOv5::sequentialInference(std::vector<YOLOv5Image>& images, cudaStream_t stream)
{
    std::vector<void*> buffers(2); //GPU显存的缓冲区，此处vector的大小应该与engine->getNbBindings()的返回值相等
    size_t input_size = sizeof(short) * fig_size * fig_size * channels; //一幅输入图片所占的空间，图片的一个像素与short类型的字节数相等
    size_t output_size_base = sizeof(short) * (classtypes.size() + 5) * 3 * (fig_size / 32) * (fig_size / 32); //计算最小输出所占空间，其余两个可以通过*4和*16计算，输出为半精度浮点数，与short类型字节数相等

    //为输入和输出分配GPU内存，buffers的每一个元素对应于一个输入/输出，它们都需要根据相应的尺寸分配内存
    //注意cudaMalloc的用法，函数的第一个参数为二重指针，其中第一层指针保存的是GPU内存的地址，而第二层指针对应的是内层指针的地址。在传入参数的时候，传入的是一个指针变量的地址，而最终分配的GPU内存块的起始地址会被保存在这个指针变量中，在后续使用的时候需要注意这一点。
    CUDA_CHECK(cudaMalloc(&buffers[0], input_size)); 
    CUDA_CHECK(cudaMalloc(&buffers[1], 21 * output_size_base)); 
    
    for (int i = 0; i < images.size(); i++)
    {
        //要特别注意OpenCV读取图片时，三个颜色通道是按照BGR的顺序排列的，而且在内存中是按照每个像素点的三个通道依次存储。而YOLOv5模型要求的颜色通道顺序为RGB，且数据的存储是按照每个通道的像素点顺序排列。
        //因此，首先我们要对原始图片进行通道分离，并将其转换为float16的数据格式，然后按照RGB通道顺序将图片信息进行重组。
        cv::Mat temp_mat;
        images[i].frame.convertTo(temp_mat, CV_16F, 1.0 / 255.0);
        std::vector<cv::Mat> channel_seperated_img(3);
        cv::split(temp_mat, channel_seperated_img);
        char* data = (char*)malloc(input_size);

        for (int j = 2; j >= 0; j--)
        {
            memcpy((void*)(data + (2 - j) * (input_size / 3)), (void*)(channel_seperated_img[j].datastart), input_size / 3);//注意void指针在加减运算的时候，其地址加减的为实际值，而不是像int*一样以元素大小为单位。
        }
        
        CUDA_CHECK(cudaMemcpyAsync(buffers[0], data, input_size, cudaMemcpyHostToDevice, stream)); //将输入从内存复制到显存。
        free(data); //可以释放掉之前重组图片时使用的内存

        context->enqueueV2((void**)buffers.data(), stream, nullptr); //将数据送入context，进行推理
		//获取推理结果
        half* temp;
        temp = (half*)malloc(output_size_base * 21);
        CUDA_CHECK(cudaMemcpyAsync((void*)temp, buffers[1], output_size_base * 21, cudaMemcpyDeviceToHost, stream)); //将输出从显存复制到内存
        images[i].inference_result = temp;

        cudaStreamSynchronize(stream);
    }
    
    CUDA_CHECK(cudaFree(buffers[0])); //推理完成之后释放掉显存
    CUDA_CHECK(cudaFree(buffers[1]));
}
```

需要注意的是，如果使用动态尺寸模型进行推理，那么在推理之前，需要使用`context.setBindingDimensions()`函数指定动态尺寸的具体数值。



# 附：开发环境配置

无论要部署什么神经网络模型，需要提前安装与配置好的C++开发环境包括CUDA和TensorRT函数库。由于YOLOv5属于计算机视觉相关的任务，因此还需要安装OpenCV函数库，并且重新编译为支持CUDA的版本。

OpenCV的重新编译过程如下（这一过程中会遇到很多坑，下面也将说明一些坑及其解决办法）：

1. 下载cmake软件以及OpenCV的扩展模块opencv_contrib
2. 打开cmake，将source code的路径设置为OpenCV目录下的sources文件夹，并设置编译完成后的文件放在哪个文件夹下（最好放在一个空白文件夹里）
3. 然后点击configure，在弹出的窗口中设置平台为x64，然后点击finish，便会自动在设置的文件夹下面生成工程。此时会出现许多编译选项，此时需要勾选其中所有的CUDA选项（**除了BUILD_CUDA_STUBS，因为选中这一项之后会导致编译后的代码只有CUDA的空壳子**），并且需要将opencv_contrib/modules的路径添加到OPENCV_EXTRA_MODULES_PATH处。同时，最好把BUILD_opencv_world选项选中，这样可以将所有的库编译到一起，方便使用。至于其它选项，按照自己的需要进行修改。
4. 在编译选项设置完成之后，再点击configure。在这一步中，可能会因为网络问题导致需要从网上下载的一些库无法下载到，此时需要从github的opencv_3rdparty仓库内手动下载，下载完成之后需要在对应的Cmake文件中，将下载路径设置为本地的file://……。在手动下载的时候，需要注意版本的问题，否则会导致文件的哈希码无法对应，这同样会导致失败。可以参考https://blog.csdn.net/fzp95/article/details/109276633。在这一步骤中，可能也会因为其它原因导致失败，有些是因为勾选了一些编译选项，导致需要额外编译一些内容，这些则需要根据自己的需求进行修改调整。
5. 在configure成功之后，点击generate，如果成功便完成了Cmake的部分
6. 打开第2步设置的工程文件存放路径，然后使用Visual Studio打开工程文件，切换到Release模式下，右键点击ALL_BUILD选择【生成….】。运行完成之后，再次右键选择install，选择【生成…】。这一过程需要花费若干小时的时间。

接下来是Windows平台下面的环境部署流程：

1. 在操作系统的环境变量中添加相应的路径，主要是\*/bin/文件夹和\*/lib/文件夹的路径，从而使得系统可以寻找到这些函数库中可执行文件以及链接库的路径。
2. 在Visual Studio中，需要在Project->Properties中设置如下参数（配置时记得选x64，以及根据需要选择Debug还是Release模式）：

   - C/C++->General->Additional Include Directories：添加这些函数库对应文件路径下的\*/include/文件夹
   - Linker->General->Additional Library Directories：添加这些函数库对应文件路径下的\*/lib/文件夹
   - Linker->Input->Additional Dependencies：添加.lib函数库。以YOLOv5模型为例，需要添加如下这些：cudart.lib;cublas.lib;cudnn.lib;cudnn64_8.lib;myelin64_1.lib;nvinfer.lib;nvinfer_plugin.lib;nvonnxparser.lib;nvparsers.lib;opencv_world451d.lib（注意OpenCV的Debug与Release需要使用不同的.lib文件，Release使用的是文件名不带后缀d的那个）



# 参考

1. TensorRT的简介：https://www.cnblogs.com/qccz123456/p/11767858.html
2. TensorRT的C++ API简介（中文版）：https://blog.csdn.net/u010552731/article/details/89501819，https://blog.csdn.net/yangjf91/article/details/97912773
3. Nvidia官方的C++ API文档：https://docs.nvidia.com/deeplearning/tensorrt/developer-guide/index.html#c_topics
4. 读取ONNX模型并做模型推理的程序示例（其中一些代码可能需要做一些小修改）：https://learnopencv.com/how-to-run-inference-using-tensorrt-c-api/，https://www.edge-ai-vision.com/2020/04/speeding-up-deep-learning-inference-using-tensorrt/
5. TensorRT运行YOLO v4的示例：https://github.com/linghu8812/tensorrt_inference/tree/master/Yolov4
6. Nvidia开发者博客，TensorRT的基础教程：https://developer.nvidia.com/blog/speed-up-inference-tensorrt/
7. ONNX的使用：https://www.jianshu.com/p/65cfb475584a
8. ONNX的官方仓库：https://github.com/onnx/onnx
9. PyTorch模型转换为ONNX格式：https://pytorch.org/docs/stable/onnx.html
10. Visual studio中，TensorRT的环境配置：https://www.pianshen.com/article/81911746264/
11. opencv的环境配置：https://blog.csdn.net/qq_42517195/article/details/80797328，https://jingyan.baidu.com/article/73c3ce285feb20e50343d9ff.html
12. opencv带有cuda的重新编译：https://blog.csdn.net/qq_30623591/article/details/82084113，https://blog.csdn.net/yangshengwei230612/article/details/108987333，https://blog.csdn.net/wolffytom/article/details/49976487，https://blog.csdn.net/fengxinzioo/article/details/109402921
14. 解决下载失败的问题：https://blog.csdn.net/fzp95/article/details/109276633，https://blog.csdn.net/u011028345/article/details/74568109
15. Linux环境下的配置：https://blog.csdn.net/u013230291/article/details/104233668

