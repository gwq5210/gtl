/**
 * @file gtl_compressed_pair.h
 * @author gwq5210 (gwq5210@qq.com)
 * @brief 压缩pair的实现
 * @date 2021-05-28
 * 
 * @copyright Copyright (c) 2021. All rights reserved.
 */

#pragma once

namespace gtl {

template <typename T1, typename T2>
class CompressedPair : public T2 {
 public:
  using Base = T2;
  using first_type = T1;
  using second_type = T2;
  using first_reference = T1&;
  using first_const_reference = const T1&;
  using second_reference = T2&;
  using second_const_reference = const T2&;

  CompressedPair() = default;
  CompressedPair(first_const_reference first, second_const_reference second) : first_(first), Base(second) {}
  explicit CompressedPair(first_const_reference first) : first_(first) {}
  explicit CompressedPair(second_const_reference second) : Base(second) {}

  first_reference first() { return first_; }
  first_const_reference first() const { return first_; }
  second_reference second() { return *this; }
  second_const_reference second() const { return *this; }

 private:
  T1 first_;
};

}  // namespace gtl