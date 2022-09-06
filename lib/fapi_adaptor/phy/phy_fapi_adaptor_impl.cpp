/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "phy_fapi_adaptor_impl.h"

using namespace srsgnb;
using namespace fapi_adaptor;

phy_fapi_adaptor_impl::phy_fapi_adaptor_impl(unsigned                    sector_id,
                                             downlink_processor_pool&    dl_processor_pool,
                                             resource_grid_pool&         rg_pool,
                                             uplink_request_processor&   ul_request_processor,
                                             subcarrier_spacing          scs_common,
                                             const fapi::prach_config&   prach_cfg,
                                             const fapi::carrier_config& carrier_cfg) :
  fapi_translator(sector_id, dl_processor_pool, rg_pool, ul_request_processor, scs_common, prach_cfg, carrier_cfg),
  time_translator(fapi_translator)
{
}

upper_phy_timing_notifier& phy_fapi_adaptor_impl::get_timing_notifier()
{
  return time_translator;
}

void phy_fapi_adaptor_impl::set_slot_time_message_notifier(fapi::slot_time_message_notifier& fapi_time_slot_notifier)
{
  time_translator.set_slot_time_message_notifier(fapi_time_slot_notifier);
}

void phy_fapi_adaptor_impl::set_slot_data_message_notifier(fapi::slot_data_message_notifier& fapi_data_notifier)
{
  results_translator.set_slot_data_message_notifier(fapi_data_notifier);
}

fapi::slot_message_gateway& phy_fapi_adaptor_impl::get_slot_message_gateway()
{
  return fapi_translator;
}

upper_phy_rx_results_notifier& phy_fapi_adaptor_impl::get_rx_results_notifier()
{
  return results_translator;
}
