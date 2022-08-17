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

#include "../../phy_test_utils.h"
#include "srsgnb/phy/upper/signal_processors/signal_processor_factories.h"

namespace srsgnb {

class dmrs_pusch_estimator_spy : public dmrs_pusch_estimator
{
public:
  void estimate(channel_estimate& estimate, const resource_grid_reader& grid, const configuration& config) override
  {
    // TBD.
  }
};

PHY_SPY_FACTORY_TEMPLATE(dmrs_pusch_estimator);

} // namespace srsgnb