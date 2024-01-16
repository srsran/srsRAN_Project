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

#include "../common/e1ap_cu_up_test_messages.h"
#include "../common/test_helpers.h"
#include "srsran/e1ap/common/e1ap_common.h"
#include "srsran/e1ap/cu_up/e1ap_cu_up.h"
#include "srsran/e1ap/cu_up/e1ap_cu_up_factory.h"
#include "srsran/support/executors/manual_task_worker.h"
#include <gtest/gtest.h>

namespace srsran {
namespace srs_cu_up {

class dummy_e1ap_connection_client : public srs_cu_up::e1ap_connection_client
{
public:
  e1ap_message last_tx_e1ap_pdu;

  std::unique_ptr<e1ap_message_notifier>
  handle_cu_up_connection_request(std::unique_ptr<e1ap_message_notifier> cu_up_rx_pdu_notifier) override;

private:
  std::unique_ptr<e1ap_message_notifier> cu_up_rx_pdu_notifier;
};

/// Fixture class for E1AP
class e1ap_cu_up_test : public ::testing::Test
{
protected:
  e1ap_cu_up_test();
  ~e1ap_cu_up_test() override;

  /// \brief Run E1 Setup Procedure to completion.
  void run_e1_setup_procedure();

  /// \brief Helper method to setup a bearer at the CU-UP
  void setup_bearer(unsigned int cu_cp_ue_e1ap_id);

  /// Dummy E1AP gateway to connect to CU-CP and send E1AP PDUs.
  dummy_e1ap_connection_client e1ap_gw;

  timer_manager timers;

  srslog::basic_logger& e1ap_logger = srslog::fetch_basic_logger("E1AP");
  srslog::basic_logger& test_logger = srslog::fetch_basic_logger("TEST");

  dummy_e1ap_cu_up_notifier       cu_up_notifier;
  manual_task_worker              cu_up_worker{128};
  std::unique_ptr<e1ap_interface> e1ap;
};

} // namespace srs_cu_up
} // namespace srsran
