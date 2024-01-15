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

#include "srsran/adt/span.h"

namespace srsran {
namespace srsvec {

namespace detail {
void* mem_alloc(std::size_t size);
void  mem_free(void* ptr);
} // namespace detail

/// Type to store a dynamic amount of aligned contiguous elements
template <typename T>
class aligned_vec : public span<T>
{
  void dealloc() { detail::mem_free(this->data()); }

public:
  aligned_vec<T>& operator=(aligned_vec<T>&& other)      = delete;
  aligned_vec<T>& operator=(const aligned_vec<T>& other) = delete;
  aligned_vec(const aligned_vec<T>& other)               = delete;
  aligned_vec(aligned_vec<T>&& other) noexcept           = delete;

  aligned_vec() : span<T>(nullptr, 0UL){};
  explicit aligned_vec(std::size_t size) { resize(size); }

  void resize(unsigned new_size)
  {
    if (new_size == this->size()) {
      return;
    }
    dealloc();

    T*      ptr_    = (T*)detail::mem_alloc(sizeof(T) * new_size);
    span<T> o       = span<T>(ptr_, new_size);
    *(span<T>*)this = o;
  }

  ~aligned_vec() { dealloc(); }
};

} // namespace srsvec
} // namespace srsran
