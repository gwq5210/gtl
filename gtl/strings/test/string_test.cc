/**
 * @file string_test.cc
 * @author gwq5210 (gwq5210@qq.com)
 * @brief string单元测试
 * @date 2021-11-07
 * 
 * @copyright Copyright (c) 2021. All rights reserved.
 */

#include <cstdio>

#include "gtest/gtest.h"

#include "gtl/string.h"

using gtl::StringImpl;

TEST(string_test, small_string_test) {
  {
    StringImpl<char> ss;
    EXPECT_EQ(ss.size(), 0);
    EXPECT_EQ(ss.empty(), true);
  }
}