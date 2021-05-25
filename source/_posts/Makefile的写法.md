---
title: Makefile的写法
tags: 
- Linux
categories: [Linux]
sharing: True
---

# make

对于包含有许多源文件的大型工程，整个工程需要按照一定的编译规则进行编译，才能成功地将整个工程编译为可执行文件。`make`是最常用的工程构建工具，它可以用于自动编译和维护项目源码。同时，`make`命令可以只更新修改后的文件以及与之相关的所有文件，而对未修改的文件不做任何处理，从而提高效率。

<!--more-->

`make`命令的语法为：

```shell
make [options] [target] [VAR=VALUE]
```

其中`options`为命令行选项，可以用`make --help`来查看；`target`代表`make`命令需要完成的任务，如install，clean等（它与makefile的内容有关，下面会详细说明）；`[VAR=VALUE]`代表指定的环境变量。

当执行 `make` 时，`make` 会在当前的目录下搜寻 Makefile (or makefile) 这个文本文件，执行文件内容所对应的操作。当然，也可以使用`-f`或者`--file`参数，指定某个特定的文件当成makefile进行解析，如`make -f Make.Linux` 或 `make --file Make.AIX`。

常用的选项包括：

- `-B`：更新所有的目标
- `-f=, --file=`：使用指定的makefile
- `-j`：指同时运行命令的个数，如果没有这个参数则运行尽可能多的命令
- `-k`：出错也不停止运行
- `-n`：仅输出执行过程中的命令序列，但是并不执行
- `-p`：输出makefile的所有数据，包括所有的规则和变量
- `-s`：运行命令时不输出任何命令的输出



# Makefile

## 概述

与`make`命令相关联的文件为makefile，这一文件告诉`make`命令需要按照怎么样的规则去编译和链接程序。makefile文件由一系列的规则构成，每一条规则的形式如下：

```makefile
<target> : <prerequisites> 
[tab]  <commands>
```

每条规则中包含目标（target）、前置条件（prerequisites）和命令（commands）三部分。一条规则明确了构建目标的前置条件，以及如何去构建。例如下面是一个makefile的文件示例：

```makefile
edit : main.o kbd.o command.o display.o \
        insert.o search.o files.o utils.o
    cc -o edit main.o kbd.o command.o display.o \
        insert.o search.o files.o utils.o

main.o : main.c defs.h
    cc -c main.c
kbd.o : kbd.c defs.h command.h
    cc -c kbd.c
command.o : command.c defs.h command.h
    cc -c command.c
display.o : display.c defs.h buffer.h
    cc -c display.c
insert.o : insert.c defs.h buffer.h
    cc -c insert.c
search.o : search.c defs.h buffer.h
    cc -c search.c
files.o : files.c defs.h buffer.h command.h
    cc -c files.c
utils.o : utils.c defs.h
    cc -c utils.c
clean :
    rm edit main.o kbd.o command.o display.o \
        insert.o search.o files.o utils.o
```

## 目标

一个目标通常是一个文件名，指明`make`命令要构建的对象，例如上面示例中的`main.o`文件。

目标也可以是某个操作的名字（伪目标），例如上面示例的`clean`。当我们输入`make clean`，就会执行`clean`这个目标所对应的命令。但是按照这种写法，如果当前目录中有一个文件名称为`clean`，那么这个命令就不会执行，因为`make`发现`clean`文件已经存在，就认为没有必要重新构建。为了避免这种情况，可以使用`.PHONY`声明`clean`数据伪目标，写法如下：

```makefile
.PHONY: clean
clean:
        rm *.o
```

将其声明为伪目标之后，`make`就不会去检查`clean`这个文件是否存在，而是每次都会执行对应的命令。

## 前置条件

前置条件通常是一组文件名，之间用空格分隔。它指定了目标是否重新构建的判断标准：只要有一个前置文件不存在，或者有过更新（前置文件的last-modification时间戳比目标的时间戳新），目标就需要重新构建。

例如上面示例中`main.o`文件的生成：

