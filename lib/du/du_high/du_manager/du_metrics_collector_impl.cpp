/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "du_metrics_collector_impl.h"
#include "srsran/adt/concurrent_queue.h"
#include "srsran/adt/mpmc_queue.h"
#include "srsran/mac/mac_metrics.h"
#include "srsran/mac/mac_metrics_notifier.h"
#include "srsran/scheduler/scheduler_metrics.h"
#include "srsran/srslog/srslog.h"
#include "srsran/support/executors/execute_until_success.h"
#include "srsran/support/memory_pool/unbounded_object_pool.h"

using namespace srsran;
using namespace srs_du;

// class du_manager_metrics_collector_impl::sched_metrics_aggregator

class du_manager_metrics_collector_impl::sched_metrics_aggregator
{
  static constexpr unsigned capacity = MAX_NOF_DU_CELLS * 4;

public:
  sched_metrics_aggregator(du_manager_metrics_collector_impl& parent_,
                           task_executor&                     du_mng_exec_,
                           timer_manager&                     timers_,
                           srslog::basic_logger&              logger_) :
    parent(parent_),
    du_mng_exec(du_mng_exec_),
    timers(timers_),
    logger(logger_),
    metrics_pool(capacity),
    sched_queue(capacity)
  {
    for (unsigned i = 0; i != capacity; ++i) {
      auto metrics = std::make_unique<scheduler_cell_metrics>();
      metrics->events.reserve(MAX_NOF_DU_UES * 2);
      metrics->ue_metrics.reserve(MAX_NOF_DU_UES);
      metrics_pool.push_blocking(std::move(metrics));
    }
  }

  void add_cell() { nof_active_cells++; }
  void rem_cell() { nof_active_cells--; }

  void handle_scheduler_metrics_report(const scheduler_cell_metrics& report)
  {
    std::unique_ptr<scheduler_cell_metrics> obj;
    if (not metrics_pool.try_pop(obj)) {
      logger.warning("Failed to push scheduler metrics report to queue.");
      return;
    }
    *obj = report;
    if (not sched_queue.try_push(std::move(obj))) {
      logger.warning("Failed to push scheduler metrics report to queue.");
      metrics_pool.push_blocking(std::move(obj));
      return;
    }

    // If the token is acquired, it means that no other thread is running the pending tasks and we need to
    // dispatch a job to run them to the executor.
    bool token_acquired = report_count.fetch_add(1U, std::memory_order_acq_rel) == 0U;
    if (not token_acquired) {
      return;
    }

    // Dispatch the job to aggregate metrics to the executor.
    defer_until_success(du_mng_exec, timers, [this]() { handle_reports(); });
  }

private:
  using metrics_queue = concurrent_queue<std::unique_ptr<scheduler_cell_metrics>,
                                         concurrent_queue_policy::lockfree_mpmc,
                                         concurrent_queue_wait_policy::sleep>;

  void handle_reports()
  {
    std::unique_ptr<scheduler_cell_metrics> cell_metrics;
    uint32_t                                queue_size = this->report_count.load(std::memory_order_acquire);
    while (queue_size > 0) {
      unsigned pop_count = 0;
      for (; pop_count != queue_size and sched_queue.try_pop(cell_metrics); ++pop_count) {
        handle_cell_metrics(*cell_metrics);
        metrics_pool.push_blocking(std::move(cell_metrics));
      }
      if (pop_count == 0) {
        logger.warning("Failure to pop scheduler metrics report from queue.");
      }
      queue_size = this->report_count.fetch_sub(pop_count, std::memory_order_acq_rel) - pop_count;
    }
  }

  void handle_cell_metrics(const scheduler_cell_metrics& metrics)
  {
    srsran_assert(metrics.pci != INVALID_PCI, "Invalid PCI in scheduler metrics report.");

    if (not next_sched_report.cells.empty() and
        std::abs(metrics.slot - next_sched_report.cells[0].slot) >
            parent.params.period.count() * metrics.slot.nof_slots_per_subframe() / 2) {
      // A new report has been detected before expected.
      logger.warning("New scheduler metric report detected before expected ({} != {})",
                     metrics.slot,
                     next_sched_report.cells[0].slot);
      parent.handle_scheduler_metrics_report(next_sched_report);
      next_sched_report.cells.clear();
    }

    auto cell_it = std::find_if(next_sched_report.cells.begin(),
                                next_sched_report.cells.end(),
                                [pci = metrics.pci](const scheduler_cell_metrics& cell) { return cell.pci == pci; });
    if (cell_it != next_sched_report.cells.end()) {
      // Update existing cell metrics.
      logger.warning("Scheduler metric report for duplicate cell with pci={} detected", metrics.pci);
      *cell_it = metrics;
    } else {
      // Add new cell metrics.
      next_sched_report.cells.push_back(metrics);
    }
    std::sort(next_sched_report.cells.begin(), next_sched_report.cells.end(), [](const auto& a, const auto& b) {
      return a.pci < b.pci;
    });

    if (next_sched_report.cells.size() >= nof_active_cells) {
      parent.handle_scheduler_metrics_report(next_sched_report);
      next_sched_report.cells.clear();
    }
  }

