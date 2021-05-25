#pragma once

#define PRINT_TYPE_SIZE(Type) printf(#Type " size: %zu\n", sizeof Type);

struct Person {
  Person(const Person& other) : id(other.id) {
    // printf("%s %d\n", __FUNCTION__, id);
  }
  Person(int i) : id(i) {
    // printf("%s %d\n", __FUNCTION__, id);
  }
  ~Person() {
    // printf("%d\n", id);
    id = 0;
  }
  int id;
};