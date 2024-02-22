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

#include "srsran/adt/span.h"

namespace srsran {
namespace srsvec {

namespace detail {
void* mem_alloc(std::size_t size);
void  mem_free(void* ptr);
} // namespace detail

/// Type to store a dynamic amount of aligned contiguous elements.
template <typename T>
class aligned_vec : public span<T>
{
  void dealloc() { detail::mem_free(this->data()); }

public:
  aligned_vec<T>& operator=(aligned_vec<T>&& other)      = delete;
  aligned_vec<T>& operator=(const aligned_vec<T>& other) = delete;
  aligned_vec(const aligned_vec<T>& other)               = delete;
  aligned_vec(aligned_vec<T>&& other) noexcept           = delete;

  aligned_vec() = default;
  explicit aligned_vec(std::size_t size) { resize(size); }
  ~aligned_vec() { dealloc(); }

  void resize(std::size_t new_size)
  {
    if (new_size == this->size()) {
      return;
    }

    dealloc();

    T*       p = reinterpret_cast<T*>(detail::mem_alloc(sizeof(T) * new_size));
    span<T>::operator=(span<T>(p, new_size));
  }
};

} // namespace srsvec
} // namespace srsran