```makefile
main.o : main.c defs.h
    cc -c main.c
```

在本例中，`main.o`的前置条件是`main.c`和`defs.h`两个文件，例如当这两个文件有过更新时，在执行make命令时便会执行所对应的命令。

## 命令

命令表示如何更新目标文件，由一行或者多行shell命令组成，如上例中的`cc -c main.c`。它是构建目标的具体指令，它的运行结果通常就是生成目标文件。

需要注意的是，每行命令之前必须有一个tab键。如果想用其他键，可以用内置变量`.RECIPEPREFIX`声明。同时每一行命令会在一个单独的shell中执行，这些shell之间没有任何的继承关系。因此如果多条命令之间互相关联，可以将这些命令写在一行，并使用`;`隔开；也或者是在一个规则之前加上`.ONESHELL:`命令。

## 工作流程

在默认的方式下，也就是只输入 `make` 命令，则会执行如下的步骤：

1. `make`会在当前目录下寻找名字为Makefile或makefile的文件。
2. 如果找到，它会执行其中的第一条规则，寻找文件中的第一个目标文件（target）。在上面的例子中，会找到edit这个文件，并把这个文件作为最终的目标文件。
3. 如果edit文件不存在，或是edit所依赖的后面的 `.o` 文件的文件修改时间要比 edit这个文件新，那么，他就会执行后面所定义的命令来生成 edit这个文件。
4. 如果edit所依赖的 `.o` 文件也不存在，那么`make`会在makefile中寻找目标为 `.o` 文件的依赖性，如果找到相应的规则，则再根据那一个规则生成 `.o` 文件。（这有点像一个堆栈的过程）
5. 当然，由于`.c`文件和`.h`文件是存在的，于是make会生成 `.o` 文件，然后再用 `.o` 文件生成make的主要目标，也就是执行文件 edit 。

这就是整个make的依赖性，make会一层又一层地去找文件的依赖关系，直到最终编译出第一个目标文件。在寻找的过程中，如果出现错误，比如最后被依赖的文件找不到，那么make就会直接退出并报错；而对于所定义的命令的错误，或是编译不成功，make便会忽略。make只考虑文件的依赖性，也就是说，如果在寻找到依赖关系并执行之后，冒号后面的文件还是不存在，那么make命令就无法成功执行。



# Makefile语法

## 注释

在makefile文件中，`#`表示注释。如：

```makefile
# 这是注释
result.txt: source.txt
    # 这是注释
    cp source.txt result.txt # 这也是注释
```

## 回声

正常情况下，make会将每一条命令都打印出来（包括注释），然后再执行。如果不想打印某些命令，则可以在命令的前面加上`@`符号。

## 通配符与模式匹配

makefile的通配符与bash一致，支持`*` ， `?` 和 `~`这三个通配符。例如在命令中支持如`rm -f *.o`这样的写法。

此外，make命令允许对文件名进行类似正则运算的匹配，主要用到的匹配符是`%`。比如，假定当前目录下有 f1.c 和 f2.c 两个源码文件，`%.o: %.c`等同于：

```makefile
f1.o: f1.c
f2.o: f2.c
```

## 变量与赋值

### 自定义变量

makefile允许使用等号自定义变量，然后使用`$()`来引用自定义的变量，例如：

```makefile
txt = Hello World
test:
    @echo $(txt)
```

makefile的赋值运算符有四种，它们的用法如下：

```makefile
VARIABLE = value
# 在执行时扩展，允许递归扩展。
VARIABLE := value
# 在定义时扩展。
VARIABLE ?= value
# 只有在该变量为空时才设置值。
VARIABLE += value
# 将值追加到变量的尾端。
```

### 内置变量

Make命令提供一系列内置变量，比如，\$(CC) 指向当前使用的C语言编译器，\$(MAKE) 指向当前使用的Make工具。详细的说明见：https://www.gnu.org/software/make/manual/html_node/Implicit-Variables.html

### 自动变量

