/*
 *
 * Copyright 2013-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "du_high_worker_manager.h"
#include "dummy_test_components.h"
#include "lib/du_high/du_high.h"

namespace srsran {
namespace srs_du {

class dummy_f1ap_tx_pdu_notifier : public f1ap_message_notifier
{
public:
  dummy_f1ap_tx_pdu_notifier(task_executor& test_exec_);

  void connect(f1ap_message_handler& f1ap_du_msg_handler);

  void on_new_message(const f1ap_message& msg) override;

  f1ap_message          last_f1ap_msg;
  task_executor&        test_exec;
  f1ap_message_handler* f1ap_du = nullptr;
  srslog::basic_logger& logger  = srslog::fetch_basic_logger("TEST");
};

mac_rx_data_indication create_ccch_message(slot_point sl_rx, rnti_t rnti);

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

  du_high du_obj;

  slot_point next_slot;
};

} // namespace srs_du
} // namespace srsran
