/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/phy/lower/lower_phy_error_notifier.h"

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
  /// Creates an adapter with the desired logging level.
  phy_error_adapter(std::string log_level) : logger(srslog::fetch_basic_logger("Low-PHY")) {}

  // See interface for documentation.
  void on_late_resource_grid(const late_resource_grid_context& context) override
  {
    logger.set_context(context.slot.sfn(), context.slot.slot_index());
    logger.info("Real-time failure in low-phy: Downlink data late for sector {}, slot {} and symbol {}.",
                context.sector,
                context.slot,
                context.symbol);
  }

  // See interface for documentation.
  void on_prach_request_late(const prach_buffer_context& context) override
  {
    logger.set_context(context.slot.sfn(), context.slot.slot_index());
    logger.info("Real-time failure in low-phy: PRACH request late for sector {}, slot {} and start symbol {}.",
                context.sector,
                context.slot,
                context.start_symbol);
  }

  // See interface for documentation.
  void on_prach_request_overflow(const prach_buffer_context& context) override
  {
    logger.set_context(context.slot.sfn(), context.slot.slot_index());
    logger.info("Real-time failure in low-phy: PRACH request overflow for sector {}, slot {} and start symbol {}.",
                context.sector,
                context.slot,
                context.start_symbol);
  }
};

} // namespace srsran
