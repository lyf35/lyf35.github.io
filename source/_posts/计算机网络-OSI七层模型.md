---
title: 计算机网络-OSI七层模型
categories: [理论知识, 计算机网络]
tags: 
- 计算机网络
- 计算机基础
sharing: True
date: 2020-11-27 20:27:19
updated: 2021-01-06 22:12:40
---

# OSI七层模型

OSI（Open System Interconnect），即开放式系统互联。 一般都叫OSI参考模型，是ISO（国际标准化组织）组织在1985年研究的网络互连模型。ISO为了更好的使网络应用更为普及，推出了OSI参考模型。其含义就是推荐所有公司使用这个规范来控制网络。这样所有公司都有相同的规范。

OSI模型把网络从逻辑上分为七层，每一层都有相应的物理设备。它是一种框架式的设计方法，最主要的功能就是帮助不同类型的主机实现数据传输； 最大优点就是将服务、接口和协议三个概念明确的区分起来。但是这一模型复杂且不实用；在实际中，经常使用的是TCP/IP五层（或四层，将物理层与数据链路层看作同一层）模型。
<!-- more -->

![img](https://raw.githubusercontent.com/lyf35/blog_figures/main/img/20201104232856.png)

OSI七层模型的示意图

为了使数据分组从源传送到目的地，源端OSI模型的每一层都必须与目的端的对等层进行通信，这种通信方式称为对等层通信。在每一层通信过程中，使用本层自己协议进行通信。



# TCP/IP五层模型

在TCP/IP五层模型中，每一层都呼叫它的下一层提供的接口来完成自己的需求。（如果是四层模型数据链路层和物理层在一层） 在每一层实现的协议都不同，即每一层的服务不同。同时，每一层为了实现不同的服务，也工作着不同的设备。

1. 物理层：负责光电信号传递方式。集线器工作在物理层。
2. 数据链路层：负责设备之间的数据帧的传输和识别，例如网卡设备的驱动，帧同步，冲突检测，数据差错校验等工作。 交换机工作在数据链路层。
3.  网络层：负责地址管理和路由选择。路由器工作在网络层。 
4.  传输层：负责两台主机之间的数据传输。 
5.  应用层：负责应用程序之间的沟通。网络编程主要针对的就是应用层。

应用层的封装在应用程序中完成；传输层和网络层的封装在操作系统完成；数据链路层和物理层的封装在设备驱动程序与网络接口中完成。

TCP/IP五层模型与OSI七层模型的对应关系如下：

![img](https://raw.githubusercontent.com/lyf35/blog_figures/main/img/20201104232906.png)



# 数据传送

在信息的传递过程中，为了便于数据传输，从应用层向下的每一层都会为数据加一个报头，将其封装成包。在到达设备或是终端的时候，再层层拆包。其大致过程如下图所示

![img](https://raw.githubusercontent.com/lyf35/blog_figures/main/img/20201104232912.png)



# 参考

1. https://www.cnblogs.com/qishui/p/5428938.html
2. https://blog.csdn.net/qq_22238021/article/details/80279001?utm_medium=distribute.pc_relevant.none-task-blog-BlogCommendFromMachineLearnPai2-1.add_param_isCf&depth_1-utm_source=distribute.pc_relevant.none-task-blog-BlogCommendFromMachineLearnPai2-1.add_param_isCf
3. https://blog.csdn.net/qq_39326472/article/details/88089747
4. Computer Networking: A Top-down Approach