/**
 * @file gtl_seq_op.h
 * @author gwq5210 (gwq5210@qq.com)
 * @brief 序列操作的实现
 * @date 2021-05-26
 * 
 * @copyright Copyright (c) 2021. All rights reserved.
 */

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

template <typename InputIt, typename SizeType, typename UnaryPredicate>
InputIt find_if_n(InputIt first, SizeType n, UnaryPredicate p) {
  for (; n > 0; ++first, --n) {
    if (p(*first)) {
      return first;
    }
  }
  return first;
}

template <typename InputIt, typename SizeType, typename UnaryPredicate>
InputIt find_if_not_n(InputIt first, SizeType n, UnaryPredicate p) {
  for (; n > 0; ++first, --n) {
    if (!p(*first)) {
      return first;
    }
  }
  return first;
}

template <typename InputIt, typename SizeType, typename T>
InputIt find_n(InputIt first, SizeType n, const T& v) {
  for (; n > 0; ++first, --n) {
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

template <typename InputIt, typename SizeType, typename BinaryPredicate>
InputIt adjacent_find_n(InputIt first, SizeType n, BinaryPredicate p) {
  auto next = first;
  if (n > 0) {
    ++next;
    for (; n > 1; ++next, ++first, --n) {
      if (p(*first, *next)) {
        return first;
      }
    }
  }
  return next;
}

template <typename InputIt, typename SizeType>
InputIt adjacent_find_n(InputIt first, SizeType n) {
  auto next = first;
  if (n > 0) {
    ++next;
    for (; n > 1; ++next, ++first, --n) {
      if (*first == *next) {
        return first;
      }
    }
  }
  return next;
}

// 修改序列的操作

}  // namespace gtl