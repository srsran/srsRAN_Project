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

#include "srsran/pdcp/pdcp_tx_pdu.h"

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
  virtual void handle_sdu(pdcp_tx_pdu sdu) = 0;

  /// \brief Enqueues a notification to discard the given PDCP TX PDU at the DU.
  ///
  /// The queued notifications are either piggy-backed with the next SDU or sent periodically (whatever comes first).
  /// \param pdcp_sn The sequence number of the PDCP PDU that shall be discarded by lower layers.
  virtual void discard_sdu(uint32_t pdcp_sn) = 0;
};

} // namespace srs_cu_up
} // namespace srsran
