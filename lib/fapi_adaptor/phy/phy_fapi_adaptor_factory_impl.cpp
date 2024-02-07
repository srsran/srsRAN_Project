/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
