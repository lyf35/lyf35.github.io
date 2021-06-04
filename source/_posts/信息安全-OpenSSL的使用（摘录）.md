---
title: 信息安全-OpenSSL的基本使用
tags: 
- 信息安全
- OpenSSL
categories: [工程实践, 信息安全]
sharing: True
date: 2020-10-15 13:20:36
updated: 2021-01-05 22:14:05
---

# 简介

OpenSSL是一个C语言实现的，功能丰富且自包含的安全工具箱，提供的主要功能包括SSL协议实现、加密解密算法、非对称算法的密钥生成、数字证书验证等。

OpenSSL Manual: https://www.openssl.org/docs/man1.1.1/man7/

# 安装与使用

在linux系统下面，可以使用apt-get命令安装OpenSSL和libssl-dev（这个必须安装，包含了lib和header等信息，安装之后才能在c/cpp程序中使用OpenSSL的函数）。

如果是简单地对文件做一些加密或者哈希等操作，只需要在命令行中执行openssl ……就可以，结果会直接打印在命令行内。

如果在c/cpp程序中使用了OpenSSL函数，编译的时候使用这一命令：g++ test.cpp -lssl -lcrypto，后两个编译选项是为了在编译的时候将OpenSSL的函数库链接进去。

<!--more-->

# Bash命令

注：本文的Bash命令部分来自于https://github.com/theno/openssl-examples/blob/master/links.md

## 查看OpenSSL是什么版本？

使用version选项

```
$ openssl version
OpenSSL 0.9.8zh 14 Jan 2016
```

跟上-a选项能得到更多的信息：

```
$ openssl versoin -a
OpenSSL 0.9.8zh 14 Jan 2016
built on: May 15 2016
platform: darwin64-x86_64-llvm
options:  bn(64,64) md2(int) rc4(ptr,char) des(idx,cisc,16,int) blowfish(idx)
compiler: -arch x86_64 -fmessage-length=0 -pipe -Wno-trigraphs -fpascal-strings -fasm-blocks -O3 -D_REENTRANT -DDSO_DLFCN -DHAVE_DLFCN_H -DL_ENDIAN -DMD32_REG_T=int -DOPENSSL_NO_IDEA -DOPENSSL_PIC -DOPENSSL_THREADS -DZLIB -mmacosx-version-min=10.6
OPENSSLDIR: "/System/Library/OpenSSL"
```

## 查看有哪些命令选项？

内置的查看命令(比如openssl list-standard-commands)只能查出一组，有个技巧是加上一个无效的选项(比如help或-h)，就可以得到所有的结果。

```
$ openssl help
openssl:Error: 'help' is an invalid command.

Standard commands
asn1parse      ca             ciphers        crl            crl2pkcs7
dgst           dh             dhparam        dsa            dsaparam
ec             ecparam        enc            engine         errstr
gendh          gendsa         genrsa         nseq           ocsp
passwd         pkcs12         pkcs7          pkcs8          prime
rand           req            rsa            rsautl         s_client
s_server       s_time         sess_id        smime          speed
spkac          verify         version        x509

Message Digest commands (see the `dgst' command for more details)
md2            md4            md5            mdc2           rmd160
sha            sha1

