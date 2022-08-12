#include "gtl/thread/thread.h"

#include <string>

int Print(const std::string& msg) {
  sleep(1);
  GTL_INFO("print {}", msg);
  return 0;
}

int main(int argc, char *argv[]) {
  gtl::Thread thread(Print, std::string("123"));
  thread.Join();
  gtl::Thread thread_cancel(Print, std::string("123cancel"));
  thread_cancel.Cancel();
  thread_cancel.Join();
  Print("1234");
  return 0;
}