make命令有一些自动变量，它们的值与自动变量所在的规则有关，详情可参考https://www.gnu.org/software/make/manual/html_node/Automatic-Variables.html。常用的主要有：

- `$@`：指代当前规则的目标，例如

  ```makefile
  a.txt: 
      touch $@
  ```

  中的`$@`就指代`a.txt`

- `$<`：指代第一个前置条件，例如规则为 `t: p1 p2`，那么`$< `就指代`p1`

- `$?`：指代比目标更新的所有前置条件，之间用空格分割。比如规则为`t: p1 p2`，其中 `p2`的时间戳比`t`新，`$?`就指代`p2`

- `$^`：指代所有前置条件，之间用空格分割。比如规则为`t: p1 p2`，`$^`就指代`p1 p2`

- `$*`：指代匹配符`%`匹配的部分，比如`%`匹配`f1.txt`中的`f1` ，`$*`就表示`f1`

- `$(@D)`和`$(@F)`：指向`$@`的目录名和文件名，比如`$@`是 `src/input.c`，那么`$(@D)`的值为`src` ，`$(@F)`的值为`input.c`

- `$(<D)`和`$(<F)`：`$(<D)`和`$(<F)`分别指向`$<`的目录名和文件名。 

## 条件判断

makefile支持条件判断表达式，让`make`根据运行时的不同情况选择不同的执行分支。例如：

```makefile
libs_for_gcc = -lgnu
normal_libs =

ifeq ($(CC),gcc)
    libs=$(libs_for_gcc)
else
    libs=$(normal_libs)
endif

foo: $(objects)
    $(CC) -o foo $(objects) $(libs)
```

条件表达式的语法为:

```makefile
<conditional-directive>
<text-if-true>
endif
```

以及:

```makefile
<conditional-directive>
<text-if-true>
else
<text-if-false>
endif
```

其中 `<conditional-directive>` 表示条件关键字，这个关键字有四个:

- `ifeq`：

  ```makefile
  ifeq (<arg1>, <arg2>)
  ifeq '<arg1>' '<arg2>'
  ifeq "<arg1>" "<arg2>"
  ifeq "<arg1>" '<arg2>'
  ifeq '<arg1>' "<arg2>"
  ```

  它比较参数 `arg1` 和 `arg2` 的值是否相同。

- `ifneq`：

  ```makefile
  ifneq (<arg1>, <arg2>)
  ifneq '<arg1>' '<arg2>'
  ifneq "<arg1>" "<arg2>"
  ifneq "<arg1>" '<arg2>'
  ifneq '<arg1>' "<arg2>"
  ```

  它比较参数 `arg1` 和 `arg2` 的值是否不同。

- `ifdef`：

  ```makefile
  ifdef <variable-name>
  ```

  如果变量 `<variable-name>` 的值非空，那到表达式为真。否则，表达式为假。当然， `<variable-name>` 同样可以是一个函数的返回值。注意， `ifdef` 只是测试一个变量是否有值，其并不会把变量扩展到当前位置。

- `ifndef`：

  ```makefile
  ifndef <variable-name>
  ```

  它与`ifdef`的意思相反。

## 函数

makefile支持使用函数，语法格式为：`$(function arguments)`或`${function arguments}`。

makefile提供了一些内置函数，详情可参考：https://www.gnu.org/software/make/manual/html_node/Functions.html ，https://seisman.github.io/how-to-write-makefile/functions.html



# 附：程序的编译和链接

## 编译过程

无论是C还是C++，在编译程序文件时，首先要把源文件编译成中间代码文件，在Windows下也就是 `.obj` 文件，UNIX下是 `.o` 文件，即Object File，这个动作叫做编译（compile）。然后再把大量的Object File合成执行文件，这个动作叫作链接（link）。

在编译时，编译器需要的是语法的正确，函数与变量的声明的正确。因此，如果某些函数和变量的声明在头文件中，则需要在编译的时候指明头文件所在的位置。一般来说，每个源文件都应该对应于一个中间目标文件（ `.o` 文件或 `.obj` 文件）。

