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

#include "srsgnb/fapi_adaptor/phy/phy_fapi_adaptor_factory.h"

namespace srsgnb {
namespace fapi_adaptor {

/// Implementation of the PHY-FAPI adaptor factory.
class phy_fapi_adaptor_factory_impl : public phy_fapi_adaptor_factory
{
public:
  phy_fapi_adaptor_factory_impl(downlink_processor_pool&  dl_processor_pool,
                                resource_grid_pool&       rg_pool,
                                uplink_request_processor& ul_request_processor) :
    dl_processor_pool(dl_processor_pool), rg_pool(rg_pool), ul_request_processor(ul_request_processor)
  {
  }

  // See interface for documentation.
  std::unique_ptr<phy_fapi_adaptor> create(phy_fapi_adaptor_factory_config config) override;

private:
  downlink_processor_pool&  dl_processor_pool;
  resource_grid_pool&       rg_pool;
  uplink_request_processor& ul_request_processor;
};

} // namespace fapi_adaptor
} // namespace srsgnb
