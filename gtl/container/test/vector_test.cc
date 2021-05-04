#include <cstdio>
#include <vector>

#include "gtest/gtest.h"

#include "gtl_vector.h"

using gtl::Vector;

struct Person {
  Person(const Person& other): id_(other.id_) {
    // printf("%s %d\n", __FUNCTION__, id_);
  }
  Person(int id = 0): id_(id) {
    // printf("%s %d\n", __FUNCTION__, id_);
  }
  int id_;
};

template <class Iterator>
void print(Iterator first, Iterator end) {
  auto it = first;
  printf("[");
  while (it != end) {
    if (it != first) {
      printf(",");
    }
    printf("%d", *it);
    it++;
  }
  printf("]\n");
}

template<class T>
void print(const Vector<T> &vec) {
  printf("(size = %zu, capacity = %zu)", vec.size(), vec.capacity());
  print(vec.begin(), vec.end());
}

TEST(vector_test, constructor_assign_test) {
  Vector<int> empty_vec;
  EXPECT_EQ(empty_vec.size(), 0);
  EXPECT_EQ(empty_vec.empty(), true);

  int vec_size = 30;
  int value = 10;
  Vector<int> vec_with_size(vec_size, value);
  EXPECT_EQ(vec_with_size.size(), vec_size);
  EXPECT_EQ(vec_with_size.capacity(), 32);
  EXPECT_EQ(vec_with_size.empty(), false);
  for (auto v: vec_with_size) {
    EXPECT_EQ(v, value);
  }

  vec_size = 17;
  Vector<int> vec_init_list{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
  EXPECT_EQ(vec_init_list.size(), vec_size);
  EXPECT_EQ(vec_init_list.capacity(), 32);
  EXPECT_EQ(vec_init_list.empty(), false);
  for (int i = 0; i < vec_size; i++) {
    EXPECT_EQ(vec_init_list[i], i);
  }

  Vector<int> vec_assign_init_list = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
  EXPECT_EQ(vec_assign_init_list.size(), vec_size);
  EXPECT_EQ(vec_assign_init_list.capacity(), 32);
  EXPECT_EQ(vec_assign_init_list.empty(), false);
  for (int i = 0; i < vec_size; i++) {
    EXPECT_EQ(vec_assign_init_list[i], i);
  }

  Vector<int> vec_assign_init_list2;
  vec_assign_init_list2 = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
  EXPECT_EQ(vec_assign_init_list2.size(), vec_size);
  EXPECT_EQ(vec_assign_init_list2.capacity(), 32);
  EXPECT_EQ(vec_assign_init_list2.empty(), false);
  for (int i = 0; i < vec_size; i++) {
    EXPECT_EQ(vec_assign_init_list2[i], i);
  }

  Vector<int> vec_range(vec_init_list.begin(), vec_init_list.end());
  EXPECT_EQ(vec_range.size(), vec_size);
  EXPECT_EQ(vec_range.capacity(), 32);
  EXPECT_EQ(vec_range.empty(), false);
  for (int i = 0; i < vec_size; i++) {
    EXPECT_EQ(vec_range[i], i);
  }

  Vector<int> vec_copy(vec_init_list);
  EXPECT_EQ(vec_copy.size(), vec_size);
  EXPECT_EQ(vec_copy.capacity(), 32);
  EXPECT_EQ(vec_copy.empty(), false);
  for (int i = 0; i < vec_size; i++) {
    EXPECT_EQ(vec_copy[i], i);
  }

  Vector<int> vec_assign = vec_init_list;
  EXPECT_EQ(vec_assign.size(), vec_size);
  EXPECT_EQ(vec_assign.capacity(), 32);
  EXPECT_EQ(vec_assign.empty(), false);
  for (int i = 0; i < vec_size; i++) {
    EXPECT_EQ(vec_assign[i], i);
  }

  Vector<int> vec_assign2;
  vec_assign2 = vec_init_list;
  EXPECT_EQ(vec_assign2.size(), vec_size);
  EXPECT_EQ(vec_assign2.capacity(), 32);
  EXPECT_EQ(vec_assign2.empty(), false);
  for (int i = 0; i < vec_size; i++) {
    EXPECT_EQ(vec_assign2[i], i);
  }

  Vector<int> vec_move(std::move(vec_init_list));
  EXPECT_EQ(vec_move.size(), vec_size);
  EXPECT_EQ(vec_move.capacity(), 32);
  EXPECT_EQ(vec_move.empty(), false);
  for (int i = 0; i < vec_size; i++) {
    EXPECT_EQ(vec_move[i], i);
  }
  EXPECT_EQ(vec_init_list.size(), 0);
  EXPECT_EQ(vec_init_list.capacity(), 0);
  EXPECT_EQ(vec_init_list.empty(), true);

  Vector<int> vec_move2(std::move(vec_move));
  EXPECT_EQ(vec_move2.size(), vec_size);
  EXPECT_EQ(vec_move2.capacity(), 32);
  EXPECT_EQ(vec_move2.empty(), false);
  for (int i = 0; i < vec_size; i++) {
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
  for (int i = 0; i < n; i++) {
    vec.push_back(i);
    EXPECT_EQ(vec.back(), i);
  }
  int i = 0;
  for (auto it = vec.begin(); it != vec.end(); it++, i++) {
    EXPECT_EQ(*it, i);
  }
  i = n - 1;
  for (auto it = vec.rbegin(); it != vec.rend(); it++, i--) {
    EXPECT_EQ(*it, i);
  }
  EXPECT_EQ(vec.size(), n);
  EXPECT_EQ(vec.empty(), false);
  for (int i = 0; i < n; i++) {
    vec.emplace(vec.begin(), i);
    EXPECT_EQ(vec.front(), i);
  }
  for (int i = 0; i < n; i++) {
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

  vec.clear();
  EXPECT_EQ(0, vec.size());
  EXPECT_EQ(true, vec.empty());

  new_vec.insert_safe(new_vec.begin(), new_vec.begin(), new_vec.end());
  for (size_t i = 0; i < new_vec.size() / 2; i++) {
    EXPECT_EQ(new_vec[i], new_vec[i + new_vec.size() / 2]);
  }
  auto vec_copy = new_vec;
  new_vec.insert_safe(new_vec.begin() + new_vec.size() / 2, new_vec.begin(), new_vec.end());
  for (size_t i = 0; i < vec_copy.size(); i++) {
    EXPECT_EQ(vec_copy[i], new_vec[vec_copy.size() / 2 + i]);
  }

  Vector<Person> persons;
  persons.reserve(n);
  for (int i = 0; i < n; i++) {
    persons.emplace_back(i);
  }
  printf("persons capacity: %zu %zu\n", persons.capacity(), persons.size());
  persons.insert_safe(persons.begin(), persons.begin(), persons.end());
  for (size_t i = 0; i < persons.size() / 2; i++) {
    EXPECT_EQ(persons[i].id_, i);
    EXPECT_EQ(persons[i].id_, persons[i + persons.size() / 2].id_);
  }
  printf("persons capacity: %zu %zu\n", persons.capacity(), persons.size());
}