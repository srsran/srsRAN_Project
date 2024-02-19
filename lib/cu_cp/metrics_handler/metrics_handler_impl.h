/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "../du_processor/du_metrics_handler.h"
#include "../ue_manager/ue_metrics_handler.h"
#include "srsran/cu_cp/cu_cp_f1c_handler.h"
#include "srsran/cu_cp/cu_cp_metrics_handler.h"
#include "srsran/support/executors/task_executor.h"
#include "srsran/support/timers.h"

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