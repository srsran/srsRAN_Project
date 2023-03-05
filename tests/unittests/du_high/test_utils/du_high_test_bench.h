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
#include "tests/unittests/f1ap/common/test_helpers.h"

namespace srsran {
namespace srs_du {

class dummy_f1ap_pdu_handler : public f1ap_message_handler
{
public:
  f1ap_message   last_f1ap_msg;
  task_executor* ctrl_exec;

  explicit dummy_f1ap_pdu_handler(task_executor& task_exec) : ctrl_exec(&task_exec) {}

  void handle_message(const f1ap_message& msg) override
  {
    ctrl_exec->execute([this, msg]() { last_f1ap_msg = msg; });
  }
};

class du_high_test_bench
{
public:
  du_high_test_bench();
  ~du_high_test_bench()
  {
    // Stop workers before starting to take down other components.
    workers.stop();
  }

  du_high_worker_manager  workers;
  dummy_f1ap_pdu_handler  pdu_handler;
  dummy_f1ap_pdu_notifier cu_notifier{&pdu_handler};
  phy_test_dummy          phy;
  mac_pcap_dummy          pcap;
  timer_manager           timers;

  du_high du_obj;
};

} // namespace srs_du
} // namespace srsran
