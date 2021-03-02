---
title: 数据分析-Pandas
tags: 
- 数据分析
categories: [数据分析]
sharing: True
---

# 简介

Pandas是一个用来处理表格类型数据的函数库。Pandas的官方文档：http://pandas.pydata.org/pandas-docs/stable/


```python
import numpy as np
import pandas as pd
```

# 数据结构

## 主要数据结构

### Series

一个Series由一个index和与之对应的一系列value组成，相当于是一个单列的表格。它的构造方式有：


```python
series1=pd.Series(np.array((4,7,-5,3))) #传入一个list、tuple、或是numpy的一维数组，此时会默认为这些元素添加一个从0到n-1的RangeIndex
```


```python
series1
```


    0    4
    1    7
    2   -5
    3    3
    dtype: int32

<!--more-->




```python
pd.Series([2,3,-3,1],index=['a','c','b','d']) #Series的构造函数支持传入一个自定义的index
```


    a    2
    c    3
    b   -3
    d    1
    dtype: int64




```python
dict1={'Ohio': 35000, 'Texas': 71000, 'Oregon': 16000, 'Utah': 5000}
states = ['California', 'Ohio', 'Oregon', 'Texas']
```


```python
pd.Series(dict1) #可以传入一个dict来构造Series，map中每个元素的key作为index，并与每个元素的value相对应。dict的key部分可以为tuple，这样会生成一个多重index的Series
```


    Ohio      35000
    Texas     71000
    Oregon    16000
    Utah       5000
    dtype: int64




```python
pd.Series(dict1,index=states) #使用dict构造Series时，同样可以传入index参数，Series中的index便使用传入的index参数。此时，每个index对应的value会从dict中获取，如果某个index在dict没有对应的key-value，那么Series中这个index对应的值为NaN
```


    California        NaN
    Ohio          35000.0
    Oregon        16000.0
    Texas         71000.0
    dtype: float64



一个Series具有以下的属性：


```python
series1.values #Series的values属性保存了Series中的数据，value属性为一个numpy的一维数组
```


    array([ 4,  7, -5,  3])




```python
series1.index #Series的index属性保存了这些value对应的index
```


    RangeIndex(start=0, stop=4, step=1)




```python
series1.index.name='test_series' #一个Series的index可以有自己的名字，默认为空。如果加上名字之后，在print这个Series时便可以看到为这个index加上的名字
```


```python
series1
```


    test_series
    0    4
    1    7
    2   -5
    3    3
    dtype: int32



### DataFrame

DataFrame数据结构相当于一个表格。也可以理解为一个DataFrame由多个Series拼在一起，其中这些Series具有共同的index。

许多种数据类型都可以被用来构造DataFrame，包括但不限于：

- 二维的array
- 一个value为array、list或者tuple的dict
- 一个value为Series的dict
- 一个value为dict的dict
- 一个元素为dict或者Series的list
- 一个元素为list或者tuple的list
- 另一个DataFrame

下面为创建DataFrame的示例：


```python
data = {'state': ['Ohio', 'Ohio', 'Ohio', 'Nevada', 'Nevada'],
'year': [2000, 2001, 2002, 2001, 2002],
'pop': [1.5, 1.7, 3.6, 2.4, 2.9]}
dataframe1=pd.DataFrame(data) #可以传入一个dict，其中每个dict的value为等长的list或tuple，或是numpy的一维数组。在DataFrame中，dict中的key会被作为是column的名称，value则为每列的值，同时自动为这些数据添加一个index。
```


```python
dataframe1
```


<div>
<style scoped>
    .dataframe tbody tr th:only-of-type {
        vertical-align: middle;
    }
    .dataframe tbody tr th {
        vertical-align: top;
    }
    .dataframe thead th {
        text-align: right;
    }
</style>
<table border="1" class="dataframe">
  <thead>
    <tr style="text-align: right;">
      <th></th>
      <th>state</th>
      <th>year</th>
      <th>pop</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <th>0</th>
      <td>Ohio</td>
      <td>2000</td>
      <td>1.5</td>
    </tr>
    <tr>
      <th>1</th>
      <td>Ohio</td>
      <td>2001</td>
      <td>1.7</td>
    </tr>
    <tr>
      <th>2</th>
      <td>Ohio</td>
      <td>2002</td>
      <td>3.6</td>
    </tr>
    <tr>
      <th>3</th>
      <td>Nevada</td>
      <td>2001</td>
      <td>2.4</td>
    </tr>
    <tr>
      <th>4</th>
      <td>Nevada</td>
      <td>2002</td>
      <td>2.9</td>
    </tr>
  </tbody>
</table>
</div>


```python
pd.DataFrame(data, columns=['year', 'state', 'debt'],index=['one', 'two', 'three', 'four', 'five']) #在使用dict构造DataFrame时，支持传入columns和index两个可选属性。此时，DataFrame的columns则会设置为传入的columns参数，每个column对应的值会从dict中获得，如果dict中不存在对应的key，则该列的所有值都为空
```


<div>
<style scoped>
    .dataframe tbody tr th:only-of-type {
        vertical-align: middle;
    }
    .dataframe tbody tr th {
        vertical-align: top;
    }
    .dataframe thead th {
        text-align: right;
    }
</style>
<table border="1" class="dataframe">
  <thead>
    <tr style="text-align: right;">
      <th></th>
      <th>year</th>
      <th>state</th>
      <th>debt</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <th>one</th>
      <td>2000</td>
      <td>Ohio</td>
      <td>NaN</td>
    </tr>
    <tr>
      <th>two</th>
      <td>2001</td>
      <td>Ohio</td>
      <td>NaN</td>
    </tr>
    <tr>
      <th>three</th>
      <td>2002</td>
      <td>Ohio</td>
      <td>NaN</td>
    </tr>
    <tr>
      <th>four</th>
      <td>2001</td>
      <td>Nevada</td>
      <td>NaN</td>
    </tr>
    <tr>
      <th>five</th>
      <td>2002</td>
      <td>Nevada</td>
      <td>NaN</td>
    </tr>
  </tbody>
</table>
</div>


```python
data2={'Nevada': {2001: 2.4, 2002: 2.9}, 'Ohio': {2000: 1.5, 2001: 1.7, 2003: 3.6}}
pd.DataFrame(data2) #另一种构造DataFrame的方法，同样是传入一个dict，但是这个dict中的value也是一个dict，相当于一个两层嵌套的dict。外层dict中的key会被作为columns的名称，而内层dict的key则被作为index的名称。在最终得到的DataFrame中，index为所有内层dict中的key取并集。如果某个内层dict中，某些index对应的key-value不存在，那么在构造DataFrame时，对应的位置会被设为NaN。在构造dict的时候，key部分可以为一个tuple，这样会自动构造为多重index的DataFrame
```


<div>
<style scoped>
    .dataframe tbody tr th:only-of-type {
        vertical-align: middle;
    }
    .dataframe tbody tr th {
        vertical-align: top;
    }
    .dataframe thead th {
        text-align: right;
    }
</style>
<table border="1" class="dataframe">
  <thead>
    <tr style="text-align: right;">
      <th></th>
      <th>Nevada</th>
      <th>Ohio</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <th>2001</th>
      <td>2.4</td>
      <td>1.7</td>
    </tr>
    <tr>
      <th>2002</th>
      <td>2.9</td>
      <td>NaN</td>
    </tr>
    <tr>
      <th>2000</th>
      <td>NaN</td>
      <td>1.5</td>
    </tr>
    <tr>
      <th>2003</th>
      <td>NaN</td>
      <td>3.6</td>
    </tr>
  </tbody>
</table>
</div>


```python
pd.DataFrame(data2,index=[2000,2001,2002,2004]) #使用两层嵌套dict构造DataFrame时，支持传入一个index属性，此时DataFrame中使用传入的index。每个index对应的值会在相应的内层dict中索引，如果不存在则为NaN。
```


<div>
<style scoped>
    .dataframe tbody tr th:only-of-type {
        vertical-align: middle;
    }
    .dataframe tbody tr th {
        vertical-align: top;
    }
    .dataframe thead th {
        text-align: right;
    }
</style>
<table border="1" class="dataframe">
  <thead>
    <tr style="text-align: right;">
      <th></th>
      <th>Nevada</th>
      <th>Ohio</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <th>2000</th>
      <td>NaN</td>
      <td>1.5</td>
    </tr>
    <tr>
      <th>2001</th>
      <td>2.4</td>
      <td>1.7</td>
    </tr>
    <tr>
      <th>2002</th>
      <td>2.9</td>
      <td>NaN</td>
    </tr>
    <tr>
      <th>2004</th>
      <td>NaN</td>
      <td>NaN</td>
    </tr>
  </tbody>
</table>
</div>


```python
pd.DataFrame(np.random.rand(5,3)) #可以传入一个numpy的二维array进行构造
```


<div>
<style scoped>
    .dataframe tbody tr th:only-of-type {
        vertical-align: middle;
    }
    .dataframe tbody tr th {
        vertical-align: top;
    }
    .dataframe thead th {
        text-align: right;
    }
</style>
<table border="1" class="dataframe">
  <thead>
    <tr style="text-align: right;">
      <th></th>
      <th>0</th>
      <th>1</th>
      <th>2</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <th>0</th>
      <td>0.860528</td>
      <td>0.190016</td>
      <td>0.355405</td>
    </tr>
    <tr>
      <th>1</th>
      <td>0.707203</td>
      <td>0.021066</td>
      <td>0.883826</td>
    </tr>
    <tr>
      <th>2</th>
      <td>0.672148</td>
      <td>0.091125</td>
      <td>0.984936</td>
    </tr>
    <tr>
      <th>3</th>
      <td>0.816604</td>
      <td>0.672571</td>
      <td>0.421406</td>
    </tr>
    <tr>
      <th>4</th>
      <td>0.327092</td>
      <td>0.616550</td>
      <td>0.054370</td>
    </tr>
  </tbody>
</table>
</div>



一个DataFrame具有下面的属性：


```python
dataframe1.values #DataFrame也有values属性，它是一个二维的array
```


    array([['Ohio', 2000, 1.5],
           ['Ohio', 2001, 1.7],
           ['Ohio', 2002, 3.6],
           ['Nevada', 2001, 2.4],
           ['Nevada', 2002, 2.9]], dtype=object)




```python
dataframe1.index #与Series一样，具有index属性
```


    RangeIndex(start=0, stop=5, step=1)




```python
dataframe1.columns #相比于Series，多了一个columns属性
```


    Index(['state', 'year', 'pop'], dtype='object')




```python
dataframe1.index.name='test_index'
dataframe1.columns.name='test_columns' #与Series类似，DataFrame的index和columns也可以有自己的名字
```


```python
dataframe1
```


<div>
<style scoped>
    .dataframe tbody tr th:only-of-type {
        vertical-align: middle;
    }
    .dataframe tbody tr th {
        vertical-align: top;
    }
    .dataframe thead th {
        text-align: right;
    }
</style>
<table border="1" class="dataframe">
  <thead>
    <tr style="text-align: right;">
      <th>test_columns</th>
      <th>state</th>
      <th>year</th>
      <th>pop</th>
    </tr>
    <tr>
      <th>test_index</th>
      <th></th>
      <th></th>
      <th></th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <th>0</th>
      <td>Ohio</td>
      <td>2000</td>
      <td>1.5</td>
    </tr>
    <tr>
      <th>1</th>
      <td>Ohio</td>
      <td>2001</td>
      <td>1.7</td>
    </tr>
    <tr>
      <th>2</th>
      <td>Ohio</td>
      <td>2002</td>
      <td>3.6</td>
    </tr>
    <tr>
      <th>3</th>
      <td>Nevada</td>
      <td>2001</td>
      <td>2.4</td>
    </tr>
    <tr>
      <th>4</th>
      <td>Nevada</td>
      <td>2002</td>
      <td>2.9</td>
    </tr>
  </tbody>
</table>
</div>



### Panel

Panel是多个DataFrame的组合，相当于Excel中的一个.xlsx文件（其中包括多张表）。但是在目前的Pandas中已经被废弃，可能在一些旧的Pandas版本中才会有，此处不再赘述。

## index属性

在上面的代码中，我们使用了index属性提取出一个series或dataframe的index，或使用columns属性提取出一个dataframe的column。这些操作得到的都是一个index类型的**常量**，也就是说index的数据无法修改！

index常量可以直接用于修改其它series或dataframe的index。同时，这个index可以像python自带的set一样使用。

Pandas中，index的类型包括但不限于：

- Index：最基本的也是最通用的index类型
- Int64Index：整数类型的index
- MultiIndex：多重index
- DatetimeIndex：使用NumPy的`datetime64`类型，保存的是毫秒形式的时间戳
- PeriodIndex：周期性数据构成的index
- RangeIndex：index为一组等差数列组成的range，具有开始值、结束值和步长。

同时index支持下面这些方法，来构造新的index。其中，一些方法对于某些特定的index类型不适用：

- append：将一个新的index附加到已有的index后面
- diff：取两个index中不重复的元素
- intersection：取两个index的交集
- union：取两个index的并集
- delete：传入一个整数，删除对应位置的元素
- drop：传入一个元素的值，删除掉这个元素
- insert：传入一个整数与一个值，将这个值插入到整数对应的位置
- unique：输出index中不重复的元素

index也支持下面的这些运算操作：

- isin：传入一个list，输出一个布尔类型的数组，表示list中的元素是否在index中
- is_monotonic：如果每个元素都大于等于它的前一个，则返回True
- is_unique：如果index中没有重复元素则返回True

下面为一些用法示例：


```python
dataframe1.index.union(dataframe1.columns)
```


    Index([0, 1, 2, 3, 4, 'pop', 'state', 'year'], dtype='object')




```python
dataframe1.columns.is_unique
```


    True




```python
dataframe1.columns.drop('pop')
```


    Index(['state', 'year'], dtype='object', name='test_columns')



## 具有重复数据的index

在Series和DataFrame的index中，可以出现重复的元素，例如：


```python
dataframe2=pd.DataFrame(np.random.randn(5,3),index=['a','a','b','b','c'])
```


```python
dataframe2.index.is_unique #此时调用index的is_unique属性，返回值为False
```


    False




```python
dataframe2.loc['b'] #此时对重复的index做索引时得到的是一个DataFrame而不是一个Series
```


<div>
<style scoped>
    .dataframe tbody tr th:only-of-type {
        vertical-align: middle;
    }
    .dataframe tbody tr th {
        vertical-align: top;
    }
    .dataframe thead th {
        text-align: right;
    }
</style>
<table border="1" class="dataframe">
  <thead>
    <tr style="text-align: right;">
      <th></th>
      <th>0</th>
      <th>1</th>
      <th>2</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <th>b</th>
      <td>1.589025</td>
      <td>-0.504598</td>
      <td>0.021645</td>
    </tr>
    <tr>
      <th>b</th>
      <td>0.629079</td>
      <td>-0.703262</td>
      <td>-0.759489</td>
    </tr>
  </tbody>
</table>
</div>


# IO操作

## 读取文件

Pandas常用的从文件中读取表格数据的函数有：

```python
pd.read_csv('filename') #读取.csv格式的文件，可以是本地文件路径，也可以是一个URL。默认的分隔符为逗号
pd.read_table('filename') #从文件中读取表格数据，可以是本地文件路径，也可以是URL。默认分割符为'\t'
```

在这两个函数中，常用的参数包括：

- path：文件路径，可以为本地文件路径或者URL
- sep/delimiter：表格数据的分割符，可以使用正则表达式
- header：文件中哪一行作为结果中的columns（列名称），默认为0。如果不使用任意一行作为columns，则这一参数应该设置为None
- index_col：在最终结果中，哪些列被作为index，可以传入数字序号或者列名称
- names：传入一个list，作为columns（列名称），此时header参数应该为None
- skiprows：传入一个数字，代表跳过前若干行不读取，然后从接下来的行作为文件的开头进行读取；或是传入一个列表，代表跳过列表中的行
- na_values：传入一个list，其中包含的元素作为NaN处理；或是传入一个dict，其中每个元素的key对应于列名，value为一个list，代表作为NaN处理的元素。默认情况下，文件中的空格或者一些特殊值都会被看作是NaN
- comment：传入一个或一些字符，用于在一行的结尾将注释部分与表格内容分隔开
- converters：传入一个dict，其中的元素为列名称和函数的映射，代表对某一列使用某个函数进行数据转换
- nrows：代表从文件开始位置读取多少行
- chunksize：传入一个整数，表示一个DataFrame中的行上限，如果原始数据中的行数大于这一上限，则最终得到的是多个DataFrame
- skip_footer：代表忽略文件最后的若干行
- parse_dates：是否试图将原始数据中的时间数据转换为datetime类型，默认为False。如果为True，则读取数据时会对所有的列尝试进行类型转换操作
- encoding：原始数据的编码方式

此外，还有一些从其他数据来源中读取数据的函数，包括（此处不再详细赘述每个函数的具体用法）：

```python
pd.read_excel('filename','sheetname') #从.xls或.xlsx文件中读取表格数据
pd.read_html('filename') #从html文件中读取其中所有的表格
pd.read_json('filename') #从JSON格式文件中读取表格
pd.read_pickle('filename') #从Python pickle格式的二进制文件中读取表格数据。相应地，保存数据的时候用to_pickle()函数。
pd.read_sql('SQL command', database) #从SQL数据库中读取数据
```



## 写入文件

Pandas提供了将Series或者DataFrame转换为csv文件的函数：

```python
pd.to_csv('filename')
```

这一函数可以传入的参数包括：

- sep：指定分隔符，默认为逗号
- na_rep：指定缺失数据的输出值，默认输出为空字符串
- index：代表是否将index写入到文件中，默认为True
- header：代表是否将columns（列名称）写入到文件中，默认为True
- cols：指定写入到文件中去的列名称

如果要将文件写入Excel格式，可以使用函数：

```python
frame.to_excel('filename')
```




# 基本操作与运算

## reindex

reindex函数传入一个list、tuple或者index，将其作为新的index，并将原来的数据按照新index对应起来。如果新的index中有新的元素，则它对应的值默认为NaN，也可以传入参数对其进行赋值。如果旧的index中某些元素不包含在新的index中，则新数据不包含相应的元素。


```python
obj = pd.Series([4.5, 7.2, -5.3, 3.6], index=['d', 'b', 'a', 'c'])
```


```python
obj
```


    d    4.5
    b    7.2
    a   -5.3
    c    3.6
    dtype: float64




```python
obj.reindex(['a','b','c','d','e']) #原来的Series中没有'e'这个index，因此相应位置默认为NaN
```


    a   -5.3
    b    7.2
    c    3.6
    d    4.5
    e    NaN
    dtype: float64




```python
obj.reindex(['a','b','e'],fill_value=0) #原Series中有的index，如果传入的index中没有则不包含相应的值，传入fill_value可以对新index对应的值进行填充
```


    a   -5.3
    b    7.2
    e    0.0
    dtype: float64




```python
obj2 = pd.Series([4.5, 7.2, -5.3, 3.6], index=[1,2,3,4])
obj2.reindex([2,3,4,5],method='ffill') #如果原始的index为单调递增或者单调递减的情况下，可以传入method参数设置填充方法。其中ffill或者pad代表空位置使用从它向上追溯与它最近的非空元素。
```


    2    7.2
    3   -5.3
    4    3.6
    5    3.6
    dtype: float64




```python
obj2.reindex([0,2,3,4,5],method='bfill') #bfill或者backfill代表空位置使用从它向下追溯与它最近的非空元素。注意，ffill和bfill在填充的时候，是在原Series的index中进行查找（如本例的0）。如果要做ffill操作的index最小，或者做bfill操作的index最大，则仍然会被填充为NaN。
```


    0    4.5
    2    7.2
    3   -5.3
    4    3.6
    5    NaN
    dtype: float64



对于DataFrame来说，则可以同时改变它的index或者columns：


```python
obj3=pd.DataFrame(np.arange(9).reshape((3, 3)), index=['a', 'c', 'd'],columns=['Ohio', 'Texas', 'California'])
obj3.reindex(['a','b','c','d']) #默认情况下，使用传入的list、tuple或者index去修改DataFrame的index。如果只修改columns则需要将传入参数设置为columns=……。
```


<div>
<style scoped>
    .dataframe tbody tr th:only-of-type {
        vertical-align: middle;
    }
    .dataframe tbody tr th {
        vertical-align: top;
    } 
    .dataframe thead th {
        text-align: right;
    }
</style>
<table border="1" class="dataframe">
  <thead>
    <tr style="text-align: right;">
      <th></th>
      <th>Ohio</th>
      <th>Texas</th>
      <th>California</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <th>a</th>
      <td>0.0</td>
      <td>1.0</td>
      <td>2.0</td>
    </tr>
    <tr>
      <th>b</th>
      <td>NaN</td>
      <td>NaN</td>
      <td>NaN</td>
    </tr>
    <tr>
      <th>c</th>
      <td>3.0</td>
      <td>4.0</td>
      <td>5.0</td>
    </tr>
    <tr>
      <th>d</th>
      <td>6.0</td>
      <td>7.0</td>
      <td>8.0</td>
    </tr>
  </tbody>
</table>
</div>


```python
obj3.reindex(index=['a','b','c','d'],columns=['Texas','Utah','California'])
```


<div>
<style scoped>
    .dataframe tbody tr th:only-of-type {
        vertical-align: middle;
    }
    .dataframe tbody tr th {
        vertical-align: top;
    }
    .dataframe thead th {
        text-align: right;
    }
</style>
<table border="1" class="dataframe">
  <thead>
    <tr style="text-align: right;">
      <th></th>
      <th>Texas</th>
      <th>Utah</th>
      <th>California</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <th>a</th>
      <td>1.0</td>
      <td>NaN</td>
      <td>2.0</td>
    </tr>
    <tr>
      <th>b</th>
      <td>NaN</td>
      <td>NaN</td>
      <td>NaN</td>
    </tr>
    <tr>
      <th>c</th>
      <td>4.0</td>
      <td>NaN</td>
      <td>5.0</td>
    </tr>
    <tr>
      <th>d</th>
      <td>7.0</td>
      <td>NaN</td>
      <td>8.0</td>
    </tr>
  </tbody>
</table>
</div>


```python
obj3.index=obj3.index.map(str.upper) #可以传入一个dict或者一个函数对index进行修改
```


```python
obj3
```


<div>
<style scoped>
    .dataframe tbody tr th:only-of-type {
        vertical-align: middle;
    }
    .dataframe tbody tr th {
        vertical-align: top;
    }
    .dataframe thead th {
        text-align: right;
    }
</style>
<table border="1" class="dataframe">
  <thead>
    <tr style="text-align: right;">
      <th></th>
      <th>Ohio</th>
      <th>Texas</th>
      <th>California</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <th>A</th>
      <td>0</td>
      <td>1</td>
      <td>2</td>
    </tr>
    <tr>
      <th>C</th>
      <td>3</td>
      <td>4</td>
      <td>5</td>
    </tr>
    <tr>
      <th>D</th>
      <td>6</td>
      <td>7</td>
      <td>8</td>
    </tr>
  </tbody>
