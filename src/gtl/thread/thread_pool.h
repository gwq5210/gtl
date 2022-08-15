#pragma once

#include "gtl/thread/mutex.h"
#include "gtl/thread/thread.h"

namespace gtl {

class ThreadPool {
 public:
  struct Options {

  };

  static void Run();

  ThreadPool();

  bool Start();
  bool AddTask();
  bool Stop();

 private:
  Vector<Thread> threads_;
};

}  // namespace gtl