#pragma once

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#include <string>

#include "gtl/container/list_node.h"
#include "gtl/logging.h"
#include "gtl/memory/allocator.h"
#include "gtl/port.h"
#include "gtl/thread/mutex.h"

#include "fmt/format.h"

namespace gtl {

class GtlMemoryAllocator : public MemoryAllocator {
 public:
  static GtlMemoryAllocator& GetInstance() {
    static GtlMemoryAllocator instance;
    return instance;
  }

  static int NextBlockId() { return block_id_++; }

  static std::string GenerateBlockName() {
    int block_id = NextBlockId();
    return fmt::format("block_{}", block_id);
  }

  static const int kMaxBlockCount = 1024;
  static const int kMaxBlockNameSize = 32;
  static const int kAllocateBlockMemorySize = 128 * 1024;
  static const int kMinBlockMemorySize = 16;
  static std::atomic_int block_id_;

  struct BlockHeader {
    static void* ToData(BlockHeader* block) { return block->data; }
    static BlockHeader* ToBlockHeader(void* ptr) {
      return reinterpret_cast<BlockHeader*>(static_cast<char*>(ptr) - kBlockHeaderSize);
    }
    BlockHeader(size_t block_size, const std::string& block_name = "") : size(block_size) { set_name(block_name); }
    void set_name(const std::string& block_name) {
      if (!block_name.empty()) {
        strncpy(name, block_name.c_str(), kMaxBlockNameSize);
      } else {
        name[0] = '\0';
      }
      name[kMaxBlockNameSize - 1] = '\0';
    }
    void set_used(bool block_used) { used = block_used; }
    char name[kMaxBlockNameSize] = "";
    bool used = false;
    bool heap = false;
    size_t size = 0;
    BlockHeader* region = nullptr;
    doubly_list::ListNode block_list;
    union {
      doubly_list::ListNode free_list;
      char data[1];
    };
  };

  static const int kBlockHeaderSize = GTL_OFFSETOF(BlockHeader, data);
  static std::string GetBlockInfo(BlockHeader* block) {
    std::string info = fmt::format("[region: {}, block: {}, data: {}-{}] {:>10} [{}] [{}]", fmt::ptr(block->region),
                                   fmt::ptr(block), fmt::ptr(BlockHeader::ToData(block)),
                                   fmt::ptr(static_cast<char*>(BlockHeader::ToData(block)) + block->size), block->size,
                                   block->used ? "USED" : "FREE", block->heap ? "HEAP" : "MMAP");
    if (block->name[0] != '\0') {
      info += fmt::format(" [{}]", block->name);
    }
    return info;
  }

  virtual void* Malloc(size_t size) override;
  virtual void Free(void* ptr) override;
  virtual void* Calloc(size_t nmemb, size_t size) override;
  virtual void* Realloc(void* ptr, size_t size) override;
  virtual std::string MemoryInfo() const override;
  virtual std::string LeakStats() const override;

 private:
  GtlMemoryAllocator() = default;
  ~GtlMemoryAllocator() = default;
  GtlMemoryAllocator(const GtlMemoryAllocator& other) = delete;
  GtlMemoryAllocator& operator=(const GtlMemoryAllocator& other) = delete;

  void* AllocMemory(size_t size);
  void FreeMemory(void* ptr, size_t size);
  BlockHeader* NewBlock(size_t size);
  void DeleteBlock(BlockHeader* block);
  BlockHeader* FindFreeBlock(size_t size) { return FindFirstFit(size); }
  BlockHeader* FindBestFit(size_t size);
  BlockHeader* FindFirstFit(size_t size);
  BlockHeader* FindWorstFit(size_t size);
  void AddToFreeList(BlockHeader* block);
  void RemoveFromFreeList(BlockHeader* block);
  void AddToBlockList(BlockHeader* prev_block, BlockHeader* block);
  void AddToBlockListTail(BlockHeader* block);
  void RemoveFromBlockList(BlockHeader* block);
  BlockHeader* SplitBlock(BlockHeader* block, size_t new_size);
  BlockHeader* MergeBlock(BlockHeader* block);
  BlockHeader* MergeRightBlock(BlockHeader* block);
  BlockHeader* MergeTwoBlock(BlockHeader* lblock, BlockHeader* rblock);
  bool IsBlockFree(BlockHeader* block);

  std::atomic_int region_count_{0};
  std::atomic_int block_count_{0};
  doubly_list::ListNode block_head_;
  std::atomic_int free_block_count_{0};
  doubly_list::ListNode free_head_;
  Mutex mutex_;
};

}  // namespace gtl