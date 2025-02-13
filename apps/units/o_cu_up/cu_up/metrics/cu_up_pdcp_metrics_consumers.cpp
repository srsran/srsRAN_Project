/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "cu_up_pdcp_metrics_consumers.h"
#include "cu_up_pdcp_metrics.h"
#include "srsran/pdcp/pdcp_metrics.h"

using namespace srsran;

void cu_up_pdcp_metrics_consumer_e2::handle_metric(const app_services::metrics_set& metric)
{
  notifier.report_metrics(static_cast<const cu_up_pdcp_metrics_impl&>(metric).get_metrics());
}

cu_up_pdcp_metrics_consumer_json::cu_up_pdcp_metrics_consumer_json(srslog::basic_logger& logger_,
                                                                   srslog::log_channel&  log_chan_,
                                                                   task_executor&        executor_,
                                                                   unique_timer          timer_,
                                                                   unsigned              report_period_ms_) :
  report_period_ms(report_period_ms_),
  logger(logger_),
  log_chan(log_chan_),
  executor(executor_),
  timer(std::move(timer_))
{
  srsran_assert(report_period_ms > 10, "CU-UP report period is too fast to work with current JSON consumer");
  srsran_assert(timer.is_valid(), "Invalid timer passed to metrics controller");

  // Shift the timer a little.
  timer.set(std::chrono::milliseconds(report_period_ms / 10), [this](timer_id_t tid) { initialize_timer(); });
  timer.run();
}

void cu_up_pdcp_metrics_consumer_json::handle_metric(const app_services::metrics_set& metric)
{
  // Implement aggregation.
}

void cu_up_pdcp_metrics_consumer_json::print_metrics()
{
  if (aggr_metrics.empty()) {
    return;
  }

  // Implement me and remove this line.
  log_chan.enabled();

  // Clear metrics after printing.
  clear_metrics();
}

void cu_up_pdcp_metrics_consumer_json::initialize_timer()
{
  timer.set(std::chrono::milliseconds(report_period_ms), [this](timer_id_t tid) {
    if (!executor.execute([this]() {
          print_metrics();
          timer.run();
        })) {
      logger.warning("Failed to enqueue task to print CU-UP metrics");
    }
  });
  timer.run();
}
