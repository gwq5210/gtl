#pragma once

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#include <string>

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
  virtual std::string MemoryInfo() const { return "unsupported"; }
  virtual std::string LeakStats() const { return "unsupported"; }
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

}  // namespace gtl