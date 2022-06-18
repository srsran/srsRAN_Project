/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_LIB_FAPI_ADAPTOR_PHY_PHY_FAPI_ADAPTOR_FACTORY_IMPL_H
#define SRSGNB_LIB_FAPI_ADAPTOR_PHY_PHY_FAPI_ADAPTOR_FACTORY_IMPL_H

#include "srsgnb/fapi_adaptor/phy/phy_fapi_adaptor_factory.h"

namespace srsgnb {

class downlink_processor_pool;
class resource_grid_pool;

namespace fapi_adaptor {

/// Implementation of the PHY-FAPI adaptor factory.
class phy_fapi_adaptor_factory_impl : public phy_fapi_adaptor_factory
{
public:
  phy_fapi_adaptor_factory_impl(downlink_processor_pool& dl_processor_pool, resource_grid_pool& rg_pool) :
    dl_processor_pool(dl_processor_pool), rg_pool(rg_pool)
  {
  }

  // See interface for documentation.
  std::unique_ptr<phy_fapi_adaptor> create(phy_fapi_adaptor_factory_config config) override;

private:
  downlink_processor_pool& dl_processor_pool;
  resource_grid_pool&      rg_pool;
};

} // namespace fapi_adaptor
} // namespace srsgnb

#endif // SRSGNB_LIB_FAPI_ADAPTOR_PHY_PHY_FAPI_ADAPTOR_FACTORY_IMPL_H