</table>
</div>


```python
obj3.rename(index=str.title,columns=str.upper) #使用rename函数也可以实现同样的功能。默认情况下这一函数会生成一个副本，如果要直接修改原变量则需要加上inplace=True参数。
```


<div>
<style scoped>
    .dataframe tbody tr th:only-of-type {
        vertical-align: middle;
    }
    .dataframe tbody tr th {
        vertical-align: top;
    }
    .dataframe thead th {
        text-align: right;
    }
</style>
<table border="1" class="dataframe">
  <thead>
    <tr style="text-align: right;">
      <th></th>
      <th>OHIO</th>
      <th>TEXAS</th>
      <th>CALIFORNIA</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <th>A</th>
      <td>0</td>
      <td>1</td>
      <td>2</td>
    </tr>
    <tr>
      <th>C</th>
      <td>3</td>
      <td>4</td>
      <td>5</td>
    </tr>
    <tr>
      <th>D</th>
      <td>6</td>
      <td>7</td>
      <td>8</td>
    </tr>
  </tbody>
</table>
</div>


```python
obj3.rename(columns={'Ohio': 'INDIANA'},index={'C': 'peekaboo'})
```


<div>
<style scoped>
    .dataframe tbody tr th:only-of-type {
        vertical-align: middle;
    }
    .dataframe tbody tr th {
        vertical-align: top;
    }
    .dataframe thead th {
        text-align: right;
    }
</style>
<table border="1" class="dataframe">
  <thead>
    <tr style="text-align: right;">
      <th></th>
      <th>INDIANA</th>
      <th>Texas</th>
      <th>California</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <th>A</th>
      <td>0</td>
      <td>1</td>
      <td>2</td>
    </tr>
    <tr>
      <th>peekaboo</th>
      <td>3</td>
      <td>4</td>
      <td>5</td>
    </tr>
    <tr>
      <th>D</th>
      <td>6</td>
      <td>7</td>
      <td>8</td>
    </tr>
  </tbody>
</table>
</div>



## 删除操作

drop函数可以从Series或者DataFrame中删除某个index对应的行，或者DataFrame中某个column对应的列：


```python
obj3.drop(['A']) #默认查找index并删除行
```


<div>
<style scoped>
    .dataframe tbody tr th:only-of-type {
        vertical-align: middle;
    }
    .dataframe tbody tr th {
        vertical-align: top;
    }
    .dataframe thead th {
        text-align: right;
    }
</style>
<table border="1" class="dataframe">
  <thead>
    <tr style="text-align: right;">
      <th></th>
      <th>Ohio</th>
      <th>Texas</th>
      <th>California</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <th>C</th>
      <td>3</td>
      <td>4</td>
      <td>5</td>
    </tr>
    <tr>
      <th>D</th>
      <td>6</td>
      <td>7</td>
      <td>8</td>
    </tr>
  </tbody>
</table>
</div>


```python
obj3.drop('Texas',axis=1) #要删除column则需要指定axis参数等于1
```


<div>
<style scoped>
    .dataframe tbody tr th:only-of-type {
        vertical-align: middle;
    }
    .dataframe tbody tr th {
        vertical-align: top;
    }
    .dataframe thead th {
        text-align: right;
    }
</style>
<table border="1" class="dataframe">
  <thead>
    <tr style="text-align: right;">
      <th></th>
      <th>Ohio</th>
      <th>California</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <th>A</th>
      <td>0</td>
      <td>2</td>
    </tr>
    <tr>
      <th>C</th>
      <td>3</td>
      <td>5</td>
    </tr>
    <tr>
      <th>D</th>
      <td>6</td>
      <td>8</td>
    </tr>
  </tbody>
</table>
</div>



## 切片与索引操作

Series和DataFrame都支持索引和切片操作，同时支持使用布尔运算进行索引。

下面为Series的一些使用示例：


```python
series_index=pd.Series(np.arange(4.),index=['a','b','c','d'])
```


```python
series_index[1] #可以像索引list或者ndarray一样使用序号进行索引，但是这种索引方式要求index中的元素不是int类型，否则会使用int索引index中的元素
```


    1.0




```python
series_index['b'] #可以按照index的值来索引
```


    1.0




```python
series_index[2:4] #使用序号进行切片操作，按照左闭右开区间取数据
```


    c    2.0
    d    3.0
    dtype: float64




```python
series_index['a':'c'] #使用index的值进行切片操作，此时取数据按照闭区间来操作
```


    a    0.0
    b    1.0
    c    2.0
    dtype: float64




```python
series_index[['b','a']] #按照传入的list进行索引
```


    b    1.0
    a    0.0
    dtype: float64




```python
series_index[[2,1]]
```


    c    2.0
    b    1.0
    dtype: float64




```python
series_index[series_index<2] #使用布尔表达式进行索引，相当于传入布尔数组来索引
```


    a    0.0
    b    1.0
    dtype: float64




```python
series_index['b':'c']=4
```


```python
series_index #索引操作获得的是原始数据的副本，如果对索引得到的数据进行修改操作，相当于修改原始数据
```


    a    0.0
    b    4.0
    c    4.0
    d    3.0
    dtype: float64



对于DataFrame来说，它的索引与切片操作比Series要复杂一些。一些使用示例如下：


```python
dataframe_index=pd.DataFrame(np.arange(16).reshape((4, 4)),index=['Ohio', 'Colorado', 'Utah', 'New York'], columns=['one', 'two', 'three', 'four'])
```


```python
dataframe_index['two'] #传入columns中的元素，按列做索引，得到一个Series。
#注意DataFrame不支持使用index的元素进行索引！例如dataframe_index['Ohio']操作是错误的。
```


    Ohio         1
    Colorado     5
    Utah         9
    New York    13
    Name: two, dtype: int32




```python
dataframe_index[['three','one']] #传入一个list进行索引，索引结果按照list中的元素顺序排列。
#需要注意的是，不能使用columns中的元素来做切片操作！例如dataframe_index['one':'three']为非法操作。
```


<div>
<style scoped>
    .dataframe tbody tr th:only-of-type {
        vertical-align: middle;
    }
    .dataframe tbody tr th {
        vertical-align: top;
    }
    .dataframe thead th {
        text-align: right;
    }
</style>
<table border="1" class="dataframe">
  <thead>
    <tr style="text-align: right;">
      <th></th>
      <th>three</th>
      <th>one</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <th>Ohio</th>
      <td>2</td>
      <td>0</td>
    </tr>
    <tr>
      <th>Colorado</th>
      <td>6</td>
      <td>4</td>
    </tr>
    <tr>
      <th>Utah</th>
      <td>10</td>
      <td>8</td>
    </tr>
    <tr>
      <th>New York</th>
      <td>14</td>
      <td>12</td>
    </tr>
  </tbody>
</table>
</div>


```python
dataframe_index[:2] #DataFrame也可以使用数字来做切片操作，这一方法是对行进行切片操作
#注意：DataFrame不支持像Series一样直接传入数字进行索引！例如dataframe_index[2]，dataframe_index[[0,1]],dataframe_index[[0,1],[1,2]]都为错误操作。要使用数字索引，需要使用iloc运算。
```


<div>
<style scoped>
    .dataframe tbody tr th:only-of-type {
        vertical-align: middle;
    }
    .dataframe tbody tr th {
        vertical-align: top;
    }
    .dataframe thead th {
        text-align: right;
    }
</style>
<table border="1" class="dataframe">
  <thead>
    <tr style="text-align: right;">
      <th></th>
      <th>one</th>
      <th>two</th>
      <th>three</th>
      <th>four</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <th>Ohio</th>
      <td>0</td>
      <td>1</td>
      <td>2</td>
      <td>3</td>
    </tr>
    <tr>
      <th>Colorado</th>
      <td>4</td>
      <td>5</td>
      <td>6</td>
      <td>7</td>
    </tr>
  </tbody>
</table>
</div>


```python
dataframe_index.take([2,3,1,1]) #如果要按行取数据，可以使用take函数。函数接收一个元素为整数的list，然后按照这些整数选出对应的行。list中的元素可以有重复。如果要对数据进行随机打乱，则可以借助NumPy的random.permutation函数
```


<div>
<style scoped>
    .dataframe tbody tr th:only-of-type {
        vertical-align: middle;
    }
    .dataframe tbody tr th {
        vertical-align: top;
    }
    .dataframe thead th {
        text-align: right;
    }
</style>
<table border="1" class="dataframe">
  <thead>
    <tr style="text-align: right;">
      <th></th>
      <th>one</th>
      <th>two</th>
      <th>three</th>
      <th>four</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <th>Utah</th>
      <td>8</td>
      <td>9</td>
      <td>10</td>
      <td>11</td>
    </tr>
    <tr>
      <th>New York</th>
      <td>12</td>
      <td>13</td>
      <td>14</td>
      <td>15</td>
    </tr>
    <tr>
      <th>Colorado</th>
      <td>4</td>
      <td>5</td>
      <td>6</td>
      <td>7</td>
    </tr>
    <tr>
      <th>Colorado</th>
      <td>4</td>
      <td>5</td>
      <td>6</td>
      <td>7</td>
    </tr>
  </tbody>
</table>
</div>


```python
dataframe_index[dataframe_index<5] #可以在索引时使用布尔表达式，相当于传入布尔数组
```


<div>
<style scoped>
    .dataframe tbody tr th:only-of-type {
        vertical-align: middle;
    }
    .dataframe tbody tr th {
        vertical-align: top;
    }
    .dataframe thead th {
        text-align: right;
    }
</style>
<table border="1" class="dataframe">
  <thead>
    <tr style="text-align: right;">
      <th></th>
      <th>one</th>
      <th>two</th>
      <th>three</th>
      <th>four</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <th>Ohio</th>
      <td>0.0</td>
      <td>1.0</td>
      <td>2.0</td>
      <td>3.0</td>
    </tr>
    <tr>
      <th>Colorado</th>
      <td>4.0</td>
      <td>NaN</td>
      <td>NaN</td>
      <td>NaN</td>
    </tr>
    <tr>
      <th>Utah</th>
      <td>NaN</td>
      <td>NaN</td>
      <td>NaN</td>
      <td>NaN</td>
    </tr>
    <tr>
      <th>New York</th>
      <td>NaN</td>
      <td>NaN</td>
      <td>NaN</td>
      <td>NaN</td>
    </tr>
  </tbody>
</table>
</div>


```python
dataframe_index[dataframe_index['three']>5]
```


<div>
<style scoped>
    .dataframe tbody tr th:only-of-type {
        vertical-align: middle;
    }
    .dataframe tbody tr th {
        vertical-align: top;
    }
    .dataframe thead th {
        text-align: right;
    }
</style>
<table border="1" class="dataframe">
  <thead>
    <tr style="text-align: right;">
      <th></th>
      <th>one</th>
      <th>two</th>
      <th>three</th>
      <th>four</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <th>Colorado</th>
      <td>4</td>
      <td>5</td>
      <td>6</td>
      <td>7</td>
    </tr>
    <tr>
      <th>Utah</th>
      <td>8</td>
      <td>9</td>
      <td>10</td>
      <td>11</td>
    </tr>
    <tr>
      <th>New York</th>
      <td>12</td>
      <td>13</td>
      <td>14</td>
      <td>15</td>
    </tr>
  </tbody>
</table>
</div>


```python
dataframe_index['five']=[3,4,5,6] #可以使用这种方式向DataFrame中插入一个新列（当然Series也支持这种操作），要求索引使用的关键词在columns中不存在，并且赋值使用的list的大小要求与DataFrame的行数一致。注意这样的操作会直接在原始数据上做就地的修改
```


```python
dataframe_index 
```


<div>
<style scoped>
    .dataframe tbody tr th:only-of-type {
        vertical-align: middle;
    }
    .dataframe tbody tr th {
        vertical-align: top;
    }
    .dataframe thead th {
        text-align: right;
    }
</style>
<table border="1" class="dataframe">
  <thead>
    <tr style="text-align: right;">
      <th></th>
      <th>one</th>
      <th>two</th>
      <th>three</th>
      <th>four</th>
      <th>five</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <th>Ohio</th>
      <td>0</td>
      <td>1</td>
      <td>2</td>
      <td>3</td>
      <td>3</td>
    </tr>
    <tr>
      <th>Colorado</th>
      <td>4</td>
      <td>5</td>
      <td>6</td>
      <td>7</td>
      <td>4</td>
    </tr>
    <tr>
      <th>Utah</th>
      <td>8</td>
      <td>9</td>
      <td>10</td>
      <td>11</td>
      <td>5</td>
    </tr>
    <tr>
      <th>New York</th>
      <td>12</td>
      <td>13</td>
      <td>14</td>
      <td>15</td>
      <td>6</td>
    </tr>
  </tbody>
</table>
</div>



除了上述这些操作，pandas还提供了`loc`和`iloc`两种运算符进行索引操作，`loc`是使用index和columns中的内容进行索引，而`iloc`则是使用整数。同样，也可以使用`at`和`iat`两个运算符，索引某行和某列对应的那个元素。这种索引方式的含义清晰，不会让人混淆，因此在索引操作时最好使用这种办法。

下面是一些使用示例：


```python
dataframe_index.loc['Ohio':'Utah',['one','two']] #先索引index，再索引columns，支持切片操作以及传入list的索引方式
```


<div>
<style scoped>
    .dataframe tbody tr th:only-of-type {
        vertical-align: middle;
    }
    .dataframe tbody tr th {
        vertical-align: top;
    }
    .dataframe thead th {
        text-align: right;
    }
</style>
<table border="1" class="dataframe">
  <thead>
    <tr style="text-align: right;">
      <th></th>
      <th>one</th>
      <th>two</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <th>Ohio</th>
      <td>0</td>
      <td>1</td>
    </tr>
    <tr>
      <th>Colorado</th>
      <td>4</td>
      <td>5</td>
    </tr>
    <tr>
      <th>Utah</th>
      <td>8</td>
      <td>9</td>
    </tr>
  </tbody>
</table>
</div>


```python
dataframe_index.loc[:,['one','two']] #如果只索引columns，则index部分必须加上:代表索引所有的index。
```


<div>
<style scoped>
    .dataframe tbody tr th:only-of-type {
        vertical-align: middle;
    }
    .dataframe tbody tr th {
        vertical-align: top;
    }
    .dataframe thead th {
        text-align: right;
    }
</style>
<table border="1" class="dataframe">
  <thead>
    <tr style="text-align: right;">
      <th></th>
      <th>one</th>
      <th>two</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <th>Ohio</th>
      <td>0</td>
      <td>1</td>
    </tr>
    <tr>
      <th>Colorado</th>
      <td>4</td>
      <td>5</td>
    </tr>
    <tr>
      <th>Utah</th>
      <td>8</td>
      <td>9</td>
    </tr>
    <tr>
      <th>New York</th>
      <td>12</td>
      <td>13</td>
    </tr>
  </tbody>
</table>
</div>


```python
dataframe_index.loc[['New York','Utah']] #如果只索引index，则columns部分的索引可以省略。
```


<div>
<style scoped>
    .dataframe tbody tr th:only-of-type {
        vertical-align: middle;
    }
    .dataframe tbody tr th {
        vertical-align: top;
    }
    .dataframe thead th {
        text-align: right;
    }
</style>
<table border="1" class="dataframe">
  <thead>
    <tr style="text-align: right;">
      <th></th>
      <th>one</th>
      <th>two</th>
      <th>three</th>
      <th>four</th>
      <th>five</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <th>New York</th>
      <td>12</td>
      <td>13</td>
      <td>14</td>
      <td>15</td>
      <td>6</td>
    </tr>
    <tr>
      <th>Utah</th>
      <td>8</td>
      <td>9</td>
      <td>10</td>
      <td>11</td>
      <td>5</td>
    </tr>
  </tbody>
</table>
</div>


```python
dataframe_index.iloc[2:4,1:3] #与NumPy切片的方式类似
```


<div>
<style scoped>
    .dataframe tbody tr th:only-of-type {
        vertical-align: middle;
    }
    .dataframe tbody tr th {
        vertical-align: top;
    }
    .dataframe thead th {
        text-align: right;
    }
</style>
<table border="1" class="dataframe">
  <thead>
    <tr style="text-align: right;">
      <th></th>
      <th>two</th>
      <th>three</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <th>Utah</th>
      <td>9</td>
      <td>10</td>
    </tr>
    <tr>
      <th>New York</th>
      <td>13</td>
      <td>14</td>
    </tr>
  </tbody>
</table>
</div>


```python
dataframe_index.iloc[[2,3],[0,1]] #如果传入两个list进行索引，则分别对应于索引的行与列，类似于NumPy.ix_()函数。
```


<div>
<style scoped>
    .dataframe tbody tr th:only-of-type {
        vertical-align: middle;
    }
    .dataframe tbody tr th {
        vertical-align: top;
    }
    .dataframe thead th {
        text-align: right;
    }
</style>
<table border="1" class="dataframe">
  <thead>
    <tr style="text-align: right;">
      <th></th>
      <th>one</th>
      <th>two</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <th>Utah</th>
      <td>8</td>
      <td>9</td>
    </tr>
    <tr>
      <th>New York</th>
      <td>12</td>
      <td>13</td>
    </tr>
  </tbody>
</table>
</div>


```python
dataframe_index.at['Utah','one'] #只能索引某个元素，不支持切片以及传入list
```


    8




```python
dataframe_index.iat[1,1] #使用整数坐标来索引某个元素
```


    5



## 数学运算

Series和DataFrame支持一些数学运算，包括:

- `add()`，或使用运算符+
- `sub()`，或使用运算符-
- `mul()`，或使用运算符*
- `div()`，或使用运算符/

以加法为例，对两个index不相同的Series相加时，最后得到的index是两个index的并集。默认情况下，运算结果满足以下规律：如果是两个index里面都有的key，则它对应的结果是这个key在两个Series里对应的value之和；如果某些key只出现在其中一个index里面，则相加后对应的value值为空。

DataFrame也满足上述的规则，并且在考虑index的基础上，也要考虑columns。

下面是一些使用示例：


```python
dataframe_math1=pd.DataFrame(np.arange(12.).reshape((3, 4)), columns=list('abcd'))
dataframe_math2=pd.DataFrame(np.arange(20.).reshape((4, 5)), columns=list('abcde'))
```


```python
dataframe_math1+dataframe_math2
```


<div>
<style scoped>
    .dataframe tbody tr th:only-of-type {
        vertical-align: middle;
    }
    .dataframe tbody tr th {
        vertical-align: top;
    }
    .dataframe thead th {
        text-align: right;
    }
</style>
<table border="1" class="dataframe">
  <thead>
    <tr style="text-align: right;">
      <th></th>
      <th>a</th>
      <th>b</th>
      <th>c</th>
      <th>d</th>
      <th>e</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <th>0</th>
      <td>0.0</td>
      <td>2.0</td>
      <td>4.0</td>
      <td>6.0</td>
      <td>NaN</td>
    </tr>
    <tr>
      <th>1</th>
      <td>9.0</td>
      <td>11.0</td>
      <td>13.0</td>
      <td>15.0</td>
      <td>NaN</td>
    </tr>
    <tr>
      <th>2</th>
      <td>18.0</td>
      <td>20.0</td>
      <td>22.0</td>
      <td>24.0</td>
      <td>NaN</td>
    </tr>
    <tr>
      <th>3</th>
      <td>NaN</td>
      <td>NaN</td>
      <td>NaN</td>
      <td>NaN</td>
      <td>NaN</td>
    </tr>
  </tbody>
</table>
</div>


```python
dataframe_math1.add(dataframe_math2,fill_value=0) #与上面一条语句的作用相同。但是add函数支持传入fill_value参数，此时如果某些key只在一个DataFrame中出现，那么另一个DataFrame中相应的位置会被补上传入的值来参与运算。
```


<div>
<style scoped>
    .dataframe tbody tr th:only-of-type {
        vertical-align: middle;
    }
    .dataframe tbody tr th {
        vertical-align: top;
    }
    .dataframe thead th {
        text-align: right;
    }
</style>
<table border="1" class="dataframe">
  <thead>
    <tr style="text-align: right;">
      <th></th>
      <th>a</th>
      <th>b</th>
      <th>c</th>
      <th>d</th>
      <th>e</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <th>0</th>
      <td>0.0</td>
      <td>2.0</td>
      <td>4.0</td>
      <td>6.0</td>
      <td>4.0</td>
    </tr>
    <tr>
      <th>1</th>
      <td>9.0</td>
      <td>11.0</td>
      <td>13.0</td>
      <td>15.0</td>
      <td>9.0</td>
    </tr>
    <tr>
      <th>2</th>
      <td>18.0</td>
      <td>20.0</td>
      <td>22.0</td>
      <td>24.0</td>
      <td>14.0</td>
    </tr>
    <tr>
      <th>3</th>
      <td>15.0</td>
      <td>16.0</td>
      <td>17.0</td>
      <td>18.0</td>
      <td>19.0</td>
    </tr>
  </tbody>
</table>
</div>



如果是一个DataFrame和Series进行运算，那么默认会把Series的元素按行复制进行扩展，然后参与运算。如果Series的index和DataFrame的columns不是完全相同，也可以进行运算，运算规则与两个DataFrame的运算规则类似。例如：


```python
series_math1=dataframe_math1.iloc[0]
dataframe_math1-series_math1
```


<div>
<style scoped>
    .dataframe tbody tr th:only-of-type {
        vertical-align: middle;
    }
    .dataframe tbody tr th {
        vertical-align: top;
    }
    .dataframe thead th {
        text-align: right;
    }
</style>
<table border="1" class="dataframe">
  <thead>
    <tr style="text-align: right;">
      <th></th>
      <th>a</th>
      <th>b</th>
      <th>c</th>
      <th>d</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <th>0</th>
      <td>0.0</td>
      <td>0.0</td>
      <td>0.0</td>
      <td>0.0</td>
    </tr>
    <tr>
      <th>1</th>
      <td>4.0</td>
      <td>4.0</td>
      <td>4.0</td>
      <td>4.0</td>
    </tr>
    <tr>
      <th>2</th>
      <td>8.0</td>
      <td>8.0</td>
      <td>8.0</td>
      <td>8.0</td>
    </tr>
  </tbody>
</table>
</div>


```python
series_math2=series_math1.reindex(['a','b','c','e'])
dataframe_math1-series_math2
```


<div>
<style scoped>
    .dataframe tbody tr th:only-of-type {
        vertical-align: middle;
    }
    .dataframe tbody tr th {
        vertical-align: top;
    }
    .dataframe thead th {
        text-align: right;
    }
