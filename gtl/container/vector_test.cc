#include <cstdio>

#include "vector.h"

using gtl::vector;

template<class Iterator>
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

int main(int argc, char *argv[]) {
  int n = 10;
  vector<int> arr;
  for (int i = 0; i < n; i++) {
    arr.push_back(i);
  }
  print(arr.rbegin(), arr.rend());
  print(arr.begin(), arr.end());
  arr.erase(arr.begin());
  print(arr.begin(), arr.end());
  arr.pop_back();
  print(arr.begin(), arr.end());
  arr.insert(arr.begin(), 10);
  print(arr.begin(), arr.end());
  arr.front() = 100;
  arr.back() = 101;
  print(arr.begin(), arr.end());
  arr.resize(15, 10);
  print(arr.begin(), arr.end());
}