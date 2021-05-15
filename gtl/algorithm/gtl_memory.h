/**
 * @file gtl_memory.h
 * @author gwq5210 (gwq5210@qq.com)
 * @brief 内存管理相关的函数库
 * @date 2021-05-15
 * 
 * @copyright Copyright (c) 2021. All rights reserved.
 */

#pragma once

namespace gtl {

template <typename T, typename... Args>
T* construct_at(T* p, Args&&... args) {
  new (p) T(std::forward<Args>(args)...);
  return p;
}

}  // namespace gtl