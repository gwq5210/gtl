#pragma once

#include <string>

template <class Iterator>
void print_range(const std::string& name, Iterator first, Iterator end) {
  auto it = first;
  printf("%s [", name.c_str());
  while (it != end) {
    if (it != first) {
      printf(",");
    }
    printf("%d", *it);
    ++it;
  }
  printf("]\n");
}

template <class Container>
void print(const Container& c) {
  printf("(size = %zu, capacity = %zu)", c.size(), c.capacity());
  print(c.begin(), c.end());
}