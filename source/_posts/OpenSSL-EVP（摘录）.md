---
title: OpenSSL-EVP（摘录）
tags: 
- 信息安全
- OpenSSL
categories: [信息安全, OpenSSL, 摘录]
sharing: True

---

注：本文内容摘自参考部分的相关资料



# 简介

Openssl EVP(high-level cryptographic functions[1])提供了丰富的密码学中的各种函数。Openssl 中实现了各种对称算法、摘要算法以及签名/验签算法。EVP 函数将这些具体的算法进行了封装。 官方建议使用这些算法时最好调用EVP的上层封装，不建议单独使用某个算法。

 EVP系列的函数的声明包含在”evp.h”里面，这是一系列封装了openssl加密库里面所有算法的函数。通过这样的统一的封装，使得只需要在初始化参数的时候做很少的改变，就可以使用相同的代码但采用不同的加密算法进行数据的加密和解密。 

EVP系列函数主要封装了**加密、摘要、编码**三大类型的算法，使用算法前需要调用OpenSSL_add_all_algorithms函数。

其中以**加密算法与摘要算法**为基本，公开密钥算法是对数据加密采用了对称加密算法，对密钥采用非对称加密（公钥加密，私钥解密）。数字签名是非对称算法（私钥签名，公钥认证）。 

<!--more-->

EVP封装的功能函数有：

- 实现了base64编解码BIO；
- 实现了加解密BIO；
- 实现了摘要BIO；
- 实现了reliable BIO；
- 封装了摘要算法；
- 封装了对称加解密算法；
- 封装了非对称密钥的加密(公钥)、解密(私钥)、签名与验证以及辅助函数；
- 基于口令的加密(PBE)；
- 对称密钥处理；
- 数字信封：数字信封用对方的公钥加密对称密钥，数据则用此对称密钥加密。发送给对方时，同时发送对称密钥密文和数据密文。接收方首先用自己的私钥解密密钥密文，得到对称密钥，然后用它解密数据。
- 其他辅助函数。



# 源码结构

evp源码位于crypto/evp目录，可以分为如下几类：

## 全局函数

主要包括 c_allc.c、c_alld.c、c_all.c 以及names.c。他们加载openssl  支持的所有的对称算法和摘要算法，放入到哈希表中。实现了OpenSSL_add_all_digests、OpenSSL_add_all_ciphers  以及OpenSSL_add_all_algorithms(调用了前两个函数)函数。在进行计算时，用户也可以单独加载摘要函数（EVP_add_digest）和对称计算函数（EVP_add_cipher）。

## BIO扩充

包括 bio_b64.c、bio_enc.c、bio_md.c 和bio_ok.c，各自实现了BIO_METHOD方法，分别用于base64 编解码、对称加解密以及摘要。

## 摘要算法封装

由 digest.c 实现，实现过程中调用了对应摘要算法的回调函数。各个摘要算法提供了自己的EVP_MD 静态结构，对应源码为m_xxx.c。

## 对称算法封装

由evp_enc.c 实现，实现过程调用了具体对称算法函数，实现了Update 操作。各种对称算法都提供了一个EVP_CIPHER  静态结构，对应源码为e_xxx.c。需要注意的是，e_xxx.c  中不提供完整的加解密运算，它只提供基本的对于一个block_size数据的计算，完整的计算由evp_enc.c  来实现。

想实现一个自己的对称算法时，可以参考e_xxx.c 的实现方式。一般至少需要实现如下功能：

- 构造一个新的静态的 EVP_CIPHER 结构；
- 实现 EVP_CIPHER 结构中的init 函数，该函数用于设置iv，设置加解密标记、以及根据外送密钥生成自己的内部密钥；
- 实现 do_cipher 函数，该函数仅对block_size 字节的数据进行对称运算；
- 实现 cleanup 函数，该函数主要用于清除内存中的密钥信息。

## 非对称算法封装

主要是以 p_开头的文件。其中，p_enc.c 封装了公钥加密；p_dec.c 封装了私钥解密；p_lib.c  实现一些辅助函数；p_sign.c 封装了签名函数；p_verify.c 封装了验签函数；p_seal.c 封装了数字信封；p_open.c  封装了解数字信封。

## 基于口令的加密

包括 p5_crpt2.c、p5_crpt.c 和evp_pbe.c。 



# 主要函数

## 对称加解密函数

- EVP_BytesToKey：计算密钥函数，它根据算法类型、摘要算法、salt 以及输入数据计算出一个对称密钥和初始化向量iv。
- PKCS5_PBE_keyivgen 和PKCS5_v2_PBE_keyivgen：实现了 PKCS5 基于口令生成密钥和初始化向量的算法。
- PKCS5_PBE_add：加载所有 openssl 实现的基于口令生成密钥的算法。
- EVP_PBE_alg_add：添加一个 PBE 算法。

## 其他函数

- EVP_add_cipher：将对称算法加入到全局变量，以供调用。
- EVP_add_digest：将摘要算法加入到全局变量中，以供调用。
- EVP_CIPHER_CTX_ctrl：对称算法控制函数，它调用了用户实现的ctrl 回调函数。
- EVP_CIPHER_CTX_set_key_length：当对称算法密钥长度为可变长时，设置对称算法的密钥长度。 
- EVP_CIPHER_CTX_set_padding：设置对称算法的填充，对称算法有时候会涉及填充。加密分组长度大于一时，用户输入数据不是加密分组的整数倍时，会涉及到填充。填充在最后一个分组来完成，openssl 分组填充时，如果有n 个填充，则将最后一个分组用n 来填满。
- EVP_CIPHER_get_asn1_iv：获取原始iv，存放在ASN1_TYPE 结构中。
- EVP_CIPHER_param_to_asn1：设置对称算法参数，参数存放在ASN1_TYPE 类型中，它调用用户实现的回调函数set_asn1_parameters 来实现。
- EVP_CIPHER_type：获取对称算法的类型。
- EVP_CipherInit/EVP_CipherInit_ex：对称算法计算(加/解密)初始化函数，_ex 函数多了硬件enginge 参数，EVP_EncryptInit 和EVP_DecryptInit 函数也调用本函数。
- EVP_CipherUpdate：对称计算（加/解密）函数，它调用了EVP_EncryptUpdate 和EVP_DecryptUpdate函数。
- EVP_CipherFinal/EVP_CipherFinal_ex：对称计算( 加/ 解) 函数， 调用了EVP_EncryptFinal(ex) 和EVP_DecryptFinal(ex)；本函数主要用来处理最后加密分组，可能会有对称计算。
- EVP_cleanup：清除加载的各种算法，包括对称算法、摘要算法以及PBE 算法，并清除这些算法相关的哈希表的内容。
- EVP_get_cipherbyname：根据字串名字来获取一种对称算法(EVP_CIPHER)，本函数查询对称算法哈希表。
- EVP_get_digestbyname：根据字串获取摘要算法(EVP_MD)，本函数查询摘要算法哈希表。
- EVP_get_pw_prompt：获取口令提示信息字符串.
- int EVP_PBE_CipherInit(ASN1_OBJECT *pbe_obj, const char *pass, int passlen, 
   ASN1_TYPE *param, EVP_CIPHER_CTX *ctx, int en_de)：PBE 初始化函数。本函数用口令生成对称算法的密钥和初始化向量，并作加/解密初始化操作。本函数再加上后续的EVP_CipherUpdate 以及EVP_CipherFinal_ex构成一个完整的加密过程（可参考crypto/p12_decr.c 的PKCS12_pbe_crypt 函数）.
- EVP_PBE_cleanup：删除所有的PBE 信息，释放全局堆栈中的信息.
- EVP_PKEY *EVP_PKCS82PKEY(PKCS8_PRIV_KEY_INFO *p8)：将PKCS8_PRIV_KEY_INFO(x509.h 中定义)中保存的私钥转换为EVP_PKEY结构。
- EVP_PKEY2PKCS8/EVP_PKEY2PKCS8_broken：将EVP_PKEY 结构中的私钥转换为PKCS8_PRIV_KEY_INFO 数据结构存储。
- EVP_PKEY_bits：非对称密钥大小，为比特数。
- EVP_PKEY_cmp_parameters：比较非对称密钥的密钥参数，用于DSA 和ECC 密钥。
- EVP_PKEY_copy_parameters：拷贝非对称密钥的密钥参数，用于DSA 和ECC 密钥。
- EVP_PKEY_free：释放非对称密钥数据结构。
- EVP_PKEY_get1_DH/EVP_PKEY_set1_DH：获取/设置EVP_PKEY 中的DH 密钥。
- EVP_PKEY_get1_DSA/EVP_PKEY_set1_DSA：获取/设置EVP_PKEY 中的DSA 密钥。
- EVP_PKEY_get1_RSA/EVP_PKEY_set1_RSA：获取/设置EVP_PKEY 中结构中的RSA 结构密钥。
- EVP_PKEY_missing_parameters：检查非对称密钥参数是否齐全，用于DSA 和ECC 密钥。
- EVP_PKEY_new：生成一个EVP_PKEY 结构。
- EVP_PKEY_size：获取非对称密钥的字节大小。
- EVP_PKEY_type：获取EVP_PKEY 中表示的非对称密钥的类型。
- int EVP_read_pw_string(char *buf,int length,const char *prompt,int verify)：获取用户输入的口令；buf 用来存放用户输入的口令，length 为buf 长度，prompt为提示给用户的信息，如果为空，它采用内置的提示信息，verify 为0 时，不要求验证用户输入的口令，否则回要求用户输入两遍。返回0 表示成功。
- EVP_set_pw_prompt：设置内置的提示信息，用于需要用户输入口令的场合。



# 对称加密

## 基本数据结构

