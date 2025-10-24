/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "srsran/du/du_high/du_metrics_notifier.h"
#include "srsran/f1ap/gateways/f1c_connection_client.h"
#include "srsran/support/executors/task_executor.h"
#include <map>

namespace srsran {
namespace srs_du {

/// Dummy F1C connection client that emulates the CU-CP side.
class dummy_f1c_test_client : public f1c_connection_client
{
public:
  using message_number        = uint64_t;
  bool cell_start_on_f1_setup = true;

  /// List of (message number, F1AP message) sent by the DU to the CU.
  std::map<message_number, f1ap_message> f1ap_ul_msgs;

  dummy_f1c_test_client(task_executor& test_exec_, bool cell_start_on_f1_setup_ = true);

  std::unique_ptr<f1ap_message_notifier>
  handle_du_connection_request(std::unique_ptr<f1ap_message_notifier> du_rx_pdu_notifier) override;

  /// Number tagged to the next message sent by the DU to the CU.
  message_number next_ul_message_number() const { return next_msg_number; }

  /// Whether the DU signalled to the connection client its disconnection.
  bool du_released_connection() const { return du_released_client; }

  /// \brief Sets the state of the F1 channel (up or down).
  /// If the channel is set down, any on-going connection is lost, and no new connection attempts are accepted.
  void set_f1_channel_state(bool up);

private:
  task_executor& test_exec;
  /// Number to be assigned to the next F1AP message sent by the DU to the CU.
  message_number next_msg_number = 0;
  /// Callable to trigger F1 connection losses.
  unique_function<void()> on_connection_loss;
  /// Current F1 channel state.
  bool f1c_is_up = true;
  /// Whether the DU has released the connection handler (e.g. due to connection loss).
  bool du_released_client = false;
};

/// Dummy DU metrics consumer class that stores the last received report.
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
