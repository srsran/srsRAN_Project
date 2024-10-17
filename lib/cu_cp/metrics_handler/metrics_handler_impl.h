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

#include "../du_processor/du_metrics_handler.h"
#include "../ue_manager/ue_metrics_handler.h"
#include "srsran/cu_cp/cu_cp_f1c_handler.h"
#include "srsran/cu_cp/cu_cp_metrics_handler.h"
#include "srsran/support/executors/task_executor.h"
#include "srsran/support/timers.h"
#include <mutex>

namespace srsran {
namespace srs_cu_cp {

class metrics_handler_impl final : public metrics_handler
{
public:
  metrics_handler_impl(task_executor&                 cu_cp_exec_,
                       timer_manager&                 timers,
                       ue_metrics_handler&            ue_handler_,
                       du_repository_metrics_handler& du_handler_);

  std::unique_ptr<metrics_report_session>
  create_periodic_report_session(const periodic_metric_report_request& request) override;

  metrics_report request_metrics_report() const override;

private:
  /// Context of a periodic metric report session
  struct periodic_session_context {
    unique_timer             timer;
    metrics_report_notifier* report_notifier;
  };

  // Generate new metrics report.
  metrics_report create_report() const;

  unsigned create_periodic_session(const periodic_metric_report_request& request);
  void     request_session_reconfiguration(unsigned session_id, const periodic_metric_report_request& request);
  void     request_session_deletion(unsigned session_id);

  task_executor&                 cu_cp_exec;
  timer_manager&                 timers;
  ue_metrics_handler&            ue_handler;
  du_repository_metrics_handler& du_handler;
  srslog::basic_logger&          logger;

  // Member variables to manage pool of sessions.
  std::mutex                           mutex;
  std::deque<periodic_session_context> sessions;
  std::vector<unsigned>                free_list;
};

} // namespace srs_cu_cp
} // namespace srsran