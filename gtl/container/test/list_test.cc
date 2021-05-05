#include <cstdio>
#include <list>

#include "gtest/gtest.h"

#include "gtl_list.h"

using gtl::List;

TEST(list_test, modifiers_iterators_test) {
  int n = 102400;
  List<int> list;
  for (int i = 0; i < n; i++) {
    list.push_back(i);
    EXPECT_EQ(list.back(), i);
  }
  EXPECT_EQ(list.size(), n);
  int i = 0;
  for (auto v: list) {
    EXPECT_EQ(v, i);
    i++;
  }
  list.erase(list.begin(), list.end());
  EXPECT_EQ(list.size(), 0);
  EXPECT_EQ(list.empty(), true);
}

TEST(list_test, operations_test) {
  int n = 10;
  List<int> list;
  for (int i = 0; i < n; i++) {
    list.push_back(i * 2);
    EXPECT_EQ(list.back(), i * 2);
  }
  for (auto v: list) {
    printf("%d,", v);
  }
  printf("\n");
  EXPECT_EQ(list.size(), n);
  list.merge(List<int>{2, 3, 4, 5, 6});
  EXPECT_EQ(list.size(), n + 5);
  for (auto v: list) {
    printf("%d,", v);
  }
  printf("\n");
  list.reverse();
  for (auto v: list) {
    printf("%d,", v);
  }
  printf("\n");
  list.sort();
  for (auto v: list) {
    printf("%d,", v);
  }
  printf("\n");
}