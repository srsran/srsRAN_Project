/*
 *
 * Copyright 2013-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once
#include "srsgnb/phy/upper/rx_softbuffer.h"

namespace srsgnb {

/// \brief Wraps a softbuffer instance and locks it inside a block scope.
///
/// The locking mechanism prevents the pool from reserving and freeing a softbuffer as long as it is being used inside a
/// block scope.
class unique_rx_softbuffer
{
public:
  /// Public interface required for locking and unlocking the internal softbuffer.
  class softbuffer : public rx_softbuffer
  {
  public:
    /// \brief Locks the softbuffer.
    virtual void lock() = 0;

    /// \brief Unlocks the softbuffer.
    virtual void unlock() = 0;
  };

  /// Builds an invalid softbuffer.
  unique_rx_softbuffer() = default;

  /// Builds a unique softbuffer from a softbuffer reference. It locks the internal buffer.
  explicit unique_rx_softbuffer(softbuffer& softbuffer_) : ptr(&softbuffer_)
  {
    if (ptr != nullptr) {
      ptr->lock();
    }
  }

  /// Destructor - it releases the softbuffer.
  ~unique_rx_softbuffer() { release(); }

  /// Copy constructor is deleted to prevent the unique softbuffer from being shared across multiple scopes.
  unique_rx_softbuffer(unique_rx_softbuffer& /**/) = delete;

  /// Move constructor is the only way to move the softbuffer to a different scope.
  unique_rx_softbuffer(unique_rx_softbuffer&& other) noexcept
  {
    ptr       = other.ptr;
    other.ptr = nullptr;
  };

  /// Gets the softbuffer.
  rx_softbuffer& get()
  {
    srsgnb_assert(is_valid(), "Invalid softbuffer.");
    return *ptr;
  }
  rx_softbuffer& operator*() { return get(); }
  rx_softbuffer& operator->() { return get(); }

  /// Returns true if the unique softbuffer contains a valid softbuffer.
  bool is_valid() const { return ptr != nullptr; }

  /// Releases the softbuffer.
  void release()
  {
    if (ptr != nullptr) {
      ptr->unlock();
      ptr = nullptr;
    }
  }

private:
  /// Underlying pointer to the softbuffer. Set to nullptr for an invalid softbuffer.
  softbuffer* ptr = nullptr;
};

} // namespace srsgnb
