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

#include "srsran/phy/upper/upper_phy_factories.h"

namespace srsran {

/// Parameters used to instantiate the DU-low.
struct du_low_configuration {
  downlink_processor_factory_sw_config dl_proc_cfg;
  std::vector<upper_phy_config>        upper_phy;
};

} // namespace srsran