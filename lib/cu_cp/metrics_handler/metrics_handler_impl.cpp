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

#include "metrics_handler_impl.h"
#include "srsran/support/executors/sync_task_executor.h"
#include "srsran/support/srsran_assert.h"
#include <thread>

using namespace srsran;
using namespace srs_cu_cp;

metrics_handler_impl::metrics_handler_impl(task_executor&                 cu_cp_exec_,
                                           timer_manager&                 timers_,
                                           ue_metrics_handler&            ue_handler_,
                                           du_repository_metrics_handler& du_handler_) :
  cu_cp_exec(cu_cp_exec_),
  timers(timers_),
  ue_handler(ue_handler_),
  du_handler(du_handler_),
  logger(srslog::fetch_basic_logger("CU-CP"))
{
}

std::unique_ptr<metrics_report_session>
metrics_handler_impl::create_periodic_report_session(const periodic_metric_report_request& request)
{
  class periodic_metrics_report_session_impl final : public metrics_report_session
  {
  public:
    periodic_metrics_report_session_impl(metrics_handler_impl& handler_, unsigned session_id_) :
      handler(&handler_), session_id(session_id_)
    {
    }
    ~periodic_metrics_report_session_impl() override { stop(); }

    void reconfigure_request(const periodic_metric_report_request& request) override
    {
      srsran_assert(handler != nullptr, "Using invalid metric report session");
      handler->request_session_reconfiguration(session_id, request);
    }

    /// Close the session, explicitly stopping the reporting of new metrics.
    void stop() override
    {
      if (handler != nullptr) {
        handler->request_session_deletion(session_id);
      }
    }

  private:
    metrics_handler_impl* handler = nullptr;
    unsigned              session_id;
  };

  // Allocate new Session context
  unsigned session_id = create_periodic_session(request);

  // Return handler to the session
  return std::make_unique<periodic_metrics_report_session_impl>(*this, session_id);
}

metrics_report metrics_handler_impl::request_metrics_report() const
{
  metrics_report report;

  force_blocking_execute(
      cu_cp_exec,
      [&]() { report = create_report(); },
      [this]() {
        logger.warning("Postponing metrics report request. Cause: CU-CP task queue is full");
        std::this_thread::sleep_for(std::chrono::milliseconds{100});
      });

  return report;
}

metrics_report metrics_handler_impl::create_report() const
{
  metrics_report report;

  report.ues = ue_handler.handle_ue_metrics_report_request();
  report.dus = du_handler.handle_du_metrics_report_request();

  // TODO: Get metrics of connected CU-CP/AMF nodes.

  return report;
}

unsigned metrics_handler_impl::create_periodic_session(const periodic_metric_report_request& request)
{
  // Allocate new session
  unsigned session_id;
  {
    std::lock_guard<std::mutex> lock(mutex);
    if (free_list.empty()) {
      sessions.emplace_back();
      sessions.back().timer = timers.create_unique_timer(cu_cp_exec);
      session_id            = sessions.size() - 1;
    } else {
      session_id = free_list.back();
      free_list.pop_back();
    }
  }

  // Setup session timer and handler.
  sessions[session_id].report_notifier = request.report_notifier;
  sessions[session_id].timer.set(request.period, [this, session_id](timer_id_t tid) {
    // Generate a report.
    metrics_report report = create_report();

    // Notify report.
    sessions[session_id].report_notifier->notify_metrics_report_request(report);

    // Auto-schedule new timer run, given that we are reporting metrics periodically.
    sessions[session_id].timer.run();
  });

  // Start the first run of the timer.
  sessions[session_id].timer.run();

  return session_id;
}

void metrics_handler_impl::request_session_reconfiguration(unsigned                              session_id,
                                                           const periodic_metric_report_request& request)
{
  std::lock_guard<std::mutex> lock(mutex);
  sessions[session_id].timer.set(request.period);
}

void metrics_handler_impl::request_session_deletion(unsigned session_id)
{
  // Deallocate session.
  std::lock_guard<std::mutex> lock(mutex);
  sessions[session_id].timer.stop();
  free_list.push_back(session_id);
}
