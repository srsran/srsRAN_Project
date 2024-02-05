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

#include "rx_buffer_codeblock_pool.h"
#include "srsran/adt/bit_buffer.h"
#include "srsran/adt/optional.h"
#include "srsran/adt/span.h"
#include "srsran/adt/static_vector.h"
#include "srsran/phy/upper/log_likelihood_ratio.h"
#include "srsran/phy/upper/rx_buffer_pool.h"
#include "srsran/phy/upper/unique_rx_buffer.h"
#include "srsran/ran/sch/sch_constants.h"
#include "srsran/support/srsran_assert.h"
#include <atomic>
#include <cstdint>

namespace srsran {

enum class rx_buffer_status : uint8_t { successful = 0, already_in_use, insufficient_cb };

/// Implements a receiver buffer interface.
class rx_buffer_impl : public unique_rx_buffer::callback
{
private:
  /// Internal buffer states.
  enum class state : uint8_t {
    /// \brief The buffer has not been configured with any codeblock.
    ///
    /// It is available for being reserved.
    available,
    /// \brief The buffer is reserved with a number of codeblocks.
    ///
    /// It allows:
    /// -\c expire:  called when the buffer context has expired, it transitions to \c available.
    /// -\c reserve: makes a new reservation.
    reserved,
    /// \brief The buffer is reserved and locked in a context. It does not accept new reservations or retransmissions,
    /// and prevents the buffer from expiring.
    ///
    /// It allows:
    /// - \c unlock:  a transmission did not match the CRC, it transitions to \c reserved; and
    /// - \c release: a transmission matched the CRC and the buffer releases all its resources, it transitions to \c
    ///               available.
    locked
  };

  /// Current buffer state.
  std::atomic<state> current_state = {state::available};
  /// Reference to the codeblock pool.
  rx_buffer_codeblock_pool& codeblock_pool;
  /// Stores codeblocks CRCs.
  static_vector<bool, MAX_NOF_SEGMENTS> crc;
  /// Stores codeblock identifiers.
  static_vector<unsigned, MAX_NOF_SEGMENTS> codeblock_ids;

  /// Frees reserved codeblocks. The codeblocks are returned to the pool.
  void free()
  {
    // Free all codeblocks.
    for (unsigned cb_id : codeblock_ids) {
      codeblock_pool.free(cb_id);
    }

    // Indicate the buffer is available by clearing the codeblocks identifiers.
    codeblock_ids.clear();
  }

public:
  /// \brief Creates a receive buffer.
  /// \param pool Codeblock buffer pool.
  explicit rx_buffer_impl(rx_buffer_codeblock_pool& pool) : codeblock_pool(pool)
  {
    // Do nothing.
  }

  /// Copy constructor - creates another buffer with the same codeblock pool.
  rx_buffer_impl(const rx_buffer_impl& other) noexcept : codeblock_pool(other.codeblock_pool) {}

  /// Move constructor - creates another buffer with the same codeblock pool.
  rx_buffer_impl(rx_buffer_impl&& other) noexcept : codeblock_pool(other.codeblock_pool) {}

  /// \brief Reserves a number of codeblocks from the pool.
  ///
  /// It optionally resets the CRCs.
  ///
  /// \param nof_codeblocks Number of codeblocks to reserve.
  /// \param reset_crc      Set to true for reset the codeblock CRCs.
  /// \return The reservation status.
  rx_buffer_status reserve(unsigned nof_codeblocks, bool reset_crc)
  {
    // It cannot reserve resources if it is locked.
    if (current_state.load() == state::locked) {
      return rx_buffer_status::already_in_use;
    }

    // If the current number of codeblocks is larger than required, free the excess of codeblocks.
    while (codeblock_ids.size() > nof_codeblocks) {
      // Get the codeblock identifier at the back and remove from the list.
      unsigned cb_id = codeblock_ids.back();
      codeblock_ids.pop_back();

      // Free the codeblock.
      codeblock_pool.free(cb_id);
    }

    // If the current number of codeblocks is less than required, reserve the remaining codeblocks.
    while (codeblock_ids.size() < nof_codeblocks) {
      // Reserve codeblock.
      optional<unsigned> cb_id = codeblock_pool.reserve();

      // Free the entire buffer if one codeblock cannot be reserved.
      if (!cb_id.has_value()) {
        free();
        current_state = state::available;
        return rx_buffer_status::insufficient_cb;
      }

      // Append the codeblock identifier to the list.
      codeblock_ids.push_back(cb_id.value());
    }

    // Resize CRCs.
    crc.resize(nof_codeblocks);

    // Reset CRCs if necessary.
    if (reset_crc) {
      reset_codeblocks_crc();
    }
    current_state = state::reserved;

    return rx_buffer_status::successful;
  }

