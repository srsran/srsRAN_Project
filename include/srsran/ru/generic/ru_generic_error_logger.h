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

#include "srsran/instrumentation/traces/du_traces.h"
#include "srsran/phy/lower/lower_phy_error_notifier.h"
#include "srsran/phy/support/resource_grid_context.h"

namespace srsran {

/// Implements a generic lower physical layer error logger.
class ru_generic_error_logger : public lower_phy_error_notifier
{
  /// Logger.
  srslog::basic_logger& logger;

public:
  /// Creates an adapter with a given logger.
  explicit ru_generic_error_logger(srslog::basic_logger& logger_) : logger(logger_) {}

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
