/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/phy/lower/lower_phy_factory.h"
#include "lower_phy_impl.h"

using namespace srsgnb;

std::unique_ptr<lower_phy> lower_phy_factory::create(lower_phy_configuration& config)
{
  return std::make_unique<lower_phy_impl>(config);
}