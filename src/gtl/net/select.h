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
  template <typename InputIt>
  void Add(InputIt first, InputIt last) {
    Clear();
    Add(first, last);
  }

  void Clear() { FD_ZERO(&set_); }
  bool Has(int fd) const { return FD_ISSET(fd, &set_); }
  template <typename InputIt>
  void Add(InputIt first, InputIt last) {
    for (InputIt it = first; it != last; ++it) {
      Add(*it);
    }
  }
  void Add(int fd) { FD_SET(fd, &set_); }
  void Del(int fd) { FD_CLR(fd, &set_); }
  fd_set& data() { return set_; }
  const fd_set& data() const { return set_; }

 private:
  fd_set set_;
};

class Select {
 public:
  void AddReadFd(int fd) { readfds_.Add(fd); }
  bool HasReadFd(int fd) { return readfds_.Has(fd); }
  void DelReadFd(int fd) { return readfds_.Del(fd); }

  void AddWriteFd(int fd) { writefds_.Add(fd); }
  bool HasWriteFd(int fd) { return writefds_.Has(fd); }
  void DelWriteFd(int fd) { return writefds_.Del(fd); }

  void AddExceptFd(int fd) { exceptfds_.Add(fd); }
  bool HasExceptFd(int fd) { return exceptfds_.Has(fd); }
  void DelExceptFd(int fd) { return exceptfds_.Del(fd); }

  FdSet& readfds() { return readfds_; }
  const FdSet& readfds() const { return readfds_; }
  FdSet& writefds() { return writefds; }
  const FdSet& writefds() const { return writefds; }
  FdSet& exceptfds() { return exceptfds; }
  const FdSet& exceptfds() const { return exceptfds; }

  int Wait(int nfds, FdSet* readfds, FdSet* writefds, FdSet* exceptfds, int timeout_ms = -1) {
    struct timeval tv;
    if (timeout_ms >= 0) {
      tv = MsToTimeVal(timeout_ms);
    }
    if (readfds) {
      *readfds = *readfds_;
    }
    if (writefds) {
      *writefds = *writefds_;
    }
    if (exceptfds) {
      *exceptfds = *exceptfds_;
    }
    int ret = select(nfds, readfds ? &readfds->data() : nullptr, writefds ? &writefds->data() : nullptr,
                     exceptfds ? &exceptfds->data() : nullptr, timeout_ms >= 0 ? &tv : nullptr);
    if (ret == -1) {
      GTL_ERROR("select failed, errno:{}, errmsg:{}", errno, strerror(errno));
    }
    return ret;
  }

 private:
  FdSet readfds_;
  FdSet writefds_;
  FdSet exceptfds_;
};

}  // namespace gtl