/**
 * @file heap_op_test.cc
 * @author gwq5210 (gwq5210@qq.com)
 * @brief 堆操作算法的单元测试
 * @date 2021-05-27
 *
 * @copyright Copyright (c) 2021. All rights reserved.
 */

#include <algorithm>
#include <numeric>

#include "gtest/gtest.h"

#include "gtl/common.h"
#include "gtl/heap_op.h"
#include "gtl/vector.h"

using gtl::Vector;

TEST(heap_op_test, heap_op_test) {
  int n = 102400;
  Vector<int> vec(n);
  Vector<int> vec_copy;
  std::iota(vec.begin(), vec.end(), 0);
  vec_copy = vec;

  EXPECT_EQ(std::is_heap_until(vec.begin(), vec.end()), gtl::is_heap_until(vec.begin(), vec.end()));
  EXPECT_EQ(std::is_heap_until(vec_copy.begin(), vec_copy.end()), gtl::is_heap_until(vec_copy.begin(), vec_copy.end()));
  EXPECT_EQ(std::is_heap(vec.begin(), vec.end()), gtl::is_heap(vec.begin(), vec.end()));
  EXPECT_EQ(std::is_heap(vec_copy.begin(), vec_copy.end()), gtl::is_heap(vec_copy.begin(), vec_copy.end()));
  gtl::make_heap(vec.begin(), vec.end());
  std::make_heap(vec_copy.begin(), vec_copy.end());
  EXPECT_EQ(std::is_heap(vec.begin(), vec.end()), gtl::is_heap(vec.begin(), vec.end()));
  EXPECT_EQ(std::is_heap(vec_copy.begin(), vec_copy.end()), gtl::is_heap(vec_copy.begin(), vec_copy.end()));
  EXPECT_EQ(std::is_heap_until(vec.begin(), vec.end()), gtl::is_heap_until(vec.begin(), vec.end()));
  EXPECT_EQ(std::is_heap_until(vec_copy.begin(), vec_copy.end()), gtl::is_heap_until(vec_copy.begin(), vec_copy.end()));

  for (int i = 0; i < n; ++i) {
    vec.push_back(n + i);
    vec_copy.push_back(n + i);
    gtl::push_heap(vec.begin(), vec.end());
    std::push_heap(vec_copy.begin(), vec_copy.end());
    EXPECT_EQ(vec.front(), n + i);
    EXPECT_EQ(vec_copy.front(), n + i);
  }
  EXPECT_EQ(std::is_heap(vec.begin(), vec.end()), gtl::is_heap(vec.begin(), vec.end()));
  EXPECT_EQ(std::is_heap(vec_copy.begin(), vec_copy.end()), gtl::is_heap(vec_copy.begin(), vec_copy.end()));
  vec.insert_safe(vec.end(), vec.begin(), vec.end());
  vec_copy.insert_safe(vec_copy.end(), vec_copy.begin(), vec_copy.end());
  EXPECT_EQ(std::is_heap(vec.begin(), vec.end()), gtl::is_heap(vec.begin(), vec.end()));
  EXPECT_EQ(std::is_heap(vec_copy.begin(), vec_copy.end()), gtl::is_heap(vec_copy.begin(), vec_copy.end()));
  vec.resize(vec.size() + n, n);
  vec_copy.resize(vec_copy.size() + n, n);
  EXPECT_EQ(std::is_heap(vec.begin(), vec.end()), gtl::is_heap(vec.begin(), vec.end()));
  EXPECT_EQ(std::is_heap(vec_copy.begin(), vec_copy.end()), gtl::is_heap(vec_copy.begin(), vec_copy.end()));
  EXPECT_EQ(std::is_heap_until(vec.begin(), vec.end()), gtl::is_heap_until(vec.begin(), vec.end()));
  EXPECT_EQ(std::is_heap_until(vec_copy.begin(), vec_copy.end()), gtl::is_heap_until(vec_copy.begin(), vec_copy.end()));

  std::reverse(vec.begin(), vec.end());
  std::reverse(vec_copy.begin(), vec_copy.end());
  EXPECT_EQ(std::is_heap(vec.begin(), vec.end()), gtl::is_heap(vec.begin(), vec.end()));
  EXPECT_EQ(std::is_heap(vec_copy.begin(), vec_copy.end()), gtl::is_heap(vec_copy.begin(), vec_copy.end()));
  EXPECT_EQ(std::is_heap_until(vec.begin(), vec.end()), gtl::is_heap_until(vec.begin(), vec.end()));
  EXPECT_EQ(std::is_heap_until(vec_copy.begin(), vec_copy.end()), gtl::is_heap_until(vec_copy.begin(), vec_copy.end()));
  gtl::make_heap(vec.begin(), vec.end());
  gtl::make_heap(vec_copy.begin(), vec_copy.end());
  gtl::sort_heap(vec.begin(), vec.end());
  std::sort_heap(vec_copy.begin(), vec_copy.end());
  EXPECT_EQ(std::is_sorted(vec.begin(), vec.end()), std::is_sorted(vec_copy.begin(), vec_copy.end()));
  EXPECT_EQ(std::is_heap_until(vec.begin(), vec.end()), gtl::is_heap_until(vec.begin(), vec.end()));
  EXPECT_EQ(std::is_heap_until(vec_copy.begin(), vec_copy.end()), gtl::is_heap_until(vec_copy.begin(), vec_copy.end()));
}