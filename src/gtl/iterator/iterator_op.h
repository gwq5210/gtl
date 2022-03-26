/**
 * @file iterator_op.h
 * @author gwq5210 (gwq5210@qq.com)
 * @brief 迭代器操作库
 * @date 2021-05-28
 *
 * @copyright Copyright (c) 2021. All rights reserved.
 */

#pragma once

#include <iterator>

namespace gtl {

namespace detail {

template <typename Iterator>
void do_advance(Iterator& it, typename std::iterator_traits<Iterator>::difference_type n, std::input_iterator_tag) {
  while (n > 0) {
    ++it;
    --n;
  }
}

template <typename Iterator>
void do_advance(Iterator& it, typename std::iterator_traits<Iterator>::difference_type n,
                std::bidirectional_iterator_tag) {
  while (n > 0) {
    ++it;
    --n;
  }
  while (n < 0) {
    --it;
    ++n;
  }
}

template <typename Iterator>
void do_advance(Iterator& it, typename std::iterator_traits<Iterator>::difference_type n,
                std::random_access_iterator_tag) {
  it += n;
}

template <typename Iterator>
typename std::iterator_traits<Iterator>::difference_type do_distance(Iterator first, Iterator last,
                                                                     std::input_iterator_tag) {
  typename std::iterator_traits<Iterator>::difference_type n = 0;
  while (first != last) {
    ++first;
    ++n;
  }
  return n;
}

template <typename Iterator>
typename std::iterator_traits<Iterator>::difference_type do_distance(Iterator first, Iterator last,
                                                                     std::random_access_iterator_tag) {
  return last - first;
}

}  // namespace detail

template <typename InputIt, typename Distance>
void advance(InputIt& it, Distance n) {
  using difference_type = typename std::iterator_traits<InputIt>::difference_type;
  using iterator_category = typename std::iterator_traits<InputIt>::iterator_category;
  detail::do_advance(it, difference_type(n), iterator_category());
}

template <typename InputIt>
typename std::iterator_traits<InputIt>::difference_type distance(InputIt first, InputIt last) {
  using iterator_category = typename std::iterator_traits<InputIt>::iterator_category;
  return detail::do_distance(first, last, iterator_category());
}

template <typename InputIt>
InputIt next(InputIt it, typename std::iterator_traits<InputIt>::difference_type n = 1) {
  gtl::advance(it, n);
  return it;
}

template <typename BidirIt>
BidirIt prev(BidirIt it, typename std::iterator_traits<BidirIt>::difference_type n = 1) {
  gtl::advance(it, -n);
  return it;
}

}  // namespace gtl