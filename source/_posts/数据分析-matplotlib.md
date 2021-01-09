---
title: 数据分析-matplotlib
tags: 
- 数据分析
categories: [数据分析]
sharing: True

---

# 简介

## 常用网址

官网主页：https://matplotlib.org/index.html （文中一些函数的具体用法可以在这一页面内查找其详细用法）

教程：https://matplotlib.org/tutorials/index.html （提供了一些常见图像的绘图示例）

示例库：https://matplotlib.org/gallery/index.html （包含了更多种类的绘图示例）

<!--more-->

## 图像结构

下图所示为matplotlib中的一副图像的结构。可以看到整个图被称为一个**figure**，可以把它看作是一张画布。在figure内，又可以分布着一个或者多个**Axes**（坐标系），相当于是画布上的一个个区域。而一个Axes又是由很多个不同的**元素**组成的。

![图像结构](https://raw.githubusercontent.com/lyf35/blog_figures/main/img/20210109161129.jpg)

## 面向对象作图

Matplotlib提供了两种接口：
- 基于Matlab的：自动创建和管理图和坐标系；
- 基于面向对象的：显式创建图和坐标系。

基于Matlab的方法由matplotlib.pyplot提供，这样绘图比较简单，但是却容易混乱。因此，为了作图的高效，推荐以面向对象的思想进行作图。

Matplotlib中的对象共分为两大类（可参照图像结构）：
- 容器类：图(figure)、坐标系(axes)、坐标轴(axis)、刻度(tick)。
- 基础类：线(line)、点(marker)、文字(text)、图例(legend)、网格(grid)、标题(title)。

面向对象的作图方式大致可以分为下面三个步骤：
1. 创建figure实例
2. 在figure上创建axes
3. 在axes上添加基础类对象

## 作图步骤

### 创建容器类对象


```python
import matplotlib.pyplot as plt
import matplotlib
import numpy as np

#方法1：同时创建figure和axes
fig, axes=plt.subplots(2,3) #创建一幅图以及其中的坐标系，其中subplots函数可以传入两个正整数（可选，如果不传入则默认为1），代表在竖向和横向上将figure均分成若干份。
ax1=axes[0][1] #可以通过索引来获得其中一个坐标系

#注意：如果两个参数都为1，那么axes就直接是一个坐标系；如果subplots中的某一个或者两个参数不为1，那么axes就是一个一维或者二维的list，list中的元素即为对应的坐标系。
#错误操作1：
#fig, axes=plt.subplots(1,1)
#ax1=axes[0] axes已经是一个object，不能再索引
#错误操作2：
#fig, axes=plt.subplots(1,5)
#ax1=axes[0][0] axes是一个一维的list，不能索引两次
```

![png](https://raw.githubusercontent.com/lyf35/blog_figures/main/img/20210109161141.png)


在上图中，我们可以看到子图的一些内容有重叠，这是因为默认情况下`constrained_layout`参数为`False`。因此绘图时不会自动对图像的元素进行位置和大小的调整，从而导致子图的一些元素放置在了subplot之外。

如果要避免这一现象，可以在创建subplot的时候传入`constrained_layout=True`，这样便会自动对图像元素进行调整，从而保证图像内容可以全部处于subplot之内。更加详细的说明参考https://matplotlib.org/tutorials/intermediate/constrainedlayout_guide.html#sphx-glr-tutorials-intermediate-constrainedlayout-guide-py

当然，也可以直接通过`plt.rcParams['figure.constrained_layout.use'] = True`这一语句完成全局设置，这样的话之后创建的每一幅图都会自动调整图像布局。

另外一种类似的用法叫做Tight Layout：https://matplotlib.org/tutorials/intermediate/tight_layout_guide.html#sphx-glr-tutorials-intermediate-tight-layout-guide-py 。但是这种用法相比于Constrained Layout更容易出错，不推荐使用。


```python
fig, axes=plt.subplots(2,3,constrained_layout=True) 
```


![png](https://raw.githubusercontent.com/lyf35/blog_figures/main/img/20210109161205.png)



```python
#方法2：将figure和axes的创建分开
#figure更细致的划分需要用到GridSpec，参考https://matplotlib.org/tutorials/intermediate/gridspec.html#sphx-glr-tutorials-intermediate-gridspec-py
fig=plt.figure(num=1,figsize=(4,4)) #相当于创建一张白纸
ax1=fig.add_subplot(221) #前两个数字代表分成m*n的小块，第三个数字代表axes在第几个小块中绘制
ax2=fig.add_subplot(222) 
ax3=fig.add_subplot(111) #后添加的axes会覆盖之前已经添加的axes，不受之前add_subplot()函数的影响
```


![png](https://raw.githubusercontent.com/lyf35/blog_figures/main/img/20210109161214.png)


### 添加基础类对象


```python
#方法1：调用函数添加
fig, ax=plt.subplots()
#图
fig.legend("This is a test") #图-图例

#坐标系
ax.plot(np.array([(0,0),(0.2,0.5),(0.4,0.6)]),label='label') #坐标系-线
ax.scatter(np.array([0,0.3,0.6]),np.array([0.5,0.2,0.4])) #坐标系-点
ax.grid() #坐标系-网格
ax.legend() #坐标系-图例
#图例的详细设置参考：
#https://matplotlib.org/tutorials/intermediate/legend_guide.html#sphx-glr-tutorials-intermediate-legend-guide-py

#插入文本（包括标题、坐标轴名称等），在图像内插入的文本支持Tex语法
#matplotlib.rcParams['text.usetex'] = True #注：如果要在图片中插入Tex格式的文本，需要先打开这一功能
ax.text(0.5,0.2,r'$\mu=100$ test') #坐标系-文字，前两个参数代表文字在x和y方向的相对位置，第三个参数代表插入的文字内容。
ax.set_title(r'Title $\frac{1}{1}$') #坐标系-标题

#坐标轴
ax.set_xlabel(r'xlabel $\sqrt{2},\ \sqrt[3]{x},\ \ldots$') #坐标系-坐标轴-x轴标签
ax.set_ylabel('ylabel') #坐标系-坐标轴-y轴标签

#刻度
ax.set_xticklabels(['one', 'two', 'three', 'four', 'five', r'$\coprod$', r'$\int$']) # 坐标系-坐标轴-刻度-x轴刻度标签
ax.set_yticklabels([1, 2, 3, 4, 5]) # 坐标系-坐标轴-刻度-y轴刻度标签

#图像的注解
ax.annotate('local max', xy=(0.2, 0.2),  xycoords='data',
            xytext=(0.8, 0.95), textcoords='axes fraction',
            arrowprops=dict(facecolor='black', shrink=0.05),
            horizontalalignment='right', verticalalignment='top',
            )
#关于注解的详细设置可参考：
#https://matplotlib.org/tutorials/text/annotations.html#sphx-glr-tutorials-text-annotations-py
```


![png](https://raw.githubusercontent.com/lyf35/blog_figures/main/img/20210109161339.png)



```python
#方法2：构造一个字典，包含对应的关键词，然后将其作为参数传入到ax.set()函数
fig, ax=plt.subplots()
props = {'title': 'Title', # 坐标系-标题
      'xlabel': 'xlabel', # 坐标系-坐标轴-标签
      'xticklabels':[0,1,2,3,4,5]} # 坐标系-坐标轴-刻度-标签
ax.set(**props)
```


![png](https://raw.githubusercontent.com/lyf35/blog_figures/main/img/20210109161429.png)


### 图像美化

图像美化的方法有两种：一种是调用`plt.style.use()`，使用官方预定义的格式；另外一种方法是通过`matplotlib.rcParams`自定义。

方法一：通过`rcParams`手动设置。一些参数的含义可以查看https://matplotlib.org/tutorials/introductory/customizing.html#matplotlibrc-sample


```python
#matplotlib.rcParams.keys() #用matplotlib.rcParams.keys()可以查看所有可以全局定义的属性，因为属性太多故此处不做显示
```


```python
params = {
        "font.size": 12,     # 全局字号
        'font.family':'Arial', # 全局字体
        "axes.spines.right":True,  # 坐标系-右侧线
        "axes.spines.top":True,   # 坐标系-上侧线
        "axes.titlesize":12,   # 坐标系-标题-字号
        "axes.labelsize": 12,  # 坐标系-标签-字号
        "legend.fontsize": 12,  # 图例-字号
        "xtick.labelsize": 10,  # 刻度-标签-字号
        "ytick.labelsize": 10,  # 刻度-标签-字号
        "xtick.direction":'in',   # 刻度-方向
        "ytick.direction":'in'  # 刻度-方向
}
matplotlib.rcParams.update(**params)
#或者按照matplotlib.rcParams['key']=value这种格式也可以进行调整

matplotlib.rc_file_defaults() #还原为本次导入matplotlib时的默认设置
#matplotlib.rcdefaults() #还原为matplotlib的默认绘图设置，类似于恢复出厂设置
```

方法二：使用预定义格式。预定义格式的效果图可以参考https://matplotlib.org/gallery/style_sheets/style_sheets_reference.html#sphx-glr-gallery-style-sheets-style-sheets-reference-py


```python
#plt.style.available #plt.style.available可以查看所有的可用样式
```


```python
plt.style.use('seaborn-paper') 

#如果只想对于某幅图片使用一种格式，可以用下面的命令，将绘图操作写入with代码块里面
#with plt.style.context('seaborn'):  #将use换成context
    #plotting commands
```

### 保存图像

要保存生成的图像，可以使用如下命令。图像支持多种格式的保存，可以使用`fig.canvas.get_supported_filetypes()`来查看支持的图片格式类型。


```python
fig.savefig('sales.png', transparent=False, dpi=80, bbox_inches="tight") 
#上面几个为常用的参数，它们的含义如下：
#transparent：代表图片背景是否透明
#dpi：输出图像的分辨率
#bbox_inches：代表图片边缘的留白如何处理，'tight'表示除去图片边缘的所有留白
```

# 图像格式

## 色彩

### 色彩表示

在绘图或者是添加文字时，可以加入`color=`参数，来表明文字/散点/曲线等的颜色。颜色的定义方法有很多，包括但不限于：

- 传入一个表示RGB或者RGBA的tuple，其中的每个元素为0到1中间的浮点数，例如[0.2,0.5,0.6,0.9]
- 传入一个表示RGB或者RGBA的十六进制格式字符串，例如'#aabbccdd'
- 传入{'b', 'g', 'r', 'c', 'm', 'y', 'k', 'w'}的其中一个，它们对应于blue, green, red, cyan, magenta, yellow, black, and white.

更多的颜色表示方法可参考：

https://matplotlib.org/tutorials/colors/colors.html#sphx-glr-tutorials-colors-colors-py 

https://matplotlib.org/gallery/color/named_colors.html#sphx-glr-gallery-color-named-colors-py


```python
t = np.linspace(0.0, 2.0, 201)
s = np.sin(2 * np.pi * t)

# 1) RGB tuple:
fig, ax = plt.subplots(facecolor=(.18, .31, .31))
# 2) hex string:
ax.set_facecolor('#eafff5')
# 3) gray level string:
ax.set_title('Voltage vs. time chart', color='0.7')
# 4) single letter color string
ax.set_xlabel('time (s)', color='c')
# 5) a named color:
ax.set_ylabel('voltage (mV)', color='peachpuff')
# 6) a named xkcd color:
ax.plot(t, s, 'xkcd:crimson')
# 7) Cn notation:
ax.plot(t, .7*s, color='C4', linestyle='--')
# 8) tab notation:
ax.tick_params(labelcolor='tab:orange')
```


![png](https://raw.githubusercontent.com/lyf35/blog_figures/main/img/20210109161422.png)


### colormap

在绘制一些图像如轮廓图、流形图等的时候，需要传入`cmap`参数，即使用一个colormap。colormap有一些系统定义好的，也可以自己定义。详细的用法可参考：

https://matplotlib.org/tutorials/colors/colorbar_only.html#sphx-glr-tutorials-colors-colorbar-only-py

https://matplotlib.org/tutorials/colors/colormap-manipulation.html#sphx-glr-tutorials-colors-colormap-manipulation-py

https://matplotlib.org/tutorials/colors/colormapnorms.html#sphx-glr-tutorials-colors-colormapnorms-py

https://matplotlib.org/tutorials/colors/colormaps.html#sphx-glr-tutorials-colors-colormaps-py

https://matplotlib.org/gallery/color/custom_cmap.html#sphx-glr-gallery-color-custom-cmap-py

### 色彩循环

如果在一幅图里面同时绘制多条曲线或者多种散点，同时绘图时又没有手动指定颜色的话，那么系统会使用默认的色彩循环方式，将它们区分开来。如果要对这些设置进行调整，可以参考：

https://matplotlib.org/gallery/color/color_cycle_default.html#sphx-glr-gallery-color-color-cycle-default-py

https://matplotlib.org/tutorials/intermediate/color_cycle.html#sphx-glr-tutorials-intermediate-color-cycle-py


## 线形

在绘制曲线的时候，可以使用`linestyle`参数设置不同的线型，这些不同的线形可以参考：https://matplotlib.org/gallery/lines_bars_and_markers/linestyles.html#sphx-glr-gallery-lines-bars-and-markers-linestyles-py

对于四种比较常见的线形，专门设置了比较简便的表示方法：'-'表示直线，'.'表示点，'--'表示虚线，'-.'表示点划线

## 标记

在绘制点的时候（例如散点图），可以使用`marker`参数设置不同的标记形状。标记形状可以自己定义，也可以使用系统自带的一些标记，可参考https://matplotlib.org/gallery/lines_bars_and_markers/marker_reference.html#sphx-glr-gallery-lines-bars-and-markers-marker-reference-py

对于标记来说，它们的大小、颜色、填充方式等信息可以自己设置，可以通过参数`markersize`, `markerfacecolor`, `markeredgecolor`, `fillstyle`, `markerfacecoloralt`等进行调整。

## 文本

在图中插入的图注、坐标轴、文本、标题等都属于文本。这些文本的颜色、字体、大小等信息都可以调整。关于文本内容的详细设置可以参考：

- 不同种类的文本：https://matplotlib.org/tutorials/text/text_intro.html#sphx-glr-tutorials-text-text-intro-py
- 文本格式设置：https://matplotlib.org/tutorials/text/text_props.html#sphx-glr-tutorials-text-text-props-py
- Tex写数学表达式：https://matplotlib.org/tutorials/text/mathtext.html#sphx-glr-tutorials-text-mathtext-py


```python
fig = plt.figure()
ax = fig.add_subplot(111)
fig.subplots_adjust(top=0.85)

# Set titles for the figure and the subplot respectively
fig.suptitle('bold figure suptitle', fontsize=14, fontweight='bold')
ax.set_title('axes title')

ax.set_xlabel('xlabel')
ax.set_ylabel('ylabel')

# Set both x- and y-axis limits to [0, 10] instead of default [0, 1]
ax.axis([0, 10, 0, 10])

ax.text(3, 8, 'boxed italics text in data coords', style='italic',
        bbox={'facecolor': 'red', 'alpha': 0.5, 'pad': 10})

ax.text(2, 6, r'an equation: $E=mc^2$', fontsize=15)

ax.text(3, 2, 'unicode: Institut für Festkörperphysik')

ax.text(0.95, 0.01, 'colored text in axes coords',
        verticalalignment='bottom', horizontalalignment='right',
        transform=ax.transAxes,
        color='green', fontsize=15)

ax.plot([2], [1], 'o')
ax.annotate('annotate', xy=(2, 1), xytext=(3, 4),
            arrowprops=dict(facecolor='black', shrink=0.05))
```


![png](https://raw.githubusercontent.com/lyf35/blog_figures/main/img/20210109161441.png)


# 绘图示例

下面这些绘图的代码示例来自于官方教程，包含了一些比较常见的绘图操作。在https://matplotlib.org/gallery/index.html#color-examples 中可以找到更多更全的绘图示例。

## 曲线图


```python
x1 = np.linspace(0.0, 5.0)
x2 = np.linspace(0.0, 2.0)

y1 = np.cos(2 * np.pi * x1) * np.exp(-x1)
y2 = np.cos(2 * np.pi * x2)

fig, (ax1, ax2) = plt.subplots(2, 1)
fig.suptitle('A tale of 2 subplots')

ax1.plot(x1, y1, 'o-') #第三个参数对应于点和线的格式，下同
ax1.set_ylabel('Damped oscillation')

ax2.plot(x2, y2, '.-')
ax2.set_xlabel('time (s)')
ax2.set_ylabel('Undamped')
```




![png](https://raw.githubusercontent.com/lyf35/blog_figures/main/img/20210109161447.png)


## 伪彩色图与轮廓图

在绘制伪彩色图时，传入的x和y对应的是网格的边界，之后会自动根据传入的网格边界将二维平面进行划分（边界可以为斜线）。而传入的z值则对应于每个网格的值，网格颜色便会使用这些值来进行染色。

而绘制轮廓图时，传入的x和y对应的是二维平面的一个个点，z对应的是这些点的值。


```python
# make these smaller to increase the resolution
dx, dy = 0.05, 0.05

# generate 2 2d grids for the x & y bounds
y, x = np.mgrid[slice(1, 5 + dy, dy), slice(1, 5 + dx, dx)]

z = np.sin(x)**10 + np.cos(10 + y*x) * np.cos(x)

# x and y are bounds, so z should be the value *inside* those bounds.
# Therefore, remove the last value from the z array.
z = z[:-1, :-1]
levels = matplotlib.ticker.MaxNLocator(nbins=15).tick_values(z.min(), z.max())

# pick the desired colormap, sensible levels, and define a normalization
# instance which takes data values and translates those into levels.
cmap = plt.get_cmap('PiYG')
norm = matplotlib.colors.BoundaryNorm(levels, ncolors=cmap.N, clip=True)

fig, (ax0, ax1) = plt.subplots(nrows=2)

im = ax0.pcolormesh(x, y, z, cmap=cmap, norm=norm)
fig.colorbar(im, ax=ax0)
ax0.set_title('pcolormesh with levels')

# contours are *point* based plots, so convert our bound into point centers
cf = ax1.contourf(x[:-1, :-1] + dx/2., y[:-1, :-1] + dy/2., z, levels=levels, cmap=cmap)
fig.colorbar(cf, ax=ax1)
ax1.set_title('contourf with levels')

# adjust spacing between subplots so `ax1` title and `ax0` tick labels
# don't overlap
fig.tight_layout()
```


![png](https://raw.githubusercontent.com/lyf35/blog_figures/main/img/20210109161500.png)


## 直方图


```python
# example data
mu = 100  # mean of distribution
sigma = 15  # standard deviation of distribution
x = mu + sigma * np.random.randn(1000)

num_bins = 50

fig, ax = plt.subplots()

# the histogram of the data
n, bins, patches = ax.hist(x, num_bins, density=True)

# add a 'best fit' line
y = ((1 / (np.sqrt(2 * np.pi) * sigma)) * np.exp(-0.5 * (1 / sigma * (bins - mu))**2))
ax.plot(bins, y, '--')
ax.set_xlabel('Smarts')
ax.set_ylabel('Probability density')
ax.set_title(r'Histogram of IQ: $\mu=100$, $\sigma=15$')

# Tweak spacing to prevent clipping of ylabel
fig.tight_layout()
```


![png](https://raw.githubusercontent.com/lyf35/blog_figures/main/img/20210109161505.png)


## 三维图


```python
from mpl_toolkits.mplot3d import Axes3D
fig, ax = plt.subplots(subplot_kw={"projection": "3d"})

# Make data.
X = np.arange(-5, 5, 0.25)
Y = np.arange(-5, 5, 0.25)
X, Y = np.meshgrid(X, Y)
R = np.sqrt(X**2 + Y**2)
Z = np.sin(R)

# Plot the surface.
surf = ax.plot_surface(X, Y, Z, cmap=matplotlib.cm.coolwarm, linewidth=0, antialiased=False)

# Customize the z axis.
ax.set_zlim(-1.01, 1.01)
ax.zaxis.set_major_locator(matplotlib.ticker.LinearLocator(10))

# Add a color bar which maps values to colors.
fig.colorbar(surf, shrink=0.5, aspect=5)
```


![png](https://raw.githubusercontent.com/lyf35/blog_figures/main/img/20210109161517.png)


## 流形图


```python
w = 3
Y, X = np.mgrid[-w:w:100j, -w:w:100j]
U = -1 - X**2 + Y
V = 1 + X - Y**2
speed = np.sqrt(U**2 + V**2)

fig = plt.figure(figsize=(7, 9))
gs = matplotlib.gridspec.GridSpec(nrows=3, ncols=2, height_ratios=[1, 1, 2])

#  Varying density along a streamline
ax0 = fig.add_subplot(gs[0, 0])
ax0.streamplot(X, Y, U, V, density=[0.5, 1])
ax0.set_title('Varying Density')

# Varying color along a streamline
ax1 = fig.add_subplot(gs[0, 1])
strm = ax1.streamplot(X, Y, U, V, color=U, linewidth=2, cmap='autumn')
fig.colorbar(strm.lines)
ax1.set_title('Varying Color')

#  Varying line width along a streamline
ax2 = fig.add_subplot(gs[1, 0])
lw = 5*speed / speed.max()
ax2.streamplot(X, Y, U, V, density=0.6, color='k', linewidth=lw)
ax2.set_title('Varying Line Width')

# Controlling the starting points of the streamlines
seed_points = np.array([[-2, -1, 0, 1, 2, -1], [-2, -1,  0, 1, 2, 2]])

ax3 = fig.add_subplot(gs[1, 1])
strm = ax3.streamplot(X, Y, U, V, color=U, linewidth=2,
                      cmap='autumn', start_points=seed_points.T)
fig.colorbar(strm.lines)
ax3.set_title('Controlling Starting Points')

# Displaying the starting points with blue symbols.
ax3.plot(seed_points[0], seed_points[1], 'bo')
ax3.set(xlim=(-w, w), ylim=(-w, w))

# Create a mask
mask = np.zeros(U.shape, dtype=bool)
mask[40:60, 40:60] = True
U[:20, :20] = np.nan
U = np.ma.array(U, mask=mask)

ax4 = fig.add_subplot(gs[2:, :])
ax4.streamplot(X, Y, U, V, color='r')
ax4.set_title('Streamplot with Masking')

ax4.imshow(~mask, extent=(-w, w, -w, w), alpha=0.5, cmap='gray', aspect='auto')
ax4.set_aspect('equal')

plt.tight_layout()
```


![png](https://raw.githubusercontent.com/lyf35/blog_figures/main/img/20210109161523.png)


## 条形图


```python
data = [[ 66386, 174296,  75131, 577908,  32015],
        [ 58230, 381139,  78045,  99308, 160454],
        [ 89135,  80552, 152558, 497981, 603535],
        [ 78415,  81858, 150656, 193263,  69638],
        [139361, 331509, 343164, 781380,  52269]]

columns = ('Freeze', 'Wind', 'Flood', 'Quake', 'Hail')
rows = ['%d year' % x for x in (100, 50, 20, 10, 5)]

values = np.arange(0, 2500, 500)
value_increment = 1000

# Get some pastel shades for the colors
colors = plt.cm.BuPu(np.linspace(0, 0.5, len(rows)))
n_rows = len(data)

index = np.arange(len(columns)) + 0.3
bar_width = 0.4

# Initialize the vertical-offset for the stacked bar chart.
y_offset = np.zeros(len(columns))

# Plot bars and create text labels for the table
cell_text = []
for row in range(n_rows):
    plt.bar(index, data[row], bar_width, bottom=y_offset, color=colors[row])
    #plt.bar(index, data[row], bar_width, bottom=y_offset, color=colors[row], tick_label=columns) #绘制正常的条形图
    #plt.barh(index, data[row], bar_width, left=y_offset, color=colors[row], tick_label=columns) #绘制水平条形图，下面代码中的x、y轴标签等信息也需要修改
    y_offset = y_offset + data[row]
    cell_text.append(['%1.1f' % (x / 1000.0) for x in y_offset])
# Reverse colors and text labels to display the last value at the top.
colors = colors[::-1]
cell_text.reverse()

#在横轴下方加入表格，不想加入表格的话可以注释掉
the_table = plt.table(cellText=cell_text,
                      rowLabels=rows,
                      rowColours=colors,
                      colLabels=columns,
                      loc='bottom')


# Adjust layout to make room for the table:
plt.subplots_adjust(left=0.2, bottom=0.2)

plt.ylabel("Loss in ${0}'s".format(value_increment))
plt.yticks(values * value_increment, ['%d' % val for val in values])
plt.xticks([]) #如果不加入表格的话，可以注释掉这一行
plt.title('Loss by Disaster')

plt.show()
```


![png](https://raw.githubusercontent.com/lyf35/blog_figures/main/img/20210109161530.png)


## 饼图


```python
# Pie chart, where the slices will be ordered and plotted counter-clockwise:
labels = 'Frogs', 'Hogs', 'Dogs', 'Logs'
sizes = [15, 30, 45, 10]
explode = (0, 0.1, 0.2, 0) #代表是否将某一个扇形区域分开

fig1, ax1 = plt.subplots()
ax1.pie(sizes, explode=explode, labels=labels, autopct='%1.1f%%',
        shadow=True, startangle=90)
ax1.axis('equal')  # Equal aspect ratio ensures that pie is drawn as a circle.
```


![png](https://raw.githubusercontent.com/lyf35/blog_figures/main/img/20210109161535.png)

## 散点图

```python
volume = np.random.rayleigh(27, size=40)
amount = np.random.poisson(10, size=40)
ranking = np.random.normal(size=40)
price = np.random.uniform(1, 10, size=40)

fig, ax = plt.subplots()

# Because the price is much too small when being provided as size for ``s``, 
# we normalize it to some useful point sizes, s=0.3*(price*3)**2
scatter = ax.scatter(volume, amount, c=ranking, s=0.3*(price*3)**2, vmin=-3, vmax=3, cmap="Spectral")

# Produce a legend for the ranking (colors). Even though there are 40 different
# rankings, we only want to show 5 of them in the legend.
legend1 = ax.legend(*scatter.legend_elements(num=5), loc="upper left", title="Ranking")
ax.add_artist(legend1)

# Produce a legend for the price (sizes). Because we want to show the prices
# in dollars, we use the *func* argument to supply the inverse of the function
# used to calculate the sizes from above. The *fmt* ensures to show the price
# in dollars. Note how we target at 5 elements here, but obtain only 4 in the
# created legend due to the automatic round prices that are chosen for us.
kw = dict(prop="sizes", num=5, color=scatter.cmap(0.7), fmt="$ {x:.2f}", func=lambda s: np.sqrt(s/.3)/3)
legend2 = ax.legend(*scatter.legend_elements(**kw), loc="lower right", title="Price")

plt.show()
```


![png](https://raw.githubusercontent.com/lyf35/blog_figures/main/img/20210109161549.png)


## 对数坐标


```python
# 注：成功运行这段程序需要更新matplotlib的版本
# Data for plotting
t = np.arange(0.01, 20.0, 0.01)

# Create figure
fig, ((ax1, ax2), (ax3, ax4)) = plt.subplots(2, 2)

# log y axis
ax1.semilogy(t, np.exp(-t / 5.0))
ax1.set(title='semilogy')
ax1.grid()

# log x axis
ax2.semilogx(t, np.sin(2 * np.pi * t))
ax2.set(title='semilogx')
ax2.grid()

# log x and y axis
ax3.loglog(t, 20 * np.exp(-t / 10.0))
ax3.set_xscale('log', base=2)
ax3.set(title='loglog base 2 on x')
ax3.grid()

# With errorbars: clip non-positive values
# Use new data for plotting
x = 10.0**np.linspace(0.0, 2.0, 20)
y = x**2.0

ax4.set_xscale("log", nonpositive='clip')
ax4.set_yscale("log", nonpositive='clip')
ax4.set(title='Errorbars go negative')
ax4.errorbar(x, y, xerr=0.1 * x, yerr=5.0 + 0.75 * y)
# ylim must be set after errorbar to allow errorbar to autoscale limits
ax4.set_ylim(bottom=0.1)

fig.tight_layout()
```


![png](https://raw.githubusercontent.com/lyf35/blog_figures/main/img/20210109161557.png)


## 极坐标


```python
r = np.arange(0, 2, 0.01)
theta = 2 * np.pi * r

fig, ax = plt.subplots(subplot_kw={'projection': 'polar'})
ax.plot(theta, r)
ax.set_rmax(2)
ax.set_rticks([0.5, 1, 1.5, 2])  # Less radial ticks
ax.set_rlabel_position(-22.5)  # Move radial labels away from plotted line
ax.grid(True)

ax.set_title("A line plot on a polar axis", va='bottom')
```


![png](https://raw.githubusercontent.com/lyf35/blog_figures/main/img/20210109161605.png)



# 相关绘图工具

## Seaborn

Seaborn是一个基于matplotlib的绘图函数库，它在牺牲了一定灵活性的前提下，将一些统计相关的常用绘图操作封装为一个函数，从而使得作图变得更加方便。例如使用seaborn可以容易地绘制出箱线图、矩阵散点图、热力图、（广义的）线性回归图等多种复杂的图像。

由于seaborn是基于matplotlib的上层函数，因此设置matplotlib的一些绘图格式也一样会影响到seaborn的绘图效果。

seaborn的官网：http://seaborn.pydata.org/

官方教程：http://seaborn.pydata.org/tutorial.html

下面的代码示例是使用seaborn绘制线性回归图（摘自官网的示例）：

```python
import seaborn as sns
sns.set_theme(color_codes=True)
tips = sns.load_dataset("tips")
sns.regplot(x="total_bill", y="tip", data=tips)
```

![img](https://raw.githubusercontent.com/lyf35/blog_figures/main/img/20210109170725.png)

从图中可以看到，`regplot`函数自动将数据绘制为散点图，并同时在图中绘制出了这些数据点的回归直线以及95%的置信区间。这一操作要比matplotlib简便地多。

此外，官网给出了一些不同种类图像的绘图示例，可以参考：http://seaborn.pydata.org/examples/index.html

## 地理绘图

如果要绘制与地理相关的一些图像，可以使用一些基于matplotlib的上层工具，例如：

geoplot：https://residentmario.github.io/geoplot/quickstart/quickstart.html

Basemap：https://matplotlib.org/basemap/



# 参考

1. Python for Data Analysis, 2nd Edition.
2. https://www.kesci.com/home/column/5b87a78131902f000f668549
3. https://zhuanlan.zhihu.com/p/113842270
4. https://blog.csdn.net/qq_27825451/article/details/81481534
5. https://www.jianshu.com/p/3937798d645b
6. https://zhuanlan.zhihu.com/p/83588651