  du_manager_metrics_collector_impl& parent;
  task_executor&                     du_mng_exec;
  timer_manager&                     timers;
  srslog::basic_logger&              logger;

  unsigned nof_active_cells = 0;

  scheduler_metrics_report next_sched_report{};

  metrics_queue metrics_pool;
  metrics_queue sched_queue;

  std::atomic<unsigned> report_count{0};
};

// class du_manager_metrics_collector_impl

du_manager_metrics_collector_impl::du_manager_metrics_collector_impl(
    const du_manager_params::metrics_config_params& params_,
    task_executor&                                  du_mng_exec_,
    timer_manager&                                  timers_,
    f1ap_metrics_collector&                         f1ap_collector_,
    mac_metrics_notifier*                           mac_notifier_,
    scheduler_metrics_notifier*                     sched_notifier_) :
  params(params_),
  du_mng_exec(du_mng_exec_),
  timers(timers_),
  mac_notifier(mac_notifier_),
  sched_notifier(sched_notifier_),
  f1ap_collector(f1ap_collector_),
  sched_aggregator(params.du_metrics != nullptr
                       ? std::make_unique<sched_metrics_aggregator>(*this,
                                                                    du_mng_exec_,
                                                                    timers_,
                                                                    srslog::fetch_basic_logger("DU-MNG"))
                       : nullptr)
{
  if (params.du_metrics != nullptr) {
    if (params.f1ap_enabled) {
      next_report.f1ap.emplace();
    }
    if (params.sched_enabled) {
      next_report.scheduler.emplace();
    }
    if (params.mac_enabled) {
      next_report.mac.emplace();
    }
  }
}

du_manager_metrics_collector_impl::~du_manager_metrics_collector_impl() {}

void du_manager_metrics_collector_impl::handle_mac_metrics_report(const mac_metric_report& report)
{
  // Forward the MAC report to notifier.
  if (mac_notifier != nullptr) {
    mac_notifier->on_new_metrics_report(report);
  }

  // In case the DU metrics notifier was specified, report the DU metrics.
  if (params.du_metrics != nullptr) {
    defer_until_success(du_mng_exec, timers, [this, report]() {
      next_report.mac = report;
      trigger_report();
    });
  }
}

void du_manager_metrics_collector_impl::handle_scheduler_metrics_report(const scheduler_cell_metrics& report)
{
  if (sched_notifier != nullptr) {
    sched_notifier->report_metrics(report);
  }

  // Aggregate different cells scheduler metric reports.
  if (params.du_metrics != nullptr) {
    sched_aggregator->handle_scheduler_metrics_report(report);
  }
}

void du_manager_metrics_collector_impl::trigger_report()
{
  if (next_report.scheduler.has_value() and next_report.scheduler->cells.empty()) {
    // Aggregated scheduler metric report not received yet.
    return;
  }
  if (next_report.mac.has_value() and next_report.mac->dl.cells.empty()) {
    // MAC metric report not received yet.
    return;
  }

  next_report.start_time = std::chrono::steady_clock::now() - params.period;
  next_report.period     = params.period;
  next_report.version    = next_version++;

  if (next_report.f1ap.has_value()) {
    // Generate F1AP metrics report.
    f1ap_collector.handle_metrics_report_request(*next_report.f1ap);
  }

  // TODO: Generate RLC report.

  // Forward new report.
  params.du_metrics->on_new_metric_report(next_report);

  // Reset report.
  // Note: We use clear() to keep the vector capacity.
  if (next_report.f1ap.has_value()) {
    next_report.f1ap->ues.clear();
  }
  if (next_report.mac.has_value()) {
    next_report.mac->dl.cells.clear();
  }
  if (next_report.scheduler.has_value()) {
    next_report.scheduler->cells.clear();
  }
}

void du_manager_metrics_collector_impl::handle_scheduler_metrics_report(const scheduler_metrics_report& report)
{
  // The scheduler metrics report for all active cells is now ready.
  next_report.scheduler = report;

  // Trigger the report generation.
  trigger_report();
}

void du_manager_metrics_collector_impl::handle_cell_start(du_cell_index_t cell_index)
{
  if (sched_aggregator != nullptr) {
    sched_aggregator->add_cell();
  }
}

void du_manager_metrics_collector_impl::handle_cell_stop(du_cell_index_t cell_index)
{
  if (sched_aggregator != nullptr) {
    sched_aggregator->rem_cell();
  }
}
