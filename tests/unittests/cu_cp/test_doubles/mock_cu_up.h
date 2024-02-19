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

#include "srsran/adt/concurrent_queue.h"
#include "srsran/cu_cp/cu_cp_e1_handler.h"
#include "srsran/e1ap/common/e1ap_message.h"

namespace srsran {
namespace srs_cu_cp {

/// \brief Mock class for the interface between CU-UP and CU-CP.
class mock_cu_up
{
public:
  virtual ~mock_cu_up() = default;

  /// Request from the CU-UP to the CU-CP to establish a E1 connection.
  virtual bool request_cu_cp_connection() = 0;

  /// Called when the CU-UP pushes a E1 Tx PDU to the CU-CP.
  virtual void push_tx_pdu(const e1ap_message& rx_pdu) = 0;

  /// Pop the last E1AP PDU received by the CU-UP.
  virtual bool try_pop_rx_pdu(e1ap_message& pdu) = 0;
};

/// Create a mock of the CU-UP for CU-CP testing.
std::unique_ptr<mock_cu_up> create_mock_cu_up(cu_cp_e1_handler& cu_cp_e1_handler);

} // namespace srs_cu_cp
} // namespace srsran