#include "gtl/thread/thread_pool.h"

namespace gtl {

bool ThreadPool::Start(size_t thread_count, const std::string& name /* = "ThreadPool" */) {
  set_name(name);
  for (size_t i = 0; i < thread_count; ++i) {
    threads_.emplace_back([this]() { this->Run(); });
  }
  return true;
}

bool ThreadPool::AddTask(std::function<void(void)>&& task) {
  if (stop_) {
    return false;
  }
  mutex_.Lock();
  tasks_.emplace(std::move(task));
  mutex_.Unlock();
  cond_var_.Signal();
  return true;
}

void ThreadPool::Run() {
  while (true) {
    mutex_.Lock();
    GTL_DEBUG("thread begin");
    cond_var_.Wait(mutex_, [this]() {
      return !tasks_.empty() || stop_;
    });
    GTL_DEBUG("thread wait end");
    if (stop_ && tasks_.empty()) {
      mutex_.Unlock();
      break;
    }
    GTL_DEBUG("stop: {}, tasks size: {}", stop_, tasks_.size());
    std::function<void(void)> func = std::move(tasks_.front());
    GTL_DEBUG("stop: {}, tasks size: {}", stop_, tasks_.size());
    tasks_.pop();
    mutex_.Unlock();
    func();
  }
}

}  // namespace gtl