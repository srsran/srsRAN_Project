/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "phy_fapi_adaptor_factory_impl.h"
#include "phy_fapi_adaptor_impl.h"

using namespace srsgnb;
using namespace fapi;
using namespace fapi_adaptor;

std::unique_ptr<phy_fapi_adaptor> phy_fapi_adaptor_factory_impl::create(const phy_fapi_adaptor_factory_config& config)
{
  return std::make_unique<phy_fapi_adaptor_impl>(
      config.sector_id, dl_processor_pool, rg_pool, config.scs_common, ul_request_processor, config.prach_cfg);
}

std::unique_ptr<phy_fapi_adaptor_factory>
srsgnb::fapi_adaptor::create_phy_fapi_adaptor_factory(downlink_processor_pool&  dl_processor_pool,
                                                      resource_grid_pool&       rg_pool,
                                                      uplink_request_processor& ul_request_processor)
{
  return std::make_unique<phy_fapi_adaptor_factory_impl>(dl_processor_pool, rg_pool, ul_request_processor);
}
