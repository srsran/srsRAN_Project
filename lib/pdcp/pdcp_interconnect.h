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

#include "srsran/adt/byte_buffer.h"
#include "srsran/adt/byte_buffer_chain.h"

namespace srsran {

/// Status report interface of the PDCP RX entity
/// The PDCP TX entity uses this class to obtain a status report according to received and missing SDUs
class pdcp_rx_status_provider
{
public:
  virtual ~pdcp_rx_status_provider() = default;

  /// \brief Compiles a PDCP status report
  ///
  /// Ref: TS 38.323, Sec. 5.4.1, Sec. 6.2.3.1 and Sec. 6.3.{9,10}
  ///
  /// \return Control PDU for PDCP status report as a byte_buffer
  virtual byte_buffer compile_status_report() = 0;
};

/// This interface represents the status report entry point of a PDCP TX entity
/// The PDCP RX entity uses this class to forward received status reports to the TX entity
class pdcp_tx_status_handler
{
public:
  virtual ~pdcp_tx_status_handler() = default;

  /// \brief Status report handler for TX entity
  /// \param status The status PDU to be processed by the PDCP TX entity
  virtual void on_status_report(byte_buffer_chain status) = 0;
};

} // namespace srsran
