/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ru_radio_metrics_collector.h"

using namespace srsran;

void ru_radio_metrics_collector::on_radio_rt_event(const event_description& description)
{
  // Count events.
  switch (description.type) {
    case event_type::LATE:
      late_count.fetch_add(1, std::memory_order_relaxed);
      break;
    case event_type::UNDERFLOW:
      underflow_count.fetch_add(1, std::memory_order_relaxed);
      break;
    case event_type::OVERFLOW:
      overflow_count.fetch_add(1, std::memory_order_relaxed);
      break;
    case event_type::UNDEFINED:
    case event_type::START_OF_BURST:
    case event_type::END_OF_BURST:
    case event_type::OTHER:
      // Uncounted.
      break;
  }
}

void ru_radio_metrics_collector::collect_metrics(radio_metrics& metrics)
{
  metrics.late_count      = late_count.exchange(0, std::memory_order_relaxed);
  metrics.underflow_count = underflow_count.exchange(0, std::memory_order_relaxed);
  metrics.overflow_count  = overflow_count.exchange(0, std::memory_order_relaxed);
}
