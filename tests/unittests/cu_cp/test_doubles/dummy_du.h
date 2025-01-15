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

#include <memory>

namespace srsran {

struct f1ap_message;

namespace srs_cu_cp {

class cu_cp_f1c_handler;

class du_test_stub
{
public:
  virtual ~du_test_stub() = default;

  virtual void push_tx_pdu(const f1ap_message& msg) = 0;

  virtual bool try_pop_rx_pdu(f1ap_message& msg) = 0;
};

struct du_stub_params {
  cu_cp_f1c_handler& cu_cp_f1c_itf;
};

/// Creates an emulator of a DU from the perspective of the CU-CP.
std::unique_ptr<du_test_stub> create_du_stub(du_stub_params params);

} // namespace srs_cu_cp
} // namespace srsran