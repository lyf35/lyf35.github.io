---
title: 数据分析-datetime
tags: 
- 数据分析
categories: [工程实践, 数据分析]
sharing: True
date: 2020-12-22 19:11:20
updated: 2021-01-09 17:31:11
---



# datetime数据类型

# 简介

python自带的datetime模块包含了一些处理时间的数据类型和函数。其中，datetime模块中包含的数据类型如下：

- date：存储由年、月、日组成的日期
- time：存储由小时、分钟、秒、微秒组成的时间
- datetime：同时存储date和time
- timedelta：表示两个datetime之间的差值

<!--more-->

下面为一些使用示例：


```python
import datetime
```


```python
now=datetime.datetime.now() #获取这段代码运行时的时间
```


```python
now #获取的是一个datetime数据类型
```




    datetime.datetime(2021, 1, 9, 15, 47, 27, 207732)




```python
print(now.year, #查看datetime中的年
    now.month, #查看datetime中的月份
    now.day, #查看datetime中的日
    now.hour,#查看datetime中的小时
    now.minute, #查看datetime中的分钟
    now.second, #查看datetime中的秒
    now.microsecond) #查看datetime中的微秒
```

    2021 1 9 15 47 27 207732



```python
now.date() #查看datetime中的date部分
```




    datetime.date(2021, 1, 9)




```python
now.time() #查看datetime中的time部分
```




    datetime.time(15, 47, 27, 207732)




```python
delta=datetime.datetime(2011,1,7)-datetime.datetime(2008,6,24,8,15) #计算两个datetime的时间差
```


```python
print(delta.days, #时间差中的天数
    delta.seconds) #时间差中的秒数（不够一天）
```

    926 56700


## 字符串与datetime的转换

一个datetime类型的变量，可以通过str或者strftime函数将其转换为字符串，例如：


```python
str(now) #将datetime转换为字符串
```




    '2021-01-09 15:47:27.207732'




```python
now.strftime('%Y-%m-%d') #按照给定的格式，将datetime转换为字符串
```




    '2021-01-09'



在上述操作中，可以使用的格式化符号包括：
- %Y：4位数表示的年份
- %y：2位数表示的年份
- %m：2位数表示的月份（01至12）
- %d：2位数表示的日期（01至31）
- %H：24小时制小时（00至23）
- %I：12小时制小时（01至12）
- %M：2位数表示的分钟（00至59）
- %S：秒数（00至61，60和61为了表示时间调整时的重复计秒）
- %w：表示周日到周六（0至6，0为周日）
- %U：表示每一年的星期数（00至53，周日作为每周的第一天，在第一个周日之前的所有天数被认为是第0周）
- %W：与%U类似，但是将周一作为每周的第一天
- %z：UTC时区的偏移量，如果没有设置时区则为空
- %F：'%Y-%m-%d'格式的简写
- %D：'%m/%d/%y'格式的简写
- %a：简写的星期名称
- %A：全拼的星期名称
- %b：简写的月份名称
- %B：全拼的月份名称
- %c：输入全部的日期和时间，例如"Tue 01 May 2012 04:20:57 PM"
- %p：输出时间对应的是AM或PM
- %x：输出'month/day/year'格式的时间
- %X：输出'hour:min:sec'格式的时间

同时，datetime也提供了将字符串转换为datetime类型变量的函数，这一函数接收两个字符串作为参数，第一个字符串表示时间，第二个字符串表示第一个字符串对应的时间格式：


```python
datetime.datetime.strptime('2011-01-03', '%Y-%m-%d') #需要同时传入一个字符串并且传入字符串对应的时间格式
```




    datetime.datetime(2011, 1, 3, 0, 0)



也可以借助第三方库dateutil，完成字符串自动转换为datetime的操作。


```python
from dateutil.parser import parse
parse('Jan 31, 1997 10:45 PM')
```




    datetime.datetime(1997, 1, 31, 22, 45)



