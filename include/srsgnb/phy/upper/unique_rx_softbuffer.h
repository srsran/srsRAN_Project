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

/// \brief Wraps a softbuffer ptr and locks it for a scope.
///
/// The softbuffer lock mechanism prevents the pool from reserving and freeing as long as it being used in any scope.
class unique_rx_softbuffer : public rx_softbuffer
{
public:
  /// Required interface for the internal softbuffer lock and unlock.
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

  /// Builds a unique buffer from a buffer pointer. It locks the internal buffer.
  explicit unique_rx_softbuffer(softbuffer& softbuffer_) : ptr(&softbuffer_)
  {
    if (ptr != nullptr) {
      ptr->lock();
    }
  }

  /// Destructor - it unlocks the softbuffer.
  ~unique_rx_softbuffer() override
  {
    if (ptr != nullptr) {
      ptr->unlock();
    }
  }

  /// Copy constructor is deleted to prevent the unique softbuffer in multiple scopes.
  unique_rx_softbuffer(unique_rx_softbuffer& /**/) = delete;

  /// Move constructor is the way to move the softbuffer across scopes.
  unique_rx_softbuffer(unique_rx_softbuffer&& other) noexcept
  {
    ptr       = other.ptr;
    other.ptr = nullptr;
  };

  // See interface for documentation.
  unsigned int get_nof_codeblocks() const override
  {
    srsgnb_assert(is_valid(), "Invalid softbuffer.");
    return ptr->get_nof_codeblocks();
  }

  // See interface for documentation.
  void reset_codeblocks_crc() override
  {
    srsgnb_assert(is_valid(), "Invalid softbuffer.");
    ptr->reset_codeblocks_crc();
  }

  // See interface for documentation.
  span<bool> get_codeblocks_crc() override
  {
    srsgnb_assert(is_valid(), "Invalid softbuffer.");
    return ptr->get_codeblocks_crc();
  }

  // See interface for documentation.
  span<log_likelihood_ratio> get_codeblock_soft_bits(unsigned codeblock_id, unsigned codeblock_size) override
  {
    srsgnb_assert(is_valid(), "Invalid softbuffer.");
    return ptr->get_codeblock_soft_bits(codeblock_id, codeblock_size);
  }

  // See interface for documentation.
  bit_buffer get_codeblock_data_bits(unsigned codeblock_id, unsigned data_size) override
  {
    srsgnb_assert(is_valid(), "Invalid softbuffer.");
    return ptr->get_codeblock_data_bits(codeblock_id, data_size);
  }

  /// Returns true if the unique softbuffer contains a valid softbuffer.
  bool is_valid() const { return ptr != nullptr; }

private:
  /// Underlying pointer to the softbuffer. Set to nullptr for an invalid softbuffer.
  softbuffer* ptr = nullptr;
};

} // namespace srsgnb
