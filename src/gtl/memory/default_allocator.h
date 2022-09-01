#pragma once

#include "gtl/memory/simple_allocator.h"

namespace gtl {

inline MemoryAllocator& DefaultAllocator() { return SimpleMemoryAllocator::GetInstance(); }

void __attribute__((destructor)) LeakStatsAtExit();

}  // namespace gtl