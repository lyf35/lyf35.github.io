---
title: OpenSSL-BIO（摘录）
tags: 
- 信息安全
- OpenSSL
categories: [信息安全, OpenSSL, 摘录]
sharing: True

---

注：本文内容摘录自参考部分的相关资料



# 简介

OpenSSL抽象I/O(Basic I/O abstraction，即BIO)是OpenSSL对于I/O类型的抽象封装，包括内存、文件、日志、标准输入输出、socket、加解密、摘要和ssl通道等。OpenSSL BIO通过回调函数为用户隐藏了底层的实现细节，所有类型的BIO调用大体类似。BIO中的数据能从一个BIO传递给另一个BIO或是应用程序。

BIO的详细介绍参见[2]

<!--more-->

# BIO结构体

BIO的结构体的定义如下：

```c++
typedef struct bio_st BIO;
struct bio_st
{
	BIO_METHOD *method; //BIO方法结构，是决定BIO类型和行为的重要参数，各种BIO的不同之处主要也正在于此项。
	/* bio, mode, argp, argi, argl, ret */
	long (*callback)(struct bio_st *,int,const char *,int, long, long);
	char *cb_arg; /* first argument for the callback */
	int init; //具体句柄初始化标记，初始化之后为1
	int shutdown; //BIO关闭标记，当该值不为0时释放资源，数值修改可以通过控制函数来设置
	int flags; //用于控制各个函数的行为
	int retry_reason; //重试原因，主要用在socket和ssl BIO的异步阻塞
	int num; //因具体BIO而异
	void *ptr; //具体bio有不同含义。比如文件BIO中它用来存放文件句柄；mem bio中它用来存放内存地址；connect bio中它用来存放BIO_CONNECT数据，accept bio中它用来存放BIO_ACCEPT数据。
	struct bio_st *next_bio;	/*下一个BIO地址，used by filter BIOs*/
	struct bio_st *prev_bio;	/*上一个BIO地址，used by filter BIOs*/
	int references; //被引用数量
	unsigned long num_read; //已读入字节数
	unsigned long num_write; //已写入字节数
	CRYPTO_EX_DATA ex_data; //存放额外数据
}；
```

在BIO的成员中，method是最关键的一个，它决定了BIO的类型。在定义一个新的BIO结构时，使用下面的函数：

```C++
 BIO* BIO_new(BIO_METHOD *type);
```

BIO_new函数除了给一些初始变量赋值之外，主要就是把type中的各个变量赋值给BIO结构中的method成员。一般type参数是通过一个返回值为BIO_METHOD类型的函数来提供的，这样的函数有：

```c++
// 【source/sink型】
BIO_METHOD* BIO_s_accept()   //一个封装了类似TCP/IP socket Accept规则的接口，并且使TCP/IP操作对于BIO接口透明。
BIO_METHOD* BIO_s_connect()  //一个封装了类似TCP/IP socket Connect规则的接口，并且使TCP/IP操作对于BIO接口透明
BIO_METHOD* BIO_s_socket()   //封装了socket接口的BIO类型
BIO_METHOD* BIO_s_bio()      //封装了一个BIO对，数据从其中一个BIO写入，从另外一个BIO读出
BIO_METHOD* BIO_s_fd()       //是一个封装了文件描述符的BIO接口，提供类似文件读写操作的功能
BIO_METHOD* BIO_s_file()     //封装了标准的文件接口的BIO，包括标准的输入输出设备如stdin等
BIO_METHOD* BIO_s_mem()      //封装了内存操作的BIO接口，包括了对内存的读写操作
BIO_METHOD* BIO_s_null()     //返回空的sink型BIO接口，写入这种接口的所有数据读被丢弃，读的时候总是返回EOF

//【filter型】
BIO_METHOD* BIO_f_base64()   //封装了base64编码方法的BIO,写的时候进行编码，读的时候解码 
BIO_METHOD* BIO_f_cipher()   //封装了加解密方法的BIO，写的时候加密，读的时候解密
BIO_METHOD* BIO_f_md()       //封装了信息摘要方法的BIO，通过该接口读写的数据都是已经经过摘要的。
BIO_METHOD* BIO_f_ssl()      //封装了openssl 的SSL协议的BIO类型，也就是为SSL协议增加了一些BIO操作方法。
BIO_METHOD* BIO_f_null()     //一个不作任何事情的BIO，对它的操作都简单传到下一个BIO去了，相当于不存在。
BIO_METHOD* BIO_f_buffer()   //封装了缓冲区操作的BIO，写入该接口的数据一般是准备传入下一个BIO接口的，从该接口读出的数据一般也是从另一个BIO传过来的。
```

# BIO_METHOD

一个BIO_METHOD类型的结构体包含如下这些成员：

```c++
typedef struct bio_method_st
{
	int type; //BIO类型，type取值对应的BIO_METHOD类型见附录
	const char *name; //BIO的名字
	int (*bwrite)(BIO *, const char *, int); //写操作回调函数
	int (*bread)(BIO *, char *, int); //读操作回调函数
	int (*bputs)(BIO *, const char *); //写入字符串回调函数
	int (*bgets)(BIO *, char *, int); //读取字符串函数
	long (*ctrl)(BIO *, int, long, void *); //控制回调函数
	int (*)(BIO *); //生成回调函数
	int (*destroy)(BIO *); //销毁回调函数
    long (*callback_ctrl)(BIO *, int, bio_info_cb *); //控制回调函数，可以由调用者实现
} BIO_METHOD;
```

# BIO的相关函数

## BIO的声明与释放

下面是用于BIO分配和释放操作的一些基本操作系列函数：

```C++
#include<openssl/bio.h>

BIO*  BIO_new(BIO_METHOD *type); //调用BIO_set()函数创建一个新的BIO
int   BIO_set(BIO *a,BIO_METHOD *type); //对一个已经创建好的BIO设置新的BIO类型，其实就是简单的对BIO的各个成员进行初始化，并将参数type赋值给该BIO
int   BIO_free(BIO *a); //释放单个BIO占用的资源，成功返回1，失败返回0
void  BIO_vfree(BIO *a); //功能与BIO_free完全相同，但是无返回值
void  BIO_free_all(BIO *a); //释放一个BIO链，如果中间某个过程出错也不会停止释放过程
```

一些类型的BIO使用BIO_new()函数创建之后便可以直接使用，如memory类型的BIO；有些BIO在创建之后还需要做一些初始化工作，如文件BIO，一般也会提供一些这样的函数来创建和初始化它们。

## BIO的控制函数

BIO的控制函数较多，见附录。这些函数用于控制BIO的状态等。

## BIO的I/O操作

BIO的基本读写函数有四个：

```c++
#include <openss/bio.h>
//以下四个函数，成功返回真正读出/写入的数据长度，失败返回0或-1，BIO未实现这一函数返回-2
int BIO_read(BIO *b, void *buf, int len); //从BIO接口中读取len字节数据到buf，
int BIO_write(BIO *b, const void *buf, int len); //往BIO写入长度为len的数据
int BIO_gets(BIO *b,char *buf, int size); //从BIO中读取一行长度最大为size的数据
int BIO_puts(BIO *b,const char *buf); //往BIO中写入一个以NULL为结束符的字符串
```

BIO也提供了一组出错原因的诊断函数，用于诊断为什么BIO在读写数据的时候不能读写数据：

