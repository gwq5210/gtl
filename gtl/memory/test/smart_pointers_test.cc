/**
 * @file smart_pointers_test.cc
 * @author gwq5210 (gwq5210@qq.com)
 * @brief 智能指针测试
 * @date 2021-11-04
 * 
 * @copyright Copyright (c) 2021. All rights reserved.
 */

#include <cstdio>

#include "gtest/gtest.h"

#include "smart_pointers.h"
#include "test_class.h"

using gtl::AutoPtr;
using gtl::test::Person;

TEST(smart_pointers_test, auto_ptr_test) {
  {
    AutoPtr<Person> p;
    EXPECT_EQ(p.get(), nullptr);
  }
  
  {
    AutoPtr<Person> p(new Person("auto_ptr"));
    auto *p_bak = p.get();
    EXPECT_NE(p.get(), nullptr);

    AutoPtr<Person> p2(p);
    EXPECT_EQ(p.get(), nullptr);
    EXPECT_EQ(p2.get(), p_bak);
  }
}