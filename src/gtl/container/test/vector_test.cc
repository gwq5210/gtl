/**
 * @file vector_test.cc
 * @author gwq5210 (gwq5210@qq.com)
 * @brief vector单元测试
 * @date 2021-05-15
 *
 * @copyright Copyright (c) 2021. All rights reserved.
 */

#include <array>
#include <cstdio>
#include <deque>
#include <forward_list>
#include <list>
#include <unordered_map>
#include <vector>

#include "gtest/gtest.h"

#include "gtl/container/array.h"
#include "gtl/container/deque.h"
#include "gtl/container/list.h"
#include "gtl/container/slist.h"
#include "gtl/container/stack.h"
#include "gtl/container/vector.h"
#include "gtl/test/test_class.h"

using gtl::Array;
using gtl::Deque;
using gtl::List;
using gtl::SList;
using gtl::Vector;
using gtl::test::Person;

TEST(vector_test, constructor_assign_iterator_test) {
  Vector<int> empty_vec;
  EXPECT_EQ(empty_vec.size(), 0);
  EXPECT_EQ(empty_vec.empty(), true);

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
}

TEST(vector_test, modifiers_iterators_test) {
  int n = 10240;
  std::vector<int> std_vec(3);
  printf("std_vec capacity %zu\n", std_vec.capacity());
  std_vec.resize(10);
  printf("std_vec capacity %zu\n", std_vec.capacity());
  std_vec.push_back(10);
  printf("std_vec capacity %zu\n", std_vec.capacity());
  std_vec.resize(25);
  printf("std_vec capacity %zu\n", std_vec.capacity());
  std_vec.resize(60);
  printf("std_vec capacity %zu\n", std_vec.capacity());
  std_vec.push_back(10);
  printf("std_vec capacity %zu\n", std_vec.capacity());
  Vector<int> vec;
  vec.reserve(33);
  EXPECT_EQ(vec.capacity(), 64);
  vec.reserve(65);
  EXPECT_EQ(vec.capacity(), 128);
  for (int i = 0; i < n; ++i) {
    vec.push_back(i);
    EXPECT_EQ(vec.back(), i);
  }
  int i = 0;
  for (auto it = vec.begin(); it != vec.end(); ++it, ++i) {
    EXPECT_EQ(*it, i);
  }
  i = n - 1;
  for (auto it = vec.rbegin(); it != vec.rend(); ++it, --i) {
    EXPECT_EQ(*it, i);
  }
  EXPECT_EQ(vec.size(), n);
  EXPECT_EQ(vec.empty(), false);
  for (int i = 0; i < n; ++i) {
    vec.emplace(vec.begin(), i);
    EXPECT_EQ(vec.front(), i);
  }
  for (int i = 0; i < n; ++i) {
    vec.insert(vec.begin() + i / 2, i);
    EXPECT_EQ(vec[i / 2], i);
  }
  int x = vec[n + 1];
  vec.erase(vec.begin() + n);
  EXPECT_EQ(x, vec[n]);
  vec.erase(vec.begin(), vec.begin() + n);
  EXPECT_EQ(x, vec.front());
  vec.resize(n - 1, n);
  EXPECT_EQ(n - 1, vec.size());
  vec.resize(n, n);
  EXPECT_EQ(n, vec.size());
  EXPECT_EQ(n, vec.back());

  n = 16;
  Vector<int> new_vec(vec.begin(), vec.begin() + n);

  new_vec.insert(new_vec.begin() + n / 2, n, 100);
  for (int i = 0; i < n; ++i) {
    EXPECT_EQ(new_vec[i + n / 2], 100);
  }
  EXPECT_EQ(2 * n, new_vec.size());
  new_vec.erase(new_vec.begin() + n / 2, new_vec.end() - n / 2);
  EXPECT_EQ(n, new_vec.size());
  vec.clear();
  EXPECT_EQ(0, vec.size());
  EXPECT_EQ(true, vec.empty());

  new_vec.insert_safe(new_vec.begin(), new_vec.begin(), new_vec.end());
  for (size_t i = 0; i < new_vec.size() / 2; ++i) {
    EXPECT_EQ(new_vec[i], new_vec[i + new_vec.size() / 2]);
  }
  auto vec_copy = new_vec;
  new_vec.insert_safe(new_vec.begin() + new_vec.size() / 2, new_vec.begin(), new_vec.end());
  for (size_t i = 0; i < vec_copy.size(); ++i) {
    EXPECT_EQ(vec_copy[i], new_vec[vec_copy.size() / 2 + i]);
  }

  Vector<Person> erase_vec;
  for (int i = 0; i < n; ++i) {
    erase_vec.emplace_back(std::to_string(i));
  }
  printf("erase one end\n");
  erase_vec.erase(erase_vec.begin() + 2);
  EXPECT_EQ(erase_vec.size(), n - 1);
  for (size_t i = 0; i < erase_vec.size(); ++i) {
    EXPECT_EQ(erase_vec[i].name(), std::to_string(i >= 2 ? i + 1 : i));
  }
  printf("erase one end\n");
  erase_vec.erase(erase_vec.begin() + 2, erase_vec.begin() + 12);
  EXPECT_EQ(erase_vec.size(), n - 11);
  for (size_t i = 0; i < erase_vec.size(); ++i) {
    EXPECT_EQ(erase_vec[i].name(), std::to_string(i >= 2 ? i + 11 : i));
  }
  printf("erase range end\n");

  Vector<Person> persons;
  persons.reserve(n);
  for (int i = 0; i < n; ++i) {
    persons.emplace_back(std::to_string(i));
  }
  EXPECT_EQ(persons.size(), n);
  printf("persons capacity: %zu %zu\n", persons.capacity(), persons.size());
  persons.insert_safe(persons.begin(), persons.begin(), persons.end());
  for (size_t i = 0; i < persons.size() / 2; ++i) {
    EXPECT_EQ(persons[i].name(), std::to_string(i));
    EXPECT_EQ(persons[i].name(), persons[i + persons.size() / 2].name());
  }
  printf("persons capacity: %zu %zu\n", persons.capacity(), persons.size());

  PRINT_TYPE_SIZE((std::array<int, 0>::iterator));
  PRINT_TYPE_SIZE((Array<int, 0>::iterator));
  PRINT_TYPE_SIZE((std::array<int, 16>::iterator));
  PRINT_TYPE_SIZE((Array<int, 16>::iterator));
  PRINT_TYPE_SIZE((std::deque<int>::iterator));
  PRINT_TYPE_SIZE((Deque<int>::iterator));
  PRINT_TYPE_SIZE((std::list<int>::iterator));
  PRINT_TYPE_SIZE((List<int>::iterator));
  PRINT_TYPE_SIZE((std::forward_list<int>::iterator));
  PRINT_TYPE_SIZE((SList<int>::iterator));
  PRINT_TYPE_SIZE((std::vector<int>::iterator));
  PRINT_TYPE_SIZE((Vector<int>::iterator));
  PRINT_TYPE_SIZE((std::array<int, 0>));
  PRINT_TYPE_SIZE((Array<int, 0>));
  PRINT_TYPE_SIZE((std::array<int, 16>));
  PRINT_TYPE_SIZE((Array<int, 16>));
  PRINT_TYPE_SIZE((std::deque<int>));
  PRINT_TYPE_SIZE((Deque<int>));
  PRINT_TYPE_SIZE((std::list<int>));
  PRINT_TYPE_SIZE((List<int>));
  PRINT_TYPE_SIZE((std::forward_list<int>));
  PRINT_TYPE_SIZE((SList<int>));
  PRINT_TYPE_SIZE((std::vector<int>));
  PRINT_TYPE_SIZE((Vector<int>));
  PRINT_TYPE_SIZE((gtl::UStorage<int>));
  PRINT_TYPE_SIZE((gtl::UStorage<int, 16>));
  PRINT_TYPE_SIZE((std::unordered_map<int, int>));
}