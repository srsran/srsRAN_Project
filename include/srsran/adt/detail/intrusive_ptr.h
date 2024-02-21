/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include <atomic>

namespace srsran {

class intrusive_ptr_atomic_ref_counter
{
public:
  void inc_ref() { ref_count.fetch_add(1, std::memory_order_relaxed); }

  bool unique() const { return ref_count.load(std::memory_order_relaxed) == 1; }

  bool dec_ref() { return ref_count.fetch_sub(1, std::memory_order_acq_rel) == 1; }

private:
  std::atomic<unsigned> ref_count{0};
};

template <typename T>
class intrusive_ptr
{
public:
  intrusive_ptr() = default;

  intrusive_ptr(T* ptr_, bool add_ref = true) : ptr(ptr_)
  {
    if (ptr != nullptr && add_ref) {
      intrusive_ptr_inc_ref(ptr);
    }
  }

  intrusive_ptr(const intrusive_ptr& other) : ptr(other.ptr)
  {
    if (ptr != nullptr) {
      intrusive_ptr_inc_ref(ptr);
    }
  }

  intrusive_ptr(intrusive_ptr&& other) noexcept : ptr(other.ptr) { other.ptr = nullptr; }

  ~intrusive_ptr()
  {
    if (ptr != nullptr) {
      intrusive_ptr_dec_ref(ptr);
    }
  }

  intrusive_ptr& operator=(intrusive_ptr& other)
  {
    if (ptr != other.ptr) {
      T* temp = ptr;
      if (other.ptr != nullptr) {
        intrusive_ptr_inc_ref(other.ptr);
      }
      ptr = other.ptr;
      if (temp != nullptr) {
        instrusive_ptr_dec_ref(temp);
      }
    }
  }

  intrusive_ptr& operator=(intrusive_ptr&& other) noexcept
  {
    intrusive_ptr<T>{std::move(other)}.swap(*this);
    return *this;
  }

  T& operator*() const { return *ptr; }

  T* operator->() const { return ptr; }

  T* get() const { return ptr; }

  void reset() { intrusive_ptr<T>{}.swap(*this); }

  void reset(T* other) { intrusive_ptr<T>{other}.swap(*this); }

  void swap(intrusive_ptr& other) noexcept
  {
    T* temp   = ptr;
    ptr       = other.ptr;
    other.ptr = temp;
  }

  bool unique() const { return intrusive_ptr_is_unique(ptr); }

private:
  T* ptr = nullptr;
};

template <typename T>
inline void swap(intrusive_ptr<T>& lhs, intrusive_ptr<T>& rhs)
{
  lhs.swap(rhs);
}

template <typename T, typename U>
inline bool operator==(const intrusive_ptr<T>& lhs, const intrusive_ptr<U>& rhs)
{
  return (lhs.get() == rhs.get());
}

template <typename T, typename U>
inline bool operator==(const intrusive_ptr<T>& lhs, U* rhs)
{
  return (lhs.get() == rhs);
}

template <typename T>
inline bool operator==(const intrusive_ptr<T>& lhs, std::nullptr_t rhs)
{
  return (lhs.get() == rhs);
}

template <typename T, typename U>
inline bool operator!=(const intrusive_ptr<T>& lhs, const intrusive_ptr<U>& rhs)
{
  return (lhs.get() != rhs.get());
}

template <typename T, typename U>
inline bool operator!=(const intrusive_ptr<T>& lhs, U* rhs)
{
  return (lhs.get() != rhs);
}

template <typename T>
inline bool operator!=(const intrusive_ptr<T>& lhs, std::nullptr_t rhs)
{
  return (lhs.get() != rhs);
}

} // namespace srsran