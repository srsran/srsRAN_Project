/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#pragma once

#include <atomic>

namespace srsran {

/// \brief Atomic reference counter that can be used as a member of objects T of a intrusive_ptr<T>.
class intrusive_ptr_atomic_ref_counter
{
public:
  void inc_ref() { ref_count.fetch_add(1, std::memory_order_relaxed); }

  bool unique() const { return ref_count.load(std::memory_order_relaxed) == 1; }

  bool dec_ref() { return ref_count.fetch_sub(1, std::memory_order_acq_rel) == 1; }

private:
  std::atomic<unsigned> ref_count{0};
};

/// \brief Smart pointer type where a reference counter for the managed object is stored in the object itself.
///
/// The advantage of this class over a shared_ptr<T> is its simplicity, avoiding two different memory regions for the
/// reference counter and the managed object.
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

  intrusive_ptr(const intrusive_ptr& other) noexcept : ptr(other.ptr)
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

  intrusive_ptr& operator=(intrusive_ptr& other) noexcept
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