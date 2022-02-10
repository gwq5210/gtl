/**
 * @file string_test.cc
 * @author gwq5210 (gwq5210@qq.com)
 * @brief string单元测试
 * @date 2021-11-07
 *
 * @copyright Copyright (c) 2021. All rights reserved.
 */

#include <cstdio>

#include <string>

#include "gtest/gtest.h"

#include "gtl/string.h"

using gtl::StringCore;

void string_test(const std::string& str, int capacity, StringCore<char>::StringType type) {
  printf("test str: %s\n", str.c_str());
  StringCore<char> ss;
  EXPECT_EQ(ss.size(), 0);
  EXPECT_EQ(ss.empty(), true);
  EXPECT_EQ(ss.capacity(), StringCore<char>::kMaxSmallSize);
  EXPECT_EQ(ss.string_type(), StringCore<char>::StringType::kSmall);

  EXPECT_LE(str.size(), capacity);
  StringCore<char> ss2(str.c_str(), str.size());
  EXPECT_STREQ(ss2.cdata(), str.c_str());
  EXPECT_EQ(ss2.size(), str.size());
  EXPECT_EQ(ss2.empty(), false);
  EXPECT_EQ(ss2.capacity(), capacity);
  EXPECT_EQ(ss2.string_type(), type);
  EXPECT_EQ(ss2.use_count(), 1);

  StringCore<char> ss3(ss2);
  EXPECT_STREQ(ss3.cdata(), str.c_str());
  EXPECT_EQ(ss3.size(), str.size());
  EXPECT_EQ(ss3.empty(), false);
  EXPECT_EQ(ss3.capacity(), capacity);
  EXPECT_EQ(ss3.string_type(), type);
  EXPECT_EQ(ss3.use_count(), type == StringCore<char>::StringType::kLarge ? 2 : 1);

  StringCore<char> ss4(std::move(ss2));
  EXPECT_STREQ(ss2.cdata(), "");
  EXPECT_EQ(ss2.size(), 0);
  EXPECT_EQ(ss2.empty(), true);
  EXPECT_EQ(ss2.capacity(), StringCore<char>::kMaxSmallSize);
  EXPECT_EQ(ss2.string_type(), StringCore<char>::StringType::kSmall);
  EXPECT_STREQ(ss4.cdata(), str.c_str());
  EXPECT_EQ(ss4.size(), str.size());
  EXPECT_EQ(ss4.empty(), false);
  EXPECT_EQ(ss4.capacity(), capacity);
  EXPECT_EQ(ss4.string_type(), type);
  EXPECT_EQ(ss4.use_count(), type == StringCore<char>::StringType::kLarge ? 2 : 1);

  StringCore<char> ss5;
  EXPECT_STREQ(ss5.cdata(), "");
  EXPECT_EQ(ss5.size(), 0);
  EXPECT_EQ(ss5.empty(), true);
  EXPECT_EQ(ss5.capacity(), StringCore<char>::kMaxSmallSize);
  EXPECT_EQ(ss5.string_type(), StringCore<char>::StringType::kSmall);
  ss5 = ss4;
  EXPECT_STREQ(ss5.cdata(), str.c_str());
  EXPECT_EQ(ss5.size(), str.size());
  EXPECT_EQ(ss5.empty(), false);
  EXPECT_EQ(ss5.capacity(), capacity);
  EXPECT_EQ(ss5.string_type(), type);
  EXPECT_EQ(ss5.use_count(), type == StringCore<char>::StringType::kLarge ? 3 : 1);

  StringCore<char> ss6;
  EXPECT_STREQ(ss6.cdata(), "");
  EXPECT_EQ(ss6.size(), 0);
  EXPECT_EQ(ss6.empty(), true);
  EXPECT_EQ(ss6.capacity(), StringCore<char>::kMaxSmallSize);
  EXPECT_EQ(ss6.string_type(), StringCore<char>::StringType::kSmall);
  ss6 = std::move(ss5);
  EXPECT_STREQ(ss6.cdata(), str.c_str());
  EXPECT_EQ(ss6.size(), str.size());
  EXPECT_EQ(ss6.empty(), false);
  EXPECT_EQ(ss6.capacity(), capacity);
  EXPECT_EQ(ss6.string_type(), type);
  EXPECT_EQ(ss6.use_count(), type == StringCore<char>::StringType::kLarge ? 3 : 1);
  EXPECT_STREQ(ss5.cdata(), "");
  EXPECT_EQ(ss5.size(), 0);
  EXPECT_EQ(ss5.empty(), true);
  EXPECT_EQ(ss5.capacity(), StringCore<char>::kMaxSmallSize);
  EXPECT_EQ(ss5.string_type(), StringCore<char>::StringType::kSmall);
  EXPECT_EQ(ss5.use_count(), 1);
}

TEST(string_test, basic_test) {
  int capacity = StringCore<char>::kMaxSmallSize;
  StringCore<char>::StringType type = StringCore<char>::StringType::kSmall;
  std::string str("hello world!");
  string_test(str, capacity, type);
  str = "hello world!hello worl";
  string_test(str, capacity, type);

  capacity = StringCore<char>::kMaxMediumSize;
  type = StringCore<char>::StringType::kMedium;
  str = "hello world!hello world!";
  string_test(str, capacity, type);

  int n = 256;
  str = "";
  for (int i = 0; i < n; ++i) {
    str += std::to_string(i);
  }
  capacity = str.size();
  type = StringCore<char>::StringType::kLarge;
  string_test(str, capacity, type);
}