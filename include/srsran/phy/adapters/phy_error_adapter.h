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

#pragma once

#include "srsran/instrumentation/traces/du_traces.h"
#include "srsran/phy/lower/lower_phy_error_notifier.h"
#include "srsran/phy/support/resource_grid_context.h"

namespace srsran {

/// \brief Implements a generic physical layer error adapter.
///
/// Currently, the adapter only logs the error and the context.
class phy_error_adapter : public lower_phy_error_notifier
{
private:
  /// Adapter logger.
  srslog::basic_logger& logger;

public:
  /// Creates an adapter with a given logger.
  explicit phy_error_adapter(srslog::basic_logger& logger_) : logger(logger_) {}

  // See interface for documentation.
  void on_late_resource_grid(const resource_grid_context& context) override
  {
    logger.warning(context.slot.sfn(),
                   context.slot.slot_index(),
                   "Real-time failure in low-phy: Downlink data late for sector {} and slot {}.",
                   context.sector,
                   context.slot);
    l1_tracer << instant_trace_event{"on_late_resource_grid", instant_trace_event::cpu_scope::global};
  }

  // See interface for documentation.
  void on_prach_request_late(const prach_buffer_context& context) override
  {
    logger.warning(context.slot.sfn(),
                   context.slot.slot_index(),
                   "Real-time failure in low-phy: PRACH request late for sector {}, slot {} and start symbol {}.",
                   context.sector,
                   context.slot,
                   context.start_symbol);
    l1_tracer << instant_trace_event{"on_prach_request_late", instant_trace_event::cpu_scope::global};
  }

  // See interface for documentation.
  void on_prach_request_overflow(const prach_buffer_context& context) override
  {
    logger.warning(context.slot.sfn(),
                   context.slot.slot_index(),
                   "Real-time failure in low-phy: PRACH request overflow for sector {}, slot {} and start symbol {}.",
                   context.sector,
                   context.slot,
                   context.start_symbol);
    l1_tracer << instant_trace_event{"on_prach_request_overflow", instant_trace_event::cpu_scope::global};
  }

  // See interface for documentation.
  void on_puxch_request_late(const resource_grid_context& context) override
  {
    logger.warning(context.slot.sfn(),
                   context.slot.slot_index(),
                   "Real-time failure in low-phy: PUxCH request late for sector {}, slot {}.",
                   context.sector,
                   context.slot);
    l1_tracer << instant_trace_event{"on_puxch_request_late", instant_trace_event::cpu_scope::global};
  }
};

} // namespace srsran
