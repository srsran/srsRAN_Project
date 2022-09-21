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
#include "srsgnb/phy/upper/channel_processors/channel_processor_factories.h"
#include "srsgnb/phy/upper/channel_processors/pucch_detector.h"

namespace srsgnb {

class pucch_detector_spy : public pucch_detector
{
public:
  std::pair<pucch_uci_message, channel_state_information> detect(const resource_grid_reader&  grid,
                                                                 const format0_configuration& config) override
  {
    return std::pair<pucch_uci_message, channel_state_information>();
  }
  pucch_uci_message detect(const resource_grid_reader&  grid,
                           const channel_estimate&      estimates,
                           const format1_configuration& config) override
  {
    return pucch_uci_message();
  }
};

PHY_SPY_FACTORY_TEMPLATE(pucch_detector);

} // namespace srsgnb
