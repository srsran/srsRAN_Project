/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/cu_cp/cu_cp_metrics_handler.h"
#include "srsran/cu_cp/du_repository.h"
#include "srsran/support/executors/task_executor.h"
#include "srsran/support/timers.h"

namespace srsran {
namespace srs_cu_cp {

class metrics_handler_impl final : public metrics_handler
{
public:
  metrics_handler_impl(task_executor& cu_cp_exec_, timer_manager& timers);

  std::unique_ptr<metrics_report_session>
  create_periodic_report_session(const periodic_metric_report_request& request) override;

  metrics_report handle_metrics_report_request(const metric_report_request& request) override;

private:
  /// Context of a periodic metric report session
  struct periodic_session_context {
    unique_timer             timer;
    metrics_report_notifier* report_notifier;
  };

  metrics_report create_report();

  unsigned create_periodic_session(const periodic_metric_report_request& request);
  void     request_session_reconfiguration(const periodic_metric_report_request& request);
  void     request_session_deletion(unsigned session_id);

  task_executor&        cu_cp_exec;
  timer_manager&        timers;
  srslog::basic_logger& logger;

  std::mutex                           mutex;
  std::deque<periodic_session_context> sessions;
  std::vector<unsigned>                free_list;
};

} // namespace srs_cu_cp
} // namespace srsran