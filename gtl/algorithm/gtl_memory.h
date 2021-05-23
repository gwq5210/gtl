/**
 * @file gtl_memory.h
 * @author gwq5210 (gwq5210@qq.com)
 * @brief 内存管理相关的函数库
 * @date 2021-05-15
 *
 * @copyright Copyright (c) 2021. All rights reserved.
 */

#pragma once

namespace gtl {

// 在给定地址 p 创建以参数 args... 初始化的 T 对象
template <typename T, typename... Args>
T* construct_at(T* p, Args&&... args) {
  ::new (const_cast<void*>(static_cast<const volatile void*>(p))) T(std::forward<Args>(args)...);
  return p;
}

// 在给定地址 p 创建默认初始化(default-initialization)的 T 对象
template <typename T>
T* default_construct_at(T* p) {
  ::new (const_cast<void*>(static_cast<const volatile void*>(p))) T;
  return p;
}

// 获得对象或函数 arg 的实际地址，即使存在 operator& 的重载
template <typename T>
std::enable_if_t<std::is_object_v<T>, T*> addressof(T& v) {
  return reinterpret_cast<T*>(&const_cast<char&>(reinterpret_cast<const volatile char&>(v)));
}

template <typename T>
std::enable_if_t<!std::is_object_v<T>, T*> addressof(T& v) {
  return &v;
}

// 右值重载被删除，以避免取 const 右值的地址
template <typename T>
const T* addressof(const T&& v) = delete;

/**
 * @brief 销毁指针p指向的对象
 * 若 T 不是数组类型，则调用 p 所指向对象的析构函数，如同用 p->~T()
 * 若 T 是数组类型，则按顺序递归地销毁 *p 的元素，如同通过调用 std::destroy(std::begin(*p), std::end(*p))
 * C++17 version:
 * template<class T> void destroy_at(T* p) { p->~T(); }
 *
 * @param p 指向要被销毁的对象的指针
 */
template <typename T>
void destroy_at(T* p) {
  if constexpr (std::is_array_v<T>) {  // C++ 20
    for (auto& v : *p) {
      gtl::destroy_at(gtl::addressof(v));
    }
  } else {
    p->~T();
  }
}

template <typename ForwardIt>
void destroy_range(ForwardIt first, ForwardIt last, std::forward_iterator_tag) {
  for (; first != last; ++first) {
    gtl::destroy_at(gtl::addressof(*first));
  }
}

template <typename ForwardIt, typename SizeType>
ForwardIt destroy_range_n(ForwardIt first, SizeType n, std::forward_iterator_tag) {
  for (; n > 0; --n, ++first) {
    gtl::destroy_at(gtl::addressof(*first));
  }
  return first;
}

/**
 * @brief 销毁范围 [first, last) 中的对象
 */
template <typename ForwardIt>
void destroy(ForwardIt first, ForwardIt last) {
  using iterator_category = std::iterator_traits<ForwardIt>::iterator_category;
  gtl::destroy_range(first, last, iterator_category());
}

/**
 * @brief 销毁从 first 开始的范围中的 n 个对象
 *
 * @return ForwardIt 已被销毁的元素的范围结尾，即 std::next(first, n)
 */
template <typename ForwardIt, typename SizeType>
ForwardIt destroy_n(ForwardIt first, SizeType n) {
  using iterator_category = std::iterator_traits<ForwardIt>::iterator_category;
  return destroy_range_n(first, n, iterator_category());
}

template <typename InputIt, typename ForwardIt>
ForwardIt uninitialized_copy_range(InputIt first, InputIt last, ForwardIt d_first, std::input_iterator_tag,
                                   std::forward_iterator_tag) {
  for (; first != last; ++first, ++d_first) {
    gtl::construct_at(gtl::addressof(*d_first), *first);
  }
  return d_first;
}

template <typename InputIt, typename SizeType, typename ForwardIt>
ForwardIt uninitialized_copy_range_n(InputIt first, SizeType count, ForwardIt d_first, std::input_iterator_tag,
                                     std::forward_iterator_tag) {
  for (; count > 0; ++first, ++d_first, --count) {
    gtl::construct_at(gtl::addressof(*d_first), *first);
  }
  return d_first;
}

/**
 * @brief 复制来自范围 [first, last) 的元素到始于 d_first 的未初始化内存
 * 若初始化中抛异常，则以未指定顺序销毁已构造的对象
 *
 * @return ForwardIt 指向最后复制的元素后一元素的迭代器
 *
 * TODO: 处理异常的情况
 */
template <typename InputIt, typename ForwardIt>
ForwardIt uninitialized_copy(InputIt first, InputIt last, ForwardIt d_first) {
  using input_iterator_category = std::iterator_traits<InputIt>::iterator_category;
  using forward_iterator_category = std::iterator_traits<ForwardIt>::iterator_category;
  return gtl::uninitialized_copy_range(first, last, d_first, input_iterator_category(), forward_iterator_category());
}

/**
 * @brief 从始于 first 的范围复制 count 个元素到始于 d_first 的未初始化内存区域
 * 若初始化期间抛异常，则以未指定顺序销毁已构造的对象
 *
 * @return ForwardIt 指向最后复制的元素后一元素的迭代器
 *
 * TODO: 处理异常的情况
 */
template <typename InputIt, typename SizeType, typename ForwardIt>
ForwardIt uninitialized_copy_n(InputIt first, SizeType count, ForwardIt d_first) {
  using input_iterator_category = std::iterator_traits<InputIt>::iterator_category;
  using forward_iterator_category = std::iterator_traits<ForwardIt>::iterator_category;
  return gtl::uninitialized_copy_range_n(first, count, d_first, input_iterator_category(), forward_iterator_category());
}

template <typename ForwardIt, typename T>
void uninitialized_fill_range(ForwardIt first, ForwardIt last, const T& v, std::forward_iterator_tag) {
  for (; first != last; ++first) {
    gtl::construct_at(gtl::addressof(*first), v);
  }
}

template <typename ForwardIt, typename SizeType, typename T>
void uninitialized_fill_range_n(ForwardIt first, SizeType count, const T& v, std::forward_iterator_tag) {
  for (; count > 0; ++first, --count) {
    gtl::construct_at(gtl::addressof(*first), v);
  }
}

/**
 * @brief 复制给定的 value 到以 [first, last) 定义的未初始化内存区域
 * 若初始化期间抛异常，则以未指定顺序销毁已构造的对象
 * 
 * TODO: 处理异常的情况
 */
template <typename ForwardIt, typename T>
void uninitialized_fill(ForwardIt first, ForwardIt last, const T& v) {
  using iterator_category = std::iterator_traits<ForwardIt>::iterator_category;
  gtl::uninitialized_fill_range(first, last, v, iterator_category());
}

/**
 * @brief 复制给定值 value 到始于 first 的未初始化内存区域的首 count 个元素
 * 若初始化期间抛异常，则以未指定顺序销毁已构造的对象
 * 
 * TODO: 处理异常的情况
 */
template <typename ForwardIt, typename SizeType, typename T>
void uninitialized_fill_n(ForwardIt first, SizeType count, const T& v) {
  using iterator_category = std::iterator_traits<ForwardIt>::iterator_category;
  gtl::uninitialized_fill_range_n(first, count, v, iterator_category());
}

template <typename InputIt, typename ForwardIt>
ForwardIt uninitialized_move_range(InputIt first, InputIt last, ForwardIt d_first, std::input_iterator_tag,
                                   std::forward_iterator_tag) {
  for (; first != last; ++first, ++d_first) {
    gtl::construct_at(gtl::addressof(*d_first), std::move(*first));
  }
  return d_first;
}

template <typename InputIt, typename SizeType, typename ForwardIt>
ForwardIt uninitialized_move_range_n(InputIt first, SizeType count, ForwardIt d_first, std::input_iterator_tag,
                                     std::forward_iterator_tag) {
  for (; count > 0; ++first, ++d_first, --count) {
    gtl::construct_at(gtl::addressof(*d_first), std::move(*first));
  }
  return d_first;
}

/**
 * @brief 从范围 [first, last) 移动元素到始于 d_first 的未初始化内存区域
 * 若此初始化抛出异常，则 [first, last) 中的某些对象留在合法但未指定的状态，并以未指定顺序销毁已构造的对象
 *
 * @return ForwardIt 指向最后被移动元素的后一元素的迭代器
 *
 * TODO: 处理异常的情况
 */
template <typename InputIt, typename ForwardIt>
ForwardIt uninitialized_move(InputIt first, InputIt last, ForwardIt d_first) {
  using input_iterator_category = std::iterator_traits<InputIt>::iterator_category;
  using forward_iterator_category = std::iterator_traits<ForwardIt>::iterator_category;
  return gtl::uninitialized_move_range(first, last, d_first, input_iterator_category(), forward_iterator_category());
}

/**
 * @brief 从始于 first 的范围移动 count 到始于 d_first 的未初始化内存区域
 * 若此初始化抛出异常，则 [first, last) 中的某些对象留在合法但未指定的状态，并以未指定顺序销毁已构造的对象
 *
 * @return ForwardIt 指向最后被移动元素的后一元素的迭代器
 *
 * TODO: 处理异常的情况
 */
template <typename InputIt, typename SizeType, typename ForwardIt>
ForwardIt uninitialized_move_n(InputIt first, SizeType count, ForwardIt d_first) {
  using input_iterator_category = std::iterator_traits<InputIt>::iterator_category;
  using forward_iterator_category = std::iterator_traits<ForwardIt>::iterator_category;
  return gtl::uninitialized_move_range_n(first, count, d_first, input_iterator_category(), forward_iterator_category());
}

template <typename ForwardIt>
void uninitialized_default_construct_range(ForwardIt first, ForwardIt last, std::forward_iterator_tag) {
  for (; first != last; ++first) {
    gtl::default_construct_at(gtl::addressof(*first));
  }
}

template <typename ForwardIt, typename SizeType>
void uninitialized_default_construct_range_n(ForwardIt first, SizeType count, std::forward_iterator_tag) {
  for (; count > 0; ++first, --count) {
    gtl::default_construct_at(gtl::addressof(*first));
  }
}

/**
 * @brief 以默认初始化(default-initialization)在范围 [first, last) 所指代的未初始化存储上构造对象
 * 若初始化期间抛异常，则以未指定顺序销毁已构造的对象
 * 
 * TODO: 处理异常的情况
 */
template <typename ForwardIt>
void uninitialized_default_construct(ForwardIt first, ForwardIt last) {
  using iterator_category = std::iterator_traits<ForwardIt>::iterator_category;
  gtl::uninitialized_fill_range(first, last, iterator_category());
}

/**
 * @brief 在 first 起始的存储中以默认初始化(default-initialization)构造 count 个对象
 * 若初始化期间抛异常，则以未指定顺序销毁已构造的对象
 * 
 * TODO: 处理异常的情况
 */
template <typename ForwardIt, typename SizeType>
void uninitialized_default_construct_n(ForwardIt first, SizeType count) {
  using iterator_category = std::iterator_traits<ForwardIt>::iterator_category;
  gtl::uninitialized_default_construct_range_n(first, count, iterator_category());
}

template <typename ForwardIt>
void uninitialized_value_construct_range(ForwardIt first, ForwardIt last, std::forward_iterator_tag) {
  for (; first != last; ++first) {
    gtl::construct_at(gtl::addressof(*first));
  }
}

template <typename ForwardIt, typename SizeType>
void uninitialized_value_construct_range_n(ForwardIt first, SizeType count, std::forward_iterator_tag) {
  for (; count > 0; ++first, --count) {
    gtl::construct_at(gtl::addressof(*first));
  }
}

/**
 * @brief 以值初始化(value-initialization)在范围 [first, last) 所指代的未初始化存储上构造对象
 * 若初始化期间抛异常，则以未指定顺序销毁已构造的对象
 * 
 * TODO: 处理异常的情况
 */
template <typename ForwardIt>
void uninitialized_value_construct(ForwardIt first, ForwardIt last) {
  using iterator_category = std::iterator_traits<ForwardIt>::iterator_category;
  gtl::uninitialized_fill_range(first, last, iterator_category());
}

/**
 * @brief 在 first 起始的存储中以值初始化(value-initialization)构造 count 个对象
 * 若初始化期间抛异常，则以未指定顺序销毁已构造的对象
 * 
 * TODO: 处理异常的情况
 */
template <typename ForwardIt, typename SizeType>
void uninitialized_value_construct_n(ForwardIt first, SizeType count) {
  using iterator_category = std::iterator_traits<ForwardIt>::iterator_category;
  gtl::uninitialized_value_construct_range_n(first, count, iterator_category());
}

}  // namespace gtl