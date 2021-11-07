/**
 * @file array_test.cc
 * @author gwq5210 (gwq5210@qq.com)
 * @brief array单元测试
 * @date 2021-05-15
 *
 * @copyright Copyright (c) 2021. All rights reserved.
 */

#include <array>
#include <cstdio>
#include <list>
#include <vector>

#include "gtest/gtest.h"

#include "gtl/array.h"
#include "gtl/list.h"
#include "gtl/vector.h"

using gtl::Array;
using gtl::List;
using gtl::Vector;

TEST(array_test, constructor_operations_test) {
  Array<int, 0> empty_arr;
  EXPECT_EQ(empty_arr.size(), 0);
  EXPECT_EQ(empty_arr.empty(), true);

  constexpr int n = 1024;
  int value = 0;
  Array<int, n> arr;
  EXPECT_EQ(arr.size(), n);
  EXPECT_EQ(arr.empty(), false);
  arr.fill(value);
  for (auto v : arr) {
    EXPECT_EQ(v, value);
  }

  {
    value = 10;
    arr.fill(value);
    for (auto v : arr) {
      EXPECT_EQ(v, value);
    }

    Array<int, n> arr_swap;
    arr_swap.fill(0);
    arr.swap(arr_swap);
    for (int i = 0; i < n; ++i) {
      EXPECT_EQ(arr[i], 0);
      EXPECT_EQ(arr_swap[i], value);
    }
    arr.swap(arr_swap);
  }

  {
    Array<int, n> arr_copy(arr);
    EXPECT_EQ(arr_copy.size(), arr.size());
    EXPECT_EQ(arr_copy.empty(), arr.empty());
    EXPECT_EQ(arr, arr_copy);
    EXPECT_EQ(arr < arr_copy, false);
    EXPECT_EQ(arr > arr_copy, false);
    EXPECT_EQ(arr <= arr_copy, true);
    EXPECT_EQ(arr >= arr_copy, true);
    for (auto v : arr_copy) {
      EXPECT_EQ(v, value);
    }
    for (int i = 0; i < n; ++i) {
      EXPECT_EQ(arr_copy[i], value);
      EXPECT_EQ(arr_copy.at(i), value);
    }
    for (auto it = arr_copy.begin(); it != arr_copy.end(); ++it) {
      EXPECT_EQ(*it, value);
    }
    for (auto it = arr_copy.rbegin(); it != arr_copy.rend(); ++it) {
      EXPECT_EQ(*it, value);
    }
    EXPECT_EQ(*arr_copy.begin(), value);
    EXPECT_EQ(arr_copy.front(), value);
    EXPECT_EQ(arr_copy.back(), value);
  }

  {
    Array<int, n> arr_copy;
    arr_copy = arr;
    EXPECT_EQ(arr_copy.size(), arr.size());
    EXPECT_EQ(arr_copy.empty(), arr.empty());
    for (auto v : arr_copy) {
      EXPECT_EQ(v, value);
    }
    for (int i = 0; i < n; ++i) {
      EXPECT_EQ(arr_copy[i], value);
      EXPECT_EQ(arr_copy.at(i), value);
    }
    for (auto it = arr_copy.begin(); it != arr_copy.end(); ++it) {
      EXPECT_EQ(*it, value);
    }
    for (auto it = arr_copy.rbegin(); it != arr_copy.rend(); ++it) {
      EXPECT_EQ(*it, value);
    }
    EXPECT_EQ(*arr_copy.begin(), value);
    EXPECT_EQ(arr_copy.front(), value);
    EXPECT_EQ(arr_copy.back(), value);
  }

  {
    constexpr int n = 17;
    Array<int, n> arr_il{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
    EXPECT_EQ(arr_il.size(), n);
    EXPECT_EQ(arr_il.empty(), false);
    for (int i = 0; i < n; ++i) {
      EXPECT_EQ(arr_il[i], i);
      EXPECT_EQ(arr_il.at(i), i);
    }
    value = 0;
    for (auto it = arr_il.begin(); it != arr_il.end(); ++it, ++value) {
      EXPECT_EQ(*it, value);
    }
    value = 16;
    for (auto it = arr_il.rbegin(); it != arr_il.rend(); ++it, --value) {
      EXPECT_EQ(*it, value);
    }
    EXPECT_EQ(*arr_il.begin(), 0);
    EXPECT_EQ(arr_il.front(), 0);
    EXPECT_EQ(arr_il.back(), 16);
  }

  {
    constexpr int n = 17;
    Array<int, n> arr_il = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
    EXPECT_EQ(arr_il.size(), n);
    EXPECT_EQ(arr_il.empty(), false);
    for (int i = 0; i < n; ++i) {
      EXPECT_EQ(arr_il[i], i);
      EXPECT_EQ(arr_il.at(i), i);
    }
    value = 0;
    for (auto it = arr_il.begin(); it != arr_il.end(); ++it, ++value) {
      EXPECT_EQ(*it, value);
    }
    value = 16;
    for (auto it = arr_il.rbegin(); it != arr_il.rend(); ++it, --value) {
      EXPECT_EQ(*it, value);
    }
    EXPECT_EQ(*arr_il.begin(), 0);
    EXPECT_EQ(arr_il.front(), 0);
    EXPECT_EQ(arr_il.back(), 16);
  }

  {
    constexpr int n = 17;
    Array<int, n> arr_il;
    arr_il = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
    EXPECT_EQ(arr_il.size(), n);
    EXPECT_EQ(arr_il.empty(), false);
    for (int i = 0; i < n; ++i) {
      EXPECT_EQ(arr_il[i], i);
      EXPECT_EQ(arr_il.at(i), i);
    }
    value = 0;
    for (auto it = arr_il.begin(); it != arr_il.end(); ++it, ++value) {
      EXPECT_EQ(*it, value);
    }
    value = 16;
    for (auto it = arr_il.rbegin(); it != arr_il.rend(); ++it, --value) {
      EXPECT_EQ(*it, value);
    }
    EXPECT_EQ(*arr_il.begin(), 0);
    EXPECT_EQ(arr_il.front(), 0);
    EXPECT_EQ(arr_il.back(), 16);
  }
}