Pandas库中包含了一个to_datetime函数，可以将一个包含时间格式字符串的list转换为datetime格式的index：


```python
import numpy as np
import pandas as pd
pd.to_datetime(['7/6/2011', '8/6/2011'])
```




    DatetimeIndex(['2011-07-06', '2011-08-06'], dtype='datetime64[ns]', freq=None)



# 时间序列

## 简介

在Pandas的数据格式中，可以使用一系列字符串或datetime类型的变量，来生成一个TimeSeries类型的变量。例：


```python
ts = pd.Series(np.random.randn(6), index=[datetime.datetime(2011, 1, 2), datetime.datetime(2011, 1, 5), datetime.datetime(2011, 1, 7),datetime.datetime(2011, 2, 8), datetime.datetime(2011, 1, 10), datetime.datetime(2011, 2, 12)])
```


```python
ts
```




    2011-01-02   -1.516700
    2011-01-05   -0.509528
    2011-01-07    0.484152
    2011-02-08   -1.096136
    2011-01-10    0.721982
    2011-02-12   -0.789547
    dtype: float64




```python
ts.index #查看index的属性，可以看到此时为datetime类型的index
```




    DatetimeIndex(['2011-01-02', '2011-01-05', '2011-01-07', '2011-02-08',
                   '2011-01-10', '2011-02-12'],
                  dtype='datetime64[ns]', freq=None)



## index操作

如果一个DataFrame或Series使用了DatetimeIndex，那么可以使用一些特殊的index方式：


```python
ts['1/10/2011'] #传入一个可以转换为时间的字符串
```




    2011-01-10    0.721982
    dtype: float64




```python
ts['20110110'] #原理同上
```




    2011-01-10    0.721982
    dtype: float64




```python
ts['2011'] #使用年份进行索引
```




    2011-01-02   -1.516700
    2011-01-05   -0.509528
    2011-01-07    0.484152
    2011-02-08   -1.096136
    2011-01-10    0.721982
    2011-02-12   -0.789547
    dtype: float64




```python
ts['2011-01'] #使用年份和月份的结合进行索引
```




    2011-01-02   -1.516700
    2011-01-05   -0.509528
    2011-01-07    0.484152
    2011-01-10    0.721982
    dtype: float64




```python
ts['20110107':] #切片操作时也可以使用表示时间的字符串
```




    2011-01-07    0.484152
    2011-02-08   -1.096136
    2011-01-10    0.721982
    2011-02-12   -0.789547
    dtype: float64



## 时间序列的生成

Pandas中的函数data_range()函数可以生成一个时间序列。例：


```python
pd.date_range('4/1/2012', '6/1/2012') #传入两个时间，一个代表开始时间，另一个代表结束时间，默认的间隔为1天
```




    DatetimeIndex(['2012-04-01', '2012-04-02', '2012-04-03', '2012-04-04',
                   '2012-04-05', '2012-04-06', '2012-04-07', '2012-04-08',
                   '2012-04-09', '2012-04-10', '2012-04-11', '2012-04-12',
                   '2012-04-13', '2012-04-14', '2012-04-15', '2012-04-16',
                   '2012-04-17', '2012-04-18', '2012-04-19', '2012-04-20',
                   '2012-04-21', '2012-04-22', '2012-04-23', '2012-04-24',
                   '2012-04-25', '2012-04-26', '2012-04-27', '2012-04-28',
                   '2012-04-29', '2012-04-30', '2012-05-01', '2012-05-02',
                   '2012-05-03', '2012-05-04', '2012-05-05', '2012-05-06',
                   '2012-05-07', '2012-05-08', '2012-05-09', '2012-05-10',
                   '2012-05-11', '2012-05-12', '2012-05-13', '2012-05-14',
                   '2012-05-15', '2012-05-16', '2012-05-17', '2012-05-18',
                   '2012-05-19', '2012-05-20', '2012-05-21', '2012-05-22',
                   '2012-05-23', '2012-05-24', '2012-05-25', '2012-05-26',
                   '2012-05-27', '2012-05-28', '2012-05-29', '2012-05-30',
                   '2012-05-31', '2012-06-01'],
                  dtype='datetime64[ns]', freq='D')




