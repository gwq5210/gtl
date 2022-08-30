#include "gtl/memory/allocator.h"

#include "gtl/algorithm/memory_op.h"
#include "gtl/logging.h"

namespace gtl {

std::atomic_int GtlMemoryAllocator::block_id_(0);

void* GtlMemoryAllocator::Malloc(size_t size) {
  BlockHeader* block = FindFreeBlock(size);
  if (block != nullptr) {
    RemoveFromFreeList(block);
    SplitBlock(block, size);
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
  AddToFreeList(block);
  GTL_INFO("free block: {}, size: {}", fmt::ptr(block), block->size);
  MergeBlock(block);
}

void GtlMemoryAllocator::RemoveFromFreeList(BlockHeader* block) {
  GTL_CHECK(block != nullptr && !block->used);

  block->set_used(true);
  doubly_list::Remove(&block->free_list);
}

void GtlMemoryAllocator::AddToFreeList(BlockHeader* block) {
  GTL_CHECK(block != nullptr);

  block->set_used(false);
  doubly_list::ListNode* pos = free_head_.next;
  ListForEach(node, free_head_) {
    BlockHeader* node_block = ListEntry(node, BlockHeader, free_list);
    if (node_block->size >= block->size) {
      break;
    }
    pos = node;
  }
  doubly_list::InsertBefore(pos, &block->free_list);
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
  if (size < kBlockMinSize) {
    size = kBlockMinSize;
  }
  void* ptr = AllocMemory(size + kBlockHeaderSize);
  if (ptr == nullptr) {
    return nullptr;
  }
  BlockHeader* block = gtl::construct_at(static_cast<BlockHeader*>(ptr), size, GenerateBlockName());
  block->heap = (size <= kMallocMmapThreshold);
  block->region = block;
  doubly_list::AddToTail(&block_head_, &block->block_list);
  return block;
}

GtlMemoryAllocator::BlockHeader* GtlMemoryAllocator::SplitBlock(BlockHeader* block, size_t new_size) {
  GTL_CHECK(block != nullptr && new_size > 0 && block->size >= new_size);

  if (block->size - new_size < kBlockHeaderSize + kBlockMinSize) {
    return nullptr;
  }

  GTL_INFO("split block: {}", GetBlockInfo(block));

  BlockHeader* new_block = gtl::construct_at(reinterpret_cast<BlockHeader*>(block->data + new_size),
                                             block->size - new_size - kBlockHeaderSize, GenerateBlockName());
  new_block->heap = block->heap;
  new_block->region = block;
  doubly_list::InsertBefore(block->block_list.next, &new_block->block_list);
  AddToFreeList(new_block);

  block->size = new_size;

  GTL_INFO("old block: {}", GetBlockInfo(block));
  GTL_INFO("new block: {}", GetBlockInfo(new_block));

  return new_block;
}

GtlMemoryAllocator::BlockHeader* GtlMemoryAllocator::MergeBlock(BlockHeader* block) {
  GTL_CHECK(block != nullptr && !block->used);
  BlockHeader* begin_block = block;
  for (doubly_list::ListNode* node = block->block_list.prev; node != &free_head_; node = node->prev) {
    BlockHeader* lblock = ListEntry(node, BlockHeader, block_list);
    if (lblock->used || lblock->region != block->region) {
      break;
    }
    begin_block = lblock;
  }
  return MergeRightBlock(begin_block);
}

GtlMemoryAllocator::BlockHeader* GtlMemoryAllocator::MergeRightBlock(BlockHeader* block) {
  GTL_CHECK(block != nullptr && !block->used);
  doubly_list::ListNode* node = block->block_list.next;
  while (node != &block_head_) {
    doubly_list::ListNode* next = node->next;
    BlockHeader* rblock = ListEntry(node, BlockHeader, block_list);
    if (rblock->used || block->region != rblock->region) {
      break;
    }
    MergeTwoBlock(block, rblock);
    node = next;
  }
  return block;
}

GtlMemoryAllocator::BlockHeader* GtlMemoryAllocator::MergeTwoBlock(BlockHeader* lblock, BlockHeader* rblock) {
  GTL_CHECK(lblock != nullptr && rblock != nullptr && !lblock->used && !rblock->used);

  GTL_INFO("lblock: {}", GetBlockInfo(lblock));
  GTL_INFO("rblock: {}", GetBlockInfo(rblock));

  doubly_list::Remove(&rblock->block_list);
  doubly_list::Remove(&rblock->free_list);
  lblock->size = lblock->size + rblock->size + kBlockHeaderSize;

  GTL_INFO("nblock: {}", GetBlockInfo(lblock));
  return lblock;
}

void* GtlMemoryAllocator::DeleteBlock(BlockHeader* block) {
  GTL_CHECK(block != nullptr && !block->used);
  return nullptr;
}

GtlMemoryAllocator::BlockHeader* GtlMemoryAllocator::FindBestFit(size_t size) {
  GTL_CHECK(size > 0);
  BlockHeader* result = nullptr;
  ListForEach(node, free_head_) {
    BlockHeader* block = ListEntry(node, BlockHeader, free_list);
    if (block->size == size) {
      return block;
    }
    if (block->size > size && (result == nullptr || result->size > block->size)) {
      result = block;
    }
  }
  return result;
}

GtlMemoryAllocator::BlockHeader* GtlMemoryAllocator::FindFirstFit(size_t size) {
  GTL_CHECK(size > 0);
  ListForEach(node, free_head_) {
    BlockHeader* block = ListEntry(node, BlockHeader, free_list);
    if (block->size >= size) {
      return block;
    }
  }
  return nullptr;
}

GtlMemoryAllocator::BlockHeader* GtlMemoryAllocator::FindWorstFit(size_t size) {
  GTL_CHECK(size > 0);
  ListForEachPrev(node, free_head_) {
    BlockHeader* block = ListEntry(node, BlockHeader, free_list);
    if (block->size >= size) {
      return block;
    }
  }
  return nullptr;
}

std::string GtlMemoryAllocator::MemoryInfo() const {
  std::string info;
  info = "-- BlockList --\n";
  ListForEach(node, block_head_) {
    BlockHeader* block = ListEntry(node, BlockHeader, block_list);
    info += GetBlockInfo(block) + "\n";
  }

  info += "\n-- FreeBlockList --\n";
  ListForEach(node, free_head_) {
    BlockHeader* block = ListEntry(node, BlockHeader, free_list);
    info += fmt::format("[{}] -> ", fmt::ptr(block));
  }
  info += "[NULL]\n";
  return info;
}

}  // namespace gtl