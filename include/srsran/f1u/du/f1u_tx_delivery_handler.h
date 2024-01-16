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

#include <cstdint>

namespace srsran {
namespace srs_du {

/// \brief This interface represents the notification entry point of the transmitting side of a F1-U bearer of the DU
/// through which the lower layer (e.g. RLC) notifies the PDCP Tx entity in the CU-UP of transmit/delivery of PDCP PDUs.
class f1u_tx_delivery_handler
{
public:
  virtual ~f1u_tx_delivery_handler() = default;

  /// \brief Handles a transmit notification from lower layers (i.e. from RLC UM/AM).
  ///
  /// Notification must be performed in ascending order of PDCP sequence numbers. It is the responsibility of the
  /// calling function (i.e. the RLC) to prevent out-of-order notifications.
  ///
  /// This function is quick and shall be called directly from pcell_executor to avoid excessive transitions across
  /// executors.
  ///
  /// Safe execution from: pcell_executor
  ///
  /// \param highest_pdcp_sn The highest transmitted PDCP sequence number
  virtual void handle_transmit_notification(uint32_t highest_pdcp_sn) = 0;

  /// \brief Handles a delivery notification from lower layers (i.e. from RLC AM).
  ///
  /// Notification must be performed in ascending order of PDCP sequence numbers. It is the responsibility of the
  /// calling function (i.e. the RLC) to prevent out-of-order notifications.
  ///
  /// This function is quick and shall be called directly from pcell_executor to avoid excessive transitions across
  /// executors.
  ///
  /// Safe execution from: pcell_executor
  ///
  /// \param highest_pdcp_sn The highest delivered PDCP sequence number
  virtual void handle_delivery_notification(uint32_t highest_pdcp_sn) = 0;
};

} // namespace srs_du
} // namespace srsran
