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

#include "lower_phy_sector.h"
#include "srsran/phy/lower/lower_phy_factory.h"
#include <memory>

namespace srsran {

struct lower_phy_configuration;

/// Helper class that creates the lower PHY sector using the given configuration.
std::unique_ptr<lower_phy_sector> create_low_phy_sector(unsigned                   max_nof_prach_concurrent_requests,
                                                        lower_phy_configuration&   low_cfg,
                                                        lower_phy_error_notifier&  error_notifier,
                                                        lower_phy_timing_notifier* timing_notifier);

} // namespace srsran
