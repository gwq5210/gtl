/**
 * @file heap_op.h
 * @author gwq5210 (gwq5210@qq.com)
 * @brief 堆操作算法
 * @date 2021-05-27
 *
 * @copyright Copyright (c) 2021. All rights reserved.
 */

#pragma once

#include <iterator>

#include "gtl/algorithm/algorithm.h"
#include "gtl/iterator/iterator.h"

namespace gtl {

template <typename RandomIt, typename Compare>
RandomIt is_heap_until(RandomIt first, RandomIt last, Compare comp) {
  auto n = gtl::distance(first, last);
  typename std::iterator_traits<RandomIt>::difference_type i = 0;
  if (n <= 1) {
    return last;
  }
  auto it = first;
  auto left_it = first + 1;
  for (; i < n / 2; ++i, ++it, ++left_it) {
    if (comp(*it, *left_it)) {
      return left_it;
    }
    ++left_it;
    if (left_it < last && comp(*it, *left_it)) {
      return left_it;
    }
  }

  return last;
}

template <typename RandomIt>
RandomIt is_heap_until(RandomIt first, RandomIt last) {
  return gtl::is_heap_until(first, last, std::less<typename std::iterator_traits<RandomIt>::value_type>());
}

template <typename RandomIt, typename Compare>
bool is_heap(RandomIt first, RandomIt last, Compare comp) {
  return gtl::is_heap_until(first, last, comp) == last;
}

template <typename RandomIt>
bool is_heap(RandomIt first, RandomIt last) {
  return gtl::is_heap(first, last, std::less<typename std::iterator_traits<RandomIt>::value_type>());
}

template <typename RandomIt, typename SizeType, typename Compare>
void sift_down(RandomIt first, RandomIt last, SizeType i, Compare comp) {
  auto n = gtl::distance(first, last);
  while (i < n / 2) {
    auto l = 2 * i + 1;
    if (l + 1 < n && comp(*(first + l), *(first + l + 1))) {
      ++l;
    }
    if (!comp(*(first + i), *(first + l))) {
      break;
    }
    gtl::iter_swap(first + i, first + l);
    i = l;
  }
}

template <typename RandomIt, typename SizeType, typename Compare>
void sift_up(RandomIt first, RandomIt last, SizeType i, Compare comp) {
  while (i > 0 && comp(*(first + (i - 1) / 2), *(first + i))) {
    gtl::iter_swap(first + i, first + (i - 1) / 2);
    i = (i - 1) / 2;
  }
}

template <typename RandomIt, typename Compare>
void make_heap(RandomIt first, RandomIt last, Compare comp) {
  if (first != last) {
    for (auto i = gtl::distance(first, last) / 2 - 1; i >= 0; --i) {
      gtl::sift_down(first, last, i, comp);
    }
  }
}

template <typename RandomIt>
void make_heap(RandomIt first, RandomIt last) {
  return gtl::make_heap(first, last, std::less<typename std::iterator_traits<RandomIt>::value_type>());
}

template <typename RandomIt, typename Compare>
void push_heap(RandomIt first, RandomIt last, Compare comp) {
  if (first != last) {
    gtl::sift_up(first, last, gtl::distance(first, last) - 1, comp);
  }
}

template <typename RandomIt>
void push_heap(RandomIt first, RandomIt last) {
  return gtl::push_heap(first, last, std::less<typename std::iterator_traits<RandomIt>::value_type>());
}

template <typename RandomIt, typename Compare>
void pop_heap(RandomIt first, RandomIt last, Compare comp) {
  if (first != last) {
    --last;
    gtl::iter_swap(first, last);
    gtl::sift_down(first, last, 0, comp);
  }
}

template <typename RandomIt>
void pop_heap(RandomIt first, RandomIt last) {
  return gtl::pop_heap(first, last, std::less<typename std::iterator_traits<RandomIt>::value_type>());
}

template <typename RandomIt, typename Compare>
void sort_heap(RandomIt first, RandomIt last, Compare comp) {
  for (; first != last; --last) {
    gtl::pop_heap(first, last, comp);
  }
}

template <typename RandomIt>
void sort_heap(RandomIt first, RandomIt last) {
  return gtl::sort_heap(first, last, std::less<typename std::iterator_traits<RandomIt>::value_type>());
}

}  // namespace gtl