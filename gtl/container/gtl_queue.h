/**
 * @file gtl_queue.h
 * @author gwq5210 (gwq5210@qq.com)
 * @brief queue的实现
 * @date 2021-05-16
 *
 * @copyright Copyright (c) 2021. All rights reserved.
 */

#pragma once

#include "gtl_deque.h"
#include "gtl_list.h"

namespace gtl {

template <typename T, typename C = List<T>>
class Queue {
 public:
};  // class Queue

template <typename T, typename C = List<T>>
using queue = Queue<T, C>;

}  // namespace gtl