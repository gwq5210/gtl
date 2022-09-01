#include "gtl/memory/simple_allocator.h"

#include "gtl/algorithm/memory_op.h"
#include "gtl/container/list_base.h"
#include "gtl/logging.h"

namespace gtl {

std::atomic_int SimpleMemoryAllocator::block_id_(0);

void* SimpleMemoryAllocator::Malloc(size_t size) {
  BlockHeader* block = nullptr;
  {
    LockGuard lock_guard(mutex_);
    block = FindFreeBlock(size);
    if (block != nullptr) {
      RemoveFromFreeList(block);
      SplitBlock(block, size);
      GTL_DEBUG("find free block: {}, size: {}", fmt::ptr(block), block->size);
      return BlockHeader::ToData(block);
    }
  }

  block = static_cast<BlockHeader*>(NewBlock(size));
  if (block == nullptr) {
    return nullptr;
  }

  block->set_used(true);
  GTL_DEBUG("new block: {}", GetBlockInfo(block));
  return BlockHeader::ToData(block);
}

void SimpleMemoryAllocator::Free(void* ptr) {
  if (ptr == nullptr) {
    return;
  }

  LockGuard lock_guard(mutex_);
  BlockHeader* block = BlockHeader::ToBlockHeader(ptr);
  AddToFreeList(block);
  GTL_DEBUG("free block: {}", GetBlockInfo(block));
  BlockHeader* merged_block = MergeBlock(block);
  GTL_DEBUG("merged block: {}", GetBlockInfo(merged_block));
  if (region_count_ > kMaxBlockCount && IsBlockFree(merged_block)) {
    GTL_DEBUG("free merged block: {}", GetBlockInfo(merged_block));
    DeleteBlock(merged_block);
  }
}

void SimpleMemoryAllocator::RemoveFromFreeList(BlockHeader* block) {
  GTL_CHECK(block != nullptr && !block->used);

  block->set_used(true);
  doubly_list::Remove(&block->free_list);
  --free_block_count_;
}

void SimpleMemoryAllocator::AddToFreeList(BlockHeader* block) {
  GTL_CHECK(block != nullptr);

  block->set_used(false);
  doubly_list::ListNode* pos = &free_head_;
  ListForEach(node, free_head_) {
    BlockHeader* node_block = ListEntry(node, BlockHeader, free_list);
    if (node_block->size >= block->size) {
      pos = node;
      break;
    }
  }
  doubly_list::InsertBefore(pos, &block->free_list);
  ++free_block_count_;
}

void SimpleMemoryAllocator::RemoveFromBlockList(BlockHeader* block) {
  GTL_CHECK(block != nullptr);

  doubly_list::Remove(&block->block_list);
  --block_count_;
}

void SimpleMemoryAllocator::AddToBlockListTail(BlockHeader* block) {
  GTL_CHECK(block != nullptr);
  doubly_list::AddToTail(&block_head_, &block->block_list);
  ++block_count_;
}

void SimpleMemoryAllocator::AddToBlockList(BlockHeader* prev_block, BlockHeader* block) {
  GTL_CHECK(block != nullptr);
  doubly_list::InsertBefore(&prev_block->block_list, &block->block_list);
  ++block_count_;
}

void* SimpleMemoryAllocator::Calloc(size_t nmemb, size_t size) {
  size_t mem_size = nmemb * size;
  if (mem_size == 0) {
    return nullptr;
  }
  void* ptr = Malloc(mem_size);
  if (ptr == nullptr) {
    return nullptr;
  }

  memset(ptr, 0, mem_size);
  return ptr;
}

void* SimpleMemoryAllocator::Realloc(void* ptr, size_t size) {
  if (ptr == nullptr) {
    return Malloc(size);
  } else if (size == 0) {
    Free(ptr);
    return nullptr;
  }

  BlockHeader* block = BlockHeader::ToBlockHeader(ptr);
  if (block->size >= size) {
    return ptr;
  }

  {
    // 相同region的block是空闲的且合起来之后有足够的空间
    LockGuard lock_guard(mutex_);
    BlockHeader* next_block = nullptr;
    if (block->block_list.next != &block_head_) {
      next_block = ListEntry(block->block_list.next, BlockHeader, block_list);
      if (next_block->region != block->region || next_block->used ||
          block->size + next_block->size + kBlockHeaderSize < size) {
        next_block = nullptr;
      }
    }
    if (next_block) {
      MergeTwoBlock(block, next_block);
      SplitBlock(block, size);
      return ptr;
    }
  }

  void* new_ptr = Malloc(size);
  if (new_ptr == nullptr) {
    return nullptr;
  }
  memcpy(new_ptr, ptr, block->size);
  Free(ptr);
  return new_ptr;
}

void* SimpleMemoryAllocator::AllocMemory(size_t size) {
  GTL_CHECK(size > 0);
  return NewAnonMemory(size);
}

void SimpleMemoryAllocator::FreeMemory(void* ptr, size_t size) {
  GTL_CHECK(ptr != nullptr && size > 0);
  DeleteAnonMemory(ptr, size);
}

SimpleMemoryAllocator::BlockHeader* SimpleMemoryAllocator::NewBlock(size_t size) {
  if (size < kMinBlockMemorySize) {
    size = kMinBlockMemorySize;
  }
  void* ptr = AllocMemory(size + kBlockHeaderSize);
  if (ptr == nullptr) {
    return nullptr;
  }
  BlockHeader* block = gtl::construct_at(static_cast<BlockHeader*>(ptr), size, GenerateBlockName());
  block->heap = false;
  block->region = block;
  ++region_count_;
  AddToBlockListTail(block);
  return block;
}

SimpleMemoryAllocator::BlockHeader* SimpleMemoryAllocator::SplitBlock(BlockHeader* block, size_t new_size) {
  GTL_CHECK(block != nullptr && new_size > 0 && block->size >= new_size);

  if (block->size - new_size < kBlockHeaderSize + kMinBlockMemorySize) {
    return nullptr;
  }

  GTL_DEBUG("split block: {}", GetBlockInfo(block));

  BlockHeader* new_block = gtl::construct_at(reinterpret_cast<BlockHeader*>(block->data + new_size),
                                             block->size - new_size - kBlockHeaderSize, GenerateBlockName());
  new_block->heap = block->heap;
  new_block->region = block->region;
  AddToBlockList(ListEntry(block->block_list.next, BlockHeader, block_list), new_block);
  AddToFreeList(new_block);

  block->size = new_size;

  GTL_DEBUG("old block: {}", GetBlockInfo(block));
  GTL_DEBUG("new block: {}", GetBlockInfo(new_block));

  return new_block;
}

SimpleMemoryAllocator::BlockHeader* SimpleMemoryAllocator::MergeBlock(BlockHeader* block) {
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

SimpleMemoryAllocator::BlockHeader* SimpleMemoryAllocator::MergeRightBlock(BlockHeader* block) {
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
  // 合并相邻block之后重新对free_list排序
  RemoveFromFreeList(block);
  AddToFreeList(block);
  return block;
}

SimpleMemoryAllocator::BlockHeader* SimpleMemoryAllocator::MergeTwoBlock(BlockHeader* lblock, BlockHeader* rblock) {
  GTL_CHECK(lblock != nullptr && rblock != nullptr && !rblock->used);

  GTL_DEBUG("merge two block begin");
  GTL_DEBUG("lblock: {}", GetBlockInfo(lblock));
  GTL_DEBUG("rblock: {}", GetBlockInfo(rblock));

  RemoveFromBlockList(rblock);
  RemoveFromFreeList(rblock);
  lblock->size = lblock->size + rblock->size + kBlockHeaderSize;

  GTL_DEBUG("new_block: {}", GetBlockInfo(lblock));
  GTL_DEBUG("merge two block end");
  return lblock;
}

void SimpleMemoryAllocator::DeleteBlock(BlockHeader* block) {
  RemoveFromFreeList(block);
  RemoveFromBlockList(block);
  FreeMemory(block, block->size + kBlockHeaderSize);
  --region_count_;
}

bool SimpleMemoryAllocator::IsBlockFree(BlockHeader* block) {
  if (block->used) {
    return false;
  }
  bool is_end = false;
  if (block->block_list.next == &block_head_) {
    is_end = true;
  } else {
    BlockHeader* next_block = ListEntry(block->block_list.next, BlockHeader, block_list);
    if (next_block->region != block->region) {
      is_end = true;
    }
  }
  return block->region == block && is_end;
}

SimpleMemoryAllocator::BlockHeader* SimpleMemoryAllocator::FindBestFit(size_t size) {
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

SimpleMemoryAllocator::BlockHeader* SimpleMemoryAllocator::FindFirstFit(size_t size) {
  GTL_CHECK(size > 0);
  ListForEach(node, free_head_) {
    BlockHeader* block = ListEntry(node, BlockHeader, free_list);
    if (block->size >= size) {
      return block;
    }
  }
  return nullptr;
}

SimpleMemoryAllocator::BlockHeader* SimpleMemoryAllocator::FindWorstFit(size_t size) {
  GTL_CHECK(size > 0);
  ListForEachPrev(node, free_head_) {
    BlockHeader* block = ListEntry(node, BlockHeader, free_list);
    if (block->size >= size) {
      return block;
    }
  }
  return nullptr;
}

std::string SimpleMemoryAllocator::MemoryInfo() const {
  std::string info;
  size_t total_bytes = 0;
  size_t free_bytes = 0;
  info = fmt::format("-- BlockList[{}/{}] --\n", region_count_, block_count_);
  ListForEach(node, block_head_) {
    BlockHeader* block = ListEntry(node, BlockHeader, block_list);
    info += GetBlockInfo(block) + "\n";
    total_bytes += block->size;
  }

  info += fmt::format("\n-- FreeBlockList[{}] --\n", free_block_count_);
  ListForEach(node, free_head_) {
    BlockHeader* block = ListEntry(node, BlockHeader, free_list);
    info += fmt::format("[{}] -> ", fmt::ptr(block));
    free_bytes += block->size;
  }
  info += "[NULL]\n";

  info += "\n-- Memory Summary --\n";
  info += fmt::format("total {} bytes\n", total_bytes);
  info += fmt::format("free {} bytes\n", free_bytes);
  return info;
}

std::string SimpleMemoryAllocator::LeakStats() const {
  std::string info;
  size_t leak_bytes = 0;
  size_t leak_block_count = 0;
  info = fmt::format("-- LeakStats[{}/{}] --\n", region_count_, block_count_);
  ListForEach(node, block_head_) {
    BlockHeader* block = ListEntry(node, BlockHeader, block_list);
    if (block->used) {
      info += GetBlockInfo(block) + "\n";
      leak_bytes += block->size;
      ++leak_block_count;
    }
  }
  if (leak_block_count > 0) {
    info += "-- Leak Summary --\n";
    info += fmt::format("{} blocks lost ({} bytes)", leak_block_count, leak_bytes);
  } else {
    info = "";
  }
  return info;
}

}  // namespace gtl