EVP_CIPHER与EVP_CIPHER_CTX两个基本结构，加密函数EVP_Encrypt(EVP_Cipher)一些列函数都是以这两个结构为基础实现了。文件evp_enc.c是最高层的封装实现，，而各个e_*.c文件则是真正实现了各种算法的加解密功能，当然它们其实也是一些封装函数，真正的算法实现在各个算法同名目录里面的文件实现。 

注意:  EVP_CIPHER是、EVP_CIPHER_CTX的成员，在加密时通过指定的加密算法（其实就是加密函数），返回对应的EVP_CIPHER的指针，然后EVP_EncryptInit函数中 调用 EVP_CIPHER来初化EVP_CIPHER_CTX。

### EVP_CIPHER结构体

```cpp
#include<openssl/evp.h>

typedef struct evp_cipher_st
{
    int nid; //是算法类型的nid识别号，openssl里面每个对象都有一个内部唯一的识别ID
    int block_size;        //是每次加密的数据块的长度，以字节为单位
    int key_len;           //是每次加密的数据块的长度，以字节为单位
    int iv_len;            //初始化向量的长度
    unsigned long flags;   //标志位
    int (*init)(EVP_CIPHER_CTX *ctx, const unsigned char *key, const unsigned char *iv, int enc); //算法结构初始化函数，可以设置为加密模式还是解密模式
    int (*do_cipher)(EVP_CIPHER_CTX *ctx, unsigned char *out, const unsigned char *in, unsigned int inl);   //进行数据加密或解密的函数
    int (*cleanup)(EVP_CIPHER_CTX *);   //释放EVP_CIPHER_CTX结构里面的数据和设置
    int ctx_size;  //设定ctx->cipher_data数据的长度
    int (*set_asn1_parameters)(EVP_CIPHER_CTX *, ASN1_TYPE *);  // 在EVP_CIPHER_CTX结构中通过参数设置一个ASN1_TYPE
    int (*get_asn1_parameters)(EVP_CIPHER_CTX *, ASN1_TYPE *);  //从一个ASN1_TYPE中取得参数
    int (*ctrl)(EVP_CIPHER_CTX *, int type, int arg, void *ptr); //其它各种操作函数
    void *app_data;  //应用数据
}EVP_CIPHER;
```

### EVP_CIPHER_CTX结构体

```cpp
#include<openssl/evp.h>
typedef struct evp_cipher_ctx_st
{
    const EVP_CIPHER *cipher;  //是该结构相关的一个EVP_CIPHER算法结构
    ENGINE *engine;            //如果加密算法是ENGINE提供的，那么该成员保存了相关的函数接口
    int encrypt;               //加密或解密的标志
    int buf_len;               //该结构缓冲区里面当前的数据长度
    unsigned char oiv[EVP_MAX_IV_LENGTH];      //初始的初始化向量
    unsigned char iv[EVP_MAX_IV_LENGTH];       //工作时候使用的初始化向量
    unsigned char buf[EVP_MAX_BLOCK_LENGTH];   //保存下来的部分需要数据
    int num;                   //在cfb/ofb模式的时候指定块长度
    void *app_data;            //应用程序要处理数据
    int key_len;               //密钥长度，算法不一样长度也不一样
    unsigned long flags; 
    void *cipher_data;         //加密后的数据
    int final_used;
    int block_mask;
    unsigned char final[EVP_MAX_BLOCK_LENGTH];//
} EVP_CIPHER_CTX;
```

## 相关函数

### 核心函数

EVP_crypt系列函数只是对EVP_Cipher系列函数的调用，EVP_Encrypt函数相当于对EVP_Cipher函数enc参数置为1，EVP_Decrypt函数相当于对EVP_Cipher函数enc参数置为0。

#### 底层函数

```cpp
//旧版本
#include<openssl/evp.h>
int EVP_CipherInit(EVP_CIPHER_CTX *ctx, const EVP_CIPHER *cipher, const unsigned char *key, const unsigned char *iv, int enc)
int EVP_CipherUpdate(EVP_CIPHER_CTX *ctx, unsigned char *out, int *outl,const unsigned char *in, int inl)
int EVP_CipherFinal(EVP_CIPHER_CTX *ctx, unsigned char *out, int *outl)
//新版本
int EVP_CipherInit_ex(EVP_CIPHER_CTX *ctx, const EVP_CIPHER *cipher, 
ENGINE *impl, const unsigned char *key,const unsigned char *iv, int enc)
int EVP_CipherUpdate(EVP_CIPHER_CTX *ctx, unsigned char *out, int *outl, const unsigned char *in, int inl)
int EVP_CipherFinal_ex(EVP_CIPHER_CTX *ctx, unsigned char *out, int *outl)
```

这三个函数是更底层的函数。完成了数据的加密和解密功能。他们根据参数enc决定执行加密还是解密操作，如果enc为1，则加密；如果enc为0，则解密；如果enc是－1，则不改变数据。**函数都是操作成功返回1，否则返回0。**

**注意**：两个版本中：EVP_EncryptInit, EVP_DecryptInit和EVP_CipherInit，这三个函数的功能分别跟函数EVP_EncryptInit_ex, EVP_DecryptInit_ex和EVP_CipherInit_ex功能相同，只是他们的ctx参数不需要进行初始化，并且使用缺省的算法库。EVP_EncryptFinal,  EVP_DecryptFinal和EVP_CipherFinal，这三个函数分别跟函数EVP_EncryptFinal_ex,EVP_DecryptFinal_ex以及EVP_CipherFinal_ex函数功能相同，不过，他们的参数ctx会在调用后自动释放。

#### 加密

```cpp
//旧版本
int EVP_EncryptInit(EVP_CIPHER_CTX *ctx, const EVP_CIPHER *cipher, const unsigned char *key, const unsigned char *iv)
int EVP_EncryptUpdate(EVP_CIPHER_CTX *ctx, unsigned char *out, int *outl,const unsigned char *in, int inl)
int EVP_EncryptFinal(EVP_CIPHER_CTX *ctx, unsigned char *out, int *outl)
//新版本
int EVP_EncryptInit_ex(EVP_CIPHER_CTX *ctx, const EVP_CIPHER *cipher, ENGINE *impl, const unsigned char *key,const unsigned char *iv) //采用ENGINE参数impl的算法来设置并初始化加密结构体。其中ctx参数必须在调用之前被初始化完毕。如果参数impl为NULL，那么就会使用缺省的实现算法。参数key是用来加密的对称密钥，iv参数是初始化向量（如果需要的话）。在算法中真正使用的密钥长度和初始化密钥长度是根据算法来决定的（也就是传入的key或者iv长度可以是任意的，实际使用的数据取决于算法，不足会自动补上，超过会自动舍去）。在调用该函数进行初始化的时候，除了参数cipher之外，所有其它参数可以设置为NULL，留到以后调用其它函数的时候再提供
int EVP_EncryptUpdate(EVP_CIPHER_CTX *ctx, unsigned char *out, int *outl, const unsigned char *in, int inl) //该函数执行对数据的加密。该函数加密从参数in输入的长度为inl的数据，并将加密好的数据写入到参数out里面去。可以通过反复调用该函数来处理一个连续的数据块
int EVP_EncryptFinal_ex(EVP_CIPHER_CTX *ctx, unsigned char *out, int *outl) //该函数处理最后（Final）的一段数据。在函数在padding功能打开的时候（缺省）才有效，这时候它将剩余的最后的所有数据进行加密处理。该算法使用标志的块padding方式（AKA PKCS padding）， PKCS 填充（padding）标准是这样定义的，在被加密的数据后面加上n个值为n的字节，使得加密后的数据长度为加密块长度的整数倍。
```

#### 解密

```cpp
//旧版本
int EVP_DecryptInit(EVP_CIPHER_CTX *ctx, const EVP_CIPHER *cipher, const unsigned char *key, const unsigned char *iv)
int EVP_DecryptUpdate(EVP_CIPHER_CTX *ctx, unsigned char *out, int *outl, const unsigned char *in, int inl)
int EVP_DecryptFinal(EVP_CIPHER_CTX *ctx, unsigned char *out, int *outl)
//新版本
int EVP_DecryptInit_ex(EVP_CIPHER_CTX *ctx, const EVP_CIPHER *cipher,
ENGINE *impl, const unsigned char *key， const unsigned char *iv)  
int EVP_DecryptUpdate(EVP_CIPHER_CTX *ctx, unsigned char *out, int *outl, const unsigned char *in, int inl)                    
int EVP_DecryptFinal_ex(EVP_CIPHER_CTX *ctx, unsigned char *out, int *outl)
```

上面这些函数对应于上一节中的加密函数，函数的参数要求与相应的加密函数相同。

### 辅助函数

#### 操作EVP_CIPHER_CTX的函数

```cpp
int EVP_CIPHER_CTX_reset(EVP_CIPHER_CTX *c)    //重置EVP_CIPHER_CTX

EVP_CIPHER_CTX *EVP_CIPHER_CTX_new(void)       //开辟EVP_CIPHER_CTX
void EVP_CIPHER_CTX_free(EVP_CIPHER_CTX *ctx)  //销毁之前开辟的EVP_CIPHER_CTX

void EVP_CIPHER_CTX_init(EVP_CIPHER_CTX *a); //初始化一个EVP_CIPHER_CTX结构体   
int EVP_CIPHER_CTX_cleanup(EVP_CIPHER_CTX *a); //清除一个EVP_CIPHER_CTX结构体中的所有信息，并释放该结构占用的所有内存
```

#### 参数设置与获取

