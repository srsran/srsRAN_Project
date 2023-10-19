/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "du_high_worker_manager.h"
#include "tests/test_doubles/f1u/dummy_f1u_du_gateway.h"
#include "tests/test_doubles/mac/dummy_mac_result_notifier.h"
#include "tests/test_doubles/mac/mac_pcap_dummy.h"
#include "tests/test_doubles/mac/mac_test_messages.h"
#include "srsran/du_high/du_high.h"
#include "srsran/du_high/du_high_configuration.h"
#include "srsran/f1ap/common/f1ap_ue_id.h"

namespace srsran {
namespace srs_du {

class dummy_f1c_test_client : public f1c_connection_client
{
public:
  /// Last messages sent to the CU.
  std::vector<f1ap_message> last_f1ap_msgs;

  dummy_f1c_test_client(task_executor& test_exec_);

  std::unique_ptr<f1ap_message_notifier>
  handle_du_connection_request(std::unique_ptr<f1ap_message_notifier> du_rx_pdu_notifier) override;

private:
  task_executor& test_exec;
};

bool is_ue_context_release_complete_valid(const f1ap_message& msg,
                                          gnb_du_ue_f1ap_id_t du_ue_id,
                                          gnb_cu_ue_f1ap_id_t cu_ue_id);

class du_high_test_bench
{
public:
  du_high_test_bench();
  ~du_high_test_bench();

  bool add_ue(rnti_t rnti);

  void run_slot();

  bool run_until(unique_function<bool()> condition, unsigned max_slot_count = 1000);

  du_high_worker_manager  workers;
  dummy_f1c_test_client   cu_notifier;
  srs_du::cu_up_simulator cu_up_sim;
  phy_test_dummy          phy;
  mac_pcap_dummy          pcap;
  timer_manager           timers;

  du_high_configuration    du_high_cfg;
  std::unique_ptr<du_high> du_hi;

  slot_point next_slot;
};

} // namespace srs_du
} // namespace srsran
