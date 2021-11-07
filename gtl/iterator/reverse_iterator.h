/**
 * @file iterator_adaptors.h
 * @author gwq5210 (gwq5210@qq.com)
 * @brief 逆向迭代器适配器实现
 * @date 2021-05-28
 *
 * @copyright Copyright (c) 2021. All rights reserved.
 */

#pragma once

#include <iterator>

#include "gtl/iterator_op.h"
#include "gtl/memory_op.h"

namespace gtl {

template <typename Iterator>
class ReverseIterator {
 public:
  using iterator_type = Iterator;
  using iterator_category = typename std::iterator_traits<Iterator>::iterator_category;
  using value_type = typename std::iterator_traits<Iterator>::value_type;
  using difference_type = typename std::iterator_traits<Iterator>::difference_type;
  using pointer = typename std::iterator_traits<Iterator>::pointer;
  using reference = typename std::iterator_traits<Iterator>::reference;

  ReverseIterator() : it_() {}
  ReverseIterator(iterator_type it) : it_(it) {}
  template <typename It>
  ReverseIterator(const ReverseIterator<It>& other) : it_(other.it_) {}

  template <typename It>
  ReverseIterator& operator=(const ReverseIterator<It>& other) {
    it_ = other.it_;
  }

  iterator_type base() const { return it_; }

  reference operator*() {
    return *gtl::prev(it_);
  }
  pointer operator->() { return gtl::addressof(**this); }
  reference operator[](difference_type n) { return *gtl::prev(n + 1); }

  ReverseIterator& operator++() {
    --it_;
    return *this;
  }
  ReverseIterator operator++(int) {
    auto ret = *this;
    --it_;
    return ret;
  }
  ReverseIterator& operator--() {
    ++it_;
    return *this;
  }
  ReverseIterator operator--(int) {
    auto ret = *this;
    ++it_;
    return ret;
  }
  ReverseIterator& operator+=(difference_type n) {
    it_ -= n;
    return *this;
  }
  ReverseIterator& operator-=(difference_type n) {
    it_ += n;
    return *this;
  }
  ReverseIterator operator+(difference_type n) const { return ReverseIterator(it_ - n); }
  ReverseIterator operator-(difference_type n) const { return ReverseIterator(it_ + n); }

  template <typename It>
  friend bool operator==(const ReverseIterator<It>& lhs, const ReverseIterator<It>& rhs);
  template <typename It>
  friend bool operator<(const ReverseIterator<It>& lhs, const ReverseIterator<It>& rhs);

 private:
  iterator_type it_;
};

template <typename Iterator>
using reverse_iterator = ReverseIterator<Iterator>;

template <typename Iterator>
ReverseIterator<Iterator> make_reverse_iterator(Iterator i) {
  return ReverseIterator<Iterator>(i);
}

template <typename Iterator>
ReverseIterator<Iterator> operator+(typename ReverseIterator<Iterator>::difference_type n,
                                    const ReverseIterator<Iterator>& it) {
  return it + n;
}

template <typename Iterator>
ReverseIterator<Iterator> operator-(typename ReverseIterator<Iterator>::difference_type n,
                                    const ReverseIterator<Iterator>& it) {
  return it - n;
}

template <typename Iterator>
bool operator==(const ReverseIterator<Iterator>& lhs, const ReverseIterator<Iterator>& rhs) {
  return lhs.it_ == rhs.it_;
}

template <typename Iterator>
bool operator!=(const ReverseIterator<Iterator>& lhs, const ReverseIterator<Iterator>& rhs) {
  return !(lhs == rhs);
}

template <typename Iterator>
bool operator<(const ReverseIterator<Iterator>& lhs, const ReverseIterator<Iterator>& rhs) {
  return lhs.it_ < rhs.it_;
}

template <typename Iterator>
bool operator>(const ReverseIterator<Iterator>& lhs, const ReverseIterator<Iterator>& rhs) {
  return rhs < lhs;
}

template <typename Iterator>
bool operator<=(const ReverseIterator<Iterator>& lhs, const ReverseIterator<Iterator>& rhs) {
  return !(lhs > rhs);
}

template <typename Iterator>
bool operator>=(const ReverseIterator<Iterator>& lhs, const ReverseIterator<Iterator>& rhs) {
  return !(lhs < rhs);
}

}  // namespace gtl