```cpp
#define EVP_MAX_IV_LENGTH 16
#define EVP_MAX_BOLCK_LENGTH 32
#define EVP_MAX_KEY_LENGTH 64

int is_partially_overlapping(const void *ptr1, const void *ptr2, int len)
int EVP_CIPHER_CTX_set_padding(EVP_CIPHER_CTX *x, int padding); //设置是否采用填充功能，默认使用。将padding设置为0代表禁止
int EVP_CIPHER_CTX_set_key_length(EVP_CIPHER_CTX *x, int keylen); //设置EVP_CIPHER_CTX密钥长度
int EVP_CIPHER_CTX_ctrl(EVP_CIPHER_CTX *ctx, int type, int arg, void *ptr); //设置不同算法特定的参数

/*这三个函数都根据给定的参数返回一个EVP_CIPHER结构。不同的是给定的参数分别是算法名称、算法的NID和一个ASN1_OBJECT结构。具体的算法名称、NID以及ASN1_OBJECT结构请参看object/object.h文件的定义。成功返回对应的EVP_CIPHER* ，失败返回NULL*/
const EVP_CIPHER *EVP_get_cipherbyname(const char *name); 
#define EVP_get_cipherbynid(a) EVP_get_cipherbyname(OBJ_nid2sn(a))
#define EVP_get_cipherbyobj(a) EVP_get_cipherbynid(OBJ_obj2nid(a))

/*下面这组函数的参数为EVP_CIPHER类型变量*/
#define EVP_CIPHER_nid(e)              ((e)->nid) //返回加密算法的nid
#define EVP_CIPHER_block_size(e)       ((e)->block_size) //返回加密块长度
#define EVP_CIPHER_key_length(e)       ((e)->key_len) //返回密钥长度
#define EVP_CIPHER_iv_length(e)        ((e)->iv_len) //返回初始化向量长度，如果没有使用IV则返回0
#define EVP_CIPHER_flags(e)            ((e)->flags) 
#define EVP_CIPHER_mode(e)             ((e)->flags) & EVP_CIPH_MODE) //返回相应结构算法的加密模式
int EVP_CIPHER_type(const EVP_CIPHER *ctx); //返回算法类型，返回值为算法的NID

/*下面这组函数的返回值与EVP_CIPHER*系列函数的返回值相同，只是函数参数为EVP_CIPHER_CTX类型变量*/
#define EVP_CIPHER_CTX_cipher(e)       ((e)->cipher) //返回EVP_CIPHER_CTX结构里面的EVP_CIPHER结构
#define EVP_CIPHER_CTX_nid(e)          ((e)->cipher->nid) 
#define EVP_CIPHER_CTX_block_size(e)   ((e)->cipher->block_size)
#define EVP_CIPHER_CTX_key_length(e)   ((e)->key_len)
#define EVP_CIPHER_CTX_iv_length(e)    ((e)->cipher->iv_len)
#define EVP_CIPHER_CTX_get_app_data(e) ((e)->app_data)
#define EVP_CIPHER_CTX_set_app_data(e,d) ((e)->app_data=(char *)(d))
#define EVP_CIPHER_CTX_type(c)         EVP_CIPHER_type(EVP_CIPHER_CTX_cipher(c)) 
#define EVP_CIPHER_CTX_flags(e)        ((e)->cipher->flags)
#define EVP_CIPHER_CTX_mode(e)         ((e)->cipher->flags & EVP_CIPH_MODE)

int EVP_CIPHER_param_to_asn1(EVP_CIPHER_CTX *c, ASN1_TYPE *type); //设置算法结构的参数，该函数必须在所设置的算法结构使用之前调用
int EVP_CIPHER_asn1_to_param(EVP_CIPHER_CTX *c, ASN1_TYPE *type); //用算法结构里面的值设置参数type的结构，其设置的内容由具体的算法决定。本函数应该在算法结构的基本算法类型已经设置了但是密钥还没有设置之前调用
```

### 算法函数

openssl对称加密算法的格式都以函数形式提供，其实该函数返回一个该算法的结构体，其形式一般如下(evp.h 、e\_···.c)： 
      `EVP_CIPHER* EVP_···(···代表加密算法的名称)(void)` 
在openssl中，所有提供的对称加密算法长度都是固定的，有特别说明的除外。

#### 分组加密的迭代模式

- ecb—电子密码本（Electronic Code Book)加密方式
- cbc—加密块链接（Cipher Block Chaining）加密方式
- cfb——加密反馈（Cipher Feedback）加密方式
- ofb—输出反馈（Output Feedback）加密方式
- ede—该加密算法采用了加密、解密、加密的方式，第一个密钥和最后一个密钥是相同的
- ede3—该加密算法采用了加密、解密、加密的方式，但是三个密钥都不相同

#### 加密算法

 **注意：下面这些加密算法函数调用时返回的都是对应EVP_CIPHER结构体指针**

- NULL算法：EVP_enc_null()，该算法不作任何事情，也就是没有进行加密处理
- DES算法：EVP_des_cbc(void), EVP_des_ecb(void), EVP_des_cfb(void), EVP_des_ofb(void) 
- 使用两个密钥的3DES算法：EVP_des_ede_cbc(void), EVP_des_ede(void), EVP_des_ede_ofb(void),EVP_des_ede_cfb(void) 
- 使用三个密钥的3DES算法：EVP_des_ede3_cbc(void), EVP_des_ede3(void), EVP_des_ede3_ofb(void), EVP_des_ede3_cfb(void) 
- DESX算法：EVP_desx_cbc(void) 
- RC2算法：EVP_rc2_cbc(void), EVP_rc2_ecb(void), EVP_rc2_cfb(void), EVP_rc2_ofb(void) ，该算法密钥长度可变，缺省为128位
- 定长的RC2算法：EVP_rc2_40_cbc(void), EVP_rc2_64_cbc(void) 
- RC4算法：EVP_rc4(void) ，密钥长度可变，默认为128位
- 定长的RC4算法：EVP_rc4_40(void) ，密钥长度为40位
- RC5算法：EVP_rc5_32_12_16_cbc(void), EVP_rc5_32_12_16_ecb(void), EVP_rc5_32_12_16_cfb(void), EVP_rc5_32_12_16_ofb(void)，密钥长度可以设置，默认128位。由于RC5算法本身实现代码的限制，加密次数只能设置为8、12或16。 
- IDEA算法：EVP_idea_cbc(void)，EVP_idea_ecb(void), EVP_idea_cfb(void), EVP_idea_ofb(void)
- Blowfish算法：EVP_bf_cbc(void), EVP_bf_ecb(void), EVP_bf_cfb(void), EVP_bf_ofb(void)，密钥长度可变
- CAST算法：EVP_cast5_cbc(void), EVP_cast5_ecb(void), EVP_cast5_cfb(void), EVP_cast5_ofb(void)，密钥长度可变
- AES算法：
  - 128位：EVP_aes_128_ecb(void)，EVP_aes_128_cbc(void)，PEVP_aes_128_cfb(void)，EVP_aes_128_ofb(void) 
  - 192位：EVP_aes_192_ecb(void)，EVP_aes_192_cbc(void)，PEVP_aes_192_cfb(void)，EVP_aes_192_ofb(void) 
  - 256位：EVP_aes_256_ecb(void)，EVP_aes_256_cbc(void)，PEVP_aes_256_cfb(void)，EVP_aes_256_ofb(void) 

## 应用流程

一般来说，EVP_Encrypt…系列函数的应用流程如下所描述(不失一般性，以加密算法3DES为例)：

1. 定义一些所需的变量

   一般情况下，输出数据缓冲区大小应该大于输入数据缓冲区。

   ```cpp
   char key[EVP_MAX_KEY_LENGTH];
   char iv[EVP_MAX_IV_LENGTH];
   EVP_CIPHER_CTX ctx;
   unsigned char out[512+8];
   int outl;
   ```

2. 给变量key和IV赋值

   可以借助函数`EVP_BytesToKey`来进行，如果有别的方法设定key和IV则该函数调用不是必须的

3. 初始化加密算法结构EVP_CIPHER_CTX

   ```cpp
   EVP_EncryptInit_ex(&ctx, EVP_des_ede3_cbc(), NULL, key, iv);
   ```

4. 进行数据加密操作

   ```cpp
   while (....)
   {
   	EVP_EncryptUpdate(ctx,out,&outl,in,inl);
   }
   ```

   **注意：EVP库的EVP_\*Update系列函数调用一次就能处理完指针in中的inl个字节数据。**这里所谓的循环是用于此类情景：每次收到若干字节放入指针in指向的缓冲区中，然后对其处理；或者每次从文件中读取若干字节到指针in所指缓冲区，再对其处理。**如果输入的数据不是整数倍，则会留到EVP\_\*\_CTX 中，等待下一次Update或EVP_Final\*来处理**，也就是**循环是用于无法一次传入所有数据的情况**，其他情况不需要使用循环。

5. 结束加密，输出最后一段数据

   ```cpp
   EVP_EncryptFinal_ex(&ctx, out, &outl)
   ```

   该函数会进行加密检测，如果加密过程有误会被检测出来

说明：解密过程与上述过程类似，只是要使用EVP_Decrypt···系列函数

## 程序示例

