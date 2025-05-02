/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

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
