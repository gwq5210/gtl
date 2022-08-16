#include "gtl/thread/thread_pool.h"

#include <string>
#include <vector>

int Print(const std::string& msg) {
  sleep(1);
  GTL_INFO("print {}", msg);
  return 0;
}

int main(int argc, char* argv[]) {
  spdlog::set_level(spdlog::level::debug);
  int task_count = 100;
  int thread_count = 200;
  gtl::ThreadPool thread_pool(thread_count);
  GTL_DEBUG("add task begin");
  for (int i = 0; i < task_count * thread_count; ++i) {
    thread_pool.AddTask(std::bind(Print, std::to_string(i)));
  }
  sleep(5);
  GTL_DEBUG("add task done");
  thread_pool.Stop();
  GTL_DEBUG("stop done");
  return 0;
}