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

#include "gtl/container/array.h"
#include "gtl/container/deque.h"
#include "gtl/container/list.h"
#include "gtl/container/slist.h"
#include "gtl/container/stack.h"
#include "gtl/container/vector.h"
#include "gtl/memory/default_allocator.h"
#include "gtl/test/test_class.h"

using gtl::Array;
using gtl::Deque;
using gtl::List;
using gtl::SList;
using gtl::test::Person;

TEST(DequeTest, ConstructorAssignIteratorTest) {
  Deque<int> empty_dq;
  EXPECT_EQ(empty_dq.size(), 0);
  EXPECT_EQ(empty_dq.empty(), true);

  int n = 1024;
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

TEST(DequeTest, ModifiersIteratorsTest) {
  int n = 1025;
  Deque<int> dq;
  for (int i = 0; i < n; ++i) {
    dq.push_back(i);
    EXPECT_EQ(dq.back(), i);
  }
  int i = 0;
  for (auto it = dq.begin(); it != dq.end(); ++it, ++i) {
    EXPECT_EQ(*it, i);
  }
  i = n - 1;
  for (auto it = dq.rbegin(); it != dq.rend(); ++it, --i) {
    EXPECT_EQ(*it, i);
  }
  EXPECT_EQ(dq.size(), n);
  EXPECT_EQ(dq.empty(), false);
  for (int i = 0; i < n; ++i) {
    dq.emplace(dq.begin(), i);
    EXPECT_EQ(dq.front(), i);
  }
  for (int i = 0; i < n; ++i) {
    dq.emplace(dq.end(), i);
    EXPECT_EQ(dq.back(), i);
  }
  for (int i = 0; i < n; ++i) {
    dq.insert(dq.begin() + i / 2, i);
    EXPECT_EQ(dq[i / 2], i);
  }
  int x = dq[n + 1];
  dq.erase(dq.begin() + n);
  EXPECT_EQ(x, dq[n]);
  dq.erase(dq.begin(), dq.begin() + n);
  EXPECT_EQ(x, dq.front());
  dq.resize(n - 1, n);
  EXPECT_EQ(n - 1, dq.size());
  dq.resize(n, n);
  EXPECT_EQ(n, dq.size());
  EXPECT_EQ(n, dq.back());

  n = 16;
  Deque<int> new_dq(dq.begin(), dq.begin() + n);
  printf("dq_new end\n");

  new_dq.insert(new_dq.begin() + n / 2, n, 100);
  for (int i = 0; i < n; ++i) {
    EXPECT_EQ(new_dq[i + n / 2], 100);
  }
  EXPECT_EQ(2 * n, new_dq.size());
  new_dq.erase(new_dq.begin() + n / 2, new_dq.end() - n / 2);
  EXPECT_EQ(n, new_dq.size());
  dq.clear();
  EXPECT_EQ(0, dq.size());
  EXPECT_EQ(true, dq.empty());

  Deque<Person> erase_dq;
  for (int i = 0; i < n; ++i) {
    erase_dq.emplace_back(std::to_string(i));
  }
  printf("erase one end\n");
  erase_dq.erase(erase_dq.begin() + 2);
  EXPECT_EQ(erase_dq.size(), n - 1);
  for (size_t i = 0; i < erase_dq.size(); ++i) {
    EXPECT_EQ(erase_dq[i].name(), std::to_string(i >= 2 ? i + 1 : i));
  }
  printf("erase one end\n");
  erase_dq.erase(erase_dq.begin() + 2, erase_dq.begin() + 12);
  EXPECT_EQ(erase_dq.size(), n - 11);
  for (size_t i = 0; i < erase_dq.size(); ++i) {
    EXPECT_EQ(erase_dq[i].name(), std::to_string(i >= 2 ? i + 11 : i));
  }
  printf("erase range end\n");

  Deque<Person> persons;
  for (int i = 0; i < n; ++i) {
    persons.emplace_back(std::to_string(i));
  }
  EXPECT_EQ(persons.size(), n);
  printf("dq test end\n");
}