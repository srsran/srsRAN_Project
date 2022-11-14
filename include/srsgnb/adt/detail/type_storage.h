
#pragma once

#include <cstddef>
#include <cstdint>
#include <type_traits>
#include <utility>

namespace srsgnb {

namespace detail {

/// Helper class to store, construct, destruct and access an object stored in a memory buffer.
/// This class shouldn't be used outside of library code, as it is inherently unsafe.
/// \tparam T type of object stored
/// \tparam MinSize lower bound for the embedded buffer size
/// \tparam AlignSize lower bound for the alignment of the embedded buffer
template <typename T, size_t MinSize = 0, size_t AlignSize = 0>
struct type_storage {
  using value_type = T;

  /// Emplace object in embedded buffer via ctor call T(Args&&...). UB if object was already created
  template <typename... Args>
  void emplace(Args&&... args) noexcept(std::is_nothrow_constructible<T, Args&&...>::value)
  {
    new (&buffer) T(std::forward<Args>(args)...);
  }
  /// Destroy object stored in embedded buffer. UB if object was not previously created
  void destroy() noexcept { get().~T(); }

  /// Get created object handle. UB if object is not created
  // Note: In C++17, std::launder is required
  T&       get() noexcept { return reinterpret_cast<T&>(buffer); }
  const T& get() const noexcept { return reinterpret_cast<const T&>(buffer); }

  void*       addr() noexcept { return static_cast<void*>(&buffer); }
  const void* addr() const noexcept { return static_cast<void*>(&buffer); }
  explicit    operator void*() noexcept { return addr(); }

  constexpr static size_t obj_size   = sizeof(T) > MinSize ? sizeof(T) : MinSize;
  constexpr static size_t align_size = alignof(T) > AlignSize ? alignof(T) : AlignSize;

  std::aligned_storage_t<obj_size, align_size> buffer; ///< Embedded memory buffer
};

/// Helper method to copy optional object from rhs to lhs
template <typename T, size_t MinSize, size_t AlignSize>
void copy_if_present_helper(
    type_storage<T, MinSize, AlignSize>&       lhs,
    const type_storage<T, MinSize, AlignSize>& rhs,
    bool                                       lhs_present,
    bool rhs_present) noexcept(std::is_nothrow_copy_constructible<T>::value&& std::is_nothrow_copy_assignable<T>::value)
{
  if (lhs_present and rhs_present) {
    lhs.get() = rhs.get();
  } else if (lhs_present) {
    lhs.destroy();
  } else if (rhs_present) {
    lhs.emplace(rhs.get());
  }
}

/// Helper method to move optional object from rhs to lhs
template <typename T, size_t MinSize, size_t AlignSize>
void move_if_present_helper(
    type_storage<T, MinSize, AlignSize>& lhs,
    type_storage<T, MinSize, AlignSize>& rhs,
    bool                                 lhs_present,
    bool rhs_present) noexcept(std::is_nothrow_move_constructible<T>::value&& std::is_nothrow_move_assignable<T>::value)
{
  if (lhs_present and rhs_present) {
    lhs.get() = std::move(rhs.get());
  } else if (lhs_present) {
    lhs.destroy();
  } else if (rhs_present) {
    lhs.emplace(std::move(rhs.get()));
  }
}

} // namespace detail

} // namespace srsgnb
