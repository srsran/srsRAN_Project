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

#include "srsran/adt/noop_functor.h"
#include "srsran/adt/spsc_queue.h"
#include "srsran/support/zero_copy_notifier.h"

namespace srsran {

/// Notifier using SPSC queue and internal pooling of transferred objects.
template <typename ReportType, typename RecycleFunc = noop_operation>
class spsc_metric_report_channel : public zero_copy_notifier<ReportType>
{
  using committer = typename zero_copy_notifier<ReportType>::committer;

  struct consumer {
    spsc_metric_report_channel* parent;

    void operator()(ReportType* report)
    {
      if (report != nullptr) {
        parent->dispose(*report);
      }
    }
  };

  struct noop_ctor {
    ReportType operator()() const { return ReportType{}; }
  };

public:
  using builder = typename zero_copy_notifier<ReportType>::builder;
  using reader  = std::unique_ptr<ReportType, consumer>;

  template <typename ConstructReport = noop_ctor>
  spsc_metric_report_channel(size_t                 capacity,
                             srslog::basic_logger&  logger_,
                             const ConstructReport& report_ctor   = noop_ctor{},
                             const RecycleFunc&     recycle_func_ = {}) :
    logger(logger_), recycle_func(recycle_func_), free_list(capacity), pending(capacity)
  {
    srsran_assert(capacity >= 2, "Capacity must be greater than 1");

    reports.reserve(capacity);
    for (unsigned i = 0; i != capacity; ++i) {
      reports.emplace_back(report_ctor());
      bool discard = free_list.try_push(i);
      report_fatal_error_if_not(discard, "Failed to fill free list");
    }
  }
  ~spsc_metric_report_channel() override
  {
    while (pop() != nullptr) {
    }
  }
  spsc_metric_report_channel(const spsc_metric_report_channel&) = delete;
  spsc_metric_report_channel(spsc_metric_report_channel&&)      = delete;

  reader pop()
  {
    unsigned idx;
    if (not pending.try_pop(idx)) {
      return reader{nullptr, consumer{nullptr}};
    }
    srsran_sanity_check(idx < reports.size(), "Invalid report being committed");
    return reader{&reports[idx], consumer{this}};
  }

  ReportType* peek()
  {
    unsigned* idx = pending.front();
    if (idx == nullptr) {
      return nullptr;
    }
    return &reports[*idx];
  }

  size_t capacity() const { return reports.size(); }

  size_t size() const { return pending.size(); }

private:
  using queue_type = concurrent_queue<unsigned, concurrent_queue_policy::lockfree_spsc>;

  ReportType& get_next() override
  {
    unsigned idx;
    if (not free_list.try_pop(idx)) {
      logger.warning("Metric report queue is depleted. Discarding next report...");
      return dummy_report;
    }
    return reports[idx];
  }

  void commit(ReportType& report) override
  {
    if (&report == &dummy_report) {
      // Ignored.
      recycle_func(report);
      return;
    }
    unsigned idx = &report - reports.data();
    srsran_sanity_check(idx < reports.size(), "Invalid report being committed");
    if (not pending.try_push(idx)) {
      logger.error("Failed to push metric report. Discarding it...");
      recycle_func(report);
    }
  }

  /// Called when the report has been consumed and can be returned to the free list.
  void dispose(ReportType& report)
  {
    unsigned idx = &report - reports.data();
    srsran_sanity_check(idx < reports.size(), "Invalid report being committed");

    // Clear the report before popping so that it can be recycled.
    recycle_func(report);

    // Make the report reusable.
    if (not free_list.try_push(idx)) {
      logger.error("Failed to recycle metric report");
    }
  }

  srslog::basic_logger& logger;
  RecycleFunc           recycle_func;

  ReportType dummy_report;

  std::vector<ReportType> reports;
  queue_type              free_list;
  queue_type              pending;
};

} // namespace srsran
