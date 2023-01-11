/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "fapi_factory.h"
#include "srsgnb/support/error_handling.h"

using namespace srsgnb;
using namespace fapi_adaptor;

std::unique_ptr<phy_fapi_adaptor> srsgnb::build_phy_fapi_adaptor(unsigned                      sector_id,
                                                                 subcarrier_spacing            scs,
                                                                 subcarrier_spacing            scs_common,
                                                                 downlink_processor_pool&      dl_processor_pool,
                                                                 resource_grid_pool&           dl_rg_pool,
                                                                 uplink_request_processor&     ul_request_processor,
                                                                 resource_grid_pool&           ul_rg_pool,
                                                                 uplink_slot_pdu_repository&   ul_pdu_repository,
                                                                 const downlink_pdu_validator& dl_pdu_validator,
                                                                 const uplink_pdu_validator&   ul_pdu_validator,
                                                                 const fapi::prach_config&     prach_cfg,
                                                                 const fapi::carrier_config&   carrier_cfg)
{
  std::unique_ptr<phy_fapi_adaptor_factory> adaptor_factory = create_phy_fapi_adaptor_factory(dl_processor_pool,
                                                                                              dl_rg_pool,
                                                                                              ul_request_processor,
                                                                                              ul_rg_pool,
                                                                                              ul_pdu_repository,
                                                                                              dl_pdu_validator,
                                                                                              ul_pdu_validator);
  report_fatal_error_if_not(adaptor_factory, "Invalid PHY adaptor factory.");

  phy_fapi_adaptor_factory_config phy_fapi_config;
  phy_fapi_config.sector_id   = sector_id;
  phy_fapi_config.scs         = scs;
  phy_fapi_config.scs_common  = scs_common;
  phy_fapi_config.prach_cfg   = prach_cfg;
  phy_fapi_config.carrier_cfg = carrier_cfg;

  return adaptor_factory->create(phy_fapi_config);
}

std::unique_ptr<mac_fapi_adaptor> srsgnb::build_mac_fapi_adaptor(unsigned                          sector_id,
                                                                 subcarrier_spacing                scs,
                                                                 fapi::slot_message_gateway&       gateway,
                                                                 fapi::slot_last_message_notifier& last_msg_notifier)
{
  std::unique_ptr<mac_fapi_adaptor_factory> adaptor_factory = create_mac_fapi_adaptor_factory();
  report_fatal_error_if_not(adaptor_factory, "Invalid MAC adaptor factory.");

  mac_fapi_adaptor_factory_config mac_fapi_config(sector_id, scs, gateway, last_msg_notifier);
  return adaptor_factory->create(mac_fapi_config);
}
