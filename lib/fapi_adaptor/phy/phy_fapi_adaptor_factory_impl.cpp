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

#include "phy_fapi_adaptor_factory_impl.h"
#include "phy_fapi_adaptor_impl.h"

using namespace srsran;
using namespace fapi_adaptor;

std::unique_ptr<phy_fapi_adaptor> phy_fapi_adaptor_factory_impl::create(phy_fapi_adaptor_factory_config&& config)
{
  phy_fapi_adaptor_impl_config adaptor_config;
  adaptor_config.sector_id            = config.sector_id;
  adaptor_config.scs                  = config.scs;
  adaptor_config.ul_pdu_repository    = &ul_pdu_repository;
  adaptor_config.ul_rg_pool           = &ul_rg_pool;
  adaptor_config.ul_request_processor = &ul_request_processor;
  adaptor_config.dl_pdu_validator     = &dl_pdu_validator;
  adaptor_config.ul_pdu_validator     = &ul_pdu_validator;
  adaptor_config.scs_common           = config.scs_common;
  adaptor_config.dl_processor_pool    = &dl_processor_pool;
  adaptor_config.dl_rg_pool           = &dl_rg_pool;
  adaptor_config.carrier_cfg          = &config.carrier_cfg;
  adaptor_config.prach_cfg            = &config.prach_cfg;
  adaptor_config.pm_repo              = std::move(config.pm_repo);

  return std::make_unique<phy_fapi_adaptor_impl>(std::move(adaptor_config));
}

std::unique_ptr<phy_fapi_adaptor_factory>
srsran::fapi_adaptor::create_phy_fapi_adaptor_factory(downlink_processor_pool&      dl_processor_pool,
                                                      resource_grid_pool&           rg_pool,
                                                      uplink_request_processor&     ul_request_processor,
                                                      resource_grid_pool&           ul_rg_pool,
                                                      uplink_slot_pdu_repository&   ul_pdu_repository,
                                                      const downlink_pdu_validator& dl_pdu_validator,
                                                      const uplink_pdu_validator&   ul_pdu_validator)
{
  return std::make_unique<phy_fapi_adaptor_factory_impl>(dl_processor_pool,
                                                         rg_pool,
                                                         ul_request_processor,
                                                         ul_rg_pool,
                                                         ul_pdu_repository,
                                                         dl_pdu_validator,
                                                         ul_pdu_validator);
}
