/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "mac_fapi_adaptor_impl.h"

using namespace srsgnb;
using namespace fapi;
using namespace fapi_adaptor;

mac_fapi_adaptor_impl::mac_fapi_adaptor_impl(slot_message_gateway& msg_gw,
                                             unsigned              sector_id_,
                                             subcarrier_spacing    scs) :
  sector_id(sector_id_), mac_translator(msg_gw), fapi_data_translator(), fapi_time_translator(mac_translator, scs)
{
  // :TODO: remove this when sector id is used (in logging)
  (void)(sector_id);
}

slot_data_message_notifier& mac_fapi_adaptor_impl::get_slot_data_notifier()
{
  return fapi_data_translator;
}

fapi::slot_time_message_notifier& mac_fapi_adaptor_impl::get_slot_time_notifier()
{
  return fapi_time_translator;
}

mac_cell_result_notifier& mac_fapi_adaptor_impl::get_cell_result_notifier()
{
  return mac_translator;
}

void mac_fapi_adaptor_impl::set_cell_slot_handler(mac_cell_slot_handler& mac_slot_handler)
{
  fapi_time_translator.set_cell_slot_handler(mac_slot_handler);
}
