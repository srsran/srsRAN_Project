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

#include "srsgnb/fapi_adaptor/phy/phy_fapi_adaptor_factory.h"

namespace srsgnb {

class uplink_slot_pdu_repository;

namespace fapi_adaptor {

/// Implementation of the PHY&ndash;FAPI adaptor factory.
class phy_fapi_adaptor_factory_impl : public phy_fapi_adaptor_factory
{
public:
  phy_fapi_adaptor_factory_impl(downlink_processor_pool&      dl_processor_pool_,
                                resource_grid_pool&           rg_pool,
                                uplink_request_processor&     ul_request_processor_,
                                resource_grid_pool&           ul_rg_pool_,
                                uplink_slot_pdu_repository&   ul_pdu_repository_,
                                const downlink_pdu_validator& dl_pdu_validator_,
                                const uplink_pdu_validator&   ul_pdu_validator_) :
    dl_processor_pool(dl_processor_pool_),
    dl_rg_pool(rg_pool),
    ul_request_processor(ul_request_processor_),
    ul_rg_pool(ul_rg_pool_),
    ul_pdu_repository(ul_pdu_repository_),
    dl_pdu_validator(dl_pdu_validator_),
    ul_pdu_validator(ul_pdu_validator_)
  {
  }

  // See interface for documentation.
  std::unique_ptr<phy_fapi_adaptor> create(const phy_fapi_adaptor_factory_config& config) override;

private:
  downlink_processor_pool&      dl_processor_pool;
  resource_grid_pool&           dl_rg_pool;
  uplink_request_processor&     ul_request_processor;
  resource_grid_pool&           ul_rg_pool;
  uplink_slot_pdu_repository&   ul_pdu_repository;
  const downlink_pdu_validator& dl_pdu_validator;
  const uplink_pdu_validator&   ul_pdu_validator;
};

} // namespace fapi_adaptor
} // namespace srsgnb