</style>
<table border="1" class="dataframe">
  <thead>
    <tr style="text-align: right;">
      <th></th>
      <th>a</th>
      <th>b</th>
      <th>c</th>
      <th>d</th>
      <th>e</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <th>0</th>
      <td>0.0</td>
      <td>0.0</td>
      <td>0.0</td>
      <td>NaN</td>
      <td>NaN</td>
    </tr>
    <tr>
      <th>1</th>
      <td>4.0</td>
      <td>4.0</td>
      <td>4.0</td>
      <td>NaN</td>
      <td>NaN</td>
    </tr>
    <tr>
      <th>2</th>
      <td>8.0</td>
      <td>8.0</td>
      <td>8.0</td>
      <td>NaN</td>
      <td>NaN</td>
    </tr>
  </tbody>
</table>
</div>


```python
series_math3=dataframe_math1['d']
dataframe_math1.subtract(series_math3,axis=0) #要想按列进行扩展，需要调用函数，并且传入axis=0这一可选参数。
```


<div>
<style scoped>
    .dataframe tbody tr th:only-of-type {
        vertical-align: middle;
    }
    .dataframe tbody tr th {
        vertical-align: top;
    }
    .dataframe thead th {
        text-align: right;
    }
</style>
<table border="1" class="dataframe">
  <thead>
    <tr style="text-align: right;">
      <th></th>
      <th>a</th>
      <th>b</th>
      <th>c</th>
      <th>d</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <th>0</th>
      <td>-3.0</td>
      <td>-2.0</td>
      <td>-1.0</td>
      <td>0.0</td>
    </tr>
    <tr>
      <th>1</th>
      <td>-3.0</td>
      <td>-2.0</td>
      <td>-1.0</td>
      <td>0.0</td>
    </tr>
    <tr>
      <th>2</th>
      <td>-3.0</td>
      <td>-2.0</td>
      <td>-1.0</td>
      <td>0.0</td>
    </tr>
  </tbody>
</table>
</div>



## 函数与映射

Numpy中的ufunc（即一元函数）也可以使用pandas中的series和dataframe类型的变量作为输入参数。下面为一些使用示例：


```python
dataframe_func = pd.DataFrame(np.random.randn(4, 3), columns=list('bde'),index=['Utah', 'Ohio', 'Texas', 'Oregon'])
dataframe_func
```


<div>
<style scoped>
    .dataframe tbody tr th:only-of-type {
        vertical-align: middle;
    }
    .dataframe tbody tr th {
        vertical-align: top;
    }
    .dataframe thead th {
        text-align: right;
    }
</style>
<table border="1" class="dataframe">
  <thead>
    <tr style="text-align: right;">
      <th></th>
      <th>b</th>
      <th>d</th>
      <th>e</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <th>Utah</th>
      <td>0.452034</td>
      <td>-1.067267</td>
      <td>2.235212</td>
    </tr>
    <tr>
      <th>Ohio</th>
      <td>-1.205085</td>
      <td>0.899914</td>
      <td>-1.097603</td>
    </tr>
    <tr>
      <th>Texas</th>
      <td>-1.553192</td>
      <td>-0.820639</td>
      <td>0.025214</td>
    </tr>
    <tr>
      <th>Oregon</th>
      <td>-0.047253</td>
      <td>1.092414</td>
      <td>-0.302945</td>
    </tr>
  </tbody>
</table>
</div>


```python
np.exp(dataframe_func)
```


<div>
<style scoped>
    .dataframe tbody tr th:only-of-type {
        vertical-align: middle;
    }
    .dataframe tbody tr th {
        vertical-align: top;
    }
    .dataframe thead th {
        text-align: right;
    }
</style>
<table border="1" class="dataframe">
  <thead>
    <tr style="text-align: right;">
      <th></th>
      <th>b</th>
      <th>d</th>
      <th>e</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <th>Utah</th>
      <td>1.571506</td>
      <td>0.343947</td>
      <td>9.348460</td>
    </tr>
    <tr>
      <th>Ohio</th>
      <td>0.299666</td>
      <td>2.459391</td>
      <td>0.333670</td>
    </tr>
    <tr>
      <th>Texas</th>
      <td>0.211572</td>
      <td>0.440151</td>
      <td>1.025534</td>
    </tr>
    <tr>
      <th>Oregon</th>
      <td>0.953846</td>
      <td>2.981464</td>
      <td>0.738640</td>
    </tr>
  </tbody>
</table>
</div>



同时也可以自己定义函数，然后使用`apply`函数作用于每一行或者每一列。如果函数返回一个数值，那么最后生成一个Series；如果函数返回一个矢量值，最后生成一个DataFrame。


```python
f1=lambda x:x.max()-x.min()
```


```python
dataframe_func.apply(f1) #默认作用于行
```


    b    2.005226
    d    2.159681
    e    3.332815
    dtype: float64




```python
dataframe_func.apply(f1,axis=1) #如果要让函数作用于列，则可以传入axis参数指定方向
```


    Utah      3.302478
    Ohio      2.104999
    Texas     1.578405
    Oregon    1.395360
    dtype: float64

> 备注：在Pandas的一些函数中，如果传入`axis=……`参数，则代表这一函数操作在这个传入的维度上面进行。假设有一个DataFrame，它的元素可以用$a_{ij}$来表示，当我们在使用`sum()`函数时传入`axis=1`，那么最终得到的结果便可以用数学公式表示为$\sum_{j=1}^{n}a_{ij}$，也就是说这个操作是在维度$j$上进行的，也就是水平方向。



`applymap`方法可以让函数作用于每一个元素：


```python
f2=lambda x: '%.2f'%x
dataframe_func.applymap(f2)
```


<div>
<style scoped>
    .dataframe tbody tr th:only-of-type {
        vertical-align: middle;
    }
    .dataframe tbody tr th {
        vertical-align: top;
    }
    .dataframe thead th {
        text-align: right;
    }
</style>
<table border="1" class="dataframe">
  <thead>
    <tr style="text-align: right;">
      <th></th>
      <th>b</th>
      <th>d</th>
      <th>e</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <th>Utah</th>
      <td>0.45</td>
      <td>-1.07</td>
      <td>2.24</td>
    </tr>
    <tr>
      <th>Ohio</th>
      <td>-1.21</td>
      <td>0.90</td>
      <td>-1.10</td>
    </tr>
    <tr>
      <th>Texas</th>
      <td>-1.55</td>
      <td>-0.82</td>
      <td>0.03</td>
    </tr>
    <tr>
      <th>Oregon</th>
      <td>-0.05</td>
      <td>1.09</td>
      <td>-0.30</td>
    </tr>
  </tbody>
</table>
</div>



## 排序

Series或DataFrame的index和columns可以使用`sort_index()`函数对其进行排序，使得行或列按照index排序后的顺序进行排列。`sort_index()`函数生成的是原始数据的一个副本。

示例：


```python
series_sort=pd.Series(range(4), index=['d', 'a', 'b', 'c'])
series_sort.sort_index()
```


    a    1
    b    2
    c    3
    d    0
    dtype: int64




```python
dataframe_sort=pd.DataFrame(np.arange(8).reshape((2, 4)), index=['three', 'one'], columns=['d', 'a', 'b', 'c'])
```


```python
dataframe_sort.sort_index() #DataFrame默认对index进行排序
```


<div>
<style scoped>
    .dataframe tbody tr th:only-of-type {
        vertical-align: middle;
    }
    .dataframe tbody tr th {
        vertical-align: top;
    }
    .dataframe thead th {
        text-align: right;
    }
</style>
<table border="1" class="dataframe">
  <thead>
    <tr style="text-align: right;">
      <th></th>
      <th>d</th>
      <th>a</th>
      <th>b</th>
      <th>c</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <th>one</th>
      <td>4</td>
      <td>5</td>
      <td>6</td>
      <td>7</td>
    </tr>
    <tr>
      <th>three</th>
      <td>0</td>
      <td>1</td>
      <td>2</td>
      <td>3</td>
    </tr>
  </tbody>
</table>
</div>


```python
dataframe_sort.sort_index(axis=1) #可以传入axis参数来对columns进行排序
```


<div>
<style scoped>
    .dataframe tbody tr th:only-of-type {
        vertical-align: middle;
    }
    .dataframe tbody tr th {
        vertical-align: top;
    }
    .dataframe thead th {
        text-align: right;
    }
</style>
<table border="1" class="dataframe">
  <thead>
    <tr style="text-align: right;">
      <th></th>
      <th>a</th>
      <th>b</th>
      <th>c</th>
      <th>d</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <th>three</th>
      <td>1</td>
      <td>2</td>
      <td>3</td>
      <td>0</td>
    </tr>
    <tr>
      <th>one</th>
      <td>5</td>
      <td>6</td>
      <td>7</td>
      <td>4</td>
    </tr>
  </tbody>
</table>
</div>


```python
dataframe_sort.sort_index(axis=1,ascending=False) #默认按照升序排列，如果要降序排列则需要设置ascending=False
```


<div>
<style scoped>
    .dataframe tbody tr th:only-of-type {
        vertical-align: middle;
    }
    .dataframe tbody tr th {
        vertical-align: top;
    }
    .dataframe thead th {
        text-align: right;
    }
</style>
<table border="1" class="dataframe">
  <thead>
    <tr style="text-align: right;">
      <th></th>
      <th>d</th>
      <th>c</th>
      <th>b</th>
      <th>a</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <th>three</th>
      <td>0</td>
      <td>3</td>
      <td>2</td>
      <td>1</td>
    </tr>
    <tr>
      <th>one</th>
      <td>4</td>
      <td>7</td>
      <td>6</td>
      <td>5</td>
    </tr>
  </tbody>
</table>
</div>



如果想对Series或者DataFrame按照元素的大小顺序进行排序，可以用`sort_values()`方法，其中NaN值会自动排在最后：


```python
dataframe_sort2=pd.DataFrame({'b': [4, 7, -3, 2], 'a': [0, 1, 0, 1]})
```


```python
dataframe_sort2.sort_values(by='b') #DataFrame需要传入一个by参数，指定按照某一列的元素大小排序。注意：不能按某一行进行排序！
```


<div>
<style scoped>
    .dataframe tbody tr th:only-of-type {
        vertical-align: middle;
    }
    .dataframe tbody tr th {
        vertical-align: top;
    }
    .dataframe thead th {
        text-align: right;
    }
</style>
<table border="1" class="dataframe">
  <thead>
    <tr style="text-align: right;">
      <th></th>
      <th>b</th>
      <th>a</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <th>2</th>
      <td>-3</td>
      <td>0</td>
    </tr>
    <tr>
      <th>3</th>
      <td>2</td>
      <td>1</td>
    </tr>
    <tr>
      <th>0</th>
      <td>4</td>
      <td>0</td>
    </tr>
    <tr>
      <th>1</th>
      <td>7</td>
      <td>1</td>
    </tr>
  </tbody>
</table>
</div>


```python
dataframe_sort2.sort_values(by=['a','b']) #by参数可以传入一个list选择多列，代表先按照list中第一个元素对应的列进行排序，如果出现相同值则使用第二个元素对应的列来排序这些相同值，以此类推
```


<div>
<style scoped>
    .dataframe tbody tr th:only-of-type {
        vertical-align: middle;
    }
    .dataframe tbody tr th {
        vertical-align: top;
    }
    .dataframe thead th {
        text-align: right;
    }
</style>
<table border="1" class="dataframe">
  <thead>
    <tr style="text-align: right;">
      <th></th>
      <th>b</th>
      <th>a</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <th>2</th>
      <td>-3</td>
      <td>0</td>
    </tr>
    <tr>
      <th>0</th>
      <td>4</td>
      <td>0</td>
    </tr>
    <tr>
      <th>3</th>
      <td>2</td>
      <td>1</td>
    </tr>
    <tr>
      <th>1</th>
      <td>7</td>
      <td>1</td>
    </tr>
  </tbody>
</table>
</div>



使用`rank()`方法，可以生成Series或DataFrame中的元素的排列次序，类似于NumPy的argsort()函数。

`rank()`在处理重复值的时候，有如下几种方法：

- average：生成重复值对应顺序的平均值。例如两个重复值分别排1和2，则它们的rank都为1.5
- min：重复值使用最小的那个rank
- max：重复值使用最大的那个rank
- first：使用重复值中第一个出现的那个对应的rank


```python
dataframe_sort2.rank() #默认按列做rank操作，使用average方法处理重复值
```


<div>
<style scoped>
    .dataframe tbody tr th:only-of-type {
        vertical-align: middle;
    }
    .dataframe tbody tr th {
        vertical-align: top;
    }
    .dataframe thead th {
        text-align: right;
    }
</style>
<table border="1" class="dataframe">
  <thead>
    <tr style="text-align: right;">
      <th></th>
      <th>b</th>
      <th>a</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <th>0</th>
      <td>3.0</td>
      <td>1.5</td>
    </tr>
    <tr>
      <th>1</th>
      <td>4.0</td>
      <td>3.5</td>
    </tr>
    <tr>
      <th>2</th>
      <td>1.0</td>
      <td>1.5</td>
    </tr>
    <tr>
      <th>3</th>
      <td>2.0</td>
      <td>3.5</td>
    </tr>
  </tbody>
</table>
</div>


```python
dataframe_sort2.rank(axis=1) #可以传入axis参数来指定rank操作的方向
```


<div>
<style scoped>
    .dataframe tbody tr th:only-of-type {
        vertical-align: middle;
    }
    .dataframe tbody tr th {
        vertical-align: top;
    }
    .dataframe thead th {
        text-align: right;
    }
</style>
<table border="1" class="dataframe">
  <thead>
    <tr style="text-align: right;">
      <th></th>
      <th>b</th>
      <th>a</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <th>0</th>
      <td>2.0</td>
      <td>1.0</td>
    </tr>
    <tr>
      <th>1</th>
      <td>2.0</td>
      <td>1.0</td>
    </tr>
    <tr>
      <th>2</th>
      <td>1.0</td>
      <td>2.0</td>
    </tr>
    <tr>
      <th>3</th>
      <td>2.0</td>
      <td>1.0</td>
    </tr>
  </tbody>
</table>
</div>


```python
dataframe_sort2.rank(method='max',ascending=False) #要改变对重复值的处理方式可以传入method参数，使用降序可以传入ascending=False参数
```


<div>
<style scoped>
    .dataframe tbody tr th:only-of-type {
        vertical-align: middle;
    }
    .dataframe tbody tr th {
        vertical-align: top;
    }
    .dataframe thead th {
        text-align: right;
    }
</style>
<table border="1" class="dataframe">
  <thead>
    <tr style="text-align: right;">
      <th></th>
      <th>b</th>
      <th>a</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <th>0</th>
      <td>2.0</td>
      <td>4.0</td>
    </tr>
    <tr>
      <th>1</th>
      <td>1.0</td>
      <td>2.0</td>
    </tr>
    <tr>
      <th>2</th>
      <td>4.0</td>
      <td>4.0</td>
    </tr>
    <tr>
      <th>3</th>
      <td>3.0</td>
      <td>2.0</td>
    </tr>
  </tbody>
</table>
</div>



# 统计函数

Pandas支持对Series和DataFrame中的数据做一些数学统计，常用的统计函数包括：

- count：统计非NaN元素的数目
- describe：自动统计dataframe中每一列的数据数目，平均值，标准差，最小值，25%，50%，75%，100%分位数，可以用于分析数据的大致统计规律
- min、max：统计最小或最大值
- argmin、argmax：统计最小或最大值出现的位置，返回对应的序号
- idxmin、idxmax：统计最小或最大值出现的位置，返回对应的index值
- quantile：统计某个百分位数（需要传入一个0到1中间的数字）
- sum：统计所有元素的和
- mean：统计所有元素的平均数
- median：统计所有元素的中位数
- mad：统计相较于平均值的绝对偏差的平均值
- var：统计方差
- std：统计标准差
- skew：统计偏度，表示随机变量与中心分布的不对称程度$（μ^3/σ^3）$，右偏为正，左偏为负
- kurt：统计峰度，表征概率密度分布曲线在平均值处峰值高低的特征数$（μ^4/σ^4）$。以正态分布为界，如果比正态分布陡，峰度大于0，否则小于0
- cumsum：计算累积和（即保留计算总和时中间每一步的结果）
- cummin、cummax：计算累积最小值或最大值
- cumprod：计算累积乘积
- diff：计算一阶算数偏差（即拿第二个值减第一个，第三个值减第二个，以此类推）
- pct_change：计算百分数改变

在统计函数中，可以使用的一些参数包括：

- axis：指定统计函数在哪个维度上计算，0为行，1为列（即0是按照竖直方向，1是按照水平方向）
- skipna：是否跳过NaN，默认为True
- level：如果使用多重index，则对每一个层级的结果进行分组

下面为一些使用示例：


```python
dataframe_stat=pd.DataFrame(np.random.rand(5,5),columns=['a','b','c','d','e'])
dataframe_stat
```


<div>
<style scoped>
    .dataframe tbody tr th:only-of-type {
        vertical-align: middle;
    }
    .dataframe tbody tr th {
        vertical-align: top;
    }
    .dataframe thead th {
        text-align: right;
    }
</style>
<table border="1" class="dataframe">
  <thead>
    <tr style="text-align: right;">
      <th></th>
      <th>a</th>
      <th>b</th>
      <th>c</th>
      <th>d</th>
      <th>e</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <th>0</th>
      <td>0.925587</td>
      <td>0.285093</td>
      <td>0.016950</td>
      <td>0.140965</td>
      <td>0.179401</td>
    </tr>
    <tr>
      <th>1</th>
      <td>0.213783</td>
      <td>0.146496</td>
      <td>0.816971</td>
      <td>0.065923</td>
      <td>0.067685</td>
    </tr>
    <tr>
      <th>2</th>
      <td>0.615921</td>
      <td>0.717184</td>
      <td>0.375118</td>
      <td>0.233150</td>
      <td>0.188625</td>
    </tr>
    <tr>
      <th>3</th>
      <td>0.469468</td>
      <td>0.960874</td>
      <td>0.769445</td>
      <td>0.503390</td>
      <td>0.928483</td>
    </tr>
    <tr>
      <th>4</th>
      <td>0.480754</td>
      <td>0.340058</td>
      <td>0.424418</td>
      <td>0.566608</td>
      <td>0.810999</td>
    </tr>
  </tbody>
</table>
</div>


```python
dataframe_stat.pct_change() #如果不指定axis则默认是按行做计算
```


<div>
<style scoped>
    .dataframe tbody tr th:only-of-type {
        vertical-align: middle;
    }
    .dataframe tbody tr th {
        vertical-align: top;
    }
    .dataframe thead th {
        text-align: right;
    }
</style>
<table border="1" class="dataframe">
  <thead>
    <tr style="text-align: right;">
      <th></th>
      <th>a</th>
      <th>b</th>
      <th>c</th>
      <th>d</th>
      <th>e</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <th>0</th>
      <td>NaN</td>
      <td>NaN</td>
      <td>NaN</td>
      <td>NaN</td>
      <td>NaN</td>
    </tr>
    <tr>
      <th>1</th>
      <td>-0.769030</td>
      <td>-0.486148</td>
      <td>47.199252</td>
      <td>-0.532347</td>
      <td>-0.622717</td>
    </tr>
    <tr>
      <th>2</th>
      <td>1.881058</td>
      <td>3.895602</td>
      <td>-0.540843</td>
      <td>2.536707</td>
      <td>1.786814</td>
    </tr>
    <tr>
      <th>3</th>
      <td>-0.237778</td>
      <td>0.339787</td>
      <td>1.051211</td>
      <td>1.159082</td>
      <td>3.922365</td>
    </tr>
    <tr>
      <th>4</th>
      <td>0.024039</td>
      <td>-0.646095</td>
      <td>-0.448410</td>
      <td>0.125585</td>
      <td>-0.126533</td>
    </tr>
  </tbody>
</table>
</div>


```python
dataframe_stat.describe()
```


<div>
<style scoped>
    .dataframe tbody tr th:only-of-type {
        vertical-align: middle;
    }
    .dataframe tbody tr th {
        vertical-align: top;
    }
    .dataframe thead th {
        text-align: right;
    }
</style>
<table border="1" class="dataframe">
  <thead>
    <tr style="text-align: right;">
      <th></th>
      <th>a</th>
      <th>b</th>
      <th>c</th>
      <th>d</th>
      <th>e</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <th>count</th>
      <td>5.000000</td>
      <td>5.000000</td>
      <td>5.000000</td>
      <td>5.000000</td>
      <td>5.000000</td>
    </tr>
    <tr>
      <th>mean</th>
      <td>0.541102</td>
      <td>0.489941</td>
      <td>0.480580</td>
      <td>0.302007</td>
      <td>0.435039</td>
    </tr>
    <tr>
      <th>std</th>
      <td>0.259489</td>
      <td>0.337565</td>
      <td>0.326279</td>
      <td>0.221912</td>
      <td>0.401825</td>
    </tr>
    <tr>
      <th>min</th>
      <td>0.213783</td>
      <td>0.146496</td>
      <td>0.016950</td>
      <td>0.065923</td>
      <td>0.067685</td>
    </tr>
    <tr>
      <th>25%</th>
      <td>0.469468</td>
      <td>0.285093</td>
      <td>0.375118</td>
      <td>0.140965</td>
      <td>0.179401</td>
    </tr>
    <tr>
      <th>50%</th>
      <td>0.480754</td>
      <td>0.340058</td>
      <td>0.424418</td>
      <td>0.233150</td>
      <td>0.188625</td>
    </tr>
    <tr>
      <th>75%</th>
      <td>0.615921</td>
      <td>0.717184</td>
      <td>0.769445</td>
      <td>0.503390</td>
      <td>0.810999</td>
    </tr>
    <tr>
      <th>max</th>
      <td>0.925587</td>
      <td>0.960874</td>
      <td>0.816971</td>
      <td>0.566608</td>
      <td>0.928483</td>
    </tr>
  </tbody>
</table>
</div>


```python
dataframe_stat.min(axis=1) #可以通过传入axis来指定计算方向
```


    0    0.016950
    1    0.065923
    2    0.188625
    3    0.469468
    4    0.340058
    dtype: float64



Pandas也提供了协方差和相关系数的计算，例如：


```python
dataframe_stat.corr() #对于一个DataFrame，计算所有列与列之间的相关系数
```


<div>
<style scoped>
    .dataframe tbody tr th:only-of-type {
        vertical-align: middle;
    }
    .dataframe tbody tr th {
        vertical-align: top;
    }
    .dataframe thead th {
        text-align: right;
    }
</style>
<table border="1" class="dataframe">
  <thead>
    <tr style="text-align: right;">
      <th></th>
      <th>a</th>
      <th>b</th>
      <th>c</th>
      <th>d</th>
      <th>e</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <th>a</th>
      <td>1.000000</td>
      <td>0.074114</td>
      <td>-0.925876</td>
      <td>-0.087650</td>
      <td>-0.130717</td>
    </tr>
    <tr>
      <th>b</th>
      <td>0.074114</td>
      <td>1.000000</td>
      <td>0.226825</td>
      <td>0.512625</td>
      <td>0.550285</td>
    </tr>
    <tr>
      <th>c</th>
      <td>-0.925876</td>
      <td>0.226825</td>
      <td>1.000000</td>
      <td>0.158210</td>
      <td>0.271455</td>
    </tr>
    <tr>
      <th>d</th>
      <td>-0.087650</td>
      <td>0.512625</td>
      <td>0.158210</td>
      <td>1.000000</td>
      <td>0.963646</td>
    </tr>
    <tr>
      <th>e</th>
      <td>-0.130717</td>
      <td>0.550285</td>
      <td>0.271455</td>
      <td>0.963646</td>
      <td>1.000000</td>
    </tr>
  </tbody>
