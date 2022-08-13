#pragma once

#include <errno.h>
#include <string.h>
#include <sys/epoll.h>

namespace gtl {

class Epoll {
 public:
  explicit Epoll(int max_events = 10240, bool et = true) { Init(max_events, et); }
  Epoll(Epoll&& other) {
    et_ = other.et_;
    efd_ = other.efd_;
    max_events_ = other.max_events_;
    events_ = other.events_;

    other.Clear();
  }
  Epoll& operator=(Epoll&& other) {
    Destroy();

    et_ = other.et_;
    efd_ = other.efd_;
    max_events_ = other.max_events_;
    events_ = other.events_;

    other.Clear();
  }
  ~Epoll() { Destroy(); }

  int Wait(int timeout_ms = -1) {
    int ret = epoll_wait(efd_, events_, max_events_, timeout_ms);
    if (ret == -1) {
      GTL_ERROR("epoll_wait failed, errno:{}, errmsg:{}", errno, strerror(errno));
    }
    return ret;
  }
  int PWait(const sigset_t* sigmask, int timeout_ms = -1) {
    int ret = epoll_pwait(efd_, events_, max_events_, timeout_ms, sigmask);
    if (ret == -1) {
      GTL_ERROR("epoll_pwait failed, errno:{}, errmsg:{}", errno, strerror(errno));
    }
    return ret;
  }
  const struct epoll_event& GetEvent(int index) const {
    GTL_CHECK(events_ && index >= 0 && index < max_events_);
    return events_[index];
  }
  int Add(int fd, uint32_t events, void* ptr = nullptr) { return Ctl(EPOLL_CTL_ADD, fd, events, ptr); }
  bool Mod(int fd, uint32_t events, void* ptr = nullptr) { return Ctl(EPOLL_CTL_MOD, fd, events, ptr); }
  bool Del(int fd, uint32_t events, void* ptr = nullptr) { return Ctl(EPOLL_CTL_DEL, fd, events, ptr); }
  bool Ctl(int op, int fd, uint32_t events, void* ptr = nullptr) {
    struct epoll_event event;
    event.evnets = et ? events | EPOLLET | events;
    event.data.ptr = ptr;
    int ret = epoll_ctl(efd_, op, fd, &event);
    if (ret != 0) {
      GTL_ERROR("epoll_ctl failed, errno:{}, errmsg:{}", errno, strerror(errno));
      return false;
    }
    return true;
  }

 private:
  Epoll(const Epoll& other) = delete;
  Epoll& operator=(const Epoll& other) = delete;

  bool Init(int max_events = 10240, bool et = true) {
    et_ = et;
    efd_ = epoll_create(1);
    max_events_ = max_events;
    events_ = new struct epoll_event[max_events_];
    GTL_CHECK(efd_ >= 0 && events_ != nullptr && max_events > 0);
  }

  void Clear() {
    et_ = true;
    efd_ = -1;
    max_events_ = 10240;
    events = nullptr;
  }

  void Destroy() {
    if (efd_ >= 0) {
      close(efd_);
    }
    delete[] events_;
  }

  bool et_ = true;
  int efd_ = -1;
  int max_events_ = 10240;
  struct epoll_event* events_ = nullptr;
};

}  // namespace gtl