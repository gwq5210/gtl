#pragma once

namespace gtl {

template <typename T, typename... Args>
T* construct_at(T* p, Args&&... args) {
  new (p) T(std::forward<Args>(args)...);
  return p;
}

} // namespace gtl