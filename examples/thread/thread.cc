#include "gtl/thread/thread.h"
#include "gtl/container/vector.h"
#include "gtl/thread/call_once.h"

#include <string>
#include <vector>

int Print(const std::string& msg) {
  sleep(1);
  GTL_INFO("print {}", msg);
  return 0;
}

void PrintOnce() { GTL_INFO("print once"); }

gtl::OnceFlag once_flag;

int main(int argc, char* argv[]) {
  gtl::Thread thread(Print, std::string("123"));
  thread.Join();
  gtl::Thread thread_cancel(Print, std::string("123cancel"));
  thread_cancel.Cancel();
  thread_cancel.Join();

  int count = 5;
  gtl::vector<gtl::Thread> threads;
  for (int i = 0; i < count; ++i) {
    threads.emplace_back([i]() {
      gtl::CallOnce(once_flag, PrintOnce);
      GTL_INFO("thread {} done", i);
    });
  }
  for (int i = 0; i < threads.size(); ++i) {
    threads[i].Join();
  }
  return 0;
}