```python
pd.date_range(start='4/1/2012', periods=20) #设置开始时间和天数
```




    DatetimeIndex(['2012-04-01', '2012-04-02', '2012-04-03', '2012-04-04',
                   '2012-04-05', '2012-04-06', '2012-04-07', '2012-04-08',
                   '2012-04-09', '2012-04-10', '2012-04-11', '2012-04-12',
                   '2012-04-13', '2012-04-14', '2012-04-15', '2012-04-16',
                   '2012-04-17', '2012-04-18', '2012-04-19', '2012-04-20'],
                  dtype='datetime64[ns]', freq='D')




```python
pd.date_range(end='6/1/2012', periods=20) #设置结束时间和天数
```




    DatetimeIndex(['2012-05-13', '2012-05-14', '2012-05-15', '2012-05-16',
                   '2012-05-17', '2012-05-18', '2012-05-19', '2012-05-20',
                   '2012-05-21', '2012-05-22', '2012-05-23', '2012-05-24',
                   '2012-05-25', '2012-05-26', '2012-05-27', '2012-05-28',
                   '2012-05-29', '2012-05-30', '2012-05-31', '2012-06-01'],
                  dtype='datetime64[ns]', freq='D')




```python
pd.date_range('1/1/2000', '1/3/2000 23:59', freq='4h') #通过设置freq参数表示时间序列的间隔
```




    DatetimeIndex(['2000-01-01 00:00:00', '2000-01-01 04:00:00',
                   '2000-01-01 08:00:00', '2000-01-01 12:00:00',
                   '2000-01-01 16:00:00', '2000-01-01 20:00:00',
                   '2000-01-02 00:00:00', '2000-01-02 04:00:00',
                   '2000-01-02 08:00:00', '2000-01-02 12:00:00',
                   '2000-01-02 16:00:00', '2000-01-02 20:00:00',
                   '2000-01-03 00:00:00', '2000-01-03 04:00:00',
                   '2000-01-03 08:00:00', '2000-01-03 12:00:00',
                   '2000-01-03 16:00:00', '2000-01-03 20:00:00'],
                  dtype='datetime64[ns]', freq='4H')



freq参数中可以使用的字母符号如下但不限于：

- D：日历天（包含周末）
- B：工作日（不含周末）
- H：每小时
- T or min：每分钟
- S：每秒钟
- L or ms：毫秒（1/1000秒）
- U：微秒
- M：每个月最后一天
- BM：每个月的最后一个工作日
- MS：每个月的第一天
- BMS：每个月的第一个工作日
- W-MON，W-TUE，……：每周的周几
- WOM-1MON，WOM-2MON，……：每个月的第几个周几
- ……

## 时移

shift函数可以将数据对应的时间上移或下移。例：


```python
ts.shift(2) #默认情况下保持index不变，将数据进行上下移动
```




    2011-01-02         NaN
    2011-01-05         NaN
    2011-01-07   -1.516700
    2011-02-08   -0.509528
    2011-01-10    0.484152
    2011-02-12   -1.096136
    dtype: float64




```python
ts.shift(2,freq='M') #传入freq参数，则可以对index进行修改，但是数据保持不变
```




    2011-02-28   -1.516700
    2011-02-28   -0.509528
    2011-02-28    0.484152
    2011-03-31   -1.096136
    2011-02-28    0.721982
    2011-03-31   -0.789547
    dtype: float64



Pandas中的时间也可以用offsets变量做修改：


```python
now+3*pd.tseries.offsets.Day()
```




    Timestamp('2021-01-12 15:47:27.207732')




```python
now+pd.tseries.offsets.MonthEnd(2)
```




    Timestamp('2021-02-28 15:47:27.207732')




