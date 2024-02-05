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

#include "srsran/phy/upper/trx_buffer_identifier.h"
#include "srsran/ran/rnti.h"
#include "srsran/ran/slot_point.h"
#include <cstdint>
#include <limits>

namespace srsran {

class unique_rx_buffer;

/// \brief Describes a receive buffer pool.
///
/// The aim of this interface is to provide the physical layer shared channel decoder with receiver buffers.
///
/// The receiver buffers are selected from a pool of resources for a given trx_buffer_identifier and kept persistent
/// until the buffer is released or expires.
///
/// The pool is designed for being unique in a sector. In other words, every sector must create its own pool.
///
/// The implementation might not be thread safe: reserve(), and run_slot() can not be called simultaneously.
///
/// \remark An assertion is triggered if the slot numerology used for reserve() and run_slot() are different.
class rx_buffer_pool
{
public:
  /// Default destructor.
  virtual ~rx_buffer_pool() = default;

  /// \brief Reserves and gets a buffer for a given identifier and number of codeblocks.
  ///
  /// It reserves a buffer for the given identifier. It guarantees that reserved buffers keep the CRC and data values.
  ///
  /// The buffers are reserved until:
  /// - the buffer is freed using unique_rx_buffer::release(),
  /// - a buffer reservation is performed with the same identifier but different number of codeblocks, or
  /// - buffer reservation expires (by means run_slot()).
  ///
  /// The pool does not initialize or modify the codeblock contents of the buffers. The modules that use the buffers are
  /// responsible for initializing and modifying their contents upon new transmissions.
  ///
  /// It is expected that the pool logs in \c PHY channel the context and the reason of a failed reservation. Possible
  /// reservation failures are:
  /// - Insufficient number of buffers, \ref rx_buffer_pool_config::nof_buffers buffers are reserved with different
  ///   identifiers;
  /// - Insufficient number of codeblocks, \ref rx_buffer_pool_config::max_nof_codeblocks codeblocks are currently
  ///   assigned to buffers;
  /// - A buffer with the same identifier is locked;
  /// - No buffer is found with the same identifier while the reservation is not marked as new data;
  /// - The number of codeblocks for a retransmission is different than the previous reservation; or
  /// - The pool operation has stopped.
  ///
  /// \param[in] slot           Slot context in which the reservation occurs.
  /// \param[in] id             Buffer identifier.
  /// \param[in] nof_codeblocks Indicates the number of codeblocks to reserve.
  /// \param[in] new_data       Set to true if the transmission is for new data.
  /// \return A valid unique buffer if the reservation was successful. Otherwise, an invalid unique buffer.
  virtual unique_rx_buffer
  reserve(const slot_point& slot, trx_buffer_identifier id, unsigned nof_codeblocks, bool new_data) = 0;

  /// \brief Runs pool housekeeping tasks.
  /// \param[in] slot Indicates the current slot.
  virtual void run_slot(const slot_point& slot) = 0;
};

/// buffer pool configuration.
struct rx_buffer_pool_config {
  /// \brief Maximum codeblock size.
  ///
  /// Set to \c ldpc::MAX_CODEBLOCK_SIZE by default.
  unsigned max_codeblock_size;
  /// Number of buffers available in the pool.
  unsigned nof_buffers;
  /// Number of codeblocks available in the pool for all the buffers.
  unsigned nof_codeblocks;
  /// buffer lifetime as a number of slots.
  unsigned expire_timeout_slots;
  /// Set to true to indicate that soft bits are not stored in the buffer.
  bool external_soft_bits;
};

/// Receive buffer pool controller interface.
class rx_buffer_pool_controller
{
public:
  /// \brief Default destructor.
  ///
  /// An assertion is triggered if any buffer is still locked. This prevents that unique buffers lose the reference to
  /// the actual buffers.
  virtual ~rx_buffer_pool_controller() = default;

  /// Gets the actual buffer pool.
  virtual rx_buffer_pool& get_pool() = 0;

  /// \brief Stops the buffer pool.
  ///
  /// It waits for all buffers to be unlocked.
  virtual void stop() = 0;
};

/// Creates a receive buffer pool.
std::unique_ptr<rx_buffer_pool_controller> create_rx_buffer_pool(const rx_buffer_pool_config& config);

} // namespace srsran
