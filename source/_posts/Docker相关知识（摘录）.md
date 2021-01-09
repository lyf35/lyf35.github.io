---
title: Docker相关知识（摘录）
tags: 
- Docker
categories: [Docker, 摘录]
sharing: True

---

注：本文内容是从参考部分摘录而来，如有侵权请及时告知！



# 容器技术

## 从环境配置开始说起

软件开发最大的麻烦事之一，就是环境配置。用户必须保证两件事：操作系统的设置，各种库和组件的安装。只有它们都正确，软件才能运行。举例来说，安装一个 Python 应用，计算机必须有 Python 引擎，还必须有各种依赖，可能还要配置环境变量。

如果某些老旧的模块与当前环境不兼容，那就麻烦了。环境配置如此麻烦，换一台机器，就要重来一次，旷日费时。很多人想到，能不能从根本上解决问题，软件可以带环境安装？也就是说，安装的时候，把原始环境一模一样地复制过来。

<!--more-->

## 虚拟机技术

虚拟机（virtual machine）就是带环境安装的一种解决方案。它可以在一种操作系统里面运行另一种操作系统，比如在 Windows  系统里面运行 Linux  系统。应用程序对此毫无感知，因为虚拟机看上去跟真实系统一模一样，而对于底层系统来说，虚拟机就是一个普通文件，不需要了就删掉，对其他部分毫无影响。

虽然用户可以通过虚拟机还原软件的原始环境。但是，这个方案有几个缺点：

1. 资源占用多：虚拟机会独占一部分内存和硬盘空间。它运行的时候，其他程序就不能使用这些资源了。哪怕虚拟机里面的应用程序，真正使用的内存只有 1MB，虚拟机依然需要几百 MB 的内存才能运行。

2. 冗余步骤多：虚拟机是完整的操作系统，一些系统级别的操作步骤，往往无法跳过，比如用户登录。

3. 启动慢：启动操作系统需要多久，启动虚拟机就需要多久。可能要等几分钟，应用程序才能真正运行。

## 容器技术

由于虚拟机存在这些缺点，Linux 发展出了另一种虚拟化技术：Linux 容器（Linux Containers，缩写为 LXC）。

