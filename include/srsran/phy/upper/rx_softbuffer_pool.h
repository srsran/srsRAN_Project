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

#include "srsran/ran/slot_point.h"
#include <cstdint>

namespace srsran {

class unique_rx_softbuffer;

/// Identifies a receive softbuffer.
struct rx_softbuffer_identifier {
  /// Indicates the UE identifier.
  uint16_t rnti;
  /// Indicates the HARQ process identifier.
  uint8_t harq_ack_id;

  /// Equal comparison to other identifier.
  bool operator==(const rx_softbuffer_identifier& other) const
  {
    return (rnti == other.rnti) && (harq_ack_id == other.harq_ack_id);
  }
};

/// \brief Describes a receive softbuffer pool.
///
/// The aim of this interface is to provide the physical layer shared channel decoder with receiver softbuffers.
///
/// The receiver softbuffers are selected from a pool of resources for a given rx_softbuffer_identifier and kept
/// persistent until the identifier is freed or expires.
///
/// The pool is designed for being unique in a sector. In other words, every sector must create its own pool.
///
/// The implementation must be thread safe: reserve_softbuffer(), free_softbuffer() and run_slot() can potentially be
/// called from different threads.
class rx_softbuffer_pool
{
public:
  /// Default destructor.
  virtual ~rx_softbuffer_pool() = default;

  /// \brief Reserves and gets a softbuffer for a given identifier and number of codeblocks.
  ///
  /// Reserves a softbuffer for the given identifier. It guarantees that reserved softbuffers keep the CRC and data
  /// values.
  ///
  /// The softbuffers are reserved until:
  /// - the softbuffer identifier is freed using free_softbuffer(),
  /// - a softbuffer reservation is performed with the same identifier but different number of codeblocks, or
  /// - softbuffer reservation expires (by means run_slot()).
  ///
  /// The pool does not initialize or modify the contents of the softbuffers. The modules that use the softbuffers are
  /// responsible for initializing and modifying their contents upon new transmissions.
  ///
  /// It is expected that the pool logs in \c PHY channel the reason of a failed reservation.
  ///
  /// \param[in] slot Indicates the slot context in which the reservation occurs.
  /// \param[in] id Identifies the softbuffer.
  /// \param[in] nof_codeblocks Indicates the number of codeblocks to reserve.
  /// \return A valid unique softbuffer if the reservation was successful. Otherwise, an invalid unique softbuffer.
  virtual unique_rx_softbuffer
  reserve_softbuffer(const slot_point& slot, const rx_softbuffer_identifier& id, unsigned nof_codeblocks) = 0;

  /// \brief Runs internal state machines and releases expired softbuffers.
  /// \param[in] slot Indicates the current slot.
  virtual void run_slot(const slot_point& slot) = 0;
};

/// Softbuffer pool configuration.
struct rx_softbuffer_pool_config {
  /// \brief Maximum codeblock size.
  ///
  /// Set to \c ldpc::MAX_CODEBLOCK_SIZE by default.
  unsigned max_codeblock_size;
  /// Number of softbuffers available in the pool.
  unsigned max_softbuffers;
  /// Number of codeblocks available in the pool for all the softbuffers.
  unsigned max_nof_codeblocks;
  /// Softbuffer lifetime as a number of slots.
  unsigned expire_timeout_slots;
  /// Set to true to indicate that soft bits are not stored in the buffer.
  bool external_soft_bits;
};

std::unique_ptr<rx_softbuffer_pool> create_rx_softbuffer_pool(const rx_softbuffer_pool_config& config);

} // namespace srsran
