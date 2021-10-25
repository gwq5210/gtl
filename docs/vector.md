- [vector概述](#vector概述)
- [vector的实现](#vector的实现)
  - [数据结构](#数据结构)
  - [空间配置策略](#空间配置策略)
  - [内存的释放](#内存的释放)
- [vector的迭代器失效](#vector的迭代器失效)
- [vector原位构造元素](#vector原位构造元素)
- [vector的特化](#vector的特化)
- [参考](#参考)

# vector概述

vector是动态连续数组，这不仅意味着vector中的元素在逻辑上相邻，而且也意味着它们在物理上也是相邻的

vector的底层是数组，这也说明必要的时候，vector的元素指针可以当做数组指针来使用

一般的实现中，vector的迭代器就是指针本身

vector元素访问复杂度

- 随机访问——常数 O(1)
- 在末尾插入或移除元素——均摊常数 O(1)
- 插入或移除元素——与到 vector 结尾的距离成线性 O(n)

vector有两个特点

- 数据连续存储
- 空间动态分配

下边主要针对空间动态分配进行介绍

# vector的实现

vector的空间是动态分配的，随着元素的增加，vector会自动扩充空间以容纳新元素

当需要扩充空间时，vector内部会进行如下操作

- 分配一块更大的空间
- 将原有的数据移动到新的位置
- 释放原有空间

可以看到自动扩充空间涉及到内存分配和元素移动，时间成本很高，需要设计一个合理的数据结构和空间配置策略

## 数据结构

vector需要表示一块连续存储空间，因此我们需要知道内存的位置，内存的大小和已存储元素的个数

一个简单的结构如下

```cpp
template<class T>
class vector {
private:
    T* start_;
    size_t size_;
    size_t capacity_;
};
```

为了降低内存分配的时间成本，vector的实际空间要比使用的空间要大一些，为将来的元素增加做好准备，为此就需要一个容量来表示当前内存的最大大小

## 空间配置策略

当vector空间满时，新添加元素，会触发自动空间分配，这个时候再分配多少合适呢

选的扩容太小比如新的大小是当前大小的1.1倍，可能会频繁导致空间分配，如果新的大小是当前大小的3或4倍，可能会导致内存的浪费

我们先看下主流的实现，gcc新的大小是当前空间大小的2倍，vs是新大小是当前空间大小的1.5倍

如果选择增长因子为2，可能是最坏的可能，因为它不允许vector重用先前分配好的任何内存，因此选择1.5是一个更好的选择

如果增长因子为k，则每次分配的内存为，当为2的时候每次分配的内存都比先前分配的所有内存和都大，选择1.5时，则可以在4次分配后重用

```shell
C, C*k,  C*k^2, C*k^3, ...
1 + 2^1 + 2^2 + 2^3... + 2^n = 2^(n+1) - 1
```

## 内存的释放

vector删除元素并不会删除已经申请好的内存，即使vector里已经不包含任何元素

C++11增加公共成员函数shrink_to_fit，允许释放未使用的内存

或者通过swap临时变量的方式释放内存

```cpp
vector<int>().swap(v);
```

# vector的迭代器失效

对容器的操作影响了元素的存放位置，称为迭代器失效

由于vector的实现特点，在发生以下情况时，迭代器和引用都会失效

- 容量发生改变
- 插入或移除元素后的迭代器(插入或移除元素前的迭代器和引用不会失效)

# vector原位构造元素

C++11提供以下两个函数来原位构造元素

- emplace
- emplace_back

这两个函数通过完美转发实现原位构造，相比push_back这一类函数，少了一次拷贝的动作

实际上，当前的push_back实现会使用move语义，不会进行拷贝

# vector的特化

STL针对vector<bool>进行了特化，可以提供更优的存储空间，实际上它实现了一个动态bitmap

# 参考

- [容器库](https://zh.cppreference.com/w/cpp/container/vector)
- [STL源码剖析](https://item.jd.com/11821611.html)
- [fbvector](https://github.com/facebook/folly/blob/main/folly/docs/FBVector.md)
- [vector实现原理](https://segmentfault.com/a/1190000040103598)