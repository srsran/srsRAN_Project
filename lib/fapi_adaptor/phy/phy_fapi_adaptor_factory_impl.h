/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#pragma once

#include "srsran/fapi_adaptor/phy/phy_fapi_adaptor_factory.h"

namespace srsran {

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
  std::unique_ptr<phy_fapi_adaptor> create(phy_fapi_adaptor_factory_config&& config) override;

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
} // namespace srsran
