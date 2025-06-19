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
  metrics.nof_skipped_symbols               = nof_skipped_symbols.exchange(0, std::memory_order_relaxed);
  metrics.max_nof_continuous_skipped_symbol = max_nof_continuous_skipped_symbol.exchange(0, std::memory_order_relaxed);
}

void timing_metrics_collector_impl::update_metrics(unsigned num_skipped_symbols)
{
  nof_skipped_symbols.fetch_add(num_skipped_symbols, std::memory_order_relaxed);

  // Update the maximum number of continuous symbols skipped.
  unsigned stored_max = max_nof_continuous_skipped_symbol.load(std::memory_order_relaxed);
  if (num_skipped_symbols > stored_max) {
    max_nof_continuous_skipped_symbol.store(num_skipped_symbols, std::memory_order_relaxed);
  }
}
