/**
 * @file cow_string_test.cc
 * @author gwq5210 (gwq5210@qq.com)
 * @brief copy on write字符串单元测试
 * @date 2021-11-06
 * 
 * @copyright Copyright (c) 2021. All rights reserved.
 */

#include "gtl/cow_string.h"

#include <string>

#include "gtest/gtest.h"

using gtl::examples::CowString;

TEST(cow_string_test, cow_string_test) {
  {
    CowString cs;
    EXPECT_EQ(cs.size(), 0);
    EXPECT_EQ(cs.capacity(), 0);
    EXPECT_EQ(cs.cc_str(), nullptr);
  }
  {
    std::string std_str("hello world!");
    CowString cs(std_str.c_str());
    EXPECT_EQ(cs.size(), std_str.size());
    EXPECT_EQ(cs.capacity(), std_str.size());
    EXPECT_STREQ(cs.cdata(), std_str.c_str());

    CowString cs2(cs);
    EXPECT_EQ(cs2.size(), std_str.size());
    EXPECT_EQ(cs2.capacity(), std_str.size());
    EXPECT_EQ(cs2.cdata(), cs.cdata());
    EXPECT_STREQ(cs2.cdata(), std_str.c_str());

    CowString cs3(std::move(cs2));
    EXPECT_EQ(cs2.size(), 0);
    EXPECT_EQ(cs2.capacity(), 0);
    EXPECT_EQ(cs2.cdata(), nullptr);
    EXPECT_EQ(cs3.size(), std_str.size());
    EXPECT_EQ(cs3.capacity(), std_str.size());
    EXPECT_EQ(cs3.cdata(), cs.cdata());
    EXPECT_STREQ(cs3.cdata(), std_str.c_str());

    CowString cs4;
    cs4 = cs3;
    EXPECT_EQ(cs4.size(), std_str.size());
    EXPECT_EQ(cs4.capacity(), std_str.size());
    EXPECT_EQ(cs4.cdata(), cs.cdata());
    EXPECT_STREQ(cs4.cdata(), std_str.c_str());

    CowString cs5;
    cs5 = std::move(cs3);
    EXPECT_EQ(cs3.size(), 0);
    EXPECT_EQ(cs3.capacity(), 0);
    EXPECT_EQ(cs3.cdata(), nullptr);
    EXPECT_EQ(cs5.size(), std_str.size());
    EXPECT_EQ(cs5.capacity(), std_str.size());
    EXPECT_EQ(cs5.cdata(), cs.cdata());
    EXPECT_STREQ(cs5.cdata(), std_str.c_str());

    CowString cs6(cs5);
    EXPECT_EQ(cs6.size(), std_str.size());
    EXPECT_EQ(cs6.capacity(), std_str.size());
    EXPECT_EQ(cs6.cdata(), cs.cdata());
    EXPECT_EQ(cs5.cdata(), cs.cdata());
    EXPECT_STREQ(cs6.cdata(), std_str.c_str());
    EXPECT_STREQ(cs5.cdata(), std_str.c_str());
    EXPECT_NE(cs6.data(), cs5.cdata());
    EXPECT_EQ(cs6.size(), std_str.size());
    EXPECT_EQ(cs6.capacity(), std_str.size());
    EXPECT_STREQ(cs6.cdata(), std_str.c_str());
  }
}