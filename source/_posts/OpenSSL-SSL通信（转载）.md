---
title: OpenSSL-SSL通信（转载）
tags: 
- 信息安全
- OpenSSL
categories: [信息安全, OpenSSL, 转载]
sharing: True

---

注：本文内容转载自https://www.cnblogs.com/LittleHann/p/3741907.html

下面的代码为服务器和客户端建立SSL连接的代码示例：

<!--more-->

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