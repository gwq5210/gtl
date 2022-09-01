#pragma once

#include "gtl/memory/simple_allocator.h"

namespace gtl {

inline MemoryAllocator& DefaultAllocator() { return SimpleMemoryAllocator::GetInstance(); }

void LeakStatsAtExit() __attribute__((destructor));

}  // namespace gtl