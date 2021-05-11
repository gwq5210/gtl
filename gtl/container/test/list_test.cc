#include <cstdio>
#include <list>

#include "gtest/gtest.h"

#include "gtl_list.h"
#include "gtl_vector.h"

using gtl::List;
using gtl::Vector;

TEST(list_test, constructor_assign_iterator_test) {
  {
    List<int> l;
    EXPECT_EQ(l.empty(), true);
    EXPECT_EQ(l.size(), 0);
  }

  int n = 102400;
  int value = 10;
  Vector<int> vec;
  for (int i = 0; i < n; i++) {
    vec.push_back(i);
  }

  {
    List<int> l(n, value);
    EXPECT_EQ(l.empty(), false);
    EXPECT_EQ(l.size(), n);
    for (auto v: l) {
      EXPECT_EQ(v, value);
    }
    for (auto it = l.begin(); it != l.end(); it++) {
      EXPECT_EQ(*it, value);
    }
    for (auto it = l.rbegin(); it != l.rend(); it++) {
      EXPECT_EQ(*it, value);
    }
    EXPECT_EQ(l.front(), value);
    EXPECT_EQ(l.back(), value);
  }

  {
    List<int> l(vec.begin(), vec.end());
    EXPECT_EQ(l.empty(), false);
    EXPECT_EQ(l.size(), n);
    int i = 0;
    for (auto it = l.begin(); it != l.end(); it++, i++) {
      EXPECT_EQ(*it, vec[i]);
    }
    i = n - 1;
    for (auto it = l.rbegin(); it != l.rend(); it++, i--) {
      EXPECT_EQ(*it, vec[i]);
    }
    EXPECT_EQ(l.front(), vec.front());
    EXPECT_EQ(l.back(), vec.back());

    List<int> l_copy(l);
    EXPECT_EQ(l_copy.empty(), false);
    EXPECT_EQ(l_copy.size(), n);
    i = 0;
    for (auto it = l_copy.begin(); it != l_copy.end(); it++, i++) {
      EXPECT_EQ(*it, vec[i]);
    }
    i = n - 1;
    for (auto it = l_copy.rbegin(); it != l_copy.rend(); it++, i--) {
      EXPECT_EQ(*it, vec[i]);
    }
    EXPECT_EQ(l_copy.front(), vec.front());
    EXPECT_EQ(l_copy.back(), vec.back());

    List<int> l_copy2;
    l_copy2 = l;
    EXPECT_EQ(l_copy2.empty(), false);
    EXPECT_EQ(l_copy2.size(), n);
    i = 0;
    for (auto it = l_copy2.begin(); it != l_copy2.end(); it++, i++) {
      EXPECT_EQ(*it, vec[i]);
    }
    i = n - 1;
    for (auto it = l_copy2.rbegin(); it != l_copy2.rend(); it++, i--) {
      EXPECT_EQ(*it, vec[i]);
    }
    EXPECT_EQ(l_copy2.front(), vec.front());
    EXPECT_EQ(l_copy2.back(), vec.back());
  }

  {
    int n = 17;
    List<int> l{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
    EXPECT_EQ(l.empty(), false);
    EXPECT_EQ(l.size(), n);
    int i = 0;
    for (auto it = l.begin(); it != l.end(); it++, i++) {
      EXPECT_EQ(*it, vec[i]);
    }
    i = n - 1;
    for (auto it = l.rbegin(); it != l.rend(); it++, i--) {
      EXPECT_EQ(*it, vec[i]);
    }
    EXPECT_EQ(l.front(), 0);
    EXPECT_EQ(l.back(), n - 1);

    List<int> l_copy = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
    EXPECT_EQ(l_copy.empty(), false);
    EXPECT_EQ(l_copy.size(), n);
    i = 0;
    for (auto it = l_copy.begin(); it != l_copy.end(); it++, i++) {
      EXPECT_EQ(*it, vec[i]);
    }
    i = n - 1;
    for (auto it = l_copy.rbegin(); it != l_copy.rend(); it++, i--) {
      EXPECT_EQ(*it, vec[i]);
    }
    EXPECT_EQ(l_copy.front(), 0);
    EXPECT_EQ(l_copy.back(), n - 1);

    List<int> l_copy2;
    l_copy2 = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
    EXPECT_EQ(l_copy2.empty(), false);
    EXPECT_EQ(l_copy2.size(), n);
    i = 0;
    for (auto it = l_copy2.begin(); it != l_copy2.end(); it++, i++) {
      EXPECT_EQ(*it, vec[i]);
    }
    i = n - 1;
    for (auto it = l_copy2.rbegin(); it != l_copy2.rend(); it++, i--) {
      EXPECT_EQ(*it, vec[i]);
    }
    EXPECT_EQ(l_copy2.front(), 0);
    EXPECT_EQ(l_copy2.back(), n - 1);

    List<int> l_move(std::move(l));
    EXPECT_EQ(l_move.empty(), false);
    EXPECT_EQ(l_move.size(), n);
    i = 0;
    for (auto it = l_move.begin(); it != l_move.end(); it++, i++) {
      EXPECT_EQ(*it, vec[i]);
    }
    i = n - 1;
    for (auto it = l_move.rbegin(); it != l_move.rend(); it++, i--) {
      EXPECT_EQ(*it, vec[i]);
    }
    EXPECT_EQ(l_move.front(), 0);
    EXPECT_EQ(l_move.back(), n - 1);
    EXPECT_EQ(l.empty(), true);
    EXPECT_EQ(l.size(), 0);

    List<int> l_move2;
    l_move2 = std::move(l_copy);
    EXPECT_EQ(l_move2.empty(), false);
    EXPECT_EQ(l_move2.size(), n);
    i = 0;
    for (auto it = l_move2.begin(); it != l_move2.end(); it++, i++) {
      EXPECT_EQ(*it, vec[i]);
    }
    i = n - 1;
    for (auto it = l_move2.rbegin(); it != l_move2.rend(); it++, i--) {
      EXPECT_EQ(*it, vec[i]);
    }
    EXPECT_EQ(l_move2.front(), 0);
    EXPECT_EQ(l_move2.back(), n - 1);
    EXPECT_EQ(l_copy.empty(), true);
    EXPECT_EQ(l_copy.size(), 0);
  }
}

TEST(list_test, modifiers_iterators_test) {
  int n = 102400;
  List<int> l;
  for (int i = 0; i < n; i++) {
    l.push_back(i);
    EXPECT_EQ(l.back(), i);
  }
  EXPECT_EQ(l.size(), n);
  int i = 0;
  for (auto v: l) {
    EXPECT_EQ(v, i);
    i++;
  }
  l.erase(l.begin(), l.end());
  EXPECT_EQ(l.size(), 0);
  EXPECT_EQ(l.empty(), true);
}

TEST(list_test, operations_test) {
  int n = 10;
  List<int> l;
  for (int i = 0; i < n; i++) {
    l.push_back(i * 2);
    EXPECT_EQ(l.back(), i * 2);
  }
  for (auto v: l) {
    printf("%d,", v);
  }
  printf("\n");
  EXPECT_EQ(l.size(), n);
  l.merge(List<int>{2, 3, 4, 5, 6});
  EXPECT_EQ(l.size(), n + 5);
  for (auto v: l) {
    printf("%d,", v);
  }
  printf("\n");
  l.reverse();
  for (auto v: l) {
    printf("%d,", v);
  }
  printf("\n");
  l.sort();
  for (auto v: l) {
    printf("%d,", v);
  }
  printf("\n");
}