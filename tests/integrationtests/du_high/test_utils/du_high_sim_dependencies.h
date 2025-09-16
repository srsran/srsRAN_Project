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

#include "srsran/du/du_high/du_metrics_notifier.h"
#include "srsran/f1ap/gateways/f1c_connection_client.h"
#include "srsran/support/executors/task_executor.h"
#include <map>
#include <vector>

namespace srsran {
namespace srs_du {

class dummy_f1c_test_client : public f1c_connection_client
{
public:
  using message_number        = uint64_t;
  bool cell_start_on_f1_setup = true;

  /// Last messages sent to the CU.
  std::vector<f1ap_message> last_f1ap_msgs;

  /// List of (message number, F1AP message) sent by the DU to the CU.
  std::map<message_number, f1ap_message> f1ap_ul_msgs;

  dummy_f1c_test_client(task_executor& test_exec_, bool cell_start_on_f1_setup_ = true);

  std::unique_ptr<f1ap_message_notifier>
  handle_du_connection_request(std::unique_ptr<f1ap_message_notifier> du_rx_pdu_notifier) override;

  /// Number tagged to the next message sent by the DU to the CU.
  message_number next_ul_message_number() const { return next_msg_number; }

private:
  task_executor& test_exec;
  message_number next_msg_number = 0;
};

class dummy_du_metrics_notifier : public du_metrics_notifier
{
public:
  std::optional<du_metrics_report> last_report;

  dummy_du_metrics_notifier(task_executor& exec_) : exec(exec_) {}

  void on_new_metric_report(const du_metrics_report& report) override
  {
    bool result = exec.defer([this, report]() { last_report = report; });
    report_fatal_error_if_not(result, "Failed to defer metric report to test thread");
  }

private:
  task_executor& exec;
};

} // namespace srs_du
} // namespace srsran
