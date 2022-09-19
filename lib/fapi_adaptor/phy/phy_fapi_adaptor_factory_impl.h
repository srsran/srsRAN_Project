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

class uplink_slot_pdu_repository;

namespace fapi_adaptor {

/// Implementation of the PHY&ndash;FAPI adaptor factory.
class phy_fapi_adaptor_factory_impl : public phy_fapi_adaptor_factory
{
public:
  phy_fapi_adaptor_factory_impl(downlink_processor_pool&    dl_processor_pool,
                                resource_grid_pool&         rg_pool,
                                uplink_request_processor&   ul_request_processor,
                                resource_grid_pool&         ul_rg_pool,
                                uplink_slot_pdu_repository& ul_pdu_repository) :
    dl_processor_pool(dl_processor_pool),
    dl_rg_pool(rg_pool),
    ul_request_processor(ul_request_processor),
    ul_rg_pool(ul_rg_pool),
    ul_pdu_repository(ul_pdu_repository)
  {
  }

  // See interface for documentation.
  std::unique_ptr<phy_fapi_adaptor> create(const phy_fapi_adaptor_factory_config& config) override;

private:
  downlink_processor_pool&    dl_processor_pool;
  resource_grid_pool&         dl_rg_pool;
  uplink_request_processor&   ul_request_processor;
  resource_grid_pool&         ul_rg_pool;
  uplink_slot_pdu_repository& ul_pdu_repository;
};

} // namespace fapi_adaptor
} // namespace srsgnb
