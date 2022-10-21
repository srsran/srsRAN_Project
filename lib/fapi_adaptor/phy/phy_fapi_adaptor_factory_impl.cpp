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
using namespace fapi_adaptor;

std::unique_ptr<phy_fapi_adaptor> phy_fapi_adaptor_factory_impl::create(const phy_fapi_adaptor_factory_config& config)
{
  phy_fapi_adaptor_impl_config adaptor_config;
  adaptor_config.sector_id            = config.sector_id;
  adaptor_config.scs                  = config.scs;
  adaptor_config.ul_pdu_repository    = &ul_pdu_repository;
  adaptor_config.ul_rg_pool           = &ul_rg_pool;
  adaptor_config.ul_request_processor = &ul_request_processor;
  adaptor_config.scs_common           = config.scs_common;
  adaptor_config.dl_processor_pool    = &dl_processor_pool;
  adaptor_config.dl_rg_pool           = &dl_rg_pool;
  adaptor_config.carrier_cfg          = &config.carrier_cfg;
  adaptor_config.prach_cfg            = &config.prach_cfg;

  return std::make_unique<phy_fapi_adaptor_impl>(adaptor_config);
}

std::unique_ptr<phy_fapi_adaptor_factory>
srsgnb::fapi_adaptor::create_phy_fapi_adaptor_factory(downlink_processor_pool&    dl_processor_pool,
                                                      resource_grid_pool&         rg_pool,
                                                      uplink_request_processor&   ul_request_processor,
                                                      resource_grid_pool&         ul_rg_pool,
                                                      uplink_slot_pdu_repository& ul_pdu_repository)
{
  return std::make_unique<phy_fapi_adaptor_factory_impl>(
      dl_processor_pool, rg_pool, ul_request_processor, ul_rg_pool, ul_pdu_repository);
}
