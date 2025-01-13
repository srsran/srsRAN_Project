/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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
#include <cstddef>

namespace srsran {

/// Polymorphic memory resource used for allocating byte buffer segments of a byte buffer.
class byte_buffer_memory_resource
{
public:
  byte_buffer_memory_resource()                                   = default;
  byte_buffer_memory_resource(const byte_buffer_memory_resource&) = default;
  virtual ~byte_buffer_memory_resource()                          = default;

  byte_buffer_memory_resource& operator=(const byte_buffer_memory_resource&) = default;

  /// Allocate a memory block with \c size_hint.
  /// \return Range of bytes allocated that can be of size equal or different from \c size_hint.
  span<uint8_t> allocate(size_t size_hint, size_t alignment = alignof(max_align_t))
  {
    return do_allocate(size_hint, alignment);
  }

  /// Deallocate a memory block allocated by this pool.
  void deallocate(void* p) { do_deallocate(p); }

  /// Verify that the memory resource is the same.
  bool is_equal(const byte_buffer_memory_resource& other) const noexcept { return do_is_equal(other); }

private:
  virtual span<uint8_t> do_allocate(size_t size_hint, size_t alignment) = 0;

  virtual void do_deallocate(void* p) = 0;

  virtual bool do_is_equal(const byte_buffer_memory_resource& other) const noexcept = 0;
};

inline bool operator==(const byte_buffer_memory_resource& lhs, const byte_buffer_memory_resource& rhs) noexcept
{
  return &lhs == &rhs || lhs.is_equal(rhs);
}

inline bool operator!=(const byte_buffer_memory_resource& lhs, const byte_buffer_memory_resource& rhs) noexcept
{
  return not(lhs == rhs);
}

} // namespace srsran