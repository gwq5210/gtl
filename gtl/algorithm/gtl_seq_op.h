/**
 * @file gtl_seq_op.h
 * @author gwq5210 (gwq5210@qq.com)
 * @brief 序列操作的实现
 * @date 2021-05-26
 *
 * @copyright Copyright (c) 2021. All rights reserved.
 */

#pragma once

#include <iterator>

namespace gtl {

// 不修改序列的操作

template <typename InputIt, typename UnaryPredicate>
InputIt find_if(InputIt first, InputIt last, UnaryPredicate p) {
  for (; first != last; ++first) {
    if (p(*first)) {
      return first;
    }
  }
  return first;
}

template <typename InputIt, typename UnaryPredicate>
InputIt find_if_not(InputIt first, InputIt last, UnaryPredicate p) {
  for (; first != last; ++first) {
    if (!p(*first)) {
      return first;
    }
  }
  return first;
}

template <typename InputIt, typename T>
InputIt find(InputIt first, InputIt last, const T& v) {
  for (; first != last; ++first) {
    if (*first == v) {
      return first;
    }
  }
  return first;
}

template <typename InputIt, typename UnaryPredicate>
bool all_of(InputIt first, InputIt last, UnaryPredicate p) {
  return gtl::find_if_not(first, last, p) == last;
}

template <typename InputIt, typename UnaryPredicate>
bool any_of(InputIt first, InputIt last, UnaryPredicate p) {
  return gtl::find_if(first, last, p) != last;
}

template <typename InputIt, typename UnaryPredicate>
bool none_of(InputIt first, InputIt last, UnaryPredicate p) {
  return gtl::find_if(first, last, p) == last;
}

template <typename InputIt, typename SizeType, typename UnaryPredicate>
bool all_of_n(InputIt first, SizeType n, UnaryPredicate p) {
  for (; n > 0; ++first, --n) {
    if (!p(*first)) {
      return false;
    }
  }
  return true;
}

template <typename InputIt, typename SizeType, typename UnaryPredicate>
bool any_of_n(InputIt first, SizeType n, UnaryPredicate p) {
  for (; n > 0; ++first, --n) {
    if (p(*first)) {
      return true;
    }
  }
  return false;
}

template <typename InputIt, typename SizeType, typename UnaryPredicate>
bool none_of_n(InputIt first, SizeType n, UnaryPredicate p) {
  for (; n > 0; ++first, --n) {
    if (p(*first)) {
      return false;
    }
  }
  return true;
}

template <typename InputIt, typename UnaryFunction>
UnaryFunction for_each(InputIt first, InputIt last, UnaryFunction f) {
  for (; first != last; ++first) {
    f(*first);
  }
  return f;
}

template <typename InputIt, typename SizeType, typename UnaryFunction>
UnaryFunction for_each_n(InputIt first, SizeType n, UnaryFunction f) {
  for (; n > 0; ++first, --n) {
    f(*first);
  }
  return f;
}

template <typename InputIt, typename T>
typename std::iterator_traits<InputIt>::difference_type count(InputIt first, InputIt last, const T& v) {
  typename std::iterator_traits<InputIt>::difference_type ret = 0;
  for (; first != last; ++first) {
    if (*first == v) {
      ++ret;
    }
  }
  return ret;
}

template <typename InputIt, typename UnaryPredicate>
typename std::iterator_traits<InputIt>::difference_type count_if(InputIt first, InputIt last, UnaryPredicate p) {
  typename std::iterator_traits<InputIt>::difference_type ret = 0;
  for (; first != last; ++first) {
    if (p(*first)) {
      ++ret;
    }
  }
  return ret;
}

template <typename InputIt, typename SizeType, typename T>
typename std::iterator_traits<InputIt>::difference_type count_n(InputIt first, SizeType n, const T& v) {
  typename std::iterator_traits<InputIt>::difference_type ret = 0;
  for (; n > 0; ++first, --n) {
    if (*first == v) {
      ++ret;
    }
  }
  return ret;
}

template <typename InputIt, typename SizeType, typename UnaryPredicate>
typename std::iterator_traits<InputIt>::difference_type count_if_n(InputIt first, SizeType n, UnaryPredicate p) {
  typename std::iterator_traits<InputIt>::difference_type ret = 0;
  for (; n > 0; ++first, --n) {
    if (p(*first)) {
      ++ret;
    }
  }
  return ret;
}

template <typename InputIt, typename BinaryPredicate>
InputIt adjacent_find(InputIt first, InputIt last, BinaryPredicate p) {
  auto next = first;
  if (first != last) {
    ++next;
    for (; next != last; ++next, ++first) {
      if (p(*first, *next)) {
        return first;
      }
    }
  }
  return next;
}

template <typename InputIt>
InputIt adjacent_find(InputIt first, InputIt last) {
  auto next = first;
  if (first != last) {
    ++next;
    for (; next != last; ++next, ++first) {
      if (*first == *next) {
        return first;
      }
    }
  }
  return next;
}

template <typename InputIt1, typename InputIt2, typename BinaryPredicate>
std::pair<InputIt1, InputIt2> missmatch(InputIt1 first1, InputIt1 last1, InputIt2 first2, BinaryPredicate p) {
  for (; first1 != last1 && p(*first1, *first2); ++first1, ++first2) {
  }
  return std::make_pair(first1, first2);
}

template <typename InputIt1, typename InputIt2>
std::pair<InputIt1, InputIt2> missmatch(InputIt1 first1, InputIt1 last1, InputIt2 first2) {
  for (; first1 != last1 && *first1 == *first2; ++first1, ++first2) {
  }
  return std::make_pair(first1, first2);
}

template <typename InputIt1, typename InputIt2, typename BinaryPredicate>
std::pair<InputIt1, InputIt2> missmatch(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2, BinaryPredicate p) {
  for (; first1 != last1 && first2 != last2 && p(*first1, *first2); ++first1, ++first2) {
  }
  return std::make_pair(first1, first2);
}

template <typename InputIt1, typename InputIt2>
std::pair<InputIt1, InputIt2> missmatch(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2) {
  for (; first1 != last1 && first2 != last2 && *first1 == *first2; ++first1, ++first2) {
  }
  return std::make_pair(first1, first2);
}

// 修改序列的操作

template <typename InputIt, typename OutputIt, typename UnaryPredicate>
OutputIt copy_if(InputIt first, InputIt last, OutputIt d_first, UnaryPredicate p) {
  for (; first != last; ++first) {
    if (p(*first)) {
      *d_first = *first;
      ++d_first;
    }
  }
  return d_first;
}

template <typename InputIt, typename OutputIt>
OutputIt copy(InputIt first, InputIt last, OutputIt d_first) {
  for (; first != last; ++first, ++d_first) {
    *d_first = *first;
  }
  return d_first;
}

template <typename InputIt, typename SizeType, typename OutputIt, typename UnaryPredicate>
OutputIt copy_if_n(InputIt first, SizeType n, OutputIt d_first, UnaryPredicate p) {
  for (; n > 0; ++first, --n) {
    if (p(*first)) {
      *d_first = *first;
      ++d_first;
    }
  }
  return d_first;
}

template <typename InputIt, typename SizeType, typename OutputIt>
OutputIt copy_n(InputIt first, SizeType n, OutputIt d_first) {
  for (; n > 0; ++first, ++d_first, --n) {
    *d_first = *first;
  }
  return d_first;
}

template <typename BidirIt1, typename BidirIt2>
BidirIt2 copy_backward(BidirIt1 first, BidirIt1 last, BidirIt2 d_last) {
  while (first != last) {
    *--d_last = *--last;
  }
  return d_last;
}

template <typename BidirIt1, typename SizeType, typename BidirIt2>
BidirIt2 copy_backward_n(BidirIt1 last, SizeType n, BidirIt2 d_last) {
  for (; n > 0; --n) {
    *--d_last = *--last;
  }
  return d_last;
}

template <typename InputIt, typename OutputIt, typename UnaryPredicate>
OutputIt move_if(InputIt first, InputIt last, OutputIt d_first, UnaryPredicate p) {
  for (; first != last; ++first) {
    if (p(*first)) {
      *d_first = std::move(*first);
      ++d_first;
    }
  }
  return d_first;
}

template <typename InputIt, typename OutputIt>
OutputIt move(InputIt first, InputIt last, OutputIt d_first) {
  for (; first != last; ++first, ++d_first) {
    *d_first = std::move(*first);
  }
  return d_first;
}

template <typename InputIt, typename SizeType, typename OutputIt, typename UnaryPredicate>
OutputIt move_if_n(InputIt first, SizeType n, OutputIt d_first, UnaryPredicate p) {
  for (; n > 0; ++first, --n) {
    if (p(*first)) {
      *d_first = std::move(*first);
      ++d_first;
    }
  }
  return d_first;
}

template <typename InputIt, typename SizeType, typename OutputIt>
OutputIt move_n(InputIt first, SizeType n, OutputIt d_first) {
  for (; n > 0; ++first, ++d_first, --n) {
    *d_first = std::move(*first);
  }
  return d_first;
}

template <typename BidirIt1, typename BidirIt2>
BidirIt2 move_backward(BidirIt1 first, BidirIt1 last, BidirIt2 d_last) {
  while (first != last) {
    *--d_last = std::move(*--last);
  }
  return d_last;
}

template <typename BidirIt1, typename SizeType, typename BidirIt2>
BidirIt2 move_backward_n(BidirIt1 last, SizeType n, BidirIt2 d_last) {
  for (; n > 0; --n) {
    *--d_last = std::move(*--last);
  }
  return d_last;
}

template <typename ForwardIt, typename T>
void fill(ForwardIt first, ForwardIt last, const T& v) {
  for (; first != last; ++first) {
    *first = v;
  }
}

template <typename OutputIt, typename SizeType, typename T>
void fill_n(OutputIt first, SizeType n, const T& v) {
  for (; n > 0; ++first, --n) {
    *first = v;
  }
}

template <typename InputIt, typename OutputIt, typename UnaryOperation>
OutputIt transform(InputIt first, InputIt last, OutputIt d_first, UnaryOperation unary_op) {
  for (; first != last; ++first, ++d_first) {
    *d_first = unary_op(*first);
  }
  return d_first;
}

template <typename InputIt1, typename InputIt2, typename OutputIt, typename BinaryOperation>
OutputIt transform(InputIt1 first1, InputIt1 last1, InputIt2 first2, OutputIt d_first, BinaryOperation binary_op) {
  for (; first1 != last1; ++first1, ++first2, ++d_first) {
    *d_first = binary_op(*first1, *first2);
  }
  return d_first;
}

template <typename InputIt, typename SizeType, typename OutputIt, typename UnaryOperation>
OutputIt transform_n(InputIt first, SizeType n, OutputIt d_first, UnaryOperation unary_op) {
  for (; n > 0; ++first, ++d_first, --n) {
    *d_first = unary_op(*first);
  }
  return d_first;
}

template <typename InputIt1, typename SizeType, typename InputIt2, typename OutputIt, typename BinaryOperation>
OutputIt transform_n(InputIt1 first1, SizeType n, InputIt2 first2, OutputIt d_first, BinaryOperation binary_op) {
  for (; n > 0; ++first1, ++first2, ++d_first, --n) {
    *d_first = binary_op(*first1, *first2);
  }
  return d_first;
}

template <typename ForwardIt, typename Generator>
void generate(ForwardIt first, ForwardIt last, Generator g) {
  for (; first != last; ++first) {
    *first = g();
  }
}

template <typename OutputIt, typename SizeType, typename Generator>
void generate(OutputIt first, SizeType n, Generator g) {
  for (; n > 0; ++first, --n) {
    *first = g();
  }
}

template <typename ForwardIt, typename UnaryPredicate>
ForwardIt remove_if(ForwardIt first, ForwardIt last, UnaryPredicate p) {
  first = gtl::find_if(first, last, p);
  for (auto it = first; it != last; ++it) {
    if (!p(*it)) {
      *first = std::move(*it);
      ++first;
    }
  }
  return first;
}

template <typename ForwardIt, typename T>
ForwardIt remove(ForwardIt first, ForwardIt last, const T& v) {
  first = gtl::find(first, last, v);
  for (auto it = first; it != last; ++it) {
    if (!(*it == v)) {
      *first = std::move(*it);
      ++first;
    }
  }
  return first;
}

template <typename ForwardIt, typename SizeType, typename UnaryPredicate>
ForwardIt remove_if_n(ForwardIt first, SizeType n, UnaryPredicate p) {
  for (auto it = first; n > 0; ++it, --n) {
    if (!p(*it)) {
      *first = std::move(*it);
      ++first;
    }
  }
  return first;
}

template <typename ForwardIt, typename SizeType, typename T>
ForwardIt remove_n(ForwardIt first, SizeType n, const T& v) {
  for (auto it = first; n > 0; ++it, --n) {
    if (!(*it == v)) {
      *first = std::move(*it);
      ++first;
    }
  }
  return first;
}

template <typename ForwardIt, typename OutputIt, typename UnaryPredicate>
ForwardIt remove_copy_if(ForwardIt first, ForwardIt last, OutputIt d_first, UnaryPredicate p) {
  for (; first != last; ++first) {
    if (!p(*first)) {
      *d_first = *first;
      ++d_first;
    }
  }
  return d_first;
}

template <typename ForwardIt, typename OutputIt, typename T>
ForwardIt remove_copy(ForwardIt first, ForwardIt last, OutputIt d_first, const T& v) {
  for (; first != last; ++first) {
    if (!(*first == v)) {
      *d_first = *first;
      ++d_first;
    }
  }
  return d_first;
}

template <typename ForwardIt, typename SizeType, typename OutputIt, typename UnaryPredicate>
ForwardIt remove_copy_if_n(ForwardIt first, SizeType n, OutputIt d_first, UnaryPredicate p) {
  for (; n > 0; ++first, --n) {
    if (!p(*first)) {
      *d_first = *first;
      ++d_first;
    }
  }
  return d_first;
}

template <typename ForwardIt, typename SizeType, typename OutputIt, typename T>
ForwardIt remove_copy_n(ForwardIt first, SizeType n, OutputIt d_first, const T& v) {
  for (; n > 0; ++first, --n) {
    if (!(*first == v)) {
      *d_first = *first;
      ++d_first;
    }
  }
  return d_first;
}

template <typename ForwardIt, typename T>
void replace(ForwardIt first, ForwardIt last, const T& old_v, const T& new_v) {
  for (; first != last; ++first) {
    if (*first == old_v) {
      *first = new_v;
    }
  }
}

template <typename ForwardIt, typename UnaryPredicate, typename T>
void replace_if(ForwardIt first, ForwardIt last, UnaryPredicate p, const T& new_v) {
  for (; first != last; ++first) {
    if (p(*first)) {
      *first = new_v;
    }
  }
}

template <typename ForwardIt, typename SizeType, typename T>
void replace_n(ForwardIt first, SizeType n, const T& old_v, const T& new_v) {
  for (; n > 0; ++first, --n) {
    if (*first == old_v) {
      *first = new_v;
    }
  }
}

template <typename ForwardIt, typename SizeType, typename UnaryPredicate, typename T>
void replace_if_n(ForwardIt first, SizeType n, UnaryPredicate p, const T& new_v) {
  for (; n > 0; ++first, --n) {
    if (p(*first)) {
      *first = new_v;
    }
  }
}

template <typename InputIt, typename OutputIt, typename T>
OutputIt replace_copy(InputIt first, InputIt last, OutputIt d_first, const T& old_v, const T& new_v) {
  for (; first != last; ++first, ++d_first) {
    if (*first == old_v) {
      *d_first = new_v;
    } else {
      *d_first = *first;
    }
  }
  return d_first;
}

template <typename InputIt, typename OutputIt, typename UnaryPredicate, typename T>
OutputIt replace_copy_if(InputIt first, InputIt last, OutputIt d_first, UnaryPredicate p, const T& new_v) {
  for (; first != last; ++first, ++d_first) {
    if (p(*first)) {
      *d_first = new_v;
    } else {
      *d_first = *first;
    }
  }
  return d_first;
}

template <typename InputIt, typename SizeType, typename OutputIt, typename T>
OutputIt replace_copy_n(InputIt first, SizeType n, OutputIt d_first, const T& old_v, const T& new_v) {
  for (; n > 0; ++first, ++d_first, --n) {
    if (*first == old_v) {
      *d_first = new_v;
    } else {
      *d_first = *first;
    }
  }
  return d_first;
}

template <typename InputIt, typename SizeType, typename OutputIt, typename UnaryPredicate, typename T>
OutputIt replace_copy_if_n(InputIt first, SizeType n, OutputIt d_first, UnaryPredicate p, const T& new_v) {
  for (; n > 0; ++first, ++d_first, --n) {
    if (p(*first)) {
      *d_first = new_v;
    } else {
      *d_first = *first;
    }
  }
}

template <typename T>
void swap(T& a, T& b) {
  T t = std::move(a);
  a = std::move(b);
  b = std::move(t);
}

template <typename T, size_t N>
void swap(T (&a)[N], T (&b)[N]) {
  for (size_t i = 0; i < N; ++i) {
    gtl::swap(a[i], b[i]);
  }
}

template <typename ForwardIt1, typename ForwardIt2>
void iter_swap(ForwardIt1 a, ForwardIt2 b) {
  gtl::swap(*a, *b);
}

template <typename ForwardIt1, typename ForwardIt2>
ForwardIt2 swap_ranges(ForwardIt1 first1, ForwardIt1 last1, ForwardIt2 first2) {
  for (; first1 != last1; ++first1, ++first2) {
    gtl::iter_swap(first1, first2);
  }
  return first2;
}

template <typename BidirIt>
void reverse(BidirIt first, BidirIt last) {
  if (first != last) {
    --last;
    for (; first != last; ++first, --last) {
      gtl::iter_swap(first, last);
    }
  }
}

template <typename BidirIt, typename OutputIt>
OutputIt reverse_copy(BidirIt first, BidirIt last, OutputIt d_first) {
  for (; first != last; ++d_first) {
    *d_first = *--last;
  }
  return d_first;
}

template <typename ForwardIt>
ForwardIt rotate(ForwardIt first, ForwardIt n_first, ForwardIt last) {
  return n_first;
}

template <typename ForwardIt, typename OutputIt>
OutputIt rotate_copy(ForwardIt first, ForwardIt n_first, ForwardIt last, OutputIt d_first) {
  return gtl::copy(first, n_first, gtl::copy(n_first, last, d_first));
}

template <typename ForwardIt, typename BinaryPredicate>
ForwardIt unique(ForwardIt first, ForwardIt last, BinaryPredicate p) {
  if (first != last) {
    auto prev = first;
    ++first;
    for (auto it = first; it != last; prev = it, ++it) {
      if (!p(*it, *prev)) {
        *first = std::move(*it);
        ++first;
      }
    }
  }
  return first;
}

template <typename ForwardIt>
ForwardIt unique(ForwardIt first, ForwardIt last) {
  if (first != last) {
    auto prev = first;
    ++first;
    for (auto it = first; it != last; prev = it, ++it) {
      if (!(*it == *prev)) {
        *first = std::move(*it);
        ++first;
      }
    }
  }
  return first;
}

template <typename ForwardIt, typename OutputIt, typename BinaryPredicate>
OutputIt unique_copy(ForwardIt first, ForwardIt last, OutputIt d_first, BinaryPredicate p) {
  if (first != last) {
    auto prev = first;
    d_first = *first;
    ++first;
    ++d_first;
    for (auto it = first; it != last; prev = it, ++it) {
      if (!p(*it, *prev)) {
        *d_first = *it;
        ++d_first;
      }
    }
  }
  return d_first;
}

template <typename ForwardIt, typename OutputIt>
OutputIt unique_copy(ForwardIt first, ForwardIt last, OutputIt d_first) {
  if (first != last) {
    auto prev = first;
    *d_first = *first;
    ++first;
    ++d_first;
    for (auto it = first; it != last; prev = it, ++it) {
      if (!(*it == *prev)) {
        *d_first = *it;
        ++d_first;
      }
    }
  }
  return d_first;
}

}  // namespace gtl