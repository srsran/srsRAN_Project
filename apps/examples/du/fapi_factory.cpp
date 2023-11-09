/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "fapi_factory.h"
#include "srsran/support/error_handling.h"

using namespace srsran;
using namespace fapi_adaptor;

std::unique_ptr<phy_fapi_adaptor>
srsran::build_phy_fapi_adaptor(unsigned                                                   sector_id,
                               subcarrier_spacing                                         scs,
                               subcarrier_spacing                                         scs_common,
                               downlink_processor_pool&                                   dl_processor_pool,
                               resource_grid_pool&                                        dl_rg_pool,
                               uplink_request_processor&                                  ul_request_processor,
                               resource_grid_pool&                                        ul_rg_pool,
                               uplink_slot_pdu_repository&                                ul_pdu_repository,
                               const downlink_pdu_validator&                              dl_pdu_validator,
                               const uplink_pdu_validator&                                ul_pdu_validator,
                               const fapi::prach_config&                                  prach_cfg,
                               const fapi::carrier_config&                                carrier_cfg,
                               std::unique_ptr<fapi_adaptor::precoding_matrix_repository> pm_repo,
                               task_executor&                                             async_executor,
                               tx_buffer_pool&                                            buffer_pool,
                               std::vector<uint8_t>                                       prach_ports)
{
  std::unique_ptr<phy_fapi_adaptor_factory> adaptor_factory = create_phy_fapi_adaptor_factory();
  report_fatal_error_if_not(adaptor_factory, "Invalid PHY adaptor factory.");

  phy_fapi_adaptor_factory_config phy_fapi_config;
  phy_fapi_config.sector_id   = sector_id;
  phy_fapi_config.scs         = scs;
  phy_fapi_config.scs_common  = scs_common;
  phy_fapi_config.prach_cfg   = prach_cfg;
  phy_fapi_config.carrier_cfg = carrier_cfg;
  phy_fapi_config.prach_ports = std::move(prach_ports);

  phy_fapi_adaptor_factory_dependencies phy_fapi_dependencies;
  phy_fapi_dependencies.dl_processor_pool    = &dl_processor_pool;
  phy_fapi_dependencies.dl_rg_pool           = &dl_rg_pool;
  phy_fapi_dependencies.dl_pdu_validator     = &dl_pdu_validator;
  phy_fapi_dependencies.buffer_pool          = &buffer_pool;
  phy_fapi_dependencies.ul_request_processor = &ul_request_processor;
  phy_fapi_dependencies.ul_rg_pool           = &ul_rg_pool;
  phy_fapi_dependencies.ul_pdu_repository    = &ul_pdu_repository;
  phy_fapi_dependencies.ul_pdu_validator     = &ul_pdu_validator;
  phy_fapi_dependencies.pm_repo              = std::move(pm_repo);
  phy_fapi_dependencies.async_executor       = &async_executor;

  return adaptor_factory->create(phy_fapi_config, std::move(phy_fapi_dependencies));
}

std::unique_ptr<mac_fapi_adaptor>
srsran::build_mac_fapi_adaptor(unsigned                                               sector_id,
                               subcarrier_spacing                                     scs,
                               fapi::slot_message_gateway&                            gateway,
                               fapi::slot_last_message_notifier&                      last_msg_notifier,
                               std::unique_ptr<fapi_adaptor::precoding_matrix_mapper> pm_mapper,
                               unsigned                                               cell_nof_prbs)
{
  std::unique_ptr<mac_fapi_adaptor_factory> adaptor_factory = create_mac_fapi_adaptor_factory();
  report_fatal_error_if_not(adaptor_factory, "Invalid MAC adaptor factory.");

  mac_fapi_adaptor_config       mac_fapi_config(sector_id, cell_nof_prbs, scs);
  mac_fapi_adaptor_dependencies mac_fapi_deps(gateway, last_msg_notifier, std::move(pm_mapper));

  return adaptor_factory->create(mac_fapi_config, std::move(mac_fapi_deps));
}