而在链接时，主要是链接函数和全局变量。所以，我们可以使用这些中间目标文件（ `.o` 文件或 `.obj` 文件）来链接我们的应用程序。链接器使用函数与全局变量所对应的中间目标文件（Object  File）来完成链接过程。

## 静态库与动态库

在大多数时候，由于源文件太多，编译生成的中间目标文件太多，而在链接时需要明显地指出中间目标文件名，这对于编译很不方便。所以，我们可以给中间目标文件打包，将其构造为函数链接库。

函数库分为静态和动态两种。对于静态库来说，在链接步骤中，连接器将从库文件取得所需的代码，复制到生成的可执行文件中。其特点是可执行文件中包含了库代码的一份完整拷贝，生成的可执行文件与函数库没有任何关系，可以方便移植，但缺点是被多次使用就会多份冗余拷贝；而动态库独立于现有的程序，其本身不可执行，但包含着程序需要调用的一些函数。对于动态库来说，程序在开始运行后调用库函数时才被载入。

 在widows平台下，静态链接库是.lib文件，动态库文件是.dll文件。在linux平台下，静态链接库是.a文件，动态链接库是.so文件。

## g++的编译参数

下面为g++常用的编译参数：

- 目录选项：
  - `-L` ：表示要链接的库所在的目录。例如 -L.  表示要链接的库在当前目录， `-L/usr/lib` 表示要连接的库在/usr/lib下。系统会自动搜索目录/usr/lib，可以不用指明。
  - `-l` ：表示需要链接库的名称，注意不是库文件名称，比如库文件为 libtest.so，那么库名称为test，编译参数需要写为`-Itest`
  - `-I `：指定头文件的所在的目录，可以使用相对路径。代码中包含的头文件便会在这一目录中寻找。
  - `-i`：指定程序中所使用的头文件，功能相当于在代码中使用#include

- 编译选项：

  - `-E`：只进行预处理，例如宏的替换、注释消除、找相关的库文件等，生成`.i`文件
  - `-S`：在编译时只生成`.s`的汇编文件
  - `-c`：在编译时只生成`.o`的obj文件
  - `-shared` ：指定生成动态链接库。例如下面的命令可以创建一个动态链接库：`g++ -fPIC -shared -o libxxx.so xx1.cpp xx2.cpp xx3.cpp`。创建静态链接库则需要使用`ar`命令，例如：` ar  cqs  libxxxx.a  xx1.o xx2.o xx3.o `。

  - `-fPIC`：表示编译为位置无关的代码。不用此选项的话编译后的代码是位置相关的，在动态载入时是通过代码拷贝的方式来满足不同进程的需要，而不能达到真正代码共享的目的。
  - `-static`：生成静态代码，不使用动态库。这样方便生成的代码进行移植，但是文件较大。

- 调试、错误与告警选项：

  - `-g`：gcc 支持4中级别的调试信息，`-g0`表示不生成调试信息，`-g3`表示生成最多的调试信息。`-g`默认为`-g2`。一般的调试信息包括行号，函数，外部变量。`-g3`包含其他额外的调试信息，比如宏定义。
  - `-Wall`：输出所有的警告信息
  - `-pedantic`：允许发出ANSI/ISO C标准所列出的所有警告
  - `-pedantic-errors`：允许发出ANSI/ISO C标准所列出的错误
  - `-w`：关闭所有警告
  - `-werror`：将所有警告当成错误来处理

- 其它：

  - `-o`：设置输出文件的名称，如果不设置默认为a.out
  - `-std`：设置C++的标准，如`-std=c++11`，`std=c++17`等
  - `-O`：主要用于优化代码。分为0-3四个级别，数字越大则优化级别越高



# 参考

1. https://seisman.github.io/how-to-write-makefile/overview.html
2. https://www.ruanyifeng.com/blog/2015/02/make.html
3. https://blog.csdn.net/surgewong/article/details/39236707/