</table>
</div>


```python
dataframe_stat.cov() #计算所有列与列之间两两的协方差
```


<div>
<style scoped>
    .dataframe tbody tr th:only-of-type {
        vertical-align: middle;
    }
    .dataframe tbody tr th {
        vertical-align: top;
    }
    .dataframe thead th {
        text-align: right;
    }
</style>
<table border="1" class="dataframe">
  <thead>
    <tr style="text-align: right;">
      <th></th>
      <th>a</th>
      <th>b</th>
      <th>c</th>
      <th>d</th>
      <th>e</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <th>a</th>
      <td>0.067334</td>
      <td>0.006492</td>
      <td>-0.078390</td>
      <td>-0.005047</td>
      <td>-0.013630</td>
    </tr>
    <tr>
      <th>b</th>
      <td>0.006492</td>
      <td>0.113950</td>
      <td>0.024983</td>
      <td>0.038401</td>
      <td>0.074642</td>
    </tr>
    <tr>
      <th>c</th>
      <td>-0.078390</td>
      <td>0.024983</td>
      <td>0.106458</td>
      <td>0.011455</td>
      <td>0.035590</td>
    </tr>
    <tr>
      <th>d</th>
      <td>-0.005047</td>
      <td>0.038401</td>
      <td>0.011455</td>
      <td>0.049245</td>
      <td>0.085928</td>
    </tr>
    <tr>
      <th>e</th>
      <td>-0.013630</td>
      <td>0.074642</td>
      <td>0.035590</td>
      <td>0.085928</td>
      <td>0.161463</td>
    </tr>
  </tbody>
</table>
</div>


```python
dataframe_stat['a'].cov(dataframe_stat['c']) #对于一个Series来说，需要传入另一个规模相同的Series来进行计算
```


    -0.07838993925634331



此外，还有一些用于统计Series中数据数目以及查找数据的函数，下面为一些使用示例：


```python
series_stat=pd.Series([1,2,3,1,1,1,1,2,4,5,5])
series_stat
```


    0     1
    1     2
    2     3
    3     1
    4     1
    5     1
    6     1
    7     2
    8     4
    9     5
    10    5
    dtype: int64




```python
series_stat.value_counts() #统计Series中每个元素出现的次数
```


    1    5
    5    2
    2    2
    4    1
    3    1
    dtype: int64




```python
series_stat.unique() #对Series中的所有元素去重，元素的排列顺序按照它们第一次出现的顺序
```


    array([1, 2, 3, 4, 5], dtype=int64)




```python
series_stat.isin([3,4,6]) #传入一个list，输出一个布尔类型的Series，对应于Series中的元素是否包含在传入的list当中
```


    0     False
    1     False
    2      True
    3     False
    4     False
    5     False
    6     False
    7     False
    8      True
    9     False
    10    False
    dtype: bool




```python
pd.DataFrame([series_stat,series_stat,series_stat]).apply(pd.value_counts) #如果要对DataFrame使用上述三个函数，可以借助apply函数
```


<div>
<style scoped>
    .dataframe tbody tr th:only-of-type {
        vertical-align: middle;
    }
    .dataframe tbody tr th {
        vertical-align: top;
    }
    .dataframe thead th {
        text-align: right;
    }
</style>
<table border="1" class="dataframe">
  <thead>
    <tr style="text-align: right;">
      <th></th>
      <th>0</th>
      <th>1</th>
      <th>2</th>
      <th>3</th>
      <th>4</th>
      <th>5</th>
      <th>6</th>
      <th>7</th>
      <th>8</th>
      <th>9</th>
      <th>10</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <th>1</th>
      <td>3.0</td>
      <td>NaN</td>
      <td>NaN</td>
      <td>3.0</td>
      <td>3.0</td>
      <td>3.0</td>
      <td>3.0</td>
      <td>NaN</td>
      <td>NaN</td>
      <td>NaN</td>
      <td>NaN</td>
    </tr>
    <tr>
      <th>2</th>
      <td>NaN</td>
      <td>3.0</td>
      <td>NaN</td>
      <td>NaN</td>
      <td>NaN</td>
      <td>NaN</td>
      <td>NaN</td>
      <td>3.0</td>
      <td>NaN</td>
      <td>NaN</td>
      <td>NaN</td>
    </tr>
    <tr>
      <th>3</th>
      <td>NaN</td>
      <td>NaN</td>
      <td>3.0</td>
      <td>NaN</td>
      <td>NaN</td>
      <td>NaN</td>
      <td>NaN</td>
      <td>NaN</td>
      <td>NaN</td>
      <td>NaN</td>
      <td>NaN</td>
    </tr>
    <tr>
      <th>4</th>
      <td>NaN</td>
      <td>NaN</td>
      <td>NaN</td>
      <td>NaN</td>
      <td>NaN</td>
      <td>NaN</td>
      <td>NaN</td>
      <td>NaN</td>
      <td>3.0</td>
      <td>NaN</td>
      <td>NaN</td>
    </tr>
    <tr>
      <th>5</th>
      <td>NaN</td>
      <td>NaN</td>
      <td>NaN</td>
      <td>NaN</td>
      <td>NaN</td>
      <td>NaN</td>
      <td>NaN</td>
      <td>NaN</td>
      <td>NaN</td>
      <td>3.0</td>
      <td>3.0</td>
    </tr>
  </tbody>
</table>
</div>



# 数据清洗

## 处理丢失数据

在Pandas中，可能会遇到Series或者DataFrame中存在丢失数据的情况。对于如何处理丢失数据，Pandas提供了一些相关的函数。


```python
dataframe_nan=pd.DataFrame(
    {'Nevada': {2001:2.4, 2002:2.9, 2003:3.3}, 
    'Ohio': {2000:1.5, 2001:1.7, 2003:3.6},
    'New York':{2003:3.5, 2004:5.7},
    'Denver':{2000:0.5, 2001:1.1, 2002:2.1, 2003:1.5, 2004:0.9}},
    index=[2000,2001,2002,2003,2004,2005])
dataframe_nan
```


<div>
<style scoped>
    .dataframe tbody tr th:only-of-type {
        vertical-align: middle;
    }
    .dataframe tbody tr th {
        vertical-align: top;
    }
    .dataframe thead th {
        text-align: right;
    }
</style>
<table border="1" class="dataframe">
  <thead>
    <tr style="text-align: right;">
      <th></th>
      <th>Nevada</th>
      <th>Ohio</th>
      <th>New York</th>
      <th>Denver</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <th>2000</th>
      <td>NaN</td>
      <td>1.5</td>
      <td>NaN</td>
      <td>0.5</td>
    </tr>
    <tr>
      <th>2001</th>
      <td>2.4</td>
      <td>1.7</td>
      <td>NaN</td>
      <td>1.1</td>
    </tr>
    <tr>
      <th>2002</th>
      <td>2.9</td>
      <td>NaN</td>
      <td>NaN</td>
      <td>2.1</td>
    </tr>
    <tr>
      <th>2003</th>
      <td>3.3</td>
      <td>3.6</td>
      <td>3.5</td>
      <td>1.5</td>
    </tr>
    <tr>
      <th>2004</th>
      <td>NaN</td>
      <td>NaN</td>
      <td>5.7</td>
      <td>0.9</td>
    </tr>
    <tr>
      <th>2005</th>
      <td>NaN</td>
      <td>NaN</td>
      <td>NaN</td>
      <td>NaN</td>
    </tr>
  </tbody>
</table>
</div>
要判断某个位置是否是NaN，可以使用`isnull()`函数，这一函数返回一个bool类型的Series或者DataFrame，指示每个位置上是否为NaN。



`dropna()`函数可以将丢失的数据删除掉。对于Series来说，丢失的数据会被直接删除掉；而对于DataFrame来说，默认操作是只要一行内有一个NaN就把这一行全部删除。下面是一些程序示例：


```python
dataframe_nan.dropna() #默认为按行删除，且只要一行内有一个NaN就把该行全部删除
```


<div>
<style scoped>
    .dataframe tbody tr th:only-of-type {
        vertical-align: middle;
    }
    .dataframe tbody tr th {
        vertical-align: top;
    }
    .dataframe thead th {
        text-align: right;
    }
</style>
<table border="1" class="dataframe">
  <thead>
    <tr style="text-align: right;">
      <th></th>
      <th>Nevada</th>
      <th>Ohio</th>
      <th>New York</th>
      <th>Denver</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <th>2003</th>
      <td>3.3</td>
      <td>3.6</td>
      <td>3.5</td>
      <td>1.5</td>
    </tr>
  </tbody>
</table>
</div>




```python
dataframe_nan.dropna(how='all') #可以传入how='all'参数，这样只有一行全部为NaN时才会删除该行
```


<div>
<style scoped>
    .dataframe tbody tr th:only-of-type {
        vertical-align: middle;
    }
    .dataframe tbody tr th {
        vertical-align: top;
    }
    .dataframe thead th {
        text-align: right;
    }
</style>
<table border="1" class="dataframe">
  <thead>
    <tr style="text-align: right;">
      <th></th>
      <th>Nevada</th>
      <th>Ohio</th>
      <th>New York</th>
      <th>Denver</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <th>2000</th>
      <td>NaN</td>
      <td>1.5</td>
      <td>NaN</td>
      <td>0.5</td>
    </tr>
    <tr>
      <th>2001</th>
      <td>2.4</td>
      <td>1.7</td>
      <td>NaN</td>
      <td>1.1</td>
    </tr>
    <tr>
      <th>2002</th>
      <td>2.9</td>
      <td>NaN</td>
      <td>NaN</td>
      <td>2.1</td>
    </tr>
    <tr>
      <th>2003</th>
      <td>3.3</td>
      <td>3.6</td>
      <td>3.5</td>
      <td>1.5</td>
    </tr>
    <tr>
      <th>2004</th>
      <td>NaN</td>
      <td>NaN</td>
      <td>5.7</td>
      <td>0.9</td>
    </tr>
  </tbody>
</table>
</div>


```python
dataframe_nan.dropna(axis=1, thresh=3) #可以通过设置axis参数，设定删除操作是对行还是对列进行。thresh参数可以用来指定当某一行或者某一列的非空元素大于等于多少时保留
```


<div>
<style scoped>
    .dataframe tbody tr th:only-of-type {
        vertical-align: middle;
    }
    .dataframe tbody tr th {
        vertical-align: top;
    }
    .dataframe thead th {
        text-align: right;
    }
</style>
<table border="1" class="dataframe">
  <thead>
    <tr style="text-align: right;">
      <th></th>
      <th>Nevada</th>
      <th>Ohio</th>
      <th>Denver</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <th>2000</th>
      <td>NaN</td>
      <td>1.5</td>
      <td>0.5</td>
    </tr>
    <tr>
      <th>2001</th>
      <td>2.4</td>
      <td>1.7</td>
      <td>1.1</td>
    </tr>
    <tr>
      <th>2002</th>
      <td>2.9</td>
      <td>NaN</td>
      <td>2.1</td>
    </tr>
    <tr>
      <th>2003</th>
      <td>3.3</td>
      <td>3.6</td>
      <td>1.5</td>
    </tr>
    <tr>
      <th>2004</th>
      <td>NaN</td>
      <td>NaN</td>
      <td>0.9</td>
    </tr>
    <tr>
      <th>2005</th>
      <td>NaN</td>
      <td>NaN</td>
      <td>NaN</td>
    </tr>
  </tbody>
</table>
</div>
如果要删除指定列中包含NaN的行或列，可以传入subset参数来进行设定。例如：

```Python
dataframe_nan.dropna(subset=['Ohio'])
```

||Nevada|Ohio|New York|Denver|
|:-:|:-:|:-:|:-:|:-:|
|2000|NaN|1.5|NaN|0.5|
|2001|2.4|1.7|NaN|1.1|
|2003|3.3|3.6|3.5|1.5|



要对缺失的数据进行填补，可以使用`fillna`函数。这一函数支持传入不同类型的参数，从而进行不同方法的填补。函数的返回值是一个新的变量，不改变原变量，如果要对原变量直接修改可以传入`inplace=True`参数。


```python
dataframe_nan.fillna(dataframe_nan.mean()) #传入一个Series，每列中的缺失值会根据Series中对应的值来进行填补，如果在Series中找不到对应的值则仍为NaN。如果传入一个dict，也按照类似的策略进行填补。需要注意，这种方法不支持按行索引的填补方式。此处的示例为使用每列数据的平均值来进行填补。
```


<div>
<style scoped>
    .dataframe tbody tr th:only-of-type {
        vertical-align: middle;
    }
    .dataframe tbody tr th {
        vertical-align: top;
    }
    .dataframe thead th {
        text-align: right;
    }
</style>
<table border="1" class="dataframe">
  <thead>
    <tr style="text-align: right;">
      <th></th>
      <th>Nevada</th>
      <th>Ohio</th>
      <th>New York</th>
      <th>Denver</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <th>2000</th>
      <td>2.866667</td>
      <td>1.500000</td>
      <td>4.6</td>
      <td>0.50</td>
    </tr>
    <tr>
      <th>2001</th>
      <td>2.400000</td>
      <td>1.700000</td>
      <td>4.6</td>
      <td>1.10</td>
    </tr>
    <tr>
      <th>2002</th>
      <td>2.900000</td>
      <td>2.266667</td>
      <td>4.6</td>
      <td>2.10</td>
    </tr>
    <tr>
      <th>2003</th>
      <td>3.300000</td>
      <td>3.600000</td>
      <td>3.5</td>
      <td>1.50</td>
    </tr>
    <tr>
      <th>2004</th>
      <td>2.866667</td>
      <td>2.266667</td>
      <td>5.7</td>
      <td>0.90</td>
    </tr>
    <tr>
      <th>2005</th>
      <td>2.866667</td>
      <td>2.266667</td>
      <td>4.6</td>
      <td>1.22</td>
    </tr>
  </tbody>
</table>
</div>


```python
dataframe_nan.fillna(0) #将每个缺失值填补为传入的数值
```


<div>
<style scoped>
    .dataframe tbody tr th:only-of-type {
        vertical-align: middle;
    }
    .dataframe tbody tr th {
        vertical-align: top;
    }
    .dataframe thead th {
        text-align: right;
    }
</style>
<table border="1" class="dataframe">
  <thead>
    <tr style="text-align: right;">
      <th></th>
      <th>Nevada</th>
      <th>Ohio</th>
      <th>New York</th>
      <th>Denver</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <th>2000</th>
      <td>0.0</td>
      <td>1.5</td>
      <td>0.0</td>
      <td>0.5</td>
    </tr>
    <tr>
      <th>2001</th>
      <td>2.4</td>
      <td>1.7</td>
      <td>0.0</td>
      <td>1.1</td>
    </tr>
    <tr>
      <th>2002</th>
      <td>2.9</td>
      <td>0.0</td>
      <td>0.0</td>
      <td>2.1</td>
    </tr>
    <tr>
      <th>2003</th>
      <td>3.3</td>
      <td>3.6</td>
      <td>3.5</td>
      <td>1.5</td>
    </tr>
    <tr>
      <th>2004</th>
      <td>0.0</td>
      <td>0.0</td>
      <td>5.7</td>
      <td>0.9</td>
    </tr>
    <tr>
      <th>2005</th>
      <td>0.0</td>
      <td>0.0</td>
      <td>0.0</td>
      <td>0.0</td>
    </tr>
  </tbody>
</table>
</div>


```python
dataframe_nan.fillna(method='ffill',limit=1) #使用向上或向下复制的方式进行填补，limit参数规定了向上或向下复制的最大次数
```


<div>
<style scoped>
    .dataframe tbody tr th:only-of-type {
        vertical-align: middle;
    }
    .dataframe tbody tr th {
        vertical-align: top;
    }
    .dataframe thead th {
        text-align: right;
    }
</style>
<table border="1" class="dataframe">
  <thead>
    <tr style="text-align: right;">
      <th></th>
      <th>Nevada</th>
      <th>Ohio</th>
      <th>New York</th>
      <th>Denver</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <th>2000</th>
      <td>NaN</td>
      <td>1.5</td>
      <td>NaN</td>
      <td>0.5</td>
    </tr>
    <tr>
      <th>2001</th>
      <td>2.4</td>
      <td>1.7</td>
      <td>NaN</td>
      <td>1.1</td>
    </tr>
    <tr>
      <th>2002</th>
      <td>2.9</td>
      <td>1.7</td>
      <td>NaN</td>
      <td>2.1</td>
    </tr>
    <tr>
      <th>2003</th>
      <td>3.3</td>
      <td>3.6</td>
      <td>3.5</td>
      <td>1.5</td>
    </tr>
    <tr>
      <th>2004</th>
      <td>3.3</td>
      <td>3.6</td>
      <td>5.7</td>
      <td>0.9</td>
    </tr>
    <tr>
      <th>2005</th>
      <td>NaN</td>
      <td>NaN</td>
      <td>5.7</td>
      <td>0.9</td>
    </tr>
  </tbody>
</table>
</div>



## 除去重复数据

`drop_duplicates()`函数可以删除掉一个series或dataframe中的重复行。默认条件下，`drop_duplicates()`函数只会删除一行的所有元素都相同的多余行。如果想基于某些特定的行作为判断标准，需要传入一个以列名称构成的list作为参数。同时，默认条件下函数会保留第一个发现的行，如果想取最后一个发现的行，需要传入`keep='last'`参数。

下面是一些使用示例：


```python
dataframe_duplicate=pd.DataFrame({'k1': ['one'] * 3 + ['two'] * 4, 'k2': [1, 1, 2, 3, 3, 4, 4]})
```


```python
dataframe_duplicate
```


<div>
<style scoped>
    .dataframe tbody tr th:only-of-type {
        vertical-align: middle;
    }
    .dataframe tbody tr th {
        vertical-align: top;
    }
    .dataframe thead th {
        text-align: right;
    }
</style>
<table border="1" class="dataframe">
  <thead>
    <tr style="text-align: right;">
      <th></th>
      <th>k1</th>
      <th>k2</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <th>0</th>
      <td>one</td>
      <td>1</td>
    </tr>
    <tr>
      <th>1</th>
      <td>one</td>
      <td>1</td>
    </tr>
    <tr>
      <th>2</th>
      <td>one</td>
      <td>2</td>
    </tr>
    <tr>
      <th>3</th>
      <td>two</td>
      <td>3</td>
    </tr>
    <tr>
      <th>4</th>
      <td>two</td>
      <td>3</td>
    </tr>
    <tr>
      <th>5</th>
      <td>two</td>
      <td>4</td>
    </tr>
    <tr>
      <th>6</th>
      <td>two</td>
      <td>4</td>
    </tr>
  </tbody>
</table>
</div>


```python
dataframe_duplicate.duplicated() #生成一个布尔类型的Series，表示每行是否为重复行
```


    0    False
    1     True
    2    False
    3    False
    4     True
    5    False
    6     True
    dtype: bool




```python
dataframe_duplicate.drop_duplicates() #丢掉重复数据，默认保留第一个发现的行
```


<div>
<style scoped>
    .dataframe tbody tr th:only-of-type {
        vertical-align: middle;
    }
    .dataframe tbody tr th {
        vertical-align: top;
    }
    .dataframe thead th {
        text-align: right;
    }
</style>
<table border="1" class="dataframe">
  <thead>
    <tr style="text-align: right;">
      <th></th>
      <th>k1</th>
      <th>k2</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <th>0</th>
      <td>one</td>
      <td>1</td>
    </tr>
    <tr>
      <th>2</th>
      <td>one</td>
      <td>2</td>
    </tr>
    <tr>
      <th>3</th>
      <td>two</td>
      <td>3</td>
    </tr>
    <tr>
      <th>5</th>
      <td>two</td>
      <td>4</td>
    </tr>
  </tbody>
</table>
</div>


```python
dataframe_duplicate.drop_duplicates(['k1']) #以名为'k1'的列作为判断标准，只要k1这一列的数据重复便视作是重复行
```


<div>
<style scoped>
    .dataframe tbody tr th:only-of-type {
        vertical-align: middle;
    }
    .dataframe tbody tr th {
        vertical-align: top;
    }
    .dataframe thead th {
        text-align: right;
    }
</style>
<table border="1" class="dataframe">
  <thead>
    <tr style="text-align: right;">
      <th></th>
      <th>k1</th>
      <th>k2</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <th>0</th>
      <td>one</td>
      <td>1</td>
    </tr>
    <tr>
      <th>3</th>
      <td>two</td>
      <td>3</td>
    </tr>
  </tbody>
</table>
</div>


```python
dataframe_duplicate.drop_duplicates(['k1'],keep='last') #保留重复数据的最后一个
```


<div>
<style scoped>
    .dataframe tbody tr th:only-of-type {
        vertical-align: middle;
    }
    .dataframe tbody tr th {
        vertical-align: top;
    }
    .dataframe thead th {
        text-align: right;
    }
</style>
<table border="1" class="dataframe">
  <thead>
    <tr style="text-align: right;">
      <th></th>
      <th>k1</th>
      <th>k2</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <th>2</th>
      <td>one</td>
      <td>2</td>
    </tr>
    <tr>
      <th>6</th>
      <td>two</td>
      <td>4</td>
    </tr>
  </tbody>
</table>
</div>



## 函数与映射

`map()`函数支持传入一个函数或者dict变量，从而对一个Series中的所有数据做映射，得到转换后的数据。注意这一操作不能对DataFrame进行。例如：


```python
dataframe_map = pd.DataFrame({'food': ['bacon', 'pulled pork', 'bacon', 'Pastrami','corned beef', 'Bacon', 'pastrami', 'honey ham', 'nova lox'],'ounces': [4, 3, 12, 6, 7.5, 8, 3, 5, 6]})
meat_to_animal = {'bacon': 'pig','pulled pork': 'pig','pastrami': 'cow','corned beef': 'cow','honey ham': 'pig','nova lox': 'salmon'}
```


```python
dataframe_map
```


<div>
<style scoped>
    .dataframe tbody tr th:only-of-type {
        vertical-align: middle;
    }
    .dataframe tbody tr th {
        vertical-align: top;
    }
    .dataframe thead th {
        text-align: right;
    }
