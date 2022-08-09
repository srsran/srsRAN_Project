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
using namespace fapi;
using namespace fapi_adaptor;

phy_fapi_adaptor_impl::phy_fapi_adaptor_impl(unsigned int              sector_id,
                                             downlink_processor_pool&  dl_processor_pool,
                                             resource_grid_pool&       rg_pool,
                                             subcarrier_spacing        scs_common,
                                             uplink_request_processor& ul_request_processor,
                                             const fapi::prach_config& prach_tlv) :
  fapi_translator(sector_id, dl_processor_pool, rg_pool, ul_request_processor, scs_common, prach_tlv),
  slot_dispatcher(phy_translator, fapi_translator)
{
}

upper_phy_timing_notifier& phy_fapi_adaptor_impl::get_upper_phy_timing_notifier()
{
  return slot_dispatcher;
}

void phy_fapi_adaptor_impl::set_fapi_slot_time_message_notifier(slot_time_message_notifier& fapi_time_slot_notifier)
{
  phy_translator.set_fapi_slot_time_message_notifier(fapi_time_slot_notifier);
}

fapi::slot_message_gateway& phy_fapi_adaptor_impl::get_slot_message_gateway()
{
  return fapi_translator;
}