Cipher commands (see the `enc' command for more details)
aes-128-cbc    aes-128-ecb    aes-192-cbc    aes-192-ecb    aes-256-cbc
aes-256-ecb    base64         bf             bf-cbc         bf-cfb
bf-ecb         bf-ofb         cast           cast-cbc       cast5-cbc
cast5-cfb      cast5-ecb      cast5-ofb      des            des-cbc
des-cfb        des-ecb        des-ede        des-ede-cbc    des-ede-cfb
des-ede-ofb    des-ede3       des-ede3-cbc   des-ede3-cfb   des-ede3-ofb
des-ofb        des3           desx           rc2            rc2-40-cbc
rc2-64-cbc     rc2-cbc        rc2-cfb        rc2-ecb        rc2-ofb
rc4            rc4-40         seed           seed-cbc       seed-cfb
seed-ecb       seed-ofb
```

所谓“Standard commands”就是openssl后跟的第一个命令选项，可以使用同样的技巧查看有哪些可执行的子选项。

```
$ openssl dgst -h
unknown option '-h'
options are
-c              to output the digest with separating colons
-d              to output debug info
-hex            output as hex dump
-binary         output in binary form
-sign   file    sign digest using private key in file
-verify file    verify a signature using public key in file
-prverify file  verify a signature using private key in file
-keyform arg    key file format (PEM or ENGINE)
-signature file signature to verify
-binary         output in binary form
-hmac key       create hashed MAC with key
-engine e       use engine e, possibly a hardware device.
-md5            to use the md5 message digest algorithm (default)
-md4            to use the md4 message digest algorithm
-md2            to use the md2 message digest algorithm
-sha1           to use the sha1 message digest algorithm
-sha            to use the sha message digest algorithm
-sha224         to use the sha224 message digest algorithm
-sha256         to use the sha256 message digest algorithm
-sha384         to use the sha384 message digest algorithm
-sha512         to use the sha512 message digest algorithm
-mdc2           to use the mdc2 message digest algorithm
-ripemd160      to use the ripemd160 message digest algorithm
```

你也可是使用man openssl查看详细。

## 查看支持哪些算法？

使用ciphers选项，更多用法参见[ciphers的说明文档](https://www.openssl.org/docs/manmaster/apps/ciphers.html)。

```
# 列出所有可用的算法
openssl ciphers -v

# 列出符合TLSv1协议的算法
openssl ciphers -v -tls1

# 列出高级别加密的算法(密钥长度大于128位)
openssl ciphers -v 'HIGH'

# 列出使用了AES的高级别加密算法
openssl ciphers -v 'AES+HIGH'
```

## 性能测试

### 如何测试当前系统的运算性能？

OpenSSL的开发者在库里内置了测性能的套件，通过speed命令选项来使用。它测试在给定的时间内可以进行多少运算，而不是指定的运算需要多少时间。这样处理是很合理的，因此测试程序在性能好坏的系统上运行的时间不会相差太多。
 只使用speed选项，测试所有算法的性能。

```
openssl speed
```

有两组结果，第一组表示指定的算法每秒可以处理多少字节的数据，第二组表示一次签名／验证花费的时间。
 下面是在2.7GHZ Intel Xeon E5处理器上的测试结果：

```
The 'numbers' are in 1000s of bytes per second processed.
type             16 bytes     64 bytes    256 bytes   1024 	bytes   8192 bytes
md2               2540.48k     5184.66k     6989.57k     7651.67k     7872.51k
mdc2                 0.00         0.00         0.00         0.00         0.00
md4              83248.41k   261068.18k   624212.82k   940529.32k  1128846.68k
md5              62411.57k   184768.36k   408835.75k   586930.52k   678061.98k
hmac(md5)        48713.62k   148265.56k   359626.67k   563050.68k   670255.79k
sha1             68829.72k   195087.40k   431001.51k   623344.42k   729505.79k
rmd160           38598.59k    96226.86k   183336.45k   235962.71k   257526.44k
rc4             480093.57k   678565.35k   783765.42k   818297.51k   838205.99k
des cbc          69500.17k    71184.75k    71491.50k    71641.77k    72010.15k
des ede3         26433.63k    26717.01k    26772.99k    26788.18k    26907.57k
idea cbc         95690.28k    99334.17k   100835.40k   100787.54k   100900.86k
seed cbc         76871.40k    77238.46k    77736.50k    77452.97k    77545.47k
rc2 cbc          48984.63k    49589.03k    50188.07k    50103.98k    50066.77k
rc5-32/12 cbc        0.00         0.00         0.00         0.00         0.00
blowfish cbc    122583.30k   129550.92k   130876.67k   131111.94k   131394.22k
cast cbc        109471.38k   114523.31k   115934.46k   116200.45k   116331.86k
aes-128 cbc     128352.23k   138604.76k   141173.42k   142832.25k   142682.79k
aes-192 cbc     107703.93k   114456.79k   117716.65k   118847.36k   118784.00k
aes-256 cbc      93374.87k    99521.51k   101198.51k   101382.49k   101635.41k
camellia-128 cbc    99270.57k   150412.42k   170346.33k   176311.91k   177913.86k
camellia-192 cbc    85896.60k   117356.52k   128556.97k   132759.72k   133425.83k
camellia-256 cbc    87351.27k   117695.15k   128972.03k   132130.47k   133455.87k
sha256           52372.61k   117766.12k   204825.69k   249974.10k   270914.90k
sha512           41278.19k   165820.37k   258298.69k   365981.70k   419864.58k
whirlpool        24803.02k    53047.07k    87593.90k   104570.54k   111159.98k
aes-128 ige     128441.31k   132981.88k   133269.08k   133738.15k   133966.51k
aes-192 ige     107831.37k   111507.07k   111800.66k   112156.67k   112219.48k
aes-256 ige      94382.07k    96351.17k    96750.68k    96958.46k    97446.44k
ghash           888644.92k  1452788.80k  1696788.74k  1763055.96k  1799086.49k
              	sign    verify    sign/s verify/s
rsa  512 bits 0.000049s 0.000004s  20547.1 248266.2
rsa 1024 bits 0.000194s 0.000011s   5146.0  90735.4
rsa 2048 bits 0.001194s 0.000037s    837.3  27277.1
rsa 4096 bits 0.008560s 0.000137s    116.8   7324.5
              	sign    verify    sign/s verify/s
dsa  512 bits 0.000048s 0.000046s  20667.7  21701.8
dsa 1024 bits 0.000113s 0.000126s   8831.9   7951.8
dsa 2048 bits 0.000362s 0.000430s   2762.0   2322.9
                          	sign    verify    sign/s verify/s
256 bit ecdsa (nistp256)   0.0001s   0.0004s   9856.1   2524.4
384 bit ecdsa (nistp384)   0.0002s   0.0008s   5103.6   1191.7
521 bit ecdsa (nistp521)   0.0004s   0.0018s   2679.0    550.3
                          	op      op/s
256 bit ecdh (nistp256)   0.0003s   3063.8
384 bit ecdh (nistp384)   0.0007s   1447.3
521 bit ecdh (nistp521)   0.0015s    666.2
```

也可以只测试指定的算法：

```
# 测试RSA的速度
openssl speed rsa

# 在多核系统中并行两组测试
openssl speed rsa -multi 2
```

### 如何测试远程连接的耗时？

使用s_time选项可以测试连接性能，直接调用会运行30秒，使用任意的加密算法，统计每秒钟通过SSL握手建立的连接数，结果分为新会话和重用会话两组：

```
openssl s_time -connect romete.host:443
# 译者注：可以拿百度来测试
openssl s_time -connect baidu.com:443
```

除了直接调用外，s_time选项也提供了很多测试的子选项

```
＃ 只通过新会话获取远端test.html页面
openssl s_time -connect remote.host:443 -www /test.html -new

# 使用SSL v3和高级别加密算法
openssl s_time \
	-connect remote.hot:443 -www /test.html -new \
	-ssl3 -cipher HIGH
	
# 测试不同加密算法的性能，每种算法测试10秒
IFS=":"
for c in $(openssl ciphers -ssl3 RSA); do
	echo $C
	openssl s_time -connect remote.host:443 \
		-www / -new -time 10 -cipher $c 2>&1 | \
		grep bytes
	echo
done
```

如果你没有可测的启用SSL的服务端，可以使用s_server选项来创建一个。

```
＃ 在一台主机上创建服务端，默认使用4433端口
openssl s_server -cert mycert.pem -www

# 在另一台主机上(可以跟服务端同一台)，运行s_time
openssl s_time -connect myhost:4433 -www / -new -ssl3
```

## 证书

### 如何生成自签名的证书？

首先考虑是否要对私钥进行加密。
 加密私钥的好处是更加安全，有人拿走了也无法使用。坏处是需要把密码保存在某个文件或每次启动服务时手动输入。
 我个人倾向不加密私钥，这样不用每次手动输入。（当你厌烦输入时，要知道去除密码保护也是可以的。）
 下面的例子会生成一个mycert.pem证书文件，其中证书的有效期是365天，私钥不加密(-ndoes决定的)。

```
openssl req \
	-x509 -nodes -days 365 -sha256 \
	-newkey rsa:2048 -keyout mycert.pem -out mycert.pem
```

这个文件中有两部分信息：RSA私钥和证书。

```
cat mycert.pem
-----BEGIN RSA PRIVATE KEY-----
...
-----END RSA PRIVATE KEY-----
-----BEGIN CERTIFICATE-----
...
-----END CERTIFICATE-----
```

证书部分包含公钥。

```
openssl x509 -in mycert.pem -pubkey -noout
-----BEGIN PUBLIC KEY-----
...
-----END PUBLIC KEY-----
```

使用这个指令后，会要求你回答一系列的问题：国家，省市，公司，职位等等。当回答“Common  Name”这个问题要注意点，你可能会填写主机名或域名，如果服务器的真实地址是mybox.mydomain.com，但用户是通过www.mydomain.com来访问的，那么你应填写用户访问的地址。
 可以通过-subj选项来直接写入这些回答，对生成证书来说惟一有用的就是Common Name(CN)字段。

```
openssl req \
	-x509 -nodes -days 365 -sha256 \
	-subj '/C=US/ST=Oregon/L=Portland/CN=www.madboa.com' \
	-newkey rsa:2048 -keyout mycert.pem -out mycert.pem
```

### 如何生成VeriSign的证书请求？

向认证机构(比如VeriSign)申请签名证书是相当繁杂和官僚的，在创建证书请求前要先准备好需要的书面文件(营业执照等)。
 上一节说到，你需要考虑是否加密私钥，下面的例子假设不加密，最后会得到两个文件：私钥文件mykey.pem，证书请求文件myreq.pem。

```
openssl req \
	-new -sha256 -newkey rsa:2048 -nodes \
	-keyout mykey.pem -out myreq.pem
```

如果是使用已有的私钥文件生成证书请求，语法更简单点：

```
openssl req -new -key mykey.pem -out myreq.pem
```

同样的，你也可以在命令中直接写入要回答的问题：

```
openssl req \
	-new -sha256 -newkey rsa:2048 -nodes \
	-subj '/CN=www.mydom.com/O=My Dom. Inc./C=US/ST=Oregon/L=Portland' \
	-keyout mykey.pem -out myreq.pem
```

在跟像VeriSign这样的机构打交道时，要特别注意在创建证书请求时填写的信息。根据我个人的经验，比如填写组织名字时用and代替&，都会造成申请不通过。
 需要仔细确认证书请求中的签名、组织等信息。

```
# 验证签名
openssl req -in myreq.pem -noout -verify -key mykey.pem
# 检查信息
openssl req -in myreq.pem -noout -text
```

将私钥文件保存在安全的地方，要使用VeriSign发回给你证书必须要用到它。证书请求一般是通过VeriSign的线上表单提交。

### 如何测试一个新证书？

s_server选项提供了一个简单有效的测试方法。下面的例子假设你已经把私钥和证书合并为一个文件(也可以是上面生成的自签名证书)：mycert.pem。
 首先启动测试服务端，默认使用4433端口，可以使用-accept选项修改。

```
openssl s_server -cert mycert.pem -www
```

如果服务端正常启动，没有报错信息，说明证书就可以使用了。
 可以在浏览器里访问测试服务器https://yourserver:4433/。注意是https，打开这个界面可以看到可用的加密算法以及连接相关的统计。如果是自签名的证书，浏览器会提示你是否相信此证书。
 怎么得到远端的证书？
 只要配合使用openssl和sed命令，就能取到远端的证书信息。

```
#!/bin/sh
#
# usage: retrieve-cert.sh remote.host.name [port]
#
REMHOST=$1
REMPORT=${2:-443}

echo |\
openssl s_client -connect ${REMHOST}:${REMPORT} 2>&1 |\
sed -ne '/-BEGIN CERTIFICATE-/,/-END CERTIFICATE-/p'
```

也可以把得到信息再交给openssl处理，比如检查证书的日期：

```
#!/bin/sh
#
for CERT in \
	www.yourdomain.com:443 \
	ldap.yourdomain.com:636 \
	imap.yourdomain.com:993
do
	echo |\
	openssl s_client -connect ${CERT} 2>/dev/null |\
	sed -ne '/-BEGIN CERTIFICATE-/,/-END CERTIFICATE-/p' |\
	openssl x509 -noout -subject -dates
done
```

怎么查看证书中的信息？
 一个SSL证书包含很多信息：颁发者，有效期，持有者，算法，公钥等。使用x509选项可以查看这些信息，下面的例子假设有一个名为cert.pem的证书文件。
 使用-text选项能同时以可读文本的形式展示信息。

```
openssl x509 -text -in cert.pem
```

还有一些选项可以指定展示某项信息。

```
# 查看颁发者
openssl x509 -noout -in cert.pem -issuer

# 查看持有者
openssl x509 -noout -in cert.pem -subject

# 有效期
openssl x509 -noout -in cert.pem -dates

# 以上全部
openssl x509 -noout -in cert.pem -issuer -subject -dates

# 哈希值
openssl x509 -noout -in cert.pem -hash

# MD5指纹
openssl x509 -noout -in cert.pem -fingerprint
```

### 如何导入／导出PKCS#12格式的证书？

PKCS#12格式的证书被许多程序使用，比如微软的IIS，这种证书文件的后缀通常是.pfx。
 创建PKCS#12格式的证书，需要私钥和证书。转换时，你会被要求输入密码(可以为空)来对证书进行加密。

```
# 创建一个包含私钥的自签名证书
openssl req \
	-x509 -sha256 -nodes -days 365 \
	-newkey rsa:2048 -keyout mycert.pem -out mycert.pem
	
# 将mycert.pem导出为PKCS#12格式的证书，mycert.pfx
openssl pkcs12 -export \
	-out mycert.pfx -in mycert.pem \
	-name "My Certificate"
```

如果你拿到PKCS#12格式的证书和它的密码(如果有的话)，可以把它导出为标准的PEM格式。

```
# 无密码情况下
openssl pkcs12 -in mycert.pfx -out mycert.pem -nodes

# 有密码情况下
openssl pkcs12 -in mycert.pfx -out mycert.pem
```

## 证书验证

使用OpenSSL库可以验证授权机构颁发的证书

### 如何验证证书？

使用verify选项来验证证书。

```
openssl verify cert.pem
```

验证成功，会返回“OK”。

```
$ openssl verify mycert.pem
mycert.pem: /C=CN/ST=JS/L=WX/O=Internet Widgits Pty Ltd/CN=Calle Zhang
error 18 at 0 depth lookup:self signed certificate
OK
```

如果异常会看到错误消息，比如

```
证书过期，通常证书都是有有效期的，一般是一年：
error 10 at 0 depth lookup:certificate has expired

自签名证书：
error 18 at 0 depth lookup:self signed certificate
```

### OpenSSL都认可哪些证书颁发机构？

在编译OpenSSL库时需要制定OpenSSL的文件目录(--openssldir参数指定)，这个目录保存着认可的证书颁发机构信息。
 默认的目录是/usr/local/ssl，很多操作系统会把它放到别处，比如/usr/share/ssl(Red  Hat/Fedora)，/etc/ssl(Gentoo)，/usr/lib/ssl(Debian)，/System/Library/OpenSSL(Macintosh OS X)。
 使用version选项加上-d查看目录在什么位置

```
openssl version -d
```

这个目录下有个certs子目录，在certs里可能会有三种类型的文件：

1. 有一个名为cert.pem的大文件，里面包含了许多认可机构(比如VeriSign，Thawte)的证书。

2. 一些以.pem为后缀的小文件，每个文件包含一个认可机构的一个证书。

3. 一些奇怪名字的链接文件，比如052eae11.0，通常是指向.pem文件。这个文件名字的第一部分其实是.pem文件里证书的hash值，后缀是序列号，因为多个链接文件可以指向同一证书。以我的Gentoo系统为例，有个指向vsignss.pem的链接文件叫f73e89fd.0，可以肯定里面证书的哈希值就是这个文件名：

   $ openssl x509 -noout -hash -in vsingss.pem f73e89fd 当OpenSSL要验证一个证书，它会查看cert.pem里面是否包含这个证书，如果没有再查看这个目录下是否存在以证书哈希值为名的文件，如果存在就验证通过。有意思的是，有些程序(比如Sendmail)会在运行时让你指定你信任证书的存储位置。

### 如何让OpenSSL信任某个证书？

把这个证书放入上文说的certs目录下，然后创建一个以哈希值命名的链接文件，下面的脚本可以完成这样的任务：

```
#!/bin/sh
#
# usage: certlink.sh filename [filename ...]

for CERTFILE in $*; do
	# make sure file exists and is a valid cert
	test -f "$CERTFILE" || continue
	HASH=$(openssl x509 -noout -hash -in "$CERTFILE")
	test -n "$HASH" || continue

	# use lowest available iterator for symlink
	for ITER in 0 1 2 3 4 5 6 7 8 9; do
		test -f "${HASH}.${ITER}" && continue
		ln -s "$CERTFILE" "${HASH}.${ITER}"
		test -L "${HASH}.${ITER}" && break
	done
done
```

## 命令行客户端和服务端

s_client和s_server选项可以启动使用了SSL的命令行客户端和服务端。
 这部分，假设你已经熟悉这些常见的协议：SMTP，HTTP等。

### 如何与SMTP服务建立安全连接？

你可以通过s_client选项测试甚至使用一个使用SSL的SMTP服务端。
 SMTP服务的安全连接使用3个端口：25(TLS)，465(SSL)和587(TLS)。OpenSSL大概是在0.9.7版本之后提供了-starttls选项：

```
# 25(TLS)端口，587(TLS)端口也是一样的
openssl s_client -connect remote.host:25 -starttls smtp

# 465(SSL)端口
openssl s_client -connect remote.host:456
```

[RFC821](http://www.ietf.org/rfc/rfc0821.txt)建议使用作为行结束符。虽然大多数的邮件服务都是以或作为行结束符的，但Qmail就不是。如果你想遵从RFC821或是要与Qmail通信，可以使用-crlf选项：

```
openssl s_client -connect remote.host:25 -crlf -starttls smtp
```

### 如何其它服务建立安全连接？

与其它服务建立安全连接本质上与上文是相同的操作。截止本文的写作日期，OpenSSL只支持命令行通过TLS连接SMTP服务，如果想连接其它服务，就只能直接使用SSL连接了:

```
# HTTPS
openssl s_client -connect remote.host:443

# LDAPS
openssl s_client -connect remote.host:636

# IMAPS
openssl s_client -connect remote.host:993

# POP3S
openssl s_client -connect remote.host:995
```

### 如何使用命令行搭建一个SSL服务端？

s_server选项可以搭建一个SSL服务端，仅限测试、调试时使用，如果要在生产环境启用安全连接，可以考虑使用[Stunnel](https://www.stunnel.org)。
 有证书后就可以通过s_server选项搭建服务了，可以搭建多个

```
# -www选项表示，服务端返回一个OpenSSL运行状态的HTML页面
openssl s_server -cert mycert.pem -www

# 使用-WWW选项，可以访问当前目录下的文件。指定了443端口(默认4433端口)。
openssl s_server -accept 443 -cert mycert.pem -WWW
```

## 哈希运算

在OpenSSL库中，计算哈希通过dgst选项来操作。哈希运算十分常见，都有专门计算哈希的工具。

### 如何计算一个文件的MD5或SHA1哈希值？

使用dgst选项，先使用openssl dgst -h命令来查看有哪些可用的哈希算法。

```
# MD5
openssl dgst -md5 filename

# SHA1
openssl dgst -sha1 filename

# SHA256
openssl dgst -sha256 filename
```

### 如何签名一个哈希值？

如果想确保得到的哈希值不被篡改，可以使用私钥对这个哈希值签名。下面的例子展示对foo-1.23.tar.gz这个文件的SHA256值进行签名：

```
# 签名过的哈希值是f00-1.23.tar.gz.sha1
openssl dgst -sha256 \
	-sign mykey.pem \
	-out foo-1.23.tar.gz.sha1 \
	foo-1.23.tar.gz
```

### 如何验证一个签名的哈希值？

验证一个签名的哈希值需要原始文件以及签名者的公钥(openssl rsa -in mycert.pem -pubout -out pubkey.pem)。

```
# 使用foo-1.23.tar.gz.sha1和pubkey.pem验证foo-1.23.tar.gz
openssl dgst -sha256 \
	-verify pubkey.pem \
	-signature foo-1.23.tar.gz.sha1 \
	foo-1.23.tar.gz
```

### 如何创建符合Apache要求的认证信息？

Apache的HTTP哈希认证需要指定的密码格式。Apache提供有htdigest工具，但只能将结果输出到文件。我们可以手动的往本地密码数据库添加认证信息。
 密码数据库的数据格式是比较简单的，有三部分：用户名，授权名，用户名加授权名加密码的MD5值，三部分以冒号分割。下面的脚本使用OpenSSL的dgst选项输出符合Apache要求的认证信息：

```
#!/bin/bash

echo "Create an Apache-friendly Digest Password Entry"
echo "-----------------------------------------------"

# get user input, disabling tty echoing for password
read -p "Enter username: " UNAME
read -p "Enter Apache AuthName: " AUTHNAME
read -s -p "Enter password: " PWORD; echo

printf "\n%s:%s:%s\n" \
	"$UNAME" \
	"$AUTHNAME" \
	$(printf "${UNAME}:${AUTHNAME}:${PWORD}" | openssl dgst -md5)
```

### 都有哪些可用的哈希算法？

可以使用内置的list-message-digest-commands选项列出可用的哈希算法：

```
openssl list-message-digest-commands
```

注意列出的结果可能是过时的。(还是通过openssl dgst -h查看吧)

## 加密／解密

### 如何使用base64编码？

使用enc -base64选项

```
# 对文件file.txt里的内容进行base64编码
openssl enc -base64 -in file.txt

# 将编码结果输出到file.txt.enc
openssl enc -base64 -in file.txt -out file.txt.enc
```

通过命令行编码指定的字符串：

```
$ echo "encode me" | openssl enc -base64
ZW5jb2RlIG1lCg==
```

注意echo会在字符串结尾添加一个换行符，可以使用-n选项去除。当你对一些认证信息编码时，这点很重要。

```
$ echo -n "encode me" | openssl enc -base64
ZW5jb2RlIG1l
```

使用-d选项可以解码：

```
$ echo "ZW5jb2RlIG1lCg==" | openssl enc -base64 -d
encode me
```

### 如何简单的加密一个文件？

有时候我们只是想简单的通过密码加密一个文件，不想使用专业的工具，也不想创建私钥／证书之类的。
 这是很简单，你需要记住密码以及使用的加密算法。
 首先选一个加密算法，可以通过下面的方法列出可用的算法：

```
# ‘Cipher commands’下的列表
openssl -h

# 算法列表，每行一个
openssl list-cipher-commands
```

选完加密算法后，还要决定是否对加密后数据进行base64编码，base64编码后数据方便查看，不编码就是二进制的形式。

```
# 使用256位AES算法CBC模式，加密file.txt为file.enc。
openssl enc -aes-256-cbc -salt -in file.txt -out file.enc

# 对加密后数据进行base64编码(-a或-base64)
openssl enc -aes-256-cbc -a -salt -in file.txt -out file.enc
```

解密file.enc

```
# 解密二进制file.enc
openssl enc -d -aes-256-cbc -in file.enc

# 解密base64格式的
openssl enc -d -aes-256-cbc -a -in file.enc
```

如果你不想每次加解密时都输入密码，可以直接在命令中提供：

```
# 在命令行提供密码
openssl enc -aes-256-cbc -salt -in file.txt \
	-out file.enc -pass pass:mySillyPassword

# 在一个文件中提供密码
openssl enc -aes-256-cbc -salt -in file.txt \
	-out file.enc -pass file:/path/to/secret/password.txt
```

## 错误

### 如何了解SSL的错误消息？

查看系统日志，可能会看到一些明显跟OpenSSL或加密有关的错误消息：

```
sshd[31784]: error: RSA_public_decrypt failed: error:0407006A:lib(4):func(112):reason(106)
sshd[770]: error: RSA_public_decrypt failed: error:0407006A:lib(4):func(112):reason(106)
```

了解错误消息的第一步，使用errstr选项解释错误码，错误码在error:和:lib之间，上面的示例是0407006A。

```
$ openssl errstr 0407006A
error:0407006A:rsa routines:RSA_padding_check_PKCS1_type_1:block type is not 01
```

如果你完整的安装了OpenSSL，可以在开发文档里找到相关信息。

## 密钥

### 如何生成RSA私钥？

使用genrsa选项。

```
# 默认1024位密钥，输出到控制台
openssl genrsa

# 2048位密钥，保存到mykey.pem
openssl genrsa -out mykey.pem 2048

# 对密钥进行加密
openssl genrsa -des3 -out mykey.pem 2048
```

### 如何生成RSA公钥？

使用rsa选项根据私钥生产公钥。

```
openssl rsa -in mykey.pem -pubout
```

### 如何生成DSA密钥？

创建DSA密钥需要一个参数文件，并且DSA的验证操作同等情况下比RSA慢。所以没有RSA使用的广泛。
 如果只想创建一个DSA密钥，可以通过dsaparam选项：

```
# 私钥保存在dsakey.pem中
openssl dsaparam -noout -out dsakey.pem -genkey 1024
```

如果想创建多个密钥，可以先创建一个参数文件，创建参数有点耗时，但创建后再生成密钥就很快了。

```
# 创建参数保存在dsaparam.pem中
openssl dsaparam -out dsaparam.pem 1024

# 创建第一个key
openssl gendsa -out key1.pem dsaparam.pem

# 第二个...
openssl gendsa -out key2.pem dsaparam.pem
```

### 如何生成椭圆曲线密钥？

椭圆曲线加密在OpenSSL 0.9.8版本加入。通过ecparam选项生成密钥。

```
openssl ecparam -out key.pem -name prime256v1 -genkey

# -name后支持的选项列表
openssl ecparam -list_curves
```

### 如何解密密钥？

也许对每次都输入密码厌烦了，你可以对密钥解密，使用rsa或dsa选项，取决于使用了哪种加密算法。
 如果你有一个加密的RSA密钥文件key.pem，下面的例子是将解密后的密钥存在文件newkey.pem中。

```
# 需要输入密钥的密码
openssl rsa -in key.pem -out newkey.pem
```

通常情况是在一个文件里存储私钥和公共证书，假如这个文件是mycert.pem，通过以下两步生成不加密的版本newcert.pem

```
# 需要输入密钥的密码
openssl rsa -in mycert.pem -out newcert.pem
openssl x509 -in mycert.pem >>newcert.pem
```

## 密码哈希

使用passwd选项，可以生成不同格式的密码哈希值。

### 如何生成加密风格(crypt-style)的密码哈希值？

生成一个新的哈希值非常简单：

```
$ openssl passwd MySecret
8E4vqBR4UOYF.
```

如果知道密码以及运算时用的盐(salt)，就能重现这个哈希值。

```
$ openssl passwd -salt 8E MySecret
8E4vqBR4UOYF.
```

### 如何生成阴影风格(shadow-style)的密码哈希值？

最近的Unix系统开始使用更加安全的哈希策略，基于MD5运算，并且使用8个字符作为盐(传统的加密风格使用2个字符作为盐)。
 加上-1选项，就能生成这样的哈希值：

```
$ openssl passwd -1 MySecret
$1$sXiKzkus$haDZ9JpVrRHBznY5OxB82.
```

这个风格的哈希值，盐位于第二个和第三个$符之间，上面例子的盐值是sXiKzkus。知道密码和盐值后可以重现这个哈希值。

```
$ openssl passwd -1 -salt sXiKzkus MySecret
$1$sXiKzkus$haDZ9JpVrRHBznY5OxB82.
```

## 素数

当前的加密技术很大程度上是依赖素数的生成和检验。难怪OpenSSL库提供大量的操作来处理素数。应该是从0.9.7e版本开始，加入了prime选项。

### 如何测试一个数是否为素数？

使用prime选项，后面跟要测试的数字。注意OpenSSL的返回数字是十六进制的。

```
$ openssl prime 119054759245460753
1A6F7AC39A53511 is not prime
```

也可以直接传入十六进制的数。

```
$ openssl prime -hex 2f
2F is prime
```

### 如何生成一组素数？

从OpenSSL 1.0版本开始，可以生成指定长度的素数：

```
$ openssl prime -generate -bits 64
16148891040401035823
$ openssl prime -generate -bits 64 -hex
E207F23B9AE52181
```

1.0之前的版本，就得传入一个范围的数判断哪个符合要求了。

```
# define start and ending points
AQUO=10000
ADQUEM=10100
for N in $(seq $AQUO $ADQUEM); do
	# use bc to convert hex to decimal
	openssl prime $N | awk '/is prime/ {print "ibase=16;"$1}' | bc
done
```

## 随机数

### 如何生成随机数？

使用rand选项可以生成二进制或base64格式的随机数。

```
# 直接在控制台输出128位base64格式的随机数
openssl rand -base64 128

# 在指定的文件中输出1024位二进制随机数
openssl rand -out random-data.bin 1024

# 从浏览器缓存取到半随机子节数据作为种子来生成随机数
cd $(find ~/.mozilla/firefox -type d -name Cache)
openssl rand -rand $(find . -type f -printf '%f:') -base64 1024
```

在类Unix系统中有个/dev/urandom的设备以及GNU的head工具，或者最近版本BSD的head，使用它们可以更有效的生成随机数：

```
# 取得32位随机数，以base64编码显示
head -c 32 /dev/urandom | openssl enc -base64
```

可以利用strings工具得到比Base64使用更多字符的随机结果：

```
# 取得32位随机数，取得可打印的字符，去除空格，替换掉换行符
echo $(head -c 32 /dev/random | strings -1) | sed 's/[[:space:]]//g'
```

在某些极端情况下，请确保了解使用random和urandom的优劣之处。可以在Linux和BSD系统的random(4)的man页面，Solaris的random(7D)的man页面查到更多的信息。

## S/MIME

S/MIME表示安全的发送和接收MIME数据，尤其是自e-mail消息中。已经有相当部分e-mail客户端默认支持S/MIME了，OpenSSL通过smime选项提供命令行S/MIME服务。
 注意[smime的man页面](https://www.openssl.org/docs/manmaster/apps/smime.html)包含了一些特别好的示例。

### 如何验证签名过的S/MIME消息？

使用mail客户端将签名过的S/MIME消息保存为msg.txt。

```
openssl smime -verify -in msg.txt
```

如果发送者的证书是被你的OpenSSL所认可的授权机构签名的，你就能看到mail的头信息，正文以及一行提示Verification successful。
 如果消息被篡改过，会输出错误消息指明哈希值或者签名不匹配：

```
Verification failure
23016:error:21071065:PKCS7 routines:PKCS7_signatureVerify:digest
failure:pk7_doit.c:804:
23016:error:21075069:PKCS7 routines:PKCS7_verify:signature
failure:pk7_smime.c:265:
```

同样，如果发送者的证书验证失败，会输出类似的错误消息：

```
Verification failure
9544:error:21075075:PKCS7 routines:PKCS7_verify:certificate verify
error:pk7_smime.c:222:Verify error:self signed certificate
```

多数e-mail客户端会再消息的签名中发送公共证书。在命令行中你可以查看这证书的数据。可以使用smime -pk7out选线导出PKCS#7证书，然后再使用pkcs7选项。这方法虽然笨但确实可用。

```
openssl smime -pk7out -in msg.txt | \
openssl pkcs7 -text -noout -print_certs
```

也可以将对方的证书保存为指定的文件。

```
openssl smime -pk7out -in msg.txt -out her-cert.pem
```

这时可以把这个证书加入你的OpenSSL信任组中或存在别处。

```
openssl smime -verify -in msg.txt -CAfile /path/to/her-cert.pem
```

### 如何加密S/MIME消息？

加入有人发给你她的公共证书，并要求你发送加密过消息给她。她的证书存为her-cert.pem，你要发的消息存为message.txt。
 默认使用比较弱的RC2-40加密方案，只要告诉OpenSSL消息和证书的地址就行。

```
openssl smime her-cert.pem -encrypt -in my-message.txt
```

如果你确定对方有处理SSL的能力，可以指定更安全的算法，比如DES3。

```
openssl smime her-cert.pem -encrypt -des3 -in my-message.txt
```

默认情况下，加密过消息以及mail头信息输出到了控制台。可以使用-out选项输出到文件中。甚至可以利用sendmail直接发送出去。

```
openssl smime her-cert.pem \
	-encrypt \
	-des3 \
	-in my-message.txt \
	-from 'Your Fullname <you@youraddress.com>' \
	-to 'Her Fullname <her@heraddress.com>' \
	-subject 'My encrypted reply' |\
sendmail her@heraddress.com
```

### 如何对S/MIME消息签名？

如果你不需要加密整个消息，只是想对消息做个签名，这样对方可以确定消息的完整性。处理方法跟加密类似，主要的不同是你需要自己的私钥和证书，因为你不可能拿对方的证书去签名。

```
openssl smime \
	-sign \
	-signer /path/to/your-cert.pem \
	-in my-message.txt \
	-from 'Your Fullname <you@youraddress.com>' \
	-to 'Her Fullname <her@heraddress.com>' \
	-subject 'My signed reply' |\
sendmail her@heraddress.com
```



# BIO模块

OpenSSL抽象I/O(Basic I/O abstraction，即BIO)是OpenSSL对于I/O类型的抽象封装，包括内存、文件、日志、标准输入输出、socket、加解密、摘要和ssl通道等。OpenSSL BIO通过回调函数为用户隐藏了底层的实现细节，所有类型的BIO调用大体类似。BIO中的数据能从一个BIO传递给另一个BIO或是应用程序。

## BIO结构体

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

## BIO_METHOD

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

## BIO的相关函数

### BIO的声明与释放

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

### BIO的控制函数

BIO的控制函数较多，见附录。这些函数用于控制BIO的状态等。

### BIO的I/O操作

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

### BIO对

BIO对是BIO中专门创建的一对缓存BIO，创建BIO对的函数如下：

```c++
#include<openssl/bio.h>
int BIO_new_bio_pair(BIO **bio1, size_t writebuf1, BIO **bio2, size_t writebuf2);
```

函数调用成功之后返回1，此时bio1和bio2都变得有效；否则返回0，bio1和bio2被设为NULL。但是注意它们在不使用的时候必须分别释放，因为有些SSL函数会隐含调用BIO_free函数。

当BIO对创建之后，它的两端都能作为数据缓冲的输入和输出，典型的应用为它一端和SSL的IO连接，另一端被应用控制。

这两个BIO对的功能是完全对称的，它们的缓冲区大小由参数writebuf1和writebuf2决定，如果设为0则使用默认大小。注意这个函数不会检查bio1和bio2是否指向其他BIO，二者的值都会被重写，且不会调用BIO_free()函数。因此在使用之前必须自己保证这两个变量为空，否则会造成内存泄漏。

因为BIO对只会简单的缓存数据，而不会直接涉及到连接，所以它看起来**就像非阻塞型的接口**，如果**写缓存满了或读缓存空的时候，调用IO函数就会立刻返回**。也就是说，应用程序必须自己对写缓存执行flush操作或对读缓存执行fill操作。

### BIO链操作

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

## BIO分类

### Source/Sink类型BIO

#### 文件指针类型BIO

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

#### 文件描述符类型BIO

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

#### Socket类型BIO

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

#### NULL型BIO

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

#### 内存类型BIO

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

#### BIO对

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

#### 连接类型BIO

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

#### 接受类型BIO

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

### Filter类型BIO

#### NULL型过滤BIO

NULL型的filterBIO的定义如下：

```c++
#include<openssl/bio.h>

BIO_METHOD * BIO_f_null(void);
```

该函数返回一个NULL型的过滤BIO_METHOD结构，NULL过滤型BIO是一个不作任何事情的BIO。针对该类型BIO的任何调用都会被简单传递中BIO链中的下一个BIO中去，也就相当于该BIO是不存在的一样。

#### 缓冲类型BIO

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

#### Base64类型BIO

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

#### Cipher类型BIO

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

#### MD类型BIO

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

#### SSL类型BIO

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






# EVP模块
## 简介

Openssl EVP(high-level cryptographic functions[1])提供了丰富的密码学中的各种函数。Openssl 中实现了各种对称算法、摘要算法以及签名/验签算法。EVP 函数将这些具体的算法进行了封装。 官方建议使用这些算法时最好调用EVP的上层封装，不建议单独使用某个算法。

 EVP系列的函数的声明包含在”evp.h”里面，这是一系列封装了openssl加密库里面所有算法的函数。通过这样的统一的封装，使得只需要在初始化参数的时候做很少的改变，就可以使用相同的代码但采用不同的加密算法进行数据的加密和解密。 

EVP系列函数主要封装了**加密、摘要、编码**三大类型的算法，使用算法前需要调用OpenSSL_add_all_algorithms函数。

其中以**加密算法与摘要算法**为基本，公开密钥算法是对数据加密采用了对称加密算法，对密钥采用非对称加密（公钥加密，私钥解密）。数字签名是非对称算法（私钥签名，公钥认证）。 

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



## 源码结构

evp源码位于crypto/evp目录，可以分为如下几类：

### 全局函数

主要包括 c_allc.c、c_alld.c、c_all.c 以及names.c。他们加载openssl  支持的所有的对称算法和摘要算法，放入到哈希表中。实现了OpenSSL_add_all_digests、OpenSSL_add_all_ciphers  以及OpenSSL_add_all_algorithms(调用了前两个函数)函数。在进行计算时，用户也可以单独加载摘要函数（EVP_add_digest）和对称计算函数（EVP_add_cipher）。

### BIO扩充

包括 bio_b64.c、bio_enc.c、bio_md.c 和bio_ok.c，各自实现了BIO_METHOD方法，分别用于base64 编解码、对称加解密以及摘要。

### 摘要算法封装

由 digest.c 实现，实现过程中调用了对应摘要算法的回调函数。各个摘要算法提供了自己的EVP_MD 静态结构，对应源码为m_xxx.c。

### 对称算法封装

由evp_enc.c 实现，实现过程调用了具体对称算法函数，实现了Update 操作。各种对称算法都提供了一个EVP_CIPHER  静态结构，对应源码为e_xxx.c。需要注意的是，e_xxx.c  中不提供完整的加解密运算，它只提供基本的对于一个block_size数据的计算，完整的计算由evp_enc.c  来实现。

想实现一个自己的对称算法时，可以参考e_xxx.c 的实现方式。一般至少需要实现如下功能：

- 构造一个新的静态的 EVP_CIPHER 结构；
- 实现 EVP_CIPHER 结构中的init 函数，该函数用于设置iv，设置加解密标记、以及根据外送密钥生成自己的内部密钥；
- 实现 do_cipher 函数，该函数仅对block_size 字节的数据进行对称运算；
- 实现 cleanup 函数，该函数主要用于清除内存中的密钥信息。

### 非对称算法封装

主要是以 p_开头的文件。其中，p_enc.c 封装了公钥加密；p_dec.c 封装了私钥解密；p_lib.c  实现一些辅助函数；p_sign.c 封装了签名函数；p_verify.c 封装了验签函数；p_seal.c 封装了数字信封；p_open.c  封装了解数字信封。

### 基于口令的加密

包括 p5_crpt2.c、p5_crpt.c 和evp_pbe.c。 



## 主要函数

### 对称加解密函数

- EVP_BytesToKey：计算密钥函数，它根据算法类型、摘要算法、salt 以及输入数据计算出一个对称密钥和初始化向量iv。
- PKCS5_PBE_keyivgen 和PKCS5_v2_PBE_keyivgen：实现了 PKCS5 基于口令生成密钥和初始化向量的算法。
- PKCS5_PBE_add：加载所有 openssl 实现的基于口令生成密钥的算法。
- EVP_PBE_alg_add：添加一个 PBE 算法。

### 其他函数

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



## 对称加密

### 基本数据结构

EVP_CIPHER与EVP_CIPHER_CTX两个基本结构，加密函数EVP_Encrypt(EVP_Cipher)一些列函数都是以这两个结构为基础实现了。文件evp_enc.c是最高层的封装实现，，而各个e_*.c文件则是真正实现了各种算法的加解密功能，当然它们其实也是一些封装函数，真正的算法实现在各个算法同名目录里面的文件实现。 

注意:  EVP_CIPHER是、EVP_CIPHER_CTX的成员，在加密时通过指定的加密算法（其实就是加密函数），返回对应的EVP_CIPHER的指针，然后EVP_EncryptInit函数中 调用 EVP_CIPHER来初化EVP_CIPHER_CTX。

#### EVP_CIPHER结构体

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

#### EVP_CIPHER_CTX结构体

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

### 相关函数

#### 核心函数

EVP_crypt系列函数只是对EVP_Cipher系列函数的调用，EVP_Encrypt函数相当于对EVP_Cipher函数enc参数置为1，EVP_Decrypt函数相当于对EVP_Cipher函数enc参数置为0。

##### 底层函数

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

##### 加密

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

##### 解密

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

#### 辅助函数

##### 操作EVP_CIPHER_CTX的函数

```cpp
int EVP_CIPHER_CTX_reset(EVP_CIPHER_CTX *c)    //重置EVP_CIPHER_CTX

EVP_CIPHER_CTX *EVP_CIPHER_CTX_new(void)       //开辟EVP_CIPHER_CTX
void EVP_CIPHER_CTX_free(EVP_CIPHER_CTX *ctx)  //销毁之前开辟的EVP_CIPHER_CTX

void EVP_CIPHER_CTX_init(EVP_CIPHER_CTX *a); //初始化一个EVP_CIPHER_CTX结构体   
int EVP_CIPHER_CTX_cleanup(EVP_CIPHER_CTX *a); //清除一个EVP_CIPHER_CTX结构体中的所有信息，并释放该结构占用的所有内存
```

##### 参数设置与获取

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

#### 算法函数

openssl对称加密算法的格式都以函数形式提供，其实该函数返回一个该算法的结构体，其形式一般如下(evp.h 、e\_···.c)： 
      `EVP_CIPHER* EVP_···(···代表加密算法的名称)(void)` 
在openssl中，所有提供的对称加密算法长度都是固定的，有特别说明的除外。

##### 分组加密的迭代模式

- ecb—电子密码本（Electronic Code Book)加密方式
- cbc—加密块链接（Cipher Block Chaining）加密方式
- cfb——加密反馈（Cipher Feedback）加密方式
- ofb—输出反馈（Output Feedback）加密方式
- ede—该加密算法采用了加密、解密、加密的方式，第一个密钥和最后一个密钥是相同的
- ede3—该加密算法采用了加密、解密、加密的方式，但是三个密钥都不相同

##### 加密算法

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

### 应用流程

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

### 程序示例

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

## 摘要

该系列函数封装了openssl加密库所有的信息摘要算法，通过这种EVP封装，当使用不同的信息摘要算法时，只需要对初始化参数修改一下就可以了，其它代码可以完全一样。这些算法包括MD2、MD5以及SHA等算法。

- 函数名称：EVP_Digest*…*
- 功能描述：该系列函数封装实现了多种信息摘要算法。
- 相关文件：digest.c,m_\*.c

### 基本数据结构

#### EVP_MD结构体

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

#### EVP_MD_CTX结构体

```cpp
typedef struct env_md_ctx_st
{
    const EVP_MD *digest;  //digest——指向上面介绍的EVP_MD结构的指针
    ENGINE *engine;        //如果算法由ENGINE提供，该指针指向该ENGINE
    unsigned long flags;   //
    void *md_data;         //信息摘要数据
}EVP_MD_CTX ;
```

### 相关函数

#### 核心函数

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

#### 辅助函数

##### 操作EVP_MD_CTX的函数

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

##### 参数设置与获取函数

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

##### 摘要算法函数

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

### 程序示例

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

## 非对称加密

主要是以 p_开头的文件。其中:

-   p_enc.c 封装了公钥加密；
-   p_dec.c 封装了私钥解密；
-   p_lib.c 实现一些辅助函数；
-   p_sign.c 封装了签名函数；
-   p_verify.c 封装了验签函数；
-   p_seal.c 封装了数字信封相关的函数；
-   p_open.c 封装了解数字信封相关的函数。

### 基本数据结构

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

### 相关函数

#### 核心函数

##### 加密

```cpp
int EVP_PKEY_encrypt_old(unsigned char *enc_key,const unsigned char *key, int key_len, EVP_PKEY *pubk) //使用RSA公钥加密

int EVP_PKEY_encrypt_init(EVP_PKEY_CTX *ctx); //使用密钥pkey对公钥算法的上下文进行初始化，以进行加密操作。返回1成功，0或负值失败，-2代表该公钥算法不支持此操作
int EVP_PKEY_encrypt(EVP_PKEY_CTX *ctx,unsigned char *out, size_t *outlen, const unsigned char *in, size_t inlen); //函数使用ctx执行公钥加密操作。使用in和inlen参数指定要加密的数据与数据长度。如果out为NULL，则输出缓冲区的最大大小写入outlen参数。如果out不为NULL，那么在调用之前，outlen参数应该包含out缓冲区的长度，如果调用成功，则将加密数据写入out，并将加密后的数据长度写入outlen。返回1成功，0或负值失败，-2表示该公钥算法不支持该操作
```

##### 解密

```cpp
int EVP_PKEY_decrypt_old(unsigned char *dec_key, const unsigned char *enc_key, int enc_key_len,EVP_PKEY *private_key);//RSA私钥解密

int EVP_PKEY_decrypt_init(EVP_PKEY_CTX *ctx); //函数使用密钥pkey初始化公钥算法的上下文以进行解密操作。返回1成功，0或负值失败，-2表示该公钥算法不支持该操作。
int EVP_PKEY_decrypt(EVP_PKEY_CTX *ctx,unsigned char *out, size_t *outlen, const unsigned char *in, size_t inlen); //函数使用ctx执行公钥解操作。使用in和inlen参数指定要解密的数据与数据字长。如果out为NULL，则输出缓冲区的最大大小写入outlen参数。如果out不为NULL，那么在调用之前，outlen参数应该包含out缓冲区的长度，如果调用成功，则将解密数据写入out，并将数据写入outlen。返回1成功，0或负值失败，-2表示该公钥算法不支持该操作。
```

#### 辅助函数

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

### 程序示例

#### 参数初始化

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

#### 密钥生成

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

## BASE64编解码

### 原理

Base 64编码将二进制数据转换为使用字符 A-Z，a-z，0-9，“+”和“/”表示来数据的可打印形式。每3个字节的二进制数据，编码为上述字符表示的4字节数据。如果输入数据长度不是3的倍数，则输出数据将使用“=”字符在最后填充。

步骤：首先将每三个字节原始二进制数据在一起展开； 然后6bit分为一个小组，每个小组前面补两个0，成为一个字节；把新编码的每个字节转为十进制，根据base64标准转换表，找到对应的字符。 如果多了一个字节，则剩余两个字节用“=”填充，如果多了两个字节，则剩余一个字节用“=”填充。

### 基本数据结构

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

### 相关函数

#### 核心函数

##### 编码

```cpp
#include <openssl/evp.h>

void EVP_EncodeInit(EVP_ENCODE_CTX *ctx); //初始化ctx以启动新的编码操作，无返回值
void EVP_EncodeUpdate(EVP_ENCODE_CTX *ctx, unsigned char *out, int *outl, const unsigned char *in, int inl); //编码in指向的缓冲区中的inl字节数据。输出存储在缓冲区out中，输出的字节数存储在outl中。调用者必须确保out指向的缓冲区足够大以容纳输出数据。只有完整的数据块（48字节）可以被直接编码完后并通过函数输出。任何剩余的字节都保存在ctx对象中，并通过后续调用EVP_EncodeUpdate（）EVP_EncodeFinal（）来处理。
void EVP_EncodeFinal(EVP_ENCODE_CTX *ctx, unsigned char *out, int *outl); //必须在编码操作结束时调用EVP_EncodeFinal（）。它将处理ctx对象中剩余的任何部分数据块。输出数据将被存储在out，输出的数据长度将存储在* outl中，包含了’\n’。无返回值

int  EVP_EncodeBlock(unsigned char *t, const unsigned char *f, int n); //对f中的输入数据进行编码，并将其存储在t中。对于每3字节的输入，将产生4字节的输出数据。如果n不能被3整除，则块被当做最后的数据块来编码，并且被填充，使得它总是可被除以4。另外还将添加‘\0’终结符字符。
```

##### 解码

```cpp
#include <openssl/evp.h>

void EVP_DecodeInit(EVP_ENCODE_CTX *ctx); //初始化ctx以开始新的解码操作
int  EVP_DecodeUpdate(EVP_ENCODE_CTX *ctx, unsigned char *out, int *outl, const unsigned char *in, int inl); //解码in指向的缓冲区中inl字节的数据。输出存储在缓冲区中out，输出的字节数存储在* outl中。调用者有责任确保out指向的缓冲区足够大以容纳输出数据。该功能将尝试在4字节块中尽可能多地解码数据。任何空格，换行符或回车符都将被忽略。任何保留在结尾的未处理数据（1,2或3个字节）的部分块将保留在ctx对象中，并由后续调用EVP_DecodeUpdate（）处理。如果遇到非法的base64字符，或者如果在数据中间遇到base64填充字符“=”，则函数返回-1表示错误。返回值为0或1表示数据成功处理。返回值0表示处理的最后输入数据字符包括base64填充字符“=”，预期不会再处理非填充字符数据
int  EVP_DecodeFinal(EVP_ENCODE_CTX *ctx, unsigned char *out, int *outl); //必须在解码操作结束时调用，如果仍然存在任何未处理的数据，那么输入数据不能是4的倍数。成功返回1，失败返回-1

int  EVP_DecodeBlock(unsigned char *t, const unsigned char *f, int n); //解码f中包含的基本64个数据的n个字节的块，并将结果存储在t中。任何前导空格，以及任何尾随的空格，换行符，回车符或EOF字符将被修剪。返回解码的数据长度，出错返回-1。
```

#### 辅助函数

```cpp
EVP_ENCODE_CTX *EVP_ENCODE_CTX_new(void); //分配，初始化并返回要用于encode / decode函数的上下文。成功返回地址，失败返回NULL
void EVP_ENCODE_CTX_free(EVP_ENCODE_CTX *ctx); //清理编码/解码上下文ctx并释放分配给它的空间
int EVP_ENCODE_CTX_copy(EVP_ENCODE_CTX *dctx, EVP_ENCODE_CTX *sctx)
int EVP_ENCODE_CTX_num(EVP_ENCODE_CTX *ctx); //返回在ctx对象中待处理的尚未编码或解码的字节数
```

### 程序示例

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

## HMAC

HMAC是基于散列函数的MAC（消息认证码），即用于消息认证的密钥哈希函数。

### 基本数据结构

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

### 相关函数

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

### 程序示例

#### 生成

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

#### 验证

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

## 数字签名

### 签名

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

### 认证

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

### 备注

在新版的OpenSSL中定义了`EVP_DigestSign*` 和 `EVP_DigestVerify*` 这两个系列的函数，将计算摘要和数字签名操作合在一起。这一系列函数的用法与不带Digest的系列函数类似，但是函数的参数稍有不同。

### 程序示例

#### 签名

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

#### 验证

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

## 数字信封

### 写信

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

### 读信

本系列函数将公钥加密了的密钥加密出来，然后进行数据的解密。其定义的函数如下：

```cpp
#include<openssl/evp.h>
int EVP_OpenInit(EVP_CIPHER_CTX *ctx,EVP_CIPHER *type,unsigned char *ek, int ekl,unsigned char *iv,EVP_PKEY *priv); //该函数初始化一个用来加密数据的ctx结构。它首先使用参数priv指定的私钥解密参数ek里面长度为ekl字节的加密密钥。然后用此密钥与参数iv指定的初始化向量初始化EVP_CIPHER_CTX。如果参数type设定的加密算法长度是可变的，那么密钥长度就会被设置为解密得到的密钥的长度；如果加密算法长度是固定的，那么得到的解密密钥的长度就必须跟固定算法长度相同才行。成功执行返回密钥的长度，否则返回0。 

#define EVP_OpenUpdate(a,b,c,d,e) EVP_DecryptUpdate(a,b,c,d,e)
int EVP_OpenUpdate(EVP_CIPHER_CTX *ctx, unsigned char *out, int *outl, unsigned char *in, int inl); //功能和使用方法与EVP_DecryptUpdate相同

int EVP_OpenFinal(EVP_CIPHER_CTX *ctx, unsigned char *out,int *outl); //调用EVP_DecryptFinal_ex完成了其功能，本函数还调用EVP_DecryptInit_ex(ctx,NULL,NULL,NULL,NULL)再次进行了初始化工作。成功执行返回1，否则返回0。
```

### 代码示例

#### 写信

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

#### 读信

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



# SSL通信

下面的代码为服务器和客户端建立SSL连接的代码示例：

```cpp
ssl-server.c:
#include <stdio.h> 
#include <stdlib.h> 
#include <errno.h> 
#include <string.h> 
#include <sys/types.h> 
#include <netinet/in.h> 
#include <sys/socket.h> 
#include <sys/wait.h> 
#include <unistd.h> 
#include <arpa/inet.h> 
#include <openssl/ssl.h> 
#include <openssl/err.h> 

#define MAXBUF 1024 

int main(int argc, char * *argv) 
{
    int sockfd, new_fd;
    socklen_t len;
    struct sockaddr_in my_addr, their_addr;
    unsigned int myport, lisnum;
    char buf[MAXBUF + 1];
    SSL_CTX * ctx;
    //指定监听端口
    if (argv[1]) 
    {
        myport = atoi(argv[1]);
    }
    else 
    {
        myport = 8888;
    }
    //最大客户端连接数
    if (argv[2]) 
    {
        lisnum = atoi(argv[2]);
    }
    else 
    {
        lisnum = 2;
    }
    /* SSL 库初始化*/
    SSL_library_init();
    /* 载入所有SSL 算法*/
    OpenSSL_add_all_algorithms();
    /* 载入所有SSL 错误消息*/
    SSL_load_error_strings();
    /* 以SSL V2 和V3 标准兼容方式产生一个SSL_CTX ，即SSL Content Text */
    ctx = SSL_CTX_new(SSLv23_server_method());
    /* 
    也可以用SSLv2_server_method() 或SSLv3_server_method() 单独表示V2 或V3标准
    */
    if (ctx == NULL) 
    {
        ERR_print_errors_fp(stdout);
        exit(1);
    }
    /* 载入用户的数字证书， 此证书用来发送给客户端。证书里包含有公钥*/
    if (SSL_CTX_use_certificate_file(ctx, argv[4], SSL_FILETYPE_PEM) <= 0) 
    {
        ERR_print_errors_fp(stdout);
        exit(1);
    }
    /* 载入用户私钥*/
    if (SSL_CTX_use_PrivateKey_file(ctx, argv[5], SSL_FILETYPE_PEM) <= 0) 
    {
        ERR_print_errors_fp(stdout);
        exit(1);
    }
    /* 检查用户私钥是否正确*/
    if (!SSL_CTX_check_private_key(ctx)) 
    {
        ERR_print_errors_fp(stdout);
        exit(1);
    }

    /* 开启一个socket 监听*/
    if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) == -1) 
    {
        perror("socket");
        exit(1);
    } 
    else 
    {
        printf("socket created\n");
    }
    bzero( &my_addr, sizeof(my_addr));
    my_addr.sin_family = PF_INET;
    my_addr.sin_port = htons(myport);
    
    //设置监听的IP
    if (argv[3]) 
    {
        my_addr.sin_addr.s_addr = inet_addr(argv[3]);
    }
    else 
    {
        //如果用户没有指定监听端口，则默认监听0.0.0.0(任意IP)
        my_addr.sin_addr.s_addr = INADDR_ANY;
    }
    if (bind(sockfd, (struct sockaddr * ) &my_addr, sizeof(struct sockaddr)) == -1) 
    {
        perror("bind");
        exit(1);
    } 
    else
    {
        printf("binded\n");
    } 
    if (listen(sockfd, lisnum) == -1) 
    {
        perror("listen");
        exit(1);
    } 
    else 
    {
        printf("begin listen\n");
    }
    while (1) 
    {
        SSL * ssl;
        len = sizeof(struct sockaddr);
        /* 等待客户端连上来*/
        if ((new_fd = accept(sockfd, (struct sockaddr * ) & their_addr, &len)) == -1) 
        {
            perror("accept");
            exit(errno);
        } 
        else 
        {
            printf("server: got connection from %s, port %d, socket %d\n", inet_ntoa(their_addr.sin_addr), ntohs(their_addr.sin_port), new_fd);
        }
        /* 基于ctx 产生一个新的SSL */
        ssl = SSL_new(ctx);
        /* 将连接用户的socket 加入到SSL */
        SSL_set_fd(ssl, new_fd);
        /* 建立SSL 连接*/
        if (SSL_accept(ssl) == -1) 
        {
            perror("accept");
            close(new_fd);
            break;
        }
        /* 开始处理每个新连接上的数据收发*/
        bzero(buf, MAXBUF + 1);
        strcpy(buf, "server->client");
        /* 发消息给客户端*/
        len = SSL_write(ssl, buf, strlen(buf));
        if (len <= 0) 
        {
            printf("消息'%s'发送失败！错误代码是%d，错误信息是'%s'\n", buf, errno, strerror(errno));
            goto finish;
        } 
        else 
        {
            printf("消息'%s'发送成功，共发送了%d 个字节！\n", buf, len);
        }
        bzero(buf, MAXBUF + 1);
        /* 接收客户端的消息*/
        len = SSL_read(ssl, buf, MAXBUF);
        if (len > 0) 
        {
            printf("接收消息成功:'%s'，共%d 个字节的数据\n", buf, len);
        }
        else 
        {
            printf("消息接收失败！错误代码是%d，错误信息是'%s'\n", errno, strerror(errno));
        }
        /* 处理每个新连接上的数据收发结束*/
        finish:
        /* 关闭SSL 连接*/
        SSL_shutdown(ssl);
        /* 释放SSL */
        SSL_free(ssl);
        /* 关闭socket */
        close(new_fd);
    }
    /* 关闭监听的socket */
    close(sockfd);
    /* 释放CTX */
    SSL_CTX_free(ctx);
    return 0;
}


ssl-client.c
#include <stdio.h> 
#include <string.h> 
#include <errno.h> 
#include <sys/socket.h> 
#include <resolv.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <arpa/inet.h> 
#include <unistd.h> 
#include <openssl/ssl.h> 
#include <openssl/err.h> 

#define MAXBUF 1024 

void ShowCerts(SSL * ssl) 
{
    X509 * cert;
    char * line;
    cert = SSL_get_peer_certificate(ssl);
    if (cert != NULL) 
    {
        printf("数字证书信息:\n");
        line = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0);
        printf("证书: %s\n", line);
        free(line);
        line = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0);
        printf("颁发者: %s\n", line);
        free(line);
        X509_free(cert);
    } 
    else 
    {
        printf("无证书信息！\n");
    }
} 

int main(int argc, char * *argv) 
{
    int sockfd, len;
    struct sockaddr_in dest;
    char buffer[MAXBUF + 1];
    SSL_CTX * ctx;
    SSL * ssl;
    if (argc != 3) 
    {
        printf("参数格式错误！正确用法如下：\n\t\t%s IP 地址端口\n\t 比如:\t%s 127.0.0.1 80\n 此程序用来从某个IP 地址的服务器某个端口接收最多MAXBUF 个字节的消息", argv[0], argv[0]);
        exit(0);
    }
    /* SSL 库初始化*/
    SSL_library_init();
    /* 载入所有SSL 算法*/
    OpenSSL_add_all_algorithms();
    /* 载入所有SSL 错误消息*/
    SSL_load_error_strings();
    /* 以SSL V2 和V3 标准兼容方式产生一个SSL_CTX ，即SSL Content Text */
    ctx = SSL_CTX_new(SSLv23_client_method());
    if (ctx == NULL) 
    {
        ERR_print_errors_fp(stdout);
        exit(1);
    }
    /* 创建一个socket 用于tcp 通信*/
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    {
        perror("Socket");
        exit(errno);
    }
    printf("socket created\n");
    /* 初始化服务器端（对方）的地址和端口信息*/
    bzero( &dest, sizeof(dest));
    dest.sin_family = AF_INET;
    //设置连接的端口
    dest.sin_port = htons(atoi(argv[2]));
    //设置连接的IP地址
    if (inet_aton(argv[1], (struct in_addr * ) &dest.sin_addr.s_addr) == 0) 
    {
        perror(argv[1]);
        exit(errno);
    }
    printf("address created\n");
    /* 连接服务器*/
    if (connect(sockfd, (struct sockaddr * ) &dest, sizeof(dest)) != 0) 
    {
        perror("Connect ");
        exit(errno);
    }
    printf("server connected\n");
    /* 基于ctx 产生一个新的SSL */
    ssl = SSL_new(ctx);
    /* 将新连接的socket 加入到SSL */
    SSL_set_fd(ssl, sockfd);
    /* 建立SSL 连接*/
    if (SSL_connect(ssl) == -1) 
    {
        ERR_print_errors_fp(stderr);
    }
    else 
    {
        printf("Connected with %s encryption\n", SSL_get_cipher(ssl));
        ShowCerts(ssl);
    }
    /* 接收对方发过来的消息，最多接收MAXBUF 个字节*/
    bzero(buffer, MAXBUF + 1);
    /* 接收服务器来的消息*/
    len = SSL_read(ssl, buffer, MAXBUF);
    if (len > 0) 
    {
        printf("接收消息成功:'%s'，共%d 个字节的数据\n", buffer, len);
    }
    else 
    {
        printf("消息接收失败！错误代码是%d，错误信息是'%s'\n", errno, strerror(errno));
        goto finish;
    }
    bzero(buffer, MAXBUF + 1);
    strcpy(buffer, "from client->server");
    /* 发消息给服务器*/
    len = SSL_write(ssl, buffer, strlen(buffer));
    if (len < 0) 
    {
        printf("消息'%s'发送失败！错误代码是%d，错误信息是'%s'\n", buffer, errno, strerror(errno));
    }
    else 
    {
        printf("消息'%s'发送成功，共发送了%d 个字节！\n", buffer, len);
    }
    finish:
    /* 关闭连接*/
    SSL_shutdown(ssl);
    SSL_free(ssl);
    close(sockfd);
    SSL_CTX_free(ctx);
    return 0;
}
```

usage:
1. 程序中用到的包含公钥的服务端证书cacert.pem和服务端私钥文件privkey.pem需要使用如下方式生成：
openssl genrsa -out privkey.pem 2048
openssl req -new -x509 -key privkey.pem -out cacert.pem -days 1095
2. 编译程序用下列命令：
gcc -Wall ssl-client.c -o client -lssl
gcc -Wall ssl-server.c -o server -lssl
3. 运行程序用如下命令：
./server 8888 3 127.0.0.1 cacert.pem privkey.pem
./client 127.0.0.1 8888



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

1. https://github.com/zhouyongtao/openssl-howto
2. https://www.openssl.org/docs/man1.1.1/man7/
3. https://github.com/theno/openssl-examples/blob/master/links.md
4. OpenSSL编程 赵春平
5. https://blog.csdn.net/liao20081228/article/details/77193729/
6. https://blog.csdn.net/liao20081228/article/details/76285896
7. https://wiki.openssl.org/index.php/EVP