```cpp
#include<stdio.h>
#include<openssl/err.h>
#include<openssl/bio.h>
#include<openssl/evp.h>
#include<openssl/conf.h>
#include<cstring>

void handleErrors(void)
{
        ERR_print_errors_fp(stderr);
        abort();
}

int encrypt(unsigned char* plaintext, int plaintext_len, unsigned char* key, unsigned char* iv, unsigned char* ciphertext)
{
        EVP_CIPHER_CTX* ctx;
        int len;
        int ciphertext_len;
        if(!(ctx=EVP_CIPHER_CTX_new()))
                handleErrors();

        if(1!=EVP_EncryptInit_ex(ctx,EVP_aes_256_cbc(),NULL,key,iv))
                handleErrors();
        if(1!=EVP_EncryptUpdate(ctx,ciphertext,&len,plaintext,plaintext_len))
                handleErrors();
        ciphertext_len=len;

        if(1!=EVP_EncryptFinal_ex(ctx,ciphertext+len,&len))
                handleErrors();
        ciphertext_len+=len;

        EVP_CIPHER_CTX_free(ctx);
        return ciphertext_len;
}

int decrypt(unsigned char* ciphertext, int ciphertext_len, unsigned char* key, unsigned char* iv, unsigned char* plainte
xt)
{
        EVP_CIPHER_CTX* ctx;
        int len;
        int plaintext_len;

        if(!(ctx=EVP_CIPHER_CTX_new()))
                handleErrors();

        if(1!=EVP_DecryptInit_ex(ctx,EVP_aes_256_cbc(),NULL,key,iv))
                handleErrors();

        if(1!=EVP_DecryptUpdate(ctx,plaintext,&len,ciphertext,ciphertext_len))
                handleErrors();
        plaintext_len=len;

        if(1!=EVP_DecryptFinal_ex(ctx,plaintext+len,&len))
                handleErrors();
        plaintext_len+=len;

        EVP_CIPHER_CTX_free(ctx);

        return plaintext_len;
}


int main()
{
        unsigned char* key=(unsigned char*) "01234567890123456789012345678901";
        unsigned char* iv=(unsigned char*) "0123456789012345";
        unsigned char* plaintext=(unsigned char*) "Hello OpenSSL! This a demonstration program about how to use EVP"; //cbc模式下试着修改某一个字符，则会出现雪崩效应，密文出现很大差别；但是ofb和cfb却不会。（与这三种方式的计算顺序有关）
        unsigned char ciphertext[128];
        unsigned char decryptedtext[128];
        int decryptedtext_len, ciphertext_len;
        ciphertext_len=encrypt(plaintext,strlen((char*)plaintext),key,iv,ciphertext);

        printf("The ciphertext is:\n");
        BIO_dump_fp(stdout,(const char*)ciphertext,ciphertext_len);
        decryptedtext_len=decrypt(ciphertext,ciphertext_len,key,iv,decryptedtext);
        decryptedtext[decryptedtext_len]='\0';
        printf("Decrypted text is:\n");
        printf("%s\n",decryptedtext);

        return 0;
}
```

如果要用认证加密（防止CCA攻击），可以参考https://wiki.openssl.org/index.php/EVP_Authenticated_Encryption_and_Decryption



# 摘要

该系列函数封装了openssl加密库所有的信息摘要算法，通过这种EVP封装，当使用不同的信息摘要算法时，只需要对初始化参数修改一下就可以了，其它代码可以完全一样。这些算法包括MD2、MD5以及SHA等算法。

- 函数名称：EVP_Digest*…*
- 功能描述：该系列函数封装实现了多种信息摘要算法。
- 相关文件：digest.c,m_\*.c

## 基本数据结构

### EVP_MD结构体

所有的摘要算法都维护着指向下面定义的结构体的一个指针，在此基础上实现了算法的功能。该结构EVP_MD的源代码如下：

```cpp
#include<opessl/evp.h>
typedef struct env_md_st
{ 
    int type;     //信息摘要算法的NID标识
    int pkey_type;//是信息摘要-签名算法体制的相应NID标识，如NID_shaWithRSAEncryption
    int md_size;  //是信息摘要算法生成的信息摘要的长度，如SHA算法是SHA_DIGEST_LENGTH，该值是20
    unsigned long flags;
    int (*init)(EVP_MD_CTX *ctx); //指向一个特定信息摘要算法的初始化函数，如对于SHA算法，指针指向SHA_Init
    int (*update)(EVP_MD_CTX *ctx,const void *data,unsigned long count); //指向一个真正计算摘要值的函数，例如SHA算法就是指向SHA_Update
    int (*final)(EVP_MD_CTX *ctx,unsigned char *md); //指向一个信息摘要值计算之后要调用的函数，该函数完成最后的一块数据的处理工作。例如SHA算法就是指向SHA_Final.
    int (*copy)(EVP_MD_CTX *to,const EVP_MD_CTX *from); //指向一个可以在两个EVP_MD_CTX结构之间拷贝参数值的函数
    int (*cleanup)(EVP_MD_CTX *ctx);
    int (*sign)();   //签名
    int (*verify)(); //认证
    int required_pkey_type[5]; //指向一个用来签名的算法EVP_PKEY的类型，如SHA算法就指向EVP_PKEY_RSA_method
    int block_size;  //一个用来进行信息摘要的输入块的的长度（单位是字节），如SHA算法就是SHA_CBLOCK
    int ctx_size;    //是CTX结构的长度，在SHA算法里面应该就是sizeof(EVP_MD*)+sizeof(SHA_CTX)
} EVP_MD;
```

如果要增加新的算法，那么可以定义这个结构，并进行必要的移植，然后就可以使用通用的函数了。

### EVP_MD_CTX结构体

```cpp
typedef struct env_md_ctx_st
{
    const EVP_MD *digest;  //digest——指向上面介绍的EVP_MD结构的指针
    ENGINE *engine;        //如果算法由ENGINE提供，该指针指向该ENGINE
    unsigned long flags;   //
    void *md_data;         //信息摘要数据
}EVP_MD_CTX ;
```

## 相关函数

### 核心函数

```cpp
//旧版本
int EVP_DigestInit(EVP_MD_CTX *ctx, const EVP_MD *type)
int EVP_DigestUpdate(EVP_MD_CTX *ctx, const void *data, size_t count)
int EVP_DigestFinal(EVP_MD_CTX *ctx, unsigned char *md, unsigned int *size)

//新版本
int EVP_DigestInit_ex(EVP_MD_CTX *ctx, const EVP_MD *type, ENGINE *impl) //使用参数impl所指向的ENGINE设置该信息摘要结构体，参数ctx在调用本函数之前必须经过初始化。参数type一般是使用像EVP_sha1这样的函数的返回值。如果impl为NULL，那么就会使用缺省实现的信息摘要函数，大多数应用程序里面impl是设置为NULL的。操作成功返回1，否则返回0。
int EVP_DigestUpdate(EVP_MD_CTX *ctx, const void *data, size_t count) //将参数d中的cnt字节数据进行信息摘要到ctx结构中去，该函数可以被调用多次，用以对更多的数据进行信息摘要。操作成功返回1，否则返回0。
int EVP_DigestFinal_ex(EVP_MD_CTX *ctx, unsigned char *md, unsigned int *size) //将ctx结构中的摘要信息数据返回到参数md中，如果参数size不是NULL，那么摘要数据的长度（字节）就会被写入到参数size中，大多数情况下写入的值是EVP_MAX_MD_SIZE。在调用本函数后，不能使用相同的ctx结构调用EVP_DigestUpdate再进行数据的信息摘要操作,但是如果调用EVP_DigestInit_ex函数重新初始化后可以进行新的信息摘要操作。操作成功返回1，否则返回0。

//高级版本
int EVP_Digest(const void *data, size_t count, unsigned char *md, unsigned int *size, const EVP_MD *type,ENGINE *impl)
```

### 辅助函数

#### 操作EVP_MD_CTX的函数

```cpp
int EVP_MD_CTX_reset(EVP_MD_CTX *ctx)
EVP_MD_CTX *EVP_MD_CTX_new(void)
void EVP_MD_CTX_free(EVP_MD_CTX *ctx)
EVP_MD_CTX *EVP_MD_CTX_create(void); //创建一个EVP_MD_CTX结构，分配内存并进行初始化，返回该结构
void EVP_MD_CTX_destroy(EVP_MD_CTX *ctx);
void EVP_MD_CTX_init(EVP_MD_CTX *ctx); //初始化一个EVP_MD_CTX结构
int  EVP_MD_CTX_cleanup(EVP_MD_CTX *ctx); //清除一个EVP_MD_CTX结构，应该在不再需要使用之后再清除
int  EVP_MD_CTX_copy(EVP_MD_CTX *out, const EVP_MD_CTX *in) //与下面的函数作用相同，但是out不用初始化
int  EVP_MD_CTX_copy_ex(EVP_MD_CTX *out, const EVP_MD_CTX *in) //将信息摘要数据从in结构拷贝到out结构中，out必须在调用之前被初始化
int  EVP_MD_CTX_ctrl(EVP_MD_CTX *ctx, int cmd, int p1, void *p2)
```

#### 参数设置与获取函数

```cpp
#define EVP_MAX_MD_SIZE 64     /* SHA512 */
int EVP_MD_type(const EVP_MD *md); //返回信息摘要结构算法的NID
int EVP_MD_pkey_type(const EVP_MD *md); //返回信息摘要结构中公钥签名算法的NID
int EVP_MD_size(const EVP_MD *md); //返回结构里面摘要信息的长度
int EVP_MD_block_size(const EVP_MD *md); //返回摘要信息分块的长度

const EVP_MD *EVP_MD_CTX_md(const EVP_MD_CTX *ctx);
#define EVP_MD_CTX_size(e)        EVP_MD_size(EVP_MD_CTX_md(e)) //返回结构里面摘要信息的长度
#define EVP_MD_CTX_block_size(e)  EVP_MD_block_size((e)->digest) //返回摘要信息分块的长度
#define EVP_MD_CTX_type(e)        EVP_MD_type((e)->digest) //返回信息摘要结构算法的NID

const EVP_MD *EVP_get_digestbyname(const char *name); //根据给定算法名称返回一个EVP_MD结构
#define EVP_get_digestbynid(a) EVP_get_digestbyname(OBJ_nid2sn(a)) //根据算法的NID返回一个EVP_MD结构
#define EVP_get_digestbyobj(a) EVP_get_digestbynid(OBJ_obj2nid(a)) //根据算法的ASN1_OBJECT结构返回一个EVP_MD结构
```

#### 摘要算法函数

