#pragma once

#include <errno.h>
#include <string.h>
#include <sys/event.h>
#include <sys/time.h>
#include <sys/types.h>

#include "gtl/logging.h"
#include "gtl/net/poller.h"
#include "gtl/time/util.h"

namespace gtl {

class Kqueue {
 public:
  using Event = struct kevent;

  enum OpType {
    kAdd = EV_ADD,
    kDel = EV_DELETE,
  };

  enum EventType {
    kReadable = 0x1,
    kWritable = 0x2,
  };

  static int ToEvents(int kqueue_events) {
    int revents = 0;
    if (Kqueue::EventReadable(kqueue_events)) {
      revents |= kReadable;
    }
    if (Kqueue::EventWritable(kqueue_events)) {
      revents |= kWritable;
    }
    return revents;
  }

  static bool EventReadable(int events) { return events & kReadable; }
  static bool EventWritable(int events) { return events & kWritable; }
  static bool KqueueEventReadable(int events) { return events == EVFILT_READ; }
  static bool KqueueEventWritable(int events) { return events == EVFILT_WRITE; }

  Kqueue() = default;
  Kqueue(Kqueue&& other) {
    kfd_ = other.kfd_;
    max_events_ = other.max_events_;
    events_ = other.events_;

    other.Clear();
  }
  Kqueue& operator=(Kqueue&& other) {
    Destroy();

    kfd_ = other.kfd_;
    max_events_ = other.max_events_;
    events_ = other.events_;

    other.Clear();
    return *this;
  }
  ~Kqueue() { Destroy(); }

  bool Init(size_t max_events = 10240) {
    kfd_ = kqueue();
    max_events_ = max_events;
    events_ = new Event[max_events_];
    GTL_CHECK(kfd_ >= 0 && events_ != nullptr);
    return true;
  }
  int Wait(int timeout_ms = -1) {
    struct timespec ts;
    if (timeout_ms >= 0) {
      ts = MsToAbsTimeSpec(timeout_ms);
    }
    int ret = kevent(kfd_, nullptr, 0, events_, max_events_, timeout_ms >= 0 ? &ts : nullptr);
    if (ret == -1) {
      GTL_ERROR("kevent failed, errno:{}, errmsg:{}", errno, strerror(errno));
    }
    return ret;
  }
  const Event& GetEvent(int index) const {
    GTL_CHECK(events_ && index >= 0 && index < max_events_);
    return events_[index];
  }
  bool Add(int fd, int events, void* ptr = nullptr) { return Ctl(kAdd, fd, events, ptr); }
  bool Del(int fd, int events, void* ptr = nullptr) { return Ctl(kDel, fd, events, ptr); }
  bool Mod(int fd, int old_events, int events, void* ptr = nullptr) {
    if (EventReadable(old_events)) {
      Del(fd, kReadable, ptr);
    }
    if (EventWritable(old_events)) {
      Del(fd, kWritable, ptr);
    }
    return Add(fd, events, ptr);
  }
  bool Ctl(int op, int fd, int events, void* ptr = nullptr) {
    Event ev[2];
    int count = 0;
    if (EventReadable(events)) {
      EV_SET(&ev[count], fd, EVFILT_READ, op, 0, 0, ptr);
      ++count;
    }
    if (EventWritable(events)) {
      EV_SET(&ev[count], fd, EVFILT_WRITE, op, 0, 0, ptr);
      ++count;
    }
    return Ctl(ev, count);
  }

 private:
  Kqueue(const Kqueue& other) = delete;
  Kqueue& operator=(const Kqueue& other) = delete;

  bool Ctl(const Event* ev, int count) {
    int ret = kevent(kfd_, ev, count, nullptr, 0, nullptr);
    if (ret != 0) {
      GTL_ERROR("kevent failed, errno:{}, errmsg:{}", errno, strerror(errno));
      return false;
    }
    return true;
  }

  void Clear() {
    kfd_ = -1;
    max_events_ = 10240;
    events_ = nullptr;
  }

  void Destroy() {
    if (kfd_ >= 0) {
      close(kfd_);
    }
    delete[] events_;
  }

  int kfd_ = -1;
  size_t max_events_ = 10240;
  Event* events_ = nullptr;
};

class KqueuePoller : public Poller {
 public:
  static int GetEvents(int events) {
    int revents = 0;
    if (EventReadable(events)) {
      revents |= Kqueue::kReadable;
    }
    if (EventWritable(events)) {
      revents |= Kqueue::kWritable;
    }
    return revents;
  }
  static int ToPollerEvents(int events) {
    int revents = 0;
    if (Kqueue::KqueueEventReadable(events)) {
      revents |= kReadable;
    }
    if (Kqueue::KqueueEventWritable(events)) {
      revents |= kWritable;
    }
    return revents;
  }

  virtual bool Init(size_t max_events = 10240) override {
    Poller::Init(max_events);
    return kq_.Init(max_events);
  }
  virtual bool Add(int fd, int events, void* ptr = nullptr) override { return kq_.Add(fd, GetEvents(events), ptr); }
  virtual bool Del(int fd, int events, void* ptr = nullptr) override { return kq_.Del(fd, GetEvents(events), ptr); }
  virtual bool Mod(int fd, int old_events, int events, void* ptr = nullptr) override {
    return kq_.Mod(fd, GetEvents(old_events), GetEvents(events), ptr);
  }
  virtual int Wait(int timeout_ms = -1) override {
    int ret = kq_.Wait(timeout_ms);
    if (ret < 0) {
      return ret;
    }
    for (int i = 0; i < ret; ++i) {
      const Kqueue::Event& event = kq_.GetEvent(i);
      results_[i].events = ToPollerEvents(event.filter);
      results_[i].ptr = event.udata;
    }
    return ret;
  }

 private:
  Kqueue kq_;
};

}  // namespace gtl