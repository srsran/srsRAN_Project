/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

std::unique_ptr<phy_fapi_adaptor>
phy_fapi_adaptor_factory_impl::create(const phy_fapi_adaptor_factory_config&  config,
                                      phy_fapi_adaptor_factory_dependencies&& dependencies)
{
  phy_fapi_adaptor_impl_config adaptor_config;
  adaptor_config.sector_id                  = config.sector_id;
  adaptor_config.nof_slots_request_headroom = config.nof_slots_request_headroom;
  adaptor_config.scs                        = config.scs;
  adaptor_config.scs_common                 = config.scs_common;
  adaptor_config.prach_cfg                  = config.prach_cfg;
  adaptor_config.carrier_cfg                = config.carrier_cfg;
  adaptor_config.prach_ports                = config.prach_ports;

  phy_fapi_adaptor_impl_dependencies adaptor_dependencies;
  adaptor_dependencies.logger               = dependencies.logger;
  adaptor_dependencies.dl_processor_pool    = dependencies.dl_processor_pool;
  adaptor_dependencies.dl_rg_pool           = dependencies.dl_rg_pool;
  adaptor_dependencies.dl_pdu_validator     = dependencies.dl_pdu_validator;
  adaptor_dependencies.ul_request_processor = dependencies.ul_request_processor;
  adaptor_dependencies.ul_rg_pool           = dependencies.ul_rg_pool;
  adaptor_dependencies.ul_pdu_repository    = dependencies.ul_pdu_repository;
  adaptor_dependencies.ul_pdu_validator     = dependencies.ul_pdu_validator;
  adaptor_dependencies.pm_repo              = std::move(dependencies.pm_repo);
  adaptor_dependencies.part2_repo           = std::move(dependencies.part2_repo);

  return std::make_unique<phy_fapi_adaptor_impl>(adaptor_config, std::move(adaptor_dependencies));
}

std::unique_ptr<phy_fapi_adaptor_factory> srsran::fapi_adaptor::create_phy_fapi_adaptor_factory()
{
  return std::make_unique<phy_fapi_adaptor_factory_impl>();
}
