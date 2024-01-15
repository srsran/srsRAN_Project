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

#include "rlc_am_pdu.h"

#pragma once

namespace srsran {

/// Provides access to status functions of the RLC RX AM entity.
/// The RLC TX AM entity uses this class to
/// - get status PDUs for transmission to lower layers
/// - get length of status PDUs for buffer state calculation
/// - check whether a status report has been polled by the peer
class rlc_rx_am_status_provider
{
public:
  virtual ~rlc_rx_am_status_provider() = default;

  /// \brief Gets a reference to the latest status report.
  ///
  /// Important Note: The returned status report is only guaranteed to be valid if \c status_report_required returns
  /// \c true. For (unit-)testing not checking this condition, a valid status report can be expected
  /// (a) once after receiving one (or more) PDUs, or (b) once after t-reassembly has expired.
  ///
  /// The returned reference stays valid until the next call of this function or the destruction of the RLC RX entity.
  /// \return The latest status report
  virtual rlc_am_status_pdu& get_status_pdu()         = 0;
  virtual uint32_t           get_status_pdu_length()  = 0;
  virtual bool               status_report_required() = 0;
};

/// This interface represents the status PDU entry point of a RLC TX AM entity.
/// The RLC RX AM entity uses this class to forward received status PDUs to the RLC TX AM entity
class rlc_tx_am_status_handler
{
public:
  virtual ~rlc_tx_am_status_handler() = default;

  /// \brief Status PDU handler for TX entity
  /// \param status The status PDU to be processed by the RLC TX AM entity
  virtual void on_status_pdu(rlc_am_status_pdu status) = 0;
};

/// This interface represents the RLC TX AM entity that the RLC RX AM must notify to inform the RLC TX AM entity that a
/// status report is triggered (either by timeout or by request of the other peer) or its size has changed (e.g. further
/// PDUs have been received)
class rlc_tx_am_status_notifier
{
public:
  virtual ~rlc_tx_am_status_notifier() = default;

  /// \brief Informs the TX entity that a status report is triggered or its size has changed, so that the buffer status
  /// report needs to be updated.
  virtual void on_status_report_changed() = 0;
};

} // namespace srsran
