#pragma once

#include <sys/selelct.h>

namespace gtl {

class FdSet {
 public:
  static const int kMaxFdSize = FD_SETSIZE;

  FdSet() { Clear(); }
  FdSet(std::initializer_list<int> il) {
    Clear();
    Add(il.begin(), il.end());
  }

  void Clear() {
    FD_ZERO(&set_);
  }
  bool Has(int fd) const { return FD_ISSET(fd, &set_); }
  template <typename InputIt>
  void Add(InputIt first, InputIt last) {
    for (InputIt it = first; it != last; ++it) {
      Add(*it);
    }
  }
  void Add(int fd) { FD_SET(fd, &set_); }
  void Del(int fd) { FD_CLR(fd, &set_); }
  fd_set* data() { return &set_; }
  const fd_set* data() const { return &set_; }

 private:
  fd_set set_;
};

class Select {

};

}  // namespace gtl