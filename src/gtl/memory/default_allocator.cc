#include "gtl/memory/default_allocator.h"

namespace gtl {

void LeakStatsAtExit() {
  MemoryAllocator& allocator = DefaultAllocator();
  std::string info = allocator.LeakStats();
  if (info.empty()) {
    fprintf(stderr, "no memory leak found\n");
  } else if (info != "unsupported") {
    fprintf(stderr, "%s\n", info.c_str());
  }
}

}  // namespace gtl