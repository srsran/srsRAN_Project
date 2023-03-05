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

class dummy_f1ap_rx_pdu_handler : public f1ap_message_handler
{
public:
  f1ap_message   last_f1ap_msg;
  task_executor* ctrl_exec;

  explicit dummy_f1ap_rx_pdu_handler(task_executor& task_exec) : ctrl_exec(&task_exec) {}

  void handle_message(const f1ap_message& msg) override
  {
    ctrl_exec->execute([this, msg]() { last_f1ap_msg = msg; });
  }
};

class dummy_f1ap_tx_pdu_notifier : public f1ap_message_notifier
{
public:
  dummy_f1ap_tx_pdu_notifier(task_executor& ctrl_exec_) : ctrl_exec(ctrl_exec_) {}
  void on_new_message(const f1ap_message& msg) override
  {
    // Dispatch storing of message to test main thread so it can be safely checked in the test function body.
    ctrl_exec.execute([this, msg]() {
      logger.info("Received F1 UL message with {}", msg.pdu.type().to_string());
      last_f1ap_msg = msg;
    });
  }

  f1ap_message          last_f1ap_msg;
  task_executor&        ctrl_exec;
  srslog::basic_logger& logger = srslog::fetch_basic_logger("TEST");
};

mac_rx_data_indication create_ccch_message(slot_point sl_rx, rnti_t rnti);

class du_high_test_bench
{
public:
  du_high_test_bench();
  ~du_high_test_bench();

  bool add_ue(rnti_t rnti);

  void run_slot();

  du_high_worker_manager     workers;
  dummy_f1ap_rx_pdu_handler  pdu_handler;
  dummy_f1ap_tx_pdu_notifier cu_notifier{workers.ctrl_worker};
  phy_test_dummy             phy;
  mac_pcap_dummy             pcap;
  timer_manager              timers;

  du_high du_obj;

  slot_point next_slot;
};

} // namespace srs_du
} // namespace srsran
