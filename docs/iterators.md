# 迭代器

迭代器模式定义：提供一种方法，使之能够依次访问某个容器所含的各个元素，而又无需暴露该容器的内部实现。

每种容器提供自己的迭代器实现，迭代器的行为与指针类似，迭代器可以是原生指针，也可以是重载operator*和operator->的对象。

## 迭代器的类型

与容器相似，该容器对应的迭代器可以获取特定类型的值，我们需要通过特性萃取技术（traits）来获取迭代器对应的类型。c++提供了iterator_traits来实现该功能。

一般来说，迭代器需要提供以下几种类型，针对原生指针，可以通过对iterator_traits进行模板特化来获取对应的类型
```
value_type
difference_type
pointer
reference
iterator_category
```

## 迭代器分类

struct input_iterator_tag { }; // 输入迭代器，只读，只能++
struct output_iterator_tag { }; // 输出迭代器，可写，只能++
struct forward_iterator_tag : public input_iterator_tag { }; // 前向迭代器，只能向前移动，只能++
struct bidirectional_iterator_tag : public forward_iterator_tag { }; // 可双向移动，可++，--
struct random_access_iterator_tag : public bidirectional_iterator_tag { }; // 可随机访问，与原生指针类似
struct contiguous_iterator_tag: public random_access_iterator_tag { }; //  (C++20) 要求逻辑相邻元素在内存中物理上也相邻

