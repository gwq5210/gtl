#include "gtl/memory/allocator.h"

#include "gtl/algorithm/memory_op.h"
#include "gtl/logging.h"

namespace gtl {

std::atomic_int SimpleMemoryAllocator::block_id_(0);

void* SimpleMemoryAllocator::Malloc(size_t size) {
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

void SimpleMemoryAllocator::Free(void* ptr) {
  if (ptr == nullptr) {
    return;
  }

  BlockHeader* block = BlockHeader::ToBlockHeader(ptr);
  AddToFreeList(block);
  GTL_INFO("free block: {}, size: {}", fmt::ptr(block), block->size);
  MergeBlock(block);
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

  return nullptr;
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
  if (size < kBlockMinSize) {
    size = kBlockMinSize;
  }
  void* ptr = AllocMemory(size + kBlockHeaderSize);
  if (ptr == nullptr) {
    return nullptr;
  }
  BlockHeader* block = gtl::construct_at(static_cast<BlockHeader*>(ptr), size, GenerateBlockName());
  block->heap = false;
  block->region = block;
  doubly_list::AddToTail(&block_head_, &block->block_list);
  return block;
}

SimpleMemoryAllocator::BlockHeader* SimpleMemoryAllocator::SplitBlock(BlockHeader* block, size_t new_size) {
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
  GTL_CHECK(lblock != nullptr && rblock != nullptr && !lblock->used && !rblock->used);

  GTL_INFO("merge two block begin");
  GTL_INFO("lblock: {}", GetBlockInfo(lblock));
  GTL_INFO("rblock: {}", GetBlockInfo(rblock));

  doubly_list::Remove(&rblock->block_list);
  RemoveFromFreeList(rblock);
  lblock->size = lblock->size + rblock->size + kBlockHeaderSize;

  GTL_INFO("new_block: {}", GetBlockInfo(lblock));
  GTL_INFO("merge two block end");
  return lblock;
}

void SimpleMemoryAllocator::DeleteBlock(BlockHeader* block) {
  FreeMemory(block, block->size + kBlockHeaderSize);
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
  if (block->region == block && is_end) {
    return true;
  }
  return false;
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
  info = "-- BlockList --\n";
  ListForEach(node, block_head_) {
    BlockHeader* block = ListEntry(node, BlockHeader, block_list);
    info += GetBlockInfo(block) + "\n";
  }

  info += fmt::format("\n-- FreeBlockList[{}] --\n", free_block_count_);
  ListForEach(node, free_head_) {
    BlockHeader* block = ListEntry(node, BlockHeader, free_list);
    info += fmt::format("[{}] -> ", fmt::ptr(block));
  }
  info += "[NULL]\n";
  return info;
}

}  // namespace gtl