  // See interface for documentation.
  unsigned get_nof_codeblocks() const override { return crc.size(); }

  // See interface for documentation.
  void reset_codeblocks_crc() override
  {
    // Set all codeblock CRC to false.
    for (bool& cb_crc : crc) {
      cb_crc = false;
    }
  }

  // See interface for documentation.
  span<bool> get_codeblocks_crc() override { return crc; }

  // See interface for documentation.
  unsigned get_absolute_codeblock_id(unsigned codeblock_id) const override
  {
    srsran_assert(codeblock_id < codeblock_ids.size(),
                  "Codeblock index ({}) is out of range ({}).",
                  codeblock_id,
                  codeblock_ids.size());
    return codeblock_ids[codeblock_id];
  }

  // See interface for documentation.
  span<log_likelihood_ratio> get_codeblock_soft_bits(unsigned codeblock_id, unsigned codeblock_size) override
  {
    srsran_assert(codeblock_id < codeblock_ids.size(),
                  "Codeblock index ({}) is out of range ({}).",
                  codeblock_id,
                  codeblock_ids.size());
    unsigned cb_id       = codeblock_ids[codeblock_id];
    unsigned cb_max_size = codeblock_pool.get_soft_bits(cb_id).size();
    srsran_assert(
        codeblock_size <= cb_max_size, "Codeblock size {} exceeds maximum size {}.", codeblock_size, cb_max_size);
    return codeblock_pool.get_soft_bits(cb_id).first(codeblock_size);
  }

  // See interface for documentation.
  bit_buffer get_codeblock_data_bits(unsigned codeblock_id, unsigned data_size) override
  {
    srsran_assert(codeblock_id < codeblock_ids.size(),
                  "Codeblock index ({}) is out of range ({}).",
                  codeblock_id,
                  codeblock_ids.size());
    unsigned cb_id         = codeblock_ids[codeblock_id];
    unsigned data_max_size = codeblock_pool.get_data_bits(cb_id).size();
    srsran_assert(
        data_size <= data_max_size, "Codeblock data size {} exceeds maximum size {}.", data_size, data_max_size);
    return codeblock_pool.get_data_bits(cb_id).first(data_size);
  }

  // See interface for documentation.
  void lock() override
  {
    state previous_state = current_state.exchange(state::locked);
    srsran_assert(previous_state == state::reserved, "Failed to lock. Invalid state.");
  }

  // See interface for documentation.
  void unlock() override
  {
    state previous_state = current_state.exchange(state::reserved);
    srsran_assert(previous_state == state::locked, "Failed to unlock. Invalid state.");
  }

  // See interface for documentation.
  void release() override
  {
    // Release all reserved codeblocks.
    free();

    // The buffer can now be reused again.
    state previous_state = current_state.exchange(state::available);
    srsran_assert(previous_state == state::locked, "Failed to release. Invalid state.");
  }

  /// Returns true if the buffer is free.
  bool is_free() const { return current_state == state::available; }

  /// Returns true if the buffer is locked.
  bool is_locked() const { return current_state == state::locked; }

  /// \brief Expires the buffer.
  ///
  /// The buffer pool shall use this method when the buffer expires. The buffer frees the reserved codeblocks if it is
  /// not blocked.
  ///
  /// \return \c true if the buffer is not locked.
  bool expire()
  {
    state expected_state = state::reserved;
    bool  from_reserved  = current_state.compare_exchange_weak(expected_state, state::available);

    // The  buffer cannot be freed if it is locked.
    if (!from_reserved) {
      return false;
    }

    // Release resources.
    free();

    return true;
  }
};

} // namespace srsran