```cpp
const EVP_MD *EVP_md_null(void);
const EVP_MD *EVP_md2(void);
const EVP_MD *EVP_md4(void);
const EVP_MD *EVP_md5(void);

const EVP_MD *EVP_sha(void);
const EVP_MD *EVP_sha1(void);
const EVP_MD *EVP_sha224(void);
const EVP_MD *EVP_sha256(void);
const EVP_MD *EVP_sha384(void);
const EVP_MD *EVP_sha512(void);       

const EVP_MD *EVP_dss(void);
const EVP_MD *EVP_dss1(void);
const EVP_MD *EVP_ecdsa(void);
const EVP_MD *EVP_mdc2(void);
const EVP_MD *EVP_ripemd160(void);
const EVP_MD *EVP_whirlpool(void);
```

## 程序示例

```cpp
#include<stdio.h>
#include<openssl/err.h>
#include<openssl/bio.h>
#include<openssl/evp.h>
#include<openssl/conf.h>
#include<cstring>

void handleErrors(void)
{
        ERR_print_errors_fp(stderr);
        abort();
}

void digest_message(const unsigned char* message, size_t message_len, unsigned ch
ar** digest, unsigned int* digest_len)
{
        EVP_MD_CTX* mdctx;
        if((mdctx=EVP_MD_CTX_new())==NULL)
                handleErrors();
        if(1!=EVP_DigestInit_ex(mdctx,EVP_sha256(),NULL))
                handleErrors();
        if(1!=EVP_DigestUpdate(mdctx,message,message_len))
                handleErrors();
        if((*digest=(unsigned char*)OPENSSL_malloc(EVP_MD_size(EVP_sha256())))==N
ULL)
                handleErrors();
        if(1!=EVP_DigestFinal_ex(mdctx,*digest,digest_len))
                handleErrors();
        for(size_t i=0;i<(*digest_len);i++)
        {
                printf("%x",(*digest)[i]);
        }
        printf("\n");
        EVP_MD_CTX_free(mdctx);
}


int main()
{
        const unsigned char* message=(unsigned char*)"01234567890123456789";
        unsigned char** digest;
        unsigned int digest_len;
        digest_message(message,strlen((const char*)message),digest,&digest_len);
        return 0;
}
```



# 非对称加密

主要是以 p_开头的文件。其中:

-   p_enc.c 封装了公钥加密；
-   p_dec.c 封装了私钥解密；
-   p_lib.c 实现一些辅助函数；
-   p_sign.c 封装了签名函数；
-   p_verify.c 封装了验签函数；
-   p_seal.c 封装了数字信封相关的函数；
-   p_open.c 封装了解数字信封相关的函数。

## 基本数据结构

该结构用来存放非对称密钥信息，可以是RSA、DSA、DH 或ECC 密钥。其中，ptr 用来存放密钥结构地址，attributes 堆栈用来存放密钥属性。

```cpp
#include<openssl/evp.h>

typedef struct evp_pkey_st 
{
    int type;
    int save_type;
    int references;
    const EVP_PKEY_ASN1_METHOD *ameth;
    ENGINE *engine;
    union 
    {
        char *ptr;
        struct rsa_st *rsa;     /* RSA */
        struct dsa_st *dsa;     /* DSA */
        struct dh_st *dh;       /* DH */
        struct ec_key_st *ec;   /* ECC */
    } pkey;
    int save_parameters;
    STACK_OF(X509_ATTRIBUTE) *attributes; /* [ 0 ] */
}; EVP_PKEY
```

## 相关函数

### 核心函数

#### 加密

```cpp
int EVP_PKEY_encrypt_old(unsigned char *enc_key,const unsigned char *key, int key_len, EVP_PKEY *pubk) //使用RSA公钥加密

int EVP_PKEY_encrypt_init(EVP_PKEY_CTX *ctx); //使用密钥pkey对公钥算法的上下文进行初始化，以进行加密操作。返回1成功，0或负值失败，-2代表该公钥算法不支持此操作
int EVP_PKEY_encrypt(EVP_PKEY_CTX *ctx,unsigned char *out, size_t *outlen, const unsigned char *in, size_t inlen); //函数使用ctx执行公钥加密操作。使用in和inlen参数指定要加密的数据与数据长度。如果out为NULL，则输出缓冲区的最大大小写入outlen参数。如果out不为NULL，那么在调用之前，outlen参数应该包含out缓冲区的长度，如果调用成功，则将加密数据写入out，并将加密后的数据长度写入outlen。返回1成功，0或负值失败，-2表示该公钥算法不支持该操作
```

#### 解密

```cpp
int EVP_PKEY_decrypt_old(unsigned char *dec_key, const unsigned char *enc_key, int enc_key_len,EVP_PKEY *private_key);//RSA私钥解密

int EVP_PKEY_decrypt_init(EVP_PKEY_CTX *ctx); //函数使用密钥pkey初始化公钥算法的上下文以进行解密操作。返回1成功，0或负值失败，-2表示该公钥算法不支持该操作。
int EVP_PKEY_decrypt(EVP_PKEY_CTX *ctx,unsigned char *out, size_t *outlen, const unsigned char *in, size_t inlen); //函数使用ctx执行公钥解操作。使用in和inlen参数指定要解密的数据与数据字长。如果out为NULL，则输出缓冲区的最大大小写入outlen参数。如果out不为NULL，那么在调用之前，outlen参数应该包含out缓冲区的长度，如果调用成功，则将解密数据写入out，并将数据写入outlen。返回1成功，0或负值失败，-2表示该公钥算法不支持该操作。
```

### 辅助函数

```cpp
#include <openssl/evp.h>
EVP_PKEY_CTX *EVP_PKEY_CTX_new(EVP_PKEY *pkey, ENGINE *e); //使用pkey和ENGINE e中指定的算法分配公钥算法上下文，返回新分配的EVP_PKEY_CTX结构，如果出现错误返回NULL
EVP_PKEY_CTX *EVP_PKEY_CTX_new_id(int id, ENGINE *e); //使用由id和e指定的算法分配公钥算法上下文，返回新分配的EVP_PKEY_CTX结构，如果出现错误返回NULL
EVP_PKEY_CTX *EVP_PKEY_CTX_dup(EVP_PKEY_CTX *ctx); //复制上下文ctx，返回新分配的EVP_PKEY_CTX结构，如果出现错误返回NULL
void EVP_PKEY_CTX_free(EVP_PKEY_CTX *ctx); //释放上下文ctx

#include <openssl/evp.h>
int EVP_PKEY_CTX_ctrl(EVP_PKEY_CTX *ctx, int keytype, int optype,int cmd, int p1, void *p2); 
int EVP_PKEY_CTX_ctrl_str(EVP_PKEY_CTX *ctx, const char *type,const char *value);

#include <openssl/rsa.h>
int EVP_PKEY_CTX_set_signature_md(EVP_PKEY_CTX *ctx, const EVP_MD *md);

int EVP_PKEY_CTX_set_rsa_padding(EVP_PKEY_CTX *ctx, int pad);
int EVP_PKEY_CTX_set_rsa_pss_saltlen(EVP_PKEY_CTX *ctx, int len);
int EVP_PKEY_CTX_set_rsa_rsa_keygen_bits(EVP_PKEY_CTX *ctx, int mbits);
int EVP_PKEY_CTX_set_rsa_keygen_pubexp(EVP_PKEY_CTX *ctx, BIGNUM *pubexp);

#include <openssl/dsa.h>
int EVP_PKEY_CTX_set_dsa_paramgen_bits(EVP_PKEY_CTX *ctx, int nbits);

#include <openssl/dh.h>
int EVP_PKEY_CTX_set_dh_paramgen_prime_len(EVP_PKEY_CTX *ctx, int len);
int EVP_PKEY_CTX_set_dh_paramgen_generator(EVP_PKEY_CTX *ctx, int gen);

#include <openssl/ec.h>
int EVP_PKEY_CTX_set_ec_paramgen_curve_nid(EVP_PKEY_CTX *ctx, int nid);

#include <openssl/evp.h>
void EVP_PKEY_CTX_set_cb(EVP_PKEY_CTX *ctx, EVP_PKEY_gen_cb *cb);
EVP_PKEY_gen_cb *EVP_PKEY_CTX_get_cb(EVP_PKEY_CTX *ctx);

#include <openssl/evp.h>
//将pkey所指的EVP_PKEY的密钥设置为key所指的密钥，成功返回1，失败返回0
int EVP_PKEY_set1_RSA(EVP_PKEY *pkey,RSA *key);
int EVP_PKEY_set1_DSA(EVP_PKEY *pkey,DSA *key);
int EVP_PKEY_set1_DH(EVP_PKEY *pkey,DH *key);
int EVP_PKEY_set1_EC_KEY(EVP_PKEY *pkey,EC_KEY *key);

//从pkey所指的EVP_PKEY中获取对应的密钥，失败返回NULL 
RSA *EVP_PKEY_get1_RSA(EVP_PKEY *pkey);
DSA *EVP_PKEY_get1_DSA(EVP_PKEY *pkey);
DH *EVP_PKEY_get1_DH(EVP_PKEY *pkey);
EC_KEY *EVP_PKEY_get1_EC_KEY(EVP_PKEY *pkey);

//将pkey所指的EVP_PKEY的密钥设置为key所指的密钥，但pkey释放时，key也会被释放成功返回1，失败返回0
int EVP_PKEY_assign(EVP_PKEY,int type ,void *key) 
int EVP_PKEY_assign_RSA(EVP_PKEY *pkey,RSA *key);
int EVP_PKEY_assign_DSA(EVP_PKEY *pkey,DSA *key);
int EVP_PKEY_assign_DH(EVP_PKEY *pkey,DH *key);
int EVP_PKEY_assign_EC_KEY(EVP_PKEY *pkey,EC_KEY *key);

//返回与type匹配的密钥的类型，EVP_PKEY_RSA, EVP_PKEY_DSA, EVP_PKEY_DH or EVP_PKEY_EC或者NID_undef
int EVP_PKEY_type(int type);

int EVP_PKEY_missing_parameters(const EVP_PKEY *pkey);
int EVP_PKEY_copy_parameters(EVP_PKEY *to, const EVP_PKEY *from);

int EVP_PKEY_cmp_parameters(const EVP_PKEY *a, const EVP_PKEY *b);
int EVP_PKEY_cmp(const EVP_PKEY *a, const EVP_PKEY *b);

int EVP_PKEY_derive_init(EVP_PKEY_CTX *ctx);
int EVP_PKEY_derive_set_peer(EVP_PKEY_CTX *ctx, EVP_PKEY *peer);
int EVP_PKEY_derive(EVP_PKEY_CTX *ctx, unsigned char *key, size_t *keylen);

#include <openssl/evp.h>
int EVP_PKEY_get_default_digest_nid(EVP_PKEY *pkey, int *pnid);

int EVP_PKEY_keygen_init(EVP_PKEY_CTX *ctx);
int EVP_PKEY_keygen(EVP_PKEY_CTX *ctx, EVP_PKEY **ppkey);
int EVP_PKEY_paramgen_init(EVP_PKEY_CTX *ctx);
int EVP_PKEY_paramgen(EVP_PKEY_CTX *ctx, EVP_PKEY **ppkey);

typedef int EVP_PKEY_gen_cb(EVP_PKEY_CTX *ctx);

int EVP_PKEY_CTX_get_keygen_info(EVP_PKEY_CTX *ctx, int idx);

void EVP_PKEY_CTX_set_app_data(EVP_PKEY_CTX *ctx, void *data);
void *EVP_PKEY_CTX_get_app_data(EVP_PKEY_CTX *ctx);

EVP_PKEY *EVP_PKEY_new(void);
void EVP_PKEY_free(EVP_PKEY *key);  

int EVP_PKEY_print_public(BIO *out, const EVP_PKEY *pkey,int indent, ASN1_PCTX *pctx);
int EVP_PKEY_print_private(BIO *out, const EVP_PKEY *pkey,int indent, ASN1_PCTX *pctx);
int EVP_PKEY_print_params(BIO *out, const EVP_PKEY *pkey,int indent, ASN1_PCTX *pctx);

int EVP_PKEY_sign_init(EVP_PKEY_CTX *ctx);
int EVP_PKEY_sign(EVP_PKEY_CTX *ctx,unsigned char *sig, size_t *siglen, const unsigned char *tbs, size_t tbslen);

int EVP_PKEY_verify_init(EVP_PKEY_CTX *ctx);
int EVP_PKEY_verify(EVP_PKEY_CTX *ctx, const unsigned char *sig, size_t siglen, const unsigned char *tbs, size_t tbslen);

int EVP_PKEY_verify_recover_init(EVP_PKEY_CTX *ctx);
int EVP_PKEY_verify_recover(EVP_PKEY_CTX *ctx,unsigned char *rout, size_t *routlen, const unsigned char *sig, size_t siglen);

int EVP_PKEY_get_default_digest_nid(EVP_PKEY *pkey, int *pnid);
```

