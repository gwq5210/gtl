/**
 * @file gtl_stack.h
 * @author gwq5210 (gwq5210@qq.com)
 * @brief stack的实现
 * @date 2021-05-16
 *
 * @copyright Copyright (c) 2021. All rights reserved.
 */

#pragma once

// #include "gtl_deque.h"
#include "gtl_list.h"

namespace gtl {

template <typename T, typename C = List<T>>
class Stack {
 public:
 private:
  C c_;
};  // class Stack

template <typename T, typename C = List<T>>
using stack = Stack<T, C>;

}  // namespace gtl