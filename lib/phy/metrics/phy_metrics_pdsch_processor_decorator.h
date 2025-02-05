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

#include "srsran/phy/metrics/phy_metrics_notifiers.h"
#include "srsran/phy/metrics/phy_metrics_reports.h"
#include "srsran/phy/upper/channel_processors/pdsch/pdsch_processor.h"
#include <memory>

namespace srsran {

/// PDSCH processor metric decorator.
class phy_metrics_pdsch_processor_decorator : public pdsch_processor, private pdsch_processor_notifier
{
public:
  /// Creates a PDSCH processor decorator from a base instance and a metric notifier.
  phy_metrics_pdsch_processor_decorator(std::unique_ptr<pdsch_processor> base_,
                                        pdsch_processor_metric_notifier& notifier_) :
    base(std::move(base_)), notifier(notifier_)
  {
    srsran_assert(base, "Invalid encoder.");
  }

  // See pdsch_processor interface for documentation.
  void process(resource_grid_writer&                                           grid,
               pdsch_processor_notifier&                                       notifier_,
               static_vector<shared_transport_block, MAX_NOF_TRANSPORT_BLOCKS> data,
               const pdu_t&                                                    pdu) override
  {
    // Prepare transmission.
    start_time            = std::chrono::high_resolution_clock::now();
    elapsed_return_ns     = {};
    elapsed_completion_ns = {};
    processor_notifier    = &notifier_;
    tbs                   = units::bytes(data.front().get_buffer().size());

    base->process(grid, *this, data, pdu);

    elapsed_return_ns = std::chrono::nanoseconds(std::chrono::high_resolution_clock::now() - start_time).count();

    report_metrics();
  }

private:
  // See pdsch_processor_notifier interface for documentation.
  void on_finish_processing() override
  {
    srsran_assert(processor_notifier != nullptr, "Invalid PDSCH processor notifier.");
    processor_notifier->on_finish_processing();
    processor_notifier = nullptr;

    elapsed_completion_ns = std::chrono::nanoseconds(std::chrono::high_resolution_clock::now() - start_time).count();

    report_metrics();
  }

  void report_metrics()
  {
    if ((elapsed_return_ns == 0) || (elapsed_completion_ns == 0)) {
      return;
    }

    notifier.new_metric({.tbs                = tbs,
                         .elapsed_return     = std::chrono::nanoseconds(elapsed_return_ns),
                         .elapsed_completion = std::chrono::nanoseconds(elapsed_completion_ns)});
  }

  std::chrono::high_resolution_clock::time_point start_time            = {};
  std::atomic<uint64_t>                          elapsed_return_ns     = {};
  std::atomic<uint64_t>                          elapsed_completion_ns = {};
  units::bytes                                   tbs;
  pdsch_processor_notifier*                      processor_notifier = nullptr;
  std::unique_ptr<pdsch_processor>               base;
  pdsch_processor_metric_notifier&               notifier;
};

} // namespace srsran