</style>
<table border="1" class="dataframe">
  <thead>
    <tr style="text-align: right;">
      <th></th>
      <th>food</th>
      <th>ounces</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <th>0</th>
      <td>bacon</td>
      <td>4.0</td>
    </tr>
    <tr>
      <th>1</th>
      <td>pulled pork</td>
      <td>3.0</td>
    </tr>
    <tr>
      <th>2</th>
      <td>bacon</td>
      <td>12.0</td>
    </tr>
    <tr>
      <th>3</th>
      <td>Pastrami</td>
      <td>6.0</td>
    </tr>
    <tr>
      <th>4</th>
      <td>corned beef</td>
      <td>7.5</td>
    </tr>
    <tr>
      <th>5</th>
      <td>Bacon</td>
      <td>8.0</td>
    </tr>
    <tr>
      <th>6</th>
      <td>pastrami</td>
      <td>3.0</td>
    </tr>
    <tr>
      <th>7</th>
      <td>honey ham</td>
      <td>5.0</td>
    </tr>
    <tr>
      <th>8</th>
      <td>nova lox</td>
      <td>6.0</td>
    </tr>
  </tbody>
</table>
</div>


```python
dataframe_map['animal']=dataframe_map['food'].map(str.lower).map(meat_to_animal) #先使用函数进行映射，再使用dict进行映射
```


```python
dataframe_map
```


<div>
<style scoped>
    .dataframe tbody tr th:only-of-type {
        vertical-align: middle;
    }
    .dataframe tbody tr th {
        vertical-align: top;
    }
    .dataframe thead th {
        text-align: right;
    }
</style>
<table border="1" class="dataframe">
  <thead>
    <tr style="text-align: right;">
      <th></th>
      <th>food</th>
      <th>ounces</th>
      <th>animal</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <th>0</th>
      <td>bacon</td>
      <td>4.0</td>
      <td>pig</td>
    </tr>
    <tr>
      <th>1</th>
      <td>pulled pork</td>
      <td>3.0</td>
      <td>pig</td>
    </tr>
    <tr>
      <th>2</th>
      <td>bacon</td>
      <td>12.0</td>
      <td>pig</td>
    </tr>
    <tr>
      <th>3</th>
      <td>Pastrami</td>
      <td>6.0</td>
      <td>cow</td>
    </tr>
    <tr>
      <th>4</th>
      <td>corned beef</td>
      <td>7.5</td>
      <td>cow</td>
    </tr>
    <tr>
      <th>5</th>
      <td>Bacon</td>
      <td>8.0</td>
      <td>pig</td>
    </tr>
    <tr>
      <th>6</th>
      <td>pastrami</td>
      <td>3.0</td>
      <td>cow</td>
    </tr>
    <tr>
      <th>7</th>
      <td>honey ham</td>
      <td>5.0</td>
      <td>pig</td>
    </tr>
    <tr>
      <th>8</th>
      <td>nova lox</td>
      <td>6.0</td>
      <td>salmon</td>
    </tr>
  </tbody>
</table>
</div>



## 数据替换

使用`replace`函数，可以对Series或者DataFrame中的某些值进行替换。`replace`函数可以传入两个参数，第一个代表需要替换的值，可以是一个值，也可以是一个list；第二个代表要替换成的值，可以是一个值，也可以是一个与第一个参数大小相同的list。也或者是只传入一个dict作为参数。例如：


```python
dataframe_replace=dataframe_map
```


```python
dataframe_replace
```


<div>
<style scoped>
    .dataframe tbody tr th:only-of-type {
        vertical-align: middle;
    }
    .dataframe tbody tr th {
        vertical-align: top;
    }
    .dataframe thead th {
        text-align: right;
    }
</style>
<table border="1" class="dataframe">
  <thead>
    <tr style="text-align: right;">
      <th></th>
      <th>food</th>
      <th>ounces</th>
      <th>animal</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <th>0</th>
      <td>bacon</td>
      <td>4.0</td>
      <td>pig</td>
    </tr>
    <tr>
      <th>1</th>
      <td>pulled pork</td>
      <td>3.0</td>
      <td>pig</td>
    </tr>
    <tr>
      <th>2</th>
      <td>bacon</td>
      <td>12.0</td>
      <td>pig</td>
    </tr>
    <tr>
      <th>3</th>
      <td>Pastrami</td>
      <td>6.0</td>
      <td>cow</td>
    </tr>
    <tr>
      <th>4</th>
      <td>corned beef</td>
      <td>7.5</td>
      <td>cow</td>
    </tr>
    <tr>
      <th>5</th>
      <td>Bacon</td>
      <td>8.0</td>
      <td>pig</td>
    </tr>
    <tr>
      <th>6</th>
      <td>pastrami</td>
      <td>3.0</td>
      <td>cow</td>
    </tr>
    <tr>
      <th>7</th>
      <td>honey ham</td>
      <td>5.0</td>
      <td>pig</td>
    </tr>
    <tr>
      <th>8</th>
      <td>nova lox</td>
      <td>6.0</td>
      <td>salmon</td>
    </tr>
  </tbody>
</table>
</div>


```python
dataframe_replace.replace('pig','PIG') #传入两个值，一个代表要替换的元素，另一个代表替换后的元素
```


<div>
<style scoped>
    .dataframe tbody tr th:only-of-type {
        vertical-align: middle;
    }
    .dataframe tbody tr th {
        vertical-align: top;
    }
    .dataframe thead th {
        text-align: right;
    }
</style>
<table border="1" class="dataframe">
  <thead>
    <tr style="text-align: right;">
      <th></th>
      <th>food</th>
      <th>ounces</th>
      <th>animal</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <th>0</th>
      <td>bacon</td>
      <td>4.0</td>
      <td>PIG</td>
    </tr>
    <tr>
      <th>1</th>
      <td>pulled pork</td>
      <td>3.0</td>
      <td>PIG</td>
    </tr>
    <tr>
      <th>2</th>
      <td>bacon</td>
      <td>12.0</td>
      <td>PIG</td>
    </tr>
    <tr>
      <th>3</th>
      <td>Pastrami</td>
      <td>6.0</td>
      <td>cow</td>
    </tr>
    <tr>
      <th>4</th>
      <td>corned beef</td>
      <td>7.5</td>
      <td>cow</td>
    </tr>
    <tr>
      <th>5</th>
      <td>Bacon</td>
      <td>8.0</td>
      <td>PIG</td>
    </tr>
    <tr>
      <th>6</th>
      <td>pastrami</td>
      <td>3.0</td>
      <td>cow</td>
    </tr>
    <tr>
      <th>7</th>
      <td>honey ham</td>
      <td>5.0</td>
      <td>PIG</td>
    </tr>
    <tr>
      <th>8</th>
      <td>nova lox</td>
      <td>6.0</td>
      <td>salmon</td>
    </tr>
  </tbody>
</table>
</div>


```python
dataframe_replace.replace(['pig','bacon'],[np.nan,'Bacon']) #传入两个list，按照list中的顺序对应起来进行替换。注意NaN可以参与到替换操作中去
```


<div>
<style scoped>
    .dataframe tbody tr th:only-of-type {
        vertical-align: middle;
    }
    .dataframe tbody tr th {
        vertical-align: top;
    }
    .dataframe thead th {
        text-align: right;
    }
</style>
<table border="1" class="dataframe">
  <thead>
    <tr style="text-align: right;">
      <th></th>
      <th>food</th>
      <th>ounces</th>
      <th>animal</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <th>0</th>
      <td>Bacon</td>
      <td>4.0</td>
      <td>NaN</td>
    </tr>
    <tr>
      <th>1</th>
      <td>pulled pork</td>
      <td>3.0</td>
      <td>NaN</td>
    </tr>
    <tr>
      <th>2</th>
      <td>Bacon</td>
      <td>12.0</td>
      <td>NaN</td>
    </tr>
    <tr>
      <th>3</th>
      <td>Pastrami</td>
      <td>6.0</td>
      <td>cow</td>
    </tr>
    <tr>
      <th>4</th>
      <td>corned beef</td>
      <td>7.5</td>
      <td>cow</td>
    </tr>
    <tr>
      <th>5</th>
      <td>Bacon</td>
      <td>8.0</td>
      <td>NaN</td>
    </tr>
    <tr>
      <th>6</th>
      <td>pastrami</td>
      <td>3.0</td>
      <td>cow</td>
    </tr>
    <tr>
      <th>7</th>
      <td>honey ham</td>
      <td>5.0</td>
      <td>NaN</td>
    </tr>
    <tr>
      <th>8</th>
      <td>nova lox</td>
      <td>6.0</td>
      <td>salmon</td>
    </tr>
  </tbody>
</table>
</div>


```python
dataframe_replace.replace({4.0:-4.0,7.5:-7.5}) #传入一个dict进行替换，key作为要替换的值，用对应的value作为替换后的值
```


<div>
<style scoped>
    .dataframe tbody tr th:only-of-type {
        vertical-align: middle;
    }
    .dataframe tbody tr th {
        vertical-align: top;
    }
    .dataframe thead th {
        text-align: right;
    }
</style>
<table border="1" class="dataframe">
  <thead>
    <tr style="text-align: right;">
      <th></th>
      <th>food</th>
      <th>ounces</th>
      <th>animal</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <th>0</th>
      <td>bacon</td>
      <td>-4.0</td>
      <td>pig</td>
    </tr>
    <tr>
      <th>1</th>
      <td>pulled pork</td>
      <td>3.0</td>
      <td>pig</td>
    </tr>
    <tr>
      <th>2</th>
      <td>bacon</td>
      <td>12.0</td>
      <td>pig</td>
    </tr>
    <tr>
      <th>3</th>
      <td>Pastrami</td>
      <td>6.0</td>
      <td>cow</td>
    </tr>
    <tr>
      <th>4</th>
      <td>corned beef</td>
      <td>-7.5</td>
      <td>cow</td>
    </tr>
    <tr>
      <th>5</th>
      <td>Bacon</td>
      <td>8.0</td>
      <td>pig</td>
    </tr>
    <tr>
      <th>6</th>
      <td>pastrami</td>
      <td>3.0</td>
      <td>cow</td>
    </tr>
    <tr>
      <th>7</th>
      <td>honey ham</td>
      <td>5.0</td>
      <td>pig</td>
    </tr>
    <tr>
      <th>8</th>
      <td>nova lox</td>
      <td>6.0</td>
      <td>salmon</td>
    </tr>
  </tbody>
</table>
</div>



## 频数统计

Pandas中的`cut()`函数可以对一个list中的数据按照给定的区间进行分类，从而进一步统计它们的频数。下面为一些使用示例：


```python
ages=[20, 22, 25, 27, 21, 23, 37, 31, 61, 45, 41, 32] 
bins=[18,25,35,60,100]
cats=pd.cut(ages,bins)
```


```python
cats #cut操作得到的变量类型为categorical，包含两个属性。其中一个是labels，为一个list，内容为元素对应的区间编号；另一个是levels，它是一个list，内容为区间。默认条件下形成的区间为左开右闭区间。
```


    [(18, 25], (18, 25], (18, 25], (25, 35], (18, 25], ..., (25, 35], (60, 100], (35, 60], (35, 60], (25, 35]]
    Length: 12
    Categories (4, interval[int64]): [(18, 25] < (25, 35] < (35, 60] < (60, 100]]




```python
pd.value_counts(cats) #借助value_counts函数，可以将categorical类型的变量转化为一个series，series的内容是区间与频数的对应关系
```


    (18, 25]     5
    (35, 60]     3
    (25, 35]     3
    (60, 100]    1
    dtype: int64




```python
pd.cut(ages,bins,right=False) #可以传入right=False参数，得到左闭右开区间
```


    [[18, 25), [18, 25), [25, 35), [25, 35), [18, 25), ..., [25, 35), [60, 100), [35, 60), [35, 60), [25, 35)]
    Length: 12
    Categories (4, interval[int64]): [[18, 25) < [25, 35) < [35, 60) < [60, 100)]




```python
group_names = ['Youth', 'YoungAdult', 'MiddleAged', 'Senior']
pd.cut(ages,bins,labels=group_names) #可以使用labels参数，这个参数接收一个list，list的大小与区间数相等，其中的内容为区间的名称。这样在输出时，区间会被自动替换为对应的名称
```


    [Youth, Youth, Youth, YoungAdult, Youth, ..., YoungAdult, Senior, MiddleAged, MiddleAged, YoungAdult]
    Length: 12
    Categories (4, object): [Youth < YoungAdult < MiddleAged < Senior]




```python
pd.cut(ages,4,precision=2) #可以传入一个数字，来表示将区间均匀地分成若干等份。函数会自动根据最大和最小值算出区间大小。同时可以传入precision参数表示计算区间大小时的小数点位数。
```


    [(19.96, 30.25], (19.96, 30.25], (19.96, 30.25], (19.96, 30.25], (19.96, 30.25], ..., (30.25, 40.5], (50.75, 61.0], (40.5, 50.75], (40.5, 50.75], (30.25, 40.5]]
    Length: 12
    Categories (4, interval[float64]): [(19.96, 30.25] < (30.25, 40.5] < (40.5, 50.75] < (50.75, 61.0]]



另一个功能相近的函数为`qcut()`。这个函数的用法有两种：第一种是传入一个整数，代表将所有元素均分为若干份，然后根据均分结果得到区间范围；第二种是传入一个list，list中的元素为0到1递增的数字，每个数字代表在该分位数处作为一个区间的起点/终点。用法如下：


```python
data_qcut=np.random.randn(1000)
```


```python
pd.qcut(data_qcut,5) #传入一个数字，表示将所有元素均分成若干个区间
```


    [(0.787, 2.964], (-3.3779999999999997, -0.802], (-0.802, -0.226], (-0.802, -0.226], (-3.3779999999999997, -0.802], ..., (-0.802, -0.226], (-0.226, 0.236], (-0.226, 0.236], (0.236, 0.787], (-0.802, -0.226]]
    Length: 1000
    Categories (5, interval[float64]): [(-3.3779999999999997, -0.802] < (-0.802, -0.226] < (-0.226, 0.236] < (0.236, 0.787] < (0.787, 2.964]]




```python
pd.value_counts(pd.qcut(data_qcut,5))
```


    (0.787, 2.964]                   200
    (0.236, 0.787]                   200
    (-0.226, 0.236]                  200
    (-0.802, -0.226]                 200
    (-3.3779999999999997, -0.802]    200
    dtype: int64




```python
pd.qcut(data_qcut,[0,0.1,0.5,0.9,1]) #传入list，使用list中的值作为分位数来划分区间
```


    [(1.267, 2.964], (-1.179, -0.0053], (-1.179, -0.0053], (-1.179, -0.0053], (-1.179, -0.0053], ..., (-1.179, -0.0053], (-1.179, -0.0053], (-0.0053, 1.267], (-0.0053, 1.267], (-1.179, -0.0053]]
    Length: 1000
    Categories (4, interval[float64]): [(-3.3779999999999997, -1.179] < (-1.179, -0.0053] < (-0.0053, 1.267] < (1.267, 2.964]]




```python
pd.value_counts(pd.qcut(data_qcut,[0,0.1,0.5,0.9,1]))
```


    (-0.0053, 1.267]                 400
    (-1.179, -0.0053]                400
    (1.267, 2.964]                   100
    (-3.3779999999999997, -1.179]    100
    dtype: int64



## Dummy变量

在Series或DataFrame中，某些列可能为类别型的变量。而在机器学习等任务中，有时可能需要将其转换为0和1编码的二值变量。Pandas的`get_dummies()`函数便可以完成这一功能。

`get_dummies`需要将一个Series或者DataFrame作为输入。假设某一列中的数据出现了k种不重复的元素，在这一操作之后会生成出k列。其中每一列都代表k种类别中的某一个，全部用0或1填充，某一列为1则代表这个类别是原始列中这一行的数据。例如：


```python
dataframe_dummy=pd.DataFrame({'key': ['b', 'b', 'a', 'c', 'a', 'b'],'data1': range(6),'key2': ['b', 'b', 'a', 'c', 'a', 'd']})
```


```python
dataframe_dummy
```


<div>
<style scoped>
    .dataframe tbody tr th:only-of-type {
        vertical-align: middle;
    }
    .dataframe tbody tr th {
        vertical-align: top;
    }
    .dataframe thead th {
        text-align: right;
    }
</style>
<table border="1" class="dataframe">
  <thead>
    <tr style="text-align: right;">
      <th></th>
      <th>key</th>
      <th>data1</th>
      <th>key2</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <th>0</th>
      <td>b</td>
      <td>0</td>
      <td>b</td>
    </tr>
    <tr>
      <th>1</th>
      <td>b</td>
      <td>1</td>
      <td>b</td>
    </tr>
    <tr>
      <th>2</th>
      <td>a</td>
      <td>2</td>
      <td>a</td>
    </tr>
    <tr>
      <th>3</th>
      <td>c</td>
      <td>3</td>
      <td>c</td>
    </tr>
    <tr>
      <th>4</th>
      <td>a</td>
      <td>4</td>
      <td>a</td>
    </tr>
    <tr>
      <th>5</th>
      <td>b</td>
      <td>5</td>
      <td>d</td>
    </tr>
  </tbody>
</table>
</div>


```python
pd.get_dummies(dataframe_dummy) #传入DataFrame时，这一函数会自动将非数字型的列全部转换为dummy变量，同时自动为这些列构造列名。
```


<div>
<style scoped>
    .dataframe tbody tr th:only-of-type {
        vertical-align: middle;
    }
    .dataframe tbody tr th {
        vertical-align: top;
    }
    .dataframe thead th {
        text-align: right;
    }
</style>
<table border="1" class="dataframe">
  <thead>
    <tr style="text-align: right;">
      <th></th>
      <th>data1</th>
      <th>key_a</th>
      <th>key_b</th>
      <th>key_c</th>
      <th>key2_a</th>
      <th>key2_b</th>
      <th>key2_c</th>
      <th>key2_d</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <th>0</th>
      <td>0</td>
      <td>0</td>
      <td>1</td>
      <td>0</td>
      <td>0</td>
      <td>1</td>
      <td>0</td>
      <td>0</td>
    </tr>
    <tr>
      <th>1</th>
      <td>1</td>
      <td>0</td>
      <td>1</td>
      <td>0</td>
      <td>0</td>
      <td>1</td>
      <td>0</td>
      <td>0</td>
    </tr>
    <tr>
      <th>2</th>
      <td>2</td>
      <td>1</td>
      <td>0</td>
      <td>0</td>
      <td>1</td>
      <td>0</td>
      <td>0</td>
      <td>0</td>
    </tr>
    <tr>
      <th>3</th>
      <td>3</td>
      <td>0</td>
      <td>0</td>
      <td>1</td>
      <td>0</td>
      <td>0</td>
      <td>1</td>
      <td>0</td>
    </tr>
    <tr>
      <th>4</th>
      <td>4</td>
      <td>1</td>
      <td>0</td>
      <td>0</td>
      <td>1</td>
      <td>0</td>
      <td>0</td>
      <td>0</td>
    </tr>
    <tr>
      <th>5</th>
      <td>5</td>
      <td>0</td>
      <td>1</td>
      <td>0</td>
      <td>0</td>
      <td>0</td>
      <td>0</td>
      <td>1</td>
    </tr>
  </tbody>
</table>
</div>


```python
pd.get_dummies(dataframe_dummy['key']) #如果传入的是一个Series，由于Series本身没有列名，因此默认会使用类别型变量自身的名称作为列名。
```


<div>
<style scoped>
    .dataframe tbody tr th:only-of-type {
        vertical-align: middle;
    }
    .dataframe tbody tr th {
        vertical-align: top;
    }
    .dataframe thead th {
        text-align: right;
    }
</style>
<table border="1" class="dataframe">
  <thead>
    <tr style="text-align: right;">
      <th></th>
      <th>a</th>
      <th>b</th>
      <th>c</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <th>0</th>
      <td>0</td>
      <td>1</td>
      <td>0</td>
    </tr>
    <tr>
      <th>1</th>
      <td>0</td>
      <td>1</td>
      <td>0</td>
    </tr>
    <tr>
      <th>2</th>
      <td>1</td>
      <td>0</td>
      <td>0</td>
    </tr>
    <tr>
      <th>3</th>
      <td>0</td>
      <td>0</td>
      <td>1</td>
    </tr>
    <tr>
      <th>4</th>
      <td>1</td>
      <td>0</td>
      <td>0</td>
    </tr>
    <tr>
      <th>5</th>
      <td>0</td>
      <td>1</td>
      <td>0</td>
    </tr>
  </tbody>
</table>
</div>


```python
pd.get_dummies(dataframe_dummy['key'],prefix='Key')#可以手动传入prefix参数，作为列名的前缀。
```


<div>
<style scoped>
    .dataframe tbody tr th:only-of-type {
        vertical-align: middle;
    }
    .dataframe tbody tr th {
        vertical-align: top;
    }
    .dataframe thead th {
        text-align: right;
    }
</style>
<table border="1" class="dataframe">
  <thead>
    <tr style="text-align: right;">
      <th></th>
      <th>Key_a</th>
      <th>Key_b</th>
      <th>Key_c</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <th>0</th>
      <td>0</td>
      <td>1</td>
      <td>0</td>
    </tr>
    <tr>
      <th>1</th>
      <td>0</td>
      <td>1</td>
      <td>0</td>
    </tr>
    <tr>
      <th>2</th>
      <td>1</td>
      <td>0</td>
      <td>0</td>
    </tr>
    <tr>
      <th>3</th>
      <td>0</td>
      <td>0</td>
      <td>1</td>
    </tr>
    <tr>
      <th>4</th>
      <td>1</td>
      <td>0</td>
      <td>0</td>
    </tr>
    <tr>
      <th>5</th>
      <td>0</td>
      <td>1</td>
      <td>0</td>
    </tr>
  </tbody>
</table>
</div>
需要注意的是，Series中的整数会自动按照这种方式编码，但是DataFrame中的整数不会。




## 字符串的处理

Pandas中提供了一些矢量化的字符串函数，可以用来批量处理Series或者DataFrame中的字符串。包括：

- cat：将字符串拼接起来，并可以使用分隔符
- contains：判断字符串是否含有特定的子串
- count：判断特定的子串出现的次数
- endswith/startswith：是否以某个特定的子串作为开头/结尾
- findall：找到子串或者正则表达式出现过的所有地方
- get：对字符串做索引
- join：使用分隔符将字符串拼接起来
- len：计算字符串长度
- lower/upper：转换为大写/小写
- match：使用正则表达式进行匹配，找到匹配的部分
- pad：在字符串的左侧/右侧/双侧(等价于center函数)添加空格
- repeat：将字符串重复若干次
- replace：将某个子串或者正则表达式匹配到的部分替换为其他字符串
- slice：将字符串进行分割
- split：根据某个分隔符或者正则表达式对字符串进行分割
- strip/rstrip/lstrip：切掉空格，然后替换成换行

这些用法与Python自带的字符串处理函数或是正则表达式处理函数类似，此处不再赘述。

# 多层index

Pandas中实现了一种叫做MultiIndex的类，允许Series和DataFrame有多层的index。

下面是Series中使用多层index的一些相关操作：