```c++
#define BIO_should_read(a)       ((a)->flags & BIO_FLAGS_READ) //如果导致IO失败的原因是BIO此时要读取数据，则返回true
#define BIO_should_write(a)      ((a)->flags & BIO_FLAGS_WRITE) //如果导致IO失败的原因是BIO此时要写数据，则返回true
#define BIO_should_io_special(a) ((a)->flags & BIO_FLAGS_IO_SPECIAL) //如果导致IO失败的原因是读写之外的原因则返回true
#define BIO_retry_type(a)        ((a)->flags & BIO_FLAGS_RWS) 
#define BIO_should_retry(a)      ((a)->flags & BIO_FLAGS_SHOULD_RETRY) //如果读写出错的情况是要求程序稍后重试，那么该函数返回true

#define BIO_FLAGS_READ 0x01
#define BIO_FLAGS_WRITE 0x02
#define BIO_FLAGS_IO_SPECIAL 0x04
#define BIO_FLAGS_RWS (BIO_FLAGS_READ|BIO_FLAGS_WRITE|BIO_FLAGS_IO_SPECIAL)
#define BIO_FLAGS_SHOULD_RETRY 0x08

BIO*    BIO_get_retry_BIO(BIO *bio, int *reason); //给出特殊情况错误的简短原因
int     BIO_get_retry_reason(BIO *bio); //返回失败的原因
```

## BIO对

BIO对是BIO中专门创建的一对缓存BIO，创建BIO对的函数如下：

```c++
#include<openssl/bio.h>
int BIO_new_bio_pair(BIO **bio1, size_t writebuf1, BIO **bio2, size_t writebuf2);
```

函数调用成功之后返回1，此时bio1和bio2都变得有效；否则返回0，bio1和bio2被设为NULL。但是注意它们在不使用的时候必须分别释放，因为有些SSL函数会隐含调用BIO_free函数。

当BIO对创建之后，它的两端都能作为数据缓冲的输入和输出，典型的应用为它一端和SSL的IO连接，另一端被应用控制。

这两个BIO对的功能是完全对称的，它们的缓冲区大小由参数writebuf1和writebuf2决定，如果设为0则使用默认大小。注意这个函数不会检查bio1和bio2是否指向其他BIO，二者的值都会被重写，且不会调用BIO_free()函数。因此在使用之前必须自己保证这两个变量为空，否则会造成内存泄漏。

因为BIO对只会简单的缓存数据，而不会直接涉及到连接，所以它看起来**就像非阻塞型的接口**，如果**写缓存满了或读缓存空的时候，调用IO函数就会立刻返回**。也就是说，应用程序必须自己对写缓存执行flush操作或对读缓存执行fill操作。

## BIO链操作

BIO结构其实是一个链式结构，在BIO链上添加或删除BIO的函数有：

```C++
BIO * BIO_push(BIO *b,BIO *append); //把名为append的链附加到名为b的链上，返回b
BIO * BIO_pop(BIO *b); //把名为b的BIO从它所在的BIO链中移除，并返回下一个BIO。如果没有则返回NULL
```

如同链表一样，一个BIO链的第一个元素便代表了整个链。在构造完BIO链之后，从链中的第一个元素开始做写操作时，数据会经过BIO链上每一个元素的处理，然后将BIO链最后一个元素的输出写入到文件中。而对于读操作来说，则是从BIO链的最后一个元素开始，沿着相反的方向进行操作，最后读取BIO链的第一个元素的输出。

