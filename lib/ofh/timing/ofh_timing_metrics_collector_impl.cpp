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