```python
series_multiIndex=pd.Series(np.random.randn(10),index=[['a', 'a', 'a', 'b', 'b', 'b', 'c', 'c', 'd', 'd'],[1, 2, 3, 1, 2, 3, 1, 2, 2, 3]]) #创建一个具有多重index的Series
```


```python
series_multiIndex
```


    a  1    2.862886
       2    1.973711
       3    1.367350
    b  1    1.245817
       2    1.270334
       3    0.872359
    c  1    0.651468
       2   -0.454727
    d  2    1.249058
       3    0.591254
    dtype: float64




```python
series_multiIndex.index #此时，访问其index属性，则会得到一个MultiIndex
```


    MultiIndex([('a', 1),
                ('a', 2),
                ('a', 3),
                ('b', 1),
                ('b', 2),
                ('b', 3),
                ('c', 1),
                ('c', 2),
                ('d', 2),
                ('d', 3)],
               )



MultiIndex可以通过传入嵌套数组来创建：


```python
pd.MultiIndex.from_arrays([['Ohio', 'Ohio', 'Colorado'], ['Green', 'Red', 'Green']],names=['state','color']) #多层index中的每一层都可以有一个名字
```


    MultiIndex([(    'Ohio', 'Green'),
                (    'Ohio',   'Red'),
                ('Colorado', 'Green')],
               names=['state', 'color'])



对具有多重index的Series或者DataFrame，在做索引操作时需要同时传入内外层的索引元素。因此，这与单层index的索引操作有些不同，下面以例子进行说明：


```python
series_multiIndex['b'] #如果仅使用外层的index进行索引，则可以省略掉内层的index
```


    1    1.245817
    2    1.270334
    3    0.872359
    dtype: float64




```python
series_multiIndex[('b',1)] #同时使用两层index进行索引，等同于语句series_multiIndex['b',1]
```


    1.2458169817277376




```python
series_multiIndex[['b','d']] #在传入list进行索引时，就不能再使用内层的索引。如series_multiIndex[['b','d'],[2,3]]便为错误操作
```


    b  1    1.245817
       2    1.270334
       3    0.872359
    d  2    1.249058
       3    0.591254
    dtype: float64




```python
series_multiIndex[:,1] #仅使用内层进行索引，需要用:来指明使用外层的全部索引。如果不指定外层，直接用内层的index进行索引就会报错。但是需要注意此时内层的索引不能使用list，例如series_multiIndex[:,[1,2]]的操作便是错误的。
```


    a    2.862886
    b    1.245817
    c    0.651468
    dtype: float64



注意：在MultiIndex中使用整数序号进行索引时会比较容易混淆。其基本规则可以大致描述为，多重index在使用整数索引时，会优先匹配index中是否包含这个整数，如果没有则将其作为序号进行索引。下面进行详细的说明：


```python
series_multiIndex[1] #在本例中，这一索引语句是按照元素的排列顺序来索引。
```


    1.973710915404737




```python
series_multiIndex2=pd.Series(np.random.randn(10),index=[[1, 2, 3, 1, 2, 3, 1, 2, 2, 3],['a', 'a', 'a', 'b', 'b', 'b', 'c', 'c', 'd', 'd']]) 
series_multiIndex2[1] #此时最外层的index为整数，因此在索引时是使用外层index进行索引的
```


    a   -0.041453
    b   -0.329131
    c   -0.776732
    dtype: float64




```python
series_multiIndex2[4] #由于外层index中不包含4，因此按照元素的排列顺序，索引第5个元素。但是在单层的index中，如果使用不包含在index中的整数进行索引则会报错。
```


    0.7369307651244501



对于使用两层index的Series来说，如果要解除多层index，可以使用`unstack()`函数；它相应的反操作为`stack()`。`stack`操作的实质是将某个columns“旋转”至index，而`unstack`操作则是将某个index“旋转”至columns。

此处有一些简单的示例，在下面的内容中还会对这一操作进行进一步的分析。


```python
series_multiIndex.unstack() #对一个Series做unstack操作，会得到一个DataFrame。默认对最里层做unstack操作。
```


<div>
<style scoped>
    .dataframe tbody tr th:only-of-type {
        vertical-align: middle;
    }
    .dataframe tbody tr th {
        vertical-align: top;
    }
    .dataframe thead th {
        text-align: right;
    }
</style>
<table border="1" class="dataframe">
  <thead>
    <tr style="text-align: right;">
      <th></th>
      <th>1</th>
      <th>2</th>
      <th>3</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <th>a</th>
      <td>2.862886</td>
      <td>1.973711</td>
      <td>1.367350</td>
    </tr>
    <tr>
      <th>b</th>
      <td>1.245817</td>
      <td>1.270334</td>
      <td>0.872359</td>
    </tr>
    <tr>
      <th>c</th>
      <td>0.651468</td>
      <td>-0.454727</td>
      <td>NaN</td>
    </tr>
    <tr>
      <th>d</th>
      <td>NaN</td>
      <td>1.249058</td>
      <td>0.591254</td>
    </tr>
  </tbody>
</table>
</div>


```python
series_multiIndex.unstack(level=0) #如果想改变unstack操作使用的index，可以传入level参数指定使用哪一层的index
```


<div>
<style scoped>
    .dataframe tbody tr th:only-of-type {
        vertical-align: middle;
    }
    .dataframe tbody tr th {
        vertical-align: top;
    }
    .dataframe thead th {
        text-align: right;
    }
</style>
<table border="1" class="dataframe">
  <thead>
    <tr style="text-align: right;">
      <th></th>
      <th>a</th>
      <th>b</th>
      <th>c</th>
      <th>d</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <th>1</th>
      <td>2.862886</td>
      <td>1.245817</td>
      <td>0.651468</td>
      <td>NaN</td>
    </tr>
    <tr>
      <th>2</th>
      <td>1.973711</td>
      <td>1.270334</td>
      <td>-0.454727</td>
      <td>1.249058</td>
    </tr>
    <tr>
      <th>3</th>
      <td>1.367350</td>
      <td>0.872359</td>
      <td>NaN</td>
      <td>0.591254</td>
    </tr>
  </tbody>
</table>
</div>


```python
series_multiIndex.unstack(level=0).stack() #与上面的结果对比，可以看出stack()操作默认是将columns“旋转”到index的最里层。unstack()操作也是类似的规则，此处不再用程序展示。如果有多层的index，也可以传入level参数设置传入到哪一层的下面。
```


    1  a    2.862886
       b    1.245817
       c    0.651468
    2  a    1.973711
       b    1.270334
       c   -0.454727
       d    1.249058
    3  a    1.367350
       b    0.872359
       d    0.591254
    dtype: float64



如果要改变层级顺序，可以使用swaplevel函数，在交换前后，Series中的元素顺序保持不变：


```python
series_multiIndex.swaplevel(0,1) #需要传入两个整数指定要交换的两层。如果每一层有自己的名字，也可以传入名字来指定要交换的层
```


    1  a    2.862886
    2  a    1.973711
    3  a    1.367350
    1  b    1.245817
    2  b    1.270334
    3  b    0.872359
    1  c    0.651468
    2  c   -0.454727
       d    1.249058
    3  d    0.591254
    dtype: float64



在部分统计函数中，可以添加level参数，这样便可以根据指定的level做统计：


```python
series_multiIndex.sum(level=1) 
```


    1    4.760171
    2    4.038376
    3    2.830964
    dtype: float64



如果要删除多重index中的某一层，可以调用`droplevel()`函数。

```python
series_multiIndex.droplevel(0)
```

```
1    1.210290
2    1.367135
3   -0.072713
1   -0.122782
2   -0.359306
3   -0.515809
1    0.420951
2   -0.667417
2   -1.967170
3    0.193437
dtype: float64
```



对于DataFrame来说，index和columns都可以使用MultiIndex。下面为一些使用示例：


```python
dataframe_multiIndex= pd.DataFrame(np.arange(12).reshape((4, 3)),index=[['a', 'a', 'b', 'b'], [1, 2, 1, 2]],columns=[['Ohio', 'Ohio', 'Colorado'],['Green', 'Red', 'Green']])
```


```python
dataframe_multiIndex
```


<div>
<style scoped>
    .dataframe tbody tr th:only-of-type {
        vertical-align: middle;
    }
    .dataframe tbody tr th {
        vertical-align: top;
    }
    .dataframe thead tr th {
        text-align: left;
    }
</style>
<table border="1" class="dataframe">
  <thead>
    <tr>
      <th></th>
      <th></th>
      <th colspan="2" halign="left">Ohio</th>
      <th>Colorado</th>
    </tr>
    <tr>
      <th></th>
      <th></th>
      <th>Green</th>
      <th>Red</th>
      <th>Green</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <th rowspan="2" valign="top">a</th>
      <th>1</th>
      <td>0</td>
      <td>1</td>
      <td>2</td>
    </tr>
    <tr>
      <th>2</th>
      <td>3</td>
      <td>4</td>
      <td>5</td>
    </tr>
    <tr>
      <th rowspan="2" valign="top">b</th>
      <th>1</th>
      <td>6</td>
      <td>7</td>
      <td>8</td>
    </tr>
    <tr>
      <th>2</th>
      <td>9</td>
      <td>10</td>
      <td>11</td>
    </tr>
  </tbody>
</table>
</div>



在DataFrame中，可以使用`set_index`函数指定某个或某些列来作为index；它的反操作是`reset_index`，可以将某个或某些index转化为列插入进DataFrame中去。下面是一些实例:


```python
dataframe_multiIndex2=pd.DataFrame({'a': range(7), 'b': range(7, 0, -1),'c': ['one', 'one', 'one', 'two', 'two', 'two', 'two'],'d': [0, 1, 2, 0, 1, 2, 3]},index=['a','b','c','d','e','f','g'])
```


```python
dataframe_multiIndex2
```


<div>
<style scoped>
    .dataframe tbody tr th:only-of-type {
        vertical-align: middle;
    }
    .dataframe tbody tr th {
        vertical-align: top;
    }
    .dataframe thead th {
        text-align: right;
    }
</style>
<table border="1" class="dataframe">
  <thead>
    <tr style="text-align: right;">
      <th></th>
      <th>a</th>
      <th>b</th>
      <th>c</th>
      <th>d</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <th>a</th>
      <td>0</td>
      <td>7</td>
      <td>one</td>
      <td>0</td>
    </tr>
    <tr>
      <th>b</th>
      <td>1</td>
      <td>6</td>
      <td>one</td>
      <td>1</td>
    </tr>
    <tr>
      <th>c</th>
      <td>2</td>
      <td>5</td>
      <td>one</td>
      <td>2</td>
    </tr>
    <tr>
      <th>d</th>
      <td>3</td>
      <td>4</td>
      <td>two</td>
      <td>0</td>
    </tr>
    <tr>
      <th>e</th>
      <td>4</td>
      <td>3</td>
      <td>two</td>
      <td>1</td>
    </tr>
    <tr>
      <th>f</th>
      <td>5</td>
      <td>2</td>
      <td>two</td>
      <td>2</td>
    </tr>
    <tr>
      <th>g</th>
      <td>6</td>
      <td>1</td>
      <td>two</td>
      <td>3</td>
    </tr>
  </tbody>
</table>
</div>


```python
dataframe_multiIndex2.set_index(['c','d']) #默认情况下，选作index的列在生成的新DataFrame中不会显示。如果传入一个list，那么多层index的层级顺序是按照list中的元素顺序从外到里。同时，这些列之前的列名也会自动被转化为index对应的名称。注意：这一操作会覆盖掉之前的index。
```


<div>
<style scoped>
    .dataframe tbody tr th:only-of-type {
        vertical-align: middle;
    }
    .dataframe tbody tr th {
        vertical-align: top;
    }
    .dataframe thead th {
        text-align: right;
    }
</style>
<table border="1" class="dataframe">
  <thead>
    <tr style="text-align: right;">
      <th></th>
      <th></th>
      <th>a</th>
      <th>b</th>
    </tr>
    <tr>
      <th>c</th>
      <th>d</th>
      <th></th>
      <th></th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <th rowspan="3" valign="top">one</th>
      <th>0</th>
      <td>0</td>
      <td>7</td>
    </tr>
    <tr>
      <th>1</th>
      <td>1</td>
      <td>6</td>
    </tr>
    <tr>
      <th>2</th>
      <td>2</td>
      <td>5</td>
    </tr>
    <tr>
      <th rowspan="4" valign="top">two</th>
      <th>0</th>
      <td>3</td>
      <td>4</td>
    </tr>
    <tr>
      <th>1</th>
      <td>4</td>
      <td>3</td>
    </tr>
    <tr>
      <th>2</th>
      <td>5</td>
      <td>2</td>
    </tr>
    <tr>
      <th>3</th>
      <td>6</td>
      <td>1</td>
    </tr>
  </tbody>
</table>
</div>


```python
dataframe_multiIndex2.set_index(['d','c'],drop=False) #传入drop=False，变为index的两列便不会被删除。
```


<div>
<style scoped>
    .dataframe tbody tr th:only-of-type {
        vertical-align: middle;
    }
    .dataframe tbody tr th {
        vertical-align: top;
    }
    .dataframe thead th {
        text-align: right;
    }
</style>
<table border="1" class="dataframe">
  <thead>
    <tr style="text-align: right;">
      <th></th>
      <th></th>
      <th>a</th>
      <th>b</th>
      <th>c</th>
      <th>d</th>
    </tr>
    <tr>
      <th>d</th>
      <th>c</th>
      <th></th>
      <th></th>
      <th></th>
      <th></th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <th>0</th>
      <th>one</th>
      <td>0</td>
      <td>7</td>
      <td>one</td>
      <td>0</td>
    </tr>
    <tr>
      <th>1</th>
      <th>one</th>
      <td>1</td>
      <td>6</td>
      <td>one</td>
      <td>1</td>
    </tr>
    <tr>
      <th>2</th>
      <th>one</th>
      <td>2</td>
      <td>5</td>
      <td>one</td>
      <td>2</td>
    </tr>
    <tr>
      <th>0</th>
      <th>two</th>
      <td>3</td>
      <td>4</td>
      <td>two</td>
      <td>0</td>
    </tr>
    <tr>
      <th>1</th>
      <th>two</th>
      <td>4</td>
      <td>3</td>
      <td>two</td>
      <td>1</td>
    </tr>
    <tr>
      <th>2</th>
      <th>two</th>
      <td>5</td>
      <td>2</td>
      <td>two</td>
      <td>2</td>
    </tr>
    <tr>
      <th>3</th>
      <th>two</th>
      <td>6</td>
      <td>1</td>
      <td>two</td>
      <td>3</td>
    </tr>
  </tbody>
</table>
</div>


```python
dataframe_multiIndex.reset_index([0,1]) #将index变为列插入到DataFrame中去。如果之前的index有名字，那么这个名字会变为列名，否则会为生成的列自动生成一个名字。需要注意的是，如果原来的columns具有多层，那么新转换得到的列名位于最外层，且它的内层为空。
```


<div>
<style scoped>
    .dataframe tbody tr th:only-of-type {
        vertical-align: middle;
    }
    .dataframe tbody tr th {
        vertical-align: top;
    }
    .dataframe thead tr th {
        text-align: left;
    }
</style>
<table border="1" class="dataframe">
  <thead>
    <tr>
      <th></th>
      <th>level_0</th>
      <th>level_1</th>
      <th colspan="2" halign="left">Ohio</th>
      <th>Colorado</th>
    </tr>
    <tr>
      <th></th>
      <th></th>
      <th></th>
      <th>Green</th>
      <th>Red</th>
      <th>Green</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <th>0</th>
      <td>a</td>
      <td>1</td>
      <td>0</td>
      <td>1</td>
      <td>2</td>
    </tr>
    <tr>
      <th>1</th>
      <td>a</td>
      <td>2</td>
      <td>3</td>
      <td>4</td>
      <td>5</td>
    </tr>
    <tr>
      <th>2</th>
      <td>b</td>
      <td>1</td>
      <td>6</td>
      <td>7</td>
      <td>8</td>
    </tr>
    <tr>
      <th>3</th>
      <td>b</td>
      <td>2</td>
      <td>9</td>
      <td>10</td>
      <td>11</td>
    </tr>
  </tbody>
</table>
</div>



# 数据的重构与组合

## 按列合并

merge函数可以将两个DataFrame按列合并起来，这一函数要求在两个DataFrame中各自选择一列，然后按照这一列中元素的对应关系，将两个DataFrame合并为一个。默认情况下，函数会自己选择名称相同的列。

默认情况下，只有当某个元素在两个DataFrame里面选中的列内都出现时（即交集），最终合并得到的DataFrame中才会包含有相应元素；只出现在其中一个DataFrame中的元素会被舍去。

`merge()`函数支持传入下面的可选参数：

- left：以左侧的DataFrame为基准进行合并
- right：以右侧的DataFrame为基准进行合并
- how：可以选择'inner'，'outer'，'left'和'right'四种模式。四种模式的意义如下：
  - inner：左右两侧元素保留交集部分（默认操作）
  - outer：左右两侧元素全部保留
  - left：保留仅出现在左边的元素，舍去仅出现在右边的元素
  - right：保留仅出现在右边的元素，舍去仅出现在左边的元素
- on：指定参与合并操作的列名
- left_on：指定左侧DataFrame中参与合并操作的列名
- right_on：指定右侧DataFrame中参与合并操作的列名
- left_index：是否使用左侧DataFrame的index参与合并，默认为False
- right_index：是否使用右侧DataFrame的index参与合并，默认为False
- sort：是否按照参与合并的元素对DataFrame进行排序，默认为True。如果要处理大表格时可以使用False来加快速度
- suffixes：传入一个二元的tuple用于区分数据来自于哪一个DataFrame，当出现重名的columns时则会自动在这些columns后面加上后缀。默认值为`('_x'和'_y')`
- copy：是否生成原始数据的副本，默认为True

下面为一些使用示例：


```python
dataframe_merge1=pd.DataFrame({'value': ['b', 'b', 'a', 'c', 'a', 'a', 'b'],'data1': range(7)})
dataframe_merge2=pd.DataFrame({'value': ['a', 'b', 'd'],'data2': range(3)})
pd.merge(dataframe_merge1,dataframe_merge2) #默认自动选取名称相同的column
```


<div>
<style scoped>
    .dataframe tbody tr th:only-of-type {
        vertical-align: middle;
    }
    .dataframe tbody tr th {
        vertical-align: top;
    }
    .dataframe thead th {
        text-align: right;
    }
</style>
<table border="1" class="dataframe">
  <thead>
    <tr style="text-align: right;">
      <th></th>
      <th>value</th>
      <th>data1</th>
      <th>data2</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <th>0</th>
      <td>b</td>
      <td>0</td>
      <td>1</td>
    </tr>
    <tr>
      <th>1</th>
      <td>b</td>
      <td>1</td>
      <td>1</td>
    </tr>
    <tr>
      <th>2</th>
      <td>b</td>
      <td>6</td>
      <td>1</td>
    </tr>
    <tr>
      <th>3</th>
      <td>a</td>
      <td>2</td>
      <td>0</td>
    </tr>
    <tr>
      <th>4</th>
      <td>a</td>
      <td>4</td>
      <td>0</td>
    </tr>
    <tr>
      <th>5</th>
      <td>a</td>
      <td>5</td>
      <td>0</td>
    </tr>
  </tbody>
</table>
</div>


```python
pd.merge(dataframe_merge1,dataframe_merge2,on='value') #可以传入on参数手动指定merge操作所参考的列，这一操作当两个DataFrame中存在多个名称相同的列时可以避免非期望的结果。
```


<div>
<style scoped>
    .dataframe tbody tr th:only-of-type {
        vertical-align: middle;
    }
    .dataframe tbody tr th {
        vertical-align: top;
    }
    .dataframe thead th {
        text-align: right;
    }
</style>
<table border="1" class="dataframe">
  <thead>
    <tr style="text-align: right;">
      <th></th>
      <th>value</th>
      <th>data1</th>
      <th>data2</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <th>0</th>
      <td>b</td>
      <td>0</td>
      <td>1</td>
    </tr>
    <tr>
      <th>1</th>
      <td>b</td>
      <td>1</td>
      <td>1</td>
    </tr>
    <tr>
      <th>2</th>
      <td>b</td>
      <td>6</td>
      <td>1</td>
    </tr>
    <tr>
      <th>3</th>
      <td>a</td>
      <td>2</td>
      <td>0</td>
    </tr>
    <tr>
      <th>4</th>
      <td>a</td>
      <td>4</td>
      <td>0</td>
    </tr>
    <tr>
      <th>5</th>
      <td>a</td>
      <td>5</td>
      <td>0</td>
    </tr>
  </tbody>
</table>
</div>


```python
pd.merge(dataframe_merge1,dataframe_merge2,left_on='data1',right_on='data2') #也可以手动指定两个名称不同的列进行合并操作。如果合并之前存在相同列名，会在合并之后将它们区分开
```


<div>
<style scoped>
    .dataframe tbody tr th:only-of-type {
        vertical-align: middle;
    }
    .dataframe tbody tr th {
        vertical-align: top;
    }
    .dataframe thead th {
        text-align: right;
    }
</style>
<table border="1" class="dataframe">
  <thead>
    <tr style="text-align: right;">
      <th></th>
      <th>value_x</th>
      <th>data1</th>
      <th>value_y</th>
      <th>data2</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <th>0</th>
      <td>b</td>
      <td>0</td>
      <td>a</td>
      <td>0</td>
    </tr>
    <tr>
      <th>1</th>
      <td>b</td>
      <td>1</td>
      <td>b</td>
      <td>1</td>
    </tr>
    <tr>
      <th>2</th>
      <td>a</td>
      <td>2</td>
      <td>d</td>
      <td>2</td>
    </tr>
  </tbody>
</table>
</div>


```python
dataframe_merge3=pd.DataFrame({'key1': ['foo', 'foo', 'bar'], 'key2': ['one', 'two', 'one'], 'lval': [1, 2, 3]})
dataframe_merge4=pd.DataFrame({'key1': ['foo', 'foo', 'bar', 'bar'],'key2': ['one', 'one', 'one', 'two'], 'rval': [4, 5, 6, 7]})
```


```python
pd.merge(dataframe_merge3,dataframe_merge4,on=['key1','key2'],how='outer') #on参数可以传入多个列名，代表按照多列进行合并
```


<div>
<style scoped>
    .dataframe tbody tr th:only-of-type {
        vertical-align: middle;
    }
    .dataframe tbody tr th {
        vertical-align: top;
    }
    .dataframe thead th {
        text-align: right;
    }
