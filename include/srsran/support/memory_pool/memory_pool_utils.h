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

#include <cstdint>

namespace srsran {

/// Checks if alignment is power of 2.
constexpr bool is_alignment_valid(std::size_t alignment)
{
  return alignment > 0 && (alignment & (alignment - 1U)) == 0U;
}

/// Checks whether the pointer is set with the correct alignment.
constexpr inline bool is_aligned(std::uintptr_t ptr, std::size_t alignment)
{
  return (ptr & (alignment - 1)) == 0;
}
inline bool is_aligned(void* ptr, std::size_t alignment)
{
  return is_aligned(reinterpret_cast<std::uintptr_t>(ptr), alignment);
}

/// \brief Moves the pointer by the given size in bytes.
constexpr inline void* advance_ptr(void* pos, std::size_t sz)
{
  return static_cast<char*>(pos) + sz;
}

/// Moves the pointer to the next aligned position.
constexpr inline std::uintptr_t align_next(std::uintptr_t pos, std::size_t alignment)
{
  return (pos + (alignment - 1)) & ~(alignment - 1);
}
inline void* align_next(void* pos, std::size_t alignment)
{
  return reinterpret_cast<void*>(align_next(reinterpret_cast<std::uintptr_t>(pos), alignment));
}

} // namespace srsran
