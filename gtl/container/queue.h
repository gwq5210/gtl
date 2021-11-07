/**
 * @file queue.h
 * @author gwq5210 (gwq5210@qq.com)
 * @brief queue的实现
 * @date 2021-05-16
 *
 * @copyright Copyright (c) 2021. All rights reserved.
 */

#pragma once

#include "gtl/deque.h"
#include "gtl/list.h"

namespace gtl {

template <typename T, typename Container = Deque<T>>
class Queue {
 public:
  using container_type = Container;
  using value_type = typename Container::value_type;
  using size_type = typename Container::size_type;
  using reference = typename Container::reference;
  using const_reference = typename Container::const_reference;

  template <typename T_, typename Container_>
  friend bool operator==(const Queue<T_, Container_>& lhs, const Queue<T_, Container_>& rhs);
  template <typename T_, typename Container_>
  friend bool operator<(const Queue<T_, Container_>& lhs, const Queue<T_, Container_>& rhs);

  Queue() = default;
  explicit Queue(const Container& c) : c_(c) {}
  explicit Queue(Container&& c) : c_(std::move(c)) {}
  template <typename InputIt, typename Category = typename std::iterator_traits<InputIt>::iterator_category>
  Queue(InputIt first, InputIt last) {
    push(first, last);
  }
  ~Queue() = default;

  // Element access
  reference front() { return c_.front(); }
  const_reference front() const { return c_.front(); }
  reference back() { return c_.back(); }
  const_reference back() const { return c_.back(); }

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
  void pop() { c_.pop_front(); }
  void swap(Queue& other) { std::swap(c_, other.c_); }

 private:
  Container c_;
};  // class Queue

template <typename T, typename Container = List<T>>
using queue = Queue<T, Container>;

template <typename T, typename Container>
bool operator==(const Queue<T, Container>& lhs, const Queue<T, Container>& rhs) {
  return lhs.c_ == rhs.c_;
}

template <typename T, typename Container>
bool operator!=(const Queue<T, Container>& lhs, const Queue<T, Container>& rhs) {
  return !(lhs == rhs);
}

template <typename T, typename Container>
bool operator<(const Queue<T, Container>& lhs, const Queue<T, Container>& rhs) {
  return lhs.c_ < rhs.c_;
}

template <typename T, typename Container>
bool operator>(const Queue<T, Container>& lhs, const Queue<T, Container>& rhs) {
  return rhs < lhs;
}

template <typename T, typename Container>
bool operator<=(const Queue<T, Container>& lhs, const Queue<T, Container>& rhs) {
  return !(lhs > rhs);
}

template <typename T, typename Container>
bool operator>=(const Queue<T, Container>& lhs, const Queue<T, Container>& rhs) {
  return !(lhs < rhs);
}

}  // namespace gtl