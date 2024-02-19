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

#include <memory>

namespace srsran {

struct f1ap_message;

namespace srs_cu_cp {

class cu_cp_f1c_handler;

/// Mock class for the interface between DU and CU-CP.
class mock_du
{
public:
  virtual ~mock_du() = default;

  /// Push new Tx PDU from DU to CU-CP.
  virtual void push_tx_pdu(const f1ap_message& msg) = 0;

  /// \brief Pop Rx PDU sent by CU-CP and received by DU.
  ///
  /// \param[out] msg Rx PDU popped.
  /// \return return true if a PDU was popped, false otherwise.
  virtual bool try_pop_rx_pdu(f1ap_message& msg) = 0;
};

/// Parameters passed to mock DU.
struct mock_du_params {
  /// \brief CU-CP DU repository passed to mock DU.
  ///
  /// The mock DU will use this to forward messages to CU-CP.
  cu_cp_f1c_handler& cu_cp;
};

/// Creates an emulator of a DU from the perspective of the CU-CP.
std::unique_ptr<mock_du> create_mock_du(mock_du_params params);

} // namespace srs_cu_cp
} // namespace srsran