/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_LIB_PHY_UPPER_UPPER_PHY_FACTORY_IMPL_H
#define SRSGNB_LIB_PHY_UPPER_UPPER_PHY_FACTORY_IMPL_H

#include "srsgnb/phy/upper/upper_phy_factory.h"

namespace srsgnb {

class upper_phy_factory_impl : public upper_phy_factory
{

public:
  std::unique_ptr<upper_phy> create(const upper_phy_config& config) override;
};

} // namespace srsgnb

#endif // SRSGNB_LIB_PHY_UPPER_UPPER_PHY_FACTORY_IMPL_H
