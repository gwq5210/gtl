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

// 获得对象或函数 arg 的实际地址，即使存在 operator& 的重载
template <typename T>
std::enable_if<std::is_object_v<T>, T*> addressof(T& v) {
  return reinterpret_cast<T*>(&const_cast<char&>(reinterpret_cast<const volatile char&>(v)));
}

template <typename T>
std::enable_if<!std::is_object_v<T>, T*> addressof(T& v) {
  return &v;
}

// 右值重载被删除，以避免取 const 右值的地址
template <typename T>
const T* addressof(const T&& v) = delete;

// 若 T 不是数组类型，则调用 p 所指向对象的析构函数，如同用 p->~T()
// 若 T 是数组类型，则按顺序递归地销毁 *p 的元素，如同通过调用 std::destroy(std::begin(*p), std::end(*p))
// 
// template<class T> void destroy_at(T* p) { p->~T(); }

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
      destroy_at(addressof(v));
    }
  } else {
    p->~T();
  }
}

template <typename ForwardIt>
void destroy_range(ForwardIt first, ForwardIt last, std::forward_iterator_tag) {
  for (; first != last; ++first) {
    destroy_at(addressof(*first));
  }
}

template <typename ForwardIt, typename SizeType>
ForwardIt destroy_range_n(ForwardIt first, SizeType n, std::forward_iterator_tag) {
  for (; n > 0; --n, ++first) {
    destroy_at(addressof(*first));
  }
  return first;
}

/**
 * @brief 销毁范围 [first, last) 中的对象
 */
template <typename ForwardIt>
void destroy(ForwardIt first, ForwardIt last) {
  using iterator_category = std::iterator_traits<ForwardIt>::iterator_category;
  destroy_range(first, last, iterator_category());
}

/**
 * @brief 销毁从 first 开始的范围中的 n 个对象
 * 
 * @return ForwardIt 已被销毁的元素的范围结尾，即 std::next(first, n)
 */
template <typename ForwardIt, typename SizeType>
ForwardIt destroy(ForwardIt first, SizeType n) {
  using iterator_category = std::iterator_traits<ForwardIt>::iterator_category;
  return destroy_range_n(first, n, iterator_category());
}

/**
 * @brief 复制来自范围 [first, last) 的元素到始于 d_first 的未初始化内存
 *        若初始化中抛异常，则以未指定顺序销毁已构造的对象
 * 
 * @return ForwardIt 指向最后复制的元素后一元素的迭代器
 */
template <typename InputIt, typename ForwardIt>
ForwardIt uninitialized_copy(InputIt first, InputIt last, ForwardIt d_first) {
  static_assert(std::is_base_of_v<std::input_iterator_tag, std::iterator_traits<InputIt>::iterator_category>);
  static_assert(std::is_base_of_v<std::forward_iterator_tag, std::iterator_traits<ForwardIt>::iterator_category>);
  for (; first != last; ++first, ++d_first) {
    construct_at(addressof(*d_first), *first);
  }
  return d_first;
}

}  // namespace gtl