/**
 * @file priority_queue.h
 * @author gwq5210 (gwq5210@qq.com)
 * @brief priority_queue的实现
 * @date 2021-05-17
 *
 * @copyright Copyright (c) 2021. All rights reserved.
 */

#pragma once

#include "gtl/algorithm.h"
#include "gtl/vector.h"

namespace gtl {

template <typename T, typename Compare = std::less<T>, typename Container = Vector<T>>
class PriorityQueue {
 public:
  using container_type = Container;
  using value_type = typename Container::value_type;
  using size_type = typename Container::size_type;
  using reference = typename Container::reference;
  using const_reference = typename Container::const_reference;

  template <typename T_, typename Compare_, typename Container_>
  friend bool operator==(const PriorityQueue<T_, Compare_, Container_>& lhs,
                         const PriorityQueue<T_, Compare_, Container_>& rhs);
  template <typename T_, typename Compare_, typename Container_>
  friend bool operator<(const PriorityQueue<T_, Compare_, Container_>& lhs,
                        const PriorityQueue<T_, Compare_, Container_>& rhs);

  PriorityQueue() = default;
  explicit PriorityQueue(const Container& c) : c_(c) { gtl::make_heap(c_.begin(), c_.end(), compare_); }
  explicit PriorityQueue(Container&& c) : c_(std::move(c)) { gtl::make_heap(c_.begin(), c_.end(), compare_); }
  template <typename InputIt, typename Category = typename std::iterator_traits<InputIt>::iterator_category>
  PriorityQueue(InputIt first, InputIt last) : c_(first, last) {
    gtl::make_heap(c_.begin(), c_.end(), compare_);
  }

  // Element access
  const_reference top() const { return c_.front(); }

  // Capacity
  bool empty() const { return c_.empty(); }
  size_type size() const { return c_.size(); }

  // Modifiers
  void push(const T& v) {
    c_.push_back(v);
    gtl::push_heap(c_.begin(), c_.end(), compare_);
  }
  void push(T&& v) {
    c_.push_back(std::move(v));
    gtl::push_heap(c_.begin(), c_.end(), compare_);
  }
  template <typename... Args>
  void emplace(Args&&... args) {
    c_.emplace_back(std::forward<Args>(args)...);
    gtl::push_heap(c_.begin(), c_.end(), compare_);
  }
  template <typename InputIt, typename Category = typename std::iterator_traits<InputIt>::iterator_category>
  void push(InputIt first, InputIt last) {
    for (; first != last; ++first) {
      emplace(*first);
    }
  }
  void pop() {
    gtl::pop_heap(c_.begin(), c_.end());
    c_.pop_back();
  }
  void swap(PriorityQueue& other) { std::swap(c_, other.c_); }

 private:
  Container c_;
  Compare compare_;
};  // class PriorityQueue

template <typename T, typename Compare = std::less<T>, typename Container = Vector<T>>
using priority_queue = PriorityQueue<T, Compare, Container>;

template <typename T, typename Compare, typename Container>
inline bool operator==(const PriorityQueue<T, Compare, Container>& lhs,
                       const PriorityQueue<T, Compare, Container>& rhs) {
  return lhs.c_ == rhs.c_;
}

template <typename T, typename Compare, typename Container>
inline bool operator!=(const PriorityQueue<T, Compare, Container>& lhs,
                       const PriorityQueue<T, Compare, Container>& rhs) {
  return !(lhs == rhs);
}

template <typename T, typename Compare, typename Container>
inline bool operator<(const PriorityQueue<T, Compare, Container>& lhs,
                      const PriorityQueue<T, Compare, Container>& rhs) {
  return lhs.c_ < rhs.c_;
}

template <typename T, typename Compare, typename Container>
inline bool operator>(const PriorityQueue<T, Compare, Container>& lhs,
                      const PriorityQueue<T, Compare, Container>& rhs) {
  return rhs < lhs;
}

template <typename T, typename Compare, typename Container>
inline bool operator<=(const PriorityQueue<T, Compare, Container>& lhs,
                       const PriorityQueue<T, Compare, Container>& rhs) {
  return !(lhs > rhs);
}

template <typename T, typename Compare, typename Container>
inline bool operator>=(const PriorityQueue<T, Compare, Container>& lhs,
                       const PriorityQueue<T, Compare, Container>& rhs) {
  return !(lhs < rhs);
}

}  // namespace gtl