**Linux 容器不是模拟一个完整的操作系统，而是对进程进行隔离。**或者说，在正常进程的外面套了一个[保护层](https://opensource.com/article/18/1/history-low-level-container-runtimes)。对于容器里面的进程来说，它接触到的各种资源都是虚拟的，从而实现与底层系统的隔离。

由于容器是进程级别的，相比虚拟机有很多优势：

1. 启动快：容器里面的应用，直接就是底层系统的一个进程，而不是虚拟机内部的进程。所以，启动容器相当于启动本机的一个进程，而不是启动一个操作系统，速度就快很多。

2. 资源占用少：容器只占用需要的资源，不占用那些没有用到的资源；虚拟机由于是完整的操作系统，不可避免要占用所有资源。另外，多个容器可以共享资源，虚拟机都是独享资源。

3. 体积小：容器只要包含用到的组件即可，而虚拟机是整个操作系统的打包，所以容器文件比虚拟机文件要小很多。

总之，容器有点像轻量级的虚拟机，能够提供虚拟化的环境，但是成本开销小得多。



# Docker

## 概述

**Docker 属于 Linux 容器的一种封装，提供简单易用的容器使用接口。**它是目前最流行的 Linux 容器解决方案。

Docker 将应用程序与该程序的依赖，打包在一个文件里面。运行这个文件，就会生成一个虚拟容器。程序在这个虚拟容器里运行，就好像在真实的物理机上运行一样。有了 Docker，就不用担心环境问题。

总体来说，Docker 的接口相当简单，用户可以方便地创建和使用容器，把自己的应用放入容器。容器还可以进行版本管理、复制、分享、修改，就像管理普通的代码一样。

Docker 的主要用途，目前有三大类。

1. 提供一次性的环境。比如，本地测试他人的软件、持续集成的时候提供单元测试和构建的环境。
2. 提供弹性的云服务。因为 Docker 容器可以随开随关，很适合动态扩容和缩容。
3. 组建微服务架构。通过多个容器，一台机器可以跑多个服务，因此在本机就可以模拟出微服务架构。

## Docker涉及到的三个概念

1. 镜像（Image）：类似于虚拟机中的镜像，是一个包含有文件系统的面向Docker引擎的只读模板。任何应用程序运行都需要环境，而镜像就是用来提供这种运行环境的。例如一个Ubuntu镜像就是一个包含Ubuntu操作系统环境的模板，同理在该镜像上装上Apache软件，就可以称为Apache镜像。
2. 容器（Container）：类似于一个轻量级的沙盒，可以将其看作一个极简的Linux系统环境（包括root权限、进程空间、用户空间和网络空间等），以及运行在其中的应用程序。Docker引擎利用容器来运行、隔离各个应用。容器是镜像创建的应用实例，可以创建、启动、停止、删除容器，各个容器之间是是相互隔离的，互不影响。注意：镜像本身是只读的，容器从镜像启动时，Docker在镜像的上层创建一个可写层，镜像本身不变。
3. 仓库（Repository）：类似于代码仓库，这里是镜像仓库，是Docker用来集中存放镜像文件的地方。注意与注册服务器（Registry）的区别：注册服务器是存放仓库的地方，一般会有多个仓库；而仓库是存放镜像的地方，一般每个仓库存放一类镜像，每个镜像利用tag进行区分，比如Ubuntu仓库存放有多个版本（12.04、14.04等）的Ubuntu镜像。

## 底层原理

Docker容器与虚拟机的底层原理如下图所示:

![v2-0327d6666507f8716c3d3630fa4cb671_720w](https://raw.githubusercontent.com/lyf35/blog_figures/main/img/20210105222310.jpg)

容器时在Linux上本机运行，并与其他容器共享主机的内核，它运行的一个独立的进程，不占用其他任何可执行文件的内存，非常轻量。而虚拟机运行的是一个完成的操作系统，通过虚拟机管理程序对主机资源进行虚拟访问，相比之下需要的资源更多。

docker基于Linux内核，提供了如下几项功能实现：

- **NameSpace**
  我们知道Linux中的PID、IPC、网络等资源是全局的，而NameSpace机制是一种资源隔离方案，在该机制下这些资源就不再是全局的了，而是属于某个特定的NameSpace，各个NameSpace下的资源互不干扰，这就使得每个NameSpace看上去就像一个独立的操作系统一样，但是只有NameSpace是不够。
- **Control groups**
  虽然有了NameSpace技术可以实现资源隔离，但进程还是可以不受控的访问系统资源，比如CPU、内存、磁盘、网络等，为了控制容器中进程对资源的访问，Docker采用control  groups技术(也就是cgroup)，有了cgroup就可以控制容器中进程对系统资源的消耗了，比如你可以限制某个容器使用内存的上限、可以在哪些CPU上运行等等。

## 工作模式

Docker使用了常见的CS架构，也就是client-server模式，Docker client负责处理用户输入的各种命令，比如docker  build、docker run，真正工作的其实是server，也就是Docker daemon，值得注意的是，Docker  client和Docker daemon可以运行在同一台机器上。

### docker build

当我们写完dockerfile交给docker“编译”时使用这个命令，client在接收到请求后转发给docker daemon，接着docker daemon根据dockerfile创建出镜像image。

![v2-f16577a98471b4c4b5b1af1036882caa_720w](https://raw.githubusercontent.com/lyf35/blog_figures/main/img/20210105222337.jpg)

### docker run

有了镜像image后就可以运行程序了，接下来使用命令docker run，docker daemon接收到该命令后找到具体的image，然后加载到内存开始执行，此时便生成一个所谓的容器。

![v2-672b29e2d53d2ab044269b026c6bc473_720w](https://raw.githubusercontent.com/lyf35/blog_figures/main/img/20210105222352.jpg)

### docker pull

Docker registry 可以用来存放各种image，公共的可以供任何人下载image的仓库就是Docker Hub。那么该怎么从Docker Hub中下载image呢，就是这里的docker pull命令了。

因此，这个命令的实现也很简单，那就是用户通过docker client发送命令，docker daemon接收到命令后向docker registry发送image下载请求，下载后存放在本地，这样我们就可以使用image了。

![v2-dac570abcf7e1776cc266a60c4b19e5e_720w](https://raw.githubusercontent.com/lyf35/blog_figures/main/img/20210105222439.jpg)



# Docker的使用

## 安装

Docker官方提供了自动安装的脚本，可以在机器上自动安装docker。只需要执行如下命令即可：`curl -fsSL https://get.docker.com | bash -s docker --mirror Aliyun`。手动安装的步骤相比于自动安装脚本要繁琐地多，可以参照网页https://www.runoob.com/docker/ubuntu-docker-install.html提供的安装教程。

在安装完成之后，执行`systemctl start docker`便可以启动Docker服务，也可以同时使用enable将docker服务设置为开机自启动，方便以后的使用。

## 镜像的使用

### 生成镜像

镜像的生成有如下几种方式：

1. 从Docker Hub中获取，例如从Docker Hub中获取Fedora的镜像：`docker pull fedora`。这一命令会自动从Docker Hub中查找并下载相应的镜像。
2. 自己写Dockerfile，然后根据Dockerfile，使用`docker build`命令生成镜像。可以参考https://www.runoob.com/docker/docker-dockerfile.html
3. 将一个容器打包成镜像：`docker commit [container-ID] [Image-name]`。可以使用`docker save -o [compressed-file-name] [Image-name]`命令，将一个镜像转为压缩文件，从而方便在其他主机上部署。这一压缩文件在其他主机上可以用`docker load < [compressed-file-name] `命令，导入到镜像文件库中。

### 镜像管理

- 列出镜像列表：`docker images`
- 查找镜像：`docker search [image-name]`
- 删除镜像：`docker rmi [image-name]`
- 设置镜像标签：`docker tag [image-id] [image-name] `

## 容器的使用

### 基本操作

- 启动容器：`docker run [options] [image-name] [commands]`，例如`docker run -it ubuntu /bin/bash`代表进入一个ubuntu镜像的终端交互界面，要退出可以直接输入`exit`即可。启动过程中使用到的不同参数可以用`--help`进行查询。常用的参数包括：
  - `-i`：允许对容器的标准输入进行交互
  - `-t`：在新容器内指定一个终端/伪终端
  - `-d`：将容器放在后台运行
- 启动停止状态的容器：先用`docker ps -a`查看所有的容器，对处于exit状态的容器可以用`docker start [container-id]`来重新启动它
- 停止一个容器：`docker stop [container-id]`，停止的容器可以用`docker restart [container-id]`重启
- 查看容器输出：`docker logs [container-id]`
- 进入容器：如果在容器启动的时候使用了`-d`参数，那么启动之后会进入后台。此时如果要进入容器，有两种方式：
  - `docker attach`：这一命令在退出容器终端之后，会导致容器停止
  - `docker exec`：这一命令在退出终端之后，不会导致容器停止
- 删除容器：`docker rm -f [container-id]`，如果要清除所有处于终止状态的容器可以使用`docker container prune`命令
- 导出容器：`docker export [container-id] > [filename]`，导出的文件名一般为`*.tar`压缩文件
- 将容器快照导入为镜像：`docker import [URL or filename] [image-name]`

### 容器状态

- created（已创建）
- restarting（重启中）
- running 或 Up（运行中）
- removing（迁移中）
- paused（暂停）
- exited（停止）
- dead（死亡）

### 网络连接

#### 端口映射

在创建docker容器的时候，可以加入`-P`或者`-p`参数来指定容器绑定的网络地址。使用`-P`参数时，容器内部端口随机映射到主机的高端口；而使用`-p`参数时，需要在这一参数后面加上主机端口与容器端口的映射关系，例如`5000:5000`、`127.0.0.1:5001:5000`等，将容器内部的端口绑定到指定的主机端口。默认绑定的是TCP端口，如果要绑定UDP端口需要在端口后面加上`/udp`。

使用`docker port`命令可以快捷地查看端口的绑定情况。

#### 连接系统

docker 有一个连接系统允许将多个容器连接在一起，共享连接信息。使用`docker network create -d [network-type] [name]`命令可以创建一个docker网络，网络类型可以为bridge或者overlay。

在这之后，如果要将一个容器连接到新创建的网络系统上，可以在创建容器的时候加入`--network [name]`这一额外参数。



# 参考

1. https://zhuanlan.zhihu.com/p/23599229
2. http://www.ruanyifeng.com/blog/2018/02/docker-tutorial.html
3. https://zhuanlan.zhihu.com/p/187505981
4. https://www.runoob.com/docker/docker-tutorial.html



