/**
 * @file gtl_stack.h
 * @author gwq5210 (gwq5210@qq.com)
 * @brief stack的实现
 * @date 2021-05-16
 *
 * @copyright Copyright (c) 2021. All rights reserved.
 */

#pragma once

#include "gtl_deque.h"
#include "gtl_list.h"

namespace gtl {

template <typename T, typename Container = Deque<T>>
class Stack {
 public:
  using container_type = Container;
  using value_type = typename Container::value_type;
  using size_type = typename Container::size_type;
  using reference = typename Container::reference;
  using const_reference = typename Container::const_reference;

  template <typename T_, typename Container_>
  friend bool operator==(const Stack<T_, Container_>& lhs, const Stack<T_, Container_>& rhs);
  template <typename T_, typename Container_>
  friend bool operator<(const Stack<T_, Container_>& lhs, const Stack<T_, Container_>& rhs);

  Stack() = default;
  explicit Stack(const Container& c) : c_(c) {}
  explicit Stack(Container&& c) : c_(std::move(c)) {}
  template <typename InputIt, typename Category = typename std::iterator_traits<InputIt>::iterator_category>
  Stack(InputIt first, InputIt last) : c_(first, last) {}

  // Element access
  reference top() { return c_.back(); }
  const_reference top() const { return c_.back(); }

  // Capacity
  bool empty() const { return c_.empty(); }
  size_type size() const { return c_.size(); }

  // Modifiers
  void push(const T& v) { c_.push_back(v); }
  void push(T&& v) { c_.push_back(std::move(v)); }
  template <typename... Args>
  void emplace(Args&&... args) {
    c_.emplace_back(std::forward<Args>(args)...);
  }
  template <typename InputIt, typename Category = typename std::iterator_traits<InputIt>::iterator_category>
  void push(InputIt first, InputIt last) {
    for (; first != last; ++first) {
      emplace(*first);
    }
  }
  void pop() { c_.pop_back(); }
  void swap(Stack& other) { std::swap(c_, other.c_); }

 private:
  Container c_;
};  // class Stack

template <typename T, typename Container = List<T>>
using stack = Stack<T, Container>;

template <typename T, typename Container>
bool operator==(const Stack<T, Container>& lhs, const Stack<T, Container>& rhs) {
  return lhs.c_ == rhs.c_;
}

template <typename T, typename Container>
bool operator!=(const Stack<T, Container>& lhs, const Stack<T, Container>& rhs) {
  return !(lhs == rhs);
}

template <typename T, typename Container>
bool operator<(const Stack<T, Container>& lhs, const Stack<T, Container>& rhs) {
  return lhs.c_ < rhs.c_;
}

template <typename T, typename Container>
bool operator>(const Stack<T, Container>& lhs, const Stack<T, Container>& rhs) {
  return rhs < lhs;
}

template <typename T, typename Container>
bool operator<=(const Stack<T, Container>& lhs, const Stack<T, Container>& rhs) {
  return !(lhs > rhs);
}

template <typename T, typename Container>
bool operator>=(const Stack<T, Container>& lhs, const Stack<T, Container>& rhs) {
  return !(lhs < rhs);
}

}  // namespace gtl