```python
pd.tseries.offsets.MonthEnd(2).rollback(now) 
```




    Timestamp('2020-12-31 15:47:27.207732')



# 时区

Python的第三方库pytz包含了时区的数据库，可以用它来设置时间的时区。使用pytz中的timezone函数，可以生成一个表示时区的变量。


```python
import pytz
tz = pytz.timezone('US/Eastern')
```

在默认情况下，Pandas生成的时间序列是没有时区的。如果想设置时区，需要传入额外参数tz。例：


```python
pd.date_range('3/9/2012 9:30', periods=10, freq='D', tz='UTC')
```




    DatetimeIndex(['2012-03-09 09:30:00+00:00', '2012-03-10 09:30:00+00:00',
                   '2012-03-11 09:30:00+00:00', '2012-03-12 09:30:00+00:00',
                   '2012-03-13 09:30:00+00:00', '2012-03-14 09:30:00+00:00',
                   '2012-03-15 09:30:00+00:00', '2012-03-16 09:30:00+00:00',
                   '2012-03-17 09:30:00+00:00', '2012-03-18 09:30:00+00:00'],
                  dtype='datetime64[ns, UTC]', freq='D')



使用tz_localize函数可以将一个没有时区的时间序列转换为一个有时区的时间序列。这样，在使用tz_convert函数对这个时间序列转换时区的时候，则会指定UTC作为时区的基准。例：


```python
ts_utc = ts.tz_localize('UTC') #tz_localize中的时区可以任意设定，在设定完成之后便作为时区的基准
```


```python
ts_utc
```




    2011-01-02 00:00:00+00:00   -1.516700
    2011-01-05 00:00:00+00:00   -0.509528
    2011-01-07 00:00:00+00:00    0.484152
    2011-02-08 00:00:00+00:00   -1.096136
    2011-01-10 00:00:00+00:00    0.721982
    2011-02-12 00:00:00+00:00   -0.789547
    dtype: float64




```python
ts_utc.tz_convert('US/Eastern')
```




    2011-01-01 19:00:00-05:00   -1.516700
    2011-01-04 19:00:00-05:00   -0.509528
    2011-01-06 19:00:00-05:00    0.484152
    2011-02-07 19:00:00-05:00   -1.096136
    2011-01-09 19:00:00-05:00    0.721982
    2011-02-11 19:00:00-05:00   -0.789547
    dtype: float64



当创建一个timestamp类型变量的时候，可以直接传入时区。例：


```python
pd.Timestamp('2011-03-12 04:00', tz='Europe/Moscow')
```




    Timestamp('2011-03-12 04:00:00+0300', tz='Europe/Moscow')



如果两个不同时区的时间序列进行合并，那么合并的结果会被自动转换为UTC时区。例如：


```python
ts.tz_localize('Europe/London')+ts.tz_localize('Asia/Shanghai')
```




    2011-01-01 16:00:00+00:00   NaN
    2011-01-02 00:00:00+00:00   NaN
    2011-01-04 16:00:00+00:00   NaN
    2011-01-05 00:00:00+00:00   NaN
    2011-01-06 16:00:00+00:00   NaN
    2011-01-07 00:00:00+00:00   NaN
    2011-01-09 16:00:00+00:00   NaN
    2011-01-10 00:00:00+00:00   NaN
    2011-02-07 16:00:00+00:00   NaN
    2011-02-08 00:00:00+00:00   NaN
    2011-02-11 16:00:00+00:00   NaN
    2011-02-12 00:00:00+00:00   NaN
    dtype: float64



# 时间周期

## Period类和PeriodIndex类

Pandas中有一个period类，可以创建一个period类型的变量，对应一个时间跨度。例：


```python
p = pd.Period(2007, freq='A-DEC') #表示时间跨度为2017全年
```


```python
p
```




    Period('2007', 'A-DEC')




```python
p+5 #对一个period类型变量做加减法，会相应地在这个时间周期的基础上对时间进行调整
```




    Period('2012', 'A-DEC')




