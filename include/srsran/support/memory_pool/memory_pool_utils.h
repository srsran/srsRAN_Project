/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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