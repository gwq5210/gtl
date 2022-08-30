#include "gtl/memory/allocator.h"

#include "gtl/algorithm/memory_op.h"
#include "gtl/logging.h"

namespace gtl {

void* GtlMemoryAllocator::Malloc(size_t size) {
  BlockHeader* block = FindFreeBlock(size);
  if (block != nullptr) {
    doubly_list::Remove(&block->free_list);
    block->set_used(true);
    GTL_INFO("find free block: {}, size: {}", fmt::ptr(block), block->size);
    return BlockHeader::ToData(block);
  }

  block = static_cast<BlockHeader*>(NewBlock(size));
  if (block == nullptr) {
    return nullptr;
  }

  GTL_INFO("new block: {}, size: {}", fmt::ptr(block), block->size);
  block->set_used(true);
  return BlockHeader::ToData(block);
}

void GtlMemoryAllocator::Free(void* ptr) {
  if (ptr == nullptr) {
    return;
  }

  BlockHeader* block = BlockHeader::ToBlockHeader(ptr);
  block->set_used(false);
  doubly_list::AddToTail(&free_head_, &block->free_list);
  GTL_INFO("free block: {}, size: {}", fmt::ptr(block), block->size);
}

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
    BlockHeader* block = ListEntry(node, BlockHeader, free_list);
    if (block->size >= size) {
      return block;
    }
  }
  return nullptr;
}

GtlMemoryAllocator::BlockHeader* GtlMemoryAllocator::FindNextFit(size_t size) { return nullptr; }

GtlMemoryAllocator::BlockHeader* GtlMemoryAllocator::FindWorstFit(size_t size) { return nullptr; }

std::string GtlMemoryAllocator::MemoryInfo() const {
  std::string info;
  info = "FreeBlock:\n";
  for (doubly_list::ListNode* node = free_head_.next; node != &free_head_; node = node->next) {
    BlockHeader* block = ListEntry(node, BlockHeader, free_list);
    info += fmt::format("block: {}, size: {}\n", fmt::ptr(block), block->size);
  }
  return info;
}

}  // namespace gtl