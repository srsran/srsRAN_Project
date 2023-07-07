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
#include "tests/test_doubles/mac/dummy_mac_result_notifier.h"
#include "tests/test_doubles/mac/mac_pcap_dummy.h"
#include "srsran/du_high/du_high.h"
#include "srsran/du_high/du_high_configuration.h"
#include "srsran/f1ap/common/f1ap_types.h"

namespace srsran {
namespace srs_du {

class dummy_f1ap_tx_pdu_notifier : public f1ap_message_notifier
{
public:
  dummy_f1ap_tx_pdu_notifier(task_executor& test_exec_);

  void connect(f1ap_message_handler& f1ap_du_msg_handler);

  void on_new_message(const f1ap_message& msg) override;

  /// Last messages sent to the CU.
  std::vector<f1ap_message> last_f1ap_msgs;
  task_executor&            test_exec;
  f1ap_message_handler*     f1ap_du = nullptr;
  srslog::basic_logger&     logger  = srslog::fetch_basic_logger("TEST");
};

mac_rx_data_indication create_ccch_message(slot_point sl_rx, rnti_t rnti);

bool is_init_ul_rrc_msg_transfer_valid(const f1ap_message& msg, rnti_t rnti);

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

  bool run_until(unique_function<bool()> condition);

  du_high_worker_manager     workers;
  dummy_f1ap_tx_pdu_notifier cu_notifier;
  phy_test_dummy             phy;
  mac_pcap_dummy             pcap;
  timer_manager              timers;

  du_high_configuration    du_high_cfg;
  std::unique_ptr<du_high> du_hi;

  slot_point next_slot;
};

} // namespace srs_du
} // namespace srsran
