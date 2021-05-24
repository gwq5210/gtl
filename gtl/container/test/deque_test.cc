/**
 * @file deque_test.cc
 * @author gwq5210 (gwq5210@qq.com)
 * @brief deque的单元测试
 * @date 2021-05-18
 *
 * @copyright Copyright (c) 2021. All rights reserved.
 */

#include <array>
#include <cstdio>
#include <deque>
#include <forward_list>
#include <list>
#include <vector>

#include "gtest/gtest.h"

#include "gtl_array.h"
#include "gtl_deque.h"
#include "gtl_list.h"
#include "gtl_slist.h"
#include "gtl_stack.h"
#include "gtl_vector.h"

using gtl::Array;
using gtl::Deque;
using gtl::List;
using gtl::SList;

TEST(deque_test, constructor_assign_iterator_test) {
  Deque<int> empty_dq;
  EXPECT_EQ(empty_dq.size(), 0);
  EXPECT_EQ(empty_dq.empty(), true);

  int n = 102400;
  int value = 0;
  {
    Deque<int> dq(n);
    printf("deque constructed\n");
    EXPECT_EQ(dq.size(), n);
    EXPECT_EQ(dq.empty(), false);
    for (auto v : dq) {
      EXPECT_EQ(v, value);
    }
    for (int i = 0; i < n; ++i) {
      EXPECT_EQ(dq[i], value);
      EXPECT_EQ(dq.at(i), value);
    }
    for (auto it = dq.begin(); it != dq.end(); ++it) {
      EXPECT_EQ(*it, value);
    }
    for (auto it = dq.rbegin(); it != dq.rend(); ++it) {
      EXPECT_EQ(*it, value);
    }
    EXPECT_EQ(*dq.begin(), value);
    EXPECT_EQ(dq.front(), value);
    EXPECT_EQ(dq.back(), value);
  }

  {
    value = 10;
    Deque<int> dq(n, value);
    EXPECT_EQ(dq.size(), n);
    EXPECT_EQ(dq.empty(), false);
    for (auto v : dq) {
      EXPECT_EQ(v, value);
    }
    for (int i = 0; i < n; ++i) {
      EXPECT_EQ(dq[i], value);
      EXPECT_EQ(dq.at(i), value);
    }
    for (auto it = dq.begin(); it != dq.end(); ++it) {
      EXPECT_EQ(*it, value);
    }
    for (auto it = dq.rbegin(); it != dq.rend(); ++it) {
      EXPECT_EQ(*it, value);
    }
    EXPECT_EQ(*dq.begin(), value);
    EXPECT_EQ(dq.front(), value);
    EXPECT_EQ(dq.back(), value);
  }

  n = 17;
  Deque<int> dq_il{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
  EXPECT_EQ(dq_il.size(), n);
  EXPECT_EQ(dq_il.empty(), false);
  for (int i = 0; i < n; ++i) {
    EXPECT_EQ(dq_il[i], i);
    EXPECT_EQ(dq_il.at(i), i);
  }
  value = 0;
  for (auto it = dq_il.begin(); it != dq_il.end(); ++it, ++value) {
    EXPECT_EQ(*it, value);
  }
  value = 16;
  for (auto it = dq_il.rbegin(); it != dq_il.rend(); ++it, --value) {
    EXPECT_EQ(*it, value);
  }
  EXPECT_EQ(*dq_il.begin(), 0);
  EXPECT_EQ(dq_il.front(), 0);
  EXPECT_EQ(dq_il.back(), 16);

  {
    Deque<int> dq_assign_init_list = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
    EXPECT_EQ(dq_assign_init_list.size(), n);
    EXPECT_EQ(dq_assign_init_list.empty(), false);
    for (int i = 0; i < n; ++i) {
      EXPECT_EQ(dq_assign_init_list[i], i);
      EXPECT_EQ(dq_assign_init_list.at(i), i);
    }
    value = 0;
    for (auto it = dq_assign_init_list.begin(); it != dq_assign_init_list.end(); ++it, ++value) {
      EXPECT_EQ(*it, value);
    }
    value = 16;
    for (auto it = dq_assign_init_list.rbegin(); it != dq_assign_init_list.rend(); ++it, --value) {
      EXPECT_EQ(*it, value);
    }
    EXPECT_EQ(*dq_assign_init_list.begin(), 0);
    EXPECT_EQ(dq_assign_init_list.front(), 0);
    EXPECT_EQ(dq_assign_init_list.back(), 16);
  }

  {
    Deque<int> dq_assign_init_list;
    dq_assign_init_list = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
    EXPECT_EQ(dq_assign_init_list.size(), n);
    EXPECT_EQ(dq_assign_init_list.empty(), false);
    for (int i = 0; i < n; ++i) {
      EXPECT_EQ(dq_assign_init_list[i], i);
      EXPECT_EQ(dq_assign_init_list.at(i), i);
    }
    value = 0;
    for (auto it = dq_assign_init_list.begin(); it != dq_assign_init_list.end(); ++it, ++value) {
      EXPECT_EQ(*it, value);
    }
    value = 16;
    for (auto it = dq_assign_init_list.rbegin(); it != dq_assign_init_list.rend(); ++it, --value) {
      EXPECT_EQ(*it, value);
    }
    EXPECT_EQ(*dq_assign_init_list.begin(), 0);
    EXPECT_EQ(dq_assign_init_list.front(), 0);
    EXPECT_EQ(dq_assign_init_list.back(), 16);
  }

  Deque<int> dq_range(dq_il.begin(), dq_il.end());
  EXPECT_EQ(dq_range.size(), n);
  EXPECT_EQ(dq_range.empty(), false);
  for (int i = 0; i < n; ++i) {
    EXPECT_EQ(dq_range[i], i);
  }

  Deque<int> dq_copy(dq_il);
  EXPECT_EQ(dq_copy.size(), n);
  EXPECT_EQ(dq_copy.empty(), false);
  for (int i = 0; i < n; ++i) {
    EXPECT_EQ(dq_copy[i], i);
  }
  EXPECT_EQ(dq_copy, dq_il);
  EXPECT_EQ(dq_copy < dq_il, false);
  EXPECT_EQ(dq_copy > dq_il, false);
  EXPECT_EQ(dq_copy <= dq_il, true);
  EXPECT_EQ(dq_copy >= dq_il, true);

  {
    Deque<int> dq_assign = dq_il;
    EXPECT_EQ(dq_assign.size(), n);
    EXPECT_EQ(dq_assign.empty(), false);
    for (int i = 0; i < n; ++i) {
      EXPECT_EQ(dq_assign[i], i);
    }
  }

  {
    Deque<int> dq_assign;
    dq_assign = dq_il;
    EXPECT_EQ(dq_assign.size(), n);
    EXPECT_EQ(dq_assign.empty(), false);
    for (int i = 0; i < n; ++i) {
      EXPECT_EQ(dq_assign[i], i);
    }
  }

  Deque<int> dq_move(std::move(dq_il));
  EXPECT_EQ(dq_move.size(), n);
  EXPECT_EQ(dq_move.empty(), false);
  for (int i = 0; i < n; ++i) {
    EXPECT_EQ(dq_move[i], i);
  }
  EXPECT_EQ(dq_il.size(), 0);
  EXPECT_EQ(dq_il.empty(), true);

  Deque<int> dq_move2;
  dq_move2 = std::move(dq_move);
  EXPECT_EQ(dq_move2.size(), n);
  EXPECT_EQ(dq_move2.empty(), false);
  for (int i = 0; i < n; ++i) {
    EXPECT_EQ(dq_move2[i], i);
  }
  EXPECT_EQ(dq_move.size(), 0);
  EXPECT_EQ(dq_move.empty(), true);
}