```python
pd.Period(2027, freq='A-DEC')-p
```




    <20 * YearEnds: month=12>



使用period_range函数，可以生成一个PeriodIndex类型的变量，是一组具有规则的时间序列：


```python
rng = pd.period_range('1/1/2000', '6/30/2000', freq='M')
```


```python
rng
```




    PeriodIndex(['2000-01', '2000-02', '2000-03', '2000-04', '2000-05', '2000-06'], dtype='period[M]', freq='M')




```python
pd.Series(np.random.randn(6), index=rng)
```




    2000-01    1.155621
    2000-02   -0.949274
    2000-03    1.474354
    2000-04    1.653150
    2000-05    0.976651
    2000-06    1.849326
    Freq: M, dtype: float64




```python
pd.PeriodIndex(['2001Q3', '2002Q2', '2003Q1'], freq='Q-DEC') #用字符串表示的时间序列也可以用于构造periodindex类型的变量
```




    PeriodIndex(['2001Q3', '2002Q2', '2003Q1'], dtype='period[Q-DEC]', freq='Q-DEC')



## Period和PeriodIndex时间频率的转换

使用asfreq函数，可以对一个Period或PeriodIndex类型的变量转换时间频率。


```python
p = pd.Period('2007', freq='A-DEC')
```


```python
p
```




    Period('2007', 'A-DEC')




```python
p.asfreq('M',how='start') #将时间跨度变小
```




    Period('2007-01', 'M')




```python
p.asfreq('A-JUN') #将时间跨度变大，之前的那个时间包含在这个更大的时间跨度内
```




    Period('2008', 'A-JUN')



## 时间序列与时间周期的互相转换

使用to_period函数，可以把一个时间序列timestamp转换为一个时间周期PeriodIndex。在转换时，会将原来时间序列中的时间自动划分到它所在的那个时间周期内。例如：


```python
rng = pd.date_range('1/1/2000', periods=3, freq='M')
ts = pd.Series(np.random.randn(3), index=rng)
pts = ts.to_period() #如果没有使用额外参数，则自动选取合适的时间周期
```


```python
pts
```




    2000-01    1.739193
    2000-02   -0.201334
    2000-03   -0.076694
    Freq: M, dtype: float64




```python
pts2=ts.to_period('D')
pts2
```




    2000-01-31    1.739193
    2000-02-29   -0.201334
    2000-03-31   -0.076694
    Freq: D, dtype: float64



使用to_timestamp()函数，可以将PeriodIndex转换回timestamp。例：


```python
pts.to_timestamp(how='end')
```




    2000-01-31 23:59:59.999999999    1.739193
    2000-02-29 23:59:59.999999999   -0.201334
    2000-03-31 23:59:59.999999999   -0.076694
    Freq: M, dtype: float64



# 时间序列相关的统计

以时间序列作为index的series或dataframe变量，可以调用pandas中一些相关的统计函数做一些相关的统计数据。

可以使用的相关函数包括：

- rolling_count：返回移动窗口的非NaN数目
- rolling_sum：计算移动窗口内的和
- rolling_mean：计算移动窗口内的平均值
- rolling_median：计算移动窗口内的中位数
- rolling_var, rolling_std：计算移动窗口内的方差、标准差
- rolling_skew, rolling_kurt：计算移动窗口内的偏度、峰度
- rolling_min, rolling_max：计算移动窗口内的最值
- rolling_quantile：计算移动窗口内的某个百分位数
- rolling_corr, rolling_cov：计算移动窗口内的协方差和相关系数
- rolling_apply：将移动窗口内的数值用到某个函数内
- ewma：指数权重的移动平均
- ewmvar, ewmstd：指数权重的移动方差、标准差
- ewmcorr, ewmcov：指数权重的相关系数、协方差

同时，对于一个以时间序列为index的series或dataframe，可以对其调用plot函数来做图。横坐标会被自动设置为时间序列中的值。



# 参考

1. Python for Data Analysis, 2nd Edition.