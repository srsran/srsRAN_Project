/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
  mac_pcap_dummy          mac_pcap;
  pcap_rlc_dummy          rlc_pcap;
  timer_manager           timers;

  du_high_configuration    du_high_cfg;
  std::unique_ptr<du_high> du_hi;

  slot_point next_slot;
};

} // namespace srs_du
} // namespace srsran
