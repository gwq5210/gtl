#include <cstdio>
#include <vector>

// #include "gtest/gtest.h"

#include "vector.h"

using gtl::vector;

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
void print(const vector<T> &vec) {
  printf("(size = %zu, capacity = %zu)", vec.size(), vec.capacity());
  print(vec.begin(), vec.end());
}

int main(int argc, char *argv[]) {
  int n = 10;
  std::vector<int> std_arr;
  vector<vector<int>> arr2;
  vector<int> arr = {1, 3, 4, 5};
  printf("size = %zu, capacity = %zu, sizeof(vector) = %zu/%zu/%zu\n", std_arr.size(), std_arr.capacity(), sizeof(std_arr), sizeof(arr), sizeof(arr2));
  arr = {2, 3, 4, 5};
  for (int i = 0; i < n; i++) {
    arr.push_back(i);
  }
  arr.push_back(1234);
  print(arr.rbegin(), arr.rend());
  print(arr);
  arr.erase(arr.begin());
  print(arr);
  arr.pop_back();
  print(arr);
  arr.insert(arr.begin(), 10);
  print(arr);
  arr.front() = 100;
  arr.back() = 101;
  print(arr);
  arr.resize(15, 10);
  print(arr);
  arr.resize(16, 10);
  print(arr);
  arr.push_back(123);
  print(arr);
}