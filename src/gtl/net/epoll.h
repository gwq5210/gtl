#pragma once

#include <errno.h>
#include <string.h>
#include <sys/epoll.h>

#include "gtl/logging.h"
#include "gtl/net/poller.h"

namespace gtl {

class Epoll {
 public:
  using Event = struct epoll_event;

  enum OpType {
    kAdd = EPOLL_CTL_ADD,
    kMod = EPOLL_CTL_MOD,
    kDel = EPOLL_CTL_DEL,
  };

  enum EventType {
    kReadable = EPOLLIN,
    kWritable = EPOLLOUT,
  };

  static bool EventReadable(uint32_t events) { return events & kReadable; }
  static bool EventWritable(uint32_t events) { return events & kWritable; }

  Epoll() = default;
  Epoll(Epoll&& other) {
    et_ = other.et_;
    efd_ = other.efd_;
    max_events_ = other.max_events_;
    events_ = other.events_;

    other.Clear();
  }
  Epoll& operator=(Epoll&& other) {
    if (this == &other) {
      return *this;
    }

    Destroy();

    et_ = other.et_;
    efd_ = other.efd_;
    max_events_ = other.max_events_;
    events_ = other.events_;

    other.Clear();
    return *this;
  }
  ~Epoll() { Destroy(); }

  bool Init(size_t max_events = 10240, bool et = true) {
    et_ = et;
    efd_ = epoll_create(1);
    max_events_ = max_events;
    events_ = new Event[max_events_];
    GTL_CHECK(efd_ >= 0 && events_ != nullptr);
    return true;
  }
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
  const Event& GetEvent(int index) const {
    GTL_CHECK(events_ && index >= 0 && index < max_events_);
    return events_[index];
  }
  bool Add(int fd, uint32_t events, void* ptr = nullptr) { return Ctl(kAdd, fd, events, ptr); }
  bool Mod(int fd, uint32_t old_events, uint32_t events, void* ptr = nullptr) { return Ctl(kMod, fd, events, ptr); }
  bool Del(int fd, uint32_t events, void* ptr = nullptr) { return Ctl(kDel, fd, events, ptr); }
  bool Ctl(int op, int fd, uint32_t events, void* ptr = nullptr) {
    Event event;
    event.events = et_ ? (events | EPOLLET) : events;
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

  void Clear() {
    et_ = true;
    efd_ = -1;
    max_events_ = 10240;
    events_ = nullptr;
  }

  void Destroy() {
    if (efd_ >= 0) {
      close(efd_);
    }
    delete[] events_;
  }

  bool et_ = true;
  int efd_ = -1;
  size_t max_events_ = 10240;
  Event* events_ = nullptr;
};

class EpollPoller : public Poller {
 public:
  static uint32_t GetEvents(int events) {
    uint32_t revents = 0;
    if (EventReadable(events)) {
      revents |= Epoll::kReadable;
    }
    if (EventWritable(events)) {
      revents |= Epoll::kWritable;
    }
    return revents;
  }
  static int ToPollerEvents(uint32_t events) {
    int revents = 0;
    if (Epoll::EventReadable(events)) {
      revents |= kReadable;
    }
    if (Epoll::EventWritable(events)) {
      revents |= kWritable;
    }
    return revents;
  }

  EpollPoller() = default;
  EpollPoller(EpollPoller&& other) {
    Poller::operator=(std::move(other));
    epoll_ = std::move(other.epoll_);
  }

  EpollPoller& operator=(EpollPoller&& other) {
    if (this == &other) {
      return *this;
    }

    Poller::operator=(std::move(other));
    epoll_ = std::move(other.epoll_);
    return *this;
  }

  virtual bool Init(size_t max_events = 10240) override {
    Poller::Init(max_events);
    return epoll_.Init(max_events);
  }
  virtual bool Add(int fd, int events, void* ptr = nullptr) override { return epoll_.Add(fd, GetEvents(events), ptr); }
  virtual bool Del(int fd, int events, void* ptr = nullptr) override { return epoll_.Del(fd, GetEvents(events), ptr); }
  virtual bool Mod(int fd, int old_events, int events, void* ptr = nullptr) override {
    return epoll_.Mod(fd, GetEvents(old_events), GetEvents(events), ptr);
  }
  virtual int Wait(int timeout_ms = -1) override {
    int ret = epoll_.Wait(timeout_ms);
    if (ret < 0) {
      return ret;
    }
    for (int i = 0; i < ret; ++i) {
      const Epoll::Event& event = epoll_.GetEvent(i);
      results_[i].events = ToPollerEvents(event.events);
      results_[i].ptr = event.data.ptr;
    }
    return ret;
  }

 private:
  EpollPoller(const EpollPoller& other) = delete;
  EpollPoller& operator=(const EpollPoller& other) = delete;

  Epoll epoll_;
};

}  // namespace gtl