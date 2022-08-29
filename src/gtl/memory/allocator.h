#pragma once

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#include <string>

#include "gtl/container/list_base.h"
#include "gtl/logging.h"

#include "fmt/format.h"

namespace gtl {

inline void* CurrentProgramBreak() { return sbrk(0); }

inline void* NewHeapMemory(size_t size) {
  void* ret = sbrk(size);
  if (ret == (void*)-1) {
    return nullptr;
  }
  return ret;
}

inline void* DeleteHeapMemory(size_t size) {
  void* ret = sbrk(-size);
  if (ret == (void*)-1) {
    return nullptr;
  }
  return ret;
}

inline void* NewAnonMemory(size_t size) {
  void* ret = mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
  if (ret == MAP_FAILED) {
    GTL_ERROR("mmap failed, errno:{}, errmsg:{}", errno, strerror(errno));
    return nullptr;
  }
  return ret;
}

inline bool DeleteAnonMemory(void* ptr, size_t size) {
  int ret = munmap(ptr, size);
  if (ret != 0) {
    GTL_ERROR("munmap failed, errno:{}, errmsg:{}", errno, strerror(errno));
    return false;
  }
  return true;
}

class MemoryAllocator {
 public:
  MemoryAllocator() = default;
  ~MemoryAllocator() = default;
  MemoryAllocator(const MemoryAllocator& other) = delete;
  MemoryAllocator& operator=(const MemoryAllocator& other) = delete;

  virtual void* Malloc(size_t size) = 0;
  virtual void Free(void* ptr) = 0;
  virtual void* Calloc(size_t nmemb, size_t size) = 0;
  virtual void* Realloc(void* ptr, size_t size) = 0;
};

class StdMemoryAllocator : public MemoryAllocator {
 public:
  static StdMemoryAllocator& GetInstance() {
    static StdMemoryAllocator instance;
    return instance;
  }

  StdMemoryAllocator() = default;
  ~StdMemoryAllocator() = default;
  StdMemoryAllocator(const StdMemoryAllocator& other) = delete;
  StdMemoryAllocator& operator=(const StdMemoryAllocator& other) = delete;

  virtual void* Malloc(size_t size) { return ::malloc(size); }
  virtual void Free(void* ptr) { ::free(ptr); }
  virtual void* Calloc(size_t nmemb, size_t size) { return ::calloc(nmemb, size); }
  virtual void* Realloc(void* ptr, size_t size) { return ::realloc(ptr, size); }
};

/*
首次适应算法（First Fit）：该算法从空闲分区链首开始查找，直至找到一个能满足其大小要求的空闲分区为止。
然后再按照作业的大小，从该分区中划出一块内存分配给请求者，余下的空闲分区仍留在空闲分区链中。
特点： 该算法倾向于使用内存中低地址部分的空闲区，在高地址部分的空闲区很少被利用，从而保留了高地址部分的大空闲区。
      显然为以后到达的大作业分配大的内存空间创造了条件。
缺点：低地址部分不断被划分，留下许多难以利用、很小的空闲区，而每次查找又都从低地址部分开始，会增加查找的开销。

循环首次适应算法（Next Fit）：该算法是由首次适应算法演变而成的。在为进程分配内存空间时，不再每次从链首开始查找，
直至找到一个能满足要求的空闲分区，并从中划出一块来分给作业。
特点：使内存中的空闲分区分布的更为均匀，减少了查找时的系统开销。
缺点：缺乏大的空闲分区，从而导致不能装入大型作业。

最佳适应算法（Best Fit）：该算法总是把既能满足要求，又是最小的空闲分区分配给作业。
为了加速查找，该算法要求将所有的空闲区按其大小排序后，以递增顺序形成一个空白链。这样每次找到的第一个满足要求的空闲区，必然是最优的。
孤立地看，该算法似乎是最优的，但事实上并不一定。因为每次分配后剩余的空间一定是最小的，在存储器中将留下许多难以利用的小空闲区。
同时每次分配后必须重新排序，这也带来了一定的开销。
特点：每次分配给文件的都是最合适该文件大小的分区。
缺点：内存中留下许多难以利用的小的空闲区。

四、最坏适应算法(Worst
Fit)：该算法按大小递减的顺序形成空闲区链，分配时直接从空闲区链的第一个空闲区中分配（不能满足需要则不分配）。
很显然，如果第一个空闲分区不能满足，那么再没有空闲分区能满足需要。
这种分配方法初看起来不太合理，但它也有很强的直观吸引力：在大空闲区中放入程序后，剩下的空闲区常常也很大，于是还能装下一个较大的新程序。
最坏适应算法与最佳适应算法的排序正好相反，它的队列指针总是指向最大的空闲区，在进行分配时，总是从最大的空闲区开始查寻。
该算法克服了最佳适应算法留下的许多小的碎片的不足，但保留大的空闲区的可能性减小了，而且空闲区回收也和最佳适应算法一样复杂。
特点：给文件分配分区后剩下的空闲区不至于太小，产生碎片的几率最小，对中小型文件分配分区操作有利。
缺点：使存储器中缺乏大的空闲区，对大型文件的分区分配不利。
*/
class GtlMemoryAllocator : public MemoryAllocator {
 public:
  static GtlMemoryAllocator& GetInstance() {
    static GtlMemoryAllocator instance;
    return instance;
  }

  static const int kMaxBlockNameSize = 32;
  static const int kBlockMinSize = 32;
  static const int kMallocMmapThreshold = 0;

  struct BlockHeader {
    BlockHeader(size_t block_size, const std::string& block_name = "") : size(block_size) { set_name(block_name); }
    void set_name(const std::string& block_name) {
      if (!block_name.empty()) {
        strncpy(name, block_name.c_str(), kMaxBlockNameSize);
      } else {
        strncpy(name, "unnamed", 7);
      }
      name[kMaxBlockNameSize - 1] = '\0';
    }
    void set_used(bool block_used) { used = block_used; }
    char name[kMaxBlockNameSize] = "unnamed";
    bool used = false;
    size_t size = 0;
    doubly_list::ListNode block_list;
    union {
      doubly_list::ListNode free_list;
      char data[1];
    };
  };

  static const int kBlockHeaderSize = GTL_OFFSETOF(BlockHeader, data);

  GtlMemoryAllocator() = default;
  ~GtlMemoryAllocator() = default;
  GtlMemoryAllocator(const GtlMemoryAllocator& other) = delete;
  GtlMemoryAllocator& operator=(const GtlMemoryAllocator& other) = delete;

  virtual void* Malloc(size_t size) override;
  virtual void Free(void* ptr) override;
  virtual void* Calloc(size_t nmemb, size_t size) override;
  virtual void* Realloc(void* ptr, size_t size) override;
  std::string MemoryInfo() const { return ""; }

 private:
  void* AllocMemory(size_t size);
  void FreeMemory(void* ptr, size_t size);
  BlockHeader* NewBlock(size_t size);
  void* DeleteBlock(BlockHeader* block);
  BlockHeader* FindFreeBlock(size_t size) { return FindFirstFit(size); }
  BlockHeader* FindBestFit(size_t size);
  BlockHeader* FindFirstFit(size_t size);
  BlockHeader* FindNextFit(size_t size);
  BlockHeader* FindWorstFit(size_t size);

  doubly_list::ListNode block_head_;
  doubly_list::ListNode free_head_;
};

}  // namespace gtl