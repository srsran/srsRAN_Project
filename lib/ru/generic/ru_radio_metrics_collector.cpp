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
