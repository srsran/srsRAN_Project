/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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