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

#include "srsran/ran/slot_point.h"
#include <cstdint>

namespace srsran {

class unique_tx_buffer;

/// Identifies a transmit buffer.
struct tx_buffer_identifier {
  /// The UE temporal identifier.
  uint16_t rnti;
  /// The HARQ process identifier.
  uint8_t harq_ack_id;

  /// Equal comparison to other identifier.
  bool operator==(const tx_buffer_identifier& other) const
  {
    return (rnti == other.rnti) && (harq_ack_id == other.harq_ack_id);
  }
};

/// \brief Describes a transmitter buffer pool interface.
///
/// The purpose of this interface is to provide the physical layer shared channel encoder with transmitter rate match
/// buffers.
///
/// Transmitter buffers are selected from a pool of resources using a given tx_buffer_identifier and remain
/// persistent until the identifier is reused or expires.
///
/// Each sector is expected to create its unique pool for the purpose of resource management.
///
/// The implementation of this interface must be thread-safe, as both reserve_buffer() and run_slot() may be called
/// from different threads.
class tx_buffer_pool
{
public:
  /// Default destructor.
  virtual ~tx_buffer_pool() = default;

  /// \brief Reserves and retrieves a transmit buffer for a given identifier and number of codeblocks.
  ///
  /// This function reserves a transmit buffer for a given identifier. It ensures that reserved buffers retain their
  /// data values.
  ///
  /// The reserved buffers remain in use until one of the following occurs:
  /// - A buffer reservation is made with the same identifier but a different number of codeblocks.
  /// - The buffer reservation expires (e.g., through the execution of run_slot()).
  ///
  /// The transmit buffer pool neither initializes nor modifies the contents of the reserved transmit buffer. The
  /// modules using the buffer are responsible for initializing and modifying its contents before new transmissions.
  ///
  /// \param[in] slot           Slot context in which the reservation takes place.
  /// \param[in] id             Identifier for the buffer transmission.
  /// \param[in] nof_codeblocks Number of codeblocks to reserve.
  /// \return A valid unique transmit buffer if the reservation was successful; otherwise, an invalid unique transmit
  ///         buffer.
  virtual unique_tx_buffer
  reserve_buffer(const slot_point& slot, const tx_buffer_identifier& id, unsigned nof_codeblocks) = 0;

  /// \brief Reserves and retrieves a transmit buffer without an identifier.
  ///
  /// This reservation method is specifically designed for system information, and the buffer resources will be
  /// automatically released once the buffer is unlocked.
  ///
  /// The transmit buffer pool does not initialize or modify the contents of the transmit buffers. Modules using these
  /// transmit buffers are responsible for initializing and modifying their contents before new transmissions.
  ///
  /// \param[in] slot           Slot context.
  /// \param[in] nof_codeblocks Number of codeblocks to reserve.
  /// \return A valid unique transmit buffer if the reservation was successful; otherwise, an invalid unique buffer.
  virtual unique_tx_buffer reserve_buffer(const slot_point& slot, unsigned nof_codeblocks) = 0;

  /// \brief Runs internal state machines and releases expired buffers.
  /// \param[in] slot Current slot.
  virtual void run_slot(const slot_point& slot) = 0;
};

/// Buffer pool configuration.
struct tx_buffer_pool_config {
  /// \brief Maximum codeblock size.
  ///
  /// Set to \c ldpc::MAX_CODEBLOCK_SIZE by default.
  unsigned max_codeblock_size;
  /// Number of transmit buffers available in the pool.
  unsigned nof_buffers;
  /// Number of codeblocks available in the pool for all the transmit buffers.
  unsigned nof_codeblocks;
  /// Buffer lifetime as a number of slots.
  unsigned expire_timeout_slots;
  /// Set to true to indicate that soft bits are not stored in the buffer.
  bool external_soft_bits;
};

/// Creates a transmit buffer pool.
std::unique_ptr<tx_buffer_pool> create_tx_buffer_pool(const tx_buffer_pool_config& config);

} // namespace srsran
