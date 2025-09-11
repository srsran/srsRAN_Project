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

#include <array>
#include <atomic>

namespace srsran {

/// \brief Single producer/single consumer lock-free triple buffer implementation.
/// This class implements a lock-free and wait-free data transfer mechanism between threads using a triple buffer
/// strategy. It is designed for scenarios with a single producer and a single consumer.
/// \tparam T The type of data to be stored in the buffer.
template <typename T>
class lockfree_triple_buffer
{
public:
  lockfree_triple_buffer() = default;
  explicit lockfree_triple_buffer(const T& default_init) :
    buffer{{padded_element{default_init}, padded_element{default_init}, padded_element{default_init}}}
  {
  }

  /// \brief Read the latest committed data from the buffer.
  /// This function is intended to be called by the consumer thread.
  /// It checks if new data is available and updates the front buffer index if necessary.
  /// \return Const reference to the latest data.
  const T& read()
  {
    unsigned dirty_idx = dirty_middle_buffer_idx.load(std::memory_order_relaxed);
    if (dirty_idx & dirty_bit) {
      front_buffer_idx = dirty_middle_buffer_idx.exchange(front_buffer_idx << 1, std::memory_order_acq_rel);
      front_buffer_idx = (front_buffer_idx >> 1);
    }
    return buffer[front_buffer_idx].value;
  }

  /// \brief Get a reference to the write buffer for the producer to modify.
  /// This function is intended to be called by the producer thread.
  /// \return Reference to the write buffer.
  T& write() { return buffer[back_buffer_idx].value; }

  /// \brief Commit the written data, making it available for reading.
  /// This function should be called by the producer thread after writing data.
  /// It atomically updates the buffer states to make the new data available.
  void commit()
  {
    back_buffer_idx = dirty_middle_buffer_idx.exchange(back_buffer_idx << 1 | dirty_bit, std::memory_order_acq_rel);
    back_buffer_idx = (back_buffer_idx >> 1);
  }

  /// \brief Write data to the buffer and commit it in a single operation.
  /// This is a convenience function that combines write() and commit().
  /// It atomically updates the buffer states to make the new data available.
  /// \param val The value to write to the buffer.
  void write_and_commit(const T& val)
  {
    write() = val;
    commit();
  }

private:
  static constexpr std::size_t k_cache_line_size = 64;
  static constexpr unsigned    dirty_bit         = 1;

  struct alignas(k_cache_line_size) padded_element {
    T value;

    padded_element() = default;
    padded_element(const T& value_) : value(value_) {}
  };
  std::array<padded_element, 3> buffer;
  alignas(k_cache_line_size) std::atomic<unsigned> dirty_middle_buffer_idx = {1 << 1};
  alignas(k_cache_line_size) unsigned front_buffer_idx                     = 0;
  alignas(k_cache_line_size) unsigned back_buffer_idx                      = 2;
};

} // namespace srsran
