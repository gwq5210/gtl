/**
 * @file slist_test.cc
 * @author gwq5210 (gwq5210@qq.com)
 * @brief 单链表单元测试
 * @date 2021-05-15
 * 
 * @copyright Copyright (c) 2021. All rights reserved.
 */

#include <cstdio>
#include <list>

#include "gtest/gtest.h"

#include "gtl_slist.h"
#include "gtl_vector.h"

using gtl::SList;
using gtl::Vector;

TEST(slist_test, constructor_assign_iterator_test) {
  {
    SList<int> l;
    SList<int> l_copy(l);
    EXPECT_EQ(l.empty(), true);
    EXPECT_EQ(l.size(), 0);
    EXPECT_EQ(l, l_copy);
    EXPECT_EQ(l < l_copy, false);
    EXPECT_EQ(l > l_copy, false);
    EXPECT_EQ(l <= l_copy, true);
    EXPECT_EQ(l >= l_copy, true);
  }

  int n = 102400;
  int value = 10;
  Vector<int> vec;
  for (int i = 0; i < n; ++i) {
    vec.push_back(i);
  }

  {
    SList<int> l(n, value);
    EXPECT_EQ(l.empty(), false);
    EXPECT_EQ(l.size(), n);
    for (auto v : l) {
      EXPECT_EQ(v, value);
    }
    for (auto it = l.begin(); it != l.end(); ++it) {
      EXPECT_EQ(*it, value);
    }
    EXPECT_EQ(l.front(), value);
    EXPECT_EQ(l.back(), value);
  }

  {
    SList<int> l(vec.begin(), vec.end());
    EXPECT_EQ(l.empty(), false);
    EXPECT_EQ(l.size(), n);
    int i = 0;
    for (auto it = l.begin(); it != l.end(); ++it, ++i) {
      EXPECT_EQ(*it, vec[i]);
    }
    EXPECT_EQ(l.front(), vec.front());
    EXPECT_EQ(l.back(), vec.back());

    SList<int> l_copy(l);
    EXPECT_EQ(l_copy.empty(), false);
    EXPECT_EQ(l_copy.size(), n);
    i = 0;
    for (auto it = l_copy.begin(); it != l_copy.end(); ++it, ++i) {
      EXPECT_EQ(*it, vec[i]);
    }
    EXPECT_EQ(l_copy.front(), vec.front());
    EXPECT_EQ(l_copy.back(), vec.back());

    SList<int> l_copy2;
    l_copy2 = l;
    EXPECT_EQ(l_copy2.empty(), false);
    EXPECT_EQ(l_copy2.size(), n);
    i = 0;
    for (auto it = l_copy2.begin(); it != l_copy2.end(); ++it, ++i) {
      EXPECT_EQ(*it, vec[i]);
    }
    EXPECT_EQ(l_copy2.front(), vec.front());
    EXPECT_EQ(l_copy2.back(), vec.back());
  }

  {
    int n = 17;
    SList<int> l{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
    EXPECT_EQ(l.empty(), false);
    EXPECT_EQ(l.size(), n);
    int i = 0;
    for (auto it = l.begin(); it != l.end(); ++it, ++i) {
      EXPECT_EQ(*it, vec[i]);
    }
    EXPECT_EQ(l.front(), 0);
    EXPECT_EQ(l.back(), n - 1);

    SList<int> l_copy = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
    EXPECT_EQ(l_copy.empty(), false);
    EXPECT_EQ(l_copy.size(), n);
    i = 0;
    for (auto it = l_copy.begin(); it != l_copy.end(); ++it, ++i) {
      EXPECT_EQ(*it, vec[i]);
    }
    EXPECT_EQ(l_copy.front(), 0);
    EXPECT_EQ(l_copy.back(), n - 1);

    SList<int> l_copy2;
    l_copy2 = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
    EXPECT_EQ(l_copy2.empty(), false);
    EXPECT_EQ(l_copy2.size(), n);
    i = 0;
    for (auto it = l_copy2.begin(); it != l_copy2.end(); ++it, ++i) {
      EXPECT_EQ(*it, vec[i]);
    }
    EXPECT_EQ(l_copy2.front(), 0);
    EXPECT_EQ(l_copy2.back(), n - 1);

    SList<int> l_move(std::move(l));
    EXPECT_EQ(l_move.empty(), false);
    EXPECT_EQ(l_move.size(), n);
    i = 0;
    for (auto it = l_move.begin(); it != l_move.end(); ++it, ++i) {
      EXPECT_EQ(*it, vec[i]);
    }
    EXPECT_EQ(l_move.front(), 0);
    EXPECT_EQ(l_move.back(), n - 1);
    EXPECT_EQ(l.empty(), true);
    EXPECT_EQ(l.size(), 0);

    SList<int> l_move2;
    l_move2 = std::move(l_copy);
    EXPECT_EQ(l_move2.empty(), false);
    EXPECT_EQ(l_move2.size(), n);
    i = 0;
    for (auto it = l_move2.begin(); it != l_move2.end(); ++it, ++i) {
      EXPECT_EQ(*it, vec[i]);
    }
    EXPECT_EQ(l_move2.front(), 0);
    EXPECT_EQ(l_move2.back(), n - 1);
    EXPECT_EQ(l_copy.empty(), true);
    EXPECT_EQ(l_copy.size(), 0);
  }
}

TEST(slist_test, modifiers_iterators_test) {
  int n = 1024;
  SList<int> l;
  for (int i = 0; i < n; ++i) {
    l.emplace_front(i);
    EXPECT_EQ(l.front(), i);
  }
  l.reverse();
  EXPECT_EQ(l.size(), n);
  l.resize(2 * n);
  EXPECT_EQ(l.size(), 2 * n);
  l.resize(n);
  EXPECT_EQ(l.size(), n);
  int i = 0;
  for (auto v : l) {
    EXPECT_EQ(v, i);
    ++i;
  }
  l.clear();
  EXPECT_EQ(l.size(), 0);
  EXPECT_EQ(l.empty(), true);
}

TEST(slist_test, operations_test) {
  int n = 1024;
  SList<int> l1;
  SList<int> l2;
  for (int i = 0; i < n; ++i) {
    l1.emplace_front(i * 2);
    l2.emplace_front(i * 2 + 1);
    EXPECT_EQ(l1.front(), i * 2);
    EXPECT_EQ(l2.front(), i * 2 + 1);
  }
  l1.reverse();
  l2.reverse();
  EXPECT_EQ(l1.size(), n);
  EXPECT_EQ(l2.size(), n);
  l1.merge(l2);
  EXPECT_EQ(l1.size(), 2 * n);
  EXPECT_EQ(l2.size(), 0);
  auto it = l1.begin();
  for (int i = 0; i < 2 * n; ++i, ++it) {
    EXPECT_EQ(*it, i);
  }
  l1.reverse();
  it = l1.begin();
  for (int i = 2 * n - 1; i >= 0; --i, ++it) {
    EXPECT_EQ(*it, i);
  }
  l1.sort();
  it = l1.begin();
  for (int i = 0; i < 2 * n; ++i, ++it) {
    EXPECT_EQ(*it, i);
  }

  SList<int> l1_copy(l1);
  l2.splice_after(l2.before_begin(), l1, l1.before_begin());
  EXPECT_EQ(l2.front(), l1_copy.front());
  l2.splice_after(l2.before_end(), l1);
  EXPECT_EQ(l2 == l1_copy, true);
}