## 程序示例

### 参数初始化

```cpp
/* Create the context for generating the parameters */
if(!(pctx = EVP_PKEY_CTX_new_id(type, NULL))) goto err;
if(!EVP_PKEY_paramgen_init(pctx)) goto err;

/* Set the paramgen parameters according to the type */
switch(type)
{
case EVP_PKEY_EC:
  /* Use the NID_X9_62_prime256v1 named curve - defined in obj_mac.h */
  if(!EVP_PKEY_CTX_set_ec_paramgen_curve_nid(pctx, NID_X9_62_prime256v1)) goto err;		
  break;

case EVP_PKEY_DSA:
  /* Set a bit length of 2048 */
  if(!EVP_PKEY_CTX_set_dsa_paramgen_bits(pctx, 2048)) goto err;		
  break;

case EVP_PKEY_DH:
  /* Set a prime length of 2048 */
  if(!EVP_PKEY_CTX_set_dh_paramgen_prime_len(pctx, 2048)) goto err;
}

/* Generate parameters */
if (!EVP_PKEY_paramgen(pctx, &params)) goto err;
```

### 密钥生成

```cpp
if(*params != NULL)
{
  if(!(kctx = EVP_PKEY_CTX_new(params, NULL))) goto err; 
}
else
{
  /* Create context for the key generation */
  if(!(kctx = EVP_PKEY_CTX_new_id(type, NULL))) goto err;
}

if(!EVP_PKEY_keygen_init(kctx)) goto err; 

/* RSA keys set the key length during key generation rather than parameter generation! */
if(type == EVP_PKEY_RSA)
{
  if(!EVP_PKEY_CTX_set_rsa_keygen_bits(kctx, 2048)) goto err;
}

/* Generate the key */
if (!EVP_PKEY_keygen(kctx, &key)) goto err;
```



# BASE64编解码

## 原理

Base 64编码将二进制数据转换为使用字符 A-Z，a-z，0-9，“+”和“/”表示来数据的可打印形式。每3个字节的二进制数据，编码为上述字符表示的4字节数据。如果输入数据长度不是3的倍数，则输出数据将使用“=”字符在最后填充。

步骤：首先将每三个字节原始二进制数据在一起展开； 然后6bit分为一个小组，每个小组前面补两个0，成为一个字节；把新编码的每个字节转为十进制，根据base64标准转换表，找到对应的字符。 如果多了一个字节，则剩余两个字节用“=”填充，如果多了两个字节，则剩余一个字节用“=”填充。

## 基本数据结构

```cpp
#include<openssl/evp.h>

typedef struct evp_Encode_Ctx_st
{
     /* number saved in a partial encode/decode */
     int num;

     /* The length is either the output line length (in input bytes) or the shortest input line length that is ok.  Once decoding begins, the length is adjusted up each time a longer line is decoded */
     int length;
     unsigned char enc_data[80];    //待编码的数据

     int line_num;   /* number read on current line */
     int expect_nl;
} EVP_ENCODE_CTX;  
```

## 相关函数

### 核心函数

#### 编码

```cpp
#include <openssl/evp.h>

void EVP_EncodeInit(EVP_ENCODE_CTX *ctx); //初始化ctx以启动新的编码操作，无返回值
void EVP_EncodeUpdate(EVP_ENCODE_CTX *ctx, unsigned char *out, int *outl, const unsigned char *in, int inl); //编码in指向的缓冲区中的inl字节数据。输出存储在缓冲区out中，输出的字节数存储在outl中。调用者必须确保out指向的缓冲区足够大以容纳输出数据。只有完整的数据块（48字节）可以被直接编码完后并通过函数输出。任何剩余的字节都保存在ctx对象中，并通过后续调用EVP_EncodeUpdate（）EVP_EncodeFinal（）来处理。
void EVP_EncodeFinal(EVP_ENCODE_CTX *ctx, unsigned char *out, int *outl); //必须在编码操作结束时调用EVP_EncodeFinal（）。它将处理ctx对象中剩余的任何部分数据块。输出数据将被存储在out，输出的数据长度将存储在* outl中，包含了’\n’。无返回值

int  EVP_EncodeBlock(unsigned char *t, const unsigned char *f, int n); //对f中的输入数据进行编码，并将其存储在t中。对于每3字节的输入，将产生4字节的输出数据。如果n不能被3整除，则块被当做最后的数据块来编码，并且被填充，使得它总是可被除以4。另外还将添加‘\0’终结符字符。
```

#### 解码

```cpp
#include <openssl/evp.h>

void EVP_DecodeInit(EVP_ENCODE_CTX *ctx); //初始化ctx以开始新的解码操作
int  EVP_DecodeUpdate(EVP_ENCODE_CTX *ctx, unsigned char *out, int *outl, const unsigned char *in, int inl); //解码in指向的缓冲区中inl字节的数据。输出存储在缓冲区中out，输出的字节数存储在* outl中。调用者有责任确保out指向的缓冲区足够大以容纳输出数据。该功能将尝试在4字节块中尽可能多地解码数据。任何空格，换行符或回车符都将被忽略。任何保留在结尾的未处理数据（1,2或3个字节）的部分块将保留在ctx对象中，并由后续调用EVP_DecodeUpdate（）处理。如果遇到非法的base64字符，或者如果在数据中间遇到base64填充字符“=”，则函数返回-1表示错误。返回值为0或1表示数据成功处理。返回值0表示处理的最后输入数据字符包括base64填充字符“=”，预期不会再处理非填充字符数据
int  EVP_DecodeFinal(EVP_ENCODE_CTX *ctx, unsigned char *out, int *outl); //必须在解码操作结束时调用，如果仍然存在任何未处理的数据，那么输入数据不能是4的倍数。成功返回1，失败返回-1

int  EVP_DecodeBlock(unsigned char *t, const unsigned char *f, int n); //解码f中包含的基本64个数据的n个字节的块，并将结果存储在t中。任何前导空格，以及任何尾随的空格，换行符，回车符或EOF字符将被修剪。返回解码的数据长度，出错返回-1。
```

### 辅助函数

```cpp
EVP_ENCODE_CTX *EVP_ENCODE_CTX_new(void); //分配，初始化并返回要用于encode / decode函数的上下文。成功返回地址，失败返回NULL
void EVP_ENCODE_CTX_free(EVP_ENCODE_CTX *ctx); //清理编码/解码上下文ctx并释放分配给它的空间
int EVP_ENCODE_CTX_copy(EVP_ENCODE_CTX *dctx, EVP_ENCODE_CTX *sctx)
int EVP_ENCODE_CTX_num(EVP_ENCODE_CTX *ctx); //返回在ctx对象中待处理的尚未编码或解码的字节数
```

