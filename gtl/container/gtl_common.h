#pragma once

#include <string>

namespace gtl {

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

template <typename SizeType>
SizeType next2power(SizeType new_size) {
  SizeType ret = 1;
  while (ret < new_size) {
    ret <<= 1;
  }
  return ret;
}

}  // namespace gtl