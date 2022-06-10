/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_APPS_EXAMPLES_PHY_LOWER_PHY_EXAMPLE_FACTORY_H
#define SRSGNB_APPS_EXAMPLES_PHY_LOWER_PHY_EXAMPLE_FACTORY_H

#include "srsgnb/phy/lower/lower_phy_configuration.h"
#include "srsgnb/phy/lower/lower_phy_control.h"
#include <memory>

namespace srsgnb {

/// Creates a generic lower PHY.
std::unique_ptr<lower_phy_control> create_lower_phy(lower_phy_configuration& config);

} // namespace srsgnb

#endif // SRSGNB_APPS_EXAMPLES_RADIO_SSB_FACTORIES_H
