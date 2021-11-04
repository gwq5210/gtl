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

using gtl::auto_ptr;
using gtl::unique_ptr;
using gtl::test::Person;
using gtl::make_auto_ptr;
using gtl::make_unique;

TEST(smart_pointers_test, auto_ptr_test) {
  {
    auto_ptr<Person> p;
    EXPECT_EQ(p.get(), nullptr);
  }

  {
    auto_ptr<Person> p(nullptr);
    EXPECT_EQ(p.get(), nullptr);
  }

  {
    auto_ptr<Person> p(make_auto_ptr<Person>("auto_ptr1"));
    auto* p_bak = p.get();
    EXPECT_NE(p.get(), nullptr);

    auto_ptr<Person> p2(p);
    EXPECT_EQ(p.get(), nullptr);
    EXPECT_EQ(p2.get(), p_bak);
  }
  
  {
    auto_ptr<Person> p(make_auto_ptr<Person>("auto_ptr2"));
    auto* p_bak = p.get();
    EXPECT_NE(p.get(), nullptr);

    auto_ptr<Person> p2(make_auto_ptr<Person>("auto_ptr3"));
    EXPECT_NE(p.get(), nullptr);
    printf("before p assign\n");
    p2 = p;
    printf("after p assign\n");
    EXPECT_EQ(p.get(), nullptr);
    EXPECT_EQ(p2.get(), p_bak);
  }
}

TEST(smart_pointers_test, unique_ptr_test) {
  {
    unique_ptr<Person> p;
    EXPECT_EQ(p.get(), nullptr);
    EXPECT_EQ(bool(p), false);
  }

  {
    unique_ptr<Person> p(nullptr);
    EXPECT_EQ(p.get(), nullptr);
    EXPECT_EQ(bool(p), false);
  }

  {
    unique_ptr<Person> p(make_unique<Person>("unique_ptr1"));
    auto* p_bak = p.get();
    EXPECT_NE(p.get(), nullptr);
    EXPECT_EQ(bool(p), true);

    unique_ptr<Person> p2(std::move(p));
    EXPECT_EQ(p.get(), nullptr);
    EXPECT_EQ(bool(p), false);
    EXPECT_EQ(p2.get(), p_bak);
    EXPECT_EQ(bool(p2), true);
  }
  
  {
    unique_ptr<Person> p(make_unique<Person>("unique_ptr2"));
    auto* p_bak = p.get();
    EXPECT_NE(p.get(), nullptr);
    EXPECT_EQ(bool(p), true);

    unique_ptr<Person> p2(make_unique<Person>("unique_ptr3"));
    EXPECT_NE(p2.get(), nullptr);
    EXPECT_EQ(bool(p2), true);
    printf("before p move\n");
    p2 = std::move(p);
    printf("after p move\n");
    EXPECT_EQ(p.get(), nullptr);
    EXPECT_EQ(p2.get(), p_bak);
    EXPECT_EQ(bool(p), false);
    EXPECT_EQ(bool(p2), true);
  }

  {
    int n = 10;
    unique_ptr<Person[]> p = make_unique<Person[]>(n);
    Person* p_bak = p.get();
    EXPECT_NE(p.get(), nullptr);
    EXPECT_EQ(bool(p), true);

    for (int i = 0; i < n; ++i) {
      p[i].set_name(std::to_string(i));
    }

    unique_ptr<Person[]> p2(std::move(p));
    EXPECT_EQ(p2.get(), p_bak);
    EXPECT_EQ(bool(p), false);
    EXPECT_EQ(bool(p2), true);

    unique_ptr<Person[]> p3;
    EXPECT_EQ(p.get(), nullptr);
    EXPECT_EQ(bool(p), false);
    p3 = std::move(p2);
    EXPECT_EQ(p3.get(), p_bak);
    EXPECT_EQ(bool(p2), false);
    EXPECT_EQ(bool(p3), true);
  }
}