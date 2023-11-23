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
#include "tx_buffer_codeblock_pool.h"
#include "srsran/adt/static_vector.h"
#include "srsran/phy/upper/unique_tx_buffer.h"
#include <mutex>

namespace srsran {

/// Transmit buffer reservation status.
enum class tx_buffer_status { successful = 0, already_in_use, insufficient_cb };

/// Converts the buffer reservation status to a string.
constexpr const char* to_string(tx_buffer_status status)
{
  switch (status) {
    default:
    case tx_buffer_status::successful:
      return "successful";
    case tx_buffer_status::already_in_use:
      return "HARQ already in use";
    case tx_buffer_status::insufficient_cb:
      return "insufficient CBs";
  }
}

/// Implements a transmitter buffer interface.
class tx_buffer_impl : public unique_tx_buffer::lockable_buffer
{
private:
  /// Protects the finite state machine from concurrent access.
  mutable std::mutex fsm_mutex;
  /// Finite state machine states.
  enum class state {
    /// \brief The buffer has not been configured with any codeblock.
    ///
    /// It is available for being reserved.
    available = 0,
    ///\brief The buffer is reserved with a number of codeblocks for an RNTI and HARQ process.
    ///
    /// It allows new reservations.
    reserved,
    /// \brief The buffer is reserved and locked in a context. It does not accept new reservations or retransmissions.
    ///
    /// The buffer is active in a scope, the buffer shall transition to \c reserved when unlock() is called.
    locked,
  };
  /// Current buffer state.
  state current_state = state::available;
  /// Reservation identifier.
  tx_buffer_identifier reservation_id = tx_buffer_identifier::unknown();
  /// Indicates the slot the buffer will expire at.
  slot_point reservation_expire_slot;
  /// Reference to the codeblock pool.
  tx_buffer_codeblock_pool& codeblock_pool;
  /// Stores codeblock identifiers.
  static_vector<unsigned, MAX_NOF_SEGMENTS> codeblock_ids;

  /// \brief Releases the reserved buffer to the pool.
  /// \remark An assertion is triggered if the buffer is locked.
  void free()
  {
    srsran_assert(current_state != state::locked, "The buffer is locked.");

    // Free all codeblocks.
    while (!codeblock_ids.empty()) {
      unsigned cb_id = codeblock_ids.back();
      codeblock_ids.pop_back();

      codeblock_pool.free(cb_id);
    }

    // Transition to available.
    current_state = state::available;
  }

public:
  /// \brief Creates a transmit buffer.
  /// \param[in] pool Codeblock buffer pool.
  tx_buffer_impl(tx_buffer_codeblock_pool& pool) : reservation_id(), codeblock_pool(pool)
  {
    // Do nothing.
  }

  /// \brief Reserves a transmit buffer.
  ///
  /// The reservation is unsuccessful if:
  /// - the buffer is already in use in an another scope, or
  /// - the pool is out of resources.
  ///
  /// \param[in] id             Reservation identifier.
  /// \param[in] expire_slot    Slot at which the reservation expires.
  /// \param[in] nof_codeblocks Number of codeblocks to reserve.
  /// \return The reservation status.
  tx_buffer_status reserve(const tx_buffer_identifier& id, const slot_point& expire_slot, unsigned nof_codeblocks)
  {
    std::unique_lock<std::mutex> lock(fsm_mutex);

    // It cannot be reserved if it is locked.
    if (current_state == state::locked) {
      return tx_buffer_status::already_in_use;
    }

    // Update reservation information.
    reservation_id          = id;
    reservation_expire_slot = expire_slot;

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
      unsigned cb_id = codeblock_pool.reserve();

      // Make sure the CB identifier is valid.
      if (cb_id == tx_buffer_codeblock_pool::UNRESERVED_CB_ID) {
        // Free the rest of the buffer.
        free();
        return tx_buffer_status::insufficient_cb;
      }

      // Append the codeblock identifier to the list.
      codeblock_ids.push_back(cb_id);
    }

    // Transition to reserved.
    current_state = state::reserved;
    return tx_buffer_status::successful;
  }

  /// \brief It runs buffers housekeeping as per slot basis.
  ///
  /// It frees the buffer if it is not locked, and the identifier is default or the reservation time expired.
  ///
  /// \param[in] slot Current slot.
  /// \return \c true if the buffer is available, otherwise \c false.
  bool run_slot(const slot_point& slot)
  {
    std::unique_lock<std::mutex> lock(fsm_mutex);

    bool is_reserved = (current_state == state::reserved);

    // Check if the
    bool default_id = match_id({});

    // Check if it is expired.
    bool is_expired = (reservation_expire_slot <= slot);

    // Check the expiration of the buffer reservation.
    if (is_reserved && (default_id || is_expired)) {
      // Free codeblocks and transition to available.
      free();
    }

    return current_state == state::available;
  }

  /// Returns true if the buffer matches the given identifier, false otherwise. It does not require state protection.
  bool match_id(const tx_buffer_identifier& identifier) const { return reservation_id == identifier; }

  // See interface for documentation.
  unsigned get_nof_codeblocks() const override { return codeblock_ids.size(); }

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
  bit_buffer get_codeblock(unsigned codeblock_id, unsigned codeblock_size) override
  {
    srsran_assert(codeblock_id < codeblock_ids.size(),
                  "Codeblock index (i.e., {}) exceeds the number of buffers (i.e., {}).",
                  codeblock_id,
                  codeblock_ids.size());
    unsigned cb_id       = codeblock_ids[codeblock_id];
    unsigned cb_max_size = codeblock_pool.get_data(cb_id).size();
    srsran_assert(
        codeblock_size <= cb_max_size, "Codeblock size {} exceeds maximum size {}.", codeblock_size, cb_max_size);
    return codeblock_pool.get_data(cb_id).first(codeblock_size);
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
};

} // namespace srsran