![这里写图片描述](https://img-blog.csdn.net/20170815180254544?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvbGlhbzIwMDgxMjI4/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/SouthEast)

BIO链也提供了遍历与查找的函数：

```c++
BIO* BIO_find_type(BIO *b,int bio_type); //在BIO链中根据特定类型的bio_type进行搜索，起始位置为b，返回最先找到的那个BIO，如果未找到则返回null
BIO* BIO_next(BIO *b); //返回b所在BIO链中的下一个BIO

#define BIO_method_type(b) ((b)->method->type)
```

其中，bio_type的值定义见附录。

# BIO分类

## Source/Sink类型BIO

### 文件指针类型BIO

文件类型BIO的相关函数和定义如下：

```c++
#include<openssl/bio.h>    

BIO_METHOD*  BIO_s_file(void); //返回文件BIO的BIO_METHOD定义

BIO* BIO_new_file(const char *filename, const char *mode); //从文件名新建BIO
BIO* BIO_new_fp(FILE *stream, int flags); //从文件指针新建BIO

BIO_set_fp(BIO *b,FILE *fp, int flags);  //将bio与文件指针绑定
BIO_get_fp(BIO *b,FILE **fpp);           //获取bio绑定的的文件指针

int BIO_read_filename(BIO *b, char *name) //设置BIO的读文件名
int BIO_write_filename(BIO *b, char *name) //设置BIO的写文件名
int BIO_append_filename(BIO *b, char *name) //设置BIO的附加文件名
int BIO_rw_filename(BIO *b, char *name) //设置BIO的读写文件名
```

file类型的BIO_METHOD结构体变量如下：

```c++
static BIO_METHOD methods_filep =
{
     BIO_TYPE_FILE,
     "FILE pointer",
     file_write,
     file_read,
     file_puts,
     file_gets,
     file_ctrl,
     file_new,
     file_free,
     NULL,
};
```

在file类型中，使用前面介绍过的**BIO_read和BIO_write**对底层的file数据流进行读写操作，file类型BIO是支持**BIO_gets和BIO_puts**函数的。 

**BIO_flush**函数在file类型BIO中只是简单调用了API函数fflush。 

**BIO_reset**函数则将文件指针重新指向文件的开始位置，它调用fseek(stream,0,0) 函数实现该功能。 

**BIO_seek**函数将文件指针位置至于所定义的位置ofs上（从文件开头开始计算的偏移ofs），它调用了文件的操作函数fseek(stream,ofs,0)，是一个宏定义形式的函数，需要注意的是，因为该函数调用了fseek函数，所以成功的时候返回0，失败的时候返回－1，这是跟标准BIO_seek函数定义不一样的，因为标准的定义是成功返回1，失败返回非正值。 

**BIO_eof**调用了feof函数。 如果在BIO结构中**设置了BIO_CLOSE**的标志，则在BIO释放的时候会**自动调用fclose函数**。

**BIO_tell**返回位置指针的值。是一个宏定义函数。

BIO文件类型操作的程序示例（省略了头文件等）：

```c++
// 1.最简单的实例程序
BIO *bio_out;
bio_out = BIO_new_fp(stdout, BIO_NOCLOSE); 
BIO_printf(bio_out, "Hello World\n");
//  2.上述例子的另一种实现方法
BIO *bio_out;
bio_out = BIO_new(BIO_s_file());
if(bio_out == NULL) 
    /*error handling*/
if(!BIO_set_fp(bio_out, stdout, BIO_NOCLOSE)) 
    /* 出错则将文件流定向到标准输出*/
BIO_printf(bio_out, "Hello World\n");
//  3.写文件操作
BIO *out;
out = BIO_new_file("filename.txt", "w");
if(!out) /*出错*/
BIO_printf(out, "Hello World\n");
BIO_free(out);
//   4.上述例子的另一种实现方法
BIO *out;
out = BIO_new(BIO_s_file());
if(out == NULL) 
    /* Error handling */
if(!BIO_write_filename(out, (void*)"filename.txt")) 
    /* Error handling */
BIO_printf(out, "Hello World\n"); //此处换成BIO_read或BIO_gets可以实现读文件操作
BIO_free(out);
```

### 文件描述符类型BIO

文件描述符类型BIO是一个Source/Sink类型的BIO，定义了以下一些类型的函数：

```C++
#include<openssl/bio.h>     

BIO_METHOD * BIO_s_fd(void); //返回一个文件描述符类型的BIO_METHOD结构
#define BIO_set_fd(b,fd,c) BIO_int_ctrl(b,BIO_C_SET_FD,c,fd) //将BIO的底层文件描述符设置为fd
#define BIO_get_fd(b,c)    BIO_ctrl(b,BIO_C_GET_FD,0,(char *)c) //返回相应的BIO底层文件描述符，存储在参数c中
BIO*    BIO_new_fd(int fd, int close_flag); //创建并返回一个底层描述符为fd，关闭标志位close_flag的文件描述符类型的BIO
```

fd类型的BIO_METHOD结构体变量定义如下：

```C++
static BIO_METHOD methods_fdp=
{
         BIO_TYPE_FD,
         "file descriptor",
         fd_write,
         fd_read,
         fd_puts,
         NULL, /* fd_gets, 与file类型的BIO相比它没有实现gets方法*/ 
         fd_ctrl,
         fd_new,
         fd_free,
         NULL,
}
```

**BIO_read**和**BIO_write**对底层的文件描述符结构进行读写操作。这两个函数的一些行为取决于他们所在的平台的文件描述符的读写函数的行为，如果底层的文件描述符是非阻塞型的，那么他们基本上是跟前面介绍过的BIO的IO操作函数一样的。

**BIO_puts**是支持的，但是**BIO_gets**在本类型描述符中是不支持的。

**BIO_reset**调用lseek(fd,0,0)函数，使文件指针指向开始的位置。调用成功返回0，失败返回－1。 

**BIO_seek**调用了lseek(fd,ofs,0)函数，设置文件指针的位置到从文件头偏移ofs的位置，成功返回文件指针的位置，失败返回－1。 

**BIO_tell**返回目前文件指针的位置，它其实调用了lseek(fd,0,1)函数，失败返回－1。 如果设置了关闭标志，那么当BIO被释放的时候底层的文件描述符就会被关闭。

程序示例（省略了头文件和主函数部分）：

```C++
BIO *out;
out = BIO_new_fd(fileno(stdout), BIO_NOCLOSE);
BIO_write(out, "Hello World\n", 13); 
BIO_free(out); 
```

附：Linux系统的文件描述符介绍https://blog.csdn.net/cywosp/article/details/38965239

### Socket类型BIO

Socket类型的BIO是一种source/sink型BIO，封装了Socket的IO操作，它相关的一 些函数定义如下：

```c++
#include<openssl/bio.h>

BIO_METHOD*  BIO_s_socket(void);

#define      BIO_set_fd(b,fd,c)  BIO_int_ctrl(b,BIO_C_SET_FD,c,fd) //将Socket描述符fd设置为BIO的底层IO结构，同时可以设置关闭标志c
#define      BIO_get_fd(b,c)     BIO_ctrl(b,BIO_C_GET_FD,0,(char *)c) //返回指定BIO的Socket描述符

BIO*         BIO_new_socket(int sock, int close_flag); //根据一个给定参数返回一个socket类型的BIO，成功则返回该BIO指针，失败返回NULL
```

Socket类型的BIO_METHOD结构定义如下：

```c++
static BIO_METHOD methods_sockp=
{
     BIO_TYPE_SOCKET,
     "socket",
     sock_write,
     sock_read,
     sock_puts,
     NULL, /* sock_gets, 在Socket的BIO类型中不支持*/
     sock_ctrl,
     sock_new,
     sock_free,
     NULL,
};
```

### NULL型BIO

NULL类型的BIO是一个空的Source/Sink型BIO，写入这个BIO的数据都被丢掉，从这里执行读操作也总是返回EOF。其相关函数定义如下：

```c++
#include<openssl/bio.h> 
     　　
BIO_METHOD * BIO_s_null(void);
```

NULL类型的BIO_METHOD结构定义如下：

```
static BIO_METHOD null_method=
{
        BIO_TYPE_NULL,
    　　 "NULL",
    　　 null_write,
    　　 null_read,
    　　 null_puts,
    　　 null_gets,
    　　 null_ctrl,
    　　 null_new,
    　　 null_free,
    　　 NULL,
};
```

从结构上看，这个类型的BIO实现了不少的函数，但是，所有这些函数都只是简单返回0、1或者输入数据的长度，而不作任何事情。

一般来说，在openssl里面，**这种类型的BIO是置放在BIO链的末尾**的，比如在应用程序中，如果你要将一些数据通过filter型的BIO  digest进行摘要算法，但不需要把它送往任何地方，又因为一个BIO链要求以source/sink型BIO开始或结束，所以这时候就可以在BIO链的末尾添加一个source/sink型的NUll类型BIO来实现这个功能。

### 内存类型BIO

内存型BIO是source/sink型BIO，它**使用内存作为它的I/O**。它定义的相关函数如下：

```c++
#include<openssl/bio.h> 

BIO_METHOD * BIO_s_mem(void);

BIO_set_mem_eof_return(BIO *b,int v) //设置一个没有数据的内存型BIO的执行读动作的行为
long  BIO_get_mem_data(BIO *b, char **pp) //将参数pp的指针指向内存型BIO的数据开始处

BIO_set_mem_buf(BIO *b,BUF_MEM *bm,int c) //将参数bm代表的BUF_MEM结构作为该BIO的底层结构
BIO_get_mem_ptr(BIO *b,BUF_MEM **pp) //将底层的BUF_MEM结构放在指针pp中

BIO *BIO_new_mem_buf(void *buf, int len); //创建一个内存型BIO，其数据为buf里面长度为len的数据，如果参数len是－1，那么就默认buf是以null结束的，使用strlen计算长度。这时候BIO被设置为只读的，不能执行写操作。它用于数据需要存储在一块静态内存中并以BIO形式存在的时候。所需要的数据是直接从内存中读取的，而不是先要执行拷贝操作（读写方式的内存BIO就是要先拷贝），这也就要求这块内存是只读的，不能改变，一直维持到BIO被释放。
```

写进该类型BIO的数据被存储在BUF_MEM结构中，该结构被定义为适合存储数据的一种结构，其结构定义如下：

```c++
typedef struct buf_mem_st
{
     int length； /* current number of bytes */
     char *data;
     int max; /* size of buffer */
} BUF_MEM;
```

注意内存型BIO的内存是可以无限扩大的，也就是说无论写入多少数据都可以成功执行。一般任何写入内存型BIO的数据都可以被读出，除非该内存型BIO为只读类型。

内存型的BIO_METHOD定义如下：

```c++
static BIO_METHOD mem_method=
{
     BIO_TYPE_MEM,
     "memory buffer",
     mem_write,
     mem_read,
     mem_puts,
     mem_gets,
     mem_ctrl,
     mem_new,
     mem_free,
     NULL,
};
```

**BIO_write和BIO_read**函数是支持的。对内存型BIO执行写操作总是成功的，因为内存型BIO的内存能够无限扩大。**任何一个对可读写的内存型BIO的**读操作都会在使用内部拷贝操作从BIO里面删除该段数据，这样一来，如果BIO里面有大量的数据，而读的却只是很小的一些片断，那么会导致操作非常慢。**使用只读的内存型BIO避免了这个问题**。在使用的时候，如果内存型BIO必须使用可读写的，那么可以加一个Buffer型BIO到BIO链中，这可以使操作速度更快。

**BIO_gets和BIO_puts**操作在内存型BIO是支持的。 

如果设置了BIO_CLOSE标志，那么内存型BIO被释放的时候其底层的BUF_MEM型BIO也同时被释放。 

 **BIO_reset**函数被调用时，如果该BIO是可读写的，那么该BIO所有数据都会被清空；如果该BIO是只读的，那么该操作只会简单将指针指向原始位置，里面的数据可以再读。

 **BIO_eof**返回true，表明只读时候BIO里面没有可读数据。 

 **BIO_ctrl_pending**返回目前BIO里面存储的数据的字节(byte)数。

程序示例：

```C++
int main()
{
	BIO     *b=NULL;
	int     len=0;
	char    *out=NULL;
	b=BIO_new(BIO_s_mem());
	len=BIO_write(b,"openssl",4); //将4字节的数据写入b（即只写入了"open"），返回值为4
	len=BIO_printf(b,"%s","Hello world!\n"); //将字符串写入b
	len=BIO_ctrl_pending(b); //返回存储字节数
	out=(char *)OPENSSL_malloc(len);
	len=BIO_read(b,out,len); //从b的缓冲区中读取全部字符串
    printf("%s",out);//使用printf可以将out中的内容打印出来
	OPENSSL_free(out);
	BIO_free(b);
	return 0;
}
```

### BIO对

BIO对也是作为一种source/sink类型的BIO来处理的，也就是说，BIO里面还提供了一种专门的BIO_METHO方法来处理BIO对的各种操作。BIO对类型的BIO各种相关的函数定义如下：

```c++
#include<openssl/bio.h> 
BIO_METHOD *BIO_s_bio(void);

#define BIO_make_bio_pair(b1,b2) (int)BIO_ctrl(b1,BIO_C_MAKE_BIO_PAIR,0,b2) //将两个单独的BIO连接起来形成BIO对
#define BIO_destroy_bio_pair(b) (int)BIO_ctrl(b,BIO_C_DESTROY_BIO_PAIR,0,NULL) //将两个连接起来的BIO对拆开，如果一个BIO对的任何一个被释放则自动执行该操作
#define BIO_shutdown_wr(b) (int)BIO_ctrl(b, BIO_C_SHUTDOWN_WR, 0, NULL) //关闭BIO对中的其中一个BIO，在这之后，针对于该BIO的写操作返回错误，从另一个BIO读取数据的时候返回剩余有效数据或者EOF

#define BIO_set_write_buf_size(b,size) (int)BIO_ctrl(b,BIO_C_SET_WRITE_BUF_SIZE,size,NULL) //设置BIO的缓冲区大小
#define BIO_get_write_buf_size(b,size) (size_t)BIO_ctrl(b,BIO_C_GET_WRITE_BUF_SIZE,size,NULL) //返回写缓冲区的大小

int BIO_new_bio_pair(BIO **bio1, size_t writebuf1, BIO **bio2, size_t writebuf2); //其实是调用BIO_new,BIO_make_bio_pair和BIO_set_write_buf_size函数来创建一对BIO对

#define BIO_get_write_guarantee(b) (int)BIO_ctrl(b,BIO_C_GET_WRITE_GUARANTEE,0,NULL) //返回当前能够写入BIO的数据的最大长度

size_t BIO_ctrl_get_write_guarantee(BIO *b); //返回当前能够写入BIO的数据的最大长度

#define BIO_get_read_request(b) (int)BIO_ctrl(b,BIO_C_GET_READ_REQUEST,0,NULL) //返回要求发送的数据的长度

size_t BIO_ctrl_get_read_request(BIO *b); //返回要求发送的数据的长度
int    BIO_ctrl_reset_read_request(BIO *b); //把BIO_get_read_request要返回值设置为0
```

所谓的BIO对只是将两个BIO的终端输出（BIO结构中参数peer的ptr成员）相互设置为对方，从而形成一种对称的结构：

```
bio1->peer->ptr=bio2
bio2->peer->ptr=bio1
```

BIO对类型的BIO_METHOD定义如下：

```C++
static BIO_METHOD methods_biop =
{       
     BIO_TYPE_BIO,
     "BIO pair",
     bio_write,
     bio_read,
     bio_puts,
     NULL /* 没有定义 bio_gets */,
     bio_ctrl,
     bio_new,
     bio_free,
     NULL /* 没有定义 bio_callback_ctrl */
};
```

因为没有提供内部数据结构的内存锁结构(lock)，所以，**一般来说一个BIO对的两个BIO都必须在一个线程下使用**。因为BIO链通常是以一个source/sink  BIO结束的，所以就可以实现应用程序通过控制BIO对的一个BIO从而控制整个BIO链的数据处理。其实，也就相当于BIO对给应用程序提供了一个处理整个BIO链的入口。BIO对的一个典型应用就是在应用程序里面控制TLS/SSL的I/O接口，一般来说，在应用程序想在TLS/SSL中使用非标准的传输方法或者不适合使用标准的socket方法的时候就可以采用这样的方法来实现。 

BIO对释放的时候，需要分别释放两个BIO，如果在使用BIO_free或者BIO_free_all释放了其中一个BIO的时候，另一个BIO就也必须要释放。 

当BIO对使用在双向应用程序的时候，如TLS/SSL，一定要对写缓冲区里面的数据执行flush操作。当然，也可以通过在BIO对中的另一个BIO调用BIO_pending函数，如果有数据在缓冲区中，那么就将它们读出并发送到底层的传输通道中去。为了使请求或BIO_should_read函数调用成功（为true），在执行任何正常的操作（如select）之前，都必须这样做才行。 

**BIO_read**函数从缓冲BIO中读取数据，如果没有数据，则发出一个重试请求。 

**BIO_write**函数往缓冲BIO中写入数据，如果缓冲区已满，则发出一个重试请求。 

**BIO_ctrl_pending和BIO_ctrl_wpending**函数可以用来查看在读或写缓冲区里面有效的数据的数量。 

**BIO_reset**函数将写缓冲区里面的数据清除。

### 连接类型BIO

该类型的BIO封装了socket的Connect方法，它使得编程的时候可以使用统一的BIO规则进行socket的connect连接的操作和数据的发送接受，而不用关心具体平台的Socket的connect方法的区别。其相关定义的一些函数如下(openssl\bio.h)：

```C++
BIO_METHOD * BIO_s_connect(void);

#define      BIO_set_conn_hostname(b,name)  BIO_ctrl(b,BIO_C_SET_CONNECT,0,(char *)name) //使用字符串设置主机名，可以是IP地址或是网址等形式
#define      BIO_set_conn_port(b,port)      BIO_ctrl(b,BIO_C_SET_CONNECT,1,(char *)port) //设置主机端口号
#define      BIO_set_conn_ip(b,ip)          BIO_ctrl(b,BIO_C_SET_CONNECT,2,(char *)ip) //使用二进制模式设置ip
#define      BIO_set_conn_int_port(b,port)  BIO_ctrl(b,BIO_C_SET_CONNECT,3,(char *)port) //以整数形式设置主机端口号
#define      BIO_get_conn_hostname(b)       BIO_ptr_ctrl(b,BIO_C_GET_CONNECT,0) //返回连接类型BIO的主机名
#define      BIO_get_conn_port(b)           BIO_ptr_ctrl(b,BIO_C_GET_CONNECT,1) //返回字符串类型的端口信息
#define      BIO_get_conn_ip(b,ip)          BIO_ptr_ctrl(b,BIO_C_SET_CONNECT,2) //返回二进制形式的ip地址
#define      BIO_get_conn_int_port(b,port)  BIO_int_ctrl(b,BIO_C_SET_CONNECT,3,port) //返回整数形式端口号
#define      BIO_set_nbio(b,n)              BIO_ctrl(b,BIO_C_SET_NBIO,(n),NULL) //设置I/O的非阻塞标志
#define      BIO_do_connect(b)              BIO_do_handshake(b) //进行连接操作，如果连接成功返回1，否则返回0或负值

BIO *        BIO_new_connect(char *str) //创建并返回一个连接类型的BIO，其实，它调用了BIO_s_connect、BIO_new和BIO_set_conn_hostname函数完成了整个操作。成功则返回一个BIO，否则返回NULL。
```

这种类型的BIO_METHOD结构定义如下：

```
static BIO_METHOD methods_connectp=
{
     BIO_TYPE_CONNECT,
     "socket connect",
     conn_write,
     conn_read,
     conn_puts,
     NULL, /* connect_gets, */
     conn_ctrl,
     conn_new,
     conn_free,
     conn_callback_ctrl,
};
```

**BIO_read和BIO_write**的操作调用底层的连接的IO操作来完成。如果在服务器地址和端口设置正确，但连接没有建立的时候调用读写操作函数，那么会先进行连接的建立操作，然后再进行读写操作。 

**BIO_puts**操作是支持的，但是**BIO_gets操作不支持**，这在该类型BIO的BIO_METHOD结构定义中就可以看出来。 

如果**关闭标志设置了，那么在BIO被释放的时候，任何活动的连接和socket都会被关闭**。 

**BIO_reset**方法被调用的时候，连接（connect）类型的BIO的任何活动连接都会被关闭，从而回到可以重新跟同样的主机建立连接的状态。

**BIO_get_fd**函数返回连接类型的BIO的底层socket，当参数c不是NULL的时候，就将该socket赋值给c，当然，socket也作为返回值。c参数应该为int\*类型。如果BIO没有初始化，则返回－1。

例：连接到本地Web服务器

```c++
#include <openssl/bio.h>
#include <stdio.h>
int	main()
{
    BIO *cbio, *out;
    int len;
    char tmpbuf[1024];

    cbio = BIO_new_connect("localhost:http");
    out = BIO_new_fp(stdout, BIO_NOCLOSE);
    if(BIO_do_connect(cbio) <= 0) 
        fprintf(stderr, "Error connecting to server\n");
    BIO_puts(cbio, "GET / HTTP/1.0\n\n");
    for(;;) 
    {
           len = BIO_read(cbio, tmpbuf, 1024);
           if(len <= 0) break;
           BIO_write(out, tmpbuf, len);
    }
    BIO_free(cbio);
    BIO_free(out);
    return 0;
}
```

### 接受类型BIO

接受（accept）类型的BIO跟连接（connect）类型BIO是相对应的，它封装了Socket的accept方法及其相关的一些操作，使得能够对不同的平台使用同一的函数进行操作。其定义的相关函数如下：

```
#include<openssl/bio.h>
BIO_METHOD * BIO_s_accept(void);  

#define      BIO_set_accept_port(b,name) BIO_ctrl(b,BIO_C_SET_ACCEPT,0,(char*)name) //使用字串名来设置接受端口，形式为"host:port"，这两部分可以为*，表示可以使用任意接口和端口
#define      BIO_get_accept_port(b) BIO_ptr_ctrl(b,BIO_C_GET_ACCEPT,0)  

BIO *        BIO_new_accept(char *host_port); //该函数将BIO_new和BIO_set_accept_port函数放在一个函数里面调用，创建一个新的接受类型BIO

#define      BIO_set_nbio_accept(b,n) BIO_ctrl(b,BIO_C_SET_ACCEPT,1,(n)?"a":NULL) //设置接受socket是否为阻塞模式，参数n为0为阻塞模式，1为非阻塞模式
#define      BIO_set_accept_bios(b,bio) BIO_ctrl(b,BIO_C_SET_ACCEPT,2,(char *)bio) //设置一个BIO链，当接受到一个连接之后，这个设置好的BIO链就会被附加到整个BIO链上。调用这一函数之后，只有在接受BIO被释放后链上的BIO才能自动释放
#define      BIO_set_bind_mode(b,mode) BIO_ctrl(b,BIO_C_SET_BIND_MODE,mode,NULL) //设置绑定模式
#define      BIO_get_bind_mode(b,mode) BIO_ctrl(b,BIO_C_GET_BIND_MODE,0,NULL) //获得绑定模式
#define      BIO_BIND_NORMAL 0 //默认绑定模式，那么另外一个socket就不能绑定到同一个端口
#define      BIO_BIND_REUSEADDR_IF_UNUSED 1 //绑定模式1，首先会尝试BIO_BIND_NORMAL的模式绑定到端口，如果失败了而且端口没有使用，那么就会使用BIO_BIND_REUSEADDR 模式绑定到端口
#define      BIO_BIND_REUSEADDR 2 //绑定模式2，另外的socket可以绑定到同一个端口
#define      BIO_do_accept(b) BIO_do_handshake(b) 当它在接受（accept）BIO设置好之后第一被调用的时候，它会创建一个接受socket并把它跟地址绑定；第二次被调用的时候，它会等待连接的到来
```

接受类型BIO的BIO_METHOD结构定义如下：

```c++
static BIO_METHOD methods_acceptp=
{
     BIO_TYPE_ACCEPT,
     "socket accept",
     acpt_write,
     acpt_read,
     acpt_puts,
     NULL, /* connect_gets, */
     acpt_ctrl,
     acpt_new,
     acpt_free,
     NULL,
};
```

这一类型的BIO对各种平台的TCP/IP的accept做了封装，所以在使用的时候就可以同一的使用BIO的规则进行操作，而不用担心因为不同的平台带来程序改写或增加移植的工作量。 

**BIO_read和BIO_write**函数操作调用了底层平台连接的I/O相关操作，如果这时候没有连接建立，端口设置正确，那么该BIO就会等待连接的建立。事实上，当一个连接建立的时候，一个新的socket类型BIO就会被创建并附加到BIO链中，形成accept->socket的BIO结构，所以这时候对初始化了的接受socket进行IO操作就会导致它处于等待连接建立的状态。当一个接受类型的BIO在BIO链的末尾的时候，在处理I/O调用之前它会先等待一个连接的建立；如果不是在末尾，那么它简单的把I/O调用传到下一个BIO。 

如果接受（accept）类型BIO的**关闭标志设置**了，那么当BIO被释放的时候，该BIO链上任何活动的连接和socket都会被关闭。 

**BIO_get_fd和BIO_set_fd**可以用来取得和设置该连接的socket描述符。

## Filter类型BIO

### NULL型过滤BIO

NULL型的filterBIO的定义如下：

```c++
#include<openssl/bio.h>

BIO_METHOD * BIO_f_null(void);
```

该函数返回一个NULL型的过滤BIO_METHOD结构，NULL过滤型BIO是一个不作任何事情的BIO。针对该类型BIO的任何调用都会被简单传递中BIO链中的下一个BIO中去，也就相当于该BIO是不存在的一样。

### 缓冲类型BIO

缓冲类型BIO的相关定义如下：

```cpp
#include<openssl/bio.h>

#define BIO_get_buffer_num_lines(b)         BIO_ctrl(b,BIO_C_GET_BUFF_NUM_LINES,0,NULL) //返回缓冲区中目前数据的行数

#define BIO_set_read_buffer_size(b,size)    BIO_int_ctrl(b,BIO_C_SET_BUFF_SIZE,size,0) //设置读缓冲区大小
#define BIO_set_write_buffer_size(b,size)   BIO_int_ctrl(b,BIO_C_SET_BUFF_SIZE,size,1) //设置写缓冲区大小
#define BIO_set_buffer_size(b,size)         BIO_ctrl(b,BIO_C_SET_BUFF_SIZE,size,NULL) //设置读写缓冲区大小

#define BIO_set_buffer_read_data(b,buf,num) BIO_ctrl(b,BIO_C_SET_BUFF_READ_DATA,num,buf) //清空缓冲区原有的数据
```

buffer类型的BIO_METHOD结构定义如下：

```
static BIO_METHOD methods_buffer=
{
     BIO_TYPE_BUFFER,
     "buffer",
     buffer_write,
     buffer_read,
     buffer_puts,
     buffer_gets,
     buffer_ctrl,
     buffer_new,
     buffer_free,
     buffer_callback_ctrl,
};
```

由结构定义可见，**该类型BIO支持所有BIO的I/O函数**。

写入缓冲(buffer)BIO的数据存储在缓冲区里面，定期写入到BIO链的下一个BIO中。事实上，**只有缓冲区已满或者调用了BIO_flush函数时**，数据才会写入下面的BIO，所以，当任何存储在缓冲区的数据需要写入的时候（如在使用BIO_pop函数从BIO链中删除一个buffer类型BIO之前），**必须使用BIO_flush函数**，如果BIO链的末尾是一个非阻塞型的BIO，有时候调用BIO_flush可能出现失败，需要重试的情况。

从该类型BIO读取数据时，数据从下一个BIO填充到该BIO内部缓冲区中，然后再读出来。该类型BIO支持BIO_gets和BIO_puts方法，事实上，BIO_gets函数是通过在下一个BIO的BIO_read函数来实现的，所以，如果一个BIO不支持BIO_gets方法（如SSL类型的BIO），可以通过预先附加一个buffer类型BIO来实现BIO_gets的功能。 

**BIO_reset**被调用的时候，该类型BIO里面的所有数据都会被清空。

### Base64类型BIO

Base64类型的BIO定义在evp目录下。当往该BIO写入数据时，数据被Base64编码，当从该BIO读数据时，数据被Base64解码。该BIO不支持BIO_gets和BIO_puts的功能。它的定义如下：

```c++
#include<openssl/bio.h>
#include<openssl/evp.h>

BIO_METHOD * BIO_f_base64(void);
```

Base64类型的BIO_METHOD结构定义如下：

```c++
static BIO_METHOD methods_b64=
{
     BIO_TYPE_BASE64,
     "base64 encoding",
     b64_write,
     b64_read,
     NULL, /* b64_puts, */
     NULL, /* b64_gets, */
     b64_ctrl,
     b64_new,
     b64_free,
     b64_callback_ctrl,
};
```

BIO_set_flags函数可以用来设置标记BIO_FLAGS_BASE64_NO_NL，该标记设置后，将把所有数据编码成为一行。由于base64编码本身格式的原因，不能准确可靠的决定编码后的数据块的结束位置，使用的时候需要注意数据的长度问题。

程序示例：将字符串进行编码并将编码后的数据输出到标准输出设备

```c++
BIO *bio, *b64;
char message[] = "Hello World \n";
b64 = BIO_new(BIO_f_base64());
bio = BIO_new_fp(stdout, BIO_NOCLOSE);
bio = BIO_push(b64, bio);
BIO_write(bio, message, strlen(message));
BIO_flush(bio);
BIO_free_all(bio);
```

### Cipher类型BIO

该类型的BIO将**写入该BIO的数据加密，从该BIO读数据时数据被解密**，它事实上封装了EVP_CipherInit、EVP_CipherUpdate、EVP_CipherFinal三种方法。它不支持BIO_puts和BIO_gets的方法，如果要使用这两个方法，可以通过在前面附加一个buffer类型的BIO来实现。它的定义如下：

```cpp
#include<openssl/bio.h>
#include<openssl/evp.h>

BIO_METHOD * BIO_f_cipher(void); //返回cipher类型的BIO_METHOD
void         BIO_set_cipher(BIO *b,const EVP_CIPHER *cipher,unsigned char *key, unsigned char *iv, int enc); //设置BIO的加密算法，key为加密密钥，iv为初始化的IV，enc=1为加密，enc=0为解密

int          BIO_get_cipher_status(BIO *b) //检测解密是否成功执行
int          BIO_get_cipher_ctx(BIO *b, EVP_CIPHER_CTX **pctx) //返回BIO的内部加密体制
```

cipher类型的BIO_METHOD结构如下：

```c++
static BIO_METHOD methods_enc=
{
     BIO_TYPE_CIPHER,
     "cipher",
     enc_write,
     enc_read,
     NULL, /* enc_puts, */
     NULL, /* enc_gets, */
     enc_ctrl,
     enc_new,
     enc_free,
     enc_callback_ctrl,
};
```

跟base64型BIO相似，当调用**BIO_flush函数**时，表明所有数据都已经通过该类型BIO加密了，用来将最后的一段数据通过该BIO进行加密。在进行加密的时候，**必须调用BIO_flush**函数来把最后的数据通过BIO进行加密，否则最后的数据会在解密的时候出现失败的情况。当从一个加密类型的BIO读取数据时，当读到最后一段数据时，会通过检测EOF自动检测到数据结束标志并自动将这段数据解密。

### MD类型BIO

MD类型BIO对通过它的任何数据都进行摘要操作（message digest），事实上，该类型BIO封装了EVP_DigestInit、EVP_DigestUpdate和EVP_DigestFinal三个函数的功能和行为。该类型BIO是完全对称的，也就是说，不管是读数据（BIO_read）还是写数据（BIO_write），都进行相同的摘要操作。

MD类型BIO的定义如下：

```cpp
#include<openssl/bio.h>
#include<openssl/evp.h>

BIO_METHOD * BIO_f_md(void);

int BIO_set_md(BIO *b,EVP_MD *md); //用参数md设置给定的BIO摘要算法。该函数必须在执行读写操作前调用，用于初始化一个摘要类型的BIO，调用成功返回1，失败返回0
int BIO_get_md(BIO *b,EVP_MD **mdp); //返回BIO摘要方法的指针到mdp参数里面。调用成功返回1，否则返回0。

int BIO_get_md_ctx(BIO *b,EVP_MD_CTX **mdcp); //返回摘要BIO的方法结构到mdcp参数里面
```

MD类型的BIO_METHOD结构如下：

```cpp
static BIO_METHOD methods_md=
{
     BIO_TYPE_MD,"message digest",
     md_write,
     md_read,
     NULL, /* md_puts, */
     md_gets,
     md_ctrl,
     md_new,
     md_free,
     md_callback_ctrl,
};
```

**BIO_gets**函数执行的时候，如果给定的size参数足够大，可以完成摘要（digest）计算，那么就会返回摘要值。BIO_puts函数是不支持的，如果需要支持该函数，可以在前面附加一个buffer类型的BIO。 
**BIO_reset**函数重新初始化一个摘要类型的BIO，事实上，它是简单重新调用了EVP_DigestInit函数进行初始化。 
注意，在从一个摘要BIO里面读取完摘要信息之后，在重新使用该BIO之前，**必须**调用BIO_reset或BIO_set_md重新初始化该BIO才行。

示例：从标准输入流中读取数据然后输出数据摘要

```cpp
#include<stdio.h>
#include<openssl/bio.h>
#include<openssl/evp.h>
#include<cstring>

int main()
{
        BIO *bio,*mdtmp;
        char buf[1024];
        int rdlen;

		bio=BIO_new_fp(stdin,BIO_NOCLOSE);
        mdtmp=BIO_new(BIO_f_md());
        BIO_set_md(mdtmp,EVP_sha1());

        bio=BIO_push(mdtmp,bio);
        mdtmp=BIO_new(BIO_f_md());
        BIO_set_md(mdtmp,EVP_md5());
        bio=BIO_push(mdtmp,bio);

        do{
                rdlen=BIO_read(bio,buf,sizeof(buf));
        }while(rdlen>0);

        do{
                EVP_MD* md;
                mdtmp=BIO_find_type(mdtmp,BIO_TYPE_MD);
                if(!mdtmp)
                        break;
                BIO_get_md(mdtmp,&md);
                printf("%s",OBJ_nid2sn(EVP_MD_type(md)));
                int mdlen=BIO_gets(mdtmp,buf,EVP_MAX_MD_SIZE);
                for(int i=0;i<mdlen;i++)
                        printf(":%02X",buf[i]);
                printf("\n");
                mdtmp=BIO_next(mdtmp);
        }while(mdtmp);
        BIO_free_all(bio);
        return 0;
}
```

### SSL类型BIO

SSL类型BIO封装了openssl里面的ssl规则和函数，相当于提供了一个使用SSL很好的有效工具。它的定义如下：

```cpp
#include<openssl/bio.h>
#include<openss/ssl.h>

BIO_METHOD *BIO_f_ssl(void);

#define BIO_set_ssl(b,ssl,c)       BIO_ctrl(b,BIO_C_SET_SSL,c,(char *)ssl) //设置ssl类型BIO的内部ssl指针指向ssl
#define BIO_get_ssl(b,sslp)        BIO_ctrl(b,BIO_C_GET_SSL,0,(char *)sslp) //返回ssl类型BIO内部的ssl结构指针，得到该指针后可以用标志的ssl函数对它进行操作
#define BIO_set_ssl_mode(b,client) BIO_ctrl(b,BIO_C_SSL_MODE,client,NULL) //设置ssl的工作模式，参数client为1代表客户端模式，client为0代表服务器模式
#define BIO_set_ssl_renegotiate_bytes(b,num) BIO_ctrl(b,BIO_C_SET_SSL_RENEGOTIATE_BYTES,num,NULL); //设置重新进行session协商的读写数据的长度为num

#define BIO_set_ssl_renegotiate_timeout(b,seconds) 
BIO_ctrl(b,BIO_C_SET_SSL_RENEGOTIATE_TIMEOUT,seconds,NULL); //设置重新进行session协商的时间
#define BIO_get_num_renegotiates(b) BIO_ctrl(b,BIO_C_SET_SSL_NUM_RENEGOTIATES,0,NULL); //返回ssl连接在因为字节限制或时间限制导致session重新协商之前总共读写的数据长度

BIO*    BIO_new_ssl(SSL_CTX *ctx,int client); //使用ctx参数所代表的SSL_CTX结构创建一个SSL类型的BIO，如果client不为0则使用客户端模式
BIO*    BIO_new_ssl_connect(SSL_CTX *ctx); //创建一个包含ssl类型BIO的新BIO链，并在后面附加了一个连接类型的BIO
BIO*    BIO_new_buffer_ssl_connect(SSL_CTX *ctx); //创建一个包含buffer型的BIO，一个SSL类型的BIO和一个连接类型的BIO
int     BIO_ssl_copy_session_id(BIO *to,BIO *from); //将BIO链from的SSL Session ID拷贝到BIO链to中
void    BIO_ssl_shutdown(BIO *bio); //关闭一个BIO链中的ssl连接
#define BIO_do_handshake(b) BIO_ctrl(b,BIO_C_DO_STATE_MACHINE,0,NULL) //在相关的BIO上启动SSL握手过程并建立SSL连接。连接成功建立返回1，否则返回0或负值
```

SSL类型的BIO_METHOD结构如下：

```cpp
static BIO_METHOD methods_sslp=
{
    BIO_TYPE_SSL,"ssl",
    ssl_write,
    ssl_read,
    ssl_puts,
    NULL, /* ssl_gets, */
    ssl_ctrl,
    ssl_new,
    ssl_free,
    ssl_callback_ctrl,
};
```

**BIO_read和BIO_write**函数调用的时候，SSL类型的BIO会使用SSL协议进行底层的I/O操作。如果此时SSL连接并没有建立，那么就会在调用第一个IO函数的时候先进行连接的建立。 

如果使用**BIO_push**将一个BIO附加到一个SSL类型的BIO上，那么SSL类型的BIO读写数据的时候，它会被自动调用。 

**BIO_reset**调用的时候，会调用SSL_shutdown函数关闭目前所有处于连接状态的SSL，然后再对下一个BIO调用BIO_reset，这功能一般就是将底层的传输连接断开。调用完成之后，SSL类型的BIO就处于初始的接受或连接状态。 

如果**设置了BIO关闭标志**，那么SSL类型BIO释放的时候，内部的SSL结构也会被SSL_free函数释放。

```cpp
#include<openssl/bio.h>
#include<openssl/ssl.h>
#include<stdio.h>

int main()
{
        BIO *sbio,*out;
        int len;
        char tmpbuf[1024];
        SSL_CTX* ctx;
        SSL* ssl;

        SSLeay_add_ssl_algorithms();
        OpenSSL_add_all_algorithms();
        ctx=SSL_CTX_new(SSLv23_client_method());
        sbio=BIO_new_ssl_connect(ctx);
        BIO_get_ssl(sbio,&ssl);
        if(!ssl)
        {
                fprintf(stderr,"Cannot locate SSL pointer\n");
                return 0;
        }
        SSL_set_mode(ssl,SSL_MODE_AUTO_RETRY);
        BIO_set_conn_hostname(sbio,"mybank.icbc.com.cn:https"); //这里可以改
        out=BIO_new_fp(stdout,BIO_NOCLOSE);
        BIO_printf(out,"Connecting......\n");
        if(BIO_do_connect(sbio)<=0)
        {
                fprintf(stderr,"Error connecting to server\n");
                return 0;
        }
        if(BIO_do_handshake(sbio)<=0)
        {
                fprintf(stderr,"Error establishing SSL connection\n");
                return 0;
        }
        BIO_puts(sbio,"GET/HTTP/1.0\n");
        for(;;)
        {
                len=BIO_read(sbio,tmpbuf,1024);
                if(len<=0) break;
                BIO_write(out,tmpbuf,len);
        }
        BIO_free_all(sbio);
        BIO_free(out);
        return 0;
}
```



# 附录

不同的BIO_METHOD对应的type取值：

```
#define BIO_TYPE_NONE 0
#define BIO_TYPE_MEM (1|0x0400)
#define BIO_TYPE_FILE (2|0x0400)
#define BIO_TYPE_FD (4|0x0400|0x0100)
#define BIO_TYPE_SOCKET (5|0x0400|0x0100)
#define BIO_TYPE_NULL (6|0x0400)
#define BIO_TYPE_SSL (7|0x0200)
#define BIO_TYPE_MD (8|0x0200)
#define BIO_TYPE_BUFFER (9|0x0200)
#define BIO_TYPE_CIPHER (10|0x0200)
#define BIO_TYPE_BASE64 (11|0x0200)
#define BIO_TYPE_CONNECT (12|0x0400|0x0100)
#define BIO_TYPE_ACCEPT (13|0x0400|0x0100)
#define BIO_TYPE_PROXY_CLIENT (14|0x0200)
#define BIO_TYPE_PROXY_SERVER (15|0x0200)
#define BIO_TYPE_NBIO_TEST (16|0x0200)
#define BIO_TYPE_NULL_FILTER (17|0x0200)
#define BIO_TYPE_BER (18|0x0200)
#define BIO_TYPE_BIO (19|0x0400)
#define BIO_TYPE_DESCRIPTOR 0x0100
#define BIO_TYPE_FILTER 0x0200
#define BIO_TYPE_SOURCE_SINK 0x0400
```



BIO的控制函数

```c++
#include <openssl/bio.h>
//最底层BIO控制函数，支持不同的命令输入从而产生不同的功能，一般无需调用
long   BIO_ctrl(BIO *bp,int cmd,long larg,void *parg); 

//上层BIO控制函数
char*  BIO_ptr_ctrl(BIO *bp,int cmd,long larg);
long   BIO_int_ctrl(BIO *bp,int cmd,long larg,int iarg);

//最上层BIO控制函数
int    BIO_reset(BIO *b); //将BIO状态初始化
int    BIO_seek(BIO *b, int ofs); //将文件相关的BIO的文件指针设置到距离开始位置ofs字节的位置上
int    BIO_tell(BIO *b); //返回文件相关BIO的当前文件指针位置
int    BIO_flush(BIO *b); //将BIO内部缓冲区的数据写出
int    BIO_eof(BIO *b); //如果BIO读取到EOF返回1，否则返回0
int    BIO_set_close(BIO *b,long flag); //设置BIO的关闭标志
int    BIO_get_close(BIO *b); //读取BIO的关闭标志
//下面四个函数都用于得到BIO中读缓存或写缓存中字符的数目
int    BIO_pending(BIO *b); 
int    BIO_wpending(BIO *b);
size_t BIO_ctrl_pending(BIO *b);
size_t BIO_ctrl_wpending(BIO *b);  

//最上层BIO控制函数的宏定义方式
#define BIO_reset(b)       (int)BIO_ctrl(b,BIO_CTRL_RESET,0,NULL)
#define BIO_seek(b,ofs)    (int)BIO_ctrl(b,BIO_C_FILE_SEEK,ofs,NULL)
#define BIO_tell(b)        (int)BIO_ctrl(b,BIO_C_FILE_TELL,0,NULL)
#define BIO_flush(b)       (int)BIO_ctrl(b,BIO_CTRL_FLUSH,0,NULL)
#define BIO_eof(b)         (int)BIO_ctrl(b,BIO_CTRL_EOF,0,NULL)
#define BIO_set_close(b,c) (int)BIO_ctrl(b,BIO_CTRL_SET_CLOSE,(c),NULL)
#define BIO_get_close(b)   (int)BIO_ctrl(b,BIO_CTRL_GET_CLOSE,0,NULL)
#define BIO_pending(b)     (int)BIO_ctrl(b,BIO_CTRL_PENDING,0,NULL)
#define BIO_wpending(b)    (int)BIO_ctrl(b,BIO_CTRL_WPENDING,0,NULL)

//最底层BIO回调函数控制函数，一般不需要调用。
long   BIO_callback_ctrl(BIO *b, int cmd, void (*fp)(struct bio_st *, int, const char *, int, long, long));
//上层BIO回调函数控制函数
#define BIO_set_callback(b,cb)      ((b)->callback=(cb)) //设置callback函数
#define BIO_get_callback(b)         ((b)->callback) //返回callback函数
#define BIO_set_callback_arg(b,arg) ((b)->cb_arg=(char *)(arg)) //设置callback函数的参数
#define BIO_get_callback_arg(b)     ((b)->cb_arg) //得到callback函数的参数

//其中，callback函数本身的声明在BIO结构体中。callback函数非常重要，许多控制功能需要用它来协助完成
/*函数参数的解释：
	b：callback函数对应的BIO
	oper：设置BIO将要执行的操作
	argp，argi，argl：根据oper定义操作的不同而不同，是相应操作中要使用到的参数
	retvalue：默认的callback函数返回值*/
typedef long callback(BIO *b, int oper, const char *argp,int argi, long argl, long retvalue);

//此外，还有一个用于调试目的的函数，其声明如下：
long BIO_debug_callback(BIO *bio,int cmd,const char *argp,int argi,long argl,long ret);
```

bio_type的定义：

```
#define BIO_TYPE_NONE 0
#define BIO_TYPE_MEM (1|0x0400)
#define BIO_TYPE_FILE (2|0x0400)
#define BIO_TYPE_FD (4|0x0400|0x0100)
#define BIO_TYPE_SOCKET (5|0x0400|0x0100)
#define BIO_TYPE_NULL (6|0x0400)
#define BIO_TYPE_SSL (7|0x0200)
#define BIO_TYPE_MD (8|0x0200)
#define BIO_TYPE_BUFFER (9|0x0200)
#define BIO_TYPE_CIPHER (10|0x0200)
#define BIO_TYPE_BASE64 (11|0x0200)
#define BIO_TYPE_CONNECT (12|0x0400|0x0100)
#define BIO_TYPE_ACCEPT (13|0x0400|0x0100)
#define BIO_TYPE_PROXY_CLIENT (14|0x0200)
#define BIO_TYPE_PROXY_SERVER (15|0x0200)
#define BIO_TYPE_NBIO_TEST (16|0x0200)
#define BIO_TYPE_NULL_FILTER (17|0x0200)
#define BIO_TYPE_BER (18|0x0200)
#define BIO_TYPE_BIO (19|0x0400)

#define BIO_TYPE_DESCRIPTOR 0x0100
#define BIO_TYPE_FILTER 0x0200
#define BIO_TYPE_SOURCE_SINK 0x0400
```



# 参考

[1] OpenSSL编程 赵春平

[2] https://blog.csdn.net/liao20081228/article/details/77193729/

