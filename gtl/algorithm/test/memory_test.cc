/**
 * @file memory_test.cc
 * @author gwq5210 (gwq5210@qq.com)
 * @brief memory函数库的单元测试
 * @date 2021-05-19
 *
 * @copyright Copyright (c) 2021. All rights reserved.
 */

#include "gtest/gtest.h"

#include "gtl/memory_op.h"
#include "gtl/vector.h"

using gtl::Vector;

TEST(memory_test, memory_test) {
  int n = 1024;
  char* buffer = new char[sizeof(Vector<int>)];
  const Vector<int>* vec_ptr = reinterpret_cast<Vector<int>*>(buffer);
  const Vector<int>& vec = *vec_ptr;
  gtl::construct_at(vec_ptr, n);
  EXPECT_EQ(vec_ptr->size(), n);
  for (int v : vec) {
    EXPECT_EQ(v, 0);
  }
  delete[] buffer;
}