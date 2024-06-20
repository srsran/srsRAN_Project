/*
 *
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/adt/byte_buffer.h"

namespace srsran {
namespace srs_cu_up {

/// \brief This interface represents the data entry point of the transmitting side of a F1-U bearer of the CU-UP.
/// The upper layer (e.g. PDCP) will push NR-U SDUs (e.g. PDCP PDUs/RLC SDUs) into the F1-U bearer towards the DU.
/// The upper layer (e.g. PDCP) will also inform the lower layer of SDUs (e.g. PDCP PDUs) to be discarded.
class f1u_tx_sdu_handler
{
public:
  virtual ~f1u_tx_sdu_handler() = default;

  /// \brief Immediately transmits a PDCP TX PDU to lower layers towards the DU.
  /// \param sdu The PDCP TX PDU to be transmitted to lower layers.
  /// \param is_retx Determines whether the SDU is a PDCP retransmission or not.
  virtual void handle_sdu(byte_buffer sdu, bool is_retx) = 0;

  /// \brief Enqueues a notification to discard the given PDCP TX PDU at the DU.
  ///
  /// The queued notifications are either piggy-backed with the next SDU or sent periodically (whatever comes first).
  /// \param pdcp_sn The sequence number of the PDCP PDU that shall be discarded by lower layers.
  virtual void discard_sdu(uint32_t pdcp_sn) = 0;
};

} // namespace srs_cu_up
} // namespace srsran
