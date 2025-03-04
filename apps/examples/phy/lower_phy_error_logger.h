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

#include "srsran/phy/lower/lower_phy_error_notifier.h"
#include "srsran/phy/support/resource_grid_context.h"
#include "srsran/srslog/srslog.h"

namespace srsran {

/// Implements a lower physical layer error adapter to an RU error adapter.
class lower_phy_error_logger : public lower_phy_error_notifier
{
public:
  /// Creates an adapter with a given logger.
  explicit lower_phy_error_logger(srslog::basic_logger& logger_) : logger(logger_) {}

  // See interface for documentation.
  void on_late_resource_grid(const resource_grid_context& context) override
  {
    logger.warning(context.slot.sfn(),
                   context.slot.slot_index(),
                   "Real-time failure in low-phy: Downlink data late for sector {} and slot {}.",
                   context.sector,
                   context.slot);
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
  }

  // See interface for documentation.
  void on_puxch_request_late(const resource_grid_context& context) override
  {
    logger.warning(context.slot.sfn(),
                   context.slot.slot_index(),
                   "Real-time failure in low-phy: PUxCH request late for sector {}, slot {}.",
                   context.sector,
                   context.slot);
  }

private:
  /// Logger.
  srslog::basic_logger& logger;
};

} // namespace srsran
