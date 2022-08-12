#pragma once

#include <functional>

namespace gtl {

class Defer final {
 public:
  Defer(std::function<void(void)>&& func) : func_(std::move(func)) {}
  Defer(Defer&& other) = default;
  ~Defer() {
    if (func_) {
      func_();
    }
  }

 private:
  Defer(const Defer& other) = delete;
  Defer& operator=(const Defer& other) = delete;

  std::function<void(void)> func_;
};

}  // namespace gtl