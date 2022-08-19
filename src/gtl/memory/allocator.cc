#include "gtl/memory/allocator.h"

namespace gtl {

void* GtlMemoryAllocator::Malloc(size_t size) {}
void GtlMemoryAllocator::Free(void* ptr) {}
void* GtlMemoryAllocator::Calloc(size_t nmemb, size_t size) {}
void* GtlMemoryAllocator::Realloc(void* ptr, size_t size) {}
void* GtlMemoryAllocator::NewMemory(size_t size) {}

}  // namespace gtl