/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ofh_timing_metrics_collector_impl.h"
#include "srsran/ofh/timing/ofh_timing_metrics.h"

using namespace srsran;
using namespace ofh;

void timing_metrics_collector_impl::collect_metrics(timing_metrics& metrics)
{
  metrics.nof_skipped_symbols       = nof_skipped_symbols.exchange(0, std::memory_order_relaxed);
  metrics.skipped_symbols_max_burst = skipped_symbols_max_burst.exchange(0, std::memory_order_relaxed);
}

void timing_metrics_collector_impl::update_metrics(unsigned num_skipped_symbols)
{
  nof_skipped_symbols.fetch_add(num_skipped_symbols, std::memory_order_relaxed);

  // Update the maximum number of continuous symbols skipped.
  unsigned current_max = skipped_symbols_max_burst.load(std::memory_order_relaxed);
  while (num_skipped_symbols > current_max &&
         !skipped_symbols_max_burst.compare_exchange_weak(current_max, num_skipped_symbols)) {
  }
}
