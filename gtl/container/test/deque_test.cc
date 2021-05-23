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
using gtl::Vector;

TEST(deque_test, constructor_assign_iterator_test) {
  int n = 1025;
  Deque<int> dq;
  std::deque<int> a;
  EXPECT_EQ(dq.size(), 0);
  EXPECT_EQ(dq.empty(), true);
  for (int i = 0; i < n; ++i) {
    dq.emplace_back(i);
    EXPECT_EQ(dq.back(), i);
  }
  for (int i = 0; i < n; ++i) {
    dq.emplace_front(i);
    EXPECT_EQ(dq.front(), i);
  }
  // for (int i = 0; i < n; i++) {
  //   EXPECT_EQ(dq[i], i);
  // }
  printf("test2\n");
  EXPECT_EQ(dq.size(), 2 * n);
  printf("test3\n");
  EXPECT_EQ(dq.empty(), false);
  printf("test4\n");

  /*
  int vec_size = 30;
  int value = 0;
  {
    Vector<int> vec_with_size(vec_size);
    EXPECT_EQ(vec_with_size.size(), vec_size);
    EXPECT_EQ(vec_with_size.capacity(), 32);
    EXPECT_EQ(vec_with_size.empty(), false);
    for (auto v : vec_with_size) {
      EXPECT_EQ(v, value);
    }
    for (int i = 0; i < vec_size; ++i) {
      EXPECT_EQ(vec_with_size[i], value);
      EXPECT_EQ(vec_with_size.at(i), value);
    }
    for (auto it = vec_with_size.begin(); it != vec_with_size.end(); ++it) {
      EXPECT_EQ(*it, value);
    }
    for (auto it = vec_with_size.rbegin(); it != vec_with_size.rend(); ++it) {
      EXPECT_EQ(*it, value);
    }
    EXPECT_EQ(*vec_with_size.begin(), value);
    EXPECT_EQ(vec_with_size.front(), value);
    EXPECT_EQ(vec_with_size.back(), value);
  }

  {
    value = 10;
    Vector<int> vec_with_size(vec_size, value);
    EXPECT_EQ(vec_with_size.size(), vec_size);
    EXPECT_EQ(vec_with_size.capacity(), 32);
    EXPECT_EQ(vec_with_size.empty(), false);
    for (auto v : vec_with_size) {
      EXPECT_EQ(v, value);
    }
    for (int i = 0; i < vec_size; ++i) {
      EXPECT_EQ(vec_with_size[i], value);
      EXPECT_EQ(vec_with_size.at(i), value);
    }
    for (auto it = vec_with_size.begin(); it != vec_with_size.end(); ++it) {
      EXPECT_EQ(*it, value);
    }
    for (auto it = vec_with_size.rbegin(); it != vec_with_size.rend(); ++it) {
      EXPECT_EQ(*it, value);
    }
    EXPECT_EQ(*vec_with_size.begin(), value);
    EXPECT_EQ(vec_with_size.front(), value);
    EXPECT_EQ(vec_with_size.back(), value);
  }

  vec_size = 17;
  Vector<int> vec_init_list{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
  EXPECT_EQ(vec_init_list.size(), vec_size);
  EXPECT_EQ(vec_init_list.capacity(), 32);
  EXPECT_EQ(vec_init_list.empty(), false);
  for (int i = 0; i < vec_size; ++i) {
    EXPECT_EQ(vec_init_list[i], i);
    EXPECT_EQ(vec_init_list.at(i), i);
  }
  value = 0;
  for (auto it = vec_init_list.begin(); it != vec_init_list.end(); ++it, ++value) {
    EXPECT_EQ(*it, value);
  }
  value = 16;
  for (auto it = vec_init_list.rbegin(); it != vec_init_list.rend(); ++it, --value) {
    EXPECT_EQ(*it, value);
  }
  EXPECT_EQ(*vec_init_list.begin(), 0);
  EXPECT_EQ(vec_init_list.front(), 0);
  EXPECT_EQ(vec_init_list.back(), 16);

  {
    Vector<int> vec_assign_init_list = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
    EXPECT_EQ(vec_assign_init_list.size(), vec_size);
    EXPECT_EQ(vec_assign_init_list.capacity(), 32);
    EXPECT_EQ(vec_assign_init_list.empty(), false);
    for (int i = 0; i < vec_size; ++i) {
      EXPECT_EQ(vec_assign_init_list[i], i);
      EXPECT_EQ(vec_assign_init_list.at(i), i);
    }
    value = 0;
    for (auto it = vec_assign_init_list.begin(); it != vec_assign_init_list.end(); ++it, ++value) {
      EXPECT_EQ(*it, value);
    }
    value = 16;
    for (auto it = vec_assign_init_list.rbegin(); it != vec_assign_init_list.rend(); ++it, --value) {
      EXPECT_EQ(*it, value);
    }
    EXPECT_EQ(*vec_assign_init_list.begin(), 0);
    EXPECT_EQ(vec_assign_init_list.front(), 0);
    EXPECT_EQ(vec_assign_init_list.back(), 16);
  }

  {
    Vector<int> vec_assign_init_list;
    vec_assign_init_list = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
    EXPECT_EQ(vec_assign_init_list.size(), vec_size);
    EXPECT_EQ(vec_assign_init_list.capacity(), 32);
    EXPECT_EQ(vec_assign_init_list.empty(), false);
    for (int i = 0; i < vec_size; ++i) {
      EXPECT_EQ(vec_assign_init_list[i], i);
      EXPECT_EQ(vec_assign_init_list.at(i), i);
    }
    value = 0;
    for (auto it = vec_assign_init_list.begin(); it != vec_assign_init_list.end(); ++it, ++value) {
      EXPECT_EQ(*it, value);
    }
    value = 16;
    for (auto it = vec_assign_init_list.rbegin(); it != vec_assign_init_list.rend(); ++it, --value) {
      EXPECT_EQ(*it, value);
    }
    EXPECT_EQ(*vec_assign_init_list.begin(), 0);
    EXPECT_EQ(vec_assign_init_list.front(), 0);
    EXPECT_EQ(vec_assign_init_list.back(), 16);
  }

  Vector<int> vec_range(vec_init_list.begin(), vec_init_list.end());
  EXPECT_EQ(vec_range.size(), vec_size);
  EXPECT_EQ(vec_range.capacity(), 32);
  EXPECT_EQ(vec_range.empty(), false);
  for (int i = 0; i < vec_size; ++i) {
    EXPECT_EQ(vec_range[i], i);
  }

  Vector<int> vec_copy(vec_init_list);
  EXPECT_EQ(vec_copy.size(), vec_size);
  EXPECT_EQ(vec_copy.capacity(), 32);
  EXPECT_EQ(vec_copy.empty(), false);
  for (int i = 0; i < vec_size; ++i) {
    EXPECT_EQ(vec_copy[i], i);
  }
  EXPECT_EQ(vec_copy, vec_init_list);
  EXPECT_EQ(vec_copy < vec_init_list, false);
  EXPECT_EQ(vec_copy > vec_init_list, false);
  EXPECT_EQ(vec_copy <= vec_init_list, true);
  EXPECT_EQ(vec_copy >= vec_init_list, true);

  {
    Vector<int> vec_assign = vec_init_list;
    EXPECT_EQ(vec_assign.size(), vec_size);
    EXPECT_EQ(vec_assign.capacity(), 32);
    EXPECT_EQ(vec_assign.empty(), false);
    for (int i = 0; i < vec_size; ++i) {
      EXPECT_EQ(vec_assign[i], i);
    }
  }

  {
    Vector<int> vec_assign;
    vec_assign = vec_init_list;
    EXPECT_EQ(vec_assign.size(), vec_size);
    EXPECT_EQ(vec_assign.capacity(), 32);
    EXPECT_EQ(vec_assign.empty(), false);
    for (int i = 0; i < vec_size; ++i) {
      EXPECT_EQ(vec_assign[i], i);
    }
  }

  Vector<int> vec_move(std::move(vec_init_list));
  EXPECT_EQ(vec_move.size(), vec_size);
  EXPECT_EQ(vec_move.capacity(), 32);
  EXPECT_EQ(vec_move.empty(), false);
  for (int i = 0; i < vec_size; ++i) {
    EXPECT_EQ(vec_move[i], i);
  }
  EXPECT_EQ(vec_init_list.size(), 0);
  EXPECT_EQ(vec_init_list.capacity(), 0);
  EXPECT_EQ(vec_init_list.empty(), true);

  Vector<int> vec_move2;
  vec_move2 = std::move(vec_move);
  EXPECT_EQ(vec_move2.size(), vec_size);
  EXPECT_EQ(vec_move2.capacity(), 32);
  EXPECT_EQ(vec_move2.empty(), false);
  for (int i = 0; i < vec_size; ++i) {
    EXPECT_EQ(vec_move2[i], i);
  }
  EXPECT_EQ(vec_move.size(), 0);
  EXPECT_EQ(vec_move.capacity(), 0);
  EXPECT_EQ(vec_move.empty(), true);
  */
}