</style>
<table border="1" class="dataframe">
  <thead>
    <tr style="text-align: right;">
      <th></th>
      <th>key1</th>
      <th>key2</th>
      <th>lval</th>
      <th>rval</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <th>0</th>
      <td>foo</td>
      <td>one</td>
      <td>1.0</td>
      <td>4.0</td>
    </tr>
    <tr>
      <th>1</th>
      <td>foo</td>
      <td>one</td>
      <td>1.0</td>
      <td>5.0</td>
    </tr>
    <tr>
      <th>2</th>
      <td>foo</td>
      <td>two</td>
      <td>2.0</td>
      <td>NaN</td>
    </tr>
    <tr>
      <th>3</th>
      <td>bar</td>
      <td>one</td>
      <td>3.0</td>
      <td>6.0</td>
    </tr>
    <tr>
      <th>4</th>
      <td>bar</td>
      <td>two</td>
      <td>NaN</td>
      <td>7.0</td>
    </tr>
  </tbody>
</table>
</div>


```python
dataframe_merge5=pd.DataFrame({'key1': ['Ohio', 'Ohio', 'Ohio', 'Nevada', 'Nevada'], 'key2': [2000, 2001, 2002, 2001, 2002],'data': np.arange(5.)})
dataframe_merge6=pd.DataFrame(np.arange(12).reshape((6, 2)), index=[['Nevada', 'Nevada', 'Ohio', 'Ohio', 'Ohio', 'Ohio'], [2001, 2000, 2000, 2000, 2001, 2002]],columns=['event1', 'event2'])
```


```python
pd.merge(dataframe_merge5,dataframe_merge6,left_on=['key1','key2'],right_index=True,how='outer') #如果是与多层index进行合并，则需要同时传入多个列名，合并完成之后这些index会变为DataFrame中的列
```


<div>
<style scoped>
    .dataframe tbody tr th:only-of-type {
        vertical-align: middle;
    }
    .dataframe tbody tr th {
        vertical-align: top;
    }
    .dataframe thead th {
        text-align: right;
    }
</style>
<table border="1" class="dataframe">
  <thead>
    <tr style="text-align: right;">
      <th></th>
      <th>key1</th>
      <th>key2</th>
      <th>data</th>
      <th>event1</th>
      <th>event2</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <th>0</th>
      <td>Ohio</td>
      <td>2000</td>
      <td>0.0</td>
      <td>4.0</td>
      <td>5.0</td>
    </tr>
    <tr>
      <th>0</th>
      <td>Ohio</td>
      <td>2000</td>
      <td>0.0</td>
      <td>6.0</td>
      <td>7.0</td>
    </tr>
    <tr>
      <th>1</th>
      <td>Ohio</td>
      <td>2001</td>
      <td>1.0</td>
      <td>8.0</td>
      <td>9.0</td>
    </tr>
    <tr>
      <th>2</th>
      <td>Ohio</td>
      <td>2002</td>
      <td>2.0</td>
      <td>10.0</td>
      <td>11.0</td>
    </tr>
    <tr>
      <th>3</th>
      <td>Nevada</td>
      <td>2001</td>
      <td>3.0</td>
      <td>0.0</td>
      <td>1.0</td>
    </tr>
    <tr>
      <th>4</th>
      <td>Nevada</td>
      <td>2002</td>
      <td>4.0</td>
      <td>NaN</td>
      <td>NaN</td>
    </tr>
    <tr>
      <th>4</th>
      <td>Nevada</td>
      <td>2000</td>
      <td>NaN</td>
      <td>2.0</td>
      <td>3.0</td>
    </tr>
  </tbody>
</table>
</div>



与`merge()`函数功能类似的还有`join()`函数，用法为：`DataFrame1.join(DataFrame2)`。`join()`函数支持传入一个list，代表同时对三个及以上的DataFrame进行合并。`join()`函数可以使用的参数与`merge()`函数类似。

由于这两个函数的作用相同，此处不再赘述。

## 拼接

Pandas中的`concat`函数类似于NumPy中的`concatenate`函数，传入一个由series或dataframe组成的list，将它们拼接在一起。

`concat`函数支持的参数包括：

- objs：一个包含了Pandas数据类型的list或者dict（必须传入的参数）
- axis：进行拼接的方向，默认为行方向（即0）
- join：'inner'或者'outer'其中的一个，默认为'outer'。inner代表只留下index或者columns中的交集部分进行拼接，而outer则代表拼接操作按照并集的方式进行，没有对应元素的使用NaN填充
- keys：指明一系列拼接时使用的关键字，它们用于在拼接之后形成多层index，从而区分出数据的来源
- levels：如果传入keys参数，levels参数用于指明被用作层级index的哪一层
- names：如果使用key或者level参数，设定它们在index中对应的名字
- verify_integrity：检查数据是否有重复并报错，默认为False，即允许数据重复
- ignore_index：在合并的维度上丢掉原来所有的index，将其重新编号为0至n-1

下面是一些使用示例：


```python
series_concat1=pd.Series([0,1],index=['a','b'])
series_concat2=pd.Series([2,3,4],index=['c','d','e'])
series_concat3=pd.Series([5,6],index=['f','g'])
dataframe_concat1=pd.DataFrame(np.arange(6).reshape(3, 2), index=['a', 'b', 'c'],columns=['one', 'two'])
dataframe_concat2=pd.DataFrame(5 + np.arange(4).reshape(2, 2), index=['a', 'c'], columns=['three', 'four'])
```


```python
pd.concat([series_concat1,series_concat2,series_concat3]) #默认按照竖直方向合并
```


    a    0
    b    1
    c    2
    d    3
    e    4
    f    5
    g    6
    dtype: int64




```python
pd.concat([series_concat1,series_concat2,series_concat3],axis=1) #按照水平方向合并，默认采用outer合并方式
```


<div>
<style scoped>
    .dataframe tbody tr th:only-of-type {
        vertical-align: middle;
    }
    .dataframe tbody tr th {
        vertical-align: top;
    }
    .dataframe thead th {
        text-align: right;
    }
</style>
<table border="1" class="dataframe">
  <thead>
    <tr style="text-align: right;">
      <th></th>
      <th>0</th>
      <th>1</th>
      <th>2</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <th>a</th>
      <td>0.0</td>
      <td>NaN</td>
      <td>NaN</td>
    </tr>
    <tr>
      <th>b</th>
      <td>1.0</td>
      <td>NaN</td>
      <td>NaN</td>
    </tr>
    <tr>
      <th>c</th>
      <td>NaN</td>
      <td>2.0</td>
      <td>NaN</td>
    </tr>
    <tr>
      <th>d</th>
      <td>NaN</td>
      <td>3.0</td>
      <td>NaN</td>
    </tr>
    <tr>
      <th>e</th>
      <td>NaN</td>
      <td>4.0</td>
      <td>NaN</td>
    </tr>
    <tr>
      <th>f</th>
      <td>NaN</td>
      <td>NaN</td>
      <td>5.0</td>
    </tr>
    <tr>
      <th>g</th>
      <td>NaN</td>
      <td>NaN</td>
      <td>6.0</td>
    </tr>
  </tbody>
</table>
</div>


```python
pd.concat([series_concat1,series_concat2,series_concat3],keys=['one','two','three'],names=['upper','lower']) #在拼接完成之后将key作为额外的一层index来表示数据来源，并可以传入names为新的index中的每一层进行命名
```


    upper  lower
    one    a        0
           b        1
    two    c        2
           d        3
           e        4
    three  f        5
           g        6
    dtype: int64




```python
pd.concat([dataframe_concat1,dataframe_concat2],axis=1,ignore_index=True) #合并时忽略掉原有的index，为合并后的DataFrame赋予一个Range类型的index
```


<div>
<style scoped>
    .dataframe tbody tr th:only-of-type {
        vertical-align: middle;
    }
    .dataframe tbody tr th {
        vertical-align: top;
    }
    .dataframe thead th {
        text-align: right;
    }
</style>
<table border="1" class="dataframe">
  <thead>
    <tr style="text-align: right;">
      <th></th>
      <th>0</th>
      <th>1</th>
      <th>2</th>
      <th>3</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <th>a</th>
      <td>0</td>
      <td>1</td>
      <td>5.0</td>
      <td>6.0</td>
    </tr>
    <tr>
      <th>b</th>
      <td>2</td>
      <td>3</td>
      <td>NaN</td>
      <td>NaN</td>
    </tr>
    <tr>
      <th>c</th>
      <td>4</td>
      <td>5</td>
      <td>7.0</td>
      <td>8.0</td>
    </tr>
  </tbody>
</table>
</div>



## 填充式合并

Pandas中含有一个`x.combine_first(y)`函数，可以将y中有而x中没有的数据加入到x中，同时x中原有的数据不做任何修改：


```python
dataframe_combine1=pd.DataFrame({'a': [1., np.nan, 5., np.nan],'b': [np.nan, 2., np.nan, 6.],'c': range(2, 18, 4)})
dataframe_combine2=pd.DataFrame({'a': [5., 4., np.nan, 3., 7.],'b': [np.nan, 3., 4., 6., 8.]})
```


```python
dataframe_combine1.combine_first(dataframe_combine2) #自动按照DataFrame中的列名进行列的匹配，并且只填充NaN，其余部分不变
```


<div>
<style scoped>
    .dataframe tbody tr th:only-of-type {
        vertical-align: middle;
    }
    .dataframe tbody tr th {
        vertical-align: top;
    }
    .dataframe thead th {
        text-align: right;
    }
</style>
<table border="1" class="dataframe">
  <thead>
    <tr style="text-align: right;">
      <th></th>
      <th>a</th>
      <th>b</th>
      <th>c</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <th>0</th>
      <td>1.0</td>
      <td>NaN</td>
      <td>2.0</td>
    </tr>
    <tr>
      <th>1</th>
      <td>4.0</td>
      <td>2.0</td>
      <td>6.0</td>
    </tr>
    <tr>
      <th>2</th>
      <td>5.0</td>
      <td>4.0</td>
      <td>10.0</td>
    </tr>
    <tr>
      <th>3</th>
      <td>3.0</td>
      <td>6.0</td>
      <td>14.0</td>
    </tr>
    <tr>
      <th>4</th>
      <td>7.0</td>
      <td>8.0</td>
      <td>NaN</td>
    </tr>
  </tbody>
</table>
</div>


```python
dataframe_combine2.combine_first(dataframe_combine1) #注意这一操作的顺序是不可交换的
```


<div>
<style scoped>
    .dataframe tbody tr th:only-of-type {
        vertical-align: middle;
    }
    .dataframe tbody tr th {
        vertical-align: top;
    }
    .dataframe thead th {
        text-align: right;
    }
</style>
<table border="1" class="dataframe">
  <thead>
    <tr style="text-align: right;">
      <th></th>
      <th>a</th>
      <th>b</th>
      <th>c</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <th>0</th>
      <td>5.0</td>
      <td>NaN</td>
      <td>2.0</td>
    </tr>
    <tr>
      <th>1</th>
      <td>4.0</td>
      <td>3.0</td>
      <td>6.0</td>
    </tr>
    <tr>
      <th>2</th>
      <td>5.0</td>
      <td>4.0</td>
      <td>10.0</td>
    </tr>
    <tr>
      <th>3</th>
      <td>3.0</td>
      <td>6.0</td>
      <td>14.0</td>
    </tr>
    <tr>
      <th>4</th>
      <td>7.0</td>
      <td>8.0</td>
      <td>NaN</td>
    </tr>
  </tbody>
</table>
</div>



## 重排与旋转

Pandas中的`stack()`操作可以将某个columns“旋转”至index，与之对应的`unstack()`操作则是将某个index“旋转”至columns。这两个操作都会把选择的那层index或者column插入到最内层。

使用示例：


```python
dataframe_stack=pd.DataFrame(np.arange(6).reshape((2, 3)),index=pd.Index(['Ohio', 'Colorado'], name='state'),columns=pd.MultiIndex.from_arrays([['a','a','b'],['one', 'two', 'three']]))
dataframe_stack
```


<div>
<style scoped>
    .dataframe tbody tr th:only-of-type {
        vertical-align: middle;
    }
    .dataframe tbody tr th {
        vertical-align: top;
    }
    .dataframe thead tr th {
        text-align: left;
    }
    .dataframe thead tr:last-of-type th {
        text-align: right;
    }
</style>
<table border="1" class="dataframe">
  <thead>
    <tr>
      <th></th>
      <th colspan="2" halign="left">a</th>
      <th>b</th>
    </tr>
    <tr>
      <th></th>
      <th>one</th>
      <th>two</th>
      <th>three</th>
    </tr>
    <tr>
      <th>state</th>
      <th></th>
      <th></th>
      <th></th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <th>Ohio</th>
      <td>0</td>
      <td>1</td>
      <td>2</td>
    </tr>
    <tr>
      <th>Colorado</th>
      <td>3</td>
      <td>4</td>
      <td>5</td>
    </tr>
  </tbody>
</table>
</div>


```python
dataframe_stack.stack() #stack默认选择最里层的columns，与之类似的是unstack默认选择的是最里层的index。需要注意stack和unstack操作都可能会产生NaN数据
```


<div>
<style scoped>
    .dataframe tbody tr th:only-of-type {
        vertical-align: middle;
    }
    .dataframe tbody tr th {
        vertical-align: top;
    }
    .dataframe thead th {
        text-align: right;
    }
</style>
<table border="1" class="dataframe">
  <thead>
    <tr style="text-align: right;">
      <th></th>
      <th></th>
      <th>a</th>
      <th>b</th>
    </tr>
    <tr>
      <th>state</th>
      <th></th>
      <th></th>
      <th></th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <th rowspan="3" valign="top">Ohio</th>
      <th>one</th>
      <td>0.0</td>
      <td>NaN</td>
    </tr>
    <tr>
      <th>three</th>
      <td>NaN</td>
      <td>2.0</td>
    </tr>
    <tr>
      <th>two</th>
      <td>1.0</td>
      <td>NaN</td>
    </tr>
    <tr>
      <th rowspan="3" valign="top">Colorado</th>
      <th>one</th>
      <td>3.0</td>
      <td>NaN</td>
    </tr>
    <tr>
      <th>three</th>
      <td>NaN</td>
      <td>5.0</td>
    </tr>
    <tr>
      <th>two</th>
      <td>4.0</td>
      <td>NaN</td>
    </tr>
  </tbody>
</table>
</div>


```python
dataframe_stack.stack(level=0) #可以通过传入level参数来指定stack操作对哪一层columns进行操作。从外向里对应的level值由0开始递增，当然也可以传入负数做逆序索引。unstack也可以做类似操作
```


<div>
<style scoped>
    .dataframe tbody tr th:only-of-type {
        vertical-align: middle;
    }
    .dataframe tbody tr th {
        vertical-align: top;
    }
    .dataframe thead th {
        text-align: right;
    }
</style>
<table border="1" class="dataframe">
  <thead>
    <tr style="text-align: right;">
      <th></th>
      <th></th>
      <th>one</th>
      <th>three</th>
      <th>two</th>
    </tr>
    <tr>
      <th>state</th>
      <th></th>
      <th></th>
      <th></th>
      <th></th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <th rowspan="2" valign="top">Ohio</th>
      <th>a</th>
      <td>0.0</td>
      <td>NaN</td>
      <td>1.0</td>
    </tr>
    <tr>
      <th>b</th>
      <td>NaN</td>
      <td>2.0</td>
      <td>NaN</td>
    </tr>
    <tr>
      <th rowspan="2" valign="top">Colorado</th>
      <th>a</th>
      <td>3.0</td>
      <td>NaN</td>
      <td>4.0</td>
    </tr>
    <tr>
      <th>b</th>
      <td>NaN</td>
      <td>5.0</td>
      <td>NaN</td>
    </tr>
  </tbody>
</table>
</div>


```python
dataframe_stack.unstack('state') #可以传入某一层对应的名称来进行操作
```


              state   
    a  one    Ohio        0
              Colorado    3
       two    Ohio        1
              Colorado    4
    b  three  Ohio        2
              Colorado    5
    dtype: int32



pivot函数可以对一个DataFrame进行重排，要求这个DataFrame中的index为默认值。函数返回一个新的DataFrame。

如果传入三个及以上的列名称作为参数，第一个列中的内容会被转化为行的index，第二个列所包含的内容会被转化为column，之后，所有列中的内容会与index和column做对应，从而填充整个DataFrame；如果只传入两个列作为参数，那么第一列仍然作为index，原表格中的其它所有列都被保留下来，并与第二个列的内容共同构成一个多层的columns。例如：


```python
dataframe_pivot=pd.DataFrame({'key': ['foo', 'bar', 'baz'], 'A': [1, 2, 3], 'B': [4, 5, 6], 'C': [7, 8, 9]})
```


```python
dataframe_pivot
```


<div>
<style scoped>
    .dataframe tbody tr th:only-of-type {
        vertical-align: middle;
    }
    .dataframe tbody tr th {
        vertical-align: top;
    }
    .dataframe thead th {
        text-align: right;
    }
</style>
<table border="1" class="dataframe">
  <thead>
    <tr style="text-align: right;">
      <th></th>
      <th>key</th>
      <th>A</th>
      <th>B</th>
      <th>C</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <th>0</th>
      <td>foo</td>
      <td>1</td>
      <td>4</td>
      <td>7</td>
    </tr>
    <tr>
      <th>1</th>
      <td>bar</td>
      <td>2</td>
      <td>5</td>
      <td>8</td>
    </tr>
    <tr>
      <th>2</th>
      <td>baz</td>
      <td>3</td>
      <td>6</td>
      <td>9</td>
    </tr>
  </tbody>
</table>
</div>


```python
dataframe_pivot.pivot('key','B') #当只传入两个参数时，除了将选中的两列变为index和columns之外，其余数据全部保留。本例中将key这一列作为index，同时也将B这一列变为columns的其中一层
```


<div>
<style scoped>
    .dataframe tbody tr th:only-of-type {
        vertical-align: middle;
    }
    .dataframe tbody tr th {
        vertical-align: top;
    }
    .dataframe thead tr th {
        text-align: left;
    }
    .dataframe thead tr:last-of-type th {
        text-align: right;
    }
</style>
<table border="1" class="dataframe">
  <thead>
    <tr>
      <th></th>
      <th colspan="3" halign="left">A</th>
      <th colspan="3" halign="left">C</th>
    </tr>
    <tr>
      <th>B</th>
      <th>4</th>
      <th>5</th>
      <th>6</th>
      <th>4</th>
      <th>5</th>
      <th>6</th>
    </tr>
    <tr>
      <th>key</th>
      <th></th>
      <th></th>
      <th></th>
      <th></th>
      <th></th>
      <th></th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <th>bar</th>
      <td>NaN</td>
      <td>2.0</td>
      <td>NaN</td>
      <td>NaN</td>
      <td>8.0</td>
      <td>NaN</td>
    </tr>
    <tr>
      <th>baz</th>
      <td>NaN</td>
      <td>NaN</td>
      <td>3.0</td>
      <td>NaN</td>
      <td>NaN</td>
      <td>9.0</td>
    </tr>
    <tr>
      <th>foo</th>
      <td>1.0</td>
      <td>NaN</td>
      <td>NaN</td>
      <td>7.0</td>
      <td>NaN</td>
      <td>NaN</td>
    </tr>
  </tbody>
</table>
</div>


```python
dataframe_pivot.pivot('key','B','A') #如果传入三个或以上的参数，那么最终的数据只保留第三个及以后的参数所对应列中的数据
```


<div>
<style scoped>
    .dataframe tbody tr th:only-of-type {
        vertical-align: middle;
    }
    .dataframe tbody tr th {
        vertical-align: top;
    }
    .dataframe thead th {
        text-align: right;
    }
</style>
<table border="1" class="dataframe">
  <thead>
    <tr style="text-align: right;">
      <th>B</th>
      <th>4</th>
      <th>5</th>
      <th>6</th>
    </tr>
    <tr>
      <th>key</th>
      <th></th>
      <th></th>
      <th></th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <th>bar</th>
      <td>NaN</td>
      <td>2.0</td>
      <td>NaN</td>
    </tr>
    <tr>
      <th>baz</th>
      <td>NaN</td>
      <td>NaN</td>
      <td>3.0</td>
    </tr>
    <tr>
      <th>foo</th>
      <td>1.0</td>
      <td>NaN</td>
      <td>NaN</td>
    </tr>
  </tbody>
</table>
</div>



相对应地，`pivot`的反操作为`melt`。这一操作会将DataFrame变为两列或三列。

如果只传入value_vars参数，那么生成的DataFrame有两列，分别为variable和value，variable对应的是value_vars所包含的列名，value对应的是value_vars中的列名所指向的列里面的所有元素。

如果除了value_vars参数，还传入了id_vars参数，那么最终形成的DataFrame中会额外多出id_vars所对应的若干列。这些列中的内容为id_vars所对应列中的元素。

下面为一些示例：


```python
dataframe_pivot.melt(id_vars=['key']) #只传入id_vars参数的话，默认会选取其他所有的列为value_vars
```


<div>
<style scoped>
    .dataframe tbody tr th:only-of-type {
        vertical-align: middle;
    }
    .dataframe tbody tr th {
        vertical-align: top;
    }
    .dataframe thead th {
        text-align: right;
    }
</style>
<table border="1" class="dataframe">
  <thead>
    <tr style="text-align: right;">
      <th></th>
      <th>key</th>
      <th>variable</th>
      <th>value</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <th>0</th>
      <td>foo</td>
      <td>A</td>
      <td>1</td>
    </tr>
    <tr>
      <th>1</th>
      <td>bar</td>
      <td>A</td>
      <td>2</td>
    </tr>
    <tr>
      <th>2</th>
      <td>baz</td>
      <td>A</td>
      <td>3</td>
    </tr>
    <tr>
      <th>3</th>
      <td>foo</td>
      <td>B</td>
      <td>4</td>
    </tr>
    <tr>
      <th>4</th>
      <td>bar</td>
      <td>B</td>
      <td>5</td>
    </tr>
    <tr>
      <th>5</th>
      <td>baz</td>
      <td>B</td>
      <td>6</td>
    </tr>
    <tr>
      <th>6</th>
      <td>foo</td>
      <td>C</td>
      <td>7</td>
    </tr>
    <tr>
      <th>7</th>
      <td>bar</td>
      <td>C</td>
      <td>8</td>
    </tr>
    <tr>
      <th>8</th>
      <td>baz</td>
      <td>C</td>
      <td>9</td>
    </tr>
  </tbody>
</table>
</div>


```python
dataframe_pivot.melt(value_vars=['B','C'])
```


<div>
<style scoped>
    .dataframe tbody tr th:only-of-type {
        vertical-align: middle;
    }
    .dataframe tbody tr th {
        vertical-align: top;
    }
    .dataframe thead th {
        text-align: right;
    }
</style>
<table border="1" class="dataframe">
  <thead>
    <tr style="text-align: right;">
      <th></th>
      <th>variable</th>
      <th>value</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <th>0</th>
      <td>B</td>
      <td>4</td>
    </tr>
    <tr>
      <th>1</th>
      <td>B</td>
      <td>5</td>
    </tr>
    <tr>
      <th>2</th>
      <td>B</td>
      <td>6</td>
    </tr>
    <tr>
      <th>3</th>
      <td>C</td>
      <td>7</td>
    </tr>
    <tr>
      <th>4</th>
      <td>C</td>
      <td>8</td>
    </tr>
    <tr>
      <th>5</th>
      <td>C</td>
      <td>9</td>
    </tr>
  </tbody>
