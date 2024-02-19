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