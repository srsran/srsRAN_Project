/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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
#include "rx_softbuffer_codeblock_pool.h"
#include "srsran/adt/static_vector.h"
#include "srsran/phy/upper/codeblock_metadata.h"
#include "srsran/phy/upper/rx_softbuffer_pool.h"
#include "srsran/phy/upper/unique_rx_softbuffer.h"
#include "srsran/support/error_handling.h"
#include <mutex>

namespace srsran {

enum class rx_softbuffer_status { successful = 0, already_in_use, insuficient_cb };

constexpr const char* to_string(rx_softbuffer_status status)
{
  switch (status) {
    default:
    case rx_softbuffer_status::successful:
      return "successful";
    case rx_softbuffer_status::already_in_use:
      return "HARQ already in use";
    case rx_softbuffer_status::insuficient_cb:
      return "insufficient CBs";
  }
}

/// Implements a receiver softbuffer interface.
class rx_softbuffer_impl : public unique_rx_softbuffer::softbuffer
{
private:
  /// Protects the finite state machine from concurrent access.
  mutable std::mutex fsm_mutex;
  /// Finite state machine states.
  enum class state {
    /// \brief The softbuffer has not been configured with any codeblock.
    ///
    /// It is available for being reserved.
    available = 0,
    ///\brief The softbuffer is reserved with a number of codeblocks for an RNTI and HARQ process.
    ///
    /// It allows new reservations.
    reserved,
    /// \brief The softbuffer is reserved and locked in a context. It does not accept new reservations or
    /// retransmissions.
    ///
    /// It allows:
    /// - \c unlock: a transmission did not match the CRC, it transitions to \c reserved; and
    /// - \c release: a transmission matched the CRC and the softbuffer releases all its resources, it transitions to \c
    /// released.
    locked,
    /// \brief The softbuffer has been released and the resources have not been released.
    ///
    /// It allows:
    /// - \c run_slot: releases its resources and transitions to \c available; and
    /// - \c reserve: reserves or reuses its resources for a new transmission.
    released
  };
  /// Current softbuffer state.
  state current_state = state::available;
  /// Reservation identifier.
  rx_softbuffer_identifier reservation_id = {};
  /// Indicates the slot the softbuffer will expire at.
  slot_point reservation_expire_slot;
  /// Reference to the codeblock pool.
  rx_softbuffer_codeblock_pool& codeblock_pool;
  /// Stores codeblocks CRCs.
  static_vector<bool, MAX_NOF_SEGMENTS> crc;
  /// Stores codeblock identifiers.
  static_vector<unsigned, MAX_NOF_SEGMENTS> codeblock_ids;

  /// \brief Releases the reserved softbuffer to the pool.
  /// \remark An assertion is triggered if the softbuffer is locked.
  void free()
  {
    // Free codeblocks.
    for (unsigned cb_id : codeblock_ids) {
      codeblock_pool.free(cb_id);
    }

    // Indicate the buffer is available by clearing the codeblocks identifiers.
    codeblock_ids.clear();

    // Transition to available.
    current_state = state::available;
  }

public:
  /// \brief Creates a receive softbuffer.
  /// \param[in] pool Provides the codeblock softbuffer pool.
  rx_softbuffer_impl(rx_softbuffer_codeblock_pool& pool) : reservation_id(), codeblock_pool(pool)
  {
    // Do nothing.
  }

  /// \brief Reserves the softbuffer.
  ///
  /// The reservation is unsuccessful if:
  /// - the softbuffer is already in use in an another scope, or
  /// - the pool is out of resources.
  ///
  /// \param[in] id Reservation identifier.
  /// \param[in] expire_slot Slot at which the reservation expires.
  /// \param[in] nof_codeblocks Number of codeblocks to reserve.
  /// \return True if the reservation is successful, false otherwise.
  rx_softbuffer_status
  reserve(const rx_softbuffer_identifier& id, const slot_point& expire_slot, unsigned int nof_codeblocks)
  {
    std::unique_lock<std::mutex> lock(fsm_mutex);

    // It cannot be reserved if it is locked.
    if (current_state == state::locked) {
      return rx_softbuffer_status::already_in_use;
    }

    // Update reservation information.
    reservation_id          = id;
    reservation_expire_slot = expire_slot;

    // If the number of codeblocks match, skip the rest.
    if (nof_codeblocks == codeblock_ids.size()) {
      // Transitions to reserved if it is available or released.
      current_state = state::reserved;
      return rx_softbuffer_status::successful;
    }

    // Make sure there are no buffers before reserving.
    free();

    // Resize CRCs.
    crc.resize(nof_codeblocks);

    // Resize codeblocks with codeblock unreserved identifier.
    codeblock_ids.resize(nof_codeblocks, static_cast<unsigned>(rx_softbuffer_codeblock_pool::UNRESERVED_CB_ID));

    // Reserve codeblocks.
    for (unsigned& cb_id : codeblock_ids) {
      // Reserve codeblock.
      cb_id = codeblock_pool.reserve();

      // Make sure the CB identifier is valid.
      if (cb_id == rx_softbuffer_codeblock_pool::UNRESERVED_CB_ID) {
        // Free the rest of the softbuffer.
        free();
        return rx_softbuffer_status::insuficient_cb;
      }
    }

    // Transition to reserved.
    current_state = state::reserved;
    return rx_softbuffer_status::successful;
  }

  /// \brief Runs softbuffer housekeeping as per slot basis.
  ///
  /// Releases the resources if the softbuffer is \c released or the softbuffer reservation time expired while it
  /// is \c reserved.
  ///
  /// \param[in] slot Indicates the current slot.
  /// \return \c true if the softbuffer is available, otherwise \c false.
  bool run_slot(const slot_point& slot)
  {
    std::unique_lock<std::mutex> lock(fsm_mutex);

    // Check if it is released.
    bool is_released = current_state == state::released;

    // Check if it is expired.
    bool is_expired = (current_state == state::reserved) && (reservation_expire_slot <= slot);

    // Check the expiration of the buffer reservation.
    if (is_released || is_expired) {
      // Free codeblocks and transition to available.
      free();
    }

    return current_state == state::available;
  }

  /// Returns true if the buffer matches the given identifier, false otherwise. It does not require state protection.
  bool match_id(const rx_softbuffer_identifier& identifier) const { return reservation_id == identifier; }

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
    std::unique_lock<std::mutex> lock(fsm_mutex);
    srsran_assert(current_state == state::reserved, "It is only possible to reserve in reserved state.");
    current_state = state::locked;
  }

  // See interface for documentation.
  void unlock() override
  {
    std::unique_lock<std::mutex> lock(fsm_mutex);
    if (current_state == state::locked) {
      current_state = state::reserved;
    }
  }

  // See interface for documentation.
  void release() override
  {
    std::unique_lock<std::mutex> lock(fsm_mutex);
    srsran_assert((current_state == state::reserved) || (current_state == state::locked),
                  "It is only possible to reserve in reserved state.");
    current_state = state::released;
  }
};

} // namespace srsran
