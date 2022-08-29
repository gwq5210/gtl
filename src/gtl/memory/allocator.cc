#include "gtl/memory/allocator.h"

#include "gtl/algorithm/memory_op.h"
#include "gtl/logging.h"

namespace gtl {

void* GtlMemoryAllocator::Malloc(size_t size) {
  BlockHeader* block = FindFreeBlock(size);
  if (block != nullptr) {
    FreeBlockHeader* free_block_header = reinterpret_cast<FreeBlockHeader*>(block->data);
    doubly_list::Remove(&free_block_header->free_list);
    return block->data;
  }

  block = static_cast<BlockHeader*>(NewBlock(size));
  if (block == nullptr) {
    return nullptr;
  }

  block->set_used(true);
  return block;
}

void GtlMemoryAllocator::Free(void* ptr) {}

void* GtlMemoryAllocator::Calloc(size_t nmemb, size_t size) { return nullptr; }

void* GtlMemoryAllocator::Realloc(void* ptr, size_t size) { return nullptr; }

void* GtlMemoryAllocator::AllocMemory(size_t size) {
  GTL_CHECK(size > 0);
  if (size > kMallocMmapThreshold) {
    return NewAnonMemory(size);
  }
  return NewHeapMemory(size);
}

void GtlMemoryAllocator::FreeMemory(void* ptr, size_t size) {
  GTL_CHECK(ptr != nullptr && size > 0);
  if (size > kMallocMmapThreshold) {
    DeleteAnonMemory(ptr, size);
  }
}

GtlMemoryAllocator::BlockHeader* GtlMemoryAllocator::NewBlock(size_t size) {
  GTL_CHECK(size > 0);
  void* ptr = AllocMemory(size + kBlockHeaderSize);
  if (ptr == nullptr) {
    return nullptr;
  }
  BlockHeader* block = gtl::construct_at(static_cast<BlockHeader*>(ptr), size);
  doubly_list::AddToTail(&block_head_, &block->block_list);
  return block;
}

void* GtlMemoryAllocator::DeleteBlock(BlockHeader* block) {
  GTL_CHECK(block != nullptr);
  return nullptr;
}

GtlMemoryAllocator::BlockHeader* GtlMemoryAllocator::FindBestFit(size_t size) { return nullptr; }

GtlMemoryAllocator::BlockHeader* GtlMemoryAllocator::FindFirstFit(size_t size) {
  GTL_CHECK(size > 0);
  for (doubly_list::ListNode* node = free_head_.next; node != &free_head_; node = node->next) {
    BlockHeader* block = ListEntry(node, BlockHeader, data);
    if (block->size >= size) {
      return block;
    }
  }
  return nullptr;
}

GtlMemoryAllocator::BlockHeader* GtlMemoryAllocator::FindNextFit(size_t size) { return nullptr; }

GtlMemoryAllocator::BlockHeader* GtlMemoryAllocator::FindWorstFit(size_t size) { return nullptr; }

}  // namespace gtl