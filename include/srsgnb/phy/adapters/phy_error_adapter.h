/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/phy/lower/lower_phy_error_notifier.h"

namespace srsgnb {

/// \brief Implements a generic physical layer error adapter.
///
/// Currently, the adapter only prints in the log the error and the context.
class phy_error_adapter : public lower_phy_error_notifier
{
private:
  srslog::basic_logger& logger;

public:
  /// Creates an adapter with the desired logging level.
  phy_error_adapter(std::string log_level) : logger(srslog::fetch_basic_logger("Error notifier")) {}

  // See interface for documentation.
  void on_late_resource_grid(const late_resource_grid_context& context) override
  {
    logger.set_context(context.slot.system_slot());
    logger.info("Unavailable data to transmit for sector {}, slot {} and symbol {}.",
                context.sector,
                context.slot,
                context.symbol);
  }
};

} // namespace srsgnb