/**
 * @file gtl_heap_op.h
 * @author gwq5210 (gwq5210@qq.com)
 * @brief 堆操作算法
 * @date 2021-05-27
 * 
 * @copyright Copyright (c) 2021. All rights reserved.
 */

#pragma once

#include <iterator>

#include "gtl_common.h"

namespace gtl {

template <typename RandomIt, typename Compare>
bool is_heap(RandomIt first, RandomIt last, Compare comp) {
  auto n = std::distance(first, last);
  typename std::iterator_traits<RandomIt>::difference_type i = 0;
  if (n <= 1) {
    return true;
  }
  auto it = first;
  auto left_it = first + 1;
  for (; i < n / 2; ++i, ++it, ++left_it) {
    if (comp(*it, *left_it)) {
      return false;
    }
    ++left_it;
    if (left_it != last && comp(*it, *left_it)) {
      return false;
    }
  }

  return true;
}

template <typename RandomIt>
bool is_heap(RandomIt first, RandomIt last) {
  return gtl::is_heap(first, last, std::less<>());
}

template <typename RandomIt, typename Compare>
RandomIt is_heap_until(RandomIt first, RandomIt last, Compare comp) {
  auto n = std::distance(first, last);
  typename std::iterator_traits<RandomIt>::difference_type i = 0;
  if (n <= 1) {
    return true;
  }
  auto it = first;
  auto left_it = first + 1;
  for (; i < n / 2; ++i, ++it, ++left_it) {
    if (comp(*it, *left_it)) {
      return it;
    }
    ++left_it;
    if (left_it < last && comp(*it, *left_it)) {
      return it;
    }
  }

  return last;
}

template <typename RandomIt>
RandomIt is_heap_until(RandomIt first, RandomIt last) {
  return gtl::is_heap_until(first, last, std::less<>());
}

template <typename RandomIt, typename SizeType, typename Compare>
void sift_down(RandomIt first, RandomIt last, SizeType i, Compare comp) {
  auto n = std::distance(first, last);
  while (i < n / 2) {
    auto l = 2 * i + 1;
    if (l + 1 < n && comp(*(first + l), *(first + l + 1))) {
      ++l;
    }
    if (!comp(*(first + i), *(first + l))) {
      break;
    }
    std::iter_swap(first + i, first + l);
    i = l;
  }
}

template <typename RandomIt, typename SizeType, typename Compare>
void sift_up(RandomIt first, RandomIt last, SizeType i, Compare comp) {
  while (i > 0 && comp(*(first + (i - 1) / 2), *(first + i))) {
    std::iter_swap(first + i, first + (i - 1) / 2);
    i = (i - 1) / 2;
  }
}

template <typename RandomIt, typename Compare>
void make_heap(RandomIt first, RandomIt last, Compare comp) {
  if (first != last) {
    for (auto i = std::distance(first, last) / 2 - 1; i >= 0; --i) {
      gtl::sift_down(first, last, i, comp);
    }
  }
}

template <typename RandomIt>
void make_heap(RandomIt first, RandomIt last) {
  return gtl::make_heap(first, last, std::less<>());
}

template <typename RandomIt, typename Compare>
void push_heap(RandomIt first, RandomIt last, Compare comp) {
  if (first != last) {
    gtl::sift_up(first, last, std::distance(first, last) - 1, comp);
  }
}

template <typename RandomIt>
void push_heap(RandomIt first, RandomIt last) {
  return gtl::push_heap(first, last, std::less<>());
}

template <typename RandomIt, typename Compare>
void pop_heap(RandomIt first, RandomIt last, Compare comp) {
  if (first != last) {
    --last;
    std::iter_swap(first, last);
    gtl::sift_down(first, last, 0, comp);
  }
}

template <typename RandomIt>
void pop_heap(RandomIt first, RandomIt last) {
  return gtl::pop_heap(first, last, std::less<>());
}

template <typename RandomIt, typename Compare>
void sort_heap(RandomIt first, RandomIt last, Compare comp) {
  for (; first != last; --last) {
    gtl::pop_heap(first, last, comp);
  }
}

template <typename RandomIt>
void sort_heap(RandomIt first, RandomIt last) {
  return gtl::sort_heap(first, last, std::less<>());
}

}  // namespace gtl