## 程序示例

```cpp
#include <string.h>
#include <openssl/evp.h>
int	main()
{
    EVP_ENCODE_CTX ectx,dctx;
    unsigned char in[500],out[800],d[500];
    int	inl,outl,i,total,ret,total2;

    EVP_EncodeInit(&ectx);
    for(i=0;i<500;i++)
        memset(&in[i],i,1);
    inl=500;
    total=0;
    EVP_EncodeUpdate(&ectx,out,&outl,in,inl);
    total+=outl;
    EVP_EncodeFinal(&ectx,out+total,&outl);
    total+=outl;
    printf("%s\n",out);

    EVP_DecodeInit(&dctx);
    outl=500;
    total2=0;
    ret=EVP_DecodeUpdate(&dctx,d,&outl,out,total);
    if(ret<0)
    {
        printf("EVP_DecodeUpdate err!\n");
        return -1;
    }
    total2+=outl;
    ret=EVP_DecodeFinal(&dctx,d,&outl);
    total2+=outl;
    return 0;
}
```



# HMAC

HMAC是基于散列函数的MAC（消息认证码），即用于消息认证的密钥哈希函数。

## 基本数据结构

```cpp
struct hmac_ctx_st 
{
    const EVP_MD *md;
    EVP_MD_CTX md_ctx;
    EVP_MD_CTX i_ctx;
    EVP_MD_CTX o_ctx;
    unsigned int key_length;
    unsigned char key[HMAC_MAX_MD_CBLOCK];
}/* HMAC_CTX*/;
```

## 相关函数

```cpp
unsigned char *HMAC(const EVP_MD *evp_md, const void *key,int key_len, const unsigned char *d, int n,unsigned char *md, unsigned int *md_len); //使用哈希函数evp_md和key_len字节长的密钥键计算d字节的消息认证码。它将结果放在md（它必须有空格的哈希函数的输出，不超过EVP_MAX_MD_SIZE字节）。如果md为NULL，则将摘要放置在静态数组中。输出的大小放在md_len中，除非它为空。evp_md可以为EVP_sha1()等

# define HMAC_MAX_MD_CBLOCK      128/* largest known is SHA512 */
# define HMAC_size(e)    (EVP_MD_size((e)->md)
void HMAC_CTX_init(HMAC_CTX *ctx); //HMAC_CTX_init()在首次使用前初始化HMAC_CTX。必须调用
void HMAC_CTX_cleanup(HMAC_CTX *ctx); //HMAC_CTX_cleanup()从HMAC_CTX中删除密钥和其他数据，并释放任何关联的资源。当不再需要HMAC_CTX时，必须调用它
size_t HMAC_size(const HMAC_CTX *e);
HMAC_CTX *HMAC_CTX_new(void);
int HMAC_CTX_reset(HMAC_CTX *ctx);
void HMAC_CTX_free(HMAC_CTX *ctx);

//如果消息没有完全存储在内存中，可能会使用以下功能：
int HMAC_Init_ex(HMAC_CTX *ctx, const void *key, int key_len,const EVP_MD *md, ENGINE *impl); //初始化或重用HMAC_CTX结构以使用函数evp_md和key，key可以是NULL，在这种情况下，现有的一个将被重用。 HMAC_CTX_init()必须在此功能首次使用HMAC_CTX之前被调用
int HMAC_Update(HMAC_CTX *ctx, const unsigned char *data, int len); //可以重复调用消息的大小块进行身份验证（数据中为len个字节）
int HMAC_Final(HMAC_CTX *ctx, unsigned char *md, unsigned int *len); //将消息认证码放在md中，它必须具有用于散列函数输出的空间
```

## 程序示例

### 生成

```cpp
int hmac_it(const unsigned char *msg, size_t mlen, unsigned char **val, size_t *vlen, EVP_PKEY *pkey)
{
    /* Returned to caller */
    int result = 0;
    EVP_MD_CTX* ctx = NULL;
    size_t req = 0;
    int rc;
    
    if(!msg || !mlen || !val || !pkey)
        return 0;
    
    *val = NULL;
    *vlen = 0;

    ctx = EVP_MD_CTX_new();
    if (ctx == NULL) {
        printf("EVP_MD_CTX_create failed, error 0x%lx\n", ERR_get_error());
        goto err;
    }
    
    rc = EVP_DigestSignInit(ctx, NULL, EVP_sha256(), NULL, pkey);
    if (rc != 1) {
        printf("EVP_DigestSignInit failed, error 0x%lx\n", ERR_get_error());
        goto err;
    }
    
    rc = EVP_DigestSignUpdate(ctx, msg, mlen);
    if (rc != 1) {
        printf("EVP_DigestSignUpdate failed, error 0x%lx\n", ERR_get_error());
        goto err;
    }
    
    rc = EVP_DigestSignFinal(ctx, NULL, &req);
    if (rc != 1) {
        printf("EVP_DigestSignFinal failed (1), error 0x%lx\n", ERR_get_error());
        goto err;
    }
    
    *val = OPENSSL_malloc(req);
    if (*val == NULL) {
        printf("OPENSSL_malloc failed, error 0x%lx\n", ERR_get_error());
        goto err;
    }
    
    *vlen = req;
    rc = EVP_DigestSignFinal(ctx, *val, vlen);
    if (rc != 1) {
        printf("EVP_DigestSignFinal failed (3), return code %d, error 0x%lx\n", rc, ERR_get_error());
        goto err;
    }
    
    result = 1;
    
   
 err:
    EVP_MD_CTX_free(ctx);
    if (!result) {
        OPENSSL_free(*val);
        *val = NULL;
    }
    return result;
}
```

### 验证

```cpp
int verify_it(const unsigned char *msg, size_t mlen, const unsigned char *val, size_t vlen, EVP_PKEY *pkey)
{
    /* Returned to caller */
    int result = 0;
    EVP_MD_CTX* ctx = NULL;
    unsigned char buff[EVP_MAX_MD_SIZE];
    size_t size;
    int rc;

    if(!msg || !mlen || !val || !vlen || !pkey)
        return 0;
    
    ctx = EVP_MD_CTX_new();
    if (ctx == NULL) {
        printf("EVP_MD_CTX_create failed, error 0x%lx\n", ERR_get_error());
        goto err;
    }
    
    rc = EVP_DigestSignInit(ctx, NULL, EVP_sha256(), NULL, pkey);
    if (rc != 1) {
        printf("EVP_DigestSignInit failed, error 0x%lx\n", ERR_get_error());
        goto err;
    }
    
    rc = EVP_DigestSignUpdate(ctx, msg, mlen);
    if (rc != 1) {
        printf("EVP_DigestSignUpdate failed, error 0x%lx\n", ERR_get_error());
        goto err;
    }
    
    size = sizeof(buff);
    rc = EVP_DigestSignFinal(ctx, buff, &size);
    if (rc != 1) {
        printf("EVP_DigestSignFinal failed, error 0x%lx\n", ERR_get_error());
        goto err;
    }
    
    result = (vlen == size) && (CRYPTO_memcmp(val, buff, size) == 0);
 err:
    EVP_MD_CTX_free(ctx);
    return result;
}
```



# 数字签名

## 签名

**EVP_Sign系列函数**使用的基础结构跟信息摘要算法使用的基础结构是一样的，而且，其前面的两个操作步骤初始化和数据操作（信息摘要）也跟信息摘要算法是一样的，唯一不一样的是最后一步操作，本系列函数做了签名的工作。本系列函数定义的如下(openssl/evp.h)：

```cpp
#define EVP_SignInit_ex(a,b,c) EVP_DigestInit_ex(a,b,c)
int EVP_SignInit_ex(EVP_MD_CTX *ctx, const EVP_MD *type, ENGINE *impl); //使用ENGINE参数impl所代表的实现函数功能来设置结构ctx。在调用本函数前，参数ctx一定要经过EVP_MD_CTX_init函数初始化。成功返回1，失败返回0。

#define EVP_SignUpdate(a,b,c) EVP_DigestUpdate(a,b,c)
int EVP_SignUpdate(EVP_MD_CTX *ctx, const void *d, unsigned int cnt); //将一个cnt字节的数据经过信息摘要运算存储到结构ctx中，该函数可以在一个相同的ctx中调用多次来实现对更多数据的信息摘要工作。成功返回1，失败返回0

int EVP_SignFinal(EVP_MD_CTX *ctx,unsigned char *sig,unsigned int *s, EVP_PKEY *pkey); //该函数是将签名操作的信息摘要结构先调用EVP_MD_CTX_copy_ex函数拷贝一份，然后调用EVP_DigestFinal_ex完成信息摘要工作，然后开始对摘要信息用私钥pkey调用EVP_PKEY_sign_init 和EVP_PKEY_sign进行签名,并将签名信息保存在参数sig里面。

#define EVP_SignInit(a,b) EVP_DigestInit(a,b)
void EVP_SignInit(EVP_MD_CTX *ctx, const EVP_MD *type); //使用缺省实现的算法初始化算法结构ctx
int EVP_PKEY_size(EVP_PKEY *pkey); //返回一个签名信息的最大长度（单位字节）。实际签名信息的长度则由上述的函数EVP_SignFinal返回，有可能比它小
```

## 认证

与EVP_Sign系列函数一样，EVP_Verify系列函数的前两步（初始化和信息摘要处理）和信息摘要算法是一样的

