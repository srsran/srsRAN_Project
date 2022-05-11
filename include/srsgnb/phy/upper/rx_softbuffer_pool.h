/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_PHY_UPPER_RX_SOFTBUFFER_POOL_H
#define SRSGNB_PHY_UPPER_RX_SOFTBUFFER_POOL_H

#include "srsgnb/phy/upper/rx_softbuffer.h"
#include "srsgnb/ran/slot_point.h"
#include <cstdint>

namespace srsgnb {

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
  /// \param[in] slot Indicates the slot context in which the reservation occurs.
  /// \param[in] id Identifies the softbuffer.
  /// \param[in] nof_codeblocks Indicates the number of codeblocks to reserve.
  /// \return A pointer to the reserved softbuffer if the reservation was successful. Otherwise, \c nullptr.
  virtual rx_softbuffer*
  reserve_softbuffer(const slot_point& slot, const rx_softbuffer_identifier& id, unsigned nof_codeblocks) = 0;

  /// \brief Releases the specified softbuffer.
  /// \param[in] id Identifies the softbuffer.
  /// \remark If the indicated identifier is invalid, the call is ignored.
  virtual void free_softbuffer(const rx_softbuffer_identifier& id) = 0;

  /// \brief Runs internal state machines and releases expired softbuffers.
  /// \param[in] slot Indicates the current slot.
  virtual void run_slot(const slot_point& slot) = 0;
};

} // namespace srsgnb

#endif // SRSGNB_PHY_UPPER_RX_SOFTBUFFER_POOL_H
