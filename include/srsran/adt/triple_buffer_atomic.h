/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
class triple_buffer_atomic
{
public:
  triple_buffer_atomic() = default;

  /// \brief Read the latest committed data from the buffer.
  /// This function is intended to be called by the consumer thread.
  /// It checks if new data is available and updates the front buffer index if necessary.
  /// \return Reference to the latest data.
  T& read()
  {
    unsigned dirty_idx = dirty_middle_buffer_idx.load(std::memory_order_relaxed);
    if ((dirty_idx & dirty_bit) == 1) {
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
  };
  std::array<padded_element, 3> buffer;
  std::atomic<unsigned>         dirty_middle_buffer_idx = {1 << 1};
  unsigned                      front_buffer_idx        = 0;
  unsigned                      back_buffer_idx         = 2;
};

} // namespace srsran