```cpp
#define EVP_VerifyInit_ex(a,b,c) EVP_DigestInit_ex(a,b,c)
int EVP_VerifyInit_ex(EVP_MD_CTX *ctx, const EVP_MD *type, ENGINE *impl); //该函数使用参数impl所提供的算法库对验证结构ctx进行设置。在调用本函数之前，参数ctx必须经过调用EVP_MD_CTX_init进行初始化。成功返回1，失败返回0。

#define EVP_VerifyUpdate(a,b,c) EVP_DigestUpdate(a,b,c)
int EVP_VerifyUpdate(EVP_MD_CTX *ctx, const void *d, unsigned int cnt); //该函数将参数d中的cnt字节数据经过信息摘要计算后保存到ctx中，该函数可以进行多次调用，以处理更多的数据。成功调用返回1，失败返回0。

int EVP_VerifyFinal(EVP_MD_CTX *ctx,unsigned char *sigbuf, unsigned int siglen,EVP_PKEY *pkey); //该函数先调用EVP_MD_CTX_copy_ex函数将原来的ctx拷贝一份，然后调用EVP_DigestFinal_ex函数完成拷贝的ctx的信息摘要计算，最后才使用公钥pkey调用EVP_PKEY_verify_init 和EVP_PKEY_verify_进行签名的验证工作

#define EVP_VerifyInit(a,b) EVP_DigestInit(a,b)
int EVP_VerifyInit(EVP_MD_CTX *ctx, const EVP_MD *type); //使用默认实现对ctx结构进行初始化
```

## 备注

在新版的OpenSSL中定义了`EVP_DigestSign*` 和 `EVP_DigestVerify*` 这两个系列的函数，将计算摘要和数字签名操作合在一起。这一系列函数的用法与不带Digest的系列函数类似，但是函数的参数稍有不同。

## 程序示例

### 签名

```cpp
EVP_MD_CTX *mdctx = NULL;
int ret = 0;
 
*sig = NULL;
 
/* Create the Message Digest Context */
if(!(mdctx = EVP_MD_CTX_create())) goto err;
 
/* Initialise the DigestSign operation - SHA-256 has been selected as the message digest function in this example */
if(1 != EVP_DigestSignInit(mdctx, NULL, EVP_sha256(), NULL, key)) goto err;

/* Call update with the message */
if(1 != EVP_DigestSignUpdate(mdctx, msg, strlen(msg))) goto err;

/* Finalise the DigestSign operation */
/* First call EVP_DigestSignFinal with a NULL sig parameter to obtain the length of the
* signature. Length is returned in slen */
if(1 != EVP_DigestSignFinal(mdctx, NULL, slen)) goto err;
/* Allocate memory for the signature based on size in slen */
if(!(*sig = OPENSSL_malloc(sizeof(unsigned char) * (*slen)))) goto err;
/* Obtain the signature */
if(1 != EVP_DigestSignFinal(mdctx, *sig, slen)) goto err;

/* Success */
ret = 1;

err:
if(ret != 1)
{
/* Do some error handling */
}

/* Clean up */
if(*sig && !ret) OPENSSL_free(*sig);
if(mdctx) EVP_MD_CTX_destroy(mdctx);
```

### 验证

```cpp
/* Initialize `key` with a public key */
if(1 != EVP_DigestVerifyInit(mdctx, NULL, EVP_sha256(), NULL, key)) goto err;

/* Initialize `key` with a public key */
if(1 != EVP_DigestVerifyUpdate(mdctx, msg, strlen(msg))) goto err;

if(1 == EVP_DigestVerifyFinal(mdctx, sig, slen))
{
    /* Success */
}
else
{
    /* Failure */
}
```



# 数字信封

## 写信

seal系列函数是相当于完成一个电子信封的功能，它产生一个随机密钥，然后使用一个公钥对该密钥进行封装，数据可以使用该随机密钥进行对称加密。 

信封加密在进行大量数据传输的时候是必须经常要用到的，因为公开密钥算法的加解密速度很慢，但对称算法就快多了。所以一般用公开密钥算法对产生的随机密钥加密，而真正进行数据加密则使用该随机密钥进行对称加密，然后将加密后的密钥与数据一起发送。 

其定义的函数如下：

```cpp
#include<openssl/evp.h>
int EVP_SealInit(EVP_CIPHER_CTX *ctx, EVP_CIPHER *type, unsigned char **ek, int *ekl, unsigned char *iv,EVP_PKEY **pubk, int npubk);//该函数初始化一个加密算法结构EVP_CIPHER_CTX，采用了指定的加密算法，使用一个随机密钥和初始化向量IV。事实上，该函数调用EVP_EncryptInit_ex函数两次完成了ctx结构的初始化工作；参数type是算法类型，参数ek是一个缓存序列，可以存放多个被公钥加密后的密钥的信息，所以每个缓存空间都应该足够大；参数pubk是一个公钥陈列，可以包含多个公钥。函数成功执行返回npubk，失败返回0。 

#define EVP_SealUpdate(a,b,c,d,e) EVP_EncryptUpdate(a,b,c,d,e)
int EVP_SealUpdate(EVP_CIPHER_CTX *ctx, unsigned char *out, int *outl, unsigned char *in, int inl); //其完成的功能和使用方法跟EVP_EncryptUpdate函数是一样的。成功执行返回1，否则返回0。

int EVP_SealFinal(EVP_CIPHER_CTX *ctx, unsigned char *out,int *outl); //简单调用EVP_EncryptFinal_ex完成其功能。该函数还调用EVP_EncryptInit_ex(ctx,NULL,NULL,NULL,NULL)函数对ctx结构再次进行了初始化。成功返回1，否则返回0。
```

## 读信

本系列函数将公钥加密了的密钥加密出来，然后进行数据的解密。其定义的函数如下：

```cpp
#include<openssl/evp.h>
int EVP_OpenInit(EVP_CIPHER_CTX *ctx,EVP_CIPHER *type,unsigned char *ek, int ekl,unsigned char *iv,EVP_PKEY *priv); //该函数初始化一个用来加密数据的ctx结构。它首先使用参数priv指定的私钥解密参数ek里面长度为ekl字节的加密密钥。然后用此密钥与参数iv指定的初始化向量初始化EVP_CIPHER_CTX。如果参数type设定的加密算法长度是可变的，那么密钥长度就会被设置为解密得到的密钥的长度；如果加密算法长度是固定的，那么得到的解密密钥的长度就必须跟固定算法长度相同才行。成功执行返回密钥的长度，否则返回0。 

#define EVP_OpenUpdate(a,b,c,d,e) EVP_DecryptUpdate(a,b,c,d,e)
int EVP_OpenUpdate(EVP_CIPHER_CTX *ctx, unsigned char *out, int *outl, unsigned char *in, int inl); //功能和使用方法与EVP_DecryptUpdate相同

int EVP_OpenFinal(EVP_CIPHER_CTX *ctx, unsigned char *out,int *outl); //调用EVP_DecryptFinal_ex完成了其功能，本函数还调用EVP_DecryptInit_ex(ctx,NULL,NULL,NULL,NULL)再次进行了初始化工作。成功执行返回1，否则返回0。
```

## 代码示例

### 写信

```cpp
int envelope_seal(EVP_PKEY **pub_key, unsigned char *plaintext, int plaintext_len,
	unsigned char **encrypted_key, int *encrypted_key_len, unsigned char *iv,
	unsigned char *ciphertext)
{
	EVP_CIPHER_CTX *ctx;

	int ciphertext_len;

	int len;

	/* Create and initialise the context */
	if(!(ctx = EVP_CIPHER_CTX_new())) handleErrors();

	/* Initialise the envelope seal operation. This operation generates
	 * a key for the provided cipher, and then encrypts that key a number
	 * of times (one for each public key provided in the pub_key array). In
	 * this example the array size is just one. This operation also
	 * generates an IV and places it in iv. */
	if(1 != EVP_SealInit(ctx, EVP_aes_256_cbc(), encrypted_key,
		encrypted_key_len, iv, pub_key, 1))
		handleErrors();

	/* Provide the message to be encrypted, and obtain the encrypted output.
	 * EVP_SealUpdate can be called multiple times if necessary
	 */
	if(1 != EVP_SealUpdate(ctx, ciphertext, &len, plaintext, plaintext_len))
		handleErrors();
	ciphertext_len = len;

	/* Finalise the encryption. Further ciphertext bytes may be written at
	 * this stage.
	 */
	if(1 != EVP_SealFinal(ctx, ciphertext + len, &len)) handleErrors();
	ciphertext_len += len;

	/* Clean up */
	EVP_CIPHER_CTX_free(ctx);

	return ciphertext_len;
}
```

### 读信

```cpp
int envelope_open(EVP_PKEY *priv_key, unsigned char *ciphertext, int ciphertext_len,
	unsigned char *encrypted_key, int encrypted_key_len, unsigned char *iv,
	unsigned char *plaintext)
{
	EVP_CIPHER_CTX *ctx;

	int len;

	int plaintext_len;

	/* Create and initialise the context */
	if(!(ctx = EVP_CIPHER_CTX_new())) handleErrors();

	/* Initialise the decryption operation. The asymmetric private key is
	 * provided and priv_key, whilst the encrypted session key is held in
	 * encrypted_key */
	if(1 != EVP_OpenInit(ctx, EVP_aes_256_cbc(), encrypted_key,
		encrypted_key_len, iv, priv_key))
		handleErrors();

	/* Provide the message to be decrypted, and obtain the plaintext output.
	 * EVP_OpenUpdate can be called multiple times if necessary
	 */
	if(1 != EVP_OpenUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len))
		handleErrors();
	plaintext_len = len;

	/* Finalise the decryption. Further plaintext bytes may be written at
	 * this stage.
	 */
	if(1 != EVP_OpenFinal(ctx, plaintext + len, &len)) handleErrors();
	plaintext_len += len;

	/* Clean up */
	EVP_CIPHER_CTX_free(ctx);

	return plaintext_len;
}
```



# 参考

1. https://blog.csdn.net/liao20081228/article/details/76285896
2. https://wiki.openssl.org/index.php/EVP
3. OpenSSL编程 赵春平



