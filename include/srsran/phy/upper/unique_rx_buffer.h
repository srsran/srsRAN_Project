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
#include "srsran/phy/upper/rx_buffer.h"
#include "srsran/support/srsran_assert.h"

namespace srsran {

/// \brief Wraps a receive buffer instance and locks it inside a scope.
///
/// The locking mechanism prevents the pool from reserving and freeing a buffer as long as it is being used inside a
/// block scope.
class unique_rx_buffer
{
public:
  /// \brief Public interface required for locking and unlocking the internal buffer.
  ///
  /// The implementation must be thread safe. In other words, lock(), unlock() and release() might be called from
  /// different threads.
  class callback : public rx_buffer
  {
  public:
    /// Locks the buffer.
    virtual void lock() = 0;

    /// Unlocks the buffer.
    virtual void unlock() = 0;

    /// Releases (after unlocking) the buffer resources.
    virtual void release() = 0;
  };

  /// Builds an invalid buffer.
  explicit unique_rx_buffer() = default;

  /// Builds a unique buffer from a buffer reference. It locks the internal buffer.
  explicit unique_rx_buffer(callback& instance_) : ptr(&instance_)
  {
    if (ptr != nullptr) {
      ptr->lock();
    }
  }

  /// Destructor - it unlocks the buffer.
  ~unique_rx_buffer()
  {
    if (ptr != nullptr) {
      ptr->unlock();
      ptr = nullptr;
    }
  }

  /// Copy constructor is deleted to prevent the unique buffer from being shared across multiple scopes.
  unique_rx_buffer(unique_rx_buffer& /**/) = delete;

  /// Move constructor is the only way to move the buffer to a different scope.
  unique_rx_buffer(unique_rx_buffer&& other) noexcept
  {
    ptr       = other.ptr;
    other.ptr = nullptr;
  };

  /// Move assigment operator.
  unique_rx_buffer& operator=(unique_rx_buffer&& other) noexcept
  {
    // Unlock current soft buffer if it is actually not unlocked.
    if (ptr != nullptr) {
      ptr->unlock();
    }

    // Move the other soft buffer ownership to the current soft buffer.
    ptr       = other.ptr;
    other.ptr = nullptr;

    return *this;
  }

  /// Gets the buffer.
  rx_buffer& get()
  {
    srsran_assert(is_valid(), "Invalid buffer.");
    return *ptr;
  }

  /// Gets a read-only buffer.
  const rx_buffer& get() const
  {
    srsran_assert(is_valid(), "Invalid buffer.");
    return *ptr;
  }

  rx_buffer&       operator*() { return get(); }
  rx_buffer*       operator->() { return &get(); }
  const rx_buffer& operator*() const { return get(); }
  const rx_buffer* operator->() const { return &get(); }

  /// Returns true if the unique buffer contains a valid buffer.
  bool is_valid() const { return ptr != nullptr; }

  /// Unlock and releases the buffer resources.
  void release()
  {
    srsran_assert(ptr != nullptr, "Invalid buffer for releasing.");
    ptr->release();
    ptr = nullptr;
  }

  /// Unlocks the buffer resources.
  void unlock()
  {
    srsran_assert(ptr != nullptr, "Invalid buffer for unlocking.");
    ptr->unlock();
    ptr = nullptr;
  }

private:
  /// Underlying pointer to the buffer. Set to nullptr for an invalid buffer.
  callback* ptr = nullptr;
};

} // namespace srsran