</table>
</div>


```python
dataframe_pivot.melt(id_vars=['A','B'],value_vars=['key','C'])
```


<div>
<style scoped>
    .dataframe tbody tr th:only-of-type {
        vertical-align: middle;
    }
    .dataframe tbody tr th {
        vertical-align: top;
    }
    .dataframe thead th {
        text-align: right;
    }
</style>
<table border="1" class="dataframe">
  <thead>
    <tr style="text-align: right;">
      <th></th>
      <th>A</th>
      <th>B</th>
      <th>variable</th>
      <th>value</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <th>0</th>
      <td>1</td>
      <td>4</td>
      <td>key</td>
      <td>foo</td>
    </tr>
    <tr>
      <th>1</th>
      <td>2</td>
      <td>5</td>
      <td>key</td>
      <td>bar</td>
    </tr>
    <tr>
      <th>2</th>
      <td>3</td>
      <td>6</td>
      <td>key</td>
      <td>baz</td>
    </tr>
    <tr>
      <th>3</th>
      <td>1</td>
      <td>4</td>
      <td>C</td>
      <td>7</td>
    </tr>
    <tr>
      <th>4</th>
      <td>2</td>
      <td>5</td>
      <td>C</td>
      <td>8</td>
    </tr>
    <tr>
      <th>5</th>
      <td>3</td>
      <td>6</td>
      <td>C</td>
      <td>9</td>
    </tr>
  </tbody>
</table>
</div>



# 数据的分组

## 生成GroupBy变量
Series和DataFrame可以基于提供的一组关键字，按照行或列对其进行分组。

分组的方法有以下几类：

1. 一个list或array，其中的元素作为分组的依据，要求大小与待分组轴向的大小相等
2. 一个dict或Series，将待分组的数据以及这些数据所在的分组对应起来
3. 一个列名，该列中的元素作为分组的依据，等同于传入一个Series 
4. 一个函数，可以作用于index或者是元素
5. 一个index，适用于多层index的情况

使用`groupby`函数，并传入上述三种参数中的其中一个，便可得到一个类型为`SeriesGroupBy`或者`DataFrameGroupBy`的变量。

下面为一些使用示例：


```python
dataframe_groupby=pd.DataFrame({'key1' : ['a', 'a', 'b', 'b', 'a'],'key2' : ['one', 'two', 'one', 'two', 'one'], 'data1' : np.random.randn(5), 'data2' : np.random.randn(5)})
```


```python
dataframe_groupby
```


<div>
<style scoped>
    .dataframe tbody tr th:only-of-type {
        vertical-align: middle;
    }
    .dataframe tbody tr th {
        vertical-align: top;
    }
    .dataframe thead th {
        text-align: right;
    }
</style>
<table border="1" class="dataframe">
  <thead>
    <tr style="text-align: right;">
      <th></th>
      <th>key1</th>
      <th>key2</th>
      <th>data1</th>
      <th>data2</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <th>0</th>
      <td>a</td>
      <td>one</td>
      <td>0.773597</td>
      <td>-0.769454</td>
    </tr>
    <tr>
      <th>1</th>
      <td>a</td>
      <td>two</td>
      <td>0.552522</td>
      <td>-0.123829</td>
    </tr>
    <tr>
      <th>2</th>
      <td>b</td>
      <td>one</td>
      <td>-0.264872</td>
      <td>-0.145577</td>
    </tr>
    <tr>
      <th>3</th>
      <td>b</td>
      <td>two</td>
      <td>-0.222394</td>
      <td>-0.357799</td>
    </tr>
    <tr>
      <th>4</th>
      <td>a</td>
      <td>one</td>
      <td>-1.691627</td>
      <td>0.530238</td>
    </tr>
  </tbody>
</table>
</div>


```python
grouped_series=dataframe_groupby['data1'].groupby(dataframe_groupby['key1']) #使用Series作为分组的依据
```


```python
grouped_series #得到一个GroupBy类型的变量
```


    <pandas.core.groupby.generic.SeriesGroupBy object at 0x000001B04A2A2688>




```python
grouped_series2=dataframe_groupby['data1'].groupby([dataframe_groupby['key1'],dataframe_groupby['key2']]) #可以传入多个Series进行分组，最终得到的Series具有多重index
```


```python
states = np.array(['Ohio', 'California', 'California', 'Ohio', 'Ohio'])
years = np.array([2005, 2005, 2006, 2005, 2006])
grouped_arr=dataframe_groupby['data1'].groupby([states,years]) #传入array来进行分组，array中的元素作为分组的依据
```


```python
grouped_colname=dataframe_groupby.groupby(['key1','key2']) #可以传入多个列名进行分组
```


```python
grouped_col=dataframe_groupby.groupby(dataframe_groupby.dtypes,axis=1) #默认情况下按照行进行分组，可以传入axis=1来按列进行分组
```


```python
mapping={0:'red',1:'green',2:'blue',3:'red',4:'blue'}
grouped_mapping=dataframe_groupby.groupby(mapping) #使用dict来进行分组，与传入一个Series的作用类似
```


```python
func=lambda x: x>0
grouped_func=dataframe_groupby['data1'].groupby(func) #使用函数来进行分组
```


```python
columns = pd.MultiIndex.from_arrays([['US', 'US', 'US', 'JP', 'JP'],[1, 3, 5, 1, 3]], names=['cty', 'tenor'])
dataframe_groupby2=pd.DataFrame(np.random.randn(4, 5), columns=columns)
grouped_hier=dataframe_groupby2.groupby(level='cty', axis=1) #使用多层index中的某一层进行分组
```

## GroupBy变量的使用

### 查看内容

groupby`函数得到的`GroupBy`变量可以按照下面的两种方法查看或是使用每个分组的内容：

1. 用于循环遍历，每个循环输出一个分组名称和内容
2. 转换为list类型，list中的每个元素都是一个tuple，包含每个分组的名称和内容

### 索引

如果`GroupBy`变量是从一个DataFrame分组而来的，可以使用DataFrame的列名对其进行索引，得到一个`SeriesGroupBy`变量。

### 使用函数

`GroupBy`变量也支持一些统计函数，包括：

- count：统计分组中非NaN的数目
- sum：统计所有非NaN元素的和
- mean：计算所有非NaN元素的平均值
- median：计算所有非NaN元素的中位数
- std、var：统计非偏（即分母为n-1）标准差或方差
- min、max：统计非NaN元素的最大与最小值
- prod：统计所有非NaN元素的乘积
- first、last：第一个或者最后一个非NaN元素
- describe：对每个分组的数据进行统计，输出一些统计量

另外，还有几个比较特殊的函数用法：

- agg：这一函数的用法有下面几种
    1. 可以自己定义函数，然后作为参数传入到`agg`方法中，从而作用于每一个分组。传入的函数要求是聚合函数（即输出像统计函数一样会降维）
    2. 支持传入一个元素为统计函数名称的list
    3. 或是传入一个以二维tuple为元素的list，tuple的第一个元素为统计结果列的名称，第二个元素为统计函数的名称或是一个函数变量。
- apply：传入一个函数作为输入参数，对每个分组调用这一函数，然后用concat方法将计算结果合起来，得到一个多重index的DataFrame。因此，每个分组的计算结果要满足concat操作的要求。
- transform：对每个分组使用一个函数，要求函数的输出结果必须为一个标量，或是大小相同的array。这一操作完成之后，会生成一个与分组前规模相同的Series或者DataFrame，其中元素的值便是使用原始数据所在分组的函数输出来填充进去的。

下面为一些使用示例：


```python
for name,group in grouped_func:
    print(name) #name对应于分组的名称
    print(group) #group对应于每个分组的内容，可以是一个Series或者DataFrame
```

    False
    0    0.773597
    Name: data1, dtype: float64
    True
    1    0.552522
    2   -0.264872
    3   -0.222394
    4   -1.691627
    Name: data1, dtype: float64



```python
list(grouped_col) #将GroupBy转化为一个list
```


    [(dtype('float64'),
            data1     data2
      0  0.773597 -0.769454
      1  0.552522 -0.123829
      2 -0.264872 -0.145577
      3 -0.222394 -0.357799
      4 -1.691627  0.530238),
     (dtype('O'),
        key1 key2
      0    a  one
      1    a  two
      2    b  one
      3    b  two
      4    a  one)]




```python
grouped_hier['US'] #从DataFrameGroupBy中索引得到SeriesGroupBy
```


    <pandas.core.groupby.generic.SeriesGroupBy object at 0x000001B04A2ACFC8>




```python
grouped_colname.sum() #对GroupBy变量使用统计函数
```


<div>
<style scoped>
    .dataframe tbody tr th:only-of-type {
        vertical-align: middle;
    }
    .dataframe tbody tr th {
        vertical-align: top;
    }
    .dataframe thead th {
        text-align: right;
    }
</style>
<table border="1" class="dataframe">
  <thead>
    <tr style="text-align: right;">
      <th></th>
      <th></th>
      <th>data1</th>
      <th>data2</th>
    </tr>
    <tr>
      <th>key1</th>
      <th>key2</th>
      <th></th>
      <th></th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <th rowspan="2" valign="top">a</th>
      <th>one</th>
      <td>-0.918030</td>
      <td>-0.239216</td>
    </tr>
    <tr>
      <th>two</th>
      <td>0.552522</td>
      <td>-0.123829</td>
    </tr>
    <tr>
      <th rowspan="2" valign="top">b</th>
      <th>one</th>
      <td>-0.264872</td>
      <td>-0.145577</td>
    </tr>
    <tr>
      <th>two</th>
      <td>-0.222394</td>
      <td>-0.357799</td>
    </tr>
  </tbody>
</table>
</div>


```python
grouped_hier.describe()
#这一函数相当于是下面两条命令的简化版：
#f=lambda x:x.describe()
#grouped.apply(f)
```


<div>
<style scoped>
    .dataframe tbody tr th:only-of-type {
        vertical-align: middle;
    }
    .dataframe tbody tr th {
        vertical-align: top;
    }
    .dataframe thead th {
        text-align: right;
    }
</style>
<table border="1" class="dataframe">
  <thead>
    <tr style="text-align: right;">
      <th></th>
      <th></th>
      <th></th>
      <th>count</th>
      <th>mean</th>
      <th>std</th>
      <th>min</th>
      <th>25%</th>
      <th>50%</th>
      <th>75%</th>
      <th>max</th>
    </tr>
    <tr>
      <th>cty</th>
      <th>cty</th>
      <th>tenor</th>
      <th></th>
      <th></th>
      <th></th>
      <th></th>
      <th></th>
      <th></th>
      <th></th>
      <th></th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <th rowspan="2" valign="top">JP</th>
      <th rowspan="2" valign="top">JP</th>
      <th>1</th>
      <td>4.0</td>
      <td>-0.469233</td>
      <td>1.009621</td>
      <td>-1.229898</td>
      <td>-0.940842</td>
      <td>-0.832966</td>
      <td>-0.361357</td>
      <td>1.018895</td>
    </tr>
    <tr>
      <th>3</th>
      <td>4.0</td>
      <td>0.167405</td>
      <td>0.584559</td>
      <td>-0.516235</td>
      <td>-0.212018</td>
      <td>0.221274</td>
      <td>0.600696</td>
      <td>0.743307</td>
    </tr>
    <tr>
      <th rowspan="3" valign="top">US</th>
      <th rowspan="3" valign="top">US</th>
      <th>1</th>
      <td>4.0</td>
      <td>-0.356559</td>
      <td>1.681193</td>
      <td>-1.717014</td>
      <td>-1.399371</td>
      <td>-0.874332</td>
      <td>0.168480</td>
      <td>2.039443</td>
    </tr>
    <tr>
      <th>3</th>
      <td>4.0</td>
      <td>-0.080853</td>
      <td>0.551140</td>
      <td>-0.674817</td>
      <td>-0.479963</td>
      <td>-0.060771</td>
      <td>0.338339</td>
      <td>0.472946</td>
    </tr>
    <tr>
      <th>5</th>
      <td>4.0</td>
      <td>0.227364</td>
      <td>1.239056</td>
      <td>-1.354026</td>
      <td>-0.461604</td>
      <td>0.498428</td>
      <td>1.187396</td>
      <td>1.266626</td>
    </tr>
  </tbody>
</table>
</div>


```python
def diff(arr):
    return arr.max()-arr.min()
grouped_arr.agg(diff)
```


    California  2005    0.00000
                2006    0.00000
    Ohio        2005    0.99599
                2006    0.00000
    Name: data1, dtype: float64




```python
grouped_series.agg(['count','max','min']) #传入一个元素为统计函数名称的list
```


<div>
<style scoped>
    .dataframe tbody tr th:only-of-type {
        vertical-align: middle;
    }
    .dataframe tbody tr th {
        vertical-align: top;
    }
    .dataframe thead th {
        text-align: right;
    }
</style>
<table border="1" class="dataframe">
  <thead>
    <tr style="text-align: right;">
      <th></th>
      <th>count</th>
      <th>max</th>
      <th>min</th>
    </tr>
    <tr>
      <th>key1</th>
      <th></th>
      <th></th>
      <th></th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <th>a</th>
      <td>3</td>
      <td>0.773597</td>
      <td>-1.691627</td>
    </tr>
    <tr>
      <th>b</th>
      <td>2</td>
      <td>-0.222394</td>
      <td>-0.264872</td>
    </tr>
  </tbody>
</table>
</div>


```python
grouped_mapping.agg([('sum_val','sum'),('bar',np.std)]) #传入一个元素为tuple的list
```


<div>
<style scoped>
    .dataframe tbody tr th:only-of-type {
        vertical-align: middle;
    }
    .dataframe tbody tr th {
        vertical-align: top;
    }
    .dataframe thead tr th {
        text-align: left;
    }
</style>
<table border="1" class="dataframe">
  <thead>
    <tr>
      <th></th>
      <th colspan="2" halign="left">data1</th>
      <th colspan="2" halign="left">data2</th>
    </tr>
    <tr>
      <th></th>
      <th>sum_val</th>
      <th>bar</th>
      <th>sum_val</th>
      <th>bar</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <th>blue</th>
      <td>-1.956499</td>
      <td>1.008868</td>
      <td>0.384661</td>
      <td>0.477873</td>
    </tr>
    <tr>
      <th>green</th>
      <td>0.552522</td>
      <td>NaN</td>
      <td>-0.123829</td>
      <td>NaN</td>
    </tr>
    <tr>
      <th>red</th>
      <td>0.551203</td>
      <td>0.704271</td>
      <td>-1.127252</td>
      <td>0.291084</td>
    </tr>
  </tbody>
</table>
</div>


```python
grouped_hier.transform(np.mean) #transform函数的使用示例，输出与原始的DataFrame具有相同规模，其中的元素来源于数据所在分组计算出的函数输出
```


<div>
<style scoped>
    .dataframe tbody tr th:only-of-type {
        vertical-align: middle;
    }
    .dataframe tbody tr th {
        vertical-align: top;
    }
    .dataframe thead tr th {
        text-align: left;
    }
</style>
<table border="1" class="dataframe">
  <thead>
    <tr>
      <th>cty</th>
      <th colspan="3" halign="left">US</th>
      <th colspan="2" halign="left">JP</th>
    </tr>
    <tr>
      <th>tenor</th>
      <th>1</th>
      <th>3</th>
      <th>5</th>
      <th>1</th>
      <th>3</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <th>0</th>
      <td>0.392919</td>
      <td>0.392919</td>
      <td>0.392919</td>
      <td>0.786027</td>
      <td>0.786027</td>
    </tr>
    <tr>
      <th>1</th>
      <td>-1.020843</td>
      <td>-1.020843</td>
      <td>-1.020843</td>
      <td>-0.243295</td>
      <td>-0.243295</td>
    </tr>
    <tr>
      <th>2</th>
      <td>-0.529225</td>
      <td>-0.529225</td>
      <td>-0.529225</td>
      <td>-0.668838</td>
      <td>-0.668838</td>
    </tr>
    <tr>
      <th>3</th>
      <td>0.877084</td>
      <td>0.877084</td>
      <td>0.877084</td>
      <td>-0.477551</td>
      <td>-0.477551</td>
    </tr>
  </tbody>
</table>
</div>



# 数据透视表

Pandas提供的`pivot_table`函数可以用来构造数据透视表。这一函数支持的参数如下：

- values：用于做数据透视表的数据列，默认使用全部的数据列
- index：传入一个列名或者是其它用于分组的关键词，用作数据透视表的行
- columns：传入一个列名或者是其它用于分组的关键词，用作数据透视表的列
- aggfunc：传入一个聚合函数或者是聚合函数列表，默认为`mean`
- fill_value：对数据透视表中NaN的数据的填充值
- margins：是否加入每行/每列的总计以及总体的总计，默认为False
- dropna：如果为True，则不包含全部为NaN的列

如果要做交叉表，即表中元素为数据频数的特殊数据透视表，可以用`crosstab`函数。

下面为一些示例：


```python
data=[[16.99, 1.01, 'No', 'Sun', 'Dinner', 2, 0.059447],
[10.34, 1.66, 'No', 'Sun', 'Dinner', 3, 0.160542],
[21.01, 3.50, 'No', 'Sat', 'Lunch', 3, 0.166587],
[23.68, 3.31, 'Yes', 'Sat', 'Breakfast', 2, 0.139780],
[24.59, 3.61, 'No', 'Sun', 'Dinner', 4, 0.146808],
[25.29, 4.71, 'Yes', 'Sat', 'Lunch', 4, 0.186240]]
dataframe_pivottable=pd.DataFrame(data,columns=['total_bill','tip','smoker','day','time','size','tip_pct'])
```


```python
dataframe_pivottable.pivot_table(index=['smoker','day'],columns='time',margins=True)
```


<div>
<style scoped>
    .dataframe tbody tr th:only-of-type {
        vertical-align: middle;
    }
    .dataframe tbody tr th {
        vertical-align: top;
    }
    .dataframe thead tr th {
        text-align: left;
    }
    .dataframe thead tr:last-of-type th {
        text-align: right;
    }
</style>
<table border="1" class="dataframe">
  <thead>
    <tr>
      <th></th>
      <th></th>
      <th colspan="4" halign="left">size</th>
      <th colspan="4" halign="left">tip</th>
      <th colspan="4" halign="left">tip_pct</th>
      <th colspan="4" halign="left">total_bill</th>
    </tr>
    <tr>
      <th></th>
      <th>time</th>
      <th>Breakfast</th>
      <th>Dinner</th>
      <th>Lunch</th>
      <th>All</th>
      <th>Breakfast</th>
      <th>Dinner</th>
      <th>Lunch</th>
      <th>All</th>
      <th>Breakfast</th>
      <th>Dinner</th>
      <th>Lunch</th>
      <th>All</th>
      <th>Breakfast</th>
      <th>Dinner</th>
      <th>Lunch</th>
      <th>All</th>
    </tr>
    <tr>
      <th>smoker</th>
      <th>day</th>
      <th></th>
      <th></th>
      <th></th>
      <th></th>
      <th></th>
      <th></th>
      <th></th>
      <th></th>
      <th></th>
      <th></th>
      <th></th>
      <th></th>
      <th></th>
      <th></th>
      <th></th>
      <th></th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <th rowspan="2" valign="top">No</th>
      <th>Sat</th>
      <td>NaN</td>
      <td>NaN</td>
      <td>3.0</td>
      <td>3</td>
      <td>NaN</td>
      <td>NaN</td>
      <td>3.500</td>
      <td>3.500000</td>
      <td>NaN</td>
      <td>NaN</td>
      <td>0.166587</td>
      <td>0.166587</td>
      <td>NaN</td>
      <td>NaN</td>
      <td>21.01</td>
      <td>21.010000</td>
    </tr>
    <tr>
      <th>Sun</th>
      <td>NaN</td>
      <td>3.0</td>
      <td>NaN</td>
      <td>3</td>
      <td>NaN</td>
      <td>2.093333</td>
      <td>NaN</td>
      <td>2.093333</td>
      <td>NaN</td>
      <td>0.122266</td>
      <td>NaN</td>
      <td>0.122266</td>
      <td>NaN</td>
      <td>17.306667</td>
      <td>NaN</td>
      <td>17.306667</td>
    </tr>
    <tr>
      <th>Yes</th>
      <th>Sat</th>
      <td>2.0</td>
      <td>NaN</td>
      <td>4.0</td>
      <td>3</td>
      <td>3.31</td>
      <td>NaN</td>
      <td>4.710</td>
      <td>4.010000</td>
      <td>0.13978</td>
      <td>NaN</td>
      <td>0.186240</td>
      <td>0.163010</td>
      <td>23.68</td>
      <td>NaN</td>
      <td>25.29</td>
      <td>24.485000</td>
    </tr>
    <tr>
      <th>All</th>
      <th></th>
      <td>2.0</td>
      <td>3.0</td>
      <td>3.5</td>
      <td>3</td>
      <td>3.31</td>
      <td>2.093333</td>
      <td>4.105</td>
      <td>2.966667</td>
      <td>0.13978</td>
      <td>0.122266</td>
      <td>0.176414</td>
      <td>0.143234</td>
      <td>23.68</td>
      <td>17.306667</td>
      <td>23.15</td>
      <td>20.316667</td>
    </tr>
  </tbody>
</table>
</div>


```python
pd.crosstab([dataframe_pivottable['smoker'],dataframe_pivottable['time']],dataframe_pivottable['day'],margins=True) 
```


<div>
<style scoped>
    .dataframe tbody tr th:only-of-type {
        vertical-align: middle;
    }
    .dataframe tbody tr th {
        vertical-align: top;
    }
    .dataframe thead th {
        text-align: right;
    }
</style>
<table border="1" class="dataframe">
  <thead>
    <tr style="text-align: right;">
      <th></th>
      <th>day</th>
      <th>Sat</th>
      <th>Sun</th>
      <th>All</th>
    </tr>
    <tr>
      <th>smoker</th>
      <th>time</th>
      <th></th>
      <th></th>
      <th></th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <th rowspan="2" valign="top">No</th>
      <th>Dinner</th>
      <td>0</td>
      <td>3</td>
      <td>3</td>
    </tr>
    <tr>
      <th>Lunch</th>
      <td>1</td>
      <td>0</td>
      <td>1</td>
    </tr>
    <tr>
      <th rowspan="2" valign="top">Yes</th>
      <th>Breakfast</th>
      <td>1</td>
      <td>0</td>
      <td>1</td>
    </tr>
    <tr>
      <th>Lunch</th>
      <td>1</td>
      <td>0</td>
      <td>1</td>
    </tr>
    <tr>
      <th>All</th>
      <th></th>
      <td>3</td>
      <td>3</td>
      <td>6</td>
    </tr>
  </tbody>
</table>
</div>


# 参考

1. Python for Data Analysis, 2nd Edition