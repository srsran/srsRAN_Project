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

#include "srsran/adt/circular_array.h"
#include "srsran/phy/upper/rx_buffer_pool.h"
#include "srsran/phy/upper/uplink_slot_pdu_repository.h"
#include "srsran/phy/upper/upper_phy_rx_symbol_handler.h"
#include "srsran/srslog/srslog.h"
#include <vector>

namespace srsran {

class uplink_processor;
class uplink_processor_pool;
class upper_phy_rx_results_notifier;

/// \brief Represents a pool of payload buffers.
///
/// The pool comprises a circular byte array, and this class provides functionality
/// to assign a specified number of bytes from the pool. When the array is exhausted,
/// the payload view is started from the beginning again.
class rx_payload_buffer_pool
{
  /// Maximum number of slots to store.
  static constexpr size_t nof_slots = 40U;
  /// Maximum number of bits that could potentially be allocated in a slot.
  static constexpr units::bits max_buffer_size = units::bits(MAX_RB * 156 * 8);
  /// Minimum block size. It ensures that the payload offsets are selected using multiples of blocks.
  static constexpr unsigned min_block_size = 64;

public:
  /// Returns the next available portion of the pool.
  span<uint8_t> acquire_payload_buffer(units::bytes size)
  {
    // Convert the maximum buffer size from bits to bytes for comparison and allocation.
    static constexpr units::bytes max_buffer_size_bytes = max_buffer_size.truncate_to_bytes();

    srsran_assert(size <= max_buffer_size_bytes, "Buffer size (i.e., {}) exceeds maximum {}.", size, pool.size());

    // Round the number of consumed bytes to the next block.
    size_t count = divide_ceil(size.value(), min_block_size) * min_block_size;

    // Reset the available bytes if the pool is exhausted.
    if (available.size() < count) {
      available = pool;
    }

    // Select the first bytes as the payload for the transmission.
    span<uint8_t> payload = available.first(size.value());

    // Advance available bytes.
    available = available.last(available.size() - count);
    return payload;
  }

private:
  /// Pool.
  std::array<uint8_t, max_buffer_size.truncate_to_bytes().value() * nof_slots> pool;
  /// Span that points to the unused portion of the pool.
  span<uint8_t> available;
};

/// \brief Implementation of the upper PHY handler of receive symbols events.
///
/// In this implementation, handling the newly received symbol event takes the following steps.
///     1. Get an uplink processor from the pool.
///     2. Call the corresponding method from the uplink processor based on the type of the received symbol (PRACH, SRS,
///        PUSCH).
class upper_phy_rx_symbol_handler_impl : public upper_phy_rx_symbol_handler
{
public:
  upper_phy_rx_symbol_handler_impl(uplink_processor_pool&         ul_processor_pool_,
                                   uplink_slot_pdu_repository&    ul_pdu_repository_,
                                   rx_buffer_pool&                buffer_pool_,
                                   upper_phy_rx_results_notifier& rx_results_notifier_,
                                   srslog::basic_logger&          logger_);

  // See interface for documentation.
  void handle_rx_symbol(const upper_phy_rx_symbol_context& context, const resource_grid_reader& grid) override;

  // See interface for documentation.
  void handle_rx_prach_window(const prach_buffer_context& context, const prach_buffer& buffer) override;

  // See interface for documentation.
  void handle_rx_srs_symbol(const upper_phy_rx_symbol_context& context) override;

private:
  /// Process the given PUSCH PDU using the given uplink processor, grid and slot.
  void process_pusch(const uplink_processor::pusch_pdu& pdu,
                     uplink_processor&                  ul_processor,
                     const resource_grid_reader&        grid,
                     slot_point                         slot);

private:
  /// Uplink processor pool.
  uplink_processor_pool& ul_processor_pool;
  /// Uplink slot PDU registry.
  uplink_slot_pdu_repository& ul_pdu_repository;
  /// buffer pool.
  rx_buffer_pool& rm_buffer_pool;
  /// Upper PHY results notifier.
  upper_phy_rx_results_notifier& rx_results_notifier;
  /// Upper PHY logger.
  srslog::basic_logger& logger;
  /// Pool of containers for the payload.
  rx_payload_buffer_pool rx_payload_pool;
};

} // namespace srsran
