#include "gtl/memory/default_allocator.h"

namespace gtl {

void LeakStatsAtExit() {
  MemoryAllocator& allocator = DefaultAllocator();
  std::string info = allocator.LeakStats();
  if (info.empty()) {
    GTL_WARN("no memory leak found");
  } else if (info != "unsupported") {
    GTL_WARN("{}", info);